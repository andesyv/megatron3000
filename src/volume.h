#ifndef VOLUME_H
#define VOLUME_H
#include <QObject>
#include <QOpenGLFunctions_4_3_Core>
#include <optional>
#include <vector>

class Volume : public QObject, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    Volume();

    bool loadData(const QString &fileName);

    void bind(GLuint binding = 0);
    void unbind();

private:
    unsigned short m_width{0}, m_height{0}, m_depth{0};

    std::vector<float> m_volumeData;
    GLuint m_texBuffer;
    bool m_texInitiated{false};
    std::optional<GLuint> m_binding{std::nullopt};

    void generateTexture();

public:
    ~Volume();
};

#endif // VOLUME_H
