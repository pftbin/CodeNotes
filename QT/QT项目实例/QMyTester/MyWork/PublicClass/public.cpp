#include "public.h"

////////////////////////////////////////////////////////////////////////////////////
QString RestoreESCString(const QString& escString)
{
    QString result = escString;
    result.replace(QString("&amp;"),QString("&"));
    result.replace(QString("&lt;"),QString("<"));
    result.replace(QString("&gt;"),QString(">"));
    result.replace(QString("&quot;"),QString("\""));
    result.replace(QString("&apos;"),QString("\'"));
    result.replace(QString("&nbsp;"),QString(" "));

    return result;
}
QString CovertToESCString(const QString& beConvertString)
{
    QString result = beConvertString;
    result.replace(QString("&"),QString("&amp;"));
    result.replace(QString("<"),QString("&lt;"));
    result.replace(QString(">"),QString("&gt;"));
    result.replace(QString("\""),QString("&quot;"));
    result.replace(QString("\'"),QString("&apos;"));
    result.replace(QString(" "),QString("&nbsp;"));

    return result;
}
QString ConvertToQString(const QVariant &variant)
{
    switch(variant.typeId())
    {
    case QMetaType::QString:
        return variant.toString().trimmed();
    case QMetaType::Int:
    case QMetaType::UInt:
        return QString::number(variant.toInt());
    case QMetaType::Double:
        return QString::number(variant.toDouble());
    // Add more cases here for other types that you want to convert
    default:
        return variant.toString().trimmed();
    }
}

std::string QStringToStdString(QString& qString)
{
    return qString.toStdString();
}
QString StdStringToQString(std::string& stdString)
{
    return QString::fromStdString(stdString);
}

int MatchQStringListIdx(QStringList& stringList, QString& qString)
{
    int nMatchIndex = -1;
    for (int i = 0; i < stringList.size(); i++)
    {
        if (stringList[i] == qString)
        {
            nMatchIndex = i;
            break;
        }
    }

    return nMatchIndex;
}

////////////////////////////////////////////////////////////////////////////////////
bool isFolderExists(const QString& path)
{
    QDir dir(path);
    return dir.exists();
}
bool isFileExists(const QString& path)
{
    QFile file(path);
    return file.exists();
}
bool PathFileExists(const QString& path)
{
    QDir dir(path);
    QFile file(path);
    return (dir.exists() || file.exists());
}
QString openSelectFileDlg(QString strTitle, const QStringList &filterList)
{
    QString filter;
    for (const QString &fileType : filterList)
    {
        filter += fileType + ";;";
    }
    filter.chop(2); // 去除最后的两个分号

    return QFileDialog::getOpenFileName(nullptr, strTitle, "", filter);
}
QString openSaveFileDlg(QString defaultSuffix)
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    // 设置默认后缀
    dialog.setDefaultSuffix(defaultSuffix);

    // 设置默认的文件名和保存路径
    //dialog.selectFile("example.txt");
    //dialog.setDirectory("/path/to/save");

    return dialog.getSaveFileName();
}
QString openSelectFolderDialog(QString strTitle)
{
    QString folderPath = QFileDialog::getExistingDirectory(nullptr, strTitle, QDir::homePath());
    return folderPath;
}
bool saveStringToFile(const QString& filePath, const QString& content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream textStream(&file);
    textStream.setEncoding(QStringConverter::Encoding::Utf8); // 设置编码为UTF-8
    textStream << content;

    file.close();
    return true;
}
bool openSelectColorDialog(QColor& color)
{
    QColorDialog dialog;
    dialog.setCurrentColor(color);
    if (dialog.exec())
    {
        color = dialog.selectedColor();
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////
int AfxMessageBox(QString strTitle, QString strMessage, unsigned int uID, bool bChinese)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(strTitle);
    msgBox.setText(strMessage);
    msgBox.setIcon(QMessageBox::Question); // 问题图标

    switch (uID)
    {
        case MB_RETRYCANCEL:
        {
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Retry)->setText(("重试"));
                msgBox.button(QMessageBox::Cancel)->setText(("取消"));
            }
        }
        break;
        case MB_YESNO:
        {
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Yes)->setText(("是"));
                msgBox.button(QMessageBox::No)->setText(("否"));
            }
        }
        break;
        case MB_YESNOCANCEL:
        {
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Cancel)->setText(("取消"));
                msgBox.button(QMessageBox::Yes)->setText(("是"));
                msgBox.button(QMessageBox::No)->setText(("否"));
            }
        }
        break;
        case MB_ABORTRETRYIGNORE:
        {
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Abort | QMessageBox::Ignore);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Retry)->setText(("重试"));
                msgBox.button(QMessageBox::Abort)->setText(("中止"));
                msgBox.button(QMessageBox::Ignore)->setText(("忽略"));
            }
        }
        break;
        case MB_OKCANCEL:
        {
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Ok)->setText(("确认"));
                msgBox.button(QMessageBox::Cancel)->setText(("取消"));
            }
        }
        break;
        case MB_OK:
        default:
        {
            msgBox.setStandardButtons(QMessageBox::Ok);
            if (bChinese)
            {
                msgBox.button(QMessageBox::Ok)->setText(("确认"));
            }
        }
        break;
    }

    return msgBox.exec();
}
int AfxWarningBox(QString strTitle, QString strMessage, bool bChinese)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(strTitle);
    msgBox.setText(strMessage);
    msgBox.setIcon(QMessageBox::Warning); // 警告图标
    msgBox.setStandardButtons(QMessageBox::Ok);
    if (bChinese)
    {
        msgBox.button(QMessageBox::Ok)->setText(("确认"));
    }

    return msgBox.exec();
}
int AfxErrorBox(QString strTitle, QString strMessage, bool bChinese)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(strTitle);
    msgBox.setText(strMessage);
    msgBox.setIcon(QMessageBox::Critical); // 错误图标
    msgBox.setStandardButtons(QMessageBox::Ok);
    if (bChinese)
    {
        msgBox.button(QMessageBox::Ok)->setText(("确认"));
    }

    return msgBox.exec();
}

