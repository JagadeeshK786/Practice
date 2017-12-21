#include <string>
#include <iostream>
#include <io.h>
#include <fcntl.h>

#include <tchar.h>
#include <windows.h>
#include <ShlObj.h>

const wchar_t* str_hkey(HKEY hRootKey) {
	if (hRootKey == HKEY_CURRENT_USER) return L"HKEY_CURRENT_USER";

	return L"UNKNOWN";
}

bool delete_key(HKEY hRootKey, std::wstring strSubkey, std::wstring strKeyToDelete)
{
	HKEY hkOpen;

	std::wcout
		<< L"\t将删除: "
		<< str_hkey(hRootKey)
		<< L"\\"
		<< strSubkey
		<< L"\\"
		<< strKeyToDelete
		<< std::endl;

	if (RegOpenKeyExW(
		hRootKey,
		strSubkey.c_str(),
		0,
		DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE,
		&hkOpen
	)
		!= ERROR_SUCCESS)
	{

		std::wcerr << L"\t无法打开该键！" << std::endl;
		return false;
	}

	LSTATUS lr = RegDeleteTree(hkOpen, strKeyToDelete.c_str());
	if (lr != ERROR_SUCCESS && lr != ERROR_FILE_NOT_FOUND) {
		std::wcout << L"\t无法删除该键(" << lr << L")！" << std::endl;
		RegCloseKey(hkOpen);
		return false;
	}

	std::wcout << L"\t已删除！\n\n";

	return true;
}

void unassoc(const std::wstring& ext)
{
	delete_key(
		HKEY_CURRENT_USER,
		LR"(Software\Classes)",
		ext + L"_auto_file"
	)
		&& delete_key(
			HKEY_CURRENT_USER,
			LR"(Software\Classes)",
			std::wstring(L".") + ext
		)
		&& delete_key(
			HKEY_CURRENT_USER,
			LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts)",
			std::wstring(L".") + ext
		)
		&& delete_key(
			HKEY_CURRENT_USER,
			LR"(Software\Microsoft\Windows\Roaming\OpenWith\FileExts)",
			std::wstring(L".") + ext
		)
		;
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}

int main()
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	std::wcout <<
		LR"(--- Win10文件扩展名关联去除 ---

该程序用于去除程序与文件扩展名的关联。
请仅将该程序用于 “Windows 10” 系统。

警告：随意地删除已关联的文件扩展名可能会对系统造成不可挽回的、
灾难性的后果，请谨慎地使用该程序。

免责声明：本人不承担对使用该程序造成的后果的任何责任。在此及以前，
本程序都不会对系统造成任何不良后果。若不同意该声明，请立即停止
使用本程序。

联系：女孩不哭 QQ:191035066 Em:anhbk@qq.com 2015-03-05

主页： http://blog.twofei.com/windows/usage/win10-unassoc-file-extension.html

)";

	std::wcout << LR"(同意以上的协议请输入“yes”: )";
	std::wstring yes_or_no;
	std::getline(std::wcin, yes_or_no);
	if (yes_or_no != L"yes") {
		return 1;
	}

	std::wcout << std::endl;

	for (;;) {
		std::wcout << LR"(请小心地输入单个扩展名(不包含小数点)：)";
		std::wstring ext;
		std::getline(std::wcin, ext);
		if (ext == L"" || ext.find(L' ') != std::wstring::npos) {
			std::wcout << LR"(不正确的扩展名！)" << std::endl;
			continue;
		}

		unassoc(ext);
	}

	return 0;
}