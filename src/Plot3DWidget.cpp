#include "Plot3DWidget.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <cmath>
#include <algorithm>
#include <array>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─────────────────────────────────────────────────────────────
//  Helpers: 3-D → 2-D perspective projection
//
//  The camera looks at (panX, panY, 0) from a spherical
//  position defined by azimuth (yaw) and elevation (pitch).
//
//  Rotation order: first rotate around world-Z by -azimuth,
//  then rotate the result around the camera-X by -elevation.
// ─────────────────────────────────────────────────────────────

Plot3DWidget::Plot3DWidget(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumSize(400, 300);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

// ──────────────────────────────────────────
//  Public interface
// ──────────────────────────────────────────

void Plot3DWidget::addCurve(const Curve3D &curve) {
    m_curves.push_back(curve);
    update();
}

void Plot3DWidget::clearCurves() {
    m_curves.clear();
    update();
}

void Plot3DWidget::resetView() {
    m_azimuth   = 30.0;
    m_elevation = 25.0;
    m_zoom      = 60.0;
    m_panX = m_panY = 0.0;
    update();
}

// ──────────────────────────────────────────
//  Projection
// ──────────────────────────────────────────

QPointF Plot3DWidget::project(double wx, double wy, double wz, double *depth) const {
    // Translate by pan
    double tx = wx - m_panX;
    double ty = wy - m_panY;
    double tz = wz;

    const double az = m_azimuth   * M_PI / 180.0;
    const double el = m_elevation * M_PI / 180.0;

    // Rotate around Z by azimuth
    double rx = tx * std::cos(az) + ty * std::sin(az);
    double ry = -tx * std::sin(az) + ty * std::cos(az);
    double rz = tz;

    // Rotate around (new) X by elevation
    double cx =  rx;
    double cy =  ry * std::cos(el) - rz * std::sin(el);
    double cz =  ry * std::sin(el) + rz * std::cos(el);

    // Perspective: camera is at z = cameraDistance along camera-Z
    const double camDist = 8.0;
    double perspective = camDist / (camDist + cz);

    double sx = width()  / 2.0 + cx * perspective * m_zoom;
    double sy = height() / 2.0 - cy * perspective * m_zoom;

    if (depth) *depth = cz;
    return {sx, sy};
}

// ──────────────────────────────────────────
//  Painting
// ──────────────────────────────────────────

void Plot3DWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(245, 248, 252));

    drawGrid(p);
    drawAxes(p);
    drawCurves(p);
    drawLabels(p);

    // Instructions overlay
    p.setPen(QColor(130, 140, 160));
    QFont f = p.font();
    f.setPointSize(8);
    p.setFont(f);
    p.drawText(rect().adjusted(4, 0, -4, -4),
               Qt::AlignBottom | Qt::AlignLeft,
               "Left-drag: rotate  |  Right-drag / Shift+drag: pan  |  Wheel: zoom  |  WASD: move camera  |  Double-click: reset");
}

void Plot3DWidget::drawGrid(QPainter &p) {
    // Draw a flat ground grid on the z=0 plane
    const int lines = 20;
    const double step = 1.0;
    const double limit = lines * step;

    for (int i = -lines; i <= lines; ++i) {
        const double d = i * step;
        const bool majorLine = (i % 5 == 0);
        QPen gridPen(majorLine ? QColor(186, 195, 210) : QColor(214, 221, 232), majorLine ? 1.25 : 1.0);
        p.setPen(gridPen);

        p.drawLine(project(d, -limit, 0), project(d, limit, 0));
        p.drawLine(project(-limit, d, 0), project(limit, d, 0));
    }
}

void Plot3DWidget::drawAxes(QPainter &p) {
    const double axisLen = 12.0;

    struct Axis {
        double x0, y0, z0, x1, y1, z1;
        QColor color;
        QString label;
    };

    std::array<Axis, 3> axes = {{
        {0,0,0, axisLen,0,0, QColor(200,60,60), "X"},
        {0,0,0, 0,axisLen,0, QColor(60,180,60), "Y"},
        {0,0,0, 0,0,axisLen, QColor(60,100,220), "Z"},
    }};

    for (const auto &ax : axes) {
        QPen pen(ax.color, 3);
        p.setPen(pen);
        QPointF from = project(ax.x0, ax.y0, ax.z0);
        QPointF to   = project(ax.x1, ax.y1, ax.z1);
        p.drawLine(from, to);

        QPen arrowPen(ax.color, 2);
        p.setPen(arrowPen);
        p.drawEllipse(to, 2.0, 2.0);

        p.setPen(ax.color);
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(10);
        p.setFont(f);
        p.drawText(to + QPointF(4, 4), ax.label);
        f.setBold(false);
        p.setFont(f);
    }

    // Draw negative (dashed) part
    QPen dashPen(QColor(180, 190, 200), 1, Qt::DashLine);
    p.setPen(dashPen);
    p.drawLine(project(0,0,0), project(-axisLen/2,0,0));
    p.drawLine(project(0,0,0), project(0,-axisLen/2,0));
    p.drawLine(project(0,0,0), project(0,0,-axisLen/2));
}

