
// ShowPicDemoDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CShowPicDemoDlg �Ի���



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


// CShowPicDemoDlg ��Ϣ�������

BOOL CShowPicDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CShowPicDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO:  ѡ��ͼƬ
	//���ļ�
	CString filePath = _T("");
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����) 
	CString defaultDir = _T("./");   //Ĭ�ϴ򿪵��ļ�·�� 
	CString fileName = _T("");         //Ĭ�ϴ򿪵��ļ��� 
	CString filter = _T("ͼ�� (*.bmp; *.jpg; *.png)|*.bmp;*.jpg;*.png||");   //�ļ����ǵ����� 
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (openFileDlg.DoModal() == IDOK)
	{
		filePath = openFileDlg.GetPathName();
	}
	//
	if (filePath == _T(""))
	{
		MessageBox(_T("û��ѡ���ļ���"));
		return;
	}
	char* path = cstring_to_char(filePath);
	//std::string path;
	//cstring2string(filePath, path);
	mSource = cv::imread(path);
	delete[] path;
	if (mSource.empty())
	{
		MessageBox(_T("ͼƬ��ȡʧ�ܣ�"));
		return;
	}
	MakeShowingImage(mSource, mShow);
	ConvertMatToCImage(mShow, mCImageShow);
	RepaintPicControl();
}


void CShowPicDemoDlg::OnBnClickedBtnProcess()
{
	// TODO:  ����
	//Դͼ����mSource���档

	int magnification;
	int mag = 10;				//��ֱ֪����΢��		
	int algorithmID = 2;
	MagPara1 magPara1 = { 2.5, 0, 65, 6, 500 };
	MagPara magPara = { algorithmID, magPara1 };
	getImageMagnification(mSource, mag, magPara, magnification);


	// ������Ϻ󣬼ٶ��ѽ��д�� Mat dst�������ˣ���ͼ�������£�
	//1. MakeShowingImage(dst, mShow);
	// 2. ConvertMatToCImage(mShow, mCImageShow);
	// 3. RepaintPicControl();
}


void CShowPicDemoDlg::OnBnClickedmark()
{
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

/*�ڽ����*/
void CShowPicDemoDlg::OnBnClickedblack()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { mSource.cols, mSource.rows };

	IVRectangle rectangle = { point1, point2 };
	AdhesiveResults adhesiveResults;
	//---------------------------�ڽ������----------------------------------//rectInImage
	AdhesivePara adhesivePara = { 1, { 2, 52, 3.5 }, {} };
	AdhesivePara adhesivePara2 = { 2, {}, { 1.0, 3.5 } };//bina
	getAdhesiveResults(mSource, rectangle, adhesivePara2, adhesiveResults);
	int num = adhesiveResults.adhesiveBlack.connectionNum;
	if (num==0){
		CString result = TEXT("û�м�⵽�ڽ�");
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(result);
	}
	int area = 0;
	while (num>=1){
		area = +adhesiveResults.adhesiveBlack.pArea[num-1];
		num--;
	}
	CString result;
	result.Format(_T("%d"), area);
	//CString result = TEXT("��ά��:\r\n");
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

/*���߼��*/
void CShowPicDemoDlg::OnBnClickedlight()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BrightLinePara blp;

	//��һ���㷨
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
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(TEXT("��ά����ʧ��"));
	}
	else {
		CString result = TEXT("��ά��:\r\n");
		result = result + CString(detect_str);
		GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(result);
	}

	MakeShowingImage(mSource, mShow);
	ConvertMatToCImage(mShow, mCImageShow);
	RepaintPicControl();

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}



