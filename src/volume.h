#ifndef VOLUME_H
#define VOLUME_H
#include <QObject>

class Volume : public QObject
{
    Q_OBJECT
public:
    Volume();

    bool loadData(const QString &fileName);

private:
    QVector<unsigned short> m_volumeData;
};

#endif // VOLUME_H
