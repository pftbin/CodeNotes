#include "StdAfx.h"
#include "C_XMLServer.h"

DECLARE_LOGER(LogerDeviceConfig);

#include <MsXml.h>
class CAutoCoInitialize
{
public:
	CAutoCoInitialize() { CoInitialize(NULL); }
	~CAutoCoInitialize() { CoUninitialize(); }
};

C_XMLServer::C_XMLServer(void)
{
}

C_XMLServer::~C_XMLServer(void)
{
}

//////////////////////////////////////////////////////////////////////////
IXMLDOMNode * CreateNode(CComPtr<IXMLDOMDocument>& spXMLDOM,IXMLDOMNode * pXMLItem, BSTR bstrName, BSTR bstrValue)
{
	if(spXMLDOM==NULL || pXMLItem==NULL)return NULL;
	CComQIPtr<IXMLDOMElement> element;
	IXMLDOMNode *pData;

	if(FAILED(spXMLDOM->createElement(bstrName,&element)))
		return NULL;
	if(FAILED(pXMLItem->appendChild(element,&pData)))
	{
		element.Release();
		pData->Release();
		return NULL;
	}
	element.Release();

	pData->put_text(bstrValue);
	return pData;	
}

void AppendNode(CComPtr<IXMLDOMDocument>& spXMLDOM,IXMLDOMNode * pXMLItem, BSTR bstrName, CString bstrValue)
{
	if(spXMLDOM==NULL || pXMLItem==NULL)return;
	CComQIPtr<IXMLDOMElement> element;
	CComQIPtr<IXMLDOMNode> pData;

	if(FAILED(spXMLDOM->createElement(bstrName,&element)))
		return ;
	if(FAILED(pXMLItem->appendChild(element,&pData)))
	{
		element.Release();
		pData.Release();
		return ;
	}
	element.Release();

	pData->put_text((CComBSTR)(bstrValue));
	pData.Release();	
}

void AppendNode(CComPtr<IXMLDOMDocument>& spXMLDOM,IXMLDOMNode * pXMLItem, BSTR bstrName, BSTR bstrValue)
{
	if(spXMLDOM==NULL || pXMLItem==NULL)return;
	CComQIPtr<IXMLDOMElement> element;
	CComQIPtr<IXMLDOMNode> pData;

	if(FAILED(spXMLDOM->createElement(bstrName,&element)))
		return ;
	if(FAILED(pXMLItem->appendChild(element,&pData)))
	{
		element.Release();
		pData.Release();
		return;
	}
	element.Release();

	pData->put_text(bstrValue);
	pData.Release();
}

