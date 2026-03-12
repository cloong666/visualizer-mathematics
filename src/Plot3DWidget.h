#pragma once
#include <QWidget>
#include <QPoint>
#include "CurveData.h"

class QKeyEvent;

class Plot3DWidget : public QWidget {
    Q_OBJECT
public:
    explicit Plot3DWidget(QWidget *parent = nullptr);

    void addCurve(const Curve3D &curve);
    void clearCurves();
    void resetView();
    int  curveCount() const { return static_cast<int>(m_curves.size()); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Project a 3-D world point to 2-D screen coordinates.
    // Returns a QPointF; sets z_out to the projected depth (used for clipping).
    QPointF project(double wx, double wy, double wz, double *depth = nullptr) const;

    void drawGrid(QPainter &p);
    void drawAxes(QPainter &p);
    void drawCurves(QPainter &p);
    void drawLabels(QPainter &p);

    std::vector<Curve3D> m_curves;

    // Camera / view
    double m_azimuth{30.0};   // degrees, rotation around Z
    double m_elevation{25.0}; // degrees, tilt up/down
    double m_zoom{60.0};      // world units → pixels

    // Pan offset in world-space (shift of the camera target)
    double m_panX{0.0};
    double m_panY{0.0};
    double m_panZ{0.0};

    // Drag state
    bool    m_rotating{false};
    bool    m_panning{false};
    QPoint  m_lastMouse;
    double  m_startAz{0};
    double  m_startEl{0};
    double  m_startPanX{0};
    double  m_startPanY{0};
};
