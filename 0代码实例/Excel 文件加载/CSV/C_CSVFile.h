#ifndef __C_CSVFile_H__
#define __C_CSVFile_H__

#define CSV_BUF_SIZE 128000
class C_CSVFile
{
public:
	C_CSVFile();
	~C_CSVFile();
	BOOL		LoadCSV(LPCTSTR szFilePath);
	BOOL		GetSize(UINT& iWidth, UINT& iHeight);
	UINT		GetColNumber(UINT nRow);
	CHAR*		GetItem(UINT nRow, UINT nCol);
private:
	vector<vector<CHAR*>*> m_vvCSVData;
	CHAR		m_szMaxBuffer[CSV_BUF_SIZE];
	CHAR**		m_pszItemTab;
	UINT		m_nRow;
	UINT		m_nCol;
};

#endif // __C_CSVFile_H__