#include "volume.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <vector>
#include <cmath>

Volume::Volume()
{

}

bool Volume::loadData(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "File " << " is not possible to open.";
        return false;
    }

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

    // Convert data to floating points in range [0,1]
    m_volumeData.reserve(volumeSize);
    for (const auto& val : volumeRaw) {
        // Original data uses 12 of 16 bits, giving the data range of
        // [0, 2^12]. Therefore divide by 2^12 to give the range [0, 1].
        // (upcast to double in calculation to avoid truncation)
        const auto f = static_cast<float>(static_cast<double>(val) / std::pow(2, 12));
        m_volumeData.emplace_back(f);
    }

    generateTexture();

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
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, m_width, m_height, m_depth, 0, GL_RED, GL_FLOAT, m_volumeData.data());

    // Min and mag filter: bilinear scaling
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Wrap mode: just clamp to edge since edge is 0
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_3D, 0);
    m_texInitiated = true;
}

Volume::~Volume() {
    if (m_texInitiated)
        glDeleteTextures(1, &m_texBuffer);
}