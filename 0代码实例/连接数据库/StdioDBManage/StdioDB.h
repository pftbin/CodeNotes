	
// StdioDB.h : Declaration of the CStdioDB

#ifndef __STDIODB_H_
#define __STDIODB_H_

#include "resource.h"       // main symbols
#include <comsvcs.h>
/////////////////////////////////////////////////////////////////////////////
// CStdioDB
class ATL_NO_VTABLE CStdioDB : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CStdioDB, &CLSID_StdioDB>,
	public IObjectConstruct,
	public IDispatchImpl<IStdioDB, &IID_IStdioDB, &LIBID_STDIODBMANAGELib>
{
public:
	CStdioDB();
	~CStdioDB();

DECLARE_REGISTRY_RESOURCEID(IDR_STDIODB)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CStdioDB)
	COM_INTERFACE_ENTRY(IStdioDB)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectConstruct)
END_COM_MAP()

private:
	int	 m_iStdioDBID;
	long m_lWriteLogFlag;
	long m_lDatabaseType;//数据库类型
	long m_lWriteEfficiencyLog;
	_ConnectionPtr m_spAdoCon;
	long m_lDBRunMode;
	BOOL m_bBeginTrans;
	BOOL m_bIsBackupDatabase;
public:
	STDMETHOD(Activate)();
	STDMETHOD_(BOOL, CanBePooled)();
	STDMETHOD_(void, Deactivate)();

// IObjectConstruct
	STDMETHOD(Construct)(IDispatch *pDisp);

// IStdioDB
public:
	STDMETHOD(CheckDBStatus)(/*[out,retval]*/long *plDBStatus);
	STDMETHOD(LockItemData)(/*[in]*/BSTR bstrDataID,/*[in]*/long lLockType,/*[in]*/long lLoginInfoID,/*[in]*/long lUserID,/*[out,retval]*/long  *plLockFlag);
	STDMETHOD(UpdateRecordsetWithLOB)(/*[in]*/_Recordset *pRs);
	STDMETHOD(GetRecordsetWithLOB)(/*[in]*/BSTR bstrSql, /*[out,retval]*/_Recordset **ppRs);
	STDMETHOD(GetNextID)(/*[in]*/BSTR bstrTableName,/*[out,retval]*/long* plNextID);
	STDMETHOD(GetDatabaseType)(/*[out,retval]*/long *plDatabaseType);
	STDMETHOD(ManualCommit)();
	STDMETHOD(DeviateTransaction)();
	STDMETHOD(UpdateRecordset)(/*[in]*/_Recordset *pRs);
	STDMETHOD(ExecuteCommand)(/*[in]*/BSTR bstrCmd, /*[out, retval]*/unsigned long *pulAffectCount);
	STDMETHOD(GetRecordset)(/*[in]*/BSTR bstrSQL, /*[out, retval]*/_Recordset **ppRs);
	STDMETHOD(EnlistResource)(BOOL bEnlist);

protected:
	BOOL IsQuerySql(BSTR bstrSql);
	CComBSTR GetRemark(_Recordset *pRs);
	void CheckLob(_Recordset *pRs);
	HRESULT CopyRecordset(_Recordset *pSrcRs, _Recordset *pTarRs,LPCTSTR lpszOldSql,LPCTSTR lpszNewSql);
	BOOL GetRelationSql(BSTR bstrSql,LPTSTR lpszBuf);
	HRESULT ReprotError(LPWSTR lpswError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark = NULL);
	STDMETHOD(ConnectDatabase)(BOOL bIsBackupDatabase = FALSE);
	STDMETHOD(ReConnect)(_com_error *pError);
	BOOL RegGetString(LPCTSTR lpszValueName,LPCTSTR lpszDefault,LPTSTR lpszValue, BOOL bIsBackupDatabase = FALSE, BOOL bAutoCreate = FALSE);
	void RegChangeString();
	void RegWriteString(LPCTSTR lpszValueName, LPCTSTR lpszValue, BOOL bIsBackupDatabase = FALSE);
	HRESULT ReprotError(_com_error *pError, LPWSTR lpswErrorMothod, LPCSTR lpszFileName, long lLineNo, LPWSTR lpswRemark = NULL);
	HRESULT GetRecordsetCore(BSTR bstrSQL, _Recordset **ppRs);
	HRESULT UpdateRecordsetCore(_Recordset *pRs);
	HRESULT ExecuteCommandCore(BSTR bstrCmd, unsigned long *pulAffectCount);
	HRESULT PrepareConnectPtr();

	BOOL CheckStatus();
};

#endif //__STDIODB_H_
