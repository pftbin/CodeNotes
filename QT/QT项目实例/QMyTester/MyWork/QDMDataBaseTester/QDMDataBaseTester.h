#ifndef QDMDATABASETESTER_H
#define QDMDATABASETESTER_H

#include <QMainWindow>
#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "QDBObject.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QDMDataBaseTester; }
QT_END_NAMESPACE

class QDMDataBaseTester : public QMainWindow
{
    Q_OBJECT

public:
    QDMDataBaseTester(QWidget *parent = nullptr);
    ~QDMDataBaseTester();

protected:
    void InitDBParmeter();
    void SaveDBParmeter();

    QSqlQuery   m_queryObject;

private slots:
    void on_m_btnTestSelect_clicked();
    void on_m_btnTestExecute_clicked();
    void on_m_btnRunSql_clicked();
    void on_m_btnCommit_clicked();

private:
    Ui::QDMDataBaseTester *ui;
};
#endif // QDMDATABASETESTER_H
