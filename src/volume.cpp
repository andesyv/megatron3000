#include "volume.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include "mini/ini.h"

Volume::Volume()
{

}

bool Volume::loadData(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "File " << fileName << " is not possible to open.";
        return false;
    }

#ifndef NDEBUG
    qDebug() << "Reading file.";
#endif

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    // unsigned short width = 0, height = 0, depth = 0;
    stream >> m_width >> m_height >> m_depth;

    qDebug() << "Width:" << m_width;
    qDebug() << "Height:" << m_height;
    qDebug() << "Depth:" << m_depth;

    int volumeSize = int(m_width)*int(m_height)*int(m_depth);
    QVector<unsigned short> volumeRaw{};
    volumeRaw.resize(volumeSize);

    if (stream.readRawData(reinterpret_cast<char*>(volumeRaw.data()),volumeSize*sizeof (unsigned short)) != volumeSize*sizeof (unsigned short))
    {
        return false;
    }



#ifndef NDEBUG
    qDebug() << "Converting and normalizing data";
#endif

    // Convert data to floating points in range [0,1]
    m_volumeData.reserve(volumeSize);
    for (const auto& val : volumeRaw) {
        // Original data uses 12 of 16 bits, giving the data range of
        // [0, 2^12]. Therefore divide by 2^12 to give the range [0, 1].
        // (upcast to double in calculation to avoid truncation)
        const auto f = static_cast<float>(static_cast<double>(val) / std::pow(2, 12));
        m_volumeData.emplace_back(f);
    }

    // Find relative scale of texture
    const auto largest = static_cast<float>(std::max({m_width, m_height, m_depth}));
    m_scale = {m_width / largest, m_height / largest, m_depth / largest};



#ifndef NDEBUG
    qDebug() << "Allocating and storing in GPU memory";
#endif

    // Generate OpenGL Texture
    generateTexture();



#ifndef NDEBUG
    qDebug() << "Loading additional settings from INI file";
#endif
    // Load additional settings in ini file:
    loadINI(fileName);



#ifndef NDEBUG
    qDebug() << "Done loading volume";
#endif
    // Fire "loaded" events:
    loaded();


    return true;
}

void Volume::bind(GLuint binding) {
    if (!m_texInitiated)
        return;

    m_binding = binding;
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_3D, m_texBuffer);
}

void Volume::unbind() {
    if (!m_texInitiated || !m_binding)
        return;
    
    glActiveTexture(GL_TEXTURE0 + *m_binding);
    glBindTexture(GL_TEXTURE_3D, 0);
    m_binding = std::nullopt;
}

void Volume::generateTexture() {
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
}

bool Volume::loadINI(const QString &fileName) {
    m_spacing = {1.f, 1.f, 1.f};

    /// I'm using https://github.com/pulzed/mINI here to do the dirty work
    const auto& iniFileName = (fileName.left(fileName.lastIndexOf('.')) + ".ini").toStdString();
    mINI::INIFile file{iniFileName};
    mINI::INIStructure ini;
    if (!file.read(ini)) {
        qWarning() << "Failed to read accompanying ini file for " << fileName;
        return false;
    }

    if (ini.has("DatFile")) {
        const auto& dataCatagory = ini.get("DatFile");
        if (dataCatagory.has("oldDat Spacing X"))
            m_spacing.setX(std::stof(dataCatagory.get("oldDat Spacing X")));
        if (dataCatagory.has("oldDat Spacing Y"))
            m_spacing.setY(std::stof(dataCatagory.get("oldDat Spacing Y")));
        if (dataCatagory.has("oldDat Spacing Z"))
            m_spacing.setZ(std::stof(dataCatagory.get("oldDat Spacing Z")));
    }

    // Normalize spacing (to make it easier for shaders)
    const auto largest = std::max({m_spacing.x(), m_spacing.y(), m_spacing.z()});
    m_spacing /= largest;

    return true;
}

Volume::~Volume() {
    if (m_texInitiated)
        glDeleteTextures(1, &m_texBuffer);
}