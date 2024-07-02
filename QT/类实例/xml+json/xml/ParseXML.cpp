#include "ParseXML.h"
#include "toolkit.h"
#include "QXmlHelper.h"

//为函数声明指针类型,用以转换void*
typedef bool (*pSavePlaylistToXml)(const PLAYLISTDETAIL& plst, QString& strXML);
typedef bool (*pSavePlaylistToFile)(const PLAYLISTDETAIL& plst, const QString& strFile);
typedef bool (*pLoadPlaylistFromXml)(const QString& strXML, PLAYLISTDETAIL& plst);
typedef bool (*pLoadPlaylistFromFile)(const QString& strFile, PLAYLISTDETAIL& plst);
typedef bool (*pSaveEventsToXml)(const std::vector<EVENT>& events, QString& strXML);
typedef bool (*pSaveEventsToFile)(const std::vector<EVENT>& events, const QString& strFile);
typedef bool (*pLoadEventsFromXml)(QString& strXML, std::vector<EVENT>& events);
typedef bool (*pLoadEventsFromFile)(const QString& strFile, std::vector<EVENT>& events);
typedef bool (*pLoadOleDDObjHeadFromDDXml)(QString& strXML, OLEDDOBJHEAD& hd);
typedef bool (*pLoadSourcePgmFromDDXml)(QString& strXML, SOURCEPGM& pgm);
typedef bool (*pLoadEventDataFromDDXml)(QString& strXML, DDEVENTDATAEX& ed);
typedef bool (*pSaveEventDataToDDXml)(const DDEVENTDATAEX& ed, QString& strXML);
typedef bool (*pLoadPlaylistFromDDXml)(QString& strXML, PLAYLISTDETAIL& plst);
typedef bool (*pSavePlaylistToDDXml)(PLAYLISTDETAIL& plst, QString& strXML);
typedef bool (*pLoadMaterialFromDDXml)(QString& strXML, MATERIAL& material);
typedef bool (*pSetClipLockXml)(QString strSrcID, int nLock, QString& strXML);
typedef bool (*pLoadMaterialFromDDXmlNewInterface)(QString& strXML, MATERIAL& material);
typedef bool (*pLoadMaterialsFromXML)(QString& strXML, std::map<QString, MATERIAL >& mapML);
typedef bool (*pSaveMaterialsToXML)(std::map<QString, MATERIAL >& mapML, QString& strXML);
typedef bool (*pSavePlayItemsFromXML)(const QString& strXML, std::vector<PLAYEDITEMINFO>& PlayedItems);
typedef bool (*pGetPipeParamsFromXML)(const QString& strXML, std::vector<BASEINFO>& Params);
typedef bool (*pLoadStoriesFromDDXml)(QString& strXML, std::vector<STORY>& stories);
typedef bool (*pSaveStoriesToDDXml)(const std::vector<STORY>& stories, QString& strXML);
typedef bool (*pSavePlaylistsBaseInfoXML)(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML);
typedef bool (*pGetPlaylistsBaseInfoFromXML)(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML);
typedef bool (*pCreateParamsXml)(PstParams pstP, QString& strXML);

