#include "volume.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <algorithm>
#include "mini/ini.h"
#include <QVector4D>
#include <QMatrix4x4>
#include <array>
#include <QVector3D>
#include "math.h"
#include "transferfunctionrenderer.h"
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <cmath>
#include <filesystem>
#include "niftiimage.h"

using namespace Slicing;

std::optional<std::vector<float>> readBytesFromNiftiFile(const NiftiImage &image, const bool &cancel);

QMatrix3x3 Plane::rot(QVector3D up) const
{
    // Look-at rotation:
    const auto right = QVector3D::crossProduct(dir, up).normalized();
    up = QVector3D::crossProduct(right, dir).normalized();
    const float matVals[] = {
        right.x(), up.x(), dir.x(),
        right.y(), up.y(), dir.y(),
        right.z(), up.z(), dir.z()};
    return QMatrix3x3{matVals};
}

QMatrix4x4 Plane::model(const QVector3D &up) const
{
    QMatrix4x4 m{rot(up)};
    m.translate(pos);
    return m;
}

Volume::Volume(QOpenGLContext *context, QOffscreenSurface *surface)
    : mContext{context}, mSurface{surface}
{
}

std::optional<QVector3D> fetchSpacingFromINIFile(const QString &fileName)
{
    QVector3D spacing{1.f, 1.f, 1.f};

    /// I'm using https://github.com/pulzed/mINI here to do the dirty work
    const auto &iniFileName = (fileName.left(fileName.lastIndexOf('.')) + ".ini").toStdString();
    mINI::INIFile file{iniFileName};
    mINI::INIStructure ini;
    if (!file.read(ini))
    {
        qWarning() << "Failed to read accompanying ini file for " << fileName;
        return std::nullopt;
    }

    if (ini.has("DatFile"))
    {
        const auto &dataCatagory = ini.get("DatFile");
        if (dataCatagory.has("oldDat Spacing X"))
            spacing.setX(std::stof(dataCatagory.get("oldDat Spacing X")));
        if (dataCatagory.has("oldDat Spacing Y"))
            spacing.setY(std::stof(dataCatagory.get("oldDat Spacing Y")));
        if (dataCatagory.has("oldDat Spacing Z"))
            spacing.setZ(std::stof(dataCatagory.get("oldDat Spacing Z")));
    }

    // Normalize spacing (to make it easier for shaders)
    const auto largest = std::max({spacing.x(), spacing.y(), spacing.z()});
    spacing /= largest;

    return spacing;
}

QVector3D fetchSpacingFromNiftiImage(const NiftiImage &image)
{
    QVector3D spacing{image.get().dx, image.get().dy, image.get().dz};

    // Normalize spacing (to make it easier for shaders)
    const auto largest = std::max({spacing.x(), spacing.y(), spacing.z()});
    spacing /= largest;

    return spacing;
}

bool Volume::loadData(const QString &fileName, const bool &cancel)
{
    if (cancel)
        return false;

    auto niftiImageOpt = NiftiImage::make(fileName.toStdString());
    const bool isNiftiFile = niftiImageOpt.has_value();

    std::optional<std::vector<float>> volumeData{};
    if (!isNiftiFile)
    {
        volumeData = readBytesFromDataFile(fileName, cancel);
    }
    else
    {
        auto &image{niftiImageOpt->get()};
        m_width = image.nx;
        m_height = image.ny;
        m_depth = image.nz;

#ifndef NDEBUG
        qDebug() << "Reading file";
#endif
        volumeData = readBytesFromNiftiFile(*niftiImageOpt, cancel);

        emit loaded();
    }

    if (cancel || !volumeData)
        return false;

    m_volumeData = std::move(*volumeData);

    // Find relative scale of texture
    const auto largest = static_cast<float>(std::max({m_width, m_height, m_depth}));
    m_scale = {m_width / largest, m_height / largest, m_depth / largest};

#ifndef NDEBUG
    qDebug() << "Allocating and storing in GPU memory";
#endif

    // Generate OpenGL Texture
    generateTexture();

    if (cancel)
        return false;

#ifndef NDEBUG
    qDebug() << "Loading additional settings from INI file";
#endif

    // Load additional settings in ini file:
    if (isNiftiFile)
    {
        m_spacing = fetchSpacingFromNiftiImage(*niftiImageOpt);
    }
    else
    {
        auto spacing{fetchSpacingFromINIFile(fileName)};
        if (!spacing)
            return false;

        m_spacing = *spacing;
    }

    if (cancel)
        return false;

#ifndef NDEBUG
    qDebug() << "Generating transfer function";
#endif
    generateTransferFunction();

    if (cancel)
        return false;

#ifndef NDEBUG
    qDebug() << "Generating slicing plane";
#endif
    generateSlicingGeometryBuffer();

    // If we got this far, we are done with the offscreen
    // context and surface and can safely remove them.
    mContext = nullptr;
    mSurface = nullptr;

#ifndef NDEBUG
    qDebug() << "Done loading volume";
#endif
    // Fire "loaded" events:
    loaded();

    return !cancel;
}

