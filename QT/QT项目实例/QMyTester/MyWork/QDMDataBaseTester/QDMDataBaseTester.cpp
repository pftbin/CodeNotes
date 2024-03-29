#include "QDMDataBaseTester.h"
#include "ui_QDMDataBaseTester.h"

#include "public.h"

QDMDataBaseTester::QDMDataBaseTester(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QDMDataBaseTester)
{
    ui->setupUi(this);

    //设定IP输入格式
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"), this);
    ui->m_editIP->setValidator(validator);

    InitDBParmeter();
}

QDMDataBaseTester::~QDMDataBaseTester()
{
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

void QDMDataBaseTester::SaveDBParmeter()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString strIniFilePath = tr("%1/Config.ini").arg(currentDir);
    QSettings settings(strIniFilePath, QSettings::IniFormat);

    settings.setValue("DB/IP", ui->m_editIP->text());
    settings.setValue("DB/Port", ui->m_editPort->text());
    settings.setValue("DB/DBName",  ui->m_editDBName->text());
    settings.setValue("DB/UserName", ui->m_editUserName->text());
    settings.setValue("DB/PassWord", ui->m_editPassWord->text());
    settings.sync(); // 确保立即保存到文件
}


/////////////////////////////////////////////////////////////////////
void QDMDataBaseTester::on_m_btnTestSelect_clicked()
{
    ui->m_editSql->setText("select * from SYSDBA.SBT_TESTBY");
}


void QDMDataBaseTester::on_m_btnTestExecute_clicked()
{
    ui->m_editSql->setText("update SYSDBA.SBT_TESTBY set AGE=15 where ID=1");
}


void QDMDataBaseTester::on_m_btnRunSql_clicked()
{
    SaveDBParmeter();

    QString ip = ui->m_editIP->text();
    QString port = ui->m_editPort->text();
    QString dbname = ui->m_editDBName->text();
    QString username = ui->m_editUserName->text();
    QString password = ui->m_editPassWord->text();

    ui->m_listInfo->delAllRowCol();
    QDBObject dbObject(ip,port,dbname,username,password);
    if (dbObject.openDatabase())
    {
        QString strSQL = ui->m_editSql->text();
        bool bSelect = (strSQL.startsWith(QString("select")))?(true):(false);

        if (bSelect && dbObject.querySql(strSQL, m_queryObject))
        {
            QSqlRecord rec = m_queryObject.record(); // 获取查询结果的元数据
            int columnCount = rec.count(); // 获取列数
            for (int i = 0; i < columnCount; i++)
            {
                QString columnName = rec.fieldName(i); // 获取列名
                ui->m_listInfo->addCol(columnName);

                //qDebug() << "Column Name: " << columnName;
            }

            int nRowIdx = 0;
            while (m_queryObject.next())
            {
                ui->m_listInfo->addRow();
                for (int nColIdx = 0; nColIdx < columnCount; nColIdx++)
                {
                    QVariant value = m_queryObject.value(nColIdx); // 获取每列的值
                    QString strText = ConvertToQString(value);

                    ui->m_listInfo->setItemText(nRowIdx,nColIdx,strText);
                    //qDebug() << "Row " << m_queryObject.at() << ", Column " << nColIdx << ": " << value.toString();
                }
                nRowIdx++;
            }
        }
        else if (!bSelect && dbObject.executeSql(strSQL))
        {
            QMessageBox::information(this,tr("Info"), tr("execute sql success"));
        }
        else
        {
            QMessageBox::warning(this,tr("Error"), tr("query/execute failed..."));
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Error"), tr("database open failed..."));
    }
}


void QDMDataBaseTester::on_m_btnCommit_clicked()
{

}

