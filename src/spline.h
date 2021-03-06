#ifndef SPLINE_H
#define SPLINE_H

#include <QOpenGLFunctions_4_5_Core>
#include <vector>
#include <QVector2D>
#include <memory>

class Volume;

class Spline : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;
    std::vector<QVector2D> mSplinePoints;
    unsigned int mSplineSegments;

public:
    Spline(const std::vector<QVector2D>& points = {}, unsigned int segments = 10);

    QVector2D eval(double t) const;
    QVector2D operator()(double t) const { return eval(t); }

    void update(std::vector<QVector2D> points);
    void setSegments(unsigned int segments);

    void draw(std::shared_ptr<Volume> volume = {});

    ~Spline();

private:
    void discretizeSpline();
    
};

#endif // SPLINE_H