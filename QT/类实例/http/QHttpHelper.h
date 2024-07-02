#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include <QString>
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QFileInfo>
#include <QFile>
#include <QDir>

typedef struct __headnode
{
    QByteArray pheadKey;
    QByteArray pheadData;
    __headnode *pNext;
    __headnode()
    {
        pNext = NULL;
    }
}HeadNode;
QString GetOperateName(QNetworkAccessManager::Operation optMethod);

class iHeaderStrategy
{
public:
    virtual void AddHeader(const QString &strHKey,const QString &strHValue) = 0;
    virtual void AddHeader(const char *szUtf8HKey,const char *szUtf8HValue) = 0;
    virtual HeadNode *next()      = 0;
    virtual void SetData(const QString& strData,bool bJson = true)
    {
        m_strData = strData.toUtf8();
        if (bJson)
        {
            AddHeader("Content-Type","application/json");
            AddHeader(("Content-Length"),QString::number(m_strData.size()));
        }
    }
    virtual QNetworkReply *OperateQuest(QNetworkAccessManager::Operation opt,QNetworkAccessManager &mng,const QNetworkRequest &qst)
    {
        QNetworkReply *pReplay = Q_NULLPTR;
        switch (opt) {
        case QNetworkAccessManager::GetOperation:
        {
           pReplay = mng.get(qst);
        }
        break;
        case QNetworkAccessManager::PostOperation:
        {
             pReplay = mng.post(qst,m_strData);
        }
        break;
        case QNetworkAccessManager::PutOperation:
        {
            pReplay = mng.put(qst,m_strData);
        }
        break;
        case QNetworkAccessManager::DeleteOperation:
        {
            pReplay = mng.deleteResource(qst);
        }
        break;
        case QNetworkAccessManager::CustomOperation:
        {
            pReplay = mng.sendCustomRequest(qst,m_strData);
        }
        break;
        default:
            break;
        }
        return pReplay;
    }
    virtual void SetReturn(const QByteArray &strReturn)
    {
        m_strReturn = strReturn;
    }
    virtual ~iHeaderStrategy()
    {
    }
    QByteArray m_strData;
    QByteArray m_strReturn;
};
class CNormalHeader:public iHeaderStrategy
{
public:
    CNormalHeader()
    {
        m_pHead = NULL;
        m_pHeadTail = NULL;
        m_pHeadCur = NULL;
    }
    virtual ~CNormalHeader()
    {
        m_pHeadCur = m_pHead;
        while(m_pHeadCur)
        {
            HeadNode *pnode = m_pHeadCur;
            m_pHeadCur = m_pHeadCur->pNext;
            if(pnode)
            {
                delete pnode;
            }
        }

    }
    virtual void AddHeader(const QString &strHKey,const QString &strHValue)
    {
        QString strtmp = strHKey;
        HeadNode *pnode = new HeadNode();
        pnode->pheadKey = strtmp.toUtf8();
        strtmp = strHValue;
        pnode->pheadData = strtmp.toUtf8();
        pnode->pNext = NULL;
        if(m_pHead == NULL)
        {
            m_pHead = pnode;
            m_pHeadTail = m_pHead;
            m_pHeadCur = m_pHead;
            return ;
        }
        else
        {
            m_pHeadTail->pNext = pnode;
            m_pHeadTail = pnode;
        }
    }
    virtual void AddHeader(const char *szUtf8HKey,const char *szUtf8HValue)
    {
        HeadNode *pnode = new HeadNode();
        pnode->pheadData = QByteArray(szUtf8HValue);
        pnode->pheadKey = QByteArray(szUtf8HKey);
        pnode->pNext = NULL;
        if(m_pHead == NULL)
        {
            m_pHead = pnode;
            m_pHeadTail = m_pHead;
            m_pHeadCur = m_pHead;
            return ;
        }
        else
        {
            m_pHeadTail->pNext = pnode;
            m_pHeadTail = pnode;
        }
    }
    virtual HeadNode *next()
    {
        if(m_pHeadCur)
        {
            HeadNode* pret = m_pHeadCur;
            m_pHeadCur = m_pHeadCur->pNext;
            return pret;
        }
        return Q_NULLPTR;
    }

protected:
    HeadNode *m_pHead;
    HeadNode *m_pHeadTail;
    HeadNode *m_pHeadCur;
};
struct DataBlock
{
    DataBlock(int nBufferSize = 1024 * 5000);
    ~DataBlock();

    char* pBuff;
    size_t nBufSize;
    size_t nPos;
};

class QHttpHelper : public QObject
{
    Q_OBJECT

public:
    QHttpHelper();

    QString Http_LastError() {return m_lastError;}

    bool Http_Get(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody, int timeout = 5000);
    bool Http_Delete(const QString& url, const QMap<QString, QString>& headers, QString& responseHeaders, QString& responseBody, int timeout = 5000);
    bool Http_Post(const QString& url, const QMap<QString, QString>& headers, const QString& body, QString& responseHeaders, QString& responseBody, int timeout = 5000);
    bool Http_Operate(const QString& url, iHeaderStrategy* pStrategy, QNetworkAccessManager::Operation opt = QNetworkAccessManager::GetOperation,int timeout = 5000);

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
    QString                 m_lastError;        //错误信息
};

#endif // HTTPHELPER_H
