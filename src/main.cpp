#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    // Application entry point
    QApplication app(argc, argv);

    MainWindow window;
    window.show();


    return app.exec();
}