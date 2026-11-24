#include <QApplication>
#include "gui/MainWindow.h"
#include "gui/Theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(Theme::stylesheet());

    MainWindow window;
    window.show();

    return app.exec();
}
