#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>

class HttpHelper : public QObject
{
    Q_OBJECT

public:
    HttpHelper();

    bool Http_Get(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody);
    bool Http_Post(const QString& url, const QMap<QString, QString>& headers, const QString& body, QString& responseHeaders, QString& responseBody);

    bool HttpDownload_Get(const QString url, QString strPath);
    bool HttpDownload_Post(const QString url, const QString data, QString strPath);
signals:
    //合成完成
    void signalfinished(QString strFileName);
    void signalerror(QNetworkReply::NetworkError);
private slots:
    //合成完成
    void on_finished();
    void on_signalerror(QNetworkReply::NetworkError);
    //读取缓存中的数据
    void on_readyRead();

private:
    QNetworkAccessManager   m_NetworkManager;   //网络管理
    QNetworkReply           *m_pReply;          //网络响应
    QFile                   *m_pDownloadedFile; //保存下载的临时文件
};

#endif // HTTPHELPER_H
