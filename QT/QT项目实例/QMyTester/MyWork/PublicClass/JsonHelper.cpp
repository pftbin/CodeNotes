#include "JsonHelper.h"

JsonHelper::JsonHelper()
{

}

//Object相关
QJsonObject JsonHelper::createJsonObject()
{
    return QJsonObject();
}

void JsonHelper::addValueToObject(QJsonObject &jsonObj, const QString &key, const QJsonValue &value)
{
    jsonObj.insert(key, value);
}

QString JsonHelper::saveToString(const QJsonObject &jsonObj)
{
    QJsonDocument jsonDoc(jsonObj);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Indented);

    return jsonString;
}

bool JsonHelper::loadStringToObject(const QString &jsonString, QJsonObject &jsonObj)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON string";
        return false;
    }

    jsonObj = jsonDoc.object();
    return true;
}

//Array相关
QJsonArray JsonHelper::createJsonArray()
{
    return QJsonArray();
}

void JsonHelper::addValueToArray(QJsonArray &jsonArray, const QJsonValue &value)
{
    jsonArray.append(value);
}

QString JsonHelper::saveToString(const QJsonArray &jsonArray)
{
    QJsonDocument jsonDoc(jsonArray);
    return jsonDoc.toJson(QJsonDocument::Indented);
}

bool loadStringToArray(const QString &jsonString, QJsonArray &jsonArray)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON string";
        return false;
    }

    jsonArray = jsonDoc.array();
    return true;
}
