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

    unsigned short width = 0, height = 0, depth = 0;
    stream >> width >> height >> depth;

    qDebug() << "Width:" << width;
    qDebug() << "Height:" << height;
    qDebug() << "Depth:" << depth;

    int volumeSize = int(width)*int(height)*int(depth);
    m_volumeData.resize(volumeSize);

    if (stream.readRawData(reinterpret_cast<char*>(m_volumeData.data()),volumeSize*sizeof (unsigned short)) != volumeSize*sizeof (unsigned short))
    {
        return false;
    }


    return true;
}
