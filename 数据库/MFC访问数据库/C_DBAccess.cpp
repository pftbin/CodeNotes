#include "pch.h"
#include "C_DBAccess.h"

C_DBAccess::C_DBAccess()
{
    CoInitialize(NULL);//³õÊ¼»¯COM¿â
    try {
        HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }
    }
    catch (_com_error& e) {
        CString strErrorMsg;
        strErrorMsg.Format(_T("Error creating ADO connection object: %s"), e.ErrorMessage());
        AfxMessageBox(strErrorMsg, MB_ICONERROR);
    }
}

C_DBAccess::~C_DBAccess()
{
    if (m_pConnection)
    {
        long lState;
        m_pConnection->get_State(&lState);
        if (lState != adStateClosed)
        {
            m_pConnection->Close();
        }
        m_pConnection.Release();
    }
    CoUninitialize();//Ð¶ÔØCOM¿â
}

BOOL C_DBAccess::Connect(LPCTSTR lpszConnectionString)
{
    try 
    {
        m_pConnection->Open(lpszConnectionString, "", "", adConnectUnspecified);
        return TRUE;
    }
    catch (_com_error& e) 
    {
        if (m_pConnection)
        {
            long lState;
            m_pConnection->get_State(&lState);
            if (lState != adStateClosed)
            {
                m_pConnection->Close();
            }
            m_pConnection.Release();
        }

        CString strErrorMsg;strErrorMsg.Format(_T("Error connecting to database: %s"), e.ErrorMessage());
        AfxMessageBox(strErrorMsg, MB_ICONERROR);
        return FALSE;
    }
}

BOOL C_DBAccess::Execute(LPCTSTR lpszSQL)
{
    try 
    {
        m_pConnection->Execute(lpszSQL, NULL, adCmdText);
        return TRUE;
    }
    catch (_com_error& e) 
    {
        if (m_pConnection)
        {
            long lState;
            m_pConnection->get_State(&lState);
            if (lState != adStateClosed)
            {
                m_pConnection->Close();
            }
            m_pConnection.Release();
        }

        CString strErrorMsg;strErrorMsg.Format(_T("Error executing SQL statement: %s"), e.ErrorMessage());
        AfxMessageBox(strErrorMsg, MB_ICONERROR);
        return FALSE;
    }
}

_RecordsetPtr C_DBAccess::Query(LPCTSTR lpszSQL)
{
    _RecordsetPtr pRecordset = NULL;
    try 
    {
        pRecordset.CreateInstance(__uuidof(Recordset));
        pRecordset->Open(lpszSQL, m_pConnection.GetInterfacePtr(), adOpenStatic, adLockReadOnly, adCmdText);
        return pRecordset;
    }
    catch (_com_error& e) 
    {
        if (m_pConnection)
        {
            long lState;
            m_pConnection->get_State(&lState);
            if (lState != adStateClosed)
            {
                m_pConnection->Close();
            }
            m_pConnection.Release();
        }

        CString strErrorMsg;strErrorMsg.Format(_T("Error executing SQL query: %s"), e.ErrorMessage());
        AfxMessageBox(strErrorMsg, MB_ICONERROR);
        return NULL;
    }
}
