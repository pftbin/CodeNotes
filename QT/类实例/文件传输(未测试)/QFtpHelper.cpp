#include "QFtpHelper.h"

//FTPClient
FTPClient::FTPClient(QObject *parent)
    : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &FTPClient::onRequestFinished);

    m_bLogIn = false;
}
bool FTPClient::login(const QString &url, const QString &username, const QString &password)
{
    m_loginUrl = QUrl(url);
    m_username = username;
    m_password = password;

    QMap<QString, QString> inputHeaderMap;
    inputHeaderMap.insert(QString("Authorization"), QString("Basic %1").arg(QString("%1:%2").arg(m_username).arg(m_password).toUtf8().toBase64()));


    QNetworkRequest request;
    request.setUrl(m_loginUrl);
    // 设置请求头
    for (auto it = inputHeaderMap.constBegin(); it != inputHeaderMap.constEnd(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    m_manager->get(request);
    return true;
}
bool FTPClient::uploadFile(const QString &url, const QString &filePath)
{
    if (!m_bLogIn)
    {
        emit uploadError("Please login before uploading");
        return false;
    }

    QUrl uploadUrl(url);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit uploadError("Failed to open file for reading");
        return false;
    }

    QNetworkRequest request(uploadUrl);
    QNetworkReply *reply = m_manager->put(request, &file);
    m_currentOperation = Upload;
    return true;
}
bool FTPClient::downloadFile(const QString &url, const QString &filePath)
{
    if (!m_bLogIn)
    {
        emit downloadError("Please login before downloading");
        return false;
    }

    QUrl downloadUrl(url);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        emit downloadError("Failed to open file for writing");
        return false;
    }

    QNetworkRequest request(downloadUrl);
    QNetworkReply *reply = m_manager->get(request);
    m_currentOperation = Download;
    return true;
}
void FTPClient::onRequestFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        if (m_currentOperation == Upload)
        {
            emit uploadError(reply->errorString());
        }
        else if (m_currentOperation == Download)
        {
            emit downloadError(reply->errorString());
        }
    }
    else
    {
        if (m_currentOperation == Upload)
        {
            emit uploadFinished();
        }
        else if (m_currentOperation == Download)
        {
            QFile file;
            file.setFileName(m_downloadFilePath);
            file.open(QIODevice::WriteOnly);
            file.write(reply->readAll());
            file.close();
            emit downloadFinished();
        }
    }
    reply->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
QFtpHelper::QFtpHelper(QObject *parent)
    : QObject{parent}
{

}