bool (*XML_SavePlaylistToXml)(const PLAYLISTDETAIL& plst, QString& strXML) = NULL;
bool (*XML_SavePlaylistToFile)(const PLAYLISTDETAIL& plst, const QString& strFile) = NULL;
bool (*XML_LoadPlaylistFromXml)(const QString& strXML, PLAYLISTDETAIL& plst) = NULL;
bool (*XML_LoadPlaylistFromFile)(const QString& strFile, PLAYLISTDETAIL& plst) = NULL;
bool (*XML_SaveEventsToXml)(const std::vector<EVENT>& events, QString& strXML) = NULL;
bool (*XML_SaveEventsToFile)(const std::vector<EVENT>& events, const QString& strFile) = NULL;
bool (*XML_LoadEventsFromXml)(QString& strXML, std::vector<EVENT>& events) = NULL;
bool (*XML_LoadEventsFromFile)(const QString& strFile, std::vector<EVENT>& events) = NULL;
bool (*XML_LoadOleDDObjHeadFromDDXml)(QString& strXML, OLEDDOBJHEAD& hd) = NULL;
bool (*XML_LoadSourcePgmFromDDXml)(QString& strXML, SOURCEPGM& pgm) = NULL;
bool (*XML_LoadEventDataFromDDXml)(QString& strXML, DDEVENTDATAEX& ed) = NULL;
bool (*XML_SaveEventDataToDDXml)(const DDEVENTDATAEX& ed, QString& strXML) = NULL;
bool (*XML_LoadPlaylistFromDDXml)(QString& strXML, PLAYLISTDETAIL& plst) = NULL;
bool (*XML_SavePlaylistToDDXml)(PLAYLISTDETAIL& plst, QString& strXML) = NULL;
bool (*XML_LoadMaterialFromDDXml)(QString& strXML, MATERIAL& material) = NULL;
bool (*XML_SetClipLockXml)(QString strSrcID, int nLock, QString& strXML) = NULL;
bool (*XML_LoadMaterialFromDDXmlNewInterface)(QString& strXML, MATERIAL& material) = NULL;
bool (*XML_LoadMaterialsFromXML)( QString& strXML,std::map<QString,MATERIAL > &mapML) = NULL;
bool (*XML_SaveMaterialsToXML)(std::map<QString,MATERIAL > &mapML, QString& strXML) = NULL;
bool (*XML_SavePlayItemsFromXML)(const QString& strXML, std::vector<PLAYEDITEMINFO>& PlayedItems) = NULL;
bool (*XML_GetPipeParamsFromXML)(const QString& strXML, std::vector<BASEINFO>& Params) = NULL;
bool (*XML_LoadStoriesFromDDXml)(QString& strXML, std::vector<STORY>& stories) = NULL;
bool (*XML_SaveStoriesToDDXml)(const std::vector<STORY>& stories, QString& strXML) = NULL;
bool (*XML_SavePlaylistsBaseInfoXML)(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML) = NULL;
bool (*XML_GetPlaylistsBaseInfoFromXML)(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML) = NULL;
bool (*XML_CreateParamsXml)(PstParams pstP, QString& strXML) = NULL;

CParseXML g_parseXml;

