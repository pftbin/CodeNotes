#ifndef XMLHELPER_H
#define XMLHELPER_H

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>

class XmlHelper
{
public:
    XmlHelper();

    //加载xml
    bool loadXmlFromFile(const QString& filePath, QDomDocument& xmlDocument);
    bool loadXmlFromString(const QString& xmlString, QDomDocument& xmlDocument);

    //保存xml
    QString saveXmlToString(const QDomDocument& xmlDocument);
    bool saveXmlToFile(const QDomDocument& xmlDocument, const QString& filePath);

    //创建xml
    QDomDocument createXmlDocument(bool addDeclaration = false);
    //xml单节点
    QDomElement addNode(QDomDocument& xmlDocument, QDomElement* pParentNode, const QString& nodeName, const QString& nodeValue = "");
    QDomElement getNode(QDomDocument& xmlDocument, const QString& nodeName);
    //xml节点列表
    QDomElement addNodeList(QDomDocument& xmlDocument, QDomElement* pParentNode, const QString& nodeName, const QString& childNodeName, const QStringList& childNodeValueList);
    QDomNodeList getNodeList(QDomDocument& xmlDocument, const QString& nodeName);
    //xml节点属性
    void addAttribute(QDomElement& element, const QString& attributeName, const QString& attributeValue);
    QString getAttributeValue(const QDomElement& element, const QString& attributeName);

};

#endif // XMLHELPER_H
