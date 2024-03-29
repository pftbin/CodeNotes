#include "QDMDataBaseTester.h"
#include "ui_QDMDataBaseTester.h"

#include "public.h"

#include "QLog.h"
IMPLEMENT_LOGER(loger_QDMDBTester);

QDMDataBaseTester::QDMDataBaseTester(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QDMDataBaseTester)
{
    ui->setupUi(this);

    //设定IP输入格式
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"), this);
    ui->m_editIP->setValidator(validator);

    INIT_LOGER(loger_QDMDBTester, QString("DMDataBaseTester.log"),QString(""));
    InitDBParmeter();
    m_pDBObject = QDBC::GetDBClient();
}

QDMDataBaseTester::~QDMDataBaseTester()
{
    SaveDBParmeter(true);
    delete ui;
}

/////////////////////////////////////////////////////////////////////
void QDMDataBaseTester::InitDBParmeter()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString strIniFilePath = tr("%1/Config.ini").arg(currentDir);
    QSettings settings(strIniFilePath, QSettings::IniFormat);

    QString DB_IP = settings.value("DB/IP").toString();
    QString DB_Port = settings.value("DB/Port").toString();
    QString DB_DBName = settings.value("DB/DBName").toString();
    QString DB_UserName = settings.value("DB/UserName").toString();
    QString DB_PassWord = settings.value("DB/PassWord").toString();

    ui->m_editIP->setText(DB_IP);
    ui->m_editPort->setText(DB_Port);
    ui->m_editDBName->setText(DB_DBName);
    ui->m_editUserName->setText(DB_UserName);
    ui->m_editPassWord->setText(DB_PassWord);
}
void QDMDataBaseTester::SaveDBParmeter(bool bToFile)
{
    m_ip = ui->m_editIP->text();
    m_port = ui->m_editPort->text();
    m_dbname = ui->m_editDBName->text();
    m_username = ui->m_editUserName->text();
    m_password = ui->m_editPassWord->text();

    if (bToFile)
    {
        QString currentDir = QCoreApplication::applicationDirPath();
        QString strIniFilePath = tr("%1/Config.ini").arg(currentDir);
        QSettings settings(strIniFilePath, QSettings::IniFormat);

        settings.setValue("DB/IP", m_ip);
        settings.setValue("DB/Port", m_port);
        settings.setValue("DB/DBName",  m_dbname);
        settings.setValue("DB/UserName", m_username);
        settings.setValue("DB/PassWord", m_password);
        settings.sync(); // 确保立即保存到文件
    }
}

/////////////////////////////////////////////////////////////////////
void QDMDataBaseTester::on_m_btnTestSelect_clicked()
{
    SaveDBParmeter(true);
    ui->m_listInfo->delAllRowCol();
    ui->m_editSql->setText("select * from SYSDBA.SBT_TESTBY");

    if (m_pDBObject && m_pDBObject->InitDBServer(m_ip,m_port,m_dbname,m_username,m_password))
    {
        QVector<QString> vecColNameText;
        QVector<QVector<QString>> vecRowItemText;
        if(m_pDBObject->SelectAllItem(vecColNameText, vecRowItemText))
        {
            int nColCount = vecColNameText.size();
            for (int i = 0; i < nColCount; i++)
            {
                QString strColName = vecColNameText[i]; // 获取列名
                ui->m_listInfo->addCol(strColName);
            }

            int nRowCount = vecRowItemText.size();
            for (int nRowIdx = 0; nRowIdx < nRowCount; nRowIdx++)
            {
                ui->m_listInfo->addRow();
                for (int nColIdx = 0; nColIdx < nColCount; nColIdx++)
                {
                    QString strText = vecRowItemText[nRowIdx][nColIdx];
                    ui->m_listInfo->setItemEdit(nRowIdx,nColIdx,strText);
                }
            }
            QMessageBox::information(this,tr("Info"), tr("select all success"));
        }
        else
        {
            QMessageBox::warning(this,tr("Error"), tr("select sql failed..."));
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Error"), tr("InitDBServer Failed..."));
    }

}
void QDMDataBaseTester::on_m_btnTestCommit_clicked()
{
    SaveDBParmeter(true);

    int nRowCount = ui->m_listInfo->getrowCount();
    int nColCount = ui->m_listInfo->getColCount();
    QVector<QString> vecColNameText;
    for (int i = 0; i < nColCount; i++)
    {
        QString strColName = ui->m_listInfo->getColText(i); // 获取列名
        vecColNameText.push_back(strColName);
    }
    QVector<QVector<QString>> vecRowItemText;
    for (int nRowIdx = 0; nRowIdx < nRowCount; nRowIdx++)
    {
        QVector<QString> vecRowItem;
        for (int nColIdx = 0; nColIdx < nColCount; nColIdx++)
        {
            QString strItemValue = ui->m_listInfo->getItemText(nRowIdx,nColIdx);
            vecRowItem.push_back(strItemValue);
        }
        vecRowItemText.append(vecRowItem);
    }

    if (m_pDBObject && m_pDBObject->InitDBServer(m_ip,m_port,m_dbname,m_username,m_password))
    {
        if (m_pDBObject->UpdateAllItem(vecColNameText, vecRowItemText))
        {
            QMessageBox::information(this,tr("Info"), tr("commit all success"));
        }
        else
        {
            QMessageBox::warning(this,tr("Error"), tr("commit all failed..."));
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Error"), tr("InitDBServer Failed..."));
    }
}

void QDMDataBaseTester::doInsert()
{
    for (int i = 0; i < 5; i++)
    {
        m_pDBObject->InsertNewItem();
        QThread::msleep(100);
    }
}
void QDMDataBaseTester::doDelete()
{
    for (int i = 0; i < 5; i++)
    {
        m_pDBObject->DeleteItem();
        QThread::msleep(200);
    }
}
void QDMDataBaseTester::doUpdate()
{
    for (int i = 0; i < 5; i++)
    {
        m_pDBObject->UpdateItem();
        QThread::msleep(300);
    }
}

void QDMDataBaseTester::EnableThreadButton(bool bEnable)
{
    ui->m_btnInsertThread->setEnabled(bEnable);
    ui->m_btnDeleteThread->setEnabled(bEnable);
    ui->m_btnUpdateThread->setEnabled(bEnable);
    ui->m_btnAllThread->setEnabled(bEnable);
}
void QDMDataBaseTester::on_m_btnInsertThread_clicked()
{
    SaveDBParmeter();

    EnableThreadButton(false);
    QThread* thread1 = new InsertThread(this);
    thread1->start();
    thread1->wait();
    EnableThreadButton(true);
    QMessageBox::information(this,tr("Info"), tr("Insert thread over..."));
}
void QDMDataBaseTester::on_m_btnDeleteThread_clicked()
{
    SaveDBParmeter();

    EnableThreadButton(false);
    QThread* thread2 = new DeleteThread(this);
    thread2->start();
    thread2->wait();
    EnableThreadButton(true);
    QMessageBox::information(this,tr("Info"), tr("Delete thread over..."));
}
void QDMDataBaseTester::on_m_btnUpdateThread_clicked()
{
    SaveDBParmeter();

    EnableThreadButton(false);
    QThread* thread3 = new UpdateThread(this);
    thread3->start();
    thread3->wait();
    EnableThreadButton(true);
    QMessageBox::information(this,tr("Info"), tr("Update thread over..."));
}
void QDMDataBaseTester::on_m_btnAllThread_clicked()
{
    SaveDBParmeter();
    if (m_pDBObject && m_pDBObject->InitDBServer(m_ip,m_port,m_dbname,m_username,m_password))
    {
        EnableThreadButton(false);
        QThread* thread1 = new InsertThread(this);
        QThread* thread2 = new DeleteThread(this);
        QThread* thread3 = new UpdateThread(this);

        thread1->start();
        thread2->start();
        thread3->start();

        thread1->wait();
        thread2->wait();
        thread3->wait();
        EnableThreadButton(true);
        QMessageBox::information(this,tr("Info"), tr("AllThread over..."));
    }
    else
    {
        QMessageBox::warning(this,tr("Error"), tr("AllThread InitDBServer Failed..."));
    }
}