bool CParseXML::m_bInited = false;
void CParseXML::Init(void)
{
    //指定动态库文件的路径
#ifdef _WIN32
	QString dllPath = QString("%1/QParseXML.dll").arg(QCoreApplication::applicationDirPath());
#elif __linux__
	QString dllPath = QString("%1/QParseXML.so").arg(QCoreApplication::applicationDirPath());
#endif

    m_libParseXML.setFileName(dllPath);
    if (m_libParseXML.load())
	{
        XML_SavePlaylistToXml = (pSavePlaylistToXml)m_libParseXML.resolve("SavePlaylistToXml");
        XML_SavePlaylistToFile = (pSavePlaylistToFile)m_libParseXML.resolve("SavePlaylistToFile");
        XML_LoadPlaylistFromXml = (pLoadPlaylistFromXml)m_libParseXML.resolve("LoadPlaylistFromXml");
        XML_LoadPlaylistFromFile = (pLoadPlaylistFromFile)m_libParseXML.resolve("LoadPlaylistFromFile");
        XML_SaveEventsToXml = (pSaveEventsToXml)m_libParseXML.resolve("SaveEventsToXml");
        XML_SaveEventsToFile = (pSaveEventsToFile)m_libParseXML.resolve("SaveEventsToFile");
        XML_LoadEventsFromXml = (pLoadEventsFromXml)m_libParseXML.resolve("LoadEventsFromXml");
        XML_LoadEventsFromFile = (pLoadEventsFromFile)m_libParseXML.resolve("LoadEventsFromFile");
        XML_LoadOleDDObjHeadFromDDXml = (pLoadOleDDObjHeadFromDDXml)m_libParseXML.resolve("LoadOleDDObjHeadFromDDXml");
        XML_LoadSourcePgmFromDDXml = (pLoadSourcePgmFromDDXml)m_libParseXML.resolve("LoadSourcePgmFromDDXml");
        XML_LoadEventDataFromDDXml = (pLoadEventDataFromDDXml)m_libParseXML.resolve("LoadEventDataFromDDXml");
        XML_SaveEventDataToDDXml = (pSaveEventDataToDDXml)m_libParseXML.resolve("SaveEventDataToDDXml");
        XML_LoadPlaylistFromDDXml = (pLoadPlaylistFromDDXml)m_libParseXML.resolve("LoadPlaylistFromDDXml");
        XML_SavePlaylistToDDXml = (pSavePlaylistToDDXml)m_libParseXML.resolve("SavePlaylistToDDXml");
        XML_LoadMaterialFromDDXml = (pLoadMaterialFromDDXml)m_libParseXML.resolve("LoadMaterialFromDDXml");
        XML_LoadMaterialFromDDXmlNewInterface = (pLoadMaterialFromDDXmlNewInterface)m_libParseXML.resolve("LoadMaterialFromDDXmlNewInterface");
        XML_LoadMaterialsFromXML = (pLoadMaterialsFromXML)m_libParseXML.resolve("LoadMaterialsFromXml");
        XML_SaveMaterialsToXML = (pSaveMaterialsToXML)m_libParseXML.resolve("SaveMaterialsToXml");
        XML_SavePlayItemsFromXML = (pSavePlayItemsFromXML)m_libParseXML.resolve("SavePlayItemsFromXML");
        XML_GetPipeParamsFromXML = (pGetPipeParamsFromXML)m_libParseXML.resolve("GetPipeParamsFromXML");
        XML_LoadStoriesFromDDXml = (pLoadStoriesFromDDXml)m_libParseXML.resolve("LoadStoriesFromDDXml");
        XML_SaveStoriesToDDXml = (pSaveStoriesToDDXml)m_libParseXML.resolve("SaveStoriesToDDXml");
        XML_SavePlaylistsBaseInfoXML = (pSavePlaylistsBaseInfoXML)m_libParseXML.resolve("SavePlaylistsBaseInfoXML");
        XML_GetPlaylistsBaseInfoFromXML = (pGetPlaylistsBaseInfoFromXML)m_libParseXML.resolve("GetPlaylistsBaseInfoFromXML");
        XML_CreateParamsXml = (pCreateParamsXml)m_libParseXML.resolve("CreateParamsXml");

		m_bInited = true;
	}

}

CParseXML::CParseXML(void)
{
	m_bInited = false;
	Init();
}

CParseXML::~CParseXML(void)
{
	if (!m_bInited) return;

    // 卸载动态库
    m_libParseXML.unload();

	XML_SavePlaylistToXml = NULL;
	XML_SavePlaylistToFile = NULL;
	XML_LoadPlaylistFromXml = NULL;
	XML_LoadPlaylistFromFile = NULL;
	XML_SaveEventsToXml = NULL;
	XML_SaveEventsToFile = NULL;
	XML_LoadEventsFromXml = NULL;
	XML_LoadEventsFromFile = NULL;
	XML_LoadOleDDObjHeadFromDDXml = NULL;
	XML_LoadSourcePgmFromDDXml = NULL;
	XML_LoadEventDataFromDDXml = NULL;
	XML_SaveEventDataToDDXml = NULL;
	XML_LoadPlaylistFromDDXml = NULL;
	XML_SavePlaylistToDDXml = NULL;
	XML_LoadMaterialFromDDXml = NULL;
	XML_SaveMaterialsToXML = NULL;
	XML_LoadMaterialsFromXML = NULL;
	XML_SavePlayItemsFromXML = NULL;
	XML_GetPipeParamsFromXML = NULL;
	XML_LoadStoriesFromDDXml = NULL;
	XML_SaveStoriesToDDXml   = NULL;
	XML_SavePlaylistsBaseInfoXML	= NULL;
	XML_GetPlaylistsBaseInfoFromXML = NULL;
	XML_CreateParamsXml = NULL;
}

