#include "XmlHelper.h"

XmlHelper::XmlHelper()
{

}

bool XmlHelper::loadXmlFromFile(const QString& filePath, QDomDocument& xmlDocument)
{
    bool result = true;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QString strText = QString(tr("Failed to open file for reading"));
        qDebug() << strText;
        result = false;
    }

    if (!xmlDocument.setContent(&file)) {
        QString strText = QString(tr("Failed to parse XML content"));
        qDebug() << strText;
        result = false;
    }
    file.close();

    return result;
}

bool XmlHelper::loadXmlFromString(const QString& xmlString, QDomDocument& xmlDocument)
{
    bool result = true;
    if (!xmlDocument.setContent(xmlString)) {
        QString strText = QString(tr("Failed to parse XML content"));
        qDebug() << strText;
        result = false;
    }

    return result;
}

QString XmlHelper::saveXmlToString(const QDomDocument& xmlDocument)
{
    QString xmlString;

    QTextStream textStream(&xmlString);
    textStream.setEncoding(QStringConverter::Encoding::Utf8); // 设置编码为UTF-8
    xmlDocument.save(textStream, 4);//设置缩进为4

    return xmlString;
}

bool XmlHelper::saveXmlToFile(const QDomDocument& xmlDocument, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QString strText = QString(tr("Failed to open file for writing"));
        qDebug() << strText;
        return false;
    }

    QTextStream textStream(&file);
    textStream.setEncoding(QStringConverter::Encoding::Utf8); // 设置编码为UTF-8
    xmlDocument.save(textStream, 4);//设置缩进为4
    file.close();

    return true;
}

QDomDocument XmlHelper::createXmlDocument(bool addDeclaration)
{
    QDomDocument xmlDocument;
    if (addDeclaration)
    {
        QDomProcessingInstruction xmlDeclaration = xmlDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        xmlDocument.appendChild(xmlDeclaration);
    }

    return xmlDocument;
}

QDomElement XmlHelper::addNode(QDomDocument& xmlDocument, QDomElement* pParentNode, const QString& nodeName, const QString& nodeValue)
{
    QDomElement element = xmlDocument.createElement(nodeName);
    if (!nodeValue.isEmpty()) {
        QDomText text = xmlDocument.createTextNode(nodeValue);
        element.appendChild(text);
    }

    if(pParentNode != nullptr)
    {
        pParentNode->appendChild(element);
        xmlDocument.appendChild(*pParentNode);
    }
    else
    {
        xmlDocument.appendChild(element);
    }

    return element;
}

QDomElement XmlHelper::getNode(QDomDocument& xmlDocument, const QString& nodeName)
{
    return xmlDocument.elementById(nodeName);
}

QDomElement XmlHelper::addNodeList(QDomDocument& xmlDocument, QDomElement* pParentNode, const QString& nodeName, const QString& childNodeName, const QStringList& childNodeValueList)
{
    QDomElement element = xmlDocument.createElement(nodeName);
    for (const QString &childNodeValue : childNodeValueList)
    {
        QDomElement childElement = xmlDocument.createElement(childNodeName);
        QDomText childText = xmlDocument.createTextNode(childNodeValue);
        childElement.appendChild(childText);//子Item添加文本
        element.appendChild(childElement);//添加子Item
    }

    if(pParentNode != nullptr)
    {
        pParentNode->appendChild(element);
        xmlDocument.appendChild(*pParentNode);
    }
    else
    {
       xmlDocument.appendChild(element);
    }

    return element;
}

QDomNodeList XmlHelper::getNodeList(QDomDocument& xmlDocument, const QString& nodeName)
{
    return xmlDocument.elementsByTagName(nodeName);
}

void XmlHelper::addAttribute(QDomElement& element, const QString& attributeName, const QString& attributeValue)
{
    element.setAttribute(attributeName, attributeValue);
}

QString XmlHelper::getAttributeValue(const QDomElement& element, const QString& attributeName)
{
    if (element.hasAttribute(attributeName))
    {
        return element.attribute(attributeName);
    }
    else
    {
        return QString(); // or return an empty string, depending on your requirement
    }
}


