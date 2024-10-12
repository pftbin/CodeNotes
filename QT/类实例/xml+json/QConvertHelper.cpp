#include "QConvertHelper.h"

QConvertHelper::QConvertHelper()
{
    m_bStringValue = false;

}

////////////////////////////////////////////////////////////////////////////////////




void QConvertHelper::AddTypeDefItem(NodeTypeItem nodeTypeItem)
{
    m_vecNodeTypeDef.push_back(nodeTypeItem);
}
void QConvertHelper::AddTypeDefItem(QString strParentNodeName, QString strNodeName, int nNodeType)
{
    NodeTypeItem nodeTypeItem;
    nodeTypeItem.strParentNodeName = strParentNodeName;
    nodeTypeItem.strNodeName = strNodeName;
    nodeTypeItem.nNodeType = nNodeType;

    m_vecNodeTypeDef.push_back(nodeTypeItem);
}
bool QConvertHelper::xmlToJson(const QString& xmlString, QString& jsonString, bool bStringValue)
{
    m_bStringValue = bStringValue;

    // 创建XML文档
    QDomDocument xmlDoc;

    // 判断XML解析是否成功
    if (!xmlDoc.setContent(xmlString))
    {
        return false;
    }

    // 获取根节点
    QDomElement rootElement = xmlDoc.documentElement();

    // 创建JSON对象、JSON数组
    QJsonObject jsonObject;
    QJsonArray jsonArray;

    // 根据子节点判断转为JSON对象还是JSON数组
    if (!isNodeList(rootElement))
    {
        // 将XML转换为JSON对象
        convertXmlElementToJsonObject(rootElement, jsonObject);
        //QJsonDocument jsonDoc(jsonObject);
        //jsonString = jsonDoc.toJson(QJsonDocument::Indented);

        // 将xml根节点作为第一个JsonObject(适配:保留根节点名称)
        QJsonObject jsonObjectEx;
        QString strFirstObjName = rootElement.tagName();
        jsonObjectEx[strFirstObjName] = jsonObject;
        QJsonDocument jsonDocEx(jsonObjectEx);
        jsonString = jsonDocEx.toJson(QJsonDocument::Indented);
    }
    else
    {
        // 将XML转换为JSON数组
        convertXmlElementToJsonArray(rootElement, jsonArray);
        QJsonDocument jsonDoc(jsonArray);
        jsonString = jsonDoc.toJson(QJsonDocument::Indented);
    }

    return true;
}
bool QConvertHelper::jsonToXml(const QString& jsonString, QString& xmlString)
{
    // 将JSON字符串转换为QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 判断JSON转换是否成功
    if (jsonDoc.isNull())
    {
        return false;
    }

    // 创建XML文档
    QDomDocument xmlDoc;

    // 创建根节点
    QDomElement rootElement = xmlDoc.createElement("root");
    xmlDoc.appendChild(rootElement);

    // 将JSON转换为XML节点
    QJsonObject jsonObject = jsonDoc.object();
    convertJsonObjectToXmlElement(jsonObject, xmlDoc, rootElement);

    // 将XML文档保存为字符串
    xmlString = xmlDoc.toString();

    return true;
}

bool QConvertHelper::haveTypeDef(QString strParentNodeName, QString strNodeName)
{
    bool bFind = false;
    for (int i = 0; i < m_vecNodeTypeDef.size(); i++)
    {
        if (m_vecNodeTypeDef[i].strParentNodeName.isEmpty() && strNodeName==m_vecNodeTypeDef[i].strNodeName)//父节点可不指定
        {
            bFind = true;
            break;
        }
        else if (strParentNodeName==m_vecNodeTypeDef[i].strParentNodeName && strNodeName==m_vecNodeTypeDef[i].strNodeName)
        {
            bFind = true;
            break;
        }
    }

    return bFind;
}
int  QConvertHelper::getTypeDef(QString strParentNodeName, QString strNodeName)
{
    int nNodeType = 0;
    for (int i = 0; i < m_vecNodeTypeDef.size(); i++)
    {
        if (m_vecNodeTypeDef[i].strParentNodeName.isEmpty() && strNodeName==m_vecNodeTypeDef[i].strNodeName)//父节点可不指定
        {
            nNodeType = m_vecNodeTypeDef[i].nNodeType;
            break;
        }
        else if (strParentNodeName==m_vecNodeTypeDef[i].strParentNodeName && strNodeName==m_vecNodeTypeDef[i].strNodeName)
        {
            nNodeType = m_vecNodeTypeDef[i].nNodeType;
            break;
        }
    }

    return nNodeType;
}


bool QConvertHelper::haveChild(const QDomElement& xmlElement)
{
    return (!xmlElement.firstChildElement().isNull());
}
bool QConvertHelper::haveAttr(const QDomElement& xmlElement)
{
    QDomNamedNodeMap attributes = xmlElement.attributes();
    return (attributes.size()>0);
}
bool QConvertHelper::isNodeList(const QDomElement& xmlElement)
{
    //判断子节点名称
    QSet<QString> setNodeName;
    QVector<QString> vecNodeName;
    for (QDomNode xmlNode = xmlElement.firstChild(); !xmlNode.isNull(); xmlNode = xmlNode.nextSibling())
    {
        if (xmlNode.isElement())
        {
            QDomElement childElement = xmlNode.toElement();
            if (!childElement.firstChildElement().isNull())
            {
                //还有下层字节点,不视为Array
                return false;
            }

            QString tagName = childElement.tagName();
            setNodeName.insert(tagName);
            vecNodeName.push_back(tagName);
        }
    }

    return (setNodeName.size()==1)&&(vecNodeName.size()>1);//子节点数量大于1但名称相同
}
bool QConvertHelper::addAttrToObject(QDomElement& xmlElement, QJsonObject& jsonObject, bool bStringValue)
{
    QDomNamedNodeMap attributes = xmlElement.attributes();
    for (int i = 0; i < attributes.size(); ++i)
    {
        QDomAttr attrNode = attributes.item(i).toAttr();
        QString strAttrName = attrNode.name();
        QString strAttrValue = attrNode.value();

        // 将子元素的属性内容添加到JSON对象中(适配:节点属性作为子成员)
        if (bStringValue)
        {
            if (jsonObject.contains(strAttrName))
                jsonObject.remove(strAttrName);
            jsonObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
        }
        else
        {
            if (jsonObject.contains(strAttrName))
                jsonObject.remove(strAttrName);
            jsonObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
            bool isIntAttr; int intValueAttr = strAttrValue.toInt(&isIntAttr);
            bool isDoubleAttr; double doubleValueAttr = strAttrValue.toDouble(&isDoubleAttr);
            bool isBoolAttr = (strAttrValue.toLower()=="true" || strAttrValue.toLower()=="false");
            if (isIntAttr)
            {
                if (jsonObject.contains(strAttrName))
                    jsonObject.remove(strAttrName);
                jsonObject[strAttrName] = QJsonValue::fromVariant(intValueAttr);
            }
            else if(isDoubleAttr)
            {
                if (jsonObject.contains(strAttrName))
                    jsonObject.remove(strAttrName);
                jsonObject[strAttrName] = QJsonValue::fromVariant(doubleValueAttr);
            }
            else if(isBoolAttr)
            {
                bool boolValueAttr = (strAttrValue.toLower()=="true");
                if (jsonObject.contains(strAttrName))
                    jsonObject.remove(strAttrName);
                jsonObject[strAttrName] = QJsonValue::fromVariant(boolValueAttr);
            }
            else
            {
                if (jsonObject.contains(strAttrName))
                    jsonObject.remove(strAttrName);
                jsonObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
            }
        }
    }

    return true;
}

