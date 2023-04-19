#pragma once  
#include "cJSON.h"  

#include <stdlib.h>    
#include <string.h>    
#include <stdio.h>    
#include <assert.h>    
#include <time.h>  
#include <iostream>    
using namespace std;

// 替换字符串  
static string& replace_all(string& str, const string& old_value, const string& new_value)
{
    while (true)
    {
        string::size_type   pos(0);
        if ((pos = str.find(old_value)) != string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return   str;
}

//string 参数  
static string Xml2Json(string strXml)
{
    string pNext = strXml;
    cJSON* reJson = cJSON_CreateObject();
    cJSON* jsonArray = cJSON_CreateArray();
    string strArrayKey = "";
    int nPos = 0;

    while ((nPos = pNext.find("<")) >= 0)
    {
        // 获取第一个节点，如：<doc><a a1="1" a2="2">123</a></doc>  
        int nPosS = pNext.find("<");
        int nPosE = pNext.find(">");
        if (nPosS < 0 || nPosE < 0)
        {
            printf("key error!");
        }

        string strKey = pNext.substr(nPosS + 1, nPosE - nPosS - 1);
        // 解释属性，如：<a a1="1" a2="2">  
        cJSON* jsonVal = NULL;
        if ((nPos = strKey.find("=")) > 0)
        {
            jsonVal = cJSON_CreateObject();
            int nPos = strKey.find(" ");
            string temp = strKey.substr(nPos + 1);
            strKey = strKey.substr(0, nPos);
            while ((nPos = temp.find("=")) > 0)
            {
                int nPos1 = temp.find("=");
                int nPos2 = temp.find("\" ", nPos1 + 1);

                string strSubKey = temp.substr(0, nPos1);
                string strSubVal = temp.substr(nPos1 + 1);
                if (nPos2 > 0)
                    strSubVal = temp.substr(nPos1 + 1, nPos2 - nPos1 - 1);

                // 去除转义字符 \"   
                if ((int)(nPos = strSubVal.find("\"")) >= 0)
                {
                    int nEnd = strSubVal.find("\\", nPos + 1);
                    strSubVal = strSubVal.substr(nPos + 1, nEnd - nPos - 1);
                }
                cJSON_AddItemToObject(jsonVal, ("-" + strSubKey).c_str(), cJSON_CreateString(strSubVal.c_str()));

                if (nPos2 < 0)
                    break;

                temp = temp.substr(nPos2 + 2);
            }
        }

        int nPosKeyE = pNext.find("</" + strKey + ">");
        if (nPosKeyE < 0)
        {
            printf("key error!");
        }
        // 获取节点内容，如<a a1="1" a2="2">123</a> 或 123  
        string strVal = pNext.substr(nPosE + 1, nPosKeyE - nPosE - 1);
        if ((nPos = strVal.find("<")) >= 0)
        {
            // 包含子节点，如<a a1="1" a2="2">123</a>  
            strVal = Xml2Json(strVal);

            if (jsonVal)
            {
                if (strVal != "")
                    cJSON_AddItemToObject(jsonVal, "#text", cJSON_Parse(strVal.c_str()));
            }
            else
            {
                jsonVal = cJSON_Parse(strVal.c_str());
            }
        }
        else
        {
            // 不包含子节点，如123  
            if (jsonVal)
            {
                if (strVal != "")
                    cJSON_AddItemToObject(jsonVal, "#text", cJSON_CreateString(strVal.c_str()));
            }
            else
            {
                jsonVal = cJSON_CreateString(strVal.c_str());
            }
        }

        // 获取下一个节点  
        pNext = pNext.substr(nPosKeyE + strKey.size() + 3);

        // 根据下一节点判断是否为数组  
        int nPosNext = pNext.find("<");
        int nPosNextSame = pNext.find("<" + strKey + ">");
        if (strArrayKey != "" || (nPosNext >= 0 && nPosNextSame >= 0 && nPosNext == nPosNextSame))
        {
            // 数组  
            cJSON_AddItemToArray(jsonArray, jsonVal);
            strArrayKey = strKey;
        }
        else
        {
            // 非数组  
            cJSON_AddItemToObject(reJson, strKey.c_str(), jsonVal);
        }
    }


    if (strArrayKey != "")
    {
        cJSON_AddItemToObject(reJson, strArrayKey.c_str(), jsonArray);
    }

    string strJson = cJSON_Print(reJson);

    if (reJson)
    {
        cJSON_Delete(reJson);
        reJson = NULL;
    }

    return strJson;
}
 
static string Json2Xml(string strJson)
{
    string strXml = "";
    cJSON* root = cJSON_Parse(strJson.c_str());
    if (!root)
    {
        printf("strJson error!");
        return "";
    }

    cJSON* pNext = root->child;
    if (!pNext)
    {
        return strJson;
    }

    int nPos = 0;
    while (pNext)
    {
        string strChild = cJSON_Print(pNext);
        string strVal = Json2Xml(strChild);

        if (pNext->string != NULL)
        {
            string strKey = pNext->string;
            if ((nPos = strKey.find("-")) == 0)
            {
                // 属性项  
                strXml.append(" ");
                strXml.append(strKey.substr(1));
                strXml.append("=");
                strXml.append(strVal);

                if (pNext->next == NULL)
                    strXml.append(">");
            }
            else if ((nPos = strKey.find("#")) == 0)
            {
                // 值  
                strXml.append(">");
                strXml.append(strVal);
            }
            else if ((int)(strVal.find("=")) > 0 && (int)(strVal.find("<")) < 0)
            {
                // 包含属性项的键值对  
                strXml.append("<" + strKey +">");
                strXml.append(strVal);
                strXml.append("</" + strKey + ">");
            }
            else
            {
                // 修正底层无键的值数组的键，如：把<JUAN_XJ_preKey>123</JUAN_XJ_preKey>中的JUAN_XJ_preKey修正  
                if ((int)strVal.find("JUAN_XJ_preKey") >= 0)
                {
                    replace_all(strVal, "JUAN_XJ_preKey", strKey);
                    strXml.append(strVal);
                }
                else
                {
                    strXml.append("<" + strKey + ">");
                    strXml.append(strVal);
                    strXml.append("</" + strKey + ">");
                }
            }
        }
        else
        {
            // 不包含键的值数组， 如：["123", "456"]，暂时转为<JUAN_XJ_preKey>123</JUAN_XJ_preKey>  
            string strPreKey = "JUAN_XJ_preKey";
            strXml.append("<" + strPreKey + ">");
            strXml.append(strVal);
            strXml.append("</" + strPreKey + ">");
        }

        pNext = pNext->next;
    }

    return strXml;
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