void Volume::bind(GLuint binding, GLuint tfBinding, GLuint geometryBinding)
{
    if (!m_texInitiated)
        return;

    m_binding = binding;
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_3D, m_texBuffer);

    m_tfBinding = tfBinding;
    glActiveTexture(GL_TEXTURE0 + tfBinding);
    glBindTexture(GL_TEXTURE_1D, m_tfBuffer);

    m_geometryBinding = geometryBinding;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_slicingGeometryBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, geometryBinding, m_slicingGeometryBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Volume::unbind()
{
    if (!m_texInitiated || !m_binding)
        return;

    glActiveTexture(GL_TEXTURE0 + *m_binding);
    glBindTexture(GL_TEXTURE_3D, 0);
    m_binding = std::nullopt;

    glActiveTexture(GL_TEXTURE0 + *m_tfBinding);
    glBindTexture(GL_TEXTURE_1D, 0);
    m_tfBinding = std::nullopt;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *m_geometryBinding, 0);
    m_geometryBinding = std::nullopt;
}

QVector3D Volume::voxelScale() const
{
    const QVector3D volumeDimemsion{
        static_cast<float>(m_width),
        static_cast<float>(m_height),
        static_cast<float>(m_depth)};
    return (QVector3D{1.f, 1.f, 1.f} / volumeDimemsion) * volumeSpacing();
}

float Volume::data(unsigned int i, unsigned int j, unsigned int k) const
{
    if (m_width <= i || m_height <= j || m_depth <= k)
        return 0.f;

    return m_volumeData.at(i + j * m_width + k * (m_width + m_height));
}

std::array<unsigned int, 3> Volume::getVoxelIndex(unsigned int i) const
{
    // width -> height -> depth
    const auto k = i / (m_width + m_height);
    const auto j = (i - k * (m_width + m_height)) / m_width;
    return {i - j * m_width + k * (m_height + m_width), j, k};
}

Volume::VoxelBounds Volume::getVoxelBounds(unsigned int i, unsigned int j, unsigned int k) const
{
    if (m_width <= i || m_height <= j || m_depth <= k)
        return {};

    const QVector3D pos{
        i / static_cast<float>(m_width),
        j / static_cast<float>(m_height),
        k / static_cast<float>(m_depth)};

    return {pos, voxelScale()};
}

Volume::VoxelBounds Volume::getVoxelBounds(const std::array<unsigned int, 3> &index) const
{
    return getVoxelBounds(index[0], index[1], index[2]);
}

void Volume::updateTransferFunction(const std::vector<QVector4D> &values)
{
    if (!m_tfInitiated)
        return;

    m_tfValues = values;
    const auto size = static_cast<GLsizei>(values.size());

    glBindTexture(GL_TEXTURE_1D, m_tfBuffer);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16F, size, 0, GL_RGBA, GL_FLOAT, values.data());
    glBindTexture(GL_TEXTURE_1D, 0);

    transferFunctionUpdated();
}

void Volume::generateTexture()
{
    if (mContext && mSurface)
        mContext->makeCurrent(mSurface);
    initializeOpenGLFunctions();

    glGenTextures(1, &m_texBuffer);
    glBindTexture(GL_TEXTURE_3D, m_texBuffer);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, m_width, m_height, m_depth, 0, GL_RED, GL_FLOAT, m_volumeData.data());

    // Min and mag filter: bilinear scaling
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Wrap mode: just clamp to edge since edge is 0
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_3D, 0);
    m_texInitiated = true;

    if (mContext)
        mContext->doneCurrent();
}

std::optional<std::vector<float>> Volume::readBytesFromDataFile(const QString &fileName, const bool &cancel)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "File " << fileName << " is not possible to open.";
        return {};
    }

    if (cancel)
        return {};

#ifndef NDEBUG
    qDebug() << "Reading file.";
#endif
    QDataStream stream{&file};
    stream.setByteOrder(QDataStream::LittleEndian);

    // unsigned short width = 0, height = 0, depth = 0;
    stream >> m_width >> m_height >> m_depth;

    QVector<unsigned short> volumeRaw{};
    qDebug() << "Width:" << m_width;
    qDebug() << "Height:" << m_height;
    qDebug() << "Depth:" << m_depth;

    int volumeSize = int(m_width) * int(m_height) * int(m_depth);
    volumeRaw.resize(volumeSize);

    emit loaded();

    if (cancel)
        return {};

    if (stream.readRawData(reinterpret_cast<char *>(volumeRaw.data()), volumeSize * sizeof(unsigned short)) != volumeSize * sizeof(unsigned short))
        return {};

    if (cancel)
        return {};

