#include "QJsonHelper.h"

QJsonHelper::QJsonHelper()
{

}

//Object相关
QJsonObject QJsonHelper::createJsonObject()
{
    return QJsonObject();
}

void QJsonHelper::addValueToObject(QJsonObject &jsonObj, const QString &key, const QJsonValue &value)
{
    jsonObj.insert(key, value);
}

QString QJsonHelper::saveToString(const QJsonObject &jsonObj)
{
    QJsonDocument jsonDoc(jsonObj);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

    return jsonString;
}

bool QJsonHelper::loadStringToObject(const QString &jsonString, QJsonObject &jsonObj)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDoc.isNull()) {
        qDebug() << "Invalid JSON string: jsonDoc is null...";
        return false;
    }
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid JSON string: jsonDoc not is object...";
        return false;
    }

    jsonObj = jsonDoc.object();
    return true;
}

//Array相关
QJsonArray QJsonHelper::createJsonArray()
{
    return QJsonArray();
}

void QJsonHelper::addValueToArray(QJsonArray &jsonArray, const QJsonValue &value)
{
    jsonArray.append(value);
}

QString QJsonHelper::saveToString(const QJsonArray &jsonArray)
{
    QJsonDocument jsonDoc(jsonArray);
    return jsonDoc.toJson(QJsonDocument::Indented);
}

bool QJsonHelper::loadStringToArray(const QString &jsonString, QJsonArray &jsonArray)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDoc.isNull()) {
        qDebug() << "Invalid JSON string: jsonDoc is null...";
        return false;
    }
    if (!jsonDoc.isArray()) {
        qDebug() << "Invalid JSON string: jsonDoc not is array...";
        return false;
    }

    jsonArray = jsonDoc.array();
    return true;
}
