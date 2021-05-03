#ifndef VOLUME_H
#define VOLUME_H
#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <optional>
#include <vector>
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix3x3>

namespace Slicing {
    // I like to define a plane as a direction and a point in the plane.
    struct Plane {
        QVector3D pos{0.f, 0.f, 0.f};
        QVector3D dir{0.f, 0.f, -1.f};

        QMatrix3x3 rot(QVector3D up) const;
        QMatrix4x4 model(const QVector3D& up) const;
    };
}

class Volume : public QObject, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    Volume() = default;

    bool loadData(const QString &fileName);

    void bind(GLuint binding = 0, GLuint tfBinding = 1, GLuint geometryBinding = 4);
    void unbind();

    bool isValid() const {
        return m_texInitiated && m_texInitiated && m_width && m_height && m_depth;
    }

    QVector3D volumeScale() const { return m_scale; }
    QVector3D volumeSpacing() const { return m_spacing; }
    auto data() const { return m_volumeData; }
    auto transferFunctionValues() const { return m_tfValues; }

    void updateTransferFunction(const std::vector<QVector4D>& values);

    /**
     * @brief Array holding all slicing goemetry that is applied to this volume
     * @note If in the future we want to implement multiple slicing goemetry types,
     * an easy approach is changing this to a list of variants/unions of the different
     * geometry types.
     */
    Slicing::Plane m_slicingGeometry;
    void updateSlicingGeometryBuffer();
    // Optional overload for specifying goemetry sent to buffer
    void updateSlicingGeometryBuffer(const Slicing::Plane& geometry);

signals:
    void loaded();
    void transferFunctionUpdated();

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


    std::vector<QVector4D> m_tfValues;
    GLuint m_tfBuffer;
    bool m_tfInitiated{false};
    std::optional<GLuint> m_tfBinding{std::nullopt};
    void generateTransferFunction();


    std::optional<GLuint> m_geometryBinding{std::nullopt};
    GLuint m_slicingGeometryBuffer;
    bool m_slicingGeometryBufferInitiated{false};
    void generateSlicingGeometryBuffer();

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
        Guard(Volume* v = nullptr, GLuint binding = 0, GLuint tfBinding = 1, GLuint geometryBinding = 4)
            : m_vol{v} {
            if (v != nullptr)
                v->bind(binding, tfBinding, geometryBinding);
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

    Guard guard(GLuint binding = 0, GLuint tfBinding = 1, GLuint geometryBinding = 4) {
        return Guard{this, binding, tfBinding, geometryBinding};
    }

    ~Volume();
};

#endif // VOLUME_H