void QConvertHelper::convertJsonObjectToXmlElement(const QJsonObject& jsonObject, QDomDocument& xmlDoc, QDomNode& xmlNode)
{
    // 遍历JSON对象的所有键值对
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
    {
        // 创建XML元素
        QDomElement xmlElement = xmlDoc.createElement(it.key());

        // 获取JSON值
        QJsonValue jsonValue = it.value();

        if (jsonValue.isObject())
        {
            // 处理子对象
            QJsonObject subObject = jsonValue.toObject();
            convertJsonObjectToXmlElement(subObject, xmlDoc, xmlElement);
        }
        else if (jsonValue.isArray())
        {
            // 处理数组
            QJsonArray jsonArray = jsonValue.toArray();
            convertJsonArrayToXmlElement(jsonArray, xmlDoc, xmlElement);
        }
        else
        {
            // 将JSON值转换为字符串
            QString valueString = jsonValue.toVariant().toString();

            // 设置XML元素的文本内容
            xmlElement.appendChild(xmlDoc.createTextNode(valueString));
        }

        // 将XML元素添加到XML节点中
        xmlNode.appendChild(xmlElement);
    }
}
void QConvertHelper::convertJsonArrayToXmlElement(const QJsonArray& jsonArray, QDomDocument& xmlDoc, QDomNode& xmlNode)
{
    // 遍历JSON数组的所有元素
    for (const QJsonValue& jsonValue : jsonArray)
    {
        // 创建XML元素
        QDomElement xmlElement = xmlDoc.createElement("item");

        if (jsonValue.isObject())
        {
            // 处理子对象
            QJsonObject subObject = jsonValue.toObject();
            convertJsonObjectToXmlElement(subObject, xmlDoc, xmlElement);
        }
        else if (jsonValue.isArray())
        {
            // 处理嵌套的数组
            QJsonArray nestedJsonArray = jsonValue.toArray();
            convertJsonArrayToXmlElement(nestedJsonArray, xmlDoc, xmlElement);
        }
        else
        {
            // 将JSON值转换为字符串
            QString valueString = jsonValue.toVariant().toString();

            // 设置XML元素的文本内容
            xmlElement.appendChild(xmlDoc.createTextNode(valueString));
        }

        // 将XML元素添加到XML节点中
        xmlNode.appendChild(xmlElement);
    }
}
void QConvertHelper::convertXmlElementToJsonObject(const QDomElement& xmlElement, QJsonObject& jsonObject)
{
    // 遍历XML元素的所有子节点
    QString parentTagName = xmlElement.tagName();
    for (QDomNode xmlNode = xmlElement.firstChild(); !xmlNode.isNull(); xmlNode = xmlNode.nextSibling())
    {
        // 判断节点类型
        if (xmlNode.isElement())
        {
            // 获取子元素
            QDomElement childElement = xmlNode.toElement();
            QString tagName = childElement.tagName();
            QString textValue = childElement.text();

            if (haveTypeDef(parentTagName,tagName)) //判断是否是需要特殊转换的节点
            {
                int nNodeType = getTypeDef(parentTagName,tagName);
                switch (nNodeType)
                {
                case Type_Array:
                {
                    if(!haveChild(childElement) && !haveAttr(childElement))
                    {
                        QJsonArray arrayItem;
                        if (jsonObject.contains(tagName)) //判断指定key是否存在,存在则转换为数组
                        {
                            arrayItem = jsonObject[tagName].toArray();
                            jsonObject.remove(tagName);
                        }

                        arrayItem.append(QJsonValue::fromVariant(textValue));
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = arrayItem;
                    }
                    else
                    {
                        QJsonArray arrayItem;
                        if (jsonObject.contains(tagName)) //判断指定key是否存在,存在则转换为数组
                        {
                            arrayItem = jsonObject[tagName].toArray();
                            jsonObject.remove(tagName);
                        }

                        QJsonObject childObject;
                        convertXmlElementToJsonObject(childElement, childObject);
                        addAttrToObject(childElement, childObject, m_bStringValue);// 添加所有属性到Object
                        if (!haveChild(childElement))// 添加叶子节点文本内容到QObject
                        {
                            if (childObject.contains("text"))
                                childObject.remove("text");
                            childObject["text"] = QJsonValue::fromVariant(textValue);
                        }

                        arrayItem.append(childObject);
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = arrayItem;
                    }

                    break;
                }
                case Type_Object:
                {
                    QJsonObject childObject;
                    convertXmlElementToJsonObject(childElement, childObject);
                    addAttrToObject(childElement, childObject, m_bStringValue);// 添加所有属性到Object

                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = childObject;

                    break;
                }
                case Type_Text:
                default:
                {
                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = QJsonValue::fromVariant(textValue);
                    break;
                }
                }

                continue;
            }

            // 判断子元素是否有子节点/属性
            if (!haveChild(childElement) && !haveAttr(childElement))
            {
                // 添加子元素的文本内容(添加到父QObject中)
                if (m_bStringValue)
                {
                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = QJsonValue::fromVariant(textValue);
                }
                else
                {
                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = QJsonValue::fromVariant(textValue);
                    bool isInt; int intValue = textValue.toInt(&isInt);
                    bool isDouble; double doubleValue = textValue.toDouble(&isDouble);
                    bool isBool = (textValue.toLower()=="true" || textValue.toLower()=="false");
                    if (isInt)
                    {
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = QJsonValue::fromVariant(intValue);
                    }
                    else if(isDouble)
                    {
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = QJsonValue::fromVariant(doubleValue);
                    }
                    else if(isBool)
                    {
                        bool boolValue = (textValue.toLower()=="true");
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = QJsonValue::fromVariant(boolValue);
                    }
                    else
                    {
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = QJsonValue::fromVariant(textValue);
                    }
                }
            }
            else
            {
                // 创建子对象、子数组
                QJsonObject childObject;
                QJsonArray ChildArray;

                if (!isNodeList(childElement))
                {
                    convertXmlElementToJsonObject(childElement, childObject);
                }
                else
                {
                    convertXmlElementToJsonArray(childElement, ChildArray);

                    // 将数组包装成Object (适配: 保留NodeList的子节点名作为数组名)
                    QString strArrayName = childElement.firstChild().toElement().tagName();
                    if (childObject.contains(strArrayName))
                        childObject.remove(strArrayName);
                    childObject[strArrayName] = ChildArray;
                }

                // 添加所有属性到Object
                addAttrToObject(childElement, childObject, m_bStringValue);

                // 添加叶子节点文本内容到QObject
                if (!haveChild(childElement))
                {
                    if (childObject.contains("text"))
                        childObject.remove("text");
                    childObject["text"] = textValue;
                }

                // 添加到父JSON对象中
                if (jsonObject.contains(tagName)) //判断指定key是否存在,存在则转换为Object数组
                {
                    if (!jsonObject[tagName].isArray())//未转换为数组则转换
                    {
                        QJsonObject objItemOld = jsonObject[tagName].toObject();

                        QJsonArray arrayItemOld;
                        arrayItemOld.append(objItemOld);
                        if (jsonObject.contains(tagName))
                            jsonObject.remove(tagName);
                        jsonObject[tagName] = arrayItemOld;
                    }

                    QJsonArray arrayItem = jsonObject[tagName].toArray();
                    arrayItem.append(childObject);
                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = arrayItem;
                }
                else
                {
                    if (jsonObject.contains(tagName))
                        jsonObject.remove(tagName);
                    jsonObject[tagName] = childObject;
                }
            }
        }
    }
}
void QConvertHelper::convertXmlElementToJsonArray(const QDomElement& xmlElement, QJsonArray& jsonArray)
{
    // 遍历XML元素的所有子节点
    for (QDomNode xmlNode = xmlElement.firstChild(); !xmlNode.isNull(); xmlNode = xmlNode.nextSibling())
    {
        // 判断节点类型
        if (xmlNode.isElement())
        {
            // 获取子元素
            QDomElement childElement = xmlNode.toElement();
            QString tagName = childElement.tagName();
            QString text = childElement.text();

            // 判断子元素是否有子节点/属性
            if (!haveChild(childElement) && !haveAttr(childElement))
            {
                // 添加子元素的文本内容(添加到父QObject中)
                if (m_bStringValue)
                {
                    jsonArray.append(QJsonValue::fromVariant(text));
                }
                else
                {
                    bool isInt; int intValue = text.toInt(&isInt);
                    bool isDouble; double doubleValue = text.toDouble(&isDouble);
                    bool isBool = (text.toLower()=="true" || text.toLower()=="false");
                    if (isInt)
                    {
                        jsonArray.append(QJsonValue::fromVariant(intValue));
                    }
                    else if(isDouble)
                    {
                        jsonArray.append(QJsonValue::fromVariant(doubleValue));
                    }
                    else if(isBool)
                    {
                        bool boolValue = (text.toLower()=="true");
                        jsonArray.append(QJsonValue::fromVariant(boolValue));
                    }
                    else
                    {
                        jsonArray.append(QJsonValue::fromVariant(text));
                    }
                }
            }
            else
            {
                // 创建子对象、子数组
                QJsonObject childObject;
                QJsonArray ChildArray;

                if (!isNodeList(childElement))
                {
                    convertXmlElementToJsonObject(childElement, childObject);
                }
                else
                {
                    convertXmlElementToJsonArray(childElement, ChildArray);

                    // 将数组包装成Object (适配: 保留NodeList的子节点名作为数组名)
                    QString strArrayName = childElement.firstChild().toElement().tagName();
                    if (childObject.contains(strArrayName))
                        childObject.remove(strArrayName);
                    childObject[strArrayName] = ChildArray;
                }

                // 获取子元素的所有属性
                QDomNamedNodeMap attributes = childElement.attributes();
                for (int i = 0; i < attributes.size(); ++i)
                {
                    QDomAttr attrNode = attributes.item(i).toAttr();
                    QString strAttrName = attrNode.name();
                    QString strAttrValue = attrNode.value();

                    // 将子元素的属性内容添加到JSON对象中(适配:节点属性作为子成员)
                    if (m_bStringValue)
                    {
                        if (childObject.contains(strAttrName))
                            childObject.remove(strAttrName);
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                    }
                    else
                    {
                        if (childObject.contains(strAttrName))
                            childObject.remove(strAttrName);
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        bool isIntAttr; int intValueAttr = strAttrValue.toInt(&isIntAttr);
                        bool isDoubleAttr; double doubleValueAttr = strAttrValue.toDouble(&isDoubleAttr);
                        bool isBoolAttr = (strAttrValue.toLower()=="true" || strAttrValue.toLower()=="false");
                        if (isIntAttr)
                        {
                            if (childObject.contains(strAttrName))
                                childObject.remove(strAttrName);
                            childObject[strAttrName] = QJsonValue::fromVariant(intValueAttr);
                        }
                        else if(isDoubleAttr)
                        {
                            if (childObject.contains(strAttrName))
                                childObject.remove(strAttrName);
                            childObject[strAttrName] = QJsonValue::fromVariant(doubleValueAttr);
                        }
                        else if(isBoolAttr)
                        {
                            bool boolValueAttr = (strAttrValue.toLower()=="true");
                            if (childObject.contains(strAttrName))
                                childObject.remove(strAttrName);
                            childObject[strAttrName] = QJsonValue::fromVariant(boolValueAttr);
                        }
                        else
                        {
                            if (childObject.contains(strAttrName))
                                childObject.remove(strAttrName);
                            childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        }
                    }
                }

                // 添加子元素的文本内容(添加到子QObject中)
                if (!haveChild(childElement))//叶子节点
                {
                    if (childObject.contains("text"))
                        childObject.remove("text");
                    childObject["text"] = text;
                }

                // 添加到父JSON数组中
                jsonArray.append(childObject);
            }
        }
    }
}
