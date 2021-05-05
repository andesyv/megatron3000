#ifndef MEGAMATH_H
#define MEGAMATH_H

// General math functions for the project

#include <vector>
#include <array>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

/// It's like megatron3000-math, but shorted down
namespace megamath {

/**
 * @brief Implementation of javascript / pythons map
 * Converts a list into another list via a transformation function.
 * @param list Input list
 * @param func Function object that will be applied on each element of list
 * @return std::vector<U> where U is return type of func
 * 
 * Example usage:
 * std::vector<int> intList = {1, 2, 3, 1, 2};
 * floatList = mapList(intList, [](auto v){ return static_cast<float>(v); });
 */
template <typename T, typename F>
auto mapList(const std::vector<T>& list, F&& func) {
    using FRetType = decltype(func(typename std::vector<T>::value_type{})); // This ugly line only determines the return type of func
    std::vector<FRetType> newList{};
    newList.reserve(list.size());
    std::transform(list.begin(), list.end(), std::back_inserter(newList), func);
    return newList;
}

// template <typename T, std::size_t N, typename F>
// auto mapList(T (&& list)[N], F&& func) {
//     using FRetType = decltype(func(T{})); // This ugly line only determines the return type of func
//     std::array<FRetType, N> newArr{};
//     std::transform(list.begin(), list.end(), newArr.begin(), func);
//     return newList;
// }

template <typename T, std::size_t N, typename F>
auto mapList(const std::array<T, N>& list, F&& func) {
    using FRetType = decltype(func(T{})); // This ugly line only determines the return type of func
    std::array<FRetType, N> newArr{};
    std::transform(list.begin(), list.end(), newArr.begin(), func);
    return newArr;
}

// https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T>
int sign(T val) {
    return (T{0} < val) - (val < T{0});
}

// de Casteljau interpolation algorithm
template <typename T>
T bezier(std::vector<T> p, double t) {
    for (auto d{p.size() - 1}; 0 < d; --d)
        for (auto i{0}; i < d; ++i)
            p[i] = (1.0 - t) * p[i] + t * p[i+1];

    return p[0];
}

template <typename T, std::size_t N>
// Note to self: rvalue reference to array ensures parameter input is temporary, which must be assigned using list-initialization.
// In practice this makes dangling references impossible (as far as I know).
T bezier(T (&& p)[N], double t) {
    for (auto d{N - 1}; 0 < d; --d)
        for (auto i{0}; i < d; ++i)
            p[i] = (1.0 - t) * p[i] + t * p[i+1];

    return p[0];
}

template <std::size_t K>
auto toQVector(float (&&vals)[K]) {
    if constexpr (K == 1)
        return float{vals[0]};
    else if constexpr (K == 2)
        return QVector2D{ vals[0], vals[1] };
    else if constexpr (K == 3)
        return QVector3D{ vals[0], vals[1], vals[2] };
    else
        return QVector4D{ vals[0], vals[1], vals[2], vals[3] };
}

template <typename T>
auto fromQVector(T&& vec) = delete;
template <> inline auto fromQVector<float>(float&& vec) { return std::array<float, 1>{ vec }; }
template <> inline auto fromQVector<QVector2D>(QVector2D&& vec) { return std::array<float, 2>{ vec.x(), vec.y() }; }
template <> inline auto fromQVector<QVector3D>(QVector3D&& vec) { return std::array<float, 3>{ vec.x(), vec.y(), vec.z() }; }
template <> inline auto fromQVector<QVector4D>(QVector4D&& vec) { return std::array<float, 4>{ vec.x(), vec.y(), vec.z(), vec.w() }; }

// inline QVector2D hermite(std::array<QVector2D, 4>&& p, double t) {        
//     const static auto InvHermiteMat = QMatrix4x4{
//         0.f, 0.f, 0.f, 1.f,
//         1.f, 1.f, 1.f, 1.f,
//         0.f, 0.f, 1.f, 0.f,
//         3.f, 2.f, 1.f, 0.f
//     }.inverted();
    
//     QVector4D weights[2]{
//         { (p[0])[0], (p[1])[0], (p[2])[0], (p[3])[0] },
//         { (p[0])[1], (p[1])[1], (p[2])[1], (p[3])[1] },
//     };

//     weights[0] = InvHermiteMat * weights[0];
//     weights[1] = InvHermiteMat * weights[1];

//     const auto [a, b, c, d] = std::make_tuple(
//         QVector2D{weights[0][0], weights[1][0]},
//         QVector2D{weights[0][1], weights[1][1]},
//         QVector2D{weights[0][2], weights[1][2]},
//         QVector2D{weights[0][3], weights[1][3]}
//     );
//     return a * t * t * t + b * t * t + c * t + d;
// }

// Generic hermite interpolation, for any types: {float, QVector2D, QVector3D, QVector4D}
template <std::size_t N>
auto hermite(std::array<std::array<float, N>, 4>&& p, double t) {    
    const static auto InvHermiteMat = QMatrix4x4{
        0.f, 0.f, 0.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        0.f, 0.f, 1.f, 0.f,
        3.f, 2.f, 1.f, 0.f
    }.inverted();
    
    QVector4D weights[N];
    for (unsigned int i{0}; i < N; ++i)
        weights[i] = InvHermiteMat * QVector4D{p[0][i], p[1][i], p[2][i], p[3][i]};

    const auto v = [&](unsigned int&& i){
        float vec[N];
        for (unsigned int j{0}; j < N; ++j)
            vec[j] = weights[j][i];
        return toQVector<N>(std::move(vec));
    };

    const auto& [a, b, c, d] = std::make_tuple( v(0), v(1), v(2), v(3) );
    return a * t * t * t + b * t * t + c * t + d;
}

// Generic hermite interpolation, for any types: {float, QVector2D, QVector3D, QVector4D}
template <typename T>
auto hermite(T (&& p)[4], double t) {
    using K = decltype(fromQVector<T>({}));
    auto arr = std::array<K, 4>{
        fromQVector(std::move(p[0])),
        fromQVector(std::move(p[1])),
        fromQVector(std::move(p[2])),
        fromQVector(std::move(p[3]))
    };
    return hermite(std::move(arr), t);
}

QVector2D piecewiseSpline(const std::vector<QVector2D>& p, double t);
template <typename T>
T piecewiseLerp(const std::vector<T>& p, double t) {
    if (p.size() < 2)
        return T{};
    else if (t < 0.0)
        return p.front();
    else if (1.0 <= t)
        return p.back();

    auto f = (p.size()-1) * t;
    const auto i = static_cast<unsigned int>(f);
    f -= i;

    return (1.0 - f) * p[i] + f * p[i+1];
}

/**
 * @brief Uses Newton-Raphson method to partially solve one root of
 * a cubic equation defined with coefficients a, b, c and d.
 * 
 * @param coefficients array of 4 floats depicting the a, b, c and d coefficients
 * @return First approximated root
 */
float partialCubicSolve(float (&& coefficients)[4], float initialGuess);

float signedDistToPlane(const QVector3D& ppos, const QVector3D& pdir, const QVector3D& point);
bool boxPlaneIntersect(const QVector3D& bpos, const QVector3D& bsize, const QVector3D& ppos, const QVector3D& pdir);

}

#endif // MEGAMATH_H