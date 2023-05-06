#pragma once
#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename_namespace("AdoNS") rename("EOF","adoEOF")
using namespace AdoNS;


class C_DBAccess
{
public:
    C_DBAccess();
    ~C_DBAccess();

    BOOL Connect(LPCTSTR lpszConnectionString);
    BOOL Execute(LPCTSTR lpszSQL);
    _RecordsetPtr Query(LPCTSTR lpszSQL);

private:
    _ConnectionPtr m_pConnection; // ADO连接对象指针
};