void Plot3DWidget::drawCurves(QPainter &p) {
    for (const auto &curve : m_curves) {
        if (curve.points.empty()) continue;

        QPen pen(curve.style.color, curve.style.lineWidth, Qt::SolidLine,
                 Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);

        bool penDown = false;
        QPointF prev;

        for (const auto &pt : curve.points) {
            if (!pt.valid ||
                std::isnan(pt.x) || std::isnan(pt.y) || std::isnan(pt.z) ||
                std::isinf(pt.x) || std::isinf(pt.y) || std::isinf(pt.z)) {
                penDown = false;
                continue;
            }
            double depth = 0;
            QPointF scr = project(pt.x, pt.y, pt.z, &depth);

            // Simple depth cull – skip points behind the camera
            if (depth < -20.0) { penDown = false; continue; }

            const double margin = 10000;
            if (scr.x() < -margin || scr.x() > width() + margin ||
                scr.y() < -margin || scr.y() > height() + margin) {
                penDown = false;
                prev = scr;
                continue;
            }

            if (penDown) {
                p.drawLine(prev, scr);
            }
            prev    = scr;
            penDown = true;
        }
    }
}

void Plot3DWidget::drawLabels(QPainter &p) {
    int y = 10;
    for (const auto &curve : m_curves) {
        if (curve.label.isEmpty()) continue;
        p.setPen(QPen(curve.style.color, curve.style.lineWidth));
        p.drawLine(10, y + 5, 30, y + 5);
        p.setPen(QColor(40, 40, 60));
        p.drawText(35, y + 9, curve.label);
        y += 20;
    }
}

// ──────────────────────────────────────────
//  Mouse / wheel events
// ──────────────────────────────────────────

void Plot3DWidget::mousePressEvent(QMouseEvent *event) {
    setFocus();
    m_lastMouse = event->position().toPoint();

    if (event->button() == Qt::LeftButton &&
        !(event->modifiers() & Qt::ShiftModifier)) {
        m_rotating = true;
        m_startAz  = m_azimuth;
        m_startEl  = m_elevation;
        setCursor(Qt::SizeAllCursor);
    } else {
        m_panning   = true;
        m_startPanX = m_panX;
        m_startPanY = m_panY;
        setCursor(Qt::ClosedHandCursor);
    }
}

void Plot3DWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = event->position().toPoint() - m_lastMouse;

    if (m_rotating) {
        m_azimuth   = m_startAz  - delta.x() * 0.5;
        m_elevation = std::clamp(m_startEl - delta.y() * 0.5, -89.0, 89.0);
        update();
    } else if (m_panning) {
        double panScale = 1.0 / m_zoom;
        m_panX = m_startPanX - delta.x() * panScale;
        m_panY = m_startPanY + delta.y() * panScale;
        update();
    }
}

void Plot3DWidget::mouseReleaseEvent(QMouseEvent *) {
    m_rotating = m_panning = false;
    setCursor(Qt::ArrowCursor);
}

void Plot3DWidget::wheelEvent(QWheelEvent *event) {
    double factor = (event->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
    m_zoom = std::clamp(m_zoom * factor, 5.0, 2000.0);
    update();
}

void Plot3DWidget::mouseDoubleClickEvent(QMouseEvent *) {
    resetView();
}

void Plot3DWidget::keyPressEvent(QKeyEvent *event) {
    double moveStep = std::max(0.2, 12.0 / m_zoom);
    const double az = m_azimuth * M_PI / 180.0;
    const double cosAz = std::cos(az);
    const double sinAz = std::sin(az);

    bool handled = true;
    switch (event->key()) {
    case Qt::Key_W:
        m_panX += sinAz * moveStep;
        m_panY += cosAz * moveStep;
        break;
    case Qt::Key_S:
        m_panX -= sinAz * moveStep;
        m_panY -= cosAz * moveStep;
        break;
    case Qt::Key_A:
        m_panX -= cosAz * moveStep;
        m_panY += sinAz * moveStep;
        break;
    case Qt::Key_D:
        m_panX += cosAz * moveStep;
        m_panY -= sinAz * moveStep;
        break;
    default:
        handled = false;
        break;
    }

    if (handled) {
        update();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
