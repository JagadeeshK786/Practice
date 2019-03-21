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
		/*
		// Make sure the service is not already stopped.
		SERVICE_STATUS_PROCESS ssp;
		DWORD dwStartTime = GetTickCount();
		DWORD dwBytesNeeded;
		DWORD dwTimeout = 30000; // 30-second time-out
		DWORD dwWaitTime;
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			WriteErrorLogEntry(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			WriteErrorLogEntry(L"QPCore is already stopped.\n");
		}

		// If a stop is pending, wait for it.

		while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
		{
			WriteErrorLogEntry(L"QPCore stop pending...\n");

			// Do not wait longer than the wait hint. A good interval is
			// one-tenth of the wait hint but not less than 1 second
			// and not more than 10 seconds.

			dwWaitTime = ssp.dwWaitHint / 10;

			if (dwWaitTime < 1000)
				dwWaitTime = 1000;
			else if (dwWaitTime > 10000)
				dwWaitTime = 10000;

			Sleep(dwWaitTime);

			if (!QueryServiceStatusEx(
				schService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&ssp,
				sizeof(SERVICE_STATUS_PROCESS),
				&dwBytesNeeded))
			{
				WriteErrorLogEntry(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
			{
				WriteErrorLogEntry(L"QPCore stopped successfully.\n");
			}

			if (GetTickCount() - dwStartTime > dwTimeout)
			{
				WriteErrorLogEntry(L"QPCore stop timed out.\n");
			}
		}

		// If the service is running, dependencies must be stopped first.
		StopDependentServices(schSCManager,schService);

		// Send a stop code to the service.

		if (!ControlService(
			schService,
			SERVICE_CONTROL_STOP,
			(LPSERVICE_STATUS)&ssp))
		{
			WriteErrorLogEntry(L"ControlService failed (%d)\n", GetLastError());
		}

		// Wait for the service to stop.

		while (ssp.dwCurrentState != SERVICE_STOPPED)
		{
			Sleep(ssp.dwWaitHint);
			if (!QueryServiceStatusEx(
				schService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&ssp,
				sizeof(SERVICE_STATUS_PROCESS),
				&dwBytesNeeded))
			{
				WriteErrorLogEntry(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
				break;

			if (GetTickCount() - dwStartTime > dwTimeout)
			{
				//printf("Wait timed out\n");
			}
		}*/
		ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	BOOL __stdcall StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
	{
		DWORD i;
		DWORD dwBytesNeeded;
		DWORD dwCount;

		LPENUM_SERVICE_STATUS   lpDependencies = NULL;
		ENUM_SERVICE_STATUS     ess;
		SC_HANDLE               hDepService;
		SERVICE_STATUS_PROCESS  ssp;

		DWORD dwStartTime = GetTickCount();
		DWORD dwTimeout = 30000; // 30-second time-out

		// Pass a zero-length buffer to get the required buffer size.
		if (EnumDependentServices(schService, SERVICE_ACTIVE,
			lpDependencies, 0, &dwBytesNeeded, &dwCount))
		{
			// If the Enum call succeeds, then there are no dependent
			// services, so do nothing.
			return TRUE;
		}
		else
		{
			if (GetLastError() != ERROR_MORE_DATA)
				return FALSE; // Unexpected error

			// Allocate a buffer for the dependencies.
			lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
				GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

			if (!lpDependencies)
				return FALSE;

			__try {
				// Enumerate the dependencies.
				if (!EnumDependentServices(schService, SERVICE_ACTIVE,
					lpDependencies, dwBytesNeeded, &dwBytesNeeded,
					&dwCount))
					return FALSE;

				for (i = 0; i < dwCount; i++)
				{
					ess = *(lpDependencies + i);
					// Open the service.
					hDepService = OpenService(schSCManager,
						ess.lpServiceName,
						SERVICE_STOP | SERVICE_QUERY_STATUS);

					if (!hDepService)
						return FALSE;

					__try {
						// Send a stop code.
						if (!ControlService(hDepService,
							SERVICE_CONTROL_STOP,
							(LPSERVICE_STATUS)&ssp))
							return FALSE;

						// Wait for the service to stop.
						while (ssp.dwCurrentState != SERVICE_STOPPED)
						{
							Sleep(ssp.dwWaitHint);
							if (!QueryServiceStatusEx(
								hDepService,
								SC_STATUS_PROCESS_INFO,
								(LPBYTE)&ssp,
								sizeof(SERVICE_STATUS_PROCESS),
								&dwBytesNeeded))
								return FALSE;

							if (ssp.dwCurrentState == SERVICE_STOPPED)
								break;

							if (GetTickCount() - dwStartTime > dwTimeout)
								return FALSE;
						}
					}
					__finally
					{
						// Always release the service handle.
						CloseServiceHandle(hDepService);
					}
				}
			}
			__finally
			{
				// Always free the enumeration buffer.
				HeapFree(GetProcessHeap(), 0, lpDependencies);
			}
		}
		return TRUE;
	}
};