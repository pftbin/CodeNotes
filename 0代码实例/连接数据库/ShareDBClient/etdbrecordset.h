#pragma once

#include <atlbase.h>

enum ET_emRECORDPOS
{
	em_RPBEGIN = 0,
	em_RPCURRENT = 1,
	em_RPEND = 2
};

/**
* @brief 该类是对recordset的一个封装,便于好操作
* @class CETDBRecordset
* @date  2006-05-22
*/
class CETDBRecordset
{
public:
	CETDBRecordset(void);
	virtual ~CETDBRecordset(void);
private:
	CETDBRecordset(const CETDBRecordset &);
	CETDBRecordset & operator = (const CETDBRecordset &);
public:
	BOOL UpdateBatch(AffectEnum affect = adAffectCurrent);
	BOOL Update();
	//!无连接的Recordset操作
	BOOL	OpenNoConnection();
	BOOL	CreateNoConnection();
	void	AddField(const CString & strFieldName, AdoNS::DataTypeEnum type = adInteger, long lSize = 4, AdoNS::FieldAttributeEnum attrib = adFldUnspecified);
	void	AddField(const CString & strFieldName, VARTYPE vt, BOOL bCanBeNull = TRUE);
	void	AddByteField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddShortField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddLongField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddLongLongField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddIntField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddUByteField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddUShortField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddULongField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddULongLongField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddUIntField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddFloatField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddDoubleField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddBoolField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddDateField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddCurrencyField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddStringField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddBlobField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	void	AddDecimalField(const CString & strFieldName, BOOL bCanBeNull = TRUE);
	//!复制操作
	BOOL	CopyFrom(_Recordset* pRs);
	BOOL	CopyTo(_Recordset** ppRs);
	//!基本的Recordset操作
	BOOL Find(const CString & strWhere,long lRow = 0);
	_Recordset**	operator& ();
	operator _Recordset* ();
	operator _RecordsetPtr ();
	
	_Recordset* Detach();	

	BOOL SetRecordset(_Recordset *pRs);
	long FieldLong(const CString & sFieldName);
	long FieldLong(int iFieldNo);
	BOOL Delete(AffectEnum affect = adAffectCurrent);
	BOOL SaveFile(const CString & sFileName);
	void Close();
	long GetRecordNo();
	//!增加新记录
	BOOL AddNew();
	//!设置字段值
	void SetField(int iFieldNo,float fValue);
	//!设置字段值
	void SetField(int iFieldNo,COleDateTime odtValue);
	//!设置字段值
	void SetField(int iFieldNo,bool bValue);
	//!设置字段值
	void SetField(int iFieldNo,int iValue);
	void SetField(int iFieldNo, double dbValue);
	//!设置字段值
	void SetField(int iFieldNo,CString sValue);
	//!设置字段值
	void SetField(int iFieldNo,void* pBuf,int iSize);
	void SetField(int iFieldNo, long lValue);
	void SetField(int iFieldNo, BYTE bValue);
	void SetField(const CString & sFieldName, BYTE bValue);
	void SetField(const CString & sFieldName, long lValue);
	void SetField(const CString & sFieldName,float fValue);
	void SetField(const CString & sFieldName,COleDateTime odtValue);
	void SetField(const CString & sFieldName, double dbValue);
	void SetField(const CString & sFieldName,bool bValue);
	void SetField(const CString & sFieldName,int iValue);
	void SetField(const CString & sFieldName,CString sValue);
	void SetField(const CString & sFieldName,void* pBuf,int iSize);
	void SetField(const CString & sFieldName,DWORD dwValue);
	void SetField(const CString & sFieldName,LONGLONG llValue);
	void SetField(const CString & sFieldName,ULONGLONG ullValue);
	//!查找指定值
	BOOL Seek(const CString & sKeyValues,int iSeekType);
	//!取得字段值
	COleDateTime FieldODT(int iFieldNo);
	COleDateTime FieldODT( const CString & sFieldName);
	CString FieldString(int iFieldNo);
	double FieldDouble(int iFieldNo);
	float FieldFloat(int iFieldNo);
	BOOL FieldBool(int iFieldNo);
	int FieldInt(int iFieldNo);
	void FieldBlob(int iFieldNo,void** pBuf,long& iSize);
	//!取得字段数
	int FieldCount();
	CString FieldString(const CString & sFieldName);
	float FieldFloat(const CString & sFieldName);
	BOOL FieldBool(const CString & sFieldName);
	BYTE FieldByte(const CString & sFieldName);
	DWORD FieldDWORD(const CString & sFieldName);
	LONGLONG FieldLONGLONG(const CString & sFieldName);
	ULONGLONG FieldULONGLONG(const CString & sFieldName);
	BYTE FieldByte(int iFieldNo);
	int FieldInt(const CString & sFieldName);
	double FieldDouble(const CString & sFieldName);
	void FieldBlob(const CString & sFieldName,void** pBuf,long& iSize);
	CString FieldName(int iFieldNo);
	CString FieldName(const CString & sFieldName);
	//!排序记录
	BOOL Sort(const CString & sSort);
	//!过滤记录
	BOOL Filter(const CString & sFilter);
	//!记录指针下移
	HRESULT MoveNext();
	//!记录指针上移
	HRESULT MovePrev();
	//!记录指针移到最后
	HRESULT MoveLast();
	//!记录指针移到最前
	HRESULT MoveFirst();
	//!移动记录指针
	HRESULT Move(long lRecordNum,ET_emRECORDPOS rpMove);
	//!是否到了记录头
	BOOL IsBOF();
	//!是否到了记录尾
	BOOL IsEOF();
	//!记录集是否打开
	BOOL IsOpen();
	//!取得当前记录数
	int GetRecordCount();
	//!取得记录集指针
	_Recordset* GetRecordset();
	_Recordset** GetRecordsetHandle();

	// 判断是否是空字段
	BOOL IsNullField(CString sFieldName);

	// 检查某个字段是否存在
	BOOL IsFieldExist(CString sFieldName);
public:
	void SetAutoRelease(BOOL bFlag);
	//!取得字段值
	_variant_t FieldVariant(long lFieldNo);
	//!取得字段值
	_variant_t FieldVariant(const CString & strFieldName);
	//!设置字段值
	void SetFieldVariant(long lFieldNo, _variant_t &var);
	//! 设置字段值
	void SetFieldVariant(const CString & strFieldName, _variant_t &var);
private:
	CComPtr<_Recordset>  m_pRecordset;
	BOOL				 m_bNeedRelease;
};

