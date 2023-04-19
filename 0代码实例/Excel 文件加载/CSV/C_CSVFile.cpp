#include "stdafx.h"
#include "C_CSVFile.h"
#include <io.h>

C_CSVFile::C_CSVFile()
{
	m_vvCSVData.clear();
	m_pszItemTab = NULL;
	m_nRow = 0;
	m_nCol = 0;
	memset(m_szMaxBuffer,0,sizeof(CHAR)*CSV_BUF_SIZE);
}

C_CSVFile::~C_CSVFile()
{
	if (m_pszItemTab)
	{
		delete[] m_pszItemTab;
	}

	for (int i = 0; i < m_vvCSVData.size(); i++)
	{
		vector<CHAR*>* pRow = m_vvCSVData[i];
		pRow->clear();
	}
	m_vvCSVData.clear();
}

BOOL C_CSVFile::LoadCSV( LPCTSTR szFilePath )
{
	if (szFilePath == NULL || _taccess(szFilePath, 0) == -1)
	{
		return FALSE;
	}
	FILE* fp = _tfopen(szFilePath, _T("rb"));
	if (!fp)
	{
		return FALSE;
	}
	memset(m_szMaxBuffer,0,CSV_BUF_SIZE);
	int iRead = fread(m_szMaxBuffer, sizeof(CHAR), CSV_BUF_SIZE, fp);


// 	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, 0, m_szMaxBuffer, -1, NULL, 0, NULL, FALSE);
// 	char* pData = new char[dwNum + 1];
// 	WideCharToMultiByte(CP_OEMCP, 0, m_szMaxBuffer, -1, pData, dwNum, NULL, FALSE);

	fclose(fp);
	if (iRead == CSV_BUF_SIZE)
	{
		return FALSE;
	}
	m_szMaxBuffer[iRead] = TEXT('\0');
	m_szMaxBuffer[iRead + 1] = TEXT('\0');

	CHAR* szStart;
	CHAR* szEnd;
	vector<CHAR*> *pvRowList = NULL;
	UINT i = 0, j = 0;
	for (int i = 0; i < m_vvCSVData.size(); i++)
	{
		vector<CHAR*>* pRow = m_vvCSVData[i];
		pRow->clear();
	}
	m_vvCSVData.clear();

	szStart = m_szMaxBuffer;
	m_nCol = 0;
	m_nRow = 0;
	while (*szStart != TEXT('\0'))
	{
		//ignore invisible char
		if (*szStart == TEXT(' ') || *szStart == TEXT('\t') || *szStart == TEXT('\r'))
		{
			++szStart;
			continue;
		}
		//reach the tail of a item
		szEnd = szStart;
		while (*szEnd != TEXT(',') && *szEnd != TEXT('\n') && *szEnd != TEXT('\0'))
		{
			++szEnd;
		}
		BOOL bLineEnd = (*szEnd == TEXT('\n') || *szEnd == TEXT('\0'));
		*szEnd = TEXT('\0');
		//trim
		for (CHAR* szTrim = szEnd - 1; szTrim >= szStart; --szTrim)
		{
			if (*szTrim == TEXT('\r') || *szTrim== TEXT(' ') || *szTrim== TEXT('\t'))
			{
				*szTrim = TEXT('\0');
			}
			else
			{
				break;
			}
		}
		if (pvRowList == NULL)
		{
			pvRowList = new vector<CHAR*>;
		}
		pvRowList->push_back(szStart);
		if (bLineEnd)
		{
			m_vvCSVData.push_back(pvRowList);
			if (m_nCol < pvRowList->size())
			{
				m_nCol = pvRowList->size();
			}
			pvRowList = NULL;
			++m_nRow;
		}

		szStart = szEnd;
		++szStart;
	}

	if (m_nCol == 0 || m_nRow == 0)
	{
		return FALSE;
	}

	//copy to array
	if (m_pszItemTab)
	{
		delete[] m_pszItemTab;
		m_pszItemTab = NULL;
	}
	m_pszItemTab = new CHAR*[m_nRow * m_nCol];
	ZeroMemory(m_pszItemTab, sizeof(CHAR) * m_nRow * m_nCol);

	for (i = 0; i < m_nRow; ++i)
	{
		vector<CHAR*>* pRow = m_vvCSVData[i];
		for (j = 0; j < m_nCol; ++j)
		{
			if (j < pRow->size())
			{
				CHAR* p = (*pRow)[j];
				m_pszItemTab[i * m_nCol + j] = p;
			}
		}
	}

	return TRUE;
}

BOOL C_CSVFile::GetSize( UINT& iWidth, UINT& iHeight )
{
	iWidth = m_nCol;
	iHeight = m_nRow;
	return TRUE;
}

UINT C_CSVFile::GetColNumber(UINT nRow)
{
	if (nRow >= m_nRow)
		return 0;

	if (nRow >= m_vvCSVData.size())
		return 0;

	UINT uiColNum = 0;
	vector<CHAR*>* pRowData = m_vvCSVData.at(nRow);
	if (pRowData)
	{
		uiColNum = (*pRowData).size();
	}

	return uiColNum;
}

CHAR* C_CSVFile::GetItem( UINT nRow, UINT nCol )
{
	if (nRow >= m_nRow || nCol >= m_nCol)
	{
		return NULL;
	}
	return m_pszItemTab[m_nCol * nRow + nCol];
}