BOOL AppendNode(IXMLDOMDocument* pXMLDOM, IXMLDOMNode* pParent, BSTR name, IXMLDOMNode** pChild)
{
	CComPtr<IXMLDOMNode> spChild;
	HRESULT hr = pXMLDOM->createNode(
		CComVariant(NODE_ELEMENT),
		name, NULL, &spChild);
	if ( FAILED(hr) ) return FALSE;
	if ( spChild.p == NULL ) return FALSE;
	//添加新节点到spXMLNode节点下去。
	CComPtr<IXMLDOMNode> spInsertedMaterialNode;
	hr = pParent->appendChild(spChild, pChild);
	if ( FAILED(hr) ) return FALSE;
	if ( pChild == NULL ) return FALSE;
	return TRUE;
}
void UpdateSavePath(CString& strPath)
{
	if(strPath.GetLength () < 3 && strPath.Find (_T(":\\")) < 0)
		return;
	CString strRoot = strPath.Left (3);
	strPath.Replace (_T(":"),_T("："));
	strPath.Replace (_T('/'),_T('／'));
	strPath.Replace (_T('*'),_T('＊'));
	strPath.Replace (_T('?'),_T('？'));
	strPath.Replace (_T('"'),_T('＂'));
	strPath.Replace (_T('<'),_T('＜'));
	strPath.Replace (_T('>'),_T('＞'));
	strPath.Replace (_T('|'),_T('｜'));
	strPath = strRoot + strPath.Right (strPath.GetLength () - 3);
}
//////////////////////////////////////////////////////////////////////////
BOOL C_XMLServer::XML_SaveDeviceConfigInfo(DEVCONFIGITEM_VEC& inDeviceConfigVec,CString strXMLPath)
{
	CAutoCoInitialize coi;
	try {
		// 通过智能指针创建一个解析器的实例。
		CComPtr<IXMLDOMDocument> spXMLDOM;
		HRESULT hr = spXMLDOM.CoCreateInstance(__uuidof(DOMDocument));
		if ( FAILED(hr) ) return FALSE;
		if ( spXMLDOM.p == NULL ) return FALSE;

		//Config
		CComPtr<IXMLDOMNode> spXMLNode_Config;
		hr = spXMLDOM->createNode(
			CComVariant(NODE_ELEMENT),
			CComBSTR(L"Config"),
			NULL,&spXMLNode_Config);
		if ( FAILED(hr) ) return FALSE;
		if ( spXMLNode_Config.p == NULL ) return FALSE;
		CComPtr<IXMLDOMNode> spInsertedNode_Config;
		hr = spXMLDOM->appendChild(spXMLNode_Config,&spInsertedNode_Config);
		if ( FAILED(hr) ) return FALSE;
		if ( spInsertedNode_Config.p == NULL ) return FALSE;

		//Channels
		CComPtr<IXMLDOMNode> spXMLNode_Channels;
		hr = spXMLDOM->createNode(
			CComVariant(NODE_ELEMENT),
			CComBSTR(L"Channels"),
			NULL,&spXMLNode_Channels);
		if ( FAILED(hr) ) return FALSE;
		if ( spXMLNode_Channels.p == NULL ) return FALSE;
		CComPtr<IXMLDOMNode> spInsertedNode_Channels;
		hr = spInsertedNode_Config->appendChild(spXMLNode_Channels, &spInsertedNode_Channels);
		if ( FAILED(hr) ) return FALSE;
		if ( spInsertedNode_Channels.p == NULL ) return FALSE;

		for (int i = 0; i < inDeviceConfigVec.size(); i++)
		{
			//Channel
			CComPtr<IXMLDOMNode> spXMLNode_Channel;
			HRESULT hr = spXMLDOM->createNode(
				CComVariant(NODE_ELEMENT),
				CComBSTR(L"Channel"),
				NULL,&spXMLNode_Channel);
			if ( FAILED(hr) ) return FALSE;
			if ( spXMLNode_Channel.p == NULL ) return FALSE;
			CComPtr<IXMLDOMNode> spInsertedNode_Channel;
			hr = spInsertedNode_Channels->appendChild(spXMLNode_Channel, &spInsertedNode_Channel);
			if ( FAILED(hr) ) return FALSE;
			if ( spInsertedNode_Channel.p == NULL ) return FALSE;

			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"ID"),CComBSTR(inDeviceConfigVec[i].strDeviceID));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"Guid"),CComBSTR(inDeviceConfigVec[i].strGUID));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"Name"),CComBSTR(inDeviceConfigVec[i].strName));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"Type"),CComBSTR(inDeviceConfigVec[i].strType));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"ComPort"),CComBSTR(inDeviceConfigVec[i].strComPort));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"IP"),CComBSTR(inDeviceConfigVec[i].strIP));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"Port"),CComBSTR(inDeviceConfigVec[i].strPort));
			AppendNode(spXMLDOM,spInsertedNode_Channel,CComBSTR(L"URL"),CComBSTR(inDeviceConfigVec[i].strURL));
		}

		hr = spXMLDOM->save(CComVariant(strXMLPath));
		if( FAILED(hr) ) return FALSE;
	} 
	catch(...) 
	{
		return FALSE;
	} 

	return TRUE;
}
