// Markup.h: interface for the C_BurXMLFile class.
//
// Markup Release 10.1
// Copyright (C) 2008 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest C_BurXMLFile and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#if !defined(_MARKUP_H_INCLUDED_)
#define _MARKUP_H_INCLUDED_

#include <stdlib.h>
#include <string.h> // memcpy, memset, strcmp...

// Major build options
// MARKUP_WCHAR wide char (2-byte UTF-16 on Windows, 4-byte UTF-32 on Linux and OS X)
// MARKUP_MBCS ANSI/double-byte strings on Windows
// MARKUP_STL (default except VC++) use STL strings instead of MFC strings
// MARKUP_SAFESTR to use string _s functions in VC++ 2005 (_MSC_VER >= 1400)
// MARKUP_WINCONV (default for VC++) for Windows API character conversion
// MARKUP_ICONV (default for GNU) for character conversion on Linux and OS X and other platforms
// MARKUP_STDCONV to use neither WINCONV or ICONV, falls back to setlocale based conversion for ANSI
//
#if _MSC_VER > 1000 // VC++
#pragma once
#if ! defined(MARKUP_SAFESTR) // not VC++ safe strings
#pragma warning(disable:4996) // VC++ 2005 deprecated function warnings
#endif // not VC++ safe strings
#if defined(MARKUP_STL) && _MSC_VER < 1400 // STL pre VC++ 2005
#pragma warning(disable:4786) // std::string long names
#endif // VC++ 2005 STL
#else // not VC++
#if ! defined(MARKUP_STL)
#define MARKUP_STL
#endif // not STL
#if defined(__GNUC__) && ! defined(MARKUP_ICONV) && ! defined(MARKUP_STDCONV) && ! defined(MARKUP_WINCONV)
#define MARKUP_ICONV
#endif // GNUC and not ICONV not STDCONV not WINCONV
#endif // not VC++
#if (defined(_UNICODE) || defined(UNICODE)) && ! defined(MARKUP_WCHAR)
#define MARKUP_WCHAR
#endif // _UNICODE or UNICODE
#if (defined(_MBCS) || defined(MBCS)) && ! defined(MARKUP_MBCS)
#define MARKUP_MBCS
#endif // _MBCS and not MBCS
#if ! defined(MARKUP_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
#define MARKUP_SIZEOFWCHAR 4
#else // sizeof(wchar_t) != 4
#define MARKUP_SIZEOFWCHAR 2
#endif // sizeof(wchar_t) != 4
#endif // not MARKUP_SIZEOFWCHAR
#if ! defined(MARKUP_WINCONV) && ! defined(MARKUP_STDCONV) && ! defined(MARKUP_ICONV)
#define MARKUP_WINCONV
#endif // not WINCONV not STDCONV not ICONV

// Text type and function defines (compiler and build-option dependent)
// 
#define MCD_ACP 0
#define MCD_UTF8 65001
#define MCD_UTF16 1200
#define MCD_UTF32 65005
#if defined(MARKUP_WCHAR)
#define MCD_CHAR wchar_t
#define MCD_PCSZ const wchar_t*
#define MCD_PSZLEN (int)wcslen
#define MCD_PSZCHR wcschr
#define MCD_PSZSTR wcsstr
#define MCD_PSZTOL wcstol
#define MCD_PSZNCMP wcsncmp
#if defined(MARKUP_SAFESTR) // VC++ safe strings
#define MCD_SSZ(sz) sz,(sizeof(sz)/sizeof(MCD_CHAR))
#define MCD_PSZCPY(sz,p) wcscpy_s(MCD_SSZ(sz),p)
#define MCD_PSZNCPY(sz,p,n) wcsncpy_s(MCD_SSZ(sz),p,n)
#define MCD_PSZCAT(sz,p) wcscat_s(MCD_SSZ(sz),p)
#define MCD_SPRINTF swprintf_s
#define MCD_FOPEN(f,n,m) {if(_wfopen_s(&f,n,m)!=0)f=NULL;}
#else // not VC++ safe strings
#if defined(__GNUC__)
#define MCD_SSZ(sz) sz,(sizeof(sz)/sizeof(MCD_CHAR))
#else // not GNUC
#define MCD_SSZ(sz) sz
#endif // not GNUC
#define MCD_PSZCPY wcscpy
#define MCD_PSZNCPY wcsncpy
#define MCD_PSZCAT wcscat
#define MCD_SPRINTF swprintf
#define MCD_FOPEN(f,n,m) f=_wfopen(n,m)
#endif // not VC++ safe strings
#define MCD_T(s) L ## s
#if MARKUP_SIZEOFWCHAR == 4 // sizeof(wchar_t) == 4
#define MCD_ENC MCD_T("UTF-32")
#else // sizeof(wchar_t) == 2
#define MCD_ENC MCD_T("UTF-16")
#endif
#define MCD_CLEN(p) 1
#else // not MARKUP_WCHAR
#define MCD_CHAR char
#define MCD_PCSZ const char*
#define MCD_PSZLEN (int)strlen
#define MCD_PSZCHR strchr
#define MCD_PSZSTR strstr
#define MCD_PSZTOL strtol
#define MCD_PSZNCMP strncmp
#if defined(MARKUP_SAFESTR) // VC++ safe strings
#define MCD_SSZ(sz) sz,(sizeof(sz)/sizeof(MCD_CHAR))
#define MCD_PSZCPY(sz,p) strcpy_s(MCD_SSZ(sz),p)
#define MCD_PSZNCPY(sz,p,n) strncpy_s(MCD_SSZ(sz),p,n)
#define MCD_PSZCAT(sz,p) strcat_s(MCD_SSZ(sz),p)
#define MCD_SPRINTF sprintf_s
#define MCD_FOPEN(f,n,m) {if(fopen_s(&f,n,m)!=0)f=NULL;}
#else // not VC++ safe strings
#define MCD_SSZ(sz) sz
#define MCD_PSZCPY strcpy
#define MCD_PSZNCPY strncpy
#define MCD_PSZCAT strcat
#define MCD_SPRINTF sprintf
#define MCD_FOPEN(f,n,m) f=fopen(n,m)
#endif // not VC++ safe strings
#define MCD_T(s) s
#if defined(MARKUP_MBCS) // MBCS/double byte
#define MCD_ENC MCD_T("")
#if defined(MARKUP_WINCONV)
#define MCD_CLEN(p) (int)_mbclen((const unsigned char*)p)
#else // not WINCONV
#define MCD_CLEN(p) (int)mblen(p,MB_CUR_MAX)
#endif // not WINCONV
#else // not MBCS/double byte
#define MCD_ENC MCD_T("UTF-8")
#define MCD_CLEN(p) 1
#endif // not MBCS/double byte
#endif // not MARKUP_WCHAR
#if _MSC_VER < 1000 // not VC++
#define MCD_STRERROR strerror(errno)
#endif // not VC++

// String type and function defines (compiler and build-option dependent)
// Define MARKUP_STL to use STL strings
//
#if defined(MARKUP_STL) // STL
#include <string>
#if defined(MARKUP_WCHAR)
#define MCD_STR std::wstring
#else // not MARKUP_WCHAR
#define MCD_STR std::string
#endif // not MARKUP_WCHAR
#define MCD_2PCSZ(s) s.c_str()
#define MCD_STRLENGTH(s) (int)s.size()
#define MCD_STRCLEAR(s) s.erase()
#define MCD_STRISEMPTY(s) s.empty()
#define MCD_STRMID(s,n,l) s.substr(n,l)
#define MCD_STRASSIGN(s,p,n) s.assign(p,n)
#define MCD_STRCAPACITY(s) (int)s.capacity()
#define MCD_STRINSERTREPLACE(d,i,r,s) d.replace(i,r,s)
#define MCD_GETBUFFER(s,n) new MCD_CHAR[n+1]; s.reserve(n)
#define MCD_RELEASEBUFFER(s,p,n) s.assign(p,n); delete[]p
#define MCD_BLDRESERVE(s,n) s.reserve(n)
#define MCD_BLDCHECK(s,n,d) ;
#define MCD_BLDRELEASE(s) ;
#define MCD_BLDAPPENDN(s,p,n) s.append(p,n)
#define MCD_BLDAPPEND(s,p) s.append(p)
#define MCD_BLDAPPEND1(s,c) s+=(MCD_CHAR)(c)
#else // not STL, i.e. MFC
/*#include <afx.h>*/
#define MCD_STR CString
#define MCD_2PCSZ(s) ((MCD_PCSZ)s)
#define MCD_STRLENGTH(s) s.GetLength()
#define MCD_STRCLEAR(s) s.Empty()
#define MCD_STRISEMPTY(s) s.IsEmpty()
#define MCD_STRMID(s,n,l) s.Mid(n,l)
#define MCD_STRASSIGN(s,p,n) memcpy(s.GetBuffer(n),p,(n)*sizeof(MCD_CHAR));s.ReleaseBuffer(n);
#define MCD_STRCAPACITY(s) (((CStringData*)((MCD_PCSZ)s)-1)->nAllocLength)
#define MCD_GETBUFFER(s,n) s.GetBuffer(n)
#define MCD_RELEASEBUFFER(s,p,n) s.ReleaseBuffer(n)
#define MCD_BLDRESERVE(s,n) MCD_CHAR*pD=s.GetBuffer(n); int nL=0
#define MCD_BLDCHECK(s,n,d) if(nL+(int)(d)>n){s.ReleaseBuffer(nL);n<<=2;pD=s.GetBuffer(n);}
#define MCD_BLDRELEASE(s) s.ReleaseBuffer(nL)
#define MCD_BLDAPPENDN(s,p,n) MCD_PSZNCPY(&pD[nL],p,n);nL+=n
#define MCD_BLDAPPEND(s,p) MCD_PSZCPY(&pD[nL],p);nL+=MCD_PSZLEN(p)
#define MCD_BLDAPPEND1(s,c) pD[nL++]=(MCD_CHAR)(c)
#endif // not STL
#define MCD_STRTOINT(s) MCD_PSZTOL(MCD_2PCSZ(s),NULL,10)

// Allow function args to accept string objects as constant string pointers
struct MCD_CSTR
{
	MCD_CSTR() { pcsz=NULL; };
	MCD_CSTR( MCD_PCSZ p ) { pcsz=p; };
	MCD_CSTR( const MCD_STR& s ) { pcsz = MCD_2PCSZ(s); };
	operator MCD_PCSZ() const { return pcsz; };
	MCD_PCSZ pcsz;
};

// On Linux and OS X, filenames are not specified in wchar_t
#if defined(MARKUP_WCHAR) && defined(__GNUC__)
#undef MCD_FOPEN
#define MCD_FOPEN(f,n,m) f=fopen(n,m)
#define MCD_T_FILENAME(s) s
#define MCD_PCSZ_FILENAME const char*
struct MCD_CSTR_FILENAME
{
	MCD_CSTR_FILENAME() { pcsz=NULL; };
	MCD_CSTR_FILENAME( MCD_PCSZ_FILENAME p ) { pcsz=p; };
	MCD_CSTR_FILENAME( const std::string& s ) { pcsz = s.c_str(); };
	operator MCD_PCSZ_FILENAME() const { return pcsz; };
	MCD_PCSZ_FILENAME pcsz;
};
#else
#define MCD_CSTR_FILENAME MCD_CSTR
#define MCD_T_FILENAME MCD_T
#define MCD_PCSZ_FILENAME MCD_PCSZ
#endif

#if defined(_DEBUG) // DEBUG 
#define _DS(i) (i?&(MCD_2PCSZ(m_strDoc))[m_aPos[i].nStart]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else // not DEBUG
#define MARKUP_SETDEBUGSTATE
#endif // not DEBUG

class C_BurXMLFile  
{
public:
	C_BurXMLFile() 
	{ 
	#if defined(_DEBUG)
		m_pMainDS = NULL;
		m_pChildDS = NULL;
	#endif
		m_iPosParent = 0;
		m_iPos = 0;
		m_iPosChild = 0;
		m_iPosFree = 0;
		m_iPosDeleted = 0;
		m_nNodeType = 0;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nDocFlags = 0;
		SetDoc( NULL );
		InitDocFlags();
	};
	C_BurXMLFile( MCD_CSTR szDoc ) 
	{ 
		#if defined(_DEBUG)
		m_pMainDS = NULL;
		m_pChildDS = NULL;
		#endif
		m_iPosParent = 0;
		m_iPos = 0;
		m_iPosChild = 0;
		m_iPosFree = 0;
		m_iPosDeleted = 0;
		m_nNodeType = 0;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nDocFlags = 0;
		SetDoc( szDoc ); 
		InitDocFlags(); 
	};
	C_BurXMLFile( int nFlags ) 
	{ 
	#if defined(_DEBUG)
		m_pMainDS = NULL;
		m_pChildDS = NULL;
	#endif
		m_iPosParent = 0;
		m_iPos = 0;
		m_iPosChild = 0;
		m_iPosFree = 0;
		m_iPosDeleted = 0;
		m_nNodeType = 0;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nDocFlags = 0;
		SetDoc( NULL ); 
		m_nDocFlags = nFlags;
	};
	C_BurXMLFile( const C_BurXMLFile& markup ) { *this = markup; };
	C_BurXMLFile& operator=( const C_BurXMLFile& markup );
	~C_BurXMLFile() {};

	// Navigate
	bool Load( MCD_CSTR_FILENAME szFileName );
	bool SetDoc( MCD_PCSZ pDoc );
	bool SetDoc( const MCD_STR& strDoc );
	bool IsWellFormed();
	bool FindElem( MCD_CSTR szName=NULL );
	bool FindChildElem( MCD_CSTR szName=NULL );
	bool IntoElem();
	bool OutOfElem();
	void ResetChildPos() { x_SetPos(m_iPosParent,m_iPos,0); };
	void ResetMainPos() { x_SetPos(m_iPosParent,0,0); };
	void ResetPos() { x_SetPos(0,0,0); };
	MCD_STR GetTagName() const;
	MCD_STR GetChildTagName() const { return x_GetTagName(m_iPosChild); };
	MCD_STR GetData() const { return x_GetData(m_iPos); };
	MCD_STR GetChildData() const { return x_GetData(m_iPosChild); };
	MCD_STR GetElemContent() const { return x_GetElemContent(m_iPos); };
	MCD_STR GetAttrib( MCD_CSTR szAttrib ) const { return x_GetAttrib(m_iPos,szAttrib); };
	MCD_STR GetChildAttrib( MCD_CSTR szAttrib ) const { return x_GetAttrib(m_iPosChild,szAttrib); };
	MCD_STR GetAttribName( int n ) const;
	int FindNode( int nType=0 );
	int GetNodeType() { return m_nNodeType; };
	bool SavePos( MCD_CSTR szPosName=MCD_T(""), int nMap = 0 );
	bool RestorePos( MCD_CSTR szPosName=MCD_T(""), int nMap = 0 );
	bool SetMapSize( int nSize, int nMap = 0 );
	const MCD_STR& GetError() const { return m_strError; };
	int GetDocFlags() const { return m_nDocFlags; };
	void SetDocFlags( int nFlags ) { m_nDocFlags = nFlags; };
	enum MarkupDocFlags
	{
		MDF_UTF16LEFILE = 1,
		MDF_UTF8PREAMBLE = 4,
		MDF_IGNORECASE = 8,
		MDF_READFILE = 16,
		MDF_WRITEFILE = 32,
		MDF_APPENDFILE = 64,
		MDF_UTF16BEFILE = 128
	};
	enum MarkupNodeFlags
	{
		MNF_WITHCDATA      = 0x01,
		MNF_WITHNOLINES    = 0x02,
		MNF_WITHXHTMLSPACE = 0x04,
		MNF_WITHREFS       = 0x08,
		MNF_WITHNOEND      = 0x10,
		MNF_ESCAPEQUOTES  = 0x100,
		MNF_NONENDED   = 0x100000,
		MNF_ILLDATA    = 0x200000
	};
	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
		MNT_LONE_END_TAG			= 128,// 0x80
		MNT_NODE_ERROR              = 32768 // 0x8000
	};

	// Create
	bool Save( MCD_CSTR_FILENAME szFileName );
	const MCD_STR& GetDoc() const { return m_strDoc; };
	bool AddElem( MCD_CSTR szName, MCD_CSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags); };
	bool InsertElem( MCD_CSTR szName, MCD_CSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_INSERT); };
	bool AddChildElem( MCD_CSTR szName, MCD_CSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_CHILD); };
	bool InsertChildElem( MCD_CSTR szName, MCD_CSTR szData=NULL, int nFlags=0 ) { return x_AddElem(szName,szData,nFlags|MNF_INSERT|MNF_CHILD); };
	bool AddElem( MCD_CSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags); };
	bool InsertElem( MCD_CSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_INSERT); };
	bool AddChildElem( MCD_CSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_CHILD); };
	bool InsertChildElem( MCD_CSTR szName, int nValue, int nFlags=0 ) { return x_AddElem(szName,nValue,nFlags|MNF_INSERT|MNF_CHILD); };
	bool AddAttrib( MCD_CSTR szAttrib, MCD_CSTR szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool AddChildAttrib( MCD_CSTR szAttrib, MCD_CSTR szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool AddAttrib( MCD_CSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
	bool AddChildAttrib( MCD_CSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
	bool AddSubDoc( MCD_CSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,0); };
	bool InsertSubDoc( MCD_CSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_INSERT); };
	MCD_STR GetSubDoc() const { return x_GetSubDoc(m_iPos); };
	bool AddChildSubDoc( MCD_CSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_CHILD); };
	bool InsertChildSubDoc( MCD_CSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,MNF_CHILD|MNF_INSERT); };
	MCD_STR GetChildSubDoc() const { return x_GetSubDoc(m_iPosChild); };
	bool AddNode( int nType, MCD_CSTR szText ) { return x_AddNode(nType,szText,0); };
	bool InsertNode( int nType, MCD_CSTR szText ) { return x_AddNode(nType,szText,MNF_INSERT); };

	// Modify
	bool RemoveElem();
	bool RemoveChildElem();
	bool RemoveNode();
	bool SetAttrib( MCD_CSTR szAttrib, MCD_CSTR szValue, int nFlags=0 ) { return x_SetAttrib(m_iPos,szAttrib,szValue,nFlags); };
	bool SetChildAttrib( MCD_CSTR szAttrib, MCD_CSTR szValue, int nFlags=0 ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue,nFlags); };
	bool SetAttrib( MCD_CSTR szAttrib, int nValue, int nFlags=0 ) { return x_SetAttrib(m_iPos,szAttrib,nValue,nFlags); };
	bool SetChildAttrib( MCD_CSTR szAttrib, int nValue, int nFlags=0 ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue,nFlags); };
	bool SetData( MCD_CSTR szData, int nFlags=0 ) { return x_SetData(m_iPos,szData,nFlags); };
	bool SetChildData( MCD_CSTR szData, int nFlags=0 ) { return x_SetData(m_iPosChild,szData,nFlags); };
	bool SetData( int nValue ) { return x_SetData(m_iPos,nValue); };
	bool SetChildData( int nValue ) { return x_SetData(m_iPosChild,nValue); };
	bool SetElemContent( MCD_CSTR szContent ) { return x_SetElemContent(szContent); };

	// Utility
	static bool ReadTextFile( MCD_CSTR_FILENAME szFileName, MCD_STR& strDoc, MCD_STR* pstrError=NULL, int* pnDocFlags=NULL, MCD_STR* pstrEncoding=NULL );
	static bool WriteTextFile( MCD_CSTR_FILENAME szFileName, const MCD_STR& strDoc, MCD_STR* pstrError=NULL, int* pnDocFlags=NULL, MCD_STR* pstrEncoding=NULL );
	static MCD_STR EscapeText( MCD_CSTR szText, int nFlags = 0 );
	static MCD_STR UnescapeText( MCD_CSTR szText, int nTextLength = -1 );
	static int UTF16To8( char *pszUTF8, const unsigned short* pwszUTF16, int nUTF8Count );
	static int UTF8To16( unsigned short* pwszUTF16, const char* pszUTF8, int nUTF8Count );
	static MCD_STR UTF8ToA( MCD_CSTR pszUTF8, int* pnFailed = NULL );
	static MCD_STR AToUTF8( MCD_CSTR pszANSI );
	static void EncodeCharUTF8( int nUChar, char* pszUTF8, int& nUTF8Len );
	static int DecodeCharUTF8( const char*& pszUTF8, const char* pszUTF8End = NULL );
	static void EncodeCharUTF16( int nUChar, unsigned short* pwszUTF16, int& nUTF16Len );
	static int DecodeCharUTF16( const unsigned short*& pwszUTF16, const unsigned short* pszUTF16End = NULL );
	static bool DetectUTF8( const char* pText, int nTextLen, int* pnNonASCII = NULL );
	static MCD_STR GetDeclaredEncoding( MCD_CSTR szDoc );

protected:

#if defined(_DEBUG)
	MCD_PCSZ m_pMainDS;
	MCD_PCSZ m_pChildDS;
#endif // DEBUG

	MCD_STR m_strDoc;
	MCD_STR m_strError;

	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_iPosDeleted;
	int m_nNodeType;
	int m_nNodeOffset;
	int m_nNodeLength;
	int m_nDocFlags;

	struct ElemPos
	{
		ElemPos() {
			nStart = 0;
			nLength = 0;
			nStartTagLen = 0; 
			nEndTagLen = 0; 
			nFlags = 0;
			iElemParent = 0;
			iElemChild = 0;
			iElemNext = 0; 
			iElemPrev = 0; 
		};
		ElemPos( const ElemPos& pos ) { *this = pos; };
		int StartTagLen() const { return nStartTagLen; };
		void SetStartTagLen( int n ) { nStartTagLen = n; };
		void AdjustStartTagLen( int n ) { nStartTagLen += n; };
		int EndTagLen() const { return nEndTagLen; };
		void SetEndTagLen( int n ) { nEndTagLen = n; };
		bool IsEmptyElement() { return (StartTagLen()==nLength)?true:false; };
		int StartContent() const { return nStart + StartTagLen(); };
		int ContentLen() const { return nLength - StartTagLen() - EndTagLen(); };
		int StartAfter() const { return nStart + nLength; };
        int Level() const { return nFlags & 0xffff; };
        void SetLevel( int nLev ) { nFlags = (nFlags & ~0xffff) | nLev; };
		void ClearVirtualParent() { memset(this,0,sizeof(ElemPos)); };

		// Memory size: 8 32-bit integers == 32 bytes
		int nStart;
		int nLength;
		unsigned int nStartTagLen : 22; // 4MB limit for start tag
		unsigned int nEndTagLen : 10; // 1K limit for end tag
        int nFlags; // 16 bits flags, 16 bits level 65536 depth limit
		int iElemParent;
		int iElemChild; // first child
		int iElemNext; // next sibling
		int iElemPrev; // if this is first, iElemPrev points to last
	};

	enum MarkupNodeFlagsInternal
	{
		MNF_REPLACE    = 0x001000,
		MNF_INSERT     = 0x002000,
		MNF_CHILD      = 0x004000,
		MNF_QUOTED     = 0x008000,
		MNF_EMPTY      = 0x010000,
		MNF_DELETED    = 0x020000,
		MNF_FIRST      = 0x080000,
		MNF_PUBLIC     = 0x300000,
		MNF_ILLFORMED  = 0x800000,
		MNF_USER      = 0xf000000
	};

	struct NodePos
	{
		NodePos() {
			nNodeType = 0;
			nStart = 0;
			nLength = 0;
			nNodeFlags = 0;
			strMeta = "";
		};
		NodePos( int n ) { nNodeFlags=n; nNodeType=0; nStart=0; nLength=0; };
		int nNodeType;
		int nStart;
		int nLength;
		int nNodeFlags;
		MCD_STR strMeta;
	};

	struct TokenPos
	{
		TokenPos( MCD_CSTR sz, int n ) { 
			Clear(); 
			nL = 0;
			nR = 0;
			nNext = 0;
			pDoc=sz; 
			nTokenFlags=n; 
			nPreSpaceStart = 0;
			nPreSpaceLength = 0;};
		void Clear() { nL=0; nR=-1; nNext=0; };
		int Length() const { return nR - nL + 1; };
		int StrNIACmp( MCD_PCSZ p1, MCD_PCSZ p2, int n )
		{
			// string compare ignore case
			bool bNonA = false;
			MCD_CHAR c1, c2;
			while ( n-- )
			{
				c1 = *p1++; c2 = *p2++;
				if ( c1 != c2 )
				{
					if (bNonA || !((c1>='a'&&c1<='z'&&c1==c2+('a'-'A'))||(c2>='a'&&c2<='z'&&c2==c1+('a'-'A'))))
						return c1 - c2;
				}
				else if ( (unsigned int)c1 > 127 )
					bNonA = true;
			}
			return 0;
		}

		bool Match( MCD_CSTR szName )
		{
			int nLen = nR - nL + 1;
			if ( nTokenFlags & MDF_IGNORECASE )
				return ( (StrNIACmp( &pDoc[nL], szName, nLen ) == 0)
					&& ( szName[nLen] == '\0' || MCD_PSZCHR(MCD_T(" =/[]"),szName[nLen]) ) );
			else
				return ( (MCD_PSZNCMP( &pDoc[nL], szName, nLen ) == 0)
					&& ( szName[nLen] == '\0' || MCD_PSZCHR(MCD_T(" =/[]"),szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		MCD_PCSZ pDoc;
		int nTokenFlags;
		int nPreSpaceStart;
		int nPreSpaceLength;
	};

	struct SavedPos
	{
		// SavedPos is an entry in the SavedPosMap hash table
		SavedPos() { nSavedPosFlags=0; iPos=0; };
		MCD_STR strName;
		int iPos;
		enum { SPM_MAIN = 1, SPM_CHILD = 2, SPM_USED = 4, SPM_LAST = 8 };
		int nSavedPosFlags;
	};

	struct SavedPosMap
	{
		// SavedPosMap is only created if SavePos/RestorePos are used
		SavedPosMap( int nSize ) { nMapSize=nSize; pTable = new SavedPos*[nSize]; memset(pTable,0,nSize*sizeof(SavedPos*)); };
		~SavedPosMap() { if (pTable) { for (int n=0;n<nMapSize;++n) if (pTable[n]) delete[] pTable[n]; delete[] pTable; } };
		SavedPos** pTable;
		int nMapSize;
	};

	struct SavedPosMapArray
	{
		// SavedPosMapArray keeps pointers to SavedPosMap instances
		SavedPosMapArray() { pMaps = NULL; };
		~SavedPosMapArray() { RemoveAll(); };
		void RemoveAll() { SavedPosMap**p = pMaps; if (p) { while (*p) delete *p++; delete[] pMaps; pMaps=NULL; } };
		SavedPosMap** pMaps; // NULL terminated array
	};
	SavedPosMapArray m_SavedPosMapArray;

	struct PosArray
	{
		PosArray() { Clear(); };
		~PosArray() { Release(); };
		enum { PA_SEGBITS = 16, PA_SEGMASK = 0xffff };
		void RemoveAll() { Release(); Clear(); };
		void Release() { for (int n=0;n<SegsUsed();++n) delete[] (char*)pSegs[n]; if (pSegs) delete[] (char*)pSegs; };
		void Clear() { nSegs=0; nSize=0; pSegs=NULL; };
		int GetSize() const { return nSize; };
		int SegsUsed() const { return ((nSize-1)>>PA_SEGBITS) + 1; };
		ElemPos& operator[](int n) const { return pSegs[n>>PA_SEGBITS][n&PA_SEGMASK]; };
		ElemPos** pSegs;
		int nSize;
		int nSegs;
	};
	PosArray m_aPos;

	struct NodeStack
	{
		NodeStack() { nTop=-1; nSize=0; pN=NULL; };
		~NodeStack() { if (pN) delete [] pN; };
		NodePos& Top() { return pN[nTop]; };
		NodePos& At( int n ) { return pN[n]; };
		void Add() { ++nTop; if (nTop==nSize) Alloc(nSize*2+6); };
		void Remove() { --nTop; };
		int TopIndex() { return nTop; };
	protected:
		void Alloc( int nNewSize ) { NodePos* pNNew = new NodePos[nNewSize]; Copy(pNNew); nSize=nNewSize; };
		void Copy( NodePos* pNNew ) { for(int n=0;n<nSize;++n) pNNew[n]=pN[n]; if (pN) delete [] pN; pN=pNNew; };
		NodePos* pN;
		int nSize;
		int nTop;
	};

	struct FilePos
	{
		FilePos() { fp = NULL; nDocFlags = 0; nFileByteLen = 0; 
		nReadByteLen = 0;
		nFileCharUnitSize = 0;
		nFileTextLen = 0;
		strIOResult = "";
		strEncoding = "";};
		FILE* fp;
		int nDocFlags;
		int nFileByteLen;
		int nReadByteLen;
		int nFileCharUnitSize;
		int nFileTextLen;
		MCD_STR strIOResult;
		MCD_STR strEncoding;
	};

	struct ConvertEncoding
	{
		ConvertEncoding( MCD_CSTR pszToEncoding, MCD_CSTR pszFromEncoding, const void* pFromBuffer, int nFromBufferLen )
		{
			strToEncoding = pszToEncoding;
			strFromEncoding = pszFromEncoding;
			pFrom = pFromBuffer;
			nFromLen = nFromBufferLen;
			nFailedChars = 0;
			nToCount = 0;
		};
		MCD_STR strToEncoding;
		MCD_STR strFromEncoding;
		const void* pFrom;
		int nFromLen;
		int nToCount;
		int nFailedChars;
	};

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};
	int x_GetFreePos()
	{
		if ( m_iPosFree == m_aPos.GetSize() )
			x_AllocPosArray();
		return m_iPosFree++;
	};
	bool x_AllocPosArray( int nNewSize = 0 );

	void InitDocFlags()
	{
		// To always ignore case, define MARKUP_IGNORECASE
#if defined(MARKUP_IGNORECASE) // ignore case
		m_nDocFlags = MDF_IGNORECASE;
#else // not ignore case
		m_nDocFlags = 0;
#endif // not ignore case
	};

	bool x_ParseDoc();
	int x_ParseElem( int iPos, TokenPos& token );
	static bool x_FindAny( MCD_PCSZ pDoc, int& nChar );
	static bool x_FindName( TokenPos& token );
	static MCD_STR x_GetToken( const TokenPos& token );
	int x_FindElem( int iPosParent, int iPos, MCD_PCSZ szPath ) const;
	MCD_STR x_GetPath( int iPos ) const;
	MCD_STR x_GetTagName( int iPos ) const;
	MCD_STR x_GetData( int iPos ) const;
	MCD_STR x_GetAttrib( int iPos, MCD_PCSZ pAttrib ) const;
	static MCD_STR x_EncodeCDATASection( MCD_PCSZ szData );
	bool x_AddElem( MCD_PCSZ pName, MCD_PCSZ pValue, int nFlags );
	bool x_AddElem( MCD_PCSZ pName, int nValue, int nFlags );
	MCD_STR x_GetSubDoc( int iPos ) const;
	bool x_AddSubDoc( MCD_PCSZ pSubDoc, int nFlags );
	static bool x_FindAttrib( TokenPos& token, MCD_PCSZ pAttrib, int n=0 );
	bool x_SetAttrib( int iPos, MCD_PCSZ pAttrib, MCD_PCSZ pValue, int nFlags=0 );
	bool x_SetAttrib( int iPos, MCD_PCSZ pAttrib, int nValue, int nFlags=0 );
	bool x_AddNode( int nNodeType, MCD_PCSZ pText, int nNodeFlags );
	void x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength );
	void x_AdjustForNode( int iPosParent, int iPos, int nShift );
	static bool x_CreateNode( MCD_STR& strNode, int nNodeType, MCD_PCSZ pText );
	int x_InsertNew( int iPosParent, int& iPosRel, NodePos& node );
	void x_LinkElem( int iPosParent, int iPosBefore, int iPos );
	int x_UnlinkElem( int iPos );
	int x_ReleaseSubDoc( int iPos );
	int x_ReleasePos( int iPos );
	bool x_GetMap( SavedPosMap*& pMap, int nMap, int nMapSize = 7 );
	void x_CheckSavedPos();
	static int x_ParseNode( TokenPos& token, NodePos& node );
	bool x_SetData( int iPos, MCD_PCSZ szData, int nFlags );
	bool x_SetData( int iPos, int nValue );
	int x_RemoveElem( int iPos );
	MCD_STR x_GetElemContent( int iPos ) const;
	bool x_SetElemContent( MCD_PCSZ szContent );
	void x_DocChange( int nLeft, int nReplace, const MCD_STR& strInsert );
	void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
	static MCD_STR x_GetLastError();
	static int x_Hash( MCD_PCSZ p, int nSize ) { unsigned int n=0; while (*p) n += (unsigned int)(*p++); return n % nSize; };
	static int x_GetEncodingCodePage( MCD_CSTR psz7Encoding );
	static bool x_EndianSwapRequired( int nDocFlags );
	static void x_EndianSwapUTF16( unsigned short* pUTF16, int nCharLen );
	static bool x_CanConvert( MCD_CSTR pszToEnc, MCD_CSTR pszFromEnc );
	static int x_ConvertEncoding( ConvertEncoding& convert, void* pTo );
#if defined(MARKUP_ICONV)
	static int x_IConv( ConvertEncoding& convert, void* pTo, int nToCharSize, int nFromCharSize );
	static const char* x_IConvName( char* szEncoding, MCD_CSTR pszEncoding );
#endif
	static bool x_Open( MCD_CSTR_FILENAME szFileName, FilePos& file );
	static bool x_Read( void* pBuffer, FilePos& file );
	static bool x_ReadText( MCD_STR& strDoc, FilePos& file );
	static bool x_Write( void* pBuffer, FilePos& file, const void* pConstBuffer = NULL );
	static bool x_WriteText( const MCD_STR& strDoc, FilePos& file );
	static bool x_Close( FilePos& file );
	static MCD_STR x_IntToStr( int n ) { MCD_CHAR sz[25]; MCD_SPRINTF(MCD_SSZ(sz),MCD_T("%d"),n); MCD_STR s=sz; return s; };
};

#endif // !defined(_MARKUP_H_INCLUDED_)
