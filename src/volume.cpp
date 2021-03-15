#include "volume.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

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
    m_volumeData.resize(volumeSize);

    if (stream.readRawData(reinterpret_cast<char*>(m_volumeData.data()),volumeSize*sizeof (unsigned short)) != volumeSize*sizeof (unsigned short))
    {
        return false;
    }

    generateTexture();

    return true;
}

void Volume::bind(GLuint binding) {
    if (!m_texInitiated)
        return;

    m_binding = binding;
    glActiveTexture(GL_TEXTURE0 + binding);
    glBindTexture(GL_TEXTURE_2D, m_texBuffer);
}

void Volume::unbind() {
    if (!m_texInitiated || !m_binding)
        return;
    
    glActiveTexture(GL_TEXTURE0 + *m_binding);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_binding = std::nullopt;
}

void Volume::generateTexture() {
    initializeOpenGLFunctions();

    glGenTextures(1, &m_texBuffer);
    glBindTexture(GL_TEXTURE_2D, m_texBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RED, GL_UNSIGNED_SHORT, m_volumeData.data());
    // Min and mag filter: bilinear scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Wrap mode: just clamp to edge since edge is 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    m_texInitiated = true;
}

Volume::~Volume() {
    if (m_texInitiated)
        glDeleteTextures(1, &m_texBuffer);
}