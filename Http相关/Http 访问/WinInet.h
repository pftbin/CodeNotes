#include <afxinet.h>
#pragma comment(lib, "Wininet.lib")
#define DF_DOWNLOAD_BUFFSIZE		(10240) //10K
BOOL DownloadFile(const CString& strURL, const CString& strLocalFilePath)
{
	BOOL bResult = FALSE;
	CInternetSession session;
	CHttpFile* pFile = nullptr;

	try
	{
		pFile = (CHttpFile*)session.OpenURL(strURL, 1, INTERNET_FLAG_SECURE);
		if (pFile)
		{
			BYTE* buffer = new BYTE[DF_DOWNLOAD_BUFFSIZE];
			memset(buffer, 0, sizeof(BYTE) * DF_DOWNLOAD_BUFFSIZE);

			CFile file;
			if (file.Open(strLocalFilePath, CFile::modeCreate | CFile::modeWrite))
			{	
				DWORD byteTotal = 0, bytesRead = 0;
				while ((bytesRead = pFile->Read(buffer, sizeof(buffer))) > 0)
				{
					file.Write(buffer, bytesRead);
					byteTotal += bytesRead;
				}
				file.Close();

				if (byteTotal > 0)
					bResult = TRUE;
			}

			delete[] buffer;
		}
	}
	catch (CInternetException* e)
	{
		// 处理异常
		e->Delete();
	}

	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
	session.Close();

	return bResult;
}