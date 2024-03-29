#include "QDMDataBaseTester.h"

#include <QApplication>
#include <QProcessEnvironment>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDMDataBaseTester w;
    w.show();
    return a.exec();
}
