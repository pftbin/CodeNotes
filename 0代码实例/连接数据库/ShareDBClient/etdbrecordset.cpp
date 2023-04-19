#include "StdAfx.h"
#include ".\etdbrecordset.h"

void WrtieDebugLog(CString sInfo)
{
//	AfxMessageBox(sInfo);
}
CETDBRecordset::CETDBRecordset(void)
{
	m_pRecordset = NULL;
	m_bNeedRelease = FALSE;
}

CETDBRecordset::~CETDBRecordset(void)
{
	if(m_bNeedRelease)
		Close();
	else
		m_pRecordset.Detach();

	m_pRecordset = NULL;
}

// ETDBRecordset.cpp: implementation of the CETDBRecordset class.
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*******************************************************************
写日志函数，把指定的信息写入到日志中主要包括对数据库的增、删、查以及
出错的信息等。
*******************************************************************/


_Recordset* CETDBRecordset::GetRecordset()
{
	return m_pRecordset.p;
}

_Recordset** CETDBRecordset::GetRecordsetHandle()
{
	return &(m_pRecordset.p);
}

int CETDBRecordset::GetRecordCount()
{
	try
	{
		if(IsOpen())
			return m_pRecordset->RecordCount;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return -1;
}

BOOL CETDBRecordset::IsOpen()
{
	try
	{
		if(m_pRecordset != NULL && m_pRecordset->State != adStateClosed)
			return TRUE;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return FALSE;
}

BOOL CETDBRecordset::IsEOF()
{
	try
	{
		if(IsOpen())
			return m_pRecordset->GetadoEOF();
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return TRUE;
}

BOOL CETDBRecordset::IsBOF()
{
	try
	{
		if(IsOpen())
			return m_pRecordset->GetBOF();
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return FALSE;
}

HRESULT CETDBRecordset::Move(long lRecordNum, ET_emRECORDPOS rpMove)
{
	HRESULT hr = -1;
	try
	{
		if(IsOpen())
		{
			if(m_pRecordset->GetRecordCount() == 0&&m_pRecordset->adoEOF)
				return S_OK;
			if(rpMove == em_RPEND)
				hr = m_pRecordset->Move(lRecordNum,(_variant_t)(long)adBookmarkLast);
			else if(rpMove == em_RPCURRENT)
				hr = m_pRecordset->Move(lRecordNum,(_variant_t)(long)adBookmarkCurrent);
			else
				hr = m_pRecordset->Move(lRecordNum,(_variant_t)(long)adBookmarkFirst);
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return hr;
}

HRESULT CETDBRecordset::MoveFirst()
{
	HRESULT hr = -1;
	try
	{
		if(IsOpen())
		{
			if(!m_pRecordset->BOF)
			{
				if(m_pRecordset->GetRecordCount() == 0&&m_pRecordset->adoEOF)
					return S_OK;
				hr = m_pRecordset->MoveFirst();
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return hr;
}

HRESULT CETDBRecordset::MoveLast()
{
	HRESULT hr = -1;
	try
	{
		if(IsOpen())
		{
			if(!m_pRecordset->adoEOF) 
			{
				if(m_pRecordset->GetRecordCount() == 0&&m_pRecordset->adoEOF)
					return S_OK;
				hr = m_pRecordset->MoveLast();
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return hr;
}

HRESULT CETDBRecordset::MovePrev()
{
	HRESULT hr = -1;
	try
	{
		if(IsOpen())
		{
			if(!m_pRecordset->BOF)
			{
				if(m_pRecordset->GetRecordCount() == 0&&m_pRecordset->adoEOF)
					return S_OK;
				hr = m_pRecordset->MovePrevious();
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return hr;
}

HRESULT CETDBRecordset::MoveNext()
{
	HRESULT hr = -1;
	try
	{
		if(IsOpen())
		{
			if(m_pRecordset->GetRecordCount() == 0&&m_pRecordset->adoEOF)
				return S_OK;
			if(!m_pRecordset->adoEOF)
				hr = m_pRecordset->MoveNext();
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return hr;
}

BOOL CETDBRecordset::Filter(const CString & sFilter)
{
	try
	{
		if(IsOpen())
		{
			m_pRecordset->Filter = (_bstr_t)sFilter;
			return true;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return FALSE;
}

BOOL CETDBRecordset::Sort(const CString & sSort)
{
	try
	{
		if(IsOpen())
		{
			m_pRecordset->Sort = (_bstr_t)sSort;
			return true;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)e.Description());
	}
	return FALSE;
}

void CETDBRecordset::FieldBlob(const CString & sFieldName,void** pBuf,long& iSize)
{
	iSize = 0;
	*pBuf = NULL;
	try
	{
		if(IsOpen())
		{
			iSize = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->ActualSize;
			if(iSize > 0)
			{
				_variant_t	varBLOB;
				varBLOB = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->GetChunk(iSize);
				if(varBLOB.vt == (VT_ARRAY | VT_UI1))
				{
					if(*pBuf = new char[iSize+1])
					{	
						void *pByte = NULL;
						SafeArrayAccessData(varBLOB.parray,(void **)&pByte);
						memcpy(*pBuf,pByte,iSize);
						SafeArrayUnaccessData(varBLOB.parray);
					}
				}
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+ (LPCTSTR)e.Description());	
	}
}

CString CETDBRecordset::FieldName(const CString & sFieldName)
{
	try
	{
		CString strTemp;
		if(IsOpen())
		{
			strTemp = (const char*)m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Name;
			return strTemp;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return _T("");
}

CString CETDBRecordset::FieldName(int iFieldNo)
{
	try
	{
		CString strTemp;
		if(IsOpen())
		{
			strTemp = (const char*)m_pRecordset->Fields->GetItem((long)iFieldNo)->Name; 
			return strTemp;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return _T("");
}

int CETDBRecordset::FieldInt(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_I8);
			int nVal = (int)var.llVal;
			return nVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0;
}

BOOL CETDBRecordset::FieldBool(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return FALSE;
			var.ChangeType(VT_BOOL);
			return var.boolVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return FALSE;
}

BYTE CETDBRecordset::FieldByte(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return FALSE;
			var.ChangeType(VT_UI1);
			return var.bVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return FALSE;
}

double CETDBRecordset::FieldDouble(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0.0f;
			var.ChangeType(VT_R8);
			return var.dblVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0.0f;
}

float CETDBRecordset::FieldFloat(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0.0f;
			var.ChangeType(VT_R4);
			return var.fltVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0.0f;
}

CString CETDBRecordset::FieldString(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return _T("");
			var.ChangeType(VT_BSTR);
			return var.bstrVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return _T("");
}
long CETDBRecordset::FieldLong(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_I8);
			long lVal = (long)var.llVal;
			return lVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return 0;
}

long CETDBRecordset::FieldLong(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_I8);
			long lVal = (long)var.llVal;
			return lVal;

		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0;
}

int CETDBRecordset::FieldCount()
{
	try
	{
		if(IsOpen())
			return m_pRecordset->Fields->Count;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return -1;
}

void CETDBRecordset::FieldBlob(int iFieldNo,void** pBuf,long& iSize)
{
	iSize = 0;
	*pBuf = NULL;
	try
	{
		if(IsOpen())
		{
			iSize = m_pRecordset->Fields->GetItem((long)iFieldNo)->ActualSize;
			if(iSize > 0)
			{
				_variant_t	varBLOB;
				varBLOB = m_pRecordset->Fields->GetItem((long)iFieldNo)->GetChunk(iSize);
				if(varBLOB.vt == (VT_ARRAY | VT_UI1))
				{
					if(*pBuf = new char[iSize+1])
					{	
						void *pByte = NULL;
						SafeArrayAccessData(varBLOB.parray,(void **)&pByte);
						memcpy(*pBuf,pByte,iSize);
						SafeArrayUnaccessData(varBLOB.parray);
					}
				}
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

int CETDBRecordset::FieldInt(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_I8);
			int nVal = (int)var.llVal;
			return nVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return 0;
}

BOOL CETDBRecordset::FieldBool(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return FALSE;
			var.ChangeType(VT_BOOL);
			return var.boolVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

BYTE CETDBRecordset::FieldByte(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return FALSE;
			var.ChangeType(VT_UI1);
			return var.bVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

double CETDBRecordset::FieldDouble(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0.0f;
			var.ChangeType(VT_R8);
			return var.dblVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return 0.0f;
}

LONGLONG CETDBRecordset::FieldLONGLONG(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_I8);
			return var.llVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0;
}
ULONGLONG CETDBRecordset::FieldULONGLONG(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0;
			var.ChangeType(VT_UI8);
			if(var.llVal == -1)
				return 0;
			return var.llVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return 0;
}
DWORD CETDBRecordset::FieldDWORD(const CString & sFieldName)
{
	return FieldLong(sFieldName);
}
float CETDBRecordset::FieldFloat(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return 0.0f;
			var.ChangeType(VT_R4);
			return var.fltVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return 0.0f;
}

CString CETDBRecordset::FieldString(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return _T("");
			var.ChangeType(VT_BSTR);
			return var.bstrVal;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return _T("");
}

COleDateTime CETDBRecordset::FieldODT(const CString & sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_variant_t)sFieldName)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return COleDateTime(1800,1,1,0,0,1);
			if(var.date < -100000 ||var.date > 100000)
				return COleDateTime(1800,1,1,0,0,1);
			return (COleDateTime)var.date;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog(sFieldName+_T("|")+(LPCTSTR)e.Description());
	}
	return COleDateTime(1800,1,1,0,0,1);
}

COleDateTime CETDBRecordset::FieldODT(int iFieldNo)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((long)iFieldNo)->Value;
			if(var.vt == VT_NULL||var.vt == VT_EMPTY)
				return COleDateTime(1800,1,1,0,0,1);
			if(var.date < -100000 ||var.date > 100000)
				return COleDateTime(1800,1,1,0,0,1);
			return (COleDateTime)var.date;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return COleDateTime(1800,1,1,0,0,1);
}

BOOL CETDBRecordset::Seek(const CString & sKeyValues, int iSeekType)
{
	try
	{
		if(IsOpen())
		{
			HRESULT hr = m_pRecordset->Seek((_bstr_t)sKeyValues,adSeekFirstEQ);
			return SUCCEEDED(hr);
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

void CETDBRecordset::SetField(const CString & sFieldName,void* pBuf,int iSize)
{
	try
	{
		VARIANT			varBLOB;
		SAFEARRAY		*psa;
		SAFEARRAYBOUND	rgsabound[1];
		if(pBuf != NULL)
		{
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = iSize;
			psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
			BYTE* pByte;
			if(SafeArrayAccessData(psa,(void**)&pByte) == NOERROR)
				memcpy((LPVOID)pByte,(LPVOID)pBuf,iSize);
			SafeArrayUnaccessData(psa);

			varBLOB.vt = VT_ARRAY | VT_UI1;
			varBLOB.parray = psa;
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->AppendChunk(varBLOB);
			//			delete psa;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, CString sValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (_bstr_t)sValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName,ULONGLONG ullValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (ULONG)ullValue;  //problom
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName,LONGLONG llValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (long)llValue;  //problom
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}
void CETDBRecordset::SetField(const CString & sFieldName, int iValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (long)iValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, bool bValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = bValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, COleDateTime odtValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (_variant_t)odtValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, double dbValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = dbValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}
void CETDBRecordset::SetField(const CString & sFieldName,DWORD dwValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = (long)dwValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}
void CETDBRecordset::SetField(const CString & sFieldName, float fValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = fValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}

}

void CETDBRecordset::SetField(int iFieldNo,void* pBuf,int iSize)
{
	try
	{
		VARIANT			varBLOB;
		SAFEARRAY		*psa;
		SAFEARRAYBOUND	rgsabound[1];
		if(pBuf != NULL)
		{
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = iSize;
			psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
			BYTE* pByte;
			if(SafeArrayAccessData(psa,(void**)&pByte) == NOERROR)
				memcpy((LPVOID)pByte,(LPVOID)pBuf,iSize);
			SafeArrayUnaccessData(psa);

			varBLOB.vt = VT_ARRAY | VT_UI1;
			varBLOB.parray = psa;
			m_pRecordset->Fields->GetItem((long)iFieldNo)->AppendChunk(varBLOB);
			//			delete psa;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, CString sValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = (_bstr_t)sValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, int iValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = (long)iValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, bool bValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = bValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, COleDateTime odtValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = (_variant_t)odtValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, double dbValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = dbValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, float fValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = fValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, long lValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = lValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(int iFieldNo, BYTE bValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((long)iFieldNo)->Value = bValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, BYTE bValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = bValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetField(const CString & sFieldName, long lValue)
{
	try
	{
		if(IsOpen())
			m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value = lValue;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

BOOL CETDBRecordset::AddNew()
{
	try
	{
		if(IsOpen())
		{
			HRESULT hr = m_pRecordset->AddNew();
			return SUCCEEDED(hr);
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

long CETDBRecordset::GetRecordNo()
{
	try
	{
		if(IsOpen())
			return m_pRecordset->AbsolutePosition;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return 0;
}

void CETDBRecordset::Close()
{
	try
	{
		if(IsOpen())
		{
			m_pRecordset->Close();
			m_pRecordset.Release();
			m_pRecordset=NULL;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

BOOL CETDBRecordset::SaveFile(const CString & sFileName)
{
	try
	{
		if(IsOpen())
		{
			HRESULT hr = m_pRecordset->Save((_bstr_t)sFileName,adPersistXML);
			return SUCCEEDED(hr);
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

BOOL CETDBRecordset::Delete(AffectEnum affect)
{
	if(IsOpen())
	{
		try
		{
			m_pRecordset->Delete(affect);
			return TRUE;
		}
		catch(_com_error& e)
		{
			WrtieDebugLog((LPCTSTR)(e.Description()));
		}
	}
	return FALSE;
}

BOOL CETDBRecordset::SetRecordset(_Recordset *pRs)
{
	ASSERT(pRs);

	try
	{
		if (m_pRecordset)
		{
			m_pRecordset->Close();
			m_pRecordset.Release();
		}
		m_pRecordset.Attach(pRs);
		m_bNeedRelease = FALSE;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return TRUE;
}

// 释放对当前Recordset的拥有权
_Recordset* CETDBRecordset::Detach()
{
	if (m_pRecordset)
	{
		return m_pRecordset.Detach();
	}
	else
	{
		return NULL;
	}
}

//一般适用于从数据库中取数据，所以如果先是存在结果集这先把它释放掉
_Recordset**	CETDBRecordset::operator&()
{
	try
	{
		if (m_pRecordset)
		{
			if(!m_bNeedRelease)
				m_pRecordset.Detach();
			m_pRecordset->Close();
			m_pRecordset.Release();
		}
		m_bNeedRelease = TRUE;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return &(m_pRecordset.p);
}

//这方法主要是适用于保存数据
CETDBRecordset::operator _Recordset* ()
{
	return m_pRecordset.p;
}

CETDBRecordset::operator _RecordsetPtr ()
{
	return m_pRecordset->Clone(adLockUnspecified);
}

BOOL CETDBRecordset::Find(const CString & strWhere,long lRow)
{
	try
	{
		if(IsOpen())
		{
			if(lRow == 0)
			{
				MoveFirst();
				HRESULT hr = m_pRecordset->Find((_bstr_t)strWhere,0,adSearchForward);
				return SUCCEEDED(hr);
			}
			else
			{
				HRESULT hr = m_pRecordset->Find((_bstr_t)strWhere,lRow,adSearchForward);
				return SUCCEEDED(hr);
			}
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return FALSE;
}

//复制操作
BOOL CETDBRecordset::CopyFrom(_Recordset* pRs)
{
	try
	{
		if (!pRs)
		{
			return FALSE;
		}

		if (m_pRecordset)
		{
			if (IsOpen())
			{
				m_pRecordset->Close();
			}
			m_pRecordset.Release();
		}

		m_pRecordset = pRs->Clone(adLockUnspecified);
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
	return TRUE;
}

BOOL CETDBRecordset::CopyTo(_Recordset** ppRs)
{
	try
	{
		if (!(ppRs && *ppRs == NULL))
		{
			return FALSE;
		}

		if (!m_pRecordset)
		{
			return FALSE;
		}

		if (!IsOpen())
		{
			return FALSE;
		}

		CComPtr<_Recordset> pRs = NULL;
		pRs = m_pRecordset->Clone(adLockUnspecified);

		if (pRs)
		{
			*ppRs = pRs.Detach();
		}

		if (!(ppRs && *ppRs))
		{
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}

	return TRUE;
}

//无连接的Recordset操作
BOOL CETDBRecordset::CreateNoConnection()
{
	try
	{
		if (m_pRecordset)
		{
			if (IsOpen())
			{
				m_pRecordset->Close();
			}
			m_pRecordset.Release();
		}

		HRESULT hr = m_pRecordset.CoCreateInstance(__uuidof(Recordset), NULL, CLSCTX_ALL);
		if (FAILED(hr))
		{
			return FALSE;
		}

		m_pRecordset->CursorLocation = adUseClient;
		m_pRecordset->CursorType = adOpenKeyset;
		m_pRecordset->LockType = adLockBatchOptimistic;
		m_bNeedRelease = TRUE;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
		return FALSE;
	}

	return TRUE;
}

BOOL CETDBRecordset::OpenNoConnection()
{
	try
	{
		if (!m_pRecordset)
		{
			return FALSE;
		}

		if (!m_pRecordset->Fields->Count)
		{
			return FALSE;
		}

		if (IsOpen())
		{
			return TRUE;
		}

		HRESULT hr = m_pRecordset->Open(vtMissing, vtMissing, adOpenKeyset, adLockBatchOptimistic, -1);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
		return FALSE;
	}

	return TRUE;
}

void CETDBRecordset::AddField(const CString & strFieldName, AdoNS::DataTypeEnum type /* = adInteger */, long lSize /* = 4 */, AdoNS::FieldAttributeEnum attrib /* = adFldFixed */)
{
	try
	{
		if (IsOpen())
		{
			CreateNoConnection();
		}
		else if(m_pRecordset == NULL)
			CreateNoConnection();

		m_pRecordset->Fields->Append(_bstr_t(strFieldName), type, lSize, attrib, vtMissing);
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::AddField(const CString &  strFieldName, VARTYPE vt, BOOL bCanBeNull /* = TRUE */)
{
	AdoNS::DataTypeEnum type = adInteger;
	long lSize = 4;
	long lAttrib = adFldFixed | adFldUpdatable;

	switch (vt)
	{
	case VT_I1:
		type = adTinyInt;
		lSize = 1;
		break;
	case VT_I2:
		type = adSmallInt;
		lSize = 2;
		break;
	case VT_I4:
	case VT_INT:
		type = adInteger;
		lSize = 4;
		break;
	case VT_I8:
		type = adBigInt;
		lSize = 8;
		break;
	case VT_UI1:
		type = adUnsignedTinyInt;
		lSize = 1;
		break;
	case VT_UI2:
		type = adUnsignedSmallInt;
		lSize = 2;
		break;
	case VT_UI4:
	case VT_UINT:
		type = adUnsignedInt;
		lSize = 4;
		break;
	case VT_UI8:
		type = adUnsignedBigInt;
		lSize = 8;
		break;
	case VT_R4:
		type = adSingle;
		lSize = 4;
		break;
	case VT_R8:
		type = adDouble;
		lSize = 8;
		break;
	case VT_BOOL:
		type = adBoolean;
		lSize = 4;
		break;
	case VT_DATE:
		type = adDate;
		lSize = 8;
		break;
	case VT_CY:
		type = adCurrency;
		lSize = 8;
		break;
	case VT_BSTR:
		type = adVarWChar;
		lSize = 300;
		lAttrib = adFldLong | adFldMayBeNull | adFldIsNullable | adFldUpdatable;
		break;
	case VT_BLOB:
		type = adLongVarBinary;
		lSize = 300;
		lAttrib = adFldLong | adFldMayBeNull | adFldIsNullable | adFldUpdatable;
		break;
	case VT_DECIMAL:
		type = adDecimal;
		lSize = 16;
		break;
	default:
		return;
		break;
	}

	if (bCanBeNull)
	{
		lAttrib |= (adFldMayBeNull | adFldIsNullable);
	}

	AdoNS::FieldAttributeEnum attrib = (AdoNS::FieldAttributeEnum) lAttrib;

	AddField(strFieldName, type, lSize, attrib);
}

void CETDBRecordset::AddByteField(const CString &  strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_I1, bCanBeNull);
}

void CETDBRecordset::AddShortField(const CString &  strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_I2, bCanBeNull);
}

void CETDBRecordset::AddLongField(const CString &  strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_I4, bCanBeNull);
}

void CETDBRecordset::AddLongLongField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_I8, bCanBeNull);
}

void CETDBRecordset::AddIntField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_INT, bCanBeNull);
}

void CETDBRecordset::AddUByteField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_UI1, bCanBeNull);
}

void CETDBRecordset::AddUShortField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_UI2, bCanBeNull);
}

void CETDBRecordset::AddULongField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_UI4, bCanBeNull);
}

void CETDBRecordset::AddULongLongField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_UI8, bCanBeNull);
}

void CETDBRecordset::AddUIntField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_UINT, bCanBeNull);
}

void CETDBRecordset::AddFloatField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_R4, bCanBeNull);
}

void CETDBRecordset::AddDoubleField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_R8, bCanBeNull);
}

void CETDBRecordset::AddBoolField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_BOOL, bCanBeNull);
}

void CETDBRecordset::AddDateField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_DATE, bCanBeNull);
}

void CETDBRecordset::AddCurrencyField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_CY, bCanBeNull);
}

void CETDBRecordset::AddStringField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_BSTR, bCanBeNull);
}

void CETDBRecordset::AddBlobField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_BLOB, bCanBeNull);
}

void CETDBRecordset::AddDecimalField(const CString & strFieldName, BOOL bCanBeNull /* = TRUE */)
{
	AddField(strFieldName, VT_DECIMAL, bCanBeNull);
}

BOOL CETDBRecordset::Update()
{
	if(IsOpen())
	{
		try
		{
			HRESULT hr = m_pRecordset->UpdateBatch(adAffectAll);
			return SUCCEEDED(hr);
		}
		catch(_com_error& e)
		{
			WrtieDebugLog((LPCTSTR)(e.Description()));
		}
	}
	return TRUE;
}

BOOL CETDBRecordset::UpdateBatch(AffectEnum affect)
{
	if(IsOpen())
	{
		try
		{
			m_pRecordset->UpdateBatch(affect);
			return TRUE;
		}
		catch(_com_error& e)
		{
			WrtieDebugLog((LPCTSTR)(e.Description()));
		}
	}
	return FALSE;
}

_variant_t CETDBRecordset::FieldVariant(long lFieldNo)
{
	_variant_t var;

	try
	{
		if (IsOpen())
		{
			var = m_pRecordset->Fields->GetItem(lFieldNo)->Value;
		}
	}
	catch (_com_error& e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}

	return var;
}

_variant_t CETDBRecordset::FieldVariant(const CString & strFieldName)
{
	_variant_t var;

	try
	{
		if (IsOpen())
		{
			var = m_pRecordset->Fields->GetItem((_bstr_t) strFieldName)->Value;
		}
	}
	catch (_com_error& e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}

	return var;
}

void CETDBRecordset::SetFieldVariant(long lFieldNo, _variant_t &var)
{
	try
	{
		if (IsOpen())
		{
			m_pRecordset->Fields->GetItem(lFieldNo)->Value = var;
		}
	}
	catch (_com_error& e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetFieldVariant(const CString & strFieldName, _variant_t &var)
{
	try
	{
		if (IsOpen())
		{
			m_pRecordset->Fields->GetItem((_bstr_t) strFieldName)->Value = var;
		}
	}
	catch (_com_error& e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
	}
}

void CETDBRecordset::SetAutoRelease(BOOL bFlag)
{
	m_bNeedRelease = bFlag;
}

BOOL CETDBRecordset::IsNullField(CString sFieldName)
{
	try
	{
		if(IsOpen())
		{
			_variant_t var = m_pRecordset->Fields->GetItem((_bstr_t)sFieldName)->Value;
			if(var.vt == VT_NULL)
				return TRUE;
			else
				return FALSE;
		}

		return FALSE;
	}
	catch(_com_error &e)
	{
		WrtieDebugLog((LPCTSTR)(e.Description()));
		return FALSE;
	}	
}

BOOL CETDBRecordset::IsFieldExist(CString sFieldName)
{
	try
	{
		if(IsOpen())
		{
			long lCount,i;
			_bstr_t bstrFieldName;
			CString strName,strFieldName;
			strFieldName = sFieldName;
			strFieldName.MakeLower();
			lCount = m_pRecordset->Fields->GetCount();
			for(i = 0; i < lCount;i++)
			{
				bstrFieldName = m_pRecordset->Fields->GetItem(i)->Name;
				strName = (LPCTSTR)bstrFieldName;
				strName.MakeLower();
				if(strName == strFieldName)
					return TRUE;
			}
		}
		return FALSE;
	}
	catch(_com_error e)
	{
		//WrtieDebugLog((LPCTSTR)(e.Description()));
		return FALSE;
	}
	return FALSE;
}
