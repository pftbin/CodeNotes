#include "QJsonXmlTester.h"
#include "ui_QJsonXmlTester.h"

#include "public.h"
#include <QMessageBox>

QJsonXmlTester::QJsonXmlTester(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

QJsonXmlTester::~QJsonXmlTester()
{
    delete ui;
}

QJsonObject QJsonXmlTester::CreateRandomJson(bool isRoot)
{
    QJsonObject ParentObject = m_jsonHelper.createJsonObject();
    int typeMaxVale = 3;//非根节点不生成object、array
    if (isRoot)
    {
        typeMaxVale = 5;
    }

    int child = getRandomInt(10);//随机子节点个数
    for (int i = 0; i < child; i++)
    {
        int type = getRandomInt(typeMaxVale);//随机节点类型
        switch(type)
        {
        case 0:
        {
            //int
            m_jsonHelper.addValueToObject(ParentObject, getRandomString(5), QJsonValue(getRandomInt(5)));
            break;
        }
        case 1:
        {
            //double
            m_jsonHelper.addValueToObject(ParentObject, getRandomString(5), QJsonValue(getRandomDouble(5)));
            break;
        }
        case 2:
        {
            //string
            m_jsonHelper.addValueToObject(ParentObject, getRandomString(5), QJsonValue(getRandomString(20)));
            break;
        }
        case 3:
        {
            //bool
            bool boolvalue = (i%2==0)?true:false;
            m_jsonHelper.addValueToObject(ParentObject, getRandomString(5), QJsonValue(boolvalue));
            break;
        }
        case 4:
        {
            //array
            QJsonArray childArray = m_jsonHelper.createJsonArray();
            int arrayCount = getRandomInt(5);
            for(int j = 0; j < arrayCount; j++)
            {
                if (i%2==0)
                    m_jsonHelper.addValueToArray(childArray, QJsonValue(getRandomString(10)));
                else if (i%3==0)
                    m_jsonHelper.addValueToArray(childArray, QJsonValue(getRandomInt(100)));
                else
                    m_jsonHelper.addValueToArray(childArray, QJsonValue(getRandomDouble(100)));
            }
            m_jsonHelper.addValueToObject(ParentObject, QString("childArray_%1").arg(getRandomInt(50)), QJsonValue(childArray));
            break;
        }
        default:
        {
            //object
            QJsonObject childObject = CreateRandomJson(false);
            m_jsonHelper.addValueToObject(ParentObject, QString("childObject_%1").arg(getRandomInt(50)), QJsonValue(childObject));
            break;
        }
        }
    }

    return ParentObject;
}

QDomDocument QJsonXmlTester::CreateRandomXml(bool isRoot)
{
    QDomDocument ParentDocument = m_xmlHelper.createXmlDocument();
    QDomElement rootElem = ParentDocument.createElement(QString("root"));
    QDomElement* pRootElem = nullptr;

    int typeMaxVale = 1;//非根节点不生成nodelist、mutilnode
    if (isRoot)
    {
        typeMaxVale = 3;
        pRootElem = &rootElem;
    }

    int childCount = getRandomInt(10);//随机子节点个数
    for (int i = 0; i < childCount; i++)
    {
        int type = getRandomInt(typeMaxVale);//随机节点类型
        switch(type)
        {
        case 0:
        {
            //node not attrible
            m_xmlHelper.addNode(ParentDocument, pRootElem, QString(getRandomString(5)),QString(getRandomString(10)));
            break;
        }
        case 1:
        {
            //node have attrible
            QDomElement chidElem = m_xmlHelper.addNode(ParentDocument, pRootElem, QString(getRandomString(5)),QString(getRandomString(10)));
            m_xmlHelper.addAttribute(chidElem, QString(getRandomString(5)),QString(getRandomString(10)));
            break;
        }
        case 2:
        {
            //nodelist
            QDomElement childListElem = ParentDocument.createElement(QString("nodeList_%1").arg(getRandomInt(50)));
            QStringList childListValues;
            int elemCount = getRandomInt(5);
            for(int j = 0; j < elemCount; j++)
            {
                if (i%2==0)
                    childListValues.append(getRandomString(5));
                else if (i%3==0)
                    childListValues.append(QString::number(getRandomInt(100)));
                else
                    childListValues.append(QString::number(getRandomDouble(100)));
            }
            m_xmlHelper.addNodeList(ParentDocument, pRootElem, childListElem.tagName(), QString("item"), childListValues);
            break;
        }
        default:
        {
            //mutilnode
            QDomDocument mutilNode = CreateRandomXml(false);
            m_xmlHelper.addNode(ParentDocument, pRootElem, QString("mutilNode_%1").arg(getRandomInt(50)), m_xmlHelper.saveXmlToString(mutilNode));
            break;
        }
        }
    }

    return ParentDocument;
}

void QJsonXmlTester::on_m_btnCreateJson_clicked()
{
    QJsonObject randomJsonObj = CreateRandomJson();
    QString strRandomJson = m_jsonHelper.saveToString(randomJsonObj);

    ui->m_editJson->setText(strRandomJson);
}

void QJsonXmlTester::on_m_btnCreateXml_clicked()
{
    QDomDocument randomXmlDoc = CreateRandomXml();
    QString strRandomXml = m_xmlHelper.saveXmlToString(randomXmlDoc);

    //字符被转义,需恢复
    strRandomXml = RestoreESCString(strRandomXml);
    ui->m_editXml->setText(strRandomXml);
}

void QJsonXmlTester::on_m_btnSaveJson_clicked()
{
    QString strJson = ui->m_editJson->toPlainText();
    QString strFilePath = openSaveFileDlg();
    if (strFilePath.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "文件路径为空...");
        message.exec();
        return;
    }

    if (saveStringToFile(strFilePath,strJson))
    {
        QMessageBox message(QMessageBox::Information, "信息", "保存成功");
        message.exec();
        return;
    }
    else
    {
        QMessageBox message(QMessageBox::Critical, "错误", "保存失败...");
        message.exec();
        return;
    }
}

void QJsonXmlTester::on_m_btnSaveXml_clicked()
{
    QString strXml = ui->m_editXml->toPlainText();
    QString strFilePath = openSaveFileDlg();
    if (strFilePath.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "文件路径为空...");
        message.exec();
        return;
    }

    if (saveStringToFile(strFilePath,strXml))
    {
        QMessageBox message(QMessageBox::Information, "信息", "保存成功");
        message.exec();
        return;
    }
    else
    {
        QMessageBox message(QMessageBox::Critical, "错误", "保存失败...");
        message.exec();
        return;
    }
}

void QJsonXmlTester::on_m_btnJson2Xml_clicked()
{
    QString strJson,strXml;
    strJson = ui->m_editJson->toPlainText();
    if(strJson.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "Json内容为空...");
        message.exec();
        return;
    }


    if (!jsonToXml(strJson, strXml))
    {
        QMessageBox message(QMessageBox::Critical, "错误", "Json转Xml失败...");
        message.exec();
        return;
    }
    else
    {
        ui->m_editXml->setText(strXml);
    }
}

void QJsonXmlTester::on_m_btnXml2Json_clicked()
{
    QString strJson,strXml;
    strXml = ui->m_editXml->toPlainText();
    if(strXml.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "Xml内容为空...");
        message.exec();
        return;
    }

    if (!xmlToJson(strXml,strJson))
    {
        QMessageBox message(QMessageBox::Critical, "错误", "Xml转Json失败...");
        message.exec();
        return;
    }
    else
    {
        ui->m_editJson->setText(strJson);
    }
}

void QJsonXmlTester::on_m_btnParseJson_clicked()
{
    QString strJson = ui->m_editJson->toPlainText();
    //解析json测试
    QJsonObject nowJsonObj;
    if (!m_jsonHelper.loadStringToObject(strJson, nowJsonObj)|| nowJsonObj.isEmpty())
    {
        QMessageBox message(QMessageBox::Critical, "错误", "不符合json格式,解析失败...");
        message.exec();
        return;
    }
    else
    {
        //解析Json
        QString strArray,strObject,strString,strDouble,strInt,strBool,strUndefined;

        // 遍历所有的键
        QList<QString> keys = nowJsonObj.keys();
        foreach (const QString& key, keys)
        {
            QJsonValue value = nowJsonObj.value(key);
            if (value.isArray()) {
                QJsonArray childArray = value.toArray();
                // 处理数组
                strArray += key;
                strArray += QString("=%1个子元素").arg(childArray.size());
                for(int i = 0; i < childArray.size(); i++)
                {
                    QJsonValue childJsonValue = childArray.at(i);
                    int childName = i+1;
                    //不考虑array、object
                    if (childJsonValue.isString())
                        strArray += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toString());
                    if (childJsonValue.isDouble())
                        strArray += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toDouble());
                    if (childJsonValue.isDouble() && qFuzzyCompare(childJsonValue.toDouble(), qFloor(childJsonValue.toDouble())))
                        strArray += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toInt());
                    if (childJsonValue.isBool())
                        strArray += QString("\n\t%1 : %2").arg(childName).arg((childJsonValue.toBool())?("true"):("false"));
                }
                strArray += QString("\n");
            }
            else if (value.isObject()) {
                QJsonObject childObject = value.toObject();
                // 处理子对象
                strObject += key;
                strObject += QString("=%1个子对象").arg(childObject.size());
                QList<QString> childKeys = childObject.keys();
                foreach (const QString& childKey, childKeys)
                {
                    QJsonValue childJsonValue = childObject.value(childKey);
                    QString childName = childKey;
                    //不考虑array、object
                    if (childJsonValue.isString())
                        strObject += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toString());
                    if (childJsonValue.isDouble())
                        strObject += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toDouble());
                    if (childJsonValue.isDouble() && qFuzzyCompare(childJsonValue.toDouble(), qFloor(childJsonValue.toDouble())))
                        strObject += QString("\n\t%1 : %2").arg(childName).arg(childJsonValue.toInt());
                    if (childJsonValue.isBool())
                        strObject += QString("\n\t%1 : %2").arg(childName).arg((childJsonValue.toBool())?("true"):("false"));
                }
                strObject += QString("\n");
            }
            else if (value.isString()) {
                QString stringValue = value.toString();
                // 处理字符串值
                strString += key;
                strString += QString("=%1").arg(stringValue);
                strString += QString("\n");
            }
            else if (value.isDouble())
            {
                // 处理数字值
                double doubleValue = value.toDouble();
                if (!qFuzzyCompare(doubleValue, qFloor(doubleValue)))// 判断是否为整数
                {
                    strDouble += key;
                    strDouble += QString("=%1").arg(doubleValue);
                    strDouble += QString("\n");
                }
                else
                {
                    int intValue = value.toInt();
                    strInt += key;
                    strInt += QString("=%1").arg(intValue);
                    strInt += QString("\n");
                }
            }
            else if (value.isBool()) {
                bool boolValue = value.toBool();
                // 处理布尔值
                strBool += key;
                strBool += QString("=%1").arg((boolValue)?("true"):("false"));
                strBool += QString("\n");
            }
            else {
                // 处理其他类型的值
                strArray += key;
                strArray += QString("\n");
            }
        }

        QString strAfterParse = QString("Json解析:\n");
        strAfterParse += QString("数组：\n%1\n").arg(strArray);
        strAfterParse += QString("对象：\n%1\n").arg(strObject);
        strAfterParse += QString("字符串：\n%1\n").arg(strString);
        strAfterParse += QString("浮点数：\n%1\n").arg(strDouble);
        strAfterParse += QString("整数值：\n%1\n").arg(strInt);
        strAfterParse += QString("布尔值：\n%1\n").arg(strBool);
        strAfterParse += QString("其他：\n%1\n").arg(strUndefined);

        strJson += QString("\n==============================\n");
        strJson += strAfterParse;
        ui->m_editJson->setText(strJson);
    }
}

void QJsonXmlTester::on_m_btnParseXml_clicked()
{
    QString strXml = ui->m_editXml->toPlainText();
    //解析Xml测试
    QDomDocument  nowXmlDoc;
    if (!m_xmlHelper.loadXmlFromString(strXml, nowXmlDoc))
    {
        QMessageBox message(QMessageBox::Critical, "错误", "不符合xml格式,解析失败...");
        message.exec();
        return;
    }
    else
    {

        //解析Json
        QString strNode,strNodeWithAttr,strNodeList,strMutilNode;

        // 遍历XML元素的所有子节点
        for (QDomNode xmlNode = nowXmlDoc.firstChild(); !xmlNode.isNull(); xmlNode = xmlNode.nextSibling())
        {
            // 判断节点类型
            if (xmlNode.isElement())
            {
                // 获取子元素
                QDomElement childElement = xmlNode.toElement();
                // 获取子元素的标签名
                QString tagName = childElement.tagName();
                // 获取子元素的文本内容
                QString text = childElement.text();

                // 判断子元素是否有子节点
                if (childElement.firstChildElement().isNull())
                {
                    if(!childElement.hasAttributes())
                    {
                        strNode += tagName;
                        strNode += QString("=%1").arg(text);
                        strNode += QString("\n");
                    }
                    else
                    {
                        strNodeWithAttr += tagName;
                        strNodeWithAttr += QString("=%1").arg(text);
                        strNodeWithAttr += QString(",%1个属性").arg(childElement.attributes().size());
                        for(int i = 0; i < childElement.attributes().size(); i++)
                        {
                            strNodeWithAttr += QString("\n\t%1 : %2").arg(childElement.attributes().item(i).nodeName()).arg(childElement.attributes().item(i).nodeValue());
                        }
                        strNodeWithAttr += QString("\n");
                    }
                }
                else
                {
                    if (!isNodeList(childElement))
                    {
                        strNodeList += tagName;
                        strNodeList += QString("=%1个Item节点").arg(childElement.childNodes().size());
                        for(int i = 0; i < childElement.childNodes().size(); i++)
                        {
                            strNodeList += QString("\n\t%1 : %2").arg(childElement.childNodes().item(i).nodeName()).arg(childElement.childNodes().item(i).nodeValue());
                        }
                        strNodeList += QString("\n");
                    }
                    else
                    {
                        strMutilNode += tagName;
                        strMutilNode += QString("=%1个子节点").arg(childElement.childNodes().size());
                        for(int i = 0; i < childElement.childNodes().size(); i++)
                        {
                            strMutilNode += QString("\n\t%1 : %2").arg(childElement.childNodes().item(i).nodeName()).arg(childElement.childNodes().item(i).nodeValue());
                        }
                        strMutilNode += QString("\n");
                    }
                }
            }
        }

        QString strAfterParse = QString("Xml解析:\n");
        strAfterParse += QString("单节点：\n%1\n").arg(strNode);
        strAfterParse += QString("带属性节点：\n%1\n").arg(strNodeWithAttr);
        strAfterParse += QString("节点列表：\n%1\n").arg(strNodeList);
        strAfterParse += QString("多子节点：\n%1\n").arg(strMutilNode);

        strXml += QString("\n==============================\n");
        strXml += strAfterParse;
        ui->m_editXml->setText(strXml);
    }
}

