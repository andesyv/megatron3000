#include "spline.h"
#include "shaders/shadermanager.h"

QVector2D piecewiseSpline(const std::vector<QVector2D>& p, double t) {
    if (p.empty())
        return {};
    else if (t < 0.0)
        return p.front();
    else if (1.0 <= t)
        return p.back();

    const auto i = static_cast<unsigned int>(p.size() * t);
    const auto& a = p[i];
    const auto& b = p[i+1];
    const auto at = QVector2D{0.75f * a.x() + 0.25f * b.x(), a.y()};
    const auto bt = QVector2D{0.25f * a.x() + 0.75f * b.x(), b.y()};

    return bezier(std::vector{a, at, bt, b}, t);
}

Spline::Spline(const std::vector<QVector2D>& points, unsigned int segments)
    : mSplinePoints{points}, mSplineSegments{segments} {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    discretizeSpline();
}

// Good ol' de Casteljau
QVector2D Spline::eval(double t) const {
    std::vector<QVector2D> p{mSplinePoints};

    for (auto d{p.size() - 1}; 0 < d; --d) {
        for (auto i{0}; i < d; ++i) {
            p.at(i) = (1.0 - t) * p.at(i) + t * p.at(i+1);
        }
    }

    return p.at(0);
}

void Spline::setSegments(unsigned int segments) {
    if (segments == mSplineSegments)
        return;
    
    mSplineSegments = segments;
    discretizeSpline();
}

void Spline::draw() {
    glBindVertexArray(mVAO);
    auto& shader = ShaderManager::get().shader("default");
    shader.bind();

    glDrawArrays(GL_LINE_STRIP, 0, mSplineSegments+1);
    glBindVertexArray(0);
}

Spline::~Spline() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

void Spline::discretizeSpline() {
    // Gather line points
    std::vector<QVector3D> linePoints;
    linePoints.reserve(mSplineSegments+1);
    for (unsigned int i{0}; i < mSplineSegments; ++i) {
        const auto t = i / static_cast<double>(mSplineSegments);
        linePoints.push_back(QVector3D{eval(t)});
    }
    linePoints.push_back(QVector3D{eval(1.0)});

    // Resize buffer and send to GPU
    glBindVertexArray(mVAO);
    glBufferData(GL_ARRAY_BUFFER, linePoints.size() * sizeof(QVector3D), linePoints.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}