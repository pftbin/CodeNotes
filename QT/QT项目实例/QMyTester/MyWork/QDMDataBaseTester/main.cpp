#include "QDMDataBaseTester.h"

#include <QApplication>
#include <QProcessEnvironment>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList keys = env.keys();

    foreach (const QString &key, keys) {
        qDebug() << key << "=" << env.value(key);
    }


    QDMDataBaseTester w;
    w.show();
    return a.exec();
}
