
////////////////////////////////////////////////////////////////////////////////////
void convertJsonArrayToXmlElement(const QJsonArray& jsonArray, QDomDocument& xmlDoc, QDomNode& xmlNode);
void convertJsonObjectToXmlElement(const QJsonObject& jsonObject, QDomDocument& xmlDoc, QDomNode& xmlNode);
void convertXmlElementToJsonObject(const QDomElement& xmlElement, QJsonObject& jsonObject);
void convertXmlElementToJsonArray(const QDomElement& xmlElement, QJsonArray& jsonArray);
//
void convertJsonObjectToXmlElement(const QJsonObject& jsonObject, QDomDocument& xmlDoc, QDomNode& xmlNode)
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
void convertJsonArrayToXmlElement(const QJsonArray& jsonArray, QDomDocument& xmlDoc, QDomNode& xmlNode)
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
bool jsonToXml(const QString& jsonString, QString& xmlString)
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

bool haveChild(const QDomElement& xmlElement)
{
    return (!xmlElement.firstChildElement().isNull());
}
bool haveAttr(const QDomElement& xmlElement)
{
    QDomNamedNodeMap attributes = xmlElement.attributes();
    return (attributes.size()>0);
}
bool isNodeList(const QDomElement& xmlElement)
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

bool g_bStringValue = false;//所有json节点都用字符串类型
QStringList g_keepArrayNodeList;//需要转换为数组的节点
void convertXmlElementToJsonObject(const QDomElement& xmlElement, QJsonObject& jsonObject)
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
                if (g_bStringValue)
                {
                    jsonObject[tagName] = QJsonValue::fromVariant(text);
                }
                else
                {
                    jsonObject[tagName] = QJsonValue::fromVariant(text);
                    bool isInt; int intValue = text.toInt(&isInt);
                    bool isDouble; double doubleValue = text.toDouble(&isDouble);
                    bool isBool = (text.toLower()=="true" || text.toLower()=="false");
                    if (isInt)
                    {
                        jsonObject[tagName] = QJsonValue(intValue);
                    }
                    else if(isDouble)
                    {
                        jsonObject[tagName] = QJsonValue(doubleValue);
                    }
                    else if(isBool)
                    {
                        bool boolValue = (text.toLower()=="true");
                        jsonObject[tagName] = QJsonValue(boolValue);
                    }
                    else
                    {
                        jsonObject[tagName] = QJsonValue::fromVariant(text);
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
                    childObject[strArrayName] = QJsonValue(ChildArray);
                }

                // 获取子元素的所有属性
                QDomNamedNodeMap attributes = childElement.attributes();
                for (int i = 0; i < attributes.size(); ++i)
                {
                    QDomAttr attrNode = attributes.item(i).toAttr();
                    QString strAttrName = attrNode.name();
                    QString strAttrValue = attrNode.value();

                    // 将子元素的属性内容添加到JSON对象中(适配:节点属性作为子成员)
                    if (g_bStringValue)
                    {
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                    }
                    else
                    {
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        bool isIntAttr; int intValueAttr = strAttrValue.toInt(&isIntAttr);
                        bool isDoubleAttr; double doubleValueAttr = strAttrValue.toDouble(&isDoubleAttr);
                        bool isBoolAttr = (strAttrValue.toLower()=="true" || strAttrValue.toLower()=="false");
                        if (isIntAttr)
                        {
                            childObject[strAttrName] = QJsonValue(intValueAttr);
                        }
                        else if(isDoubleAttr)
                        {
                            childObject[strAttrName] = QJsonValue(doubleValueAttr);
                        }
                        else if(isBoolAttr)
                        {
                            bool boolValueAttr = (strAttrValue.toLower()=="true");
                            childObject[strAttrName] = QJsonValue(boolValueAttr);
                        }
                        else
                        {
                            childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        }
                    }
                }

                // 添加子元素的文本内容(添加到子QObject中)
                if (!haveChild(childElement))//叶子节点
                    childObject["text"] = text;

                // 添加到父JSON对象中
                if (jsonObject.contains(tagName)) //判断指定key是否存在,存在则转换为Object数组
                {
                    if (!jsonObject[tagName].isArray())//未转换为数组则转换
                    {
                        QJsonObject objItemOld = jsonObject[tagName].toObject();
                        jsonObject.remove(tagName);

                        QJsonArray arrayItemOld;
                        arrayItemOld.append(objItemOld);
                        jsonObject[tagName] = QJsonValue(arrayItemOld);
                    }

                    QJsonArray arrayItem = jsonObject[tagName].toArray();
                    arrayItem.append(childObject);
                    jsonObject[tagName] = QJsonValue(arrayItem);
                }
                else if (g_keepArrayNodeList.contains(tagName)) //判断是否是需要转换为对象数组的节点
                {
                    QJsonArray arrayItem;
                    arrayItem.append(childObject);
                    jsonObject[tagName] = QJsonValue(arrayItem);
                }
                else
                {
                    jsonObject[tagName] = QJsonValue(childObject);
                }
            }
        }
    }
}
void convertXmlElementToJsonArray(const QDomElement& xmlElement, QJsonArray& jsonArray)
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
                if (g_bStringValue)
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
                        jsonArray.append(QJsonValue(intValue));
                    }
                    else if(isDouble)
                    {
                        jsonArray.append(QJsonValue(doubleValue));
                    }
                    else if(isBool)
                    {
                        bool boolValue = (text.toLower()=="true");
                        jsonArray.append(QJsonValue(boolValue));
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
                    childObject[strArrayName] = QJsonValue(ChildArray);
                }

                // 获取子元素的所有属性
                QDomNamedNodeMap attributes = childElement.attributes();
                for (int i = 0; i < attributes.size(); ++i)
                {
                    QDomAttr attrNode = attributes.item(i).toAttr();
                    QString strAttrName = attrNode.name();
                    QString strAttrValue = attrNode.value();

                    // 将子元素的属性内容添加到JSON对象中(适配:节点属性作为子成员)
                    if (g_bStringValue)
                    {
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                    }
                    else
                    {
                        childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        bool isIntAttr; int intValueAttr = strAttrValue.toInt(&isIntAttr);
                        bool isDoubleAttr; double doubleValueAttr = strAttrValue.toDouble(&isDoubleAttr);
                        bool isBoolAttr = (strAttrValue.toLower()=="true" || strAttrValue.toLower()=="false");
                        if (isIntAttr)
                        {
                            childObject[strAttrName] = QJsonValue(intValueAttr);
                        }
                        else if(isDoubleAttr)
                        {
                            childObject[strAttrName] = QJsonValue(doubleValueAttr);
                        }
                        else if(isBoolAttr)
                        {
                            bool boolValueAttr = (strAttrValue.toLower()=="true");
                            childObject[strAttrName] = QJsonValue(boolValueAttr);
                        }
                        else
                        {
                            childObject[strAttrName] = QJsonValue::fromVariant(strAttrValue);
                        }
                    }
                }

                // 添加子元素的文本内容(添加到子QObject中)
                if (!haveChild(childElement))//叶子节点
                    childObject["text"] = text;

                // 添加到父JSON数组中
                jsonArray.append(QJsonValue(childObject));
            }
        }
    }
}
bool xmlToJson(const QString& xmlString, QString& jsonString, bool bStringValue, QStringList keepArrayNodeList)
{
    g_bStringValue = bStringValue;
    g_keepArrayNodeList = keepArrayNodeList;

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
        jsonObjectEx[strFirstObjName] = QJsonValue(jsonObject);
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
