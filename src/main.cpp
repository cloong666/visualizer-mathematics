#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Curve Visualizer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("visualizer-mathematics");

    // Use Fusion style for a clean cross-platform look
    app.setStyle(QStyleFactory::create("Fusion"));

    // Light palette
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 243, 248));
    palette.setColor(QPalette::WindowText, QColor(30, 35, 50));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(235, 238, 244));
    palette.setColor(QPalette::Highlight, QColor(41, 98, 255));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(palette);

    MainWindow w;
    w.show();
    return app.exec();
}