bool CParseXML::SavePlaylistToXml(const PLAYLISTDETAIL& plst, QString& strXML)
{
	if(XML_SavePlaylistToXml == NULL) return false;
	return XML_SavePlaylistToXml(plst, strXML);
}
bool CParseXML::SavePlaylistToFile(const PLAYLISTDETAIL& plst, const QString& strFile)
{
	if(XML_SavePlaylistToFile == NULL) return false;
	return XML_SavePlaylistToFile(plst, strFile);
}
bool CParseXML::LoadPlaylistFromXml(const QString& strXML, PLAYLISTDETAIL& plst)
{
	if(XML_LoadPlaylistFromXml == NULL) return false;
	return XML_LoadPlaylistFromXml(strXML, plst);
}
bool CParseXML::LoadPlaylistFromFile(const QString& strFile, PLAYLISTDETAIL& plst)
{
	if(XML_LoadPlaylistFromFile == NULL) return false;
	return XML_LoadPlaylistFromFile(strFile, plst);
}
bool CParseXML::SaveEventsToXml(const std::vector<EVENT>& events, QString& strXML)
{
	if(XML_SaveEventsToXml == NULL) return false;
	return XML_SaveEventsToXml(events, strXML);
}
bool CParseXML::SaveEventsToFile(const std::vector<EVENT>& events, const QString& strFile)
{
	if(XML_SaveEventsToFile == NULL) return false;
	return XML_SaveEventsToFile(events, strFile);
}
bool CParseXML::LoadEventsFromXml(QString& strXML, std::vector<EVENT>& events)
{
	if(XML_LoadEventsFromXml == NULL) return false;
	return XML_LoadEventsFromXml(strXML, events);
}
bool CParseXML::LoadPlaylistFromFile(const QString& strFile, std::vector<EVENT>& events)
{
	if(XML_LoadEventsFromFile == NULL) return false;
	return XML_LoadEventsFromFile(strFile, events);
}
bool CParseXML::LoadOleDDObjHeadFromDDXml(QString& strXML, OLEDDOBJHEAD& hd)
{
	if(XML_LoadOleDDObjHeadFromDDXml == NULL) return false;
	return XML_LoadOleDDObjHeadFromDDXml(strXML, hd);
}
bool CParseXML::LoadSourcePgmFromDDXml(QString& strXML, SOURCEPGM& pgm)
{
	if(XML_LoadSourcePgmFromDDXml == NULL) return false;
	return XML_LoadSourcePgmFromDDXml(strXML, pgm);
}
bool CParseXML::LoadEventDataFromDDXml(QString& strXML, DDEVENTDATAEX& ed)
{
	if(XML_LoadEventDataFromDDXml == NULL) return false;
	return XML_LoadEventDataFromDDXml(strXML, ed);
}
bool CParseXML::SaveEventDataToDDXml(const DDEVENTDATAEX& ed, QString& strXML)
{
	if(XML_SaveEventDataToDDXml == NULL) return false;
	return XML_SaveEventDataToDDXml(ed, strXML);
}
bool CParseXML::LoadPlaylistFromDDXml(QString& strXML, PLAYLISTDETAIL& plst)
{
	if(XML_LoadPlaylistFromDDXml == NULL) return false;
	return XML_LoadPlaylistFromDDXml(strXML, plst);
}
bool CParseXML::SavePlaylistToDDXml(PLAYLISTDETAIL& plst, QString& strXML)
{
	if(XML_SavePlaylistToDDXml == NULL) return false;
	return XML_SavePlaylistToDDXml(plst, strXML);
}
bool CParseXML::LoadMaterialFromDDXml(QString& strXML, MATERIAL& material)
{
	if(XML_LoadMaterialFromDDXml == NULL) return false;
	return XML_LoadMaterialFromDDXml(strXML, material);
} 
bool CParseXML::SetClipLockXml(QString strSrcID, int nLock, QString& strXML)
{
	if (XML_SetClipLockXml)
		return XML_SetClipLockXml(strSrcID, nLock, strXML);

	return false;
}
bool CParseXML::LoadEventsFromFile(QString& strFile,std::vector<EVENT>& events)
{
	if (XML_LoadEventsFromFile)
		return XML_LoadEventsFromFile(strFile,events);
	return false;
}
bool CParseXML::LoadMaterialFromDDXmlNewInterface(QString& strXML, MATERIAL& material)
{
	if(XML_LoadMaterialFromDDXmlNewInterface)
		return XML_LoadMaterialFromDDXmlNewInterface(strXML,material);
	return false;
}
bool CParseXML::LoadMaterialsFromXML( QString& strXML,std::map<QString,MATERIAL > &mapML)
{
	if(XML_LoadMaterialsFromXML)
		return XML_LoadMaterialsFromXML(strXML,mapML);
	return false;
}
bool CParseXML::SaveMaterialsToXML(std::map<QString,MATERIAL > &mapML, QString& strXML)
{
	if(XML_SaveMaterialsToXML)
		return XML_SaveMaterialsToXML(mapML,strXML);
	return false;
}
bool CParseXML::SavePlayItemsFromXML(const QString& strXML,std::vector<PLAYEDITEMINFO>& PlayedItems)
{
	if(NULL != XML_SavePlayItemsFromXML)
	{
		return XML_SavePlayItemsFromXML (strXML, PlayedItems);
	}
	return false;
}
bool CParseXML::GetPipeParamsFromXML(const QString& strXML, std::vector<BASEINFO>& Params)
{
	if(NULL != XML_GetPipeParamsFromXML)
	{
		return XML_GetPipeParamsFromXML (strXML,Params);
	}
	return false;
}
bool CParseXML::LoadStoriesFromDDXml(QString& strXML, std::vector<STORY>& stories)
{
	if(NULL != XML_LoadStoriesFromDDXml)
	{
		return XML_LoadStoriesFromDDXml (strXML,stories);
	}
	return false;
}
bool CParseXML::SaveStoriesToDDXml(const std::vector<STORY>& stories, QString& strXML)
{
	if(NULL != XML_SaveStoriesToDDXml)
	{
		return XML_SaveStoriesToDDXml (stories,strXML);
	}
	return false;
}
bool CParseXML::SavePlaylistsBaseInfoXML(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML)
{
	if(NULL != XML_SavePlaylistsBaseInfoXML)
	{
		return XML_SavePlaylistsBaseInfoXML (plsts,strXML);
	}
	return false;
}
bool CParseXML::GetPlaylistsBaseInfoFromXML(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML)
{
	if(NULL != XML_GetPlaylistsBaseInfoFromXML)
	{
		return XML_GetPlaylistsBaseInfoFromXML (plsts,strXML);
	}
	return false;
}
bool CParseXML::CreateParamsXml(PstParams pstP, QString& strXML)
{
	if(XML_CreateParamsXml)
		return XML_CreateParamsXml(pstP, strXML);
	return false;
}

bool CParseXML::UpdateMessageID(QString& strXML, QString strName, QString strMessageID)
{
	bool bResult = false;
	QXmlHelper xmlHelper; QDomDocument spXMLDOM;

	bResult = xmlHelper.loadXmlFromString(strXML, spXMLDOM);
	if (strXML.isEmpty() || !bResult)
		return false;

	QDomNode pXmlChildNode = xmlHelper.getNode(spXMLDOM, strName);
	if (pXmlChildNode.isNull())
		return false;

    pXmlChildNode.toElement().setAttribute(QString("MessageID"), strMessageID);
	strXML = xmlHelper.saveXmlToString(spXMLDOM);

	return !strXML.isEmpty();
}
