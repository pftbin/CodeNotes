#ifndef _IPARSEXML_H_
#define _IPARSEXML_H_
#include "atom.h"

#define  TYPE_UNKNOWN         -1
#define  TYPE_DELETEML         0
#define  TYPE_DELETEPLAYLIST   1
#define  TYPE_DELETECONFIG     2
typedef struct __ConfigMQData
{
	int nType;
	PLAYLISTDETAIL plst;
	__ConfigMQData()
	{
		nType = -1;
	}
}ConfigMQData;

class IParseXML
{
public:
	virtual ~IParseXML(void){}
	virtual bool SavePlaylistToXml(const PLAYLISTDETAIL& plst, QString& strXML) = 0;
	virtual bool SavePlaylistToFile(const PLAYLISTDETAIL& plst, const QString& strFile) = 0;
	virtual bool LoadPlaylistFromXml(const QString& strXML, PLAYLISTDETAIL& plst) = 0;
	virtual bool LoadPlaylistFromFile(const QString& strFile, PLAYLISTDETAIL& plst) = 0;
	virtual bool SaveEventsToXml(const std::vector<EVENT>& events, QString& strXML) = 0;
	virtual bool SaveEventsToFile(const std::vector<EVENT>& events, const QString& strFile) = 0;
	virtual bool LoadEventsFromXml(QString& strXML, std::vector<EVENT>& events) = 0;
	virtual bool LoadEventsFromFile(QString& strFile,std::vector<EVENT>& events) = 0;
	virtual bool LoadPlaylistFromFile(const QString& strFile, std::vector<EVENT>& events) = 0;
	virtual bool LoadOleDDObjHeadFromDDXml(QString& strXML, OLEDDOBJHEAD& hd) = 0;
	virtual bool LoadSourcePgmFromDDXml(QString& strXML, SOURCEPGM& pgm) = 0;
	virtual bool LoadEventDataFromDDXml(QString& strXML, DDEVENTDATAEX& ed) = 0;
	virtual bool SaveEventDataToDDXml(const DDEVENTDATAEX& ed, QString& strXML) = 0;
	virtual bool LoadPlaylistFromDDXml(QString& strXML, PLAYLISTDETAIL& plst) = 0;
	virtual bool SavePlaylistToDDXml(PLAYLISTDETAIL& plst, QString& strXML) = 0;
	virtual bool LoadMaterialFromDDXml(QString& strXML, MATERIAL& material) = 0;
	virtual bool LoadMaterialFromDDXmls(QString& strXML, std::vector<MATERIAL>& materials)
	{
		return false;
	}
	virtual bool SetClipLockXml(QString strSrcID, int nLock, QString& strXML) = 0;
	virtual bool LoadMaterialsFromXML( QString& strXML,std::map<QString,MATERIAL > &mapML)
	{
		return false;
	};
	virtual bool SaveMaterialsToXML(std::map<QString,MATERIAL > &mapML, QString& strXML)
	{
		return false;
	};
	virtual bool LoadConfigXml(QString strxml,ConfigMQData &stconfigdata)
	{
		return false;
	}
	virtual bool LoadMosPlst(QString strXML, bool bFile, PLAYLISTDETAIL& Plst)
	{
		return false;
	}

	//管道服务
	virtual bool SavePlayItemsFromXML(const QString& strXML, std::vector<PLAYEDITEMINFO>& PlayedItems){return true;}
	virtual bool GetPipeParamsFromXML(const QString& strXML, std::vector<BASEINFO>& Params){return true;}
	virtual bool LoadStoriesFromDDXml(QString& strXML, std::vector<STORY>& stories){return true;}
	virtual bool SaveStoriesToDDXml(const std::vector<STORY>& stories, QString& strXML){return true;}
	virtual bool SavePlaylistsBaseInfoXML(const std::vector<PLAYLISTBASEINFO>& plsts, QString& strXML){return true;}
	virtual bool GetPlaylistsBaseInfoFromXML(std::vector<PLAYLISTBASEINFO>& plsts, const QString& strXML){return true;}
	virtual bool GetHumanInformation(QString &strXML,DIGITDOCINFO &docInfo){return false;}

	//
	virtual bool LoadActionXml(const QString& strXML,TimeLineAction &action) {return false;}
	virtual bool SaveActionToXml(const TimeLineAction &action,QString& strXML) {return false;}
};

#endif