
// AutoVercode.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoVercodeApp:
// �йش����ʵ�֣������ AutoVercode.cpp
//

class CAutoVercodeApp : public CWinApp
{
public:
	CAutoVercodeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoVercodeApp theApp;