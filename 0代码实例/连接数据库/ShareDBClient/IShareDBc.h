#pragma once
#include "stdafx.h"

class IShareDBc
{
public:
	IShareDBc(){}
	~IShareDBc(){}

	virtual void Release() = 0;
	virtual BOOL IsValidShareDB() = 0;
	virtual BOOL GetCGPlayListInfo(int nID, CString& strPlayListInfo) = 0;
};