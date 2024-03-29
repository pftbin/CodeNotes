#include "QHttpTester.h"
#include "ui_QHttpTester.h"
#include "public.h"

#include <QSpacerItem>
#include <QSizePolicy>
#include <QDateTime>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>

QHttpTester::QHttpTester(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitCtrl();
    m_strHttpType = "GET";
}

QHttpTester::~QHttpTester()
{
    delete ui;
}

void QHttpTester::InitCtrl()
{
    ui->m_cmbHttpType->addItem(QString("GET"));
    ui->m_cmbHttpType->addItem(QString("POST"));

    ui->m_tabPostHeader->addCol(QString("HeaderName"),150);
    ui->m_tabPostHeader->addCol(QString("HeaderValue"),250);
    for(int i = 0; i < 50; i++)//支持50个Header
    {
        ui->m_tabPostHeader->addRow();
        for(int j = 0; j < ui->m_tabPostHeader->getColCount(); j++)
        {
            ui->m_tabPostHeader->setItemEdit(i,j);
        }
    }

    //两个状态标签
    m_statebar_message = new QLabel("", this);
    m_statebar_time = new QLabel("", this);

    // 设置第一个标签控件填充左侧的宽度
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_statebar_message->setSizePolicy(sizePolicy1);
    m_statebar_message->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_statebar_message->setWordWrap(false);
    m_statebar_message->setStyleSheet("QLabel { qproperty-ElideMode: Qt::ElideRight; }");
    ui->statusbar->addWidget(m_statebar_message);

    // 设置第二个标签控件宽度固定
    QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setWidthForHeight(m_statebar_time->sizePolicy().hasHeightForWidth());
    m_statebar_time->setSizePolicy(sizePolicy2);
    m_statebar_time->setFixedWidth(120);
    ui->statusbar->addPermanentWidget(m_statebar_time);

    //Timer更新时间
    m_timerUpdateUI = new QTimer(this);
    QObject::connect(m_timerUpdateUI, &QTimer::timeout, this, &QHttpTester::on_timerUpdateUI);
    m_timerUpdateUI->start(50);
}

void QHttpTester::on_timerUpdateUI()
{
    // 获取当前时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    m_statebar_time->setText(currentDateTimeString);
}

void QHttpTester::on_m_cmbHttpType_activated(int index)
{
    if (index == 0)
    {
        m_strHttpType = "GET";
    }

    if (index == 1)
    {
        m_strHttpType = "POST";
    }
}

void QHttpTester::on_m_btnRequest_clicked()
{
    m_strHttpUrl = ui->m_editURL->toPlainText();
    if (m_strHttpUrl.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "URL为空...");
        message.exec();
        return;
    }

    QMap<QString, QString> inputHeaderMap;
    for(int i = 0; i < ui->m_tabPostHeader->getrowCount(); i++)
    {
        if(ui->m_tabPostHeader->getColCount() < 2) continue;
        for(int j = 0; j < 2; j++)
        {
            QString strKey = ui->m_tabPostHeader->getItemText(i,0);
            QString strValue = ui->m_tabPostHeader->getItemText(i,1);
            if(strKey.isEmpty() || strValue.isEmpty()) continue;

            inputHeaderMap.insert(strKey,strValue);
        }
    }
    QString inputBody = ui->m_editPostBody->toPlainText();

    //
    bool bResult = true;
    qint64 dwStart = getTickCount();
    QString strResponseHeader,strResponseBody;
    if (m_strHttpType == "GET")
    {
        //测试
        //m_strHttpUrl = "http://172.16.152.69:30069/v1/kernel/configs/role/rolename/admin/users";
        //inputHeaderMap.insert("sobeyhive-http-token","eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhdWQiOlsiaGl2ZSIsInNvYmV5Il0sInVzZXJfaW5mbyI6eyJzaXRlIjoiUzEiLCJ1c2VySWQiOiIxIiwibG9naW5OYW1lIjoiYWRtaW4iLCJ1c2VybmFtZSI6ImFkbWluIiwidXNlckNvZGUiOiJhZG1pbiIsInVzZXJUeXBlIjoxLCJuaWNrTmFtZSI6ImFkbWluMTEiLCJhdmF0YXIiOiJodHRwczovL25hcy5zb2JleS1jbG91ZDY5LmNvbToyMTA2MC9idWNrZXQtcC9hdmF0YXIvMjAyMy8xMi8wN2FkbWluMTcwMTk0NjM3MjY4Mi5wbmciLCJwaG9uZSI6IjEzNTUzNTM1MzUzIiwib3JnYW5pemF0aW9ucyI6W3sic2l0ZUNvZGUiOiJTMSIsImlkIjo0Mywib3JnYW5pemF0aW9uQ29kZSI6IjY1NmZhZTkwZjlhNTQ5MmI5MTE3NTRjZjcyN2M2NTA5Iiwib3JnYW5pemF0aW9uTmFtZSI6InN5bmMiLCJwYXJlbnRJZCI6MSwib3BlcmF0ZSI6MH1dLCJsb2NrZWQiOmZhbHNlLCJkaXNhYmxlIjpmYWxzZSwiYm91bmRUcGF1dGhUeXBlcyI6W10sInJvb3RVc2VyIjp0cnVlLCJwYXJlbnRDb2RlIjoiMmQzZjI4Yjc0YTNhN2VkZGRjYTQxMTMxNWM0MjczODkiLCJ0d29GYWN0b3JDaGVjayI6ZmFsc2UsInB3ZENoYW5nZVRpbWUiOiIyMDIyLTEyLTI3IDE1OjUxOjIxIiwicHdkQ2hhbmdlUGVyaW9kIjotMSwiYWNjb3VudE5vbkxvY2tlZCI6dHJ1ZSwiYWNjb3VudE5vbkV4cGlyZWQiOnRydWUsImNyZWRlbnRpYWxzTm9uRXhwaXJlZCI6dHJ1ZSwiZW5hYmxlZCI6dHJ1ZSwiYWRtaW4iOnRydWV9LCJncmFudF90eXBlIjoicGFzc3dvcmQiLCJ1c2VyX25hbWUiOiJhZG1pbiIsInNjb3BlIjpbImFsbCJdLCJleHAiOjE3MDQ3MTYyMDUsImp0aSI6ImRiOTMyNzNlLWQ3MzUtNGY3My1hZThhLTdhMzRjZTQ0MzM2YyIsImNsaWVudF9pZCI6ImFkbWluIn0.ZU6_RgrNl6Z7mBjmaDlPt_9yH5mQ2bttvxVJjEAkNFY");

        bResult = m_httpHelper.Http_Get(m_strHttpUrl, inputHeaderMap, strResponseHeader, strResponseBody);
    }
    if (m_strHttpType == "POST")
    {
        //测试
        //m_strHttpUrl = "http://172.16.152.69:30069/oauth/token?grant_type=password&username=admin&password=SobeyHive2016";
        //inputHeaderMap.insert("Authorization","Basic YWRtaW46JDBiRXlIaXZlJjJvMVNpeA==");

        bResult = m_httpHelper.Http_Post(m_strHttpUrl, inputHeaderMap, inputBody, strResponseHeader, strResponseBody);
    }
    qint64 dwEnd = getTickCount();

    if (bResult)
    {
        QString strStateMsg = QString("%1请求: 耗时%2毫秒 ").arg(m_strHttpType).arg(dwEnd-dwStart);
        ui->m_editRetHeader->setText(strResponseHeader);
        ui->m_editRetBody->setText(strResponseBody);
        m_statebar_message->setText(strStateMsg);
    }
    else
    {
        QString strStateMsg = QString("访问出错,URL = %1").arg(m_strHttpUrl);
        ui->m_editRetHeader->setText(strResponseHeader);
        ui->m_editRetBody->setText(strResponseBody);
        m_statebar_message->setText(strStateMsg);

        QMessageBox message(QMessageBox::Critical, "错误", strStateMsg);
        message.exec();
    }
}

void QHttpTester::on_m_btnDownload_clicked()
{
    m_strHttpUrl = ui->m_editURL->toPlainText();
    if (m_strHttpUrl.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "URL为空...");
        message.exec();
        return;
    }

    bool bResult = true;
    QString strFilePath = openSaveFileDlg();
    if (m_strHttpType == "GET")
    {
        bResult = m_httpHelper.HttpDownload_Get(m_strHttpUrl, strFilePath);
    }
    if (m_strHttpType == "POST")
    {
        QString inputBody;
        bResult = m_httpHelper.HttpDownload_Post(m_strHttpUrl, inputBody, strFilePath);
    }

    QString strText = (bResult)?(QString("下载成功, File=%1").arg(strFilePath)):(QString("下载出错,URL=%1").arg(m_strHttpUrl));
    QMessageBox message(QMessageBox::Information, "信息", strText);
    message.exec();
}

