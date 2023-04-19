#include "StdAfx.h"
#include "C_ShareDBServer.h"

DECLARE_LOGER(LogerCGMosModel);

static CString GetAppFolder(void)
{
	TCHAR exeFullName[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(),exeFullName,MAX_PATH);
	CString mPath=exeFullName;

	int mPos=mPath.ReverseFind('\\'); 

	if(mPos==-1)return CString(_T(""));	//没有发现

	mPath=mPath.Left(mPos);

	if(mPath.Right(1)==_T("\\"))
	{
		mPath=mPath.Left(mPos-1);
	}

	return mPath;
}

C_ShareDBServer::C_ShareDBServer(void)
{
	m_pShareDB = NULL;
	m_hModuleShare = NULL;
}

C_ShareDBServer::~C_ShareDBServer(void)
{
}

//////////////////////////////////////////////////////////////////////////
BOOL C_ShareDBServer::InitServer()
{
	BOOL bResult = TRUE;
	bResult &= InitShareDBClient();

	if (bResult == TRUE)
	{
		CString strLogInfo;strLogInfo.Format(_T("[InitServer]: %s"), _T("数据库服务初始化成功"));
		WRITE_LOG(LogerCGMosModel, 0, FALSE, strLogInfo);
	}
	else
	{
		CString strLogInfo;strLogInfo.Format(_T("[InitServer]: %s"), _T("数据库服务初始化失败..."));
		WRITE_LOG(LogerCGMosModel, 0, FALSE, strLogInfo);
	}
	return bResult;
}

BOOL C_ShareDBServer::GetCGPlayListInfo(int nID, CString& strPlayListInfo)
{
	if (m_pShareDB==NULL)
		return FALSE;

	return m_pShareDB->GetCGPlayListInfo(nID,strPlayListInfo);
}

//////////////////////////////////////////////////////////////////////////
BOOL C_ShareDBServer::InitShareDBClient()
{
	CString strFolder = GetAppFolder();
	CString strPath = strFolder + _T("\\ShareDBClient.dll");

	BOOL bResult = FALSE;
	m_hModuleShare = ::LoadLibrary(strPath);
	if(NULL == m_hModuleShare)
	{
		CString strLogInfo;strLogInfo.Format(_T("[InitShareDBClient]: %s"), _T("加载 ShareDBClient.dll 失败..."));
		WRITE_LOG(LogerCGMosModel, 0, FALSE, strLogInfo);
	}
	else
	{
		typedef IShareDBc* (*PGETSHAREDB)(CString strMaster);
		PGETSHAREDB pGetShareDB = (PGETSHAREDB)GetProcAddress (m_hModuleShare, "GetShareDB");
		if(NULL != pGetShareDB)
		{
			m_pShareDB = pGetShareDB(_T("CGMosModelDBServer"));
			if (m_pShareDB)
			{
				bResult = m_pShareDB->IsValidShareDB();
			}
		}
	}

	return bResult;
}

BOOL C_ShareDBServer::ExitShareDBClient()
{
	if (m_hModuleShare && m_pShareDB)
	{
		::FreeLibrary(m_hModuleShare);
		m_hModuleShare = NULL;
	}

	return TRUE;
}
