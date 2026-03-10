#include "Plot2DWidget.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>
#include <algorithm>

Plot2DWidget::Plot2DWidget(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMinimumSize(400, 300);
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
}

// ──────────────────────────────────────────
//  Public interface
// ──────────────────────────────────────────

void Plot2DWidget::addCurve(const Curve2D &curve) {
    m_curves.push_back(curve);
    update();
}

void Plot2DWidget::clearCurves() {
    m_curves.clear();
    update();
}

void Plot2DWidget::resetView() {
    m_viewX  = -10.0;
    m_viewY  = -7.0;
    m_scale  = 50.0;
    update();
}

// ──────────────────────────────────────────
//  Coordinate transforms
// ──────────────────────────────────────────

QPointF Plot2DWidget::worldToScreen(double wx, double wy) const {
    double sx = (wx - m_viewX) * m_scale;
    double sy = height() - (wy - m_viewY) * m_scale;
    return {sx, sy};
}

QPointF Plot2DWidget::screenToWorld(double sx, double sy) const {
    double wx = sx / m_scale + m_viewX;
    double wy = (height() - sy) / m_scale + m_viewY;
    return {wx, wy};
}

// ──────────────────────────────────────────
//  Painting
// ──────────────────────────────────────────

void Plot2DWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(245, 248, 252));

    drawGrid(p);
    drawAxes(p);
    drawCurves(p);
    drawLabels(p);
}

void Plot2DWidget::drawGrid(QPainter &p) {
    // Choose a nice grid step
    double rawStep = 1.0;
    double pixStep = rawStep * m_scale;
    // Adjust step so grid lines are not too dense / sparse
    while (pixStep < 30) { rawStep *= 2; pixStep = rawStep * m_scale; }
    while (pixStep > 120) { rawStep /= 2; pixStep = rawStep * m_scale; }

    QPen gridPen(QColor(210, 215, 225), 1, Qt::SolidLine);
    p.setPen(gridPen);

    // Vertical grid lines
    double xStart = std::ceil(m_viewX / rawStep) * rawStep;
    double xEnd   = m_viewX + width() / m_scale;
    for (double x = xStart; x <= xEnd; x += rawStep) {
        QPointF s = worldToScreen(x, 0);
        p.drawLine(QPointF(s.x(), 0), QPointF(s.x(), height()));
    }

    // Horizontal grid lines
    double yStart = std::ceil(m_viewY / rawStep) * rawStep;
    double yEnd   = m_viewY + height() / m_scale;
    for (double y = yStart; y <= yEnd; y += rawStep) {
        QPointF s = worldToScreen(0, y);
        p.drawLine(QPointF(0, s.y()), QPointF(width(), s.y()));
    }

    // Tick labels
    p.setPen(QColor(100, 110, 130));
    QFont font = p.font();
    font.setPointSize(8);
    p.setFont(font);

    for (double x = xStart; x <= xEnd; x += rawStep) {
        if (std::abs(x) < rawStep * 0.01) continue;
        QPointF s = worldToScreen(x, 0);
        if (s.y() < 0 || s.y() > height()) continue;
        QString label = (std::abs(x - std::round(x)) < 1e-9)
                        ? QString::number((int)std::round(x))
                        : QString::number(x, 'g', 4);
        p.drawText(QRectF(s.x() - 20, s.y() + 3, 40, 14),
                   Qt::AlignHCenter, label);
    }

    for (double y = yStart; y <= yEnd; y += rawStep) {
        if (std::abs(y) < rawStep * 0.01) continue;
        QPointF s = worldToScreen(0, y);
        if (s.x() < 0 || s.x() > width()) continue;
        QString label = (std::abs(y - std::round(y)) < 1e-9)
                        ? QString::number((int)std::round(y))
                        : QString::number(y, 'g', 4);
        p.drawText(QRectF(s.x() + 4, s.y() - 7, 36, 14),
                   Qt::AlignLeft | Qt::AlignVCenter, label);
    }
}

void Plot2DWidget::drawAxes(QPainter &p) {
    QPen axisPen(QColor(60, 70, 90), 2);
    p.setPen(axisPen);

    // X axis (y=0)
    if (m_viewY <= 0 && 0 <= m_viewY + height() / m_scale) {
        QPointF left  = worldToScreen(m_viewX, 0);
        QPointF right = worldToScreen(m_viewX + width() / m_scale, 0);
        p.drawLine(left, right);
        // Arrow
        p.drawLine(right, QPointF(right.x() - 8, right.y() - 4));
        p.drawLine(right, QPointF(right.x() - 8, right.y() + 4));
        // Label
        p.drawText(QPointF(right.x() - 12, right.y() - 6), "x");
    }

    // Y axis (x=0)
    if (m_viewX <= 0 && 0 <= m_viewX + width() / m_scale) {
        QPointF bottom = worldToScreen(0, m_viewY);
        QPointF top    = worldToScreen(0, m_viewY + height() / m_scale);
        p.drawLine(bottom, top);
        // Arrow
        p.drawLine(top, QPointF(top.x() - 4, top.y() + 8));
        p.drawLine(top, QPointF(top.x() + 4, top.y() + 8));
        // Label
        p.drawText(QPointF(top.x() + 6, top.y() + 12), "y");
    }
}

void Plot2DWidget::drawCurves(QPainter &p) {
    for (const auto &curve : m_curves) {
        if (curve.points.empty()) continue;

        QPen pen(curve.style.color, curve.style.lineWidth, Qt::SolidLine,
                 Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);

        bool penDown = false;
        QPointF prev;

        for (const auto &pt : curve.points) {
            if (!pt.valid || std::isnan(pt.x) || std::isnan(pt.y) ||
                std::isinf(pt.x) || std::isinf(pt.y)) {
                penDown = false;
                continue;
            }
            QPointF scr = worldToScreen(pt.x, pt.y);

            // Clip points very far outside viewport to avoid QPainter overflow
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

void Plot2DWidget::drawLabels(QPainter &p) {
    int y = 10;
    for (const auto &curve : m_curves) {
        if (curve.label.isEmpty()) continue;
        // Draw a short sample line
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

void Plot2DWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_panning  = true;
        m_panStart = event->position();
        m_panViewX = m_viewX;
        m_panViewY = m_viewY;
        setCursor(Qt::ClosedHandCursor);
    }
}

void Plot2DWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_panning) {
        QPointF delta = event->position() - m_panStart;
        m_viewX = m_panViewX - delta.x() / m_scale;
        m_viewY = m_panViewY + delta.y() / m_scale;
        update();
    }
}

void Plot2DWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_panning = false;
        setCursor(Qt::CrossCursor);
    }
}

void Plot2DWidget::wheelEvent(QWheelEvent *event) {
    // Zoom centred on the mouse position
    QPointF mouse = event->position();
    QPointF wBefore = screenToWorld(mouse.x(), mouse.y());

    double factor = (event->angleDelta().y() > 0) ? 1.15 : (1.0 / 1.15);
    m_scale = std::clamp(m_scale * factor, 1.0, 5000.0);

    // Adjust view so the world point under the cursor stays fixed
    m_viewX = wBefore.x() - mouse.x() / m_scale;
    m_viewY = wBefore.y() - (height() - mouse.y()) / m_scale;
    update();
}

void Plot2DWidget::mouseDoubleClickEvent(QMouseEvent *) {
    resetView();
}
