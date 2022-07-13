#include "H2Analyst.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/app-icon"));
    H2Analyst w;
    w.resize(1080, 720);
    w.show();
    return a.exec();
}
