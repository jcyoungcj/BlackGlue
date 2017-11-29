
// ShowPicDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowPicDemo.h"
#include "ShowPicDemoDlg.h"
#include "afxdialogex.h"

//using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _SHOW

#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowPicDemoDlg 对话框



CShowPicDemoDlg::CShowPicDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowPicDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowPicDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShowPicDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPEN, &CShowPicDemoDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_PROCESS, &CShowPicDemoDlg::OnBnClickedBtnProcess)
	ON_BN_CLICKED(IDC_mark, &CShowPicDemoDlg::OnBnClickedmark)
	ON_BN_CLICKED(IDC_black, &CShowPicDemoDlg::OnBnClickedblack)
	ON_BN_CLICKED(IDC_light, &CShowPicDemoDlg::OnBnClickedlight)
	ON_BN_CLICKED(IDC_BUTTON_ECC200, &CShowPicDemoDlg::OnBnClickedButtonEcc200)
END_MESSAGE_MAP()


// CShowPicDemoDlg 消息处理程序

BOOL CShowPicDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShowPicDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShowPicDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		{
			//std::unique_lock<std::mutex> lck(mMtxShow);
			if (!mCImageShow.IsNull())
				DrawCImageCenter(mCImageShow, GetDlgItem(IDC_PIC), mRectShow);
		}
		
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CShowPicDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShowPicDemoDlg::RepaintPicControl()
{
	CRect rect;
	GetDlgItem(IDC_PIC)->GetClientRect(rect);
	//InvalidateRect(rect, TRUE);
	PostMessage(WM_PAINT);
}

void CShowPicDemoDlg::MakeShowingImage(cv::Mat& src, cv::Mat& dst)
{
	if (src.empty())
		return;
	CWnd* pwnd = GetDlgItem(IDC_PIC);
	CRect rect;
	pwnd->GetClientRect(rect);

	int h, w;
	float ratio = float(src.cols) / float(src.rows);
	if (src.cols < src.rows)
	{
		h = rect.bottom;
		w = h * ratio;
	}
	else
	{
		w = rect.right;
		h = w / ratio;
	}

	resize(src, dst, cv::Size(w, h));
}

void CShowPicDemoDlg::ConvertMatToCImage(cv::Mat& src, CImage& cimage)
{
	Mat2CImage(src.data, src.cols, src.rows, src.channels(), src.step1(), cimage);
}

void CShowPicDemoDlg::OnBnClickedBtnOpen()
{
	// TODO:  选择图片
	//打开文件
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //是否打开(否则为保存) 
	CString defaultDir = _T("./");   //默认打开的文件路径 
	CString fileName = _T("");         //默认打开的文件名 
	CString filter = _T("图像 (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png||");   //文件过虑的类型 
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (openFileDlg.DoModal() == IDOK)
	{
		filePath = openFileDlg.GetPathName();
	}
	//
	if (filePath == _T(""))
	{
		MessageBox(_T("没有选择文件！"));
		return;
	}
	char* path = cstring_to_char(filePath);
	//std::string path;
	//cstring2string(filePath, path);
	mSource = cv::imread(path);
	delete[] path;
	if (mSource.empty())
	{
		MessageBox(_T("图片读取失败！"));
		return;
	}
	MakeShowingImage(mSource, mShow);
	ConvertMatToCImage(mShow, mCImageShow);
	RepaintPicControl();
}


void CShowPicDemoDlg::OnBnClickedBtnProcess()
{
	// TODO:  处理
	//源图像在mSource里面。

	int magnification;
	int mag = 10;				//已知直径，微米		
	int algorithmID = 2;
	MagPara1 magPara1 = { 2.5, 0, 65, 6, 500 };
	MagPara magPara = { algorithmID, magPara1 };
	getImageMagnification(mSource, mag, magPara, magnification);


	// 处理完毕后，假定把结果写到 Mat dst；里面了，画图流程如下：
	//1. MakeShowingImage(dst, mShow);
	// 2. ConvertMatToCImage(mShow, mCImageShow);
	// 3. RepaintPicControl();
}


