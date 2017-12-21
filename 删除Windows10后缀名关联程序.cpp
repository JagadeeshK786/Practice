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
		<< L"\t��ɾ��: "
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

		std::wcerr << L"\t�޷��򿪸ü���" << std::endl;
		return false;
	}

	LSTATUS lr = RegDeleteTree(hkOpen, strKeyToDelete.c_str());
	if (lr != ERROR_SUCCESS && lr != ERROR_FILE_NOT_FOUND) {
		std::wcout << L"\t�޷�ɾ���ü�(" << lr << L")��" << std::endl;
		RegCloseKey(hkOpen);
		return false;
	}

	std::wcout << L"\t��ɾ����\n\n";

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
		LR"(--- Win10�ļ���չ������ȥ�� ---

�ó�������ȥ���������ļ���չ���Ĺ�����
������ó������� ��Windows 10�� ϵͳ��

���棺�����ɾ���ѹ������ļ���չ�����ܻ��ϵͳ��ɲ�����صġ�
�����Եĺ�����������ʹ�øó���

�������������˲��е���ʹ�øó�����ɵĺ�����κ����Ρ��ڴ˼���ǰ��
�����򶼲����ϵͳ����κβ������������ͬ���������������ֹͣ
ʹ�ñ�����

��ϵ��Ů������ QQ:191035066 Em:anhbk@qq.com 2015-03-05

��ҳ�� http://blog.twofei.com/windows/usage/win10-unassoc-file-extension.html

)";

	std::wcout << LR"(ͬ�����ϵ�Э�������롰yes��: )";
	std::wstring yes_or_no;
	std::getline(std::wcin, yes_or_no);
	if (yes_or_no != L"yes") {
		return 1;
	}

	std::wcout << std::endl;

	for (;;) {
		std::wcout << LR"(��С�ĵ����뵥����չ��(������С����)��)";
		std::wstring ext;
		std::getline(std::wcin, ext);
		if (ext == L"" || ext.find(L' ') != std::wstring::npos) {
			std::wcout << LR"(����ȷ����չ����)" << std::endl;
			continue;
		}

		unassoc(ext);
	}

	return 0;
}