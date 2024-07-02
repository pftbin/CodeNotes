#pragma once
#include <QLibrary>
#include <QFile>
#include <QDir>
#include "IParseXML.h"
#include "MsXmlHelper.h"
using namespace MsXmlHelp;

class CParseXML : public IParseXML
{
public:
	CParseXML(void);
	~CParseXML(void);
    void Init(void);

	virtual bool SavePlaylistToXml(const PLAYLISTDETAIL& plst, QString& strXML);
	virtual bool SavePlaylistToFile(const PLAYLISTDETAIL& plst, const QString& strFile);
	virtual bool LoadPlaylistFromXml(const QString& strXML, PLAYLISTDETAIL& plst);
	virtual bool LoadPlaylistFromFile(const QString& strFile, PLAYLISTDETAIL& plst);
	virtual bool SaveEventsToXml(const std::vector<EVENT>& events, QString& strXML);
	virtual bool SaveEventsToFile(const std::vector<EVENT>& events, const QString& strFile);
	virtual bool LoadEventsFromXml(QString& strXML, std::vector<EVENT>& events);
	virtual bool LoadPlaylistFromFile(const QString& strFile, std::vector<EVENT>& events);
	virtual bool LoadOleDDObjHeadFromDDXml(QString& strXML, OLEDDOBJHEAD& hd);
	virtual bool LoadSourcePgmFromDDXml(QString& strXML, SOURCEPGM& pgm);
	virtual bool LoadEventDataFromDDXml(QString& strXML, DDEVENTDATAEX& ed);
	virtual bool SaveEventDataToDDXml(const DDEVENTDATAEX& ed, QString& strXML);
	virtual bool LoadPlaylistFromDDXml(QString& strXML, PLAYLISTDETAIL& plst);
	virtual bool SavePlaylistToDDXml(PLAYLISTDETAIL& plst, QString& strXML);
	virtual bool LoadMaterialFromDDXml(QString& strXML, MATERIAL& material); 
	virtual bool SetClipLockXml(QString strSrcID, int nLock, QString& strXML);
	virtual bool LoadEventsFromFile(QString& strFile,std::vector<EVENT>& events);

	virtual bool LoadMaterialFromDDXmlNewInterface(QString& strXML, MATERIAL& material);
	virtual bool LoadMaterialsFromXML( QString& strXML,std::map<QString,MATERIAL > &mapML);
	virtual bool SaveMaterialsToXML(std::map<QString,MATERIAL > &mapML, QString& strXML);

	//管道服务
	virtual bool SavePlayItemsFromXML(const QString& strXML, std::vector<PLAYEDITEMINFO>& PlayedItems);
	virtual bool GetPipeParamsFromXML(const QString& strXML, std::vector<BASEINFO>& Params);
	virtual bool LoadStoriesFromDDXml(QString& strXML, std::vector<STORY>& stories);
	virtual bool SaveStoriesToDDXml(const std::vector<STORY>& stories, QString& strXML);
	virtual bool SavePlaylistsBaseInfoXML(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML);
	virtual bool GetPlaylistsBaseInfoFromXML(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML);



	bool CreateParamsXml(PstParams pstP, QString& strXML);
	bool UpdateMessageID(QString& strXML, QString strName, QString strMessageID);

private:
    QLibrary    m_libParseXML;
	static bool m_bInited;
};

extern CParseXML g_parseXml;


//////////////////////////////////////////////////////////////////////////
// Functions export from dll

extern bool (*XML_SavePlaylistToXml)(const PLAYLISTDETAIL& plst, QString& strXML);
extern bool (*XML_SavePlaylistToFile)(const PLAYLISTDETAIL& plst, const QString& strFile);
extern bool (*XML_LoadPlaylistFromXml)(const QString& strXML, PLAYLISTDETAIL& plst);
extern bool (*XML_LoadPlaylistFromFile)(const QString& strFile, PLAYLISTDETAIL& plst);
extern bool (*XML_SaveEventsToXml)(const std::vector<EVENT>& events, QString& strXML);
extern bool (*XML_SaveEventsToFile)(const std::vector<EVENT>& events, const QString& strFile);
extern bool (*XML_LoadEventsFromXml)(QString& strXML, std::vector<EVENT>& events);
extern bool (*XML_LoadEventsFromFile)(const QString& strFile, std::vector<EVENT>& events);
extern bool (*XML_LoadOleDDObjHeadFromDDXml)(QString& strXML, OLEDDOBJHEAD& hd);
extern bool (*XML_LoadSourcePgmFromDDXml)(QString& strXML, SOURCEPGM& pgm);
extern bool (*XML_LoadEventDataFromDDXml)(QString& strXML, DDEVENTDATAEX& ed);
extern bool (*XML_SaveEventDataToDDXml)(const DDEVENTDATAEX& ed, QString& strXML);
extern bool (*XML_LoadPlaylistFromDDXml)(QString& strXML, PLAYLISTDETAIL& plst);
extern bool (*XML_SavePlaylistToDDXml)(PLAYLISTDETAIL& plst, QString& strXML);
extern bool (*XML_LoadMaterialFromDDXml)(QString& strXML, MATERIAL& material);
extern bool (*XML_SetClipLockXml)(QString strSrcID, int nLock, QString& strXML);

//管道服务
extern bool (*XML_SavePlayItemsFromXML)(const QString& strXML, std::vector<PLAYEDITEMINFO>& PlayedItems);
extern bool (*XML_GetPipeParamFromXML)(const QString& strXML, std::vector<BASEINFO>& Params);
extern bool (*XML_LoadStoriesFromDDXml)(QString& strXML, std::vector<STORY>& stories);
extern bool (*XML_SaveStoriesToDDXml)(const std::vector<STORY>& stories, QString& strXML);
extern bool (*XML_SavePlaylistsBaseInfoXML)(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML);
extern bool (*XML_GetPlaylistsBaseInfoFromXML)(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML);
extern bool (*XML_CreateParamsXml)(PstParams pstP, QString& strXML);
