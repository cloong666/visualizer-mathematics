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
#include <cmath>

// ─────────────────────────────────────────────────────────────
//  Mode indices
// ─────────────────────────────────────────────────────────────
static constexpr int MODE_2D_EXPLICIT   = 0;
static constexpr int MODE_2D_PARAMETRIC = 1;
static constexpr int MODE_3D_PARAMETRIC = 2;

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
    Example{ "──── 2D Explicit ────", -1, {},{},{},{},{},{}, -10, 10, 500, Qt::blue },
    Example{ "Sine wave  y=sin(x)",       MODE_2D_EXPLICIT, "sin(x)", {},{},{},{},{}, -6.28, 6.28, 400, QColor(0,120,200) },
    Example{ "Parabola  y=x^2",           MODE_2D_EXPLICIT, "x^2",    {},{},{},{},{}, -5,    5,    300, QColor(200,80,0) },
    Example{ "Cubic  y=x^3-3*x",          MODE_2D_EXPLICIT, "x^3-3*x",{},{},{},{},{}, -3,    3,    300, QColor(160,0,200) },
    Example{ "Gaussian  y=exp(-x^2)",     MODE_2D_EXPLICIT, "exp(-x^2)",{},{},{},{},{}, -4,   4,    400, QColor(0,180,100) },
    Example{ "Damped sine  y=sin(x)/x",   MODE_2D_EXPLICIT, "sin(x)/x", {},{},{},{},{}, -20,  20,   600, QColor(200,0,100) },
    Example{ "Abs cosine  y=abs(cos(x))",  MODE_2D_EXPLICIT, "abs(cos(x))",{},{},{},{},{}, -6.28, 6.28, 400, QColor(0,160,180) },
    Example{ "──── 2D Parametric ────", -1, {},{},{},{},{},{}, 0, 6.28, 500, Qt::blue },
    Example{ "Circle  x=cos(t), y=sin(t)",          MODE_2D_PARAMETRIC, {}, "cos(t)", "sin(t)", {},{}, {}, 0, 6.28, 360, QColor(0,100,220) },
    Example{ "Ellipse  x=2cos(t), y=sin(t)",        MODE_2D_PARAMETRIC, {}, "2*cos(t)", "sin(t)", {},{}, {}, 0, 6.28, 360, QColor(200,60,0) },
    Example{ "Lissajous  x=sin(3t), y=sin(2t)",     MODE_2D_PARAMETRIC, {}, "sin(3*t)", "sin(2*t)", {},{}, {}, 0, 6.28, 600, QColor(160,0,200) },
    Example{ "Archimedean spiral  x=t*cos(t),y=t*sin(t)", MODE_2D_PARAMETRIC, {}, "t*cos(t)", "t*sin(t)", {},{}, {}, 0, 25.13, 800, QColor(0,160,80) },
    Example{ "Rose  x=cos(4t)cos(t), y=cos(4t)sin(t)",    MODE_2D_PARAMETRIC, {}, "cos(4*t)*cos(t)", "cos(4*t)*sin(t)", {},{}, {}, 0, 6.28, 800, QColor(220,30,80) },
    Example{ "──── 3D Parametric ────", -1, {},{},{},{},{},{}, 0, 30, 600, Qt::blue },
    Example{ "Helix  x=cos(t), y=sin(t), z=t/5",           MODE_3D_PARAMETRIC, {},{},{}, "cos(t)", "sin(t)", "t/5",    0, 30, 600, QColor(0,120,220) },
    Example{ "Conical helix  x=t*cos(t),y=t*sin(t),z=t",   MODE_3D_PARAMETRIC, {},{},{}, "t*cos(t)", "t*sin(t)", "t",  0, 12.56, 600, QColor(200,80,0) },
    Example{ "Trefoil knot",                                MODE_3D_PARAMETRIC, {},{},{},
      "sin(t)+2*sin(2*t)", "cos(t)-2*cos(2*t)", "-sin(3*t)", 0, 6.28, 600, QColor(160,0,200) },
    Example{ "Viviani curve",                               MODE_3D_PARAMETRIC, {},{},{},
      "1-cos(t)", "sin(t)", "2*sin(t/2)",  0, 12.56, 600, QColor(0,180,100) },
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
    m_modeCombo->addItem("2D Parametric  ( x(t), y(t) )");
    m_modeCombo->addItem("3D Parametric  ( x(t), y(t), z(t) )");
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

    layout->addWidget(m_eq2DExplicitWidget);
    layout->addWidget(m_eq2DParamWidget);
    layout->addWidget(m_eq3DParamWidget);

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
    bool is2DParametric = (index == MODE_2D_PARAMETRIC);
    bool is3DParametric = (index == MODE_3D_PARAMETRIC);

    if (m_eq2DExplicitWidget)   m_eq2DExplicitWidget->setVisible(is2DExplicit);
    if (m_eq2DParamWidget)      m_eq2DParamWidget->setVisible(is2DParametric);
    if (m_eq3DParamWidget)      m_eq3DParamWidget->setVisible(is3DParametric);

    // Switch canvas
    m_canvasStack->setCurrentIndex(is3DParametric ? 1 : 0);

    // Adjust range label
    QString label = is2DExplicit ? "x Range" : "t Range";
    // (GroupBox title already says "Range & Samples")
}

void MainWindow::onPlotClicked() {
    int mode = m_modeCombo->currentIndex();
    bool ok = false;

    if (mode == MODE_2D_EXPLICIT)        ok = generateCurve2DExplicit();
    else if (mode == MODE_2D_PARAMETRIC) ok = generateCurve2DParametric();
    else if (mode == MODE_3D_PARAMETRIC) ok = generateCurve3DParametric();

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
    if (ex.mode < 0) return; // separator

    m_modeCombo->setCurrentIndex(ex.mode);
    onModeChanged(ex.mode);

    m_tMinSpin->setValue(ex.tMin);
    m_tMaxSpin->setValue(ex.tMax);
    m_samplesSpin->setValue(ex.samples);
    m_currentColor = ex.color;
    updateColorButton();

    if (ex.mode == MODE_2D_EXPLICIT) {
        m_yExpr->setText(ex.y);
    } else if (ex.mode == MODE_2D_PARAMETRIC) {
        m_xExpr2D->setText(ex.x);
        m_yExpr2D->setText(ex.yp);
    } else if (ex.mode == MODE_3D_PARAMETRIC) {
        m_xExpr3D->setText(ex.xp);
        m_yExpr3D->setText(ex.yp3);
        m_zExpr3D->setText(ex.zp3);
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

bool MainWindow::generateCurve2DParametric() {
    QString xStr = m_xExpr2D->text().trimmed();
    QString yStr = m_yExpr2D->text().trimmed();

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
