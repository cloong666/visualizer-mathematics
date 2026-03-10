#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QColorDialog>
#include "CurveData.h"
#include "ExprParser.h"

class Plot2DWidget;
class Plot3DWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onModeChanged(int index);
    void onPlotClicked();
    void onClearClicked();
    void onResetViewClicked();
    void onExampleSelected(int index);
    void onColorButtonClicked();

private:
    // ── Build UI ──
    QWidget *buildControlPanel();
    QGroupBox *buildEquationGroup();
    QGroupBox *buildRangeGroup();
    QGroupBox *buildStyleGroup();
    QGroupBox *buildExamplesGroup();

    void buildEquationFields();      // called when mode changes
    void updateEquationFieldVisibility();

    // ── Curve generation ──
    bool generateCurve2DExplicit();
    bool generateCurve2DParametric();
    bool generateCurve3DParametric();

    void logMessage(const QString &msg, bool isError = false);

    // ── Color picker ──
    void updateColorButton();

    // ── Widgets ──
    QComboBox      *m_modeCombo{nullptr};

    // Equation inputs (shown/hidden per mode)
    QWidget        *m_eq2DExplicitWidget{nullptr};
    QWidget        *m_eq2DParamWidget{nullptr};
    QWidget        *m_eq3DParamWidget{nullptr};

    QLineEdit      *m_yExpr{nullptr};        // 2D Explicit: y=f(x)
    QLineEdit      *m_xExpr2D{nullptr};      // 2D Param: x(t)
    QLineEdit      *m_yExpr2D{nullptr};      //           y(t)
    QLineEdit      *m_xExpr3D{nullptr};      // 3D Param: x(t)
    QLineEdit      *m_yExpr3D{nullptr};      //           y(t)
    QLineEdit      *m_zExpr3D{nullptr};      //           z(t)

    // Range / samples
    QDoubleSpinBox *m_tMinSpin{nullptr};
    QDoubleSpinBox *m_tMaxSpin{nullptr};
    QSpinBox       *m_samplesSpin{nullptr};

    // Style
    QPushButton    *m_colorButton{nullptr};
    QDoubleSpinBox *m_lineWidthSpin{nullptr};
    QColor          m_currentColor{Qt::blue};

    // Canvas stack
    QStackedWidget *m_canvasStack{nullptr};
    Plot2DWidget   *m_plot2D{nullptr};
    Plot3DWidget   *m_plot3D{nullptr};

    // Log
    QTextEdit      *m_logView{nullptr};

    // Examples combo
    QComboBox      *m_examplesCombo{nullptr};
};
