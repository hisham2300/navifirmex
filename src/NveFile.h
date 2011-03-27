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

#ifndef __NVE_FILE_H__
#define __NVE_FILE_H__

//���ڶ�ȡ������δ������ɵ�����

#include <list>

#include "MiniDownloader.h"
#include "tim/TString.h"

class NveFile
{
public:

	//���ļ�����
	bool load(const Tim::TString &nveFile);

	//���浽�ļ�
	bool save();
	bool save(const Tim::TString &file);

	//����һ��
	void add(const TiFile &tiFile);

	//ɾ��ָ����һ��
	void erase(size_t i);

	bool empty();

	void clear();

	void add(const std::list<TiFile> &fileList);

	const std::list<TiFile> & getlist() const {
		return _nvFileList;
	}
private:
	 std::list<TiFile>	_nvFileList;
	 Tim::TString		_filePath;
};

#endif