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
    if (child.first_child() == nullptr && child.next_sibling() == nullptr && child.previous_sibling() == nullptr)//xmlNode�ڵ���ӽڵ�����ֵ��<xmlNode>1</xmlNode>
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
        nextNode = nextNode.next_sibling();   //��ȡ��һ���ֵܽڵ�
        ++broNodeCount;
    }
    pugi::xml_node prevNode = xmlNode.previous_sibling();
    while (!prevNode.empty())
    {
        vecBrotherName.insert(prevNode.name());
        prevNode = prevNode.previous_sibling();//��ȡ��һ���ֵܽڵ�
        ++broNodeCount;
    }

    bool result = false;
    if ((broNodeCount > 0) && (vecBrotherName.size() == 1))//�ֵܽڵ����1��������ͬ
        result = true;
    
    return result;
}

//string ����  
static void Xml2Json_help(pugi::xml_node& xmlNode, cJSON* jsonNode)
{
    if (xmlNode.empty())
    {
        //���ӽڵ�==><xmlNode/>
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

            pugi::xml_node nextNode = xmlNode.parent().first_child();//��һ��
            while (!nextNode.empty())
            {
                cJSON* arrayItemJson = cJSON_CreateObject();
                cJSON_AddItemToObject(arrayJson, nextNode.name(), arrayItemJson);//�ֵܽڵ���Ϊ��������
                for (pugi::xml_node childNode : nextNode.children())
                {
                    Xml2Json_help(childNode, arrayItemJson);
                }
                nextNode = nextNode.next_sibling();   //��ȡ��һ���ֵܽڵ�
                xmlNode = nextNode;
            }
        }
        else
        {
            // Non-leaf node
            cJSON* objectJson = cJSON_CreateObject();
            cJSON_AddItemToObject(jsonNode, xmlNode.name(), objectJson);

            //XML�ڵ����� ��Ϊ Object��������
            for (pugi::xml_attribute attr : xmlNode.attributes())
            {
                cJSON_AddStringToObject(objectJson, attr.name(), attr.value());
            }

            //XML�ڵ��ӽڵ� �ݹ���뺯��
            //for (pugi::xml_node childNode : xmlNode.children())  //childNode�������������ڵ�,��ʹ�ô�д������
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
    pugi::xml_node rootNode = doc.append_child("root_53908f03dd433d55");//���ڵ㣬��Ӧjson�����ģ���
    Json2Xml_help(root, rootNode);
    cJSON_Delete(root);

    std::ostringstream oss;
    doc.save(oss, "", pugi::format_raw | pugi::format_no_declaration); //no_declaration ��ʾ����� <?xml version="1.0">
    std::string result = oss.str();

    //�Զ���:���ڵ㲻��Ҫ
    removeSubstring(result, std::string("<root_53908f03dd433d55>"));
    removeSubstring(result, std::string("</root_53908f03dd433d55>"));

    return result;
}

//CString ����
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

