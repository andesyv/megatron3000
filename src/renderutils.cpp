#include "renderutils.h"
#include "shaders/shadermanager.h"
#include <QPoint>

ScreenSpacedBuffer::ScreenSpacedBuffer() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Pos (x,y)
    const GLfloat vertices[] = {
        -1.f, -1.f,
        1.f, -1.f,
        1.f, 1.f,

        -1.f, -1.f,
        1.f, 1.f,
        -1.f, 1.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void ScreenSpacedBuffer::bind() {
    glBindVertexArray(mVAO);
}

void ScreenSpacedBuffer::unbind() {
    glBindVertexArray(0);
}

void ScreenSpacedBuffer::draw() {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

ScreenSpacedBuffer::~ScreenSpacedBuffer() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}






AxisGlyph::AxisGlyph() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Pos (x,y,z), color (r,g,b)
    const GLfloat vertices[] = {
        0.f, 0.f, 0.f,      1.f, 0.f, 0.f,
        1.f, 0.f, 0.f,      1.f, 0.f, 0.f,
        0.f, 0.f, 0.f,      0.f, 1.f, 0.f,
        0.f, 1.f, 0.f,      0.f, 1.f, 0.f,
        0.f, 0.f, 0.f,      0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,      0.f, 0.f, 1.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Check for shader and create if it doesn't exist
    auto& SM = ShaderManager::get();

#ifndef EMBEDDED_SHADERS
    if (!SM.valid("axis")) {
        auto& shader = SM.shader("axis");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "axis.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "axis.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shader!"};
        }
    }
#endif
}

void AxisGlyph::bind() {
    glBindVertexArray(mVAO);
}

void AxisGlyph::unbind() {
    glBindVertexArray(0);
}

void AxisGlyph::draw() {
    bind();
    glDrawArrays(GL_LINES, 0, 6);
    unbind();
}

AxisGlyph::~AxisGlyph() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}





NodeGlyphs::NodeGlyphs(const std::vector<QVector2D>& nodePos) {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBindVertexArray(0);

    resizeNodeBuffer(nodePos);

    auto& SM = ShaderManager::get();

#ifndef EMBEDDED_SHADERS
    if (!SM.valid("node")) {
        auto& shader = SM.shader("node");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "node.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Geometry, "node.gs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "node.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }

#endif
}

void NodeGlyphs::draw(float aspectRatio, float radius, int selectedNode) {
    glBindVertexArray(mVAO);
    
    auto& shader = ShaderManager::get().shader("node");
    shader.bind();
    const auto scale = aspectScale(aspectRatio) * radius;
    shader.setUniformValue("nodeScale", scale);
    shader.setUniformValue("selected", selectedNode);
    shader.setUniformValue("glowRadius", radius * 20.0f);

    glDrawArrays(GL_POINTS, 0, mNodeCount);
    glBindVertexArray(0);
}

void NodeGlyphs::resizeNodeBuffer(const std::vector<QVector2D>& nodePos) {
    mNodeCount = static_cast<GLsizei>(nodePos.size());
    if (mNodeCount == 0)
        return;
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, nodePos.size() * sizeof(QVector2D), nodePos.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void NodeGlyphs::updateNodeBuffer(const std::vector<QVector2D>& nodePos) {
    if (mNodeCount != static_cast<GLsizei>(nodePos.size()))
        resizeNodeBuffer(nodePos);
    else {
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, nodePos.size() * sizeof(QVector2D), nodePos.data());
        glBindVertexArray(0);
    }
}

NodeGlyphs::~NodeGlyphs() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}





WorldPlaneGlyph::WorldPlaneGlyph() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    const GLfloat posdir[] = {0.f, 0.f, 0.f, 0.f, 0.f, -1.f};

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferStorage(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), &posdir, GL_DYNAMIC_STORAGE_BIT);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    auto& SM = ShaderManager::get();

#ifndef EMBEDDED_SHADERS
    if (!SM.valid("plane")) {
        auto& shader = SM.shader("plane");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "plane.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Geometry, "plane.gs")) {
            throw std::runtime_error{"Failed to compile geometry shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "plane.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }


        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }
#endif
}

void WorldPlaneGlyph::bind() {
    glBindVertexArray(mVAO);
}

void WorldPlaneGlyph::unbind() {
    glBindVertexArray(0);
}

void WorldPlaneGlyph::draw(const QMatrix4x4& MVP, const QVector3D& up, const QVector3D& pos, const QVector3D& dir) {
    glEnable(GL_CULL_FACE);
    bind();

    // Reproject the position onto the direction to center it. (making positions orthogonal to dir = 0)
    const QVector3D centeredPos = QVector3D::dotProduct(pos, dir) * dir;

    // Update buffer
    const GLfloat vals[] = {
        centeredPos.x(), centeredPos.y(), centeredPos.z(),
        dir.x(), dir.y(), dir.z()
    };
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(GLfloat), &vals);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    auto& shader = ShaderManager::get().shader("plane");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif
    shader.bind();
    shader.setUniformValue("MVP", MVP);
    shader.setUniformValue("up", up);
    shader.setUniformValue("alpha", std::clamp(mAlpha, 0.f, 1.f));

    glDrawArrays(GL_POINTS, 0, 1);
    unbind();
}

WorldPlaneGlyph::~WorldPlaneGlyph() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}





LightGlobeGlyph::LightGlobeGlyph(const QVector2D& pos) {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    const GLfloat params[] = { pos[0], pos[1] };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferStorage(GL_ARRAY_BUFFER, 2 * sizeof(GLfloat), params, 0);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    auto& SM = ShaderManager::get();

#ifndef EMBEDDED_SHADERS
    if (!SM.valid("globe")) {
        auto& shader = SM.shader("globe");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "globe.vs"))
            throw std::runtime_error{"Failed to compile vertex shader"};

        if (!shader.addSourceRelative(QOpenGLShader::Geometry, "globe.gs"))
            throw std::runtime_error{"Failed to compile geometry shader"};

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "globe.fs"))
            throw std::runtime_error{"Failed to compile fragment shader"};


        if (!shader.link())
            throw std::runtime_error{"Failed to link shaderprogram"};
    }
#endif
}

void LightGlobeGlyph::bind() {
    glBindVertexArray(mVAO);
}

void LightGlobeGlyph::unbind() {
    glBindVertexArray(0);
}

LightGlobeGlyph::~LightGlobeGlyph() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}




QVector2D screenPointToNormalizedCoordinates(const QPoint& point, int width, int height) {
    const auto x = point.x() / static_cast<double>(width);
    const auto y = point.y() / static_cast<double>(height);
    return {
        static_cast<float>(x * 2.0 - 1.0),
        -static_cast<float>(y * 2.0 - 1.0)
    };
}