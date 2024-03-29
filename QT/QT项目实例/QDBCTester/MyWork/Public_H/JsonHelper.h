#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>

class JsonHelper
{
public:
    JsonHelper();

    // 构造一个空的JSON对象
    static QJsonObject createJsonObject();
    // 向JSON对象中添加键值对
    static void addValueToObject(QJsonObject &jsonObj, const QString &key, const QJsonValue &value);
    // 将JSON对象转换为JSON字符串
    static QString saveToString(const QJsonObject &jsonObj);
    // 将JSON字符串转换为JSON对象
    static bool loadStringToObject(const QString &jsonString, QJsonObject &jsonObj);


    // 构造一个空的JSON数组
    static QJsonArray createJsonArray();
    // 向JSON数组中添加元素
    static void addValueToArray(QJsonArray &jsonArray, const QJsonValue &value);
    // 将JSON数组转换为JSON字符串
    static QString saveToString(const QJsonArray &jsonArray);
    // 将JSON字符串转换为JSON数组
    static bool loadStringToArray(const QString &jsonString, QJsonArray &jsonArray);
};

#endif // JSONHELPER_H
