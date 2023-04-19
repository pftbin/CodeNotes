#pragma once
#include <vector>

using namespace std;

typedef struct _tagPIPECMD
{
	CString		strMessageID;
	CString		strReceive;

	_tagPIPECMD()
	{
		strMessageID = _T("");
		strReceive	 = _T("");
	}

	const _tagPIPECMD& operator=(const _tagPIPECMD& p)
	{
		strMessageID		= p.strMessageID;
		strReceive			= p.strReceive;
		return *this;
	}
}PIPECMD,*PPIPECMD;

#define BUFLENGTH	51200
