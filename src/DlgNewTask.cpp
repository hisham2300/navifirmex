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

#include <shlobj.h>

#include "DlgNewTask.h"
#include "res\resource.h"
#include "nm_message.h"
#include "common.h"
#include "Tim\File.h"
#include "LangHelper.h"

using namespace Tim;

BOOL DlgNewTask::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		_taskMgrWnd = (TaskMgrWnd*)_pData;
		OnInit();
		return TRUE;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam));
		break;

	case WM_CLOSE:
		hideWindow();
		_taskList.DeleteAllItems();
		std::vector<FileInfo>().swap(_vFileList);
		break;
	}

	return FALSE;
}

void DlgNewTask::OnInit()
{
	_taskList.assign(IDC_LIST_TASK, getSelf());

	_ctlSelAll.init(getHinst(), getSelf());
	_ctlSelReverse.init(getHinst(), getSelf());

	_ctlSelAll.create(HwndFromId(IDS_SEL_ALL), IDS_SEL_ALL);
	_ctlSelReverse.create(HwndFromId(IDS_SEL_INVERSE), IDS_SEL_INVERSE);

	_taskList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	_taskList.InsertColumn(0, TEXT("文件名"));
	_taskList.InsertColumn(1, TEXT("大小"));

	_taskList.SetColumnWidth(0, 270);
	_taskList.SetColumnWidth(1, 70);

	char szFolder[MAX_PATH] = {0};

	::SendMessage(getParent(), NM_GETLASTDIR, 0, (LPARAM)szFolder);

	::SendDlgItemMessageA(_hWnd, IDE_FOLDER, WM_SETTEXT, 0, (LPARAM)(szFolder));
}

bool DlgNewTask::SelectFolder(Tim::TString &folder)
{
	BROWSEINFO bi = {0};

	TCHAR	szFilePath[MAX_PATH] = {0};
	ITEMIDLIST *pidl = {0};

	bi.hwndOwner	= _hWnd;       
	bi.lpszTitle	= TEXT("请选择要保存到的文件夹");       
	bi.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
	bi.lpfn			= BrowseCallbackProc;

	char szFolder[MAX_PATH] = {0};

	::SendMessage(getParent(), NM_GETLASTDIR, 0, (LPARAM)szFolder);

#ifdef	UNICODE
	wchar_t wszFolder[MAX_PATH];
	atow(szFolder, wszFolder, MAX_PATH);
	bi.lParam = (LPARAM)wszFolder;
#else
	bi.lParam = (LPARAM)szFolder;
#endif

	pidl = ::SHBrowseForFolder(&bi);
	if (pidl == NULL)
		return false;

	if (!::SHGetPathFromIDList(pidl, szFilePath))
		return false;

	folder = szFilePath;
	folder.trim();

	if (!folder.empty())
	{
		if (folder.at(folder.length()-1) != TEXT('\\')){
			folder.push_back(TEXT('\\'));
		}
	}

#ifdef UNICODE
	wtoa(folder.c_str(), szFolder, MAX_PATH);
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(szFolder));
#else
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(folder.c_str()));
#endif
	return true;
}

int CALLBACK DlgNewTask::BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if   (uMsg == BFFM_INITIALIZED ){	//设置初始化目录
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

void DlgNewTask::AddTask(const FileInfo &file)
{
	_vFileList.push_back(file);

	int item = _taskList.GetItemCount();

	_taskList.InsertItem(item, (TCHAR*)file.name.c_str());

	TString size = MakeFileSizeFmt(file.size);

	_taskList.SetItemText(item, 1, (TCHAR*)size.c_str());

	_taskList.SetCheck(item);
}

void DlgNewTask::AddTask(const std::vector<FileInfo> &file)
{
	for (size_t i=0; i<file.size(); i++){
		AddTask(file[i]);
	}
}

void DlgNewTask::OnCommand(UINT id)
{
	switch (id)
	{
	case IDB_BROWSE:
		{
			TString folder;
			if (SelectFolder(folder) && !folder.empty()){
				SetItemText(IDE_FOLDER, folder);
			}
		}
		break;

	case IDB_DOWN_NOW:
		DownNow();
		break;

	case IDB_DOWN_CANCEL:
		sendMsg(WM_CLOSE);
		break;

	case IDS_SEL_ALL:
	case IDS_SEL_INVERSE:
		doSelection(id == IDS_SEL_ALL);
		break;
	}
}

void DlgNewTask::doSelection(bool fSelAll /* = true */)
{
	int count = _taskList.GetItemCount();
	if (count <= 0)
		return ;

	if (fSelAll)
	{
		for(int i=0; i<count; i++){
			_taskList.SetCheck(i);
		}
	}
	else
	{
		for(int i=0; i<count; i++)
		{
			if (_taskList.GetCheck(i)){
				_taskList.SetCheck(i, FALSE);
			}else{
				_taskList.SetCheck(i);
			}
		}
	}
}

void DlgNewTask::DownNow()
{
	TString folder = GetItemText(IDE_FOLDER);
	//
	folder.trim();

	if (folder.empty())
	{
		//msgBox(TEXT("您必须选择一个存放的目录！"), TEXT("提醒"), MB_ICONINFORMATION);
		LangHelper lang;

		TCHAR szLang[MAX_PATH] = {0};

		SendMessage(getParent(), NM_GETLANGPATH, 0, reinterpret_cast<LPARAM>(szLang));

		TString text = TEXT("您必须选择一个存放的目录！");
		TString title = TEXT("提醒");

		if (szLang[0])
		{
			if (lang.Load(szLang)){
				lang.GetMsgBox("EmptyDir", text, title);
			}
		}

		Window::msgBox(text, title, MB_ICONINFORMATION);

		return ;
	}

	//Fixed:
	//when user typed by keybord config will not save & not add the last '\'
	//Apr. 15, 2011
	if (folder.at(folder.length()-1) != TEXT('\\')){
		folder.push_back(TEXT('\\'));
	}

	char szFolder[MAX_PATH] = {0};
#ifdef UNICODE
	wtoa(folder.c_str(), szFolder, MAX_PATH);
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(szFolder));
#else
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(folder.c_str()));
#endif

	Tim::File::MakeDir(folder);	//确保存在文件夹

	int count = _taskList.GetItemCount();

	if (count <= 0)
	{
		postMsg(WM_CLOSE);
		return ;
	}

	_taskMgrWnd->setFolder(folder);

	bool hasTask = false;

	for(int i=0; i<count; i++)
	{
		if (_taskList.GetCheck(i))
		{
			_taskMgrWnd->newTask(_vFileList[i]);
			hasTask = true;
		}
	}

	if (hasTask)
	{
		BOOL fShow = TRUE;
		::SendMessage(getParent(), NM_SHOWTASKMGR, 0, (LPARAM)&fShow);

		if (fShow){
			_taskMgrWnd->showWindow();
		}
	}

	postMsg(WM_CLOSE);
}

void DlgNewTask::ResizeLabels()
{
	HDC hdc = GetDC(_hWnd);
	SIZE size;
	Rect rect;
	//HGDIOBJ hOldObj = SelectObject(hdc, DEFAULT_GUI_FONT);

	TString text = _ctlSelAll.getText();

	GetTextExtentPoint(hdc, text, text.length(), &size);

	_ctlSelAll.resizeTo(size.cx, 15);
	_ctlSelAll.GetWindowRect(&rect);
	ScreenToClient(&rect);

	_ctlSelReverse.moveTo(rect.right + 10, rect.top);

	text = _ctlSelReverse.getText();
	GetTextExtentPoint(hdc, text, text.length(), &size);
	_ctlSelReverse.resizeTo(size.cx, 15);
}