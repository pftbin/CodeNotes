#include "HttpHelper.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>
#include <QDir>
#include <QString>

HttpHelper::HttpHelper()
{
}

bool HttpHelper::Http_Get(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody)
{
    bool bResult = true;
    try
    {
        QNetworkAccessManager manager;
        QNetworkRequest request;

        QString strURL = url;
        if (!strURL.startsWith("http://") && !strURL.startsWith("https://"))
            strURL = QString("http://") + url;
        request.setUrl(QUrl(strURL));

        // 设置请求头
        for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
            request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
        }

        //
        QNetworkReply* reply = manager.get(request);

        //
        QEventLoop loop;
        int msec = 10 * 1000;
        QTimer* timer = new QTimer();
        timer->start(msec);

        QObject::connect(timer, &QTimer::timeout, &loop, &QEventLoop::quit);

        QObject::connect(reply, &QNetworkReply::finished, [&]() {
            if (reply->error() == QNetworkReply::NoError) {
                // 获取响应头
                QList<QByteArray> rawHeaders = reply->rawHeaderList();
                for (const QByteArray& rawHeader : rawHeaders) {
                    responseHeaders += rawHeader + ": " + reply->rawHeader(rawHeader) + "\n";
                }

                // 获取响应体
                responseBody = reply->readAll();

                loop.quit();
            } else {
                qDebug() << "Error:" << reply->errorString();
                loop.quit();
            }
        });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();

        qDebug() <<"Http_Get:"<<url<<",Error ="<<reply->error()<<"\n";
        bResult = (reply->error() == QNetworkReply::NoError);

    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        qDebug() << "Exception caught: " << e.what();
        bResult = false;
    }

    return bResult;
}

bool HttpHelper::Http_Post(const QString& url, const QMap<QString, QString>& headers, const QString& body, QString& responseHeaders, QString& responseBody)
{
    bool bResult = true;
    try
    {
        QNetworkAccessManager manager;
        QNetworkRequest request;

        QString strURL = url;
        if (!strURL.startsWith("http://") && !strURL.startsWith("https://"))
            strURL = QString("http://") + url;
        request.setUrl(QUrl(strURL));

        // 设置请求头
        for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
            request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
        }

        //
        QNetworkReply* reply = manager.post(request, body.toUtf8());

        //
        QEventLoop loop;
        int msec = 10 * 1000;
        QTimer* timer = new QTimer();
        timer->start(msec);

        QObject::connect(timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::finished, [&]() {
            if (reply->error() == QNetworkReply::NoError) {
                // 获取响应头
                QList<QByteArray> rawHeaders = reply->rawHeaderList();
                for (const QByteArray& rawHeader : rawHeaders) {
                    responseHeaders += rawHeader + ": " + reply->rawHeader(rawHeader) + "\n";
                }

                // 获取响应体
                responseBody = reply->readAll();

                loop.quit();
            } else {
                qDebug() << "Error:" << reply->errorString();
                loop.quit();
            }
        });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();

        qDebug() <<"Http_Post:"<<url<<",Error ="<<reply->error()<<"\n";
        bResult = (reply->error() == QNetworkReply::NoError);
    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        qDebug() << "Exception caught: " << e.what();
        bResult = false;
    }

    return bResult;
}

bool HttpHelper::HttpDownload_Get(const QString strUrl, QString strPath)
{
    if (strUrl.isEmpty())
    {
        QString strText = QString::fromLocal8Bit("请指定需要下载的URL");
        QMessageBox message(QMessageBox::NoIcon, "错误",strText);
        message.exec();
        return false;
    }

    QUrl newUrl = QUrl::fromUserInput(strUrl);
    if (!newUrl.isValid())
    {
        QString strText = QString("无效URL: %1 \n 错误信息: %2").arg(strUrl, newUrl.errorString());
        QMessageBox message(QMessageBox::NoIcon, "错误",strText);
        message.exec();
        return false;
    }

    QString fullFileName = strPath;

    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    m_pDownloadedFile =new QFile(fullFileName);  //创建临时文件
    if (!m_pDownloadedFile->open(QIODevice::WriteOnly))
    {
        QMessageBox message(QMessageBox::NoIcon, tr("Error"), tr("Open File Failed..."));
        message.exec();
        return false;
    }

    m_pReply = m_NetworkManager.get(QNetworkRequest(newUrl));

    //下载完响应finished信号
    connect(m_pReply, SIGNAL(finished()), this, SLOT(on_finished()));
    connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(on_signalerror(QNetworkReply::NetworkError)));

    //缓冲区有新的下载数据时，响应readyRead信号
    connect(m_pReply, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    return true;
}

bool HttpHelper::HttpDownload_Post(const QString strUrl, const QString strData, QString strPath)
{
    if (strUrl.isEmpty())
    {
        QMessageBox message(QMessageBox::NoIcon, tr("Error"),tr("URL is Empty..."));
        message.exec();
        return false;
    }

    QUrl newUrl = QUrl::fromUserInput(strUrl);
    if (!newUrl.isValid())
    {
        QString strText = QString(tr("Invalid URL: %1 \n Error Message: %2")).arg(strUrl, newUrl.errorString());
        QMessageBox message(QMessageBox::NoIcon, "Error",strText);
        message.exec();
        return false;
    }

    QString fullFileName = strPath;

    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    m_pDownloadedFile =new QFile(fullFileName);  //创建临时文件
    if (!m_pDownloadedFile->open(QIODevice::WriteOnly))
    {
        QMessageBox message(QMessageBox::NoIcon, tr("Error"), tr("Open File Failed..."));
        message.exec();
        return false;
    }

    m_pReply = m_NetworkManager.post(QNetworkRequest(newUrl),strData.toLatin1());

    //下载完响应finished信号
    connect(m_pReply, SIGNAL(finished()), this, SLOT(on_finished()));

    //缓冲区有新的下载数据时，响应readyRead信号
    connect(m_pReply, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    return true;
}

//网络响应结束
void HttpHelper::on_finished()
{
    QFileInfo fileInfo;
    QString strFileName = m_pDownloadedFile->fileName();
    fileInfo.setFile(strFileName);

    m_pDownloadedFile->close();
    delete m_pDownloadedFile;
    m_pDownloadedFile = Q_NULLPTR;

    m_pReply->deleteLater();
    m_pReply = Q_NULLPTR;

    emit signalfinished(strFileName);
}

void HttpHelper::on_signalerror(QNetworkReply::NetworkError error)
{
    emit signalerror(error);
}

//读取缓冲区下载的数据
void HttpHelper::on_readyRead()
{
    m_pDownloadedFile->write(m_pReply->readAll());
}