////////////////////////////////////////////////////////////////////////////////////
qint64 getTickCount()
{
    return QDateTime::currentMSecsSinceEpoch();
}
QString getCurrentTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.toString("yyyy-MM-dd HH:mm:ss");
}

////////////////////////////////////////////////////////////////////////////////////
QString getRandomString(int maxLen)
{
    QRandomGenerator random(QRandomGenerator::global()->generate());
    QString result;
    int length = random.bounded(maxLen) + 1; // 随机生成字符串长度，范围为1-maxLen
    for (int i = 0; i < length; ++i)
    {
        QChar randomChar = QChar('a' + random.bounded('z' - 'a' + 1)); // 生成随机字符，范围为a-z
        if (random.bounded(2) == 0)
        {
            randomChar = randomChar.toUpper(); // 50%的概率将字符转换为大写
        }
        result.append(randomChar);
    }

    return result;
}
int getRandomInt(int maxValue)
{
    QRandomGenerator random(QRandomGenerator::global()->generate());
    int randomNumber = random.bounded(maxValue+1); // 生成不超过maxValue的随机数

    return randomNumber;
}
double getRandomDouble(int maxvalue)
{
    QRandomGenerator random(QRandomGenerator::global()->generate());
    int randomNumber = random.bounded(maxvalue+1); // 生成不超过maxValue的随机数
    double randomFloat = randomNumber / 10.0; // 将整数转换为浮点数,一位小数

    return randomFloat;
}

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

bool isNodeList(const QDomElement& xmlElement)
{
    //判断子节点名称
    QSet<QString> setNodeName;
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
        }
    }

    return (setNodeName.size()==1);
}
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
            // 获取子元素的标签名
            QString tagName = childElement.tagName();

            // 判断子元素是否有子节点
            if (childElement.firstChildElement().isNull())
            {
                // 获取子元素的文本内容
                QString text = childElement.text();
                // 将子元素的文本内容添加到JSON对象中
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
            else
            {
                // 创建子对象、子数组
                QJsonObject childObject;
                QJsonArray ChildArray;

                if (!isNodeList(childElement))
                {
                    convertXmlElementToJsonObject(childElement, childObject);
                    // 添加到父JSON对象中
                    jsonObject[tagName] = QJsonValue(childObject);
                }
                else
                {
                    convertXmlElementToJsonArray(childElement, ChildArray);
                    // 添加到父JSON对象中
                    jsonObject[tagName] = QJsonValue(ChildArray);
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
            // 获取子元素的标签名
            QString tagName = childElement.tagName();

            // 判断子元素是否有子节点
            if (childElement.firstChildElement().isNull())
            {
                // 获取子元素的文本内容
                QString text = childElement.text();
                // 将子元素的文本内容添加到JSON数组中
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
            else
            {
                // 创建子对象、子数组
                QJsonObject childObject;
                QJsonArray ChildArray;

                if (!isNodeList(childElement))
                {
                    convertXmlElementToJsonObject(childElement, childObject);
                    // 添加到父JSON数组中
                    jsonArray.append(QJsonValue(childObject));
                }
                else
                {
                    convertXmlElementToJsonArray(childElement, ChildArray);
                    // 添加到父JSON数组中
                    jsonArray.append(QJsonValue(ChildArray));
                }

            }
        }
    }
}
bool xmlToJson(const QString& xmlString, QString& jsonString)
{
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
        QJsonDocument jsonDoc(jsonObject);
        jsonString = jsonDoc.toJson(QJsonDocument::Indented);
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



