#ifndef PUBLIC_H
#define PUBLIC_H

#include <QDebug>
#include <QVariant>
#include <QString>
#include <QChar>

////////////////////////////////////////////////////////////////////////////////////
QString RestoreESCString(const QString& escString);
QString CovertToESCString(const QString& beConvertString);
QString ConvertToQString(const QVariant &variant);

////////////////////////////////////////////////////////////////////////////////////
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
bool PathFileExists(const QString& path);
QString openSelectFileDlg(QString strTitle, const QStringList &filterList);
QString openSaveFileDlg(QString defaultSuffix = QString("txt"));
QString openSelectFolderDialog(QString strTitle);
bool saveStringToFile(const QString& filePath, const QString& content);

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
