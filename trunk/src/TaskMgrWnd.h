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

#pragma once

#ifndef __TASK_MGR_WND_H__
#define __TASK_MGR_WND_H__

#include <vector>
#include <list>

#include "tim\window.h"
#include "ProgressView.h"
#include "MiniDownloader.h"
#include "data_type.h"

using namespace Tim;

class TaskMgrWnd : public Window
{
public:
	TaskMgrWnd(){
		_taskView = 0;
	}
	~TaskMgrWnd();

	virtual void init(HINSTANCE hinst, HWND hwndParent);

public:
	//����������
	void newTask(const FileInfo &fileInfo);
	//�½���������
	void newTask(const TiFile &tiFile);
	bool hasTask() const {	return !_vDownloader.empty(); }

	static TString MakeSpeedFmt(long lSize);

protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(int type, int nWidth, int nHeight);
	void OnCommand(int id, HWND hwndCtl, UINT uNotifyCode);
	void OnDropFiles(HDROP hDrop);
	void OnDownloadStatus(DownloadStatus *pds);
	void OnFinish(BOOL bSuccess, MiniDownloader *pmd);
	void OnTimer(UINT uid);
	void OnNotify(int id, NMHDR *pnmh);
	void OnAboutClose();

	bool InitListImage();

	enum TaskStatus
	{
		ts_downloding = 0,
		ts_pause,
		ts_finish,
		ts_failed
	};

	void SetTaskStatus(int i, TaskStatus status);

	bool SelectFolder(TString &folder);
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

private:
	ProgressView * _taskView;
	std::vector<MiniDownloader*> _vDownloader;
	//��_vDownloaderһһ��Ӧ
	std::vector<long> _vDownloadedSize;
	//Ĭ�ϱ���Ŀ¼
	TString _baseFolder;

	std::list<TiFile> _tiFileList;
};

#endif