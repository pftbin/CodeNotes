#ifndef QFTPHELPER_H
#define QFTPHELPER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

//FTPClient
class FTPClient : public QObject
{
    Q_OBJECT

public:
    FTPClient(QObject *parent = nullptr);

    bool login(const QString &url, const QString &username, const QString &password);
    bool uploadFile(const QString &url, const QString &filePath);
    bool downloadFile(const QString &url, const QString &filePath);

private slots:
    void onRequestFinished(QNetworkReply *reply);

signals:
    void loginError(const QString &error);
    void uploadError(const QString &error);
    void downloadError(const QString &error);

    void loginFinished();
    void uploadFinished();
    void downloadFinished();

private:
    QNetworkAccessManager   *m_manager;
    QUrl                    m_loginUrl;
    QString                 m_username;
    QString                 m_password;
    bool                    m_bLogIn;

    enum Operation
    {
        Upload,
        Download
    };
    Operation               m_currentOperation;
    QString                 m_downloadFilePath;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
class QFtpHelper : public QObject
{
    Q_OBJECT
public:
    explicit QFtpHelper(QObject *parent = nullptr);

public:

signals:

};

#endif // QFTPHELPER_H