#ifndef NDEBUG
    qDebug() << "Converting and normalizing data";
#endif

    std::vector<float> volumeData;
    volumeData.reserve(volumeSize);

    /// TODO: upgrade to double precision
    // Convert data to floating points in range [0,1]
    for (const auto &val : volumeRaw)
    {
        // Original data uses 12 of 16 bits, giving the data range of
        // [0, 2^12]. Therefore divide by 2^12 to give the range [0, 1].
        // (upcast to double in calculation to avoid truncation)
        const auto f = static_cast<float>(static_cast<double>(val) / std::pow(2, 12));
        volumeData.emplace_back(f);
    }

    return volumeData;
}

std::optional<float> truncateBytesToFloat(std::byte *dataPtr, int numberOfBytes)
{
    // We can only store as many bytes as fit in a double. Otherwise we're out of luck
    if (sizeof(double) < numberOfBytes)
        return std::nullopt;

    unsigned long long bytes{0};
    memcpy(&bytes, dataPtr, numberOfBytes); // Convert the memory to a double value, and then divide it by the maximum to get it in the [0,1] range
    return static_cast<float>(static_cast<double>(bytes) / std::pow(2.0, static_cast<double>(8 * numberOfBytes)));
}

// I'm keeping this function away from the header file to avoid having to link the nifti library in other translations units. :)
std::optional<std::vector<float>> readBytesFromNiftiFile(const NiftiImage &image, const bool &cancel)
{
    std::vector<float> volume;
    volume.reserve(image.get().nvox);

    const auto stepSize = image.get().nbyper;
    for (std::size_t i{0}; i < image.get().nvox; ++i)
    {
        auto val = truncateBytesToFloat(reinterpret_cast<std::byte *>(image.get().data) + i * stepSize, stepSize);
        if (cancel || !val.has_value())
            return std::nullopt;
        volume.push_back(*val);
    }
    return volume;
}

void Volume::generateTransferFunction()
{
    if (mContext && mSurface)
        mContext->makeCurrent(mSurface);
    initializeOpenGLFunctions();

    // Generate some initial transfer function values:
    const std::vector<Node> initialNodes{
        {QVector2D{0.f, 0.f}},
        {QVector2D{0.05f, 0.f}},
        {QVector2D{0.95f, 1.f}},
        {QVector2D{1.f, 1.f}}};

    constexpr unsigned int INITIAL_VALUE_COUNT = 10;
    m_tfValues = TransferFunctionRenderer::calculateTransferFunctionValues(initialNodes, INITIAL_VALUE_COUNT);

    glGenTextures(1, &m_tfBuffer);
    glBindTexture(GL_TEXTURE_1D, m_tfBuffer);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16F, INITIAL_VALUE_COUNT, 0, GL_RGBA, GL_FLOAT, m_tfValues.data());

    // Min and mag filter: linear scaling
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_1D, 0);
    m_tfInitiated = true;

    transferFunctionUpdated();
    if (mContext)
        mContext->doneCurrent();
}

void Volume::generateSlicingGeometryBuffer()
{
    if (mContext && mSurface)
        mContext->makeCurrent(mSurface);
    initializeOpenGLFunctions();

    glGenBuffers(1, &m_slicingGeometryBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_slicingGeometryBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);
    updateSlicingGeometryBuffer();
    m_slicingGeometryBufferInitiated = true;

    if (mContext)
        mContext->doneCurrent();
}

void Volume::updateSlicingGeometryBuffer()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_slicingGeometryBuffer);
    // Note: Remember to pad to vec4's as the buffer is read like a struct
    const GLfloat values[]{
        m_slicingGeometry.pos.x(), m_slicingGeometry.pos.y(), m_slicingGeometry.pos.z(), 0.f,
        m_slicingGeometry.dir.x(), m_slicingGeometry.dir.y(), m_slicingGeometry.dir.z(), 0.f};
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 8 * sizeof(GLfloat), values);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    slicingGeometryUpdated();
}

void Volume::updateSlicingGeometryBuffer(const Plane &geometry)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_slicingGeometryBuffer);
    const GLfloat values[]{
        geometry.pos.x(), geometry.pos.y(), geometry.pos.z(), 0.f,
        geometry.dir.x(), geometry.dir.y(), geometry.dir.z(), 0.f};
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 8 * sizeof(GLfloat), &values);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Volume::~Volume()
{
    if (mContext && mSurface)
        mContext->makeCurrent(mSurface);
    initializeOpenGLFunctions();

    if (m_texInitiated)
        glDeleteTextures(1, &m_texBuffer);

    if (m_tfInitiated)
        glDeleteTextures(1, &m_tfBuffer);

    if (m_slicingGeometryBufferInitiated)
        glDeleteTextures(1, &m_slicingGeometryBuffer);

    if (mContext)
        mContext->doneCurrent();
}
