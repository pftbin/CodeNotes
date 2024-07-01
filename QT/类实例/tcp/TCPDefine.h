#ifndef ITCPSERVER_H
#define ITCPSERVER_H

#include <QTcpSocket>
#include "pnp.h"

enum CodeType
{
    Type_UTF8 = 0,
    Type_UTF16 =1,
};

struct ClientNode
{
    ClientNode()
    {
        strMOSID.clear();
        RecvMsg.clear();
        SendMsg.clear();
        dstip.clear();
        srcip.clear();
        port = 0;
    }
    QString     strMOSID;
    QString     RecvMsg;
    QString     SendMsg;

    QTcpSocket* sock;
    QString		dstip;
    QString     srcip;
    quint16		port;
};

static bool IsValidPacket_i(const char* buf, int len)
{
    LPPNPHDR pHdr = (LPPNPHDR)buf;

    if(len < sizeof(PNPHDR)) return false;
    if(pHdr->ver > 1) return false;
    if(pHdr->symbol != PNP_SYMBOL) return false;
    if(pHdr->off < sizeof(PNPHDR)) return false;
    if(pHdr->off > pHdr->len) return false;
    if(pHdr->len > (unsigned int)len) return false;
    unsigned short oldsum = pHdr->checksum;
    pHdr->checksum = 0;
    if(Checksum((unsigned short*)pHdr, pHdr->off) != oldsum)
    {
        pHdr->checksum = oldsum;
        return false;
    }
    pHdr->checksum = oldsum;
    return true;
}



struct ITCPHandler
{
    virtual void HandleAccept(QTcpSocket* s, void* act) = 0;
    virtual void HandleConnect(QTcpSocket* s, void* act, bool bConnected) = 0;
    virtual void HandleInput(QTcpSocket* s, void* act, char* buf, qint32 len) = 0;
    virtual void HandleOutput(QTcpSocket* s, void* act) = 0;
    virtual void HandleClose(QTcpSocket* s, void* act) = 0;
};

struct IMsgBlockStrategy
{
    virtual int GetMsgBlock(const char* buf, qint32 len) = 0;
};

#endif // ITCPSERVER_H
