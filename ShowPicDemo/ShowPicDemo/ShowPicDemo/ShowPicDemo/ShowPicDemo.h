
// ShowPicDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CShowPicDemoApp: 
// �йش����ʵ�֣������ ShowPicDemo.cpp
//

class CShowPicDemoApp : public CWinApp
{
public:
	CShowPicDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CShowPicDemoApp theApp;