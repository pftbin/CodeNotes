/* -*- C++ -*- */

//=============================================================================
/**
*  @file    Error.h
*
*  Error.h,v 1.00 2006/12/23
*
*  @author п╩нд <xiewen@163.com>
*/
//=============================================================================

#pragma once

class CError
{
public:
	CError(void);
	~CError(void);

	void SetError(int iMajor = 0, int iMinor = 0, DWORD iErrno = -1);
	DWORD GetErrorCode(void) const;
	const char* GetErrorMessageA();
	const wchar_t* GetErrorMessageW();

private:
	struct ErrorInfo {
		int				iMajor;        // major exception categories
		int				iMinor;        // for specific error reasons
		DWORD			iErrno;        // errno returned by the system if there is any
		wchar_t			szMsg[2048];   // text error message
	};
	struct MinorErrorNode {
		int				iMajor;        // major exception categories
		int				iMinor;        // for specific error reasons
		const char*		pMsgA;
		const wchar_t*	pMsgW;
	};
	struct MajorErrorNode {
		int				iMajor;        // major exception categories
		const char*		pMsgA;
		const wchar_t*	pMsgW;
	};
	
	static MinorErrorNode s_minorMap[];
	static MajorErrorNode s_majorMap[];
	
	DWORD	m_dwTlsIdx;
};
