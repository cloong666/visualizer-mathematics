#pragma once
#include <QWidget>
#include <QPointF>
#include "CurveData.h"

class Plot2DWidget : public QWidget {
    Q_OBJECT
public:
    explicit Plot2DWidget(QWidget *parent = nullptr);

    void addCurve(const Curve2D &curve);
    void clearCurves();
    void resetView();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    // World ↔ screen helpers
    QPointF worldToScreen(double wx, double wy) const;
    QPointF screenToWorld(double sx, double sy) const;

    void drawGrid(QPainter &p);
    void drawAxes(QPainter &p);
    void drawCurves(QPainter &p);
    void drawLabels(QPainter &p);

    std::vector<Curve2D> m_curves;

    // View state: world coordinates visible in the viewport
    double m_viewX{-10.0};   // world x of left edge
    double m_viewY{-7.0};    // world y of bottom edge
    double m_scale{50.0};    // pixels per world unit

    // Pan state
    bool    m_panning{false};
    QPointF m_panStart;
    double  m_panViewX{0};
    double  m_panViewY{0};
};
