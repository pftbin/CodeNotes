#ifndef QDMDATABASETESTER_H
#define QDMDATABASETESTER_H

#include <QMainWindow>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "QDBC.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QDMDataBaseTester; }
QT_END_NAMESPACE

class QDMDataBaseTester : public QMainWindow
{
    Q_OBJECT

public:
    QDMDataBaseTester(QWidget *parent = nullptr);
    ~QDMDataBaseTester();

    void doInsert();
    void doDelete();
    void doUpdate();


protected:
    void InitDBParmeter();
    void SaveDBParmeter(bool bToFile = false);

    QString     m_ip;
    QString     m_port;
    QString     m_dbname;
    QString     m_username;
    QString     m_password;
    IDBC*       m_pDBObject;

private slots:
    void on_m_btnTestSelect_clicked();
    void on_m_btnTestCommit_clicked();

    void EnableThreadButton(bool bEnable);
    void on_m_btnInsertThread_clicked();
    void on_m_btnDeleteThread_clicked();
    void on_m_btnUpdateThread_clicked();
    void on_m_btnAllThread_clicked();

private:
    Ui::QDMDataBaseTester *ui;
};

class InsertThread : public QThread
{
public:
    InsertThread(QDMDataBaseTester* pParent)
    {
        m_pParent = pParent;
    }

public:
    void run() override
    {
        if (m_pParent)
        {
            m_pParent->doInsert();
        }
    }
private:
    QDMDataBaseTester* m_pParent;
};

class DeleteThread : public QThread
{
public:
    DeleteThread(QDMDataBaseTester* pParent)
    {
        m_pParent = pParent;
    }

public:
    void run() override
    {
        if (m_pParent)
        {
            m_pParent->doDelete();
        }
    }
private:
    QDMDataBaseTester* m_pParent;
};

class UpdateThread : public QThread
{
public:
    UpdateThread(QDMDataBaseTester* pParent)
    {
        m_pParent = pParent;
    }

public:
    void run() override
    {
        if (m_pParent)
        {
            m_pParent->doUpdate();
        }
    }
private:
    QDMDataBaseTester* m_pParent;
};


#endif // QDMDATABASETESTER_H
