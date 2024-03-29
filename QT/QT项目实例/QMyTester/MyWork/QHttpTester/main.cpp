#include "QHttpTester.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QHttpTester w;

    w.setFixedSize(921,662);//固定宽高
    w.show();
    return a.exec();
}
