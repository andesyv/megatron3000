#include "math.h"
#include <array>
#include <tuple>

// using namespace megamath;

namespace megamath {
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
    const auto& at = QVector2D{b.x() - a.x(), 0.f};
    const auto& bt = at;

    return hermite({a, b, at, bt}, f);
}

float partialCubicSolve(float (&& coefficients)[4], float initialGuess) {
    constexpr unsigned int MAX_ITERATIONS = 64;
    constexpr float EPSILON = 0.001f;
    const auto [a, b, c, d] = coefficients;
    float x = initialGuess;
    const auto f = [=](float x){ return coefficients[0] * x * x * x + coefficients[1] * x * x + coefficients[2] * x + coefficients[3]; };
    const auto fd = [=](float x){ return 3 * coefficients[0] * x * x + 2 * coefficients[1] * x + coefficients[2]; };

    for (unsigned int n{0}; n < MAX_ITERATIONS; ++n) {
        const float y = f(x);
        const float d = fd(x);
        if (d < EPSILON)
            return x;
        const float xn = x - y / d;
        if (y < EPSILON)
            return xn;
        
        x = xn;
    }

    return x;
}

float signedDistToPlane(const QVector3D& ppos, const QVector3D& pdir, const QVector3D& point) {
    return QVector3D::dotProduct(point - ppos, pdir);
}

bool boxPlaneIntersect(const QVector3D& bpos, const QVector3D& bsize, const QVector3D& ppos, const QVector3D& pdir) {
    const auto [bx, by, bz] = std::make_tuple(bsize.x() * 0.5f, bsize.y() * 0.5f, bsize.z() * 0.5f);
    const std::array<QVector3D, 8> points{
        bpos + QVector3D{bx, by, bz},
        bpos + QVector3D{-bx, by, bz},
        bpos + QVector3D{bx, -by, bz},
        bpos + QVector3D{bx, by, -bz},
        bpos + QVector3D{-bx, -by, bz},
        bpos + QVector3D{-bx, by, -bz},
        bpos + QVector3D{bx, -by, -bz},
        bpos + QVector3D{-bx, -by, -bz}
    };

    const auto dists = mapList(points, [=](const auto& p){ return signedDistToPlane(ppos, pdir, p); });
    const auto [min, max] = std::minmax_element(dists.begin(), dists.end());

    // If the maximum and minimum distances have different signs, the plane intersects the box.
    return sign(*min) != sign(*max);
}

}