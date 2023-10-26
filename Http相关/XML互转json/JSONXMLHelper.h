#pragma once  
#include "cJSON.h"  

#include <stdlib.h>    
#include <string.h>    
#include <stdio.h>    
#include <assert.h>    
#include <time.h>  
#include <iostream>    
using namespace std;

#include <iostream>
#include <string>
#include <set>
#include "cJSON.h"
#include "pugixml.hpp"


static void removeSubstring(std::string& str, const std::string& substr) 
{
    size_t pos = str.find(substr);
    while (pos != std::string::npos) {
        str.erase(pos, substr.length());
        pos = str.find(substr, pos);
    }
}

static bool checkValueNode(pugi::xml_node xmlNode, std::string& value)
{
    pugi::xml_node child = xmlNode.first_child();
    if (child.first_child() == nullptr && child.next_sibling() == nullptr && child.previous_sibling() == nullptr)//xmlNode节点的子节点是数值，<xmlNode>1</xmlNode>
    {
        value = child.value();
        return true;
    }

    return false;
}

static bool checkArrayNode(pugi::xml_node xmlNode)
{
    int broNodeCount = 0;
    std::set<std::string> vecBrotherName;

    pugi::xml_node nextNode = xmlNode.next_sibling();
    while (!nextNode.empty())
    {
        vecBrotherName.insert(nextNode.name());
        nextNode = nextNode.next_sibling();   //获取下一个兄弟节点
        ++broNodeCount;
    }
    pugi::xml_node prevNode = xmlNode.previous_sibling();
    while (!prevNode.empty())
    {
        vecBrotherName.insert(prevNode.name());
        prevNode = prevNode.previous_sibling();//获取上一个兄弟节点
        ++broNodeCount;
    }

    bool result = false;
    if ((broNodeCount > 0) && (vecBrotherName.size() == 1))//兄弟节点大于1且名称相同
        result = true;
    
    return result;
}

//string 参数  
static void Xml2Json_help(pugi::xml_node& xmlNode, cJSON* jsonNode)
{
    if (xmlNode.empty())
    {
        //无子节点==><xmlNode/>
        cJSON_AddStringToObject(jsonNode, xmlNode.name(), xmlNode.child_value());
    }
    else
    {
        int broNodeCount = 0;
        std::string value = "";

        if (checkValueNode(xmlNode, value))
        {
            cJSON_AddStringToObject(jsonNode, xmlNode.name(), value.c_str());
        }
        else if (checkArrayNode(xmlNode))
        {
            // Non-leaf node
            cJSON* arrayJson = cJSON_CreateArray();
            cJSON_AddItemToObject(jsonNode, xmlNode.name(), arrayJson);

            pugi::xml_node nextNode = xmlNode.parent().first_child();//第一个
            while (!nextNode.empty())
            {
                cJSON* arrayItemJson = cJSON_CreateObject();
                cJSON_AddItemToObject(arrayJson, nextNode.name(), arrayItemJson);//兄弟节点作为数组子项
                for (pugi::xml_node childNode : nextNode.children())
                {
                    Xml2Json_help(childNode, arrayItemJson);
                }
                nextNode = nextNode.next_sibling();   //获取下一个兄弟节点
                xmlNode = nextNode;
            }
        }
        else
        {
            // Non-leaf node
            cJSON* objectJson = cJSON_CreateObject();
            cJSON_AddItemToObject(jsonNode, xmlNode.name(), objectJson);

            //XML节点属性 作为 Object对象属性
            for (pugi::xml_attribute attr : xmlNode.attributes())
            {
                cJSON_AddStringToObject(objectJson, attr.name(), attr.value());
            }

            //XML节点子节点 递归进入函数
            //for (pugi::xml_node childNode : xmlNode.children())  //childNode可能跳过几个节点,不使用此写法遍历
            for (pugi::xml_node childNode = xmlNode.first_child(); childNode; childNode = childNode.next_sibling())
            {
                Xml2Json_help(childNode, objectJson);
            }
        }
    }
}

static std::string Xml2Json(const std::string& xmlString)
{
    pugi::xml_document doc;
    if (!doc.load_string(xmlString.c_str())) 
    {
        return "Failed to parse XML string";
    }

    cJSON* rootJsonNode = cJSON_CreateObject();
    Xml2Json_help(doc.first_child(), rootJsonNode);

    char* jsonString = cJSON_Print(rootJsonNode);
    std::string result(jsonString);

    cJSON_Delete(rootJsonNode);
    free(jsonString);

    return result;
}

static void Json2Xml_help(cJSON* jsonNode, pugi::xml_node xmlNode)
{
    if (jsonNode->type == cJSON_Object) {
        cJSON* child = jsonNode->child;
        while (child != nullptr) {
            if (child->type == cJSON_Array)
            {
                //pugi::xml_node childNode = xmlNode.append_child(child->string);
                Json2Xml_help(child, xmlNode);
                child = child->next;
            }
            else
            {
                pugi::xml_node childNode = xmlNode.append_child(child->string);
                Json2Xml_help(child, childNode);
                child = child->next;
            }   
        }
    }
    else if (jsonNode->type == cJSON_Array) {
        cJSON* child = jsonNode->child;
        while (child != nullptr) {
            pugi::xml_node childNode = xmlNode.append_child(jsonNode->string);
            Json2Xml_help(child, childNode);
            child = child->next;
        }
    }
    else if (jsonNode->type == cJSON_String) {
        xmlNode.text().set(jsonNode->valuestring);
    }
    else if (jsonNode->type == cJSON_True) {
        xmlNode.text().set("true");
    }
    else if (jsonNode->type == cJSON_False) {
        xmlNode.text().set("false");
    }
    else if (jsonNode->type == cJSON_Number) {
        xmlNode.text().set(std::to_string(jsonNode->valuedouble).c_str());
    }
}

static std::string Json2Xml(const std::string& jsonString)
{
    cJSON* root = cJSON_Parse(jsonString.c_str());
    if (root == nullptr) 
    {
        return "Failed to parse JSON string";
    }

    pugi::xml_document doc;
    pugi::xml_node rootNode = doc.append_child("root_53908f03dd433d55");//根节点，对应json最外层的｛｝
    Json2Xml_help(root, rootNode);
    cJSON_Delete(root);

    std::ostringstream oss;
    doc.save(oss, "", pugi::format_raw | pugi::format_no_declaration); //no_declaration 表示不添加 <?xml version="1.0">
    std::string result = oss.str();

    //自定义:根节点不需要
    removeSubstring(result, std::string("<root_53908f03dd433d55>"));
    removeSubstring(result, std::string("</root_53908f03dd433d55>"));

    return result;
}

//CString 参数
static CString Xml2JsonEx(CString strXml)
{
    std::string sXml = (CT2A)strXml;
    std::string sJson = Xml2Json(sXml);

    CString strJson;   
    strJson = sJson.c_str();
    return strJson;
}

static CString Json2XmlEx(CString strJson)
{
    std::string sJson = (CT2A)(strJson);
    std::string sXml = Json2Xml(sJson);

    CString strXml;
    strXml = sXml.c_str();
    return strXml;
}

