#ifndef SPLINE_H
#define SPLINE_H

#include <QOpenGLFunctions_4_5_Core>
#include <vector>
#include <QVector2D>

// de Casteljau interpolation algorithm
template <typename T>
T bezier(std::vector<T> p, double t) {
    for (auto d{p.size() - 1}; 0 < d; --d)
        for (auto i{0}; i < d; ++i)
            p[i] = (1.0 - t) * p[i] + t * p[i+1];

    return p[0];
}

QVector2D piecewiseSpline(const std::vector<QVector2D>& p, double t);

class Spline : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;
    std::vector<QVector2D> mSplinePoints;
    unsigned int mSplineSegments;

public:
    Spline(const std::vector<QVector2D>& points = {}, unsigned int segments = 10);

    QVector2D eval(double t) const;
    QVector2D operator()(double t) const { return eval(t); }

    void setSegments(unsigned int segments);

    void draw();

    ~Spline();

private:
    void discretizeSpline();
    
};

#endif // SPLINE_H