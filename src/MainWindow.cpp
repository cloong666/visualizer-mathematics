#include "MainWindow.h"
#include "Plot2DWidget.h"
#include "Plot3DWidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QLabel>
#include <QScrollArea>
#include <QStatusBar>
#include <QColorDialog>
#include <QPalette>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>
#include <vector>

// ─────────────────────────────────────────────────────────────
//  Mode indices
// ─────────────────────────────────────────────────────────────
static constexpr int MODE_2D_EXPLICIT   = 0;
static constexpr int MODE_2D_IMPLICIT   = 1;
static constexpr int MODE_2D_PARAMETRIC = 2;
static constexpr int MODE_3D_PARAMETRIC = 3;
static constexpr int MODE_3D_SURFACE    = 4;
static constexpr int MODE_SEPARATOR     = -1;
static const QRegularExpression RX_X_OF_T("^x\\s*(\\(\\s*[tT]\\s*\\))?\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);
static const QRegularExpression RX_Y_OF_T("^y\\s*(\\(\\s*[tT]\\s*\\))?\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);
static const QRegularExpression RX_Z_OF_T("^z\\s*(\\(\\s*[tT]\\s*\\))?\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);

// ─────────────────────────────────────────────────────────────
//  Built-in examples
// ─────────────────────────────────────────────────────────────
struct Example {
    QString name;
    int     mode;
    QString y, x, yp, xp, yp3, zp3;   // reuse fields flexibly
    double  tMin{-10}, tMax{10};
    int     samples{500};
    QColor  color;
};

static const QList<Example> g_examples = {
    Example{ "──── 2D Explicit ────", MODE_SEPARATOR, {},{},{},{},{},{}, -10, 10, 500, Qt::blue },
    Example{ "Sine wave  y=sin(x)",       MODE_2D_EXPLICIT, "sin(x)", {},{},{},{},{}, -6.28, 6.28, 400, QColor(0,120,200) },
    Example{ "Parabola  y=x^2",           MODE_2D_EXPLICIT, "x^2",    {},{},{},{},{}, -5,    5,    300, QColor(200,80,0) },
    Example{ "Cubic  y=x^3-3*x",          MODE_2D_EXPLICIT, "x^3-3*x",{},{},{},{},{}, -3,    3,    300, QColor(160,0,200) },
    Example{ "Gaussian  y=exp(-x^2)",     MODE_2D_EXPLICIT, "exp(-x^2)",{},{},{},{},{}, -4,   4,    400, QColor(0,180,100) },
    Example{ "Damped sine  y=sin(x)/x",   MODE_2D_EXPLICIT, "sin(x)/x", {},{},{},{},{}, -20,  20,   600, QColor(200,0,100) },
    Example{ "Abs cosine  y=abs(cos(x))",  MODE_2D_EXPLICIT, "abs(cos(x))",{},{},{},{},{}, -6.28, 6.28, 400, QColor(0,160,180) },
    Example{ "──── 2D Implicit ────", MODE_SEPARATOR, {},{},{},{},{},{}, -10, 10, 500, Qt::blue },
    Example{ "Circle  x^2+y^2=4",          MODE_2D_IMPLICIT, "x^2 + y^2 = 4", {},{},{},{},{}, -3, 3, 500, QColor(180,70,0) },
    Example{ "Lemniscate  (x^2+y^2)^2=2(x^2-y^2)", MODE_2D_IMPLICIT, "(x^2+y^2)^2 = 2*(x^2-y^2)", {},{},{},{},{}, -2, 2, 600, QColor(140,0,200) },
    Example{ "──── 2D Parametric ────", MODE_SEPARATOR, {},{},{},{},{},{}, 0, 6.28, 500, Qt::blue },
    Example{ "Circle  x=cos(t), y=sin(t)",          MODE_2D_PARAMETRIC, {}, "cos(t)", "sin(t)", {},{}, {}, 0, 6.28, 360, QColor(0,100,220) },
    Example{ "Ellipse  x=2cos(t), y=sin(t)",        MODE_2D_PARAMETRIC, {}, "2*cos(t)", "sin(t)", {},{}, {}, 0, 6.28, 360, QColor(200,60,0) },
    Example{ "Lissajous  x=sin(3t), y=sin(2t)",     MODE_2D_PARAMETRIC, {}, "sin(3*t)", "sin(2*t)", {},{}, {}, 0, 6.28, 600, QColor(160,0,200) },
    Example{ "Archimedean spiral  x=t*cos(t),y=t*sin(t)", MODE_2D_PARAMETRIC, {}, "t*cos(t)", "t*sin(t)", {},{}, {}, 0, 25.13, 800, QColor(0,160,80) },
    Example{ "Rose  x=cos(4t)cos(t), y=cos(4t)sin(t)",    MODE_2D_PARAMETRIC, {}, "cos(4*t)*cos(t)", "cos(4*t)*sin(t)", {},{}, {}, 0, 6.28, 800, QColor(220,30,80) },
    Example{ "──── 3D Parametric ────", MODE_SEPARATOR, {},{},{},{},{},{}, 0, 30, 600, Qt::blue },
    Example{ "Helix  x=cos(t), y=sin(t), z=t/5",           MODE_3D_PARAMETRIC, {},{},{}, "cos(t)", "sin(t)", "t/5",    0, 30, 600, QColor(0,120,220) },
    Example{ "Conical helix  x=t*cos(t),y=t*sin(t),z=t",   MODE_3D_PARAMETRIC, {},{},{}, "t*cos(t)", "t*sin(t)", "t",  0, 12.56, 600, QColor(200,80,0) },
    Example{ "Trefoil knot",                                MODE_3D_PARAMETRIC, {},{},{},
      "sin(t)+2*sin(2*t)", "cos(t)-2*cos(2*t)", "-sin(3*t)", 0, 6.28, 600, QColor(160,0,200) },
    Example{ "Viviani curve",                               MODE_3D_PARAMETRIC, {},{},{},
      "1-cos(t)", "sin(t)", "2*sin(t/2)",  0, 12.56, 600, QColor(0,180,100) },
    Example{ "──── 3D Surface ────", MODE_SEPARATOR, {},{},{},{},{},{}, -5, 5, 100, Qt::blue },
    Example{ "Paraboloid  z=x^2+y^2", MODE_3D_SURFACE, "x^2 + y^2", {},{},{},{},{}, -2, 2, 60, QColor(180,80,0) },
    Example{ "Saddle  z=x^2-y^2", MODE_3D_SURFACE, "x^2 - y^2", {},{},{},{},{}, -2, 2, 60, QColor(100,70,200) },
    Example{ "Wave  z=sin(x)*cos(y)", MODE_3D_SURFACE, "sin(x)*cos(y)", {},{},{},{},{}, -6.28, 6.28, 80, QColor(0,140,220) },
};

// ─────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("2D / 3D Curve Visualizer");
    resize(1100, 680);

    // ── Canvases ──
    m_plot2D = new Plot2DWidget;
    m_plot3D = new Plot3DWidget;

    m_canvasStack = new QStackedWidget;
    m_canvasStack->addWidget(m_plot2D);
    m_canvasStack->addWidget(m_plot3D);
    m_canvasStack->setCurrentIndex(0);

    // ── Log ──
    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setMaximumHeight(90);
    m_logView->setFont(QFont("Consolas", 9));
    m_logView->setPlaceholderText("Messages and errors appear here…");

    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(m_canvasStack, 1);
    rightLayout->addWidget(m_logView);

    // ── Splitter ──
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(buildControlPanel());
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({280, 800});

    setCentralWidget(splitter);
    statusBar()->showMessage("Ready – select an example or enter an equation, then click Plot");

    onModeChanged(0);

    // Auto-load the first real example
    m_examplesCombo->setCurrentIndex(1);
    onExampleSelected(1);
}

// ─────────────────────────────────────────────────────────────
//  Build control panel
// ─────────────────────────────────────────────────────────────
QWidget *MainWindow::buildControlPanel() {
    QWidget *panel = new QWidget;
    panel->setMinimumWidth(240);
    panel->setMaximumWidth(310);

    QVBoxLayout *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(8);

    // ── Mode ──
    QGroupBox *modeGroup = new QGroupBox("Mode");
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);
    m_modeCombo = new QComboBox;
    m_modeCombo->addItem("2D Explicit  ( y = f(x) )");
    m_modeCombo->addItem("2D General Curve  ( F(x,y)=0 )");
    m_modeCombo->addItem("2D Parametric  ( x(t), y(t) )");
    m_modeCombo->addItem("3D Parametric  ( x(t), y(t), z(t) )");
    m_modeCombo->addItem("3D Surface  ( z = f(x,y) )");
    modeLayout->addWidget(m_modeCombo);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    layout->addWidget(modeGroup);
    layout->addWidget(buildEquationGroup());
    layout->addWidget(buildRangeGroup());
    layout->addWidget(buildStyleGroup());
    layout->addWidget(buildExamplesGroup());

    // ── Plot / Clear / Reset buttons ──
    QPushButton *plotBtn  = new QPushButton("▶  Plot");
    QPushButton *clearBtn = new QPushButton("✕  Clear");
    QPushButton *resetBtn = new QPushButton("⟳  Reset View");

    plotBtn->setDefault(true);
    plotBtn->setStyleSheet("QPushButton { background:#2962ff; color:white; font-weight:bold; padding:6px; border-radius:4px; }"
                           "QPushButton:hover { background:#1a47c2; }");
    clearBtn->setStyleSheet("QPushButton { padding:6px; border-radius:4px; }");
    resetBtn->setStyleSheet("QPushButton { padding:6px; border-radius:4px; }");

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addWidget(plotBtn);
    btnRow->addWidget(clearBtn);
    btnRow->addWidget(resetBtn);

    layout->addLayout(btnRow);
    layout->addStretch();

    connect(plotBtn,  &QPushButton::clicked, this, &MainWindow::onPlotClicked);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetViewClicked);

    return panel;
}

QGroupBox *MainWindow::buildEquationGroup() {
    QGroupBox *group = new QGroupBox("Equation");
    QVBoxLayout *layout = new QVBoxLayout(group);

    // ── 2D Explicit ──
    m_eq2DExplicitWidget = new QWidget;
    QFormLayout *f1 = new QFormLayout(m_eq2DExplicitWidget);
    f1->setContentsMargins(0,0,0,0);
    m_yExpr = new QLineEdit("sin(x)");
    m_yExpr->setPlaceholderText("e.g. sin(x)  or  x^2+1");
    f1->addRow("y =", m_yExpr);

    // ── 2D Implicit ──
    m_eq2DImplicitWidget = new QWidget;
    QFormLayout *fImplicit = new QFormLayout(m_eq2DImplicitWidget);
    fImplicit->setContentsMargins(0,0,0,0);
    m_curveExpr2D = new QLineEdit("x^2 + y^2 = 1");
    m_curveExpr2D->setPlaceholderText("e.g. x^2 + y^2 = 1");
    fImplicit->addRow("Equation:", m_curveExpr2D);

    // ── 2D Parametric ──
    m_eq2DParamWidget = new QWidget;
    QFormLayout *f2 = new QFormLayout(m_eq2DParamWidget);
    f2->setContentsMargins(0,0,0,0);
    m_xExpr2D = new QLineEdit("cos(t)");
    m_yExpr2D = new QLineEdit("sin(t)");
    m_xExpr2D->setPlaceholderText("e.g. cos(t)");
    m_yExpr2D->setPlaceholderText("e.g. sin(t)");
    f2->addRow("x(t) =", m_xExpr2D);
    f2->addRow("y(t) =", m_yExpr2D);

    // ── 3D Parametric ──
    m_eq3DParamWidget = new QWidget;
    QFormLayout *f3 = new QFormLayout(m_eq3DParamWidget);
    f3->setContentsMargins(0,0,0,0);
    m_xExpr3D = new QLineEdit("cos(t)");
    m_yExpr3D = new QLineEdit("sin(t)");
    m_zExpr3D = new QLineEdit("t/5");
    m_xExpr3D->setPlaceholderText("e.g. cos(t)");
    m_yExpr3D->setPlaceholderText("e.g. sin(t)");
    m_zExpr3D->setPlaceholderText("e.g. t/5");
    f3->addRow("x(t) =", m_xExpr3D);
    f3->addRow("y(t) =", m_yExpr3D);
    f3->addRow("z(t) =", m_zExpr3D);

    // ── 3D Surface ──
    m_eq3DSurfaceWidget = new QWidget;
    QFormLayout *f4 = new QFormLayout(m_eq3DSurfaceWidget);
    f4->setContentsMargins(0,0,0,0);
    m_surfaceExpr3D = new QLineEdit("sin(x)*cos(y)");
    m_surfaceExpr3D->setPlaceholderText("e.g. x^2+y^2 or sin(x)*cos(y)");
    f4->addRow("z(x,y) =", m_surfaceExpr3D);

    layout->addWidget(m_eq2DExplicitWidget);
    layout->addWidget(m_eq2DImplicitWidget);
    layout->addWidget(m_eq2DParamWidget);
    layout->addWidget(m_eq3DParamWidget);
    layout->addWidget(m_eq3DSurfaceWidget);

    return group;
}

QGroupBox *MainWindow::buildRangeGroup() {
    QGroupBox *group = new QGroupBox("Range & Samples");
    QFormLayout *layout = new QFormLayout(group);

    m_tMinSpin = new QDoubleSpinBox;
    m_tMinSpin->setRange(-1e6, 1e6);
    m_tMinSpin->setDecimals(4);
    m_tMinSpin->setValue(-6.2832);
    m_tMinSpin->setSingleStep(0.5);

    m_tMaxSpin = new QDoubleSpinBox;
    m_tMaxSpin->setRange(-1e6, 1e6);
    m_tMaxSpin->setDecimals(4);
    m_tMaxSpin->setValue(6.2832);
    m_tMaxSpin->setSingleStep(0.5);

    m_samplesSpin = new QSpinBox;
    m_samplesSpin->setRange(10, 100000);
    m_samplesSpin->setValue(500);
    m_samplesSpin->setSingleStep(100);

    layout->addRow("Min :", m_tMinSpin);
    layout->addRow("Max :", m_tMaxSpin);
    layout->addRow("Samples:", m_samplesSpin);

    return group;
}

QGroupBox *MainWindow::buildStyleGroup() {
    QGroupBox *group = new QGroupBox("Style");
    QFormLayout *layout = new QFormLayout(group);

    m_colorButton = new QPushButton;
    m_colorButton->setFixedHeight(24);
    updateColorButton();
    connect(m_colorButton, &QPushButton::clicked,
            this, &MainWindow::onColorButtonClicked);

    m_lineWidthSpin = new QDoubleSpinBox;
    m_lineWidthSpin->setRange(0.5, 10.0);
    m_lineWidthSpin->setValue(2.0);
    m_lineWidthSpin->setSingleStep(0.5);

    layout->addRow("Color:", m_colorButton);
    layout->addRow("Width:", m_lineWidthSpin);

    return group;
}

QGroupBox *MainWindow::buildExamplesGroup() {
    QGroupBox *group = new QGroupBox("Examples");
    QVBoxLayout *layout = new QVBoxLayout(group);

    m_examplesCombo = new QComboBox;
    for (const auto &ex : g_examples)
        m_examplesCombo->addItem(ex.name);

    connect(m_examplesCombo, QOverload<int>::of(&QComboBox::activated),
            this, &MainWindow::onExampleSelected);

    layout->addWidget(m_examplesCombo);
    return group;
}

// ─────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────

void MainWindow::onModeChanged(int index) {
    bool is2DExplicit   = (index == MODE_2D_EXPLICIT);
    bool is2DImplicit   = (index == MODE_2D_IMPLICIT);
    bool is2DParametric = (index == MODE_2D_PARAMETRIC);
    bool is3DParametric = (index == MODE_3D_PARAMETRIC);
    bool is3DSurface    = (index == MODE_3D_SURFACE);

    if (m_eq2DExplicitWidget)   m_eq2DExplicitWidget->setVisible(is2DExplicit);
    if (m_eq2DImplicitWidget)   m_eq2DImplicitWidget->setVisible(is2DImplicit);
    if (m_eq2DParamWidget)      m_eq2DParamWidget->setVisible(is2DParametric);
    if (m_eq3DParamWidget)      m_eq3DParamWidget->setVisible(is3DParametric);
    if (m_eq3DSurfaceWidget)    m_eq3DSurfaceWidget->setVisible(is3DSurface);

    // Switch canvas
    m_canvasStack->setCurrentIndex((is3DParametric || is3DSurface) ? 1 : 0);

    // Adjust range label
    QString label = is2DExplicit ? "x Range" : "t Range";
    // (GroupBox title already says "Range & Samples")
}

void MainWindow::onPlotClicked() {
    int mode = m_modeCombo->currentIndex();
    bool ok = false;

    if (mode == MODE_2D_EXPLICIT)        ok = generateCurve2DExplicit();
    else if (mode == MODE_2D_IMPLICIT)   ok = generateCurve2DImplicit();
    else if (mode == MODE_2D_PARAMETRIC) ok = generateCurve2DParametric();
    else if (mode == MODE_3D_PARAMETRIC) ok = generateCurve3DParametric();
    else if (mode == MODE_3D_SURFACE)    ok = generateSurface3D();

    if (ok) statusBar()->showMessage("Curve plotted successfully.");
}

void MainWindow::onClearClicked() {
    m_plot2D->clearCurves();
    m_plot3D->clearCurves();
    m_logView->clear();
    statusBar()->showMessage("Canvas cleared.");
}

void MainWindow::onResetViewClicked() {
    m_plot2D->resetView();
    m_plot3D->resetView();
    statusBar()->showMessage("View reset.");
}

void MainWindow::onExampleSelected(int index) {
    if (index < 0 || index >= g_examples.size()) return;
    const Example &ex = g_examples[index];
    if (ex.mode == MODE_SEPARATOR) return;

    m_modeCombo->setCurrentIndex(ex.mode);
    onModeChanged(ex.mode);

    m_tMinSpin->setValue(ex.tMin);
    m_tMaxSpin->setValue(ex.tMax);
    m_samplesSpin->setValue(ex.samples);
    m_currentColor = ex.color;
    updateColorButton();

    if (ex.mode == MODE_2D_EXPLICIT) {
        m_yExpr->setText(ex.y);
    } else if (ex.mode == MODE_2D_IMPLICIT) {
        m_curveExpr2D->setText(ex.y);
    } else if (ex.mode == MODE_2D_PARAMETRIC) {
        m_xExpr2D->setText(ex.x);
        m_yExpr2D->setText(ex.yp);
    } else if (ex.mode == MODE_3D_PARAMETRIC) {
        m_xExpr3D->setText(ex.xp);
        m_yExpr3D->setText(ex.yp3);
        m_zExpr3D->setText(ex.zp3);
    } else if (ex.mode == MODE_3D_SURFACE) {
        m_surfaceExpr3D->setText(ex.y);
    }

    onPlotClicked();
}

void MainWindow::onColorButtonClicked() {
    QColor c = QColorDialog::getColor(m_currentColor, this, "Choose curve color");
    if (c.isValid()) {
        m_currentColor = c;
        updateColorButton();
    }
}

// ─────────────────────────────────────────────────────────────
//  Curve generation
// ─────────────────────────────────────────────────────────────

bool MainWindow::generateCurve2DExplicit() {
    QString exprStr = m_yExpr->text().trimmed();
    if (exprStr.isEmpty()) {
        logMessage("Error: y-expression is empty.", true);
        return false;
    }

    // Strip leading "y=" or "f(x)=" etc.
    static QRegularExpression leadingEq("^[yY]\\s*=\\s*|^[fF]\\([xX]\\)\\s*=\\s*");
    exprStr.remove(leadingEq);

    ExprParser parser;
    std::string errorMsg;
    if (!parser.compile(exprStr.toStdString(), errorMsg)) {
        logMessage("Parse error: " + QString::fromStdString(errorMsg), true);
        return false;
    }

    double xMin    = m_tMinSpin->value();
    double xMax    = m_tMaxSpin->value();
    int    samples = m_samplesSpin->value();

    if (xMin >= xMax) {
        logMessage("Error: Min must be less than Max.", true);
        return false;
    }

    Curve2D curve;
    curve.style.color     = m_currentColor;
    curve.style.lineWidth = (float)m_lineWidthSpin->value();
    curve.label           = "y=" + exprStr;

    double step = (xMax - xMin) / (samples - 1);
    int    invalidCount = 0;

    for (int i = 0; i < samples; ++i) {
        double x = xMin + i * step;
        parser.setVar("x", x);
        bool valid = true;
        double y = parser.evaluate(valid);
        Point2D pt;
        pt.x = x;
        pt.y = y;
        pt.valid = valid && std::isfinite(y);
        if (!pt.valid) ++invalidCount;
        curve.points.push_back(pt);
    }

    m_plot2D->addCurve(curve);

    if (invalidCount > 0)
        logMessage(QString("Note: %1 point(s) skipped (NaN/Inf/domain error).").arg(invalidCount));
    else
        logMessage("Plotted: " + curve.label);

    return true;
}

bool MainWindow::generateCurve2DImplicit() {
    QString exprStr = m_curveExpr2D->text().trimmed();
    if (exprStr.isEmpty()) {
        logMessage("Error: curve equation is empty.", true);
        return false;
    }

    int eqPos = exprStr.indexOf('=');
    if (eqPos >= 0) {
        QString lhs = exprStr.left(eqPos).trimmed();
        QString rhs = exprStr.mid(eqPos + 1).trimmed();
        if (lhs.isEmpty() || rhs.isEmpty()) {
            logMessage("Error: invalid equation format. Use F(x,y)=G(x,y).", true);
            return false;
        }
        exprStr = "(" + lhs + ")-(" + rhs + ")";
    }

    ExprParser parser;
    std::string err;
    if (!parser.compile(exprStr.toStdString(), err)) {
        logMessage("Parse error in implicit equation: " + QString::fromStdString(err), true);
        return false;
    }

    double xMin = m_tMinSpin->value();
    double xMax = m_tMaxSpin->value();
    int requestedGridSize = m_samplesSpin->value();
    if (xMin >= xMax) {
        logMessage("Error: Min must be less than Max.", true);
        return false;
    }

    static constexpr int MIN_GRID_SIZE = 20;
    static constexpr int MAX_GRID_SIZE = 800;
    const int grid = std::max(MIN_GRID_SIZE, std::min(requestedGridSize, MAX_GRID_SIZE));
    const double yMin = xMin;
    const double yMax = xMax;
    const double dx = (xMax - xMin) / (grid - 1);
    const double dy = (yMax - yMin) / (grid - 1);

    struct CellSample { double value{0}; bool valid{false}; };
    std::vector<CellSample> field((size_t)grid * (size_t)grid);
    auto at = [&](int ix, int iy) -> CellSample& {
        return field[(size_t)iy * (size_t)grid + (size_t)ix];
    };

    int invalidCount = 0;
    for (int iy = 0; iy < grid; ++iy) {
        double y = yMin + iy * dy;
        for (int ix = 0; ix < grid; ++ix) {
            double x = xMin + ix * dx;
            parser.setVar("x", x);
            parser.setVar("y", y);
            bool valid = true;
            double f = parser.evaluate(valid);
            CellSample &s = at(ix, iy);
            s.value = f;
            s.valid = valid && std::isfinite(f);
            if (!s.valid) ++invalidCount;
        }
    }

    Curve2D curve;
    curve.style.color     = m_currentColor;
    curve.style.lineWidth = (float)m_lineWidthSpin->value();
    curve.label           = "F(x,y)=0";

    auto edgeCrosses = [](double v1, double v2) {
        if (!std::isfinite(v1) || !std::isfinite(v2)) return false;
        return (v1 == 0.0) || (v2 == 0.0) || ((v1 < 0.0) != (v2 < 0.0));
    };

    int hitCount = 0;
    for (int iy = 0; iy < grid - 1; ++iy) {
        for (int ix = 0; ix < grid - 1; ++ix) {
            const CellSample &a = at(ix, iy);
            const CellSample &b = at(ix + 1, iy);
            const CellSample &c = at(ix, iy + 1);
            const CellSample &d = at(ix + 1, iy + 1);
            if (!a.valid || !b.valid || !c.valid || !d.valid) continue;

            const double minV = std::min(std::min(a.value, b.value), std::min(c.value, d.value));
            const double maxV = std::max(std::max(a.value, b.value), std::max(c.value, d.value));
            if (minV > 0.0 || maxV < 0.0) continue;

            const double x0 = xMin + ix * dx;
            const double x1 = x0 + dx;
            const double y0 = yMin + iy * dy;
            const double y1 = y0 + dy;
            std::vector<Point2D> intersections;
            intersections.reserve(4);

            auto addIntersection = [&](double px1, double py1, double pv1,
                                       double px2, double py2, double pv2) {
                if (!edgeCrosses(pv1, pv2)) return;
                double denom = pv1 - pv2;
                double t = 0.5;
                if (std::abs(denom) > 1e-12) t = pv1 / denom;
                t = std::clamp(t, 0.0, 1.0);
                intersections.push_back(Point2D{
                    px1 + t * (px2 - px1),
                    py1 + t * (py2 - py1),
                    true
                });
            };

            auto appendSegment = [&](const Point2D &p0, const Point2D &p1) {
                curve.points.push_back(p0);
                curve.points.push_back(p1);
                curve.points.push_back(Point2D{0.0, 0.0, false});
                ++hitCount;
            };

            addIntersection(x0, y0, a.value, x1, y0, b.value); // top
            addIntersection(x1, y0, b.value, x1, y1, d.value); // right
            addIntersection(x0, y1, c.value, x1, y1, d.value); // bottom
            addIntersection(x0, y0, a.value, x0, y1, c.value); // left

            if (intersections.size() < 2) continue;
            if (intersections.size() == 2) {
                appendSegment(intersections[0], intersections[1]);
            } else {
                appendSegment(intersections[0], intersections[1]);
                appendSegment(intersections[2], intersections[3]);
            }
        }
    }

    if (hitCount == 0) {
        logMessage("Error: no curve points found in current range.", true);
        return false;
    }

    m_plot2D->addCurve(curve);
    if (invalidCount > 0)
        logMessage(QString("Plotted implicit curve (%1 samples, %2 invalid evaluations).").arg(hitCount).arg(invalidCount));
    else
        logMessage(QString("Plotted implicit curve (%1 samples).").arg(hitCount));
    return true;
}

bool MainWindow::generateCurve2DParametric() {
    QString xStr = m_xExpr2D->text().trimmed();
    QString yStr = m_yExpr2D->text().trimmed();
    xStr.remove(RX_X_OF_T);
    yStr.remove(RX_Y_OF_T);

    if (xStr.isEmpty() || yStr.isEmpty()) {
        logMessage("Error: both x(t) and y(t) expressions are required.", true);
        return false;
    }

    ExprParser xParser, yParser;
    std::string err;
    if (!xParser.compile(xStr.toStdString(), err)) {
        logMessage("Parse error in x(t): " + QString::fromStdString(err), true);
        return false;
    }
    if (!yParser.compile(yStr.toStdString(), err)) {
        logMessage("Parse error in y(t): " + QString::fromStdString(err), true);
        return false;
    }

    double tMin    = m_tMinSpin->value();
    double tMax    = m_tMaxSpin->value();
    int    samples = m_samplesSpin->value();

    if (tMin >= tMax) {
        logMessage("Error: Min must be less than Max.", true);
        return false;
    }

    Curve2D curve;
    curve.style.color     = m_currentColor;
    curve.style.lineWidth = (float)m_lineWidthSpin->value();
    curve.label           = QString("x=%1, y=%2").arg(xStr, yStr);

    double step = (tMax - tMin) / (samples - 1);
    int invalidCount = 0;

    for (int i = 0; i < samples; ++i) {
        double t = tMin + i * step;
        xParser.setVar("t", t);
        yParser.setVar("t", t);
        bool vx = true, vy = true;
        double x = xParser.evaluate(vx);
        double y = yParser.evaluate(vy);
        Point2D pt;
        pt.x = x; pt.y = y;
        pt.valid = vx && vy && std::isfinite(x) && std::isfinite(y);
        if (!pt.valid) ++invalidCount;
        curve.points.push_back(pt);
    }

    m_plot2D->addCurve(curve);

    if (invalidCount > 0)
        logMessage(QString("Note: %1 point(s) skipped.").arg(invalidCount));
    else
        logMessage("Plotted 2D parametric: " + curve.label);

    return true;
}

bool MainWindow::generateCurve3DParametric() {
    QString xStr = m_xExpr3D->text().trimmed();
    QString yStr = m_yExpr3D->text().trimmed();
    QString zStr = m_zExpr3D->text().trimmed();
    xStr.remove(RX_X_OF_T);
    yStr.remove(RX_Y_OF_T);
    zStr.remove(RX_Z_OF_T);

    if (xStr.isEmpty() || yStr.isEmpty() || zStr.isEmpty()) {
        logMessage("Error: x(t), y(t) and z(t) expressions are all required.", true);
        return false;
    }

    ExprParser xParser, yParser, zParser;
    std::string err;
    if (!xParser.compile(xStr.toStdString(), err)) {
        logMessage("Parse error in x(t): " + QString::fromStdString(err), true);
        return false;
    }
    if (!yParser.compile(yStr.toStdString(), err)) {
        logMessage("Parse error in y(t): " + QString::fromStdString(err), true);
        return false;
    }
    if (!zParser.compile(zStr.toStdString(), err)) {
        logMessage("Parse error in z(t): " + QString::fromStdString(err), true);
        return false;
    }

    double tMin    = m_tMinSpin->value();
    double tMax    = m_tMaxSpin->value();
    int    samples = m_samplesSpin->value();

    if (tMin >= tMax) {
        logMessage("Error: Min must be less than Max.", true);
        return false;
    }

    Curve3D curve;
    curve.style.color     = m_currentColor;
    curve.style.lineWidth = (float)m_lineWidthSpin->value();
    curve.label           = QString("x=%1,y=%2,z=%3").arg(xStr, yStr, zStr);

    double step = (tMax - tMin) / (samples - 1);
    int invalidCount = 0;

    for (int i = 0; i < samples; ++i) {
        double t = tMin + i * step;
        xParser.setVar("t", t);
        yParser.setVar("t", t);
        zParser.setVar("t", t);
        bool vx = true, vy = true, vz = true;
        double x = xParser.evaluate(vx);
        double y = yParser.evaluate(vy);
        double z = zParser.evaluate(vz);
        Point3D pt;
        pt.x = x; pt.y = y; pt.z = z;
        pt.valid = vx && vy && vz &&
                   std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
        if (!pt.valid) ++invalidCount;
        curve.points.push_back(pt);
    }

    m_plot3D->addCurve(curve);

    if (invalidCount > 0)
        logMessage(QString("Note: %1 point(s) skipped.").arg(invalidCount));
    else
        logMessage("Plotted 3D parametric: " + curve.label);

    return true;
}

bool MainWindow::generateSurface3D() {
    static constexpr float SURFACE_COLUMN_WIDTH_SCALE = 0.8f;
    QString zStr = m_surfaceExpr3D->text().trimmed();
    static const QRegularExpression RX_Z_OF_XY("^z\\s*(\\(\\s*[xX]\\s*,\\s*[yY]\\s*\\))?\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);
    zStr.remove(RX_Z_OF_XY);
    if (zStr.isEmpty()) {
        logMessage("Error: z(x,y) expression is required.", true);
        return false;
    }

    ExprParser zParser;
    std::string err;
    if (!zParser.compile(zStr.toStdString(), err)) {
        logMessage("Parse error in z(x,y): " + QString::fromStdString(err), true);
        return false;
    }

    double xMin = m_tMinSpin->value();
    double xMax = m_tMaxSpin->value();
    double yMin = xMin;
    double yMax = xMax;
    if (xMin >= xMax) {
        logMessage("Error: Min must be less than Max.", true);
        return false;
    }

    const int requestedSamples = m_samplesSpin->value();
    const int grid = std::max(8, std::min(requestedSamples, 220));
    const double xStep = (xMax - xMin) / (grid - 1);
    const double yStep = (yMax - yMin) / (grid - 1);

    std::vector<double> values((size_t)grid * (size_t)grid, 0.0);
    std::vector<bool> valid((size_t)grid * (size_t)grid, false);
    auto idx = [grid](int ix, int iy) { return (size_t)iy * (size_t)grid + (size_t)ix; };

    int invalidCount = 0;
    for (int iy = 0; iy < grid; ++iy) {
        const double y = yMin + iy * yStep;
        for (int ix = 0; ix < grid; ++ix) {
            const double x = xMin + ix * xStep;
            zParser.setVar("x", x);
            zParser.setVar("y", y);
            bool vz = true;
            const double z = zParser.evaluate(vz);
            const bool ok = vz && std::isfinite(z);
            values[idx(ix, iy)] = z;
            valid[idx(ix, iy)] = ok;
            if (!ok) ++invalidCount;
        }
    }

    bool addedAny = false;
    for (int iy = 0; iy < grid; ++iy) {
        Curve3D row;
        row.style.color = m_currentColor;
        row.style.lineWidth = (float)m_lineWidthSpin->value();
        if (iy == 0) row.label = "z=" + zStr;

        for (int ix = 0; ix < grid; ++ix) {
            const size_t id = idx(ix, iy);
            const double x = xMin + ix * xStep;
            const double y = yMin + iy * yStep;
            row.points.push_back(Point3D{x, y, values[id], valid[id]});
            if (valid[id]) addedAny = true;
        }
        m_plot3D->addCurve(row);
    }

    for (int ix = 0; ix < grid; ++ix) {
        Curve3D col;
        col.style.color = m_currentColor;
        col.style.lineWidth = std::max(0.5f, (float)m_lineWidthSpin->value() * SURFACE_COLUMN_WIDTH_SCALE);

        for (int iy = 0; iy < grid; ++iy) {
            const size_t id = idx(ix, iy);
            const double x = xMin + ix * xStep;
            const double y = yMin + iy * yStep;
            col.points.push_back(Point3D{x, y, values[id], valid[id]});
        }
        m_plot3D->addCurve(col);
    }

    if (!addedAny) {
        logMessage("Error: no valid surface points in current range.", true);
        return false;
    }

    if (invalidCount > 0)
        logMessage(QString("Plotted 3D surface z=%1 (%2x%2 grid, %3 invalid points).").arg(zStr).arg(grid).arg(invalidCount));
    else
        logMessage(QString("Plotted 3D surface z=%1 (%2x%2 grid).").arg(zStr).arg(grid));

    return true;
}

// ─────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────

void MainWindow::logMessage(const QString &msg, bool isError) {
    QString html = isError
        ? "<span style='color:red;'>&#9888; " + msg.toHtmlEscaped() + "</span>"
        : "<span style='color:#333;'>" + msg.toHtmlEscaped() + "</span>";
    m_logView->append(html);
    if (isError) statusBar()->showMessage("Error: " + msg);
}

void MainWindow::updateColorButton() {
    QString style = QString("background:%1; border:1px solid #aaa; border-radius:3px;")
                    .arg(m_currentColor.name());
    m_colorButton->setStyleSheet(style);
    m_colorButton->setText(m_currentColor.name());
}
