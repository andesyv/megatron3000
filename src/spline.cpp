#include "spline.h"
#include "shaders/shadermanager.h"

QVector2D piecewiseSpline(const std::vector<QVector2D>& p, double t) {
    if (p.size() < 2)
        return {};
    else if (t < 0.0)
        return p.front();
    else if (1.0 <= t)
        return p.back();

    auto f = (p.size()-1) * t;
    const auto i = static_cast<unsigned int>(f);
    f -= i;

    const auto& a = p[i];
    const auto& b = p[i+1];
    const auto at = QVector2D{0.75f * a.x() + 0.25f * b.x(), a.y()};
    const auto bt = QVector2D{0.25f * a.x() + 0.75f * b.x(), b.y()};

    return bezier(std::vector{a, at, bt, b}, f);
}

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
    glBufferData(GL_ARRAY_BUFFER, (mSplineSegments + 1) * sizeof(QVector3D), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

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
        const auto p = QVector3D{piecewiseSpline(mSplinePoints, t)};
        linePoints.push_back(p);
    }
    linePoints.push_back(QVector3D{piecewiseSpline(mSplinePoints, 1.0)});

    // Send to GPU
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, linePoints.size() * sizeof(QVector3D), linePoints.data());
    glBindVertexArray(0);
}