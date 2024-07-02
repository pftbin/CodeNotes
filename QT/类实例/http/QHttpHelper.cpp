#include "QHttpHelper.h"



QString GetOperateName(QNetworkAccessManager::Operation optMethod)
{
    QString strName = QString("unkown");
    switch (optMethod) {
    case QNetworkAccessManager::GetOperation:
    {
        strName = QString("GET");
    }
    break;
    case QNetworkAccessManager::PostOperation:
    {
        strName = QString("POST");
    }
    break;
    case QNetworkAccessManager::PutOperation:
    {
        strName = QString("PUT");
    }
    break;
    case QNetworkAccessManager::DeleteOperation:
    {
        strName = QString("DELETE");
    }
    break;
    case QNetworkAccessManager::CustomOperation:
    {
        strName = QString("Custom");
    }
    break;
    default:
        break;
    }
    return strName;
}
QHttpHelper::QHttpHelper()
{
}

bool QHttpHelper::Http_Get(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody, int timeout)
{
    bool bResult = true;
    QString strError = "";
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
        QTimer* timer = new QTimer();
        timer->start(timeout);

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
                strError = QString("%1").arg(reply->errorString());
                qDebug() << "[Http_Get]: Error=" << reply->errorString();
                loop.quit();
            }
        });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();
        bResult = (reply->error() == QNetworkReply::NoError);
    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        strError = QString("%1").arg(e.what());
        qDebug() << "[Http_Get]: catch exception=" << e.what();
        bResult = false;
    }

    if (!bResult)
        m_lastError = QString(tr("Http_Get: %1,Error=%2\n")).arg(url).arg(strError);
    return bResult;
}

bool QHttpHelper::Http_Delete(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody, int timeout)
{
    bool bResult = true;
    QString strError = "";
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
        QNetworkReply* reply = manager.deleteResource(request);

        //
        QEventLoop loop;
        QTimer* timer = new QTimer();
        timer->start(timeout);

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
            }
            else {
                strError = QString("%1").arg(reply->errorString());
                qDebug() << "[Http_Delete]: Error=" << reply->errorString();
                loop.quit();
            }
            });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();
        bResult = (reply->error() == QNetworkReply::NoError);
    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        strError = QString("%1").arg(e.what());
        qDebug() << "[Http_Delete]: catch exception=" << e.what();
        bResult = false;
    }

    if (!bResult)
        m_lastError = QString(tr("Http_Delete: %1,Error=%2\n")).arg(url).arg(strError);
    return bResult;
}

bool QHttpHelper::Http_Post(const QString& url, const QMap<QString, QString>& headers, const QString& body, QString& responseHeaders, QString& responseBody, int timeout)
{
    bool bResult = true;
    QString strError = "";
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
        QTimer* timer = new QTimer();
        timer->start(timeout);

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
                strError = QString("%1").arg(reply->errorString());
                qDebug() << "[Http_Post]: Error=" << reply->errorString();
                loop.quit();
            }
        });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();
        bResult = (reply->error() == QNetworkReply::NoError);
    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        strError = QString("%1").arg(e.what());
        qDebug() << "[Http_Post]: catch exception=" << e.what();
        bResult = false;
    }

    if (!bResult)
        m_lastError = QString(tr("Http_Get: %1,Error=%2\n")).arg(url).arg(strError);
    return bResult;
}

bool QHttpHelper::HttpDownload_Get(const QString strUrl, QString strPath)
{
    if (strUrl.isEmpty())
    {
        m_lastError = QString(tr("URL is empty..."));
        qDebug() << m_lastError;
        return false;
    }

    QUrl newUrl = QUrl::fromUserInput(strUrl);
    if (!newUrl.isValid())
    {
        QString strText = QString(tr("Invalid URL: %1 \n Error: %2")).arg(strUrl, newUrl.errorString());
        return false;
    }

    QString fullFileName = strPath;

    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    m_pDownloadedFile =new QFile(fullFileName);  //创建临时文件
    if (!m_pDownloadedFile->open(QIODevice::WriteOnly))
    {
        m_lastError = QString(tr("Open File Failed..."));
        qDebug() << m_lastError;
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

bool QHttpHelper::HttpDownload_Post(const QString strUrl, const QString strData, QString strPath)
{
    if (strUrl.isEmpty())
    {
        m_lastError = QString(tr("URL is empty..."));
        qDebug() << m_lastError;
        return false;
    }

    QUrl newUrl = QUrl::fromUserInput(strUrl);
    if (!newUrl.isValid())
    {
        m_lastError = QString(tr("Invalid URL: %1 \n Error: %2")).arg(strUrl, newUrl.errorString());
        qDebug() << m_lastError;
        return false;
    }

    QString fullFileName = strPath;

    if (QFile::exists(fullFileName))
        QFile::remove(fullFileName);

    m_pDownloadedFile =new QFile(fullFileName);  //创建临时文件
    if (!m_pDownloadedFile->open(QIODevice::WriteOnly))
    {
        m_lastError = QString(tr("Open File Failed..."));
        qDebug() << m_lastError;
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
void QHttpHelper::on_finished()
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

void QHttpHelper::on_signalerror(QNetworkReply::NetworkError error)
{
    emit signalerror(error);
}

//读取缓冲区下载的数据
void QHttpHelper::on_readyRead()
{
    m_pDownloadedFile->write(m_pReply->readAll());
}
bool QHttpHelper::Http_Operate(const QString& url, iHeaderStrategy* pStrategy,QNetworkAccessManager::Operation opt, int timeout)
{
    bool bResult = true;
    QString strError = "";
    if(pStrategy == Q_NULLPTR)
        return false;
    try
    {
        QNetworkAccessManager manager;
        QNetworkRequest request;

        QString strURL = url;
        if (!strURL.startsWith("http://") && !strURL.startsWith("https://"))
            strURL = QString("http://") + url;
        request.setUrl(QUrl(strURL));

        // 设置请求头
        for (auto it = pStrategy->next(); it  != Q_NULLPTR;) {
            request.setRawHeader(it->pheadKey, it->pheadData);
        }
        //
        QNetworkReply* reply = pStrategy->OperateQuest(opt,manager,request);
        if(reply == Q_NULLPTR)
        {
            return false;
        }
        //
        QEventLoop loop;
        QTimer* timer = new QTimer();
        timer->start(timeout);

        QObject::connect(timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::finished, [&]() {
            if (reply->error() == QNetworkReply::NoError) {
                // 获取响应头
                //QList<QByteArray> rawHeaders = reply->rawHeaderList();
                //for (const QByteArray& rawHeader : rawHeaders) {
                   // responseHeaders += rawHeader + ": " + reply->rawHeader(rawHeader) + "\n";
                //}

                // 获取响应体
                QByteArray responseBody = reply->readAll();
                pStrategy->SetReturn(responseBody);

                loop.quit();
            } else {
                strError = QString("%1").arg(reply->errorString());
                qDebug() << "[Http_Post]: Error=" << reply->errorString();
                loop.quit();
            }
        });

        loop.exec();
        reply->deleteLater();
        timer->stop();
        timer->deleteLater();
        bResult = (reply->error() == QNetworkReply::NoError);
    }
    catch (const std::exception& e)
    {
        // 捕获并处理异常
        strError = QString("%1").arg(e.what());
        qDebug() << "[Http_Post]: catch exception=" << e.what();
        bResult = false;
    }

    if (!bResult)
        m_lastError = QString(tr("Http_Get: %1,Error=%2\n")).arg(url).arg(strError);
    return bResult;
}
