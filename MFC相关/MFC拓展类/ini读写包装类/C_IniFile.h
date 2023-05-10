#pragma once
 
class C_IniFile
{
public:
	C_IniFile();
	virtual ~C_IniFile();
 
	//初始化INI
	C_IniFile(LPCTSTR szFileName);
 
	//创建一个INI文件
	void SetFileName(LPCTSTR szFileName);
 
	//写操作
	void WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int nValue = 0);
	void WriteFloat(LPCTSTR szSection, LPCTSTR szKey, float fValue = 0.0);
	void WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, BOOL bValue = FALSE);
	void WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue = _T(""));
 
	//读操作
	int ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int nDefaultValue = 0);
	float ReadFloat(LPCTSTR szSection, LPCTSTR szKey, float fDefaultValue = 0.0);
	BOOL ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, BOOL bDefaultValue = FALSE);
	CString ReadString(LPCTSTR szSection, LPCTSTR szKey, const LPCTSTR szDefaultValue = _T(""));
 
	//删除节、ID键
	void DeleteSection(LPCTSTR szSection);
	void DeleteKey(LPCTSTR szSection, LPCTSTR szKey);
 
private:
	TCHAR m_szFileName[MAX_PATH];
};
 