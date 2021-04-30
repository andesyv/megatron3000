#include "spline.h"
#include "shaders/shadermanager.h"
#include "volume.h"
#include "math.h"

Spline::Spline(const std::vector<QVector2D>& points, unsigned int segments)
    : mSplinePoints{points}, mSplineSegments{0} {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBindVertexArray(0);

    // Sort spline points
    std::sort(mSplinePoints.begin(), mSplinePoints.end(), [](const auto& a, const auto& b){
        return a.x() < b.x();
    });
    setSegments(segments);

    // Create shader
    auto& SM = ShaderManager::get();

#ifndef EMBEDDED_SHADERS
    if (!SM.valid("spline")) {
        auto& shader = SM.shader("spline");

        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "node.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Geometry, "spline.gs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "spline.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }
#endif
}

QVector2D Spline::eval(double t) const {
    return megamath::piecewiseSpline(mSplinePoints, t);
}

void Spline::update(std::vector<QVector2D> points) {
    // Sort points in horizontal axis
    std::sort(points.begin(), points.end(), [](const auto& a, const auto& b){
        return a.x() < b.x();
    });
    mSplinePoints = points;

    discretizeSpline();
}

void Spline::setSegments(unsigned int segments) {
    if (segments == mSplineSegments)
        return;
    
    mSplineSegments = segments;

    // Resize buffer
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, (mSplineSegments + 3) * sizeof(QVector2D), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    discretizeSpline();
}

void Spline::draw(std::shared_ptr<Volume> volume) {
    auto& shader = ShaderManager::get().shader(volume ? "spline" : "default");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif
    shader.bind();
    
    Volume::Guard volumeGuard;
    if (volume)
        volumeGuard = volume->guard();

    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINE_STRIP, 0, mSplineSegments+3);
    glBindVertexArray(0);
}

Spline::~Spline() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

void Spline::discretizeSpline() {
    // Gather line points
    std::vector<QVector2D> linePoints;
    linePoints.reserve(mSplineSegments+3);

    // Draw start point (from screen edge to first node):
    linePoints.emplace_back(-1.f, mSplinePoints.front().y());
    
    // Draw points between1
    for (unsigned int i{0}; i < mSplineSegments; ++i) {
        const auto t = i / static_cast<double>(mSplineSegments);
        const auto p = megamath::piecewiseSpline(mSplinePoints, t);
        linePoints.push_back(p);
    }

    // last point on spline:
    linePoints.push_back(megamath::piecewiseSpline(mSplinePoints, 1.0));

    // Draw end point (from last node to screen edge):
    linePoints.emplace_back(1.f, mSplinePoints.back().y());

    // Send to GPU
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, linePoints.size() * sizeof(QVector2D), linePoints.data());
    glBindVertexArray(0);
}