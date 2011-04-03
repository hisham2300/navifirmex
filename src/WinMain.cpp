/*
http://code.google.com/p/navifirmex/
Copyright (C) 2011  Just Fancy (Just_Fancy@live.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//========================================================================================================
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//========================================================================================================
//////////////////////////////////////////////////////////////////////////
#include "GUIWnd.h"
#include "Tim/SException.h"
#include "Tim\file.h"
//////////////////////////////////////////////////////////////////////////
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg = {0};

	//
	curl_global_init(CURL_GLOBAL_ALL);

	GUIWnd guiWnd;

	CreateMutex(NULL, FALSE, TEXT("NaviFirmEx_Mutex"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		int ans = MessageBox(GetActiveWindow(), \
			TEXT("���Ѿ�������һ��ʵ���ˣ�����Ҫ��������ǰ����������һ��ʵ����\r\n")
			TEXT("[ȷ��]��ǰ��ǰʵ��\r\n[��]������һ��ʵ��"),
			TEXT("ŵ���ǹ̼�������"), MB_ICONQUESTION | MB_YESNO);

		if (ans == IDYES)
		{
			HWND hWnd = FindWindow(TEXT("#32770"), TEXT("NOKIA�̼�������"));
			SetForegroundWindow(hWnd);	//���������������ʱֻ��ǰһ��
			return 0;
		}
	}

	Tim::SException::install();

	try
	{
		guiWnd.init(hInst, NULL);
		guiWnd.create(IDD_MAIN_GUI);

		HACCEL hAcc = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATOR1));

		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (
				!TranslateAccelerator(guiWnd.getSelf(), hAcc, &msg) &&
				!IsDialogMessage(guiWnd.getSelf(), &msg)
				)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	catch(const TCHAR *what)
	{
		MessageBox(GetActiveWindow(), what, TEXT("�쳣"), MB_ICONERROR);
	}
	catch(const SException &e)
	{
// 		TString str;
// 		str.format(TEXT("��[0x%08X]������[%hS]�쳣�����򼴽��˳���"), e.where(), e.what());
		char info[256];
		wsprintfA(info, "��[0x%08X]������[%s]�쳣�����򼴽��˳���", e.where(), e.what());
		MessageBoxA(GetActiveWindow(), info, "�쳣", MB_ICONERROR);
	}
	catch(std::exception &e)
	{
		MessageBoxA(GetActiveWindow(), e.what(), "C++ Standard Exception", MB_ICONERROR);
	}
	catch(...)
	{
		MessageBox(GetActiveWindow(), TEXT("δ֪�쳣�����򼴽��˳���"), TEXT("�쳣"), MB_ICONERROR);
	}

	curl_global_cleanup();

	return msg.wParam;
}