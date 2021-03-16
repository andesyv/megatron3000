#ifndef VOLUME_H
#define VOLUME_H
#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <optional>
#include <vector>

class Volume : public QObject, protected QOpenGLFunctions_4_5_Core
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
    /**
     * @brief Helper class that binds the texture on construction and unbinds on destruction.
     */
    class Guard {
    private:
        Volume* m_vol{nullptr};

    public:
        Guard(const Guard&) = delete;
        Guard(Volume* v, GLuint binding = 0)
            : m_vol{v}
            { v->bind(binding); }
        ~Guard() { m_vol->unbind(); }
    };

    Guard guard(GLuint binding = 0) { return Guard{this, binding}; }

    ~Volume();
};

#endif // VOLUME_H
