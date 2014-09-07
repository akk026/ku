// ku.cpp

#include <windows.h>

#include "ku.h"
#include "str.h"

#ifndef _DEBUG
#pragma comment(linker, "/NODEFAULTLIB:LIBCMT")
#endif

const wchar_t szWarning [] = L"Kuin Commandline Utility - �x��";
const wchar_t szError [] = L"Kuin Commandline Utility - �G���[";
const wchar_t szErrorMsg1 [] = L"�N���[���I�v�V�����Ƒ��̃I�v�V���������p���邱�Ƃ͂ł��܂���B";
const wchar_t szErrorMsg2 [] = L"�R���p�C���ƕ��@�`�F�b�N�𓯎��ɍs�����Ƃ͂ł��܂���B";

wchar_t szKuinTitle [] = L"Tes.So Kuin 1.00";
wchar_t szKuin [] = L"Kuin.exe";

BOOL CALLBACK EnumKuinWndProc(HWND hWnd, LPARAM lParam)
{
	DWORD_PTR dwLen;
	wchar_t szBuf[sizeof(szKuinTitle)];

	dwLen = (SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0) + 1) * sizeof(wchar_t);

	if (dwLen == sizeof(szKuinTitle))
	{
		SendMessage(hWnd, WM_GETTEXT, sizeof(szBuf), (LPARAM) szBuf);

		if (!wcscmp(szBuf, szKuinTitle))
		{
			*(HWND *) lParam = hWnd;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumKuinCtlsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szBuf[64];
	static int nEdits = 0;
	Controls *ctls = (Controls *) lParam;
	DWORD_PTR dwLen;

	GetClassName(hWnd, szBuf, sizeof(szBuf));
	szBuf[20] = L'\0';

	if (!wcscmp(szBuf + 15, L"EDIT."))
	{
		switch (nEdits)
		{
		case 0:
			ctls->hKuinLogEdit = hWnd;
			break;

		case 1:
			ctls->hKuinIconEdit = hWnd;
			break;

		case 2:
			ctls->hKuinSrcEdit = hWnd;
			return FALSE;
		}

		nEdits++;
	}
	else
	{
		dwLen = (SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0) + 1) * sizeof(TCHAR);

		if (dwLen == sizeof(L"�����[�X�R���p�C��"))
		{
			SendMessage(hWnd, WM_GETTEXT, sizeof(szBuf), (LPARAM) szBuf);

			if (!wcscmp(szBuf, L"�����[�X�R���p�C��"))
			{
				ctls->hKuinReleaseCompileCheck = hWnd;
			}
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumKuinPopupProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szBuf[64];

	GetClassName(hWnd, szBuf, sizeof(szBuf));

	szBuf[21] = L'\0';
	szBuf[28] = L'\0';

	if (!wcscmp(szBuf + 14, L".Window") && wcsstr(szBuf + 22, L"808"))
	{
		*(HWND *) lParam = hWnd;
		return FALSE;
	}

	return TRUE;
}

HANDLE OpenProcessOfWindow(DWORD dwDesiredAccess, BOOL bInheritHandle, HWND hWnd)
{
	DWORD dwProcessId;

	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId)
		return OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);

	return NULL;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	BOOL bClean = FALSE, bRun = FALSE, bDebug = TRUE, bRelease = FALSE, bSyntaxCheck = FALSE, bCreateProcess = FALSE, bCloseProcess = FALSE, bHideMessage = FALSE;
	LPWSTR lpszSrcFilePath = NULL, lpszIconPath = NULL;

	if (__argc > 1)
	{
		for (int i = 1; i < __argc; i++)
		{
			if ((__wargv[i][0] == L'-' || __wargv[i][0] == '/') && __wargv[i][1] == '?' && __wargv[i][2] == '\0')
			{
				MessageBox(NULL, L"�y�g�p���@�zku.exe Sample.kn [-s|-r|-e][-c][-ico {Icon}]\n\n��1: ku.exe Sample.kn -s\nSample.kn�̕��@�`�F�b�N�B\n\n��2: ku.exe Sample.kn\nSample.kn���f�o�b�O���[�h�ŃR���p�C���B\n\n��3: ku.exe Sample.kn -r\nSample.kn�������[�X���[�h�ŃR���p�C���B\n\n��4: ku.exe Sample.kn -e\nSample.kn�̃R���p�C���ɐ��������ꍇ�A�쐬���ꂽexe�����s����B\n\n��Kuin.exe�Ɠ����f�B���N�g���ɒu���Ďg�p���Ă��������B", L"Kuin Commandline Utility - �g����", MB_OK);
				return 0;
			}

			if (__wargv[i][0] == L'-')
			{
				int nErrorType = 0;

				/*
				  nErrorType

				   1: clean and other (warning)
				   2: compile and syntax check (warning)
				   3: syntax check and compile (warning)
				   4: unknown option (error)
				*/

				// CharLower(__wargv[i]);

				switch (__wargv[i][1])
				{
				case L'c':

					if (nullchk(__wargv[i], 2))
					{
						// -c

						if (bRun || bRelease || bSyntaxCheck)
						{
							nErrorType = 1;
						}
						else
						{
							bClean = TRUE;
						}
					}
					else if (__wargv[i][2] == L'p' && nullchk(__wargv[i], 3))
					{
						// -cp
						bCreateProcess = TRUE;
					}
					else
					{
						nErrorType = 4;
					}
					break;

				case L'e':

					if (nullchk(__wargv[i], 2))
					{
						// -e

						if (bClean)
						{
							nErrorType = 1;
						}
						else if (bSyntaxCheck)
						{
							nErrorType = 2;
						}
						else
						{
							bRun = TRUE;
						}
					}
					else if (__wargv[i][2] == L'p' && nullchk(__wargv[i], 3))
					{
						// -ep
						bCloseProcess = TRUE;
					}
					else
					{
						nErrorType = 4;
					}
					break;

				case L'h':

					if (__wargv[i][2] == L'm' && nullchk(__wargv[i], 3))
					{
						// -hm
						bHideMessage = TRUE;
					}
					else
					{
						nErrorType = 4;
					}
					break;

				case L'i':

					if (__wargv[i][2] == L'c' && __wargv[i][3] == L'o' && nullchk(__wargv[i], 4))
					{
						// -ico

						if (!lpszIconPath && (__argc - 1) > i)
						{
							lpszIconPath = __wargv[++i];
						}
					}
					else
					{
						nErrorType = 4;
					}
					break;

				case L'r':

					if (nullchk(__wargv[i], 2))
					{
						// -r

						if (bClean)
						{
							nErrorType = 1;
						}
						else if (bSyntaxCheck)
						{
							nErrorType = 2;
						}
						else
						{
							bRelease = TRUE;
						}
					}
					else
					{
						nErrorType = 4;
					}
					break;

				case L's':

					if (nullchk(__wargv[i], 2))
					{
						// -s
						
						if (bClean)
						{
							nErrorType = 1;
						}
						else if (bRelease || bRun)
						{
							nErrorType = 3;
						}
						else
						{
							bSyntaxCheck = TRUE;
						}
					}
					else
					{
						nErrorType = 4;
					}
					break;

				default:

					nErrorType = 4;
					break;
				}

				if (nErrorType)
				{
					if (!bHideMessage)
					{
						switch (nErrorType)
						{
						case 1:
							MessageBox(NULL, szErrorMsg1, szWarning, MB_OK | MB_ICONWARNING);
							break;

						case 2:
							MessageBox(NULL, szErrorMsg2, szWarning, MB_OK | MB_ICONWARNING);
							break;

						case 3:
							MessageBox(NULL, L"���@�`�F�b�N�ƃR���p�C���𓯎��ɍs�����Ƃ͂ł��܂���B", szWarning, MB_OK | MB_ICONWARNING);
							break;

						case 4:
							MessageBox(NULL, L"�s���ȃI�v�V�������w�肳��܂����B", szError, MB_OK | MB_ICONERROR);
							break;
						}
					}

					return 1;
				}
			}
			else if (!lpszSrcFilePath)
			{
				lpszSrcFilePath = __wargv[i];
			}
		}
	}

	if (bRelease || bSyntaxCheck)
	{
		bDebug = FALSE;
	}

	HWND hActiveWnd, hKuinWnd = NULL;
	HANDLE hKuinProcess = NULL;
	DWORD dwKuinThreadId = 0;
	int ret = 0;

	hActiveWnd = GetForegroundWindow();

	if (bCreateProcess || (!(hKuinWnd = FindWindow(NULL, szKuinTitle))))
	{
		wchar_t szCurrentDir[MAX_PATH];
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		GetModuleFileName(NULL, szCurrentDir, MAX_PATH);
		fullpath_to_dir(szCurrentDir);

		si.cb = sizeof(si);

		if (CreateProcess(NULL, szKuin, NULL, NULL, FALSE, 0, NULL, szCurrentDir, &si, &pi))
		{
			CloseHandle(pi.hThread);

			WaitForInputIdle(pi.hProcess, INFINITE);

			while (true)
			{
				if (!EnumWindows(EnumKuinWndProc, (LPARAM) &hKuinWnd))
				{
					if (!hKuinWnd)
					{
						if (!bHideMessage)
						{
							MessageBox(NULL, L"Kuin�̃E�B���h�E��ߑ��ł��܂���ł����B", L"Kuin Commandline Utility - Kuin.exe �E�B���h�E�ߑ��G���[", MB_OK | MB_ICONERROR);
						}

						ret = 3;
					}

					break;
				}

				Sleep(1);
			}

			if (ret)
			{
				CloseHandle(pi.hProcess);
			}
			else
			{
				hKuinProcess = pi.hProcess;
				dwKuinThreadId = pi.dwThreadId;
			}
		}
		else
		{
			if (!bHideMessage)
				MessageBox(NULL, L"Kuin.exe���N���ł��܂���ł����B\n\n�L���ȃp�X��Kuin.exe�����݂��邱�Ƃ��m�F���Ă���A�ēx���s���Ă��������B", L"Kuin Commandline Utility - Kuin.exe �N���G���[", MB_OK | MB_ICONERROR);

			ret = 2;
		}

		if (ret)
			return ret;
	}

	if (!bClean && !hKuinProcess)
	{
		hKuinProcess = OpenProcessOfWindow(PROCESS_QUERY_INFORMATION, FALSE, hKuinWnd);

		if (!hKuinProcess)
		{
			MessageBox(NULL, L"Kuin.exe �̃v���Z�X�ɃA�N�Z�X�ł��܂���B", szError, MB_OK | MB_ICONERROR);
			return 4;
		}

		dwKuinThreadId = GetWindowThreadProcessId(hKuinWnd, NULL);
	}

	Controls ctls;

	EnumChildWindows(hKuinWnd, EnumKuinCtlsProc, (LPARAM) &ctls);

	wchar_t szFullPath[MAX_PATH];
	HWND hKuinPopupMenu = NULL;

	if (lpszSrcFilePath)
	{
		GetFullPathName(lpszSrcFilePath, MAX_PATH, szFullPath, NULL);
		SendMessage(ctls.hKuinSrcEdit, WM_SETTEXT, 0, (LPARAM) szFullPath);
	}

	if (lpszIconPath)
	{
		GetFullPathName(lpszIconPath, MAX_PATH, szFullPath, NULL);
		SendMessage(ctls.hKuinIconEdit, WM_SETTEXT, 0, (LPARAM) szFullPath);
	}

	if ((!bRun && !bSyntaxCheck) || bClean)
	{
		EnumThreadWindows(dwKuinThreadId, EnumKuinPopupProc, (LPARAM) &hKuinPopupMenu);

		if (!hKuinPopupMenu)
		{
			SendMessage(FindWindowEx(hKuinWnd, NULL, NULL, L"���̑�(�E�N���b�N)..."), WM_RBUTTONUP, 0, MAKELPARAM(5, 5));
			SendMessage(hKuinWnd, WM_LBUTTONUP, 0, MAKELPARAM(0, 0));

			EnumThreadWindows(dwKuinThreadId, EnumKuinPopupProc, (LPARAM) &hKuinPopupMenu);
		}
	}

	if (bClean)
	{
		SendMessage(hKuinPopupMenu, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(5, 30));
		SendMessage(hKuinPopupMenu, WM_LBUTTONUP, 0, MAKELPARAM(5, 30));
	}
	else if (bSyntaxCheck)
	{
		SendMessage(FindWindowEx(hKuinWnd, NULL, NULL, L"���@�`�F�b�N"), BM_CLICK, 0, 0);
	}
	else
	{
		// BM_GETCHECK �����肪�ł��Ȃ��̂�

		HDC hDC;
		COLORREF rgb;
		int nButtonState;

		hDC = GetWindowDC(ctls.hKuinReleaseCompileCheck);
		rgb = GetPixel(hDC, 6, 9);

		if ((rgb == RGB(75, 96, 151)) || (rgb == RGB(4, 34, 113)))
			nButtonState = 1;
		else
			nButtonState = 0;

		ReleaseDC(ctls.hKuinReleaseCompileCheck, hDC);

		if ((bRelease && !nButtonState) || (bDebug && nButtonState))
			SendMessage(ctls.hKuinReleaseCompileCheck, BM_CLICK, 0, 0);

		if (bRun)
		{
			SendMessage(FindWindowEx(hKuinWnd, NULL, NULL, L"�R���p�C�������s"), BM_CLICK, 0, 0);
		}
		else
		{
			SendMessage(hKuinPopupMenu, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(5, 5));
			SendMessage(hKuinPopupMenu, WM_LBUTTONUP, 0, MAKELPARAM(5, 5));
		}
	}

	if (hActiveWnd)
	{
		SetForegroundWindow(hActiveWnd);
	}

	if (bClean)
	{
		if (bCloseProcess)
		{
			SendMessage(hKuinWnd, WM_CLOSE, 0, 0);
		}

		if (hKuinProcess)
		{
			CloseHandle(hKuinProcess);
		}

		return 0;
	}

	if (WaitForInputIdle(hKuinProcess, INFINITE))
	{
		MessageBox(NULL, L"�R���p�C���I���ҋ@���ɃG���[���������܂����B", szError, MB_OK | MB_ICONERROR);
		return 5;
	}

	DWORD_PTR dwLen;
	LPWSTR lpszBuf;

	dwLen = SendMessage(ctls.hKuinLogEdit, WM_GETTEXTLENGTH, 0, 0);

	lpszBuf = (LPTSTR) malloc(++dwLen * sizeof(TCHAR));

	if (!SendMessage(ctls.hKuinLogEdit, WM_GETTEXT, dwLen, (LPARAM) lpszBuf))
	{
		if (!bHideMessage)
		{
			MessageBox(NULL, L"�R���p�C�����ʂ��擾�ł��܂���ł����B", szError, MB_OK | MB_ICONERROR);
		}

		ret = 6;
	}
	else if (wcsstr(lpszBuf, L"> OK."))
	{
		if (bSyntaxCheck && !bHideMessage)
		{
			MessageBox(NULL, L"�w�肳�ꂽ�\�[�X�t�@�C���̕��@�͊����ł��I", L"Kuin���@�`�F�b�N", MB_OK | MB_ICONINFORMATION);
		}
	}
	else if (wcsstr(lpszBuf, L"> Failed."))
	{
		if (!bHideMessage)
		{
			LPWSTR lpszErrorStart, lpszErrorEnd, lpszErrorCurrent;

			lpszErrorStart = wcsstr(lpszBuf, L"Error: ");

			lpszErrorEnd = wcsstr(lpszErrorStart, L"\n");
			*lpszErrorEnd = L'\0';

			lpszErrorCurrent = lpszErrorStart;

			while (lpszErrorCurrent = wcsstr(lpszErrorCurrent, L" "))
			{
				if (((!(*(lpszErrorCurrent + 1) == L'E' && *(lpszErrorCurrent + 6) == L' ')) && (lpszErrorCurrent - 12 == lpszErrorStart)) || (*(lpszErrorCurrent - 1) == L'�B'))
					*lpszErrorCurrent = L'\n';

				lpszErrorCurrent++;
			}

			MessageBox(NULL, lpszErrorStart, bSyntaxCheck ? L"Kuin���@�`�F�b�N�G���[" : L"Kuin�R���p�C���G���[", MB_OK);
		}

		ret = 7;
	}
	else
	{
		MessageBox(NULL, L"�����ȃR���p�C�����ʂł��B", szError, MB_OK | MB_ICONERROR);
		ret = 8;
	}

	free(lpszBuf);

	if (hKuinProcess)
	{
		CloseHandle(hKuinProcess);
	}

	if (bCloseProcess)
	{
		SendMessage(hKuinWnd, WM_CLOSE, 0, 0);
	}

	return ret;
}
