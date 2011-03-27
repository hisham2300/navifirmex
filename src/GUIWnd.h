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
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef	__GUI_WINDOW_H__
#define __GUI_WINDOW_H__
//////////////////////////////////////////////////////////////////////////
#include <list>

#include "Tim\Dialog.h"
#include "Tim\CommonCtrls.h"
#include "res\resource.h"
#include "data_type.h"
#include "Http.h"
#include "TinyXml/tinyxml.h"
#include "ImageEx.h"
#include "ProductList.h"
#include "VariantList.h"
#include "Config.h"
#include "TaskMgrWnd.h"
#include "NveFile.h"
//////////////////////////////////////////////////////////////////////////
using namespace _TIM;
using namespace std;
//////////////////////////////////////////////////////////////////////////
class GUIWnd : public Dialog
{
public:
	GUIWnd():
		_hThreadProduct(INVALID_HANDLE_VALUE), _hThreadRelease(INVALID_HANDLE_VALUE),
		_hThreadVariant(INVALID_HANDLE_VALUE), _hTreadGetImage(INVALID_HANDLE_VALUE),
		_hBitmap(NULL), _hTreadGetFileList(INVALID_HANDLE_VALUE),
		_pConfig(0), _gdiplusToken(0), _imgProcess(0), _sortIndex(0),
		_fPrompt(TRUE), _fOverwrite(TRUE), _nvFile(NULL)
	{
		_taskMgr = NULL;
	}
	~GUIWnd(){}

	//friend class DlgDownload;
protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnCommand(int id, HWND hwndCtl, UINT uNotifyCode);
	void OnNotify(int id, NMHDR *pnmh);
	void OnPaint();
	void OnInit();
	void OnClose();
	void OnDestroy();
	void OnAboutClose(BOOL bNotEmpty, std::list<TiFile> &fileList);
	void OnUrlDown(LPCTSTR url);

	void doProductChange();
	void doReleaseChange();
	void doVariantChange();
	void doProductFilter();
	void doVariantFilter();
	void doCopyUrl(UINT cmd);	//����ָ���ļ�URL�����а�
	void doDownLoad();
	void doSelAll(bool fYes = true);
	void doRefresh(bool fNeedACK = true);			//ˢ��

	//��ȡ��Ʒ�б�
	static DWORD WINAPI GetProductListProc(LPVOID lParam);
	//������������
	int ReadProduct(const char *data, DWORD dwSize);
	//��ȡһ����Ʒ
	void GetProductInfo(TiXmlNode *node);

	//Release�б�
	static DWORD WINAPI GetReleaseListProc(LPVOID lParam);
	int	ReadRelease(const char *data, DWORD dwSize, const TString &productID);
	void GetReleaseInfo(TiXmlNode *node);

	//variant�б�
	static DWORD WINAPI GetVariantListProc(LPVOID lParam);
	int	ReadVariant(const char *data, DWORD dwSize, const TString &releaseID);
	void GetVariantInfo(TiXmlNode *node, TiXmlNode *bodyNode);

	//��ֹ�߳�
	void EndThread(HANDLE &hThread);

	void ShowProducts();
	void ShowReleases();
	void ShowVariants();

	void ShowPopupMenu();

	bool CopyTextToClipbrd(LPCTSTR lpData);

	static bool SaveToFile(LPCTSTR lpPath, LPVOID data, DWORD dwLen);

	void ShowPhonePicture(const Product &curProduct);

	//HBITMAP ImageBufferToHBitmap(const char *buffer, DWORD dwSize);
	HBITMAP ImageFileToHBitmap(const TString &file);

	//�����Ŀ¼
	TString GetRootFolder();
	//�����Ŀ¼+subPath
	TString MakeFilePath(const TString &subPath);

	void RefreshProducts();

	static DWORD WINAPI GetImageProc(LPVOID lParam);
	void DownloadImage(const TString &url, const TString &fileSave);

	//ɾ��folder
	static void DeleteDirectory(const TString &folder);

	static DWORD GetFileData(const TString &filePath, LPVOID buffer);

	static void ReplaceString(TString &srcStr, const TString &what, const TString &with);

	template<typename T>
	T * list_at(list<T> &listSrc, size_t index)
	{
		list<T>::iterator it = listSrc.begin();
		
		for (size_t i = 0; it != listSrc.end(); ++it, ++i)
		{
			if (i == index)
				return &(*it);
		}

		return NULL;
	}
	//�����ļ��Ĵ��ڡ���С�ж��Ƿ������
	static bool FileNeedUpdate(const TString &filePath, DWORD dwMinFileSize = 0);

	void AddSysMenu();

	//ȡ#ID_XX��URL
	string GetFileUrlByID(TiXmlNode *bodyNode, const char *ID);

	void ExportProductToText();
	void ExportProductToXml();
	void ExportVariantToText();
	void ExportVariantToXml();

	TString SelectFilePath(const TString &defName);

	//ȡ��ָ��CODE���ļ��б�
	static DWORD WINAPI GetFileListByCodeProc(LPVOID lParam);
	TString GetSessionID();
	void VariantDataToFiles(const char *data);

	TString GetFileElement(TiXmlNode *fileNode, const char *tagName, TiXmlNode *bodyNode = NULL);
	void ShowFiles();

	//�жϷ��������Ƿ�Ϊ������Ϣ
	bool IsErrorInfo(TiXmlDocument *pDoc, TString &strErrInfo);
	//ȡ��node��һ��childElement���ı�ֵ
	TString GetChildElementText(TiXmlNode *node, const char *childElement);

	bool SaveExportText(LPCTSTR lpDefFileName, const TString &strData);
	void UpdateStatus(const TString &infoText, int nPart = 1);
	void ShowServer(int index = 1);

	void SortListItem(int index);

	//��ѹZIP��name�ļ�����������ָ�룬 ���ݴ�С����dwSize
	static char*	GetFileDataFromZip(const TString& zipFile, const TString& name, DWORD& dwSize);
	//���浽ZIP
	static bool		SaveFileToZip(const TString& zipFile, const TString& name, LPVOID lpData, DWORD dwSize);
	//�ж�ZIP���Ƿ���name�ļ�
	static bool		FileExistsZip(const TString& zipFile, const TString& name); 
	char* GetSoap(Http& http, LPCTSTR lpPost, LPDWORD pdwSize);
	//���Ի�ȡSESSIONID
	TString TryGetSessionID();
	//ȡ���ַ�����Դ
	TString LoadSoapString(UINT uID);

	//��ǰ��������
	static void WebProgress(double total, double now, void* pGuiWnd);

	DWORD GetUrlFileSize(const TString &url);

	//����������񴰿�û�����򴴽�
	void ValidTaskMgr();

public:
		static TString MakeFileSizeFmt(DWORD dwSize);

private:
	TaskMgrWnd * _taskMgr;
	ProductList	_lbProduct;
	ListBox		_lbRelease;
	VariantList	_lbVariant;
	ListView	_lvFile;

	StatusBar	_status;

	Edit		_edProduct;
	Edit		_edVariant;

	Button		_btnDownload;

	list<Product>		_lProduct;
	vector<Release>		_vRelease;
	list<Variant>		_lVariant;

	list<Product>		_lProductFilter;
	list<Variant>		_lVariantFilter;

	TString	_jsessionidStr;

	HANDLE	_hThreadProduct;
	HANDLE	_hThreadRelease;
	HANDLE	_hThreadVariant;
	HANDLE	_hTreadGetImage;
	HANDLE	_hTreadGetFileList;

	Gdiplus::ImageEx	*_imgProcess;
	ULONG_PTR	_gdiplusToken;

	HBITMAP	_hBitmap;

	GetImageProcParam _getImgProcParam;

	//ͨ��CODE���ص�CODE
	TString		_code;
	//��ʾ���ļ��б�
	vector<FileInfo> _vFiles;

	//�������б�
	static char *_szServer[3];

	Config	*_pConfig;

	int		_sortIndex;	//0, 1 only
	enum sort_type{SORT_UP, SORT_DOWN};
	sort_type	_sortType;	//

	//�Ƿ���ʾ�����ļ�
	BOOL	_fPrompt;
	//�û�ѡ�񸲸ǻ�������
	BOOL	_fOverwrite;

	NveFile *_nvFile;
};

#endif