#ifndef PUBLIC_H
#define PUBLIC_H

#include <string>
#include <QDebug>
#include <QVariant>
#include <QChar>
#include <QString>
#include <QStringList>


////////////////////////////////////////////////////////////////////////////////////
QString RestoreESCString(const QString& escString);
QString CovertToESCString(const QString& beConvertString);
QString ConvertToQString(const QVariant &variant);
std::string QStringToStdString(QString& qString);
QString StdStringToQString(std::string& stdString);

int MatchQStringListIdx(QStringList& stringList, QString& qString);
////////////////////////////////////////////////////////////////////////////////////
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QColorDialog>
#include <QTextStream>
bool PathFileExists(const QString& path);
QString openSelectFileDlg(QString strTitle, const QStringList &filterList);
QString openSaveFileDlg(QString defaultSuffix = QString("txt"));
QString openSelectFolderDialog(QString strTitle);
bool saveStringToFile(const QString& filePath, const QString& content);
bool openSelectColorDialog(QColor& color);

////////////////////////////////////////////////////////////////////////////////////
#include <QMessageBox>
#include <QAbstractButton>

#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
int AfxMessageBox(QString strTitle, QString strMessage, unsigned int uID = MB_OK, bool bChinese = true);
int AfxWarningBox(QString strTitle, QString strMessage, bool bChinese = true);
int AfxErrorBox(QString strTitle, QString strMessage, bool bChinese = true);

////////////////////////////////////////////////////////////////////////////////////
#include <QDateTime>
#include <QElapsedTimer>
qint64 getTickCount();
QString getCurrentTime();

////////////////////////////////////////////////////////////////////////////////////
#include <QRandomGenerator>
QString getRandomString(int maxLen = 30);
int getRandomInt(int maxValue = 100000);
double getRandomDouble(int maxvalue = 100000);

////////////////////////////////////////////////////////////////////////////////////
#include <QtXml/QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariant>
bool jsonToXml(const QString& jsonString, QString& xmlString);
bool xmlToJson(const QString& xmlString, QString& jsonString);
bool isNodeList(const QDomElement& xmlElement);

#endif // PUBLIC_H
