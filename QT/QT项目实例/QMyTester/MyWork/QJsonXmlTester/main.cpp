#include "QJsonXmlTester.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QJsonXmlTester w;
    w.show();
    return a.exec();
}
