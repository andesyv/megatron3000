#include "renderutils.h"
#include "shaders/shadermanager.h"
#include <QQuaternion>

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

    // Pos (x,y,z)
    const GLfloat vertices[] = {
        -1.f, -1.f, 0.f,
        1.f, -1.f, 0.f,
        1.f, 1.f, 0.f,

        -1.f, -1.f, 0.f,
        1.f, 1.f, 0.f,
        -1.f, 1.f, 0.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    auto& SM = ShaderManager::get();

    if (!SM.valid("plane")) {
        auto& shader = SM.shader("plane");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "plane.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "plane.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }
}

void WorldPlaneGlyph::bind() {
    glBindVertexArray(mVAO);
}

void WorldPlaneGlyph::unbind() {
    glBindVertexArray(0);
}

void WorldPlaneGlyph::draw(const QMatrix4x4& MVP, const QMatrix4x4& model) {
    glEnable(GL_CULL_FACE);
    bind();
    const auto planeMat = MVP * model;
    auto& shader = ShaderManager::get().shader("plane");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif
    shader.bind();
    shader.setUniformValue("MVP", planeMat);
    shader.setUniformValue("alpha", std::clamp(mAlpha, 0.f, 1.f));

    glDrawArrays(GL_TRIANGLES, 0, 6);
    unbind();
}

void WorldPlaneGlyph::draw(const QMatrix4x4& MVP, const QVector3D& pos, const QVector3D& dir) {
    QMatrix4x4 model{MatIdentityValues};
    model.lookAt(pos, pos + dir, (MVP * QVector4D{0.f, 1.f, 0.f, 0.f}).toVector3D());
    // model.rotate(QQuaternion::rotationTo(QVector3D{0.f, 0.f, -1.f}, dir));
    // model.translate(pos);
    draw(MVP, model);
}

void WorldPlaneGlyph::draw(const QMatrix4x4& MVP, const QVector3D& pos, const QQuaternion& rot) {
    auto model = QMatrix4x4{rot.toRotationMatrix()};
    model.translate(pos);
    draw(MVP, model);
}

WorldPlaneGlyph::~WorldPlaneGlyph() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}