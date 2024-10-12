#ifndef QCONVERTHELPER_H
#define QCONVERTHELPER_H

#include <QtXml/QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariant>
#include <QString>
#include <QVector>



typedef struct tagNodeTypeItem
{
    QString     strParentNodeName;
    QString     strNodeName;
    int         nNodeType;

    tagNodeTypeItem()
    {
        strParentNodeName = "";
        strNodeName = "";
        nNodeType = 0;
    }

    void copyData(const tagNodeTypeItem& item)
    {
        strParentNodeName = item.strParentNodeName;
        strNodeName = item.strNodeName;
        nNodeType = item.nNodeType;
    }

    tagNodeTypeItem& operator=(const tagNodeTypeItem& item)
    {
        strParentNodeName = item.strParentNodeName;
        strNodeName = item.strNodeName;
        nNodeType = item.nNodeType;

        return *this;
    }

} NodeTypeItem;

class QConvertHelper
{
public:
    QConvertHelper();

    enum NodeType
    {
        Type_Text = 0,
        Type_Object,
        Type_Array,

    };

public:
    void AddTypeDefItem(NodeTypeItem nodeTypeItem);
    void AddTypeDefItem(QString strParentNodeName, QString strNodeName, int nNodeType);
    bool jsonToXml(const QString& jsonString, QString& xmlString);
    bool xmlToJson(const QString& xmlString, QString& jsonString, bool bStringValue = false);

protected:
    bool haveTypeDef(QString strParentNodeName, QString strNodeName);
    int  getTypeDef(QString strParentNodeName, QString strNodeName);

    bool haveChild(const QDomElement& xmlElement);
    bool haveAttr(const QDomElement& xmlElement);
    bool isNodeList(const QDomElement& xmlElement);
    bool addAttrToObject(QDomElement& xmlElement, QJsonObject& jsonObject, bool bStringValue = false);
    void convertJsonArrayToXmlElement(const QJsonArray& jsonArray, QDomDocument& xmlDoc, QDomNode& xmlNode);
    void convertJsonObjectToXmlElement(const QJsonObject& jsonObject, QDomDocument& xmlDoc, QDomNode& xmlNode);
    void convertXmlElementToJsonObject(const QDomElement& xmlElement, QJsonObject& jsonObject);
    void convertXmlElementToJsonArray(const QDomElement& xmlElement, QJsonArray& jsonArray);

protected:
    bool                    m_bStringValue;     //所有json节点都用字符串类型
    QVector<NodeTypeItem>   m_vecNodeTypeDef;   //需要转换为特殊类型的节点

};

#endif // QCONVERTHELPER_H
