/****************************** Module Header ******************************\
* Module Name:  SampleService.h
* Project:      CppWindowsService
* Copyright (c) Microsoft Corporation.
*
* Provides a sample service class that derives from the service base class -
* CServiceBase. The sample service logs the service start and stop
* information to the Application event log, and shows how to run the main
* function of the service in a thread pool worker thread.
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/en-us/openness/resources/licenses.aspx#MPL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma once
#include <memory>
#include <tlhelp32.h>
#include <utility>
#include "ServiceBase.h"
#include "DC_File.h"

class CSampleService : public CServiceBase
{
public:

	CSampleService(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE)
		: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue) {}
	virtual ~CSampleService(void) {}

protected:
	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv) {
		//DC::File::writeAppend("C:/Users/liuzi/Desktop/log.txt", "start\r\n");
		fucktencent();
	}

	virtual void OnPause()override {
		//DC::File::writeAppend("C:/Users/liuzi/Desktop/log.txt", "pause\r\n");
		fucktencent();
	}

	virtual void OnContinue()override {
		//DC::File::writeAppend("C:/Users/liuzi/Desktop/log.txt", "continue\r\n");
		fucktencent();
	}

	virtual void OnStop()override {
		//DC::File::writeAppend("C:/Users/liuzi/Desktop/log.txt", "stop\r\n");
		fucktencent();
	}

	virtual void OnShutdown()override {
		//DC::File::writeAppend("C:/Users/liuzi/Desktop/log.txt", "shutdown\r\n");
		fucktencent();
	}

private:
	void fucktencent() {
		auto pid=get_qprotect_pid();
		if (std::get<0>(pid)) {			
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, std::get<1>(pid));
			if (hProcess == NULL) {
				WriteErrorLogEntry(L"OpenSCManager failed");
			}
			else
			{
				if (TerminateProcess(hProcess, 0) == 0) {
					WriteErrorLogEntry(L"TerminateProcess failed");
				}
				CloseHandle(hProcess);
			}
		}

		// Get a handle to the SCM database. 
		SC_HANDLE schSCManager = OpenSCManager(
			NULL,                    // local computer
			NULL,                    // servicesActive database 
			SC_MANAGER_ALL_ACCESS);  // full access rights 

		if (NULL == schSCManager)
		{
			WriteErrorLogEntry(L"OpenSCManager failed");
			return;
		}
		std::unique_ptr<SC_HANDLE, void(*)(SC_HANDLE*)> schSCManagerH(&schSCManager, [](SC_HANDLE* h) {
			CloseServiceHandle(*h);
		});

		// Get a handle to the service.
		SC_HANDLE schService = OpenService(
			schSCManager,         // SCM database 
			L"QPCore",            // name of service 
			 SERVICE_CHANGE_CONFIG);  // full access 

		if (schService == NULL)
		{
			WriteErrorLogEntry(L"OpenService failed");
			CloseServiceHandle(schSCManager);
			return;
		}

		decltype(schSCManagerH) schServiceH(&schService, [](SC_HANDLE* h) {
			CloseServiceHandle(*h);
		});
		ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	std::pair<bool,DWORD> get_qprotect_pid() {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		using Guard = std::unique_ptr<HANDLE, void(*)(HANDLE*)>;

		// Take a snapshot of all processes in the system.
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			WriteErrorLogEntry(L"CreateToolhelp32Snapshot failed");
			return std::make_pair(false, 0);
		}
		Guard hProcessSnapGuard(&hProcessSnap, [](HANDLE* h) {CloseHandle(*h); });

		// Retrieve information about the first process
		if (!Process32First(hProcessSnap, &pe32))
		{
			WriteErrorLogEntry(L"Process32First failed");
			return std::make_pair(false, 0);
		}
		do
		{
			//std::wstring ws(pe32.szExeFile);
			//DC::File::writeAppend("C:/Users/liuzi/Desktop/names.txt", std::string(ws.begin(), ws.end())+"\r\n");
			if (wcscmp(pe32.szExeFile, L"QQProtect.exe") == 0)
				return std::make_pair(true, pe32.th32ProcessID);
		} while (Process32Next(hProcessSnap, &pe32));
		WriteErrorLogEntry(L"QQProtect not found");
		return std::make_pair(false, 0);
	}

};