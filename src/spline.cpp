#include "spline.h"
#include "shaders/shadermanager.h"

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