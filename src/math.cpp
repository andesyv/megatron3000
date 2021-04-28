#include "math.h"

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
}