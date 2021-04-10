#ifndef VOLUME_H
#define VOLUME_H
#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <optional>
#include <vector>
#include <QVector3D>

class Volume : public QObject, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    Volume() = default;

    bool loadData(const QString &fileName);

    void bind(GLuint binding = 0, GLuint tfBinding = 1);
    void unbind();

    bool isValid() const {
        return m_texInitiated && m_texInitiated && m_width && m_height && m_depth;
    }

    QVector3D volumeScale() const { return m_scale; }
    QVector3D volumeSpacing() const { return m_spacing; }

    void updateTransferFunction(const std::vector<QVector4D>& values);

signals:
    void loaded();

private:
    unsigned short m_width{0}, m_height{0}, m_depth{0};

    std::vector<float> m_volumeData;
    GLuint m_texBuffer;
    bool m_texInitiated{false};
    std::optional<GLuint> m_binding{std::nullopt};
    QVector3D m_scale{};
    QVector3D m_spacing{};

    void generateTexture();
    bool loadINI(const QString &fileName);


    GLuint m_tfBuffer;
    bool m_tfInitiated{false};
    std::optional<GLuint> m_tfBinding{std::nullopt};
    void generateTransferFunction();

public:
    /**
     * @brief Helper class that binds the texture on construction and unbinds on destruction.
     */
    class Guard {
    private:
        Volume* m_vol{nullptr};

    public:
        Guard(const Guard&) = delete;
        // Default constructor
        Guard(Volume* v = nullptr, GLuint binding = 0, GLuint tfBinding = 1)
            : m_vol{v} {
            if (v != nullptr)
                v->bind(binding, tfBinding);
        }
        // Ownership transfer constuctor (move constructor)
        Guard(Guard&& rhs)
            : m_vol{rhs.m_vol} {
            rhs.m_vol = nullptr;
        }

        void operator=(const Guard&) = delete;
        Guard& operator=(Guard&& rhs) {
            m_vol = rhs.m_vol;
            rhs.m_vol = nullptr;
            return *this;
        }

        ~Guard() {
            if (m_vol != nullptr)
                m_vol->unbind();
        }
    };

    Guard guard(GLuint binding = 0, GLuint tfBinding = 1) { return Guard{this, binding, tfBinding}; }

    ~Volume();
};

#endif // VOLUME_H