void CShowPicDemoDlg::OnBnClickedmark()
{
	
	// TODO:  在此添加控件通知处理程序代码
	MarkPara makePara = { 3.0, 3.0, 200, 99999, 20 };
	IVPoint circleCentre;
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { mSource.cols, mSource.rows };
	IVRectangle rectangle = { point1, point2 };
	clock_t read = clock();
	getMarkLocation(mSource, rectangle, makePara, circleCentre);
	//Point p(circleCentre.x, circleCentre.y);
	//circle(mSource, p, 0, Scalar(0, 255, 0));

}

/*黑胶检测*/
void CShowPicDemoDlg::OnBnClickedblack()
{
	// TODO:  在此添加控件通知处理程序代码
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { mSource.cols, mSource.rows };

	IVRectangle rectangle = { point1, point2 };
	AdhesiveResults adhesiveResults;
	//---------------------------黑胶检测结果----------------------------------//rectInImage
	AdhesivePara adhesivePara = { 1, { 2, 52, 3.5 }, {} };
	AdhesivePara adhesivePara2 = { 2, {}, { 1.0, 3.5 } };//bina
	getAdhesiveResults(mSource, rectangle, adhesivePara2, adhesiveResults);
	int num = adhesiveResults.adhesiveBlack.connectionNum;
	if (num==0){
		CString result = TEXT("没有检测到黑胶");
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(result);
	}
	int area = 0;
	while (num>=1){
		area = +adhesiveResults.adhesiveBlack.pArea[num-1];
		num--;
	}
	CString result;
	result.Format(_T("%d"), area);
	//CString result = TEXT("二维码:\r\n");
	GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(result);

	//int bb = adhesiveResults.adhesiveBlack.connectionNum;
	//while (bb){
	//	Rect r(cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
	//		adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
	//		cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
	//		adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
	//	cv::rectangle(mSource, r, Scalar(0, 255, 255), 1);
	//	bb--;
	//}


	//int ww = adhesiveResults.adhesiveWhite.connectionNum;
	//while (ww){
	//	Rect r(cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
	//		adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
	//		cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
	//		adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
	//	cv::rectangle(mSource, r, Scalar(0, 255, 0), 1);
	//	ww--;
	//}
	//MakeShowingImage(mSource, mShow);
	//ConvertMatToCImage(mShow, mCImageShow);
	//RepaintPicControl();

}

/*亮线检测*/
void CShowPicDemoDlg::OnBnClickedlight()
{
	// TODO:  在此添加控件通知处理程序代码
	BrightLinePara blp;

	//第一种算法
	blp.algorithmID = 1;
	blp.brightLinePara1.radius = 15;
	blp.brightLinePara1.thre = 40;

	BrightLineResults blr;
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { mSource.cols, mSource.rows };

	IVRectangle rect = { point1, point2 };
	int ret = getBrightLineResults(mSource, rect, blp, blr);
	cv::Mat tmp = mSource.clone();
	for (int i = 0; i < blr.arrSize; ++i)
		rectangle(tmp, blr.arrContours[i].rectBounding, cv::Scalar(0, 255, 0));

	cv::imshow("brightline.bmp", tmp);
	//MakeShowingImage(tmp, mShow);
	//ConvertMatToCImage(mShow, mCImageShow);
	//RepaintPicControl();

}




void CShowPicDemoDlg::OnBnClickedButtonEcc200()
{
	IVRectangle rect = { IVPoint{ 0, 0 }, IVPoint{ mSource.cols, mSource.rows } };
	char detect_str[255];
	int str_lenght = 0;
	memset(detect_str, 0, 255);
	DMDecode(mSource, rect, DMCodePara{ 200 }, detect_str, str_lenght);

	if (str_lenght == 0) {
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(TEXT("二维码检测失败"));
	}
	else {
		CString result = TEXT("二维码:\r\n");
		result = result + CString(detect_str);
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(result);
	}

	MakeShowingImage(mSource, mShow);
	ConvertMatToCImage(mShow, mCImageShow);
	RepaintPicControl();

	// TODO:  在此添加控件通知处理程序代码
}



