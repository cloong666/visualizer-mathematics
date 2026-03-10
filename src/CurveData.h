#pragma once
#include <QColor>
#include <QString>
#include <vector>

enum class CurveMode {
    Explicit2D,   // y = f(x)
    Parametric2D, // x(t), y(t)
    Parametric3D  // x(t), y(t), z(t)
};

struct CurveStyle {
    QColor color      = Qt::blue;
    float  lineWidth  = 2.0f;
};

// A single 2-D sample point; valid=false means skip (NaN/Inf/domain error).
struct Point2D {
    double x{0}, y{0};
    bool   valid{true};
};

// A single 3-D sample point.
struct Point3D {
    double x{0}, y{0}, z{0};
    bool   valid{true};
};

struct Curve2D {
    std::vector<Point2D> points;
    CurveStyle           style;
    QString              label;
};

struct Curve3D {
    std::vector<Point3D> points;
    CurveStyle           style;
    QString              label;
};
