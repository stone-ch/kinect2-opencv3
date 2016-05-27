#include "PrintHelper.h"

#ifdef __cplusplus
EXTERN_C
{
#endif
	__declspec(dllexport) bool Print(unsigned char* buffer,int buffersize)
	{
		DWORD dwSize = 0; 
		if(!GetDefaultPrinter(NULL, &dwSize)     
			&& GetLastError()==ERROR_INSUFFICIENT_BUFFER)    
		{    
			TCHAR *szPrinter = new TCHAR[dwSize+1];    
			if(GetDefaultPrinter(szPrinter, &dwSize))    
			{    
				HANDLE hPrinter = NULL;
				DOC_INFO_1 DocInfo;
				DWORD      dwJob;
				DWORD      dwBytesWritten = 0L;

				if(OpenPrinter((LPTSTR)(szPrinter), &hPrinter, NULL )) {
					//cout << "printer opened" << endl;
					DocInfo.pDocName = (LPTSTR)_T("My Document");
					DocInfo.pOutputFile = NULL;
					DocInfo.pDatatype = (LPTSTR)_T("RAW");
					dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo );
					if (dwJob != 0) {
						//cout << "Print job open" << endl;
						if (StartPagePrinter( hPrinter )) {
							//cout << "Page started" << endl;
							// Send the data to the printer.
							if (WritePrinter( hPrinter, (void*)buffer, buffersize, &dwBytesWritten)) {
								if (dwBytesWritten == buffersize) { }//cout << "Message sent to printer" << endl; }
							}
							EndPagePrinter (hPrinter);
							//cout << "Page Closed" << endl;
						}
						// Inform the spooler that the document is ending.
						EndDocPrinter( hPrinter );
						//cout << "Print job open" << endl;
					} else {
						//out << "Could not create print job" << endl;
					}
					// Close the printer handle.
					ClosePrinter( hPrinter );
					delete []szPrinter; 
					return true;
					//cout << "printer closed" << endl;
				} else {
					delete []szPrinter; 
					return false;
					//cout << "Could not open Printer" << endl;
				}
				//cout << "done";
			}    
		} 
		return false;
	}
#ifdef __cplusplus
}
#endif