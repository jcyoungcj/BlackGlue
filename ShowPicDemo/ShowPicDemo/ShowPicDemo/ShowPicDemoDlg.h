
// ShowPicDemoDlg.h : ͷ�ļ�

#pragma once
#include <opencv2\opencv.hpp>
#include <mutex>

#include "IVImageProcess.h"
#include "IVMagnificationDLL.h"

// CShowPicDemoDlg �Ի���
class CShowPicDemoDlg : public CDialogEx
{
// ����
public:
	CShowPicDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SHOWPICDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpen();
	cv::Mat mSource;
	cv::Mat mShow;
	CRect mRectShow;		//ͼ����ʾ������λ�ã�����ԭ����PIC�ؼ������Ͻǵ㡣
	CImage mCImageShow;
	std::mutex mMtxShow;
	void RepaintPicControl();
	void MakeShowingImage(cv::Mat& src, cv::Mat& dst);
	void ConvertMatToCImage(cv::Mat& src, CImage& cimage);
	afx_msg void OnBnClickedBtnProcess();
	afx_msg void OnBnClickedmark();
	afx_msg void OnBnClickedblack();
	afx_msg void OnBnClickedlight();
	afx_msg void OnBnClickedButtonEcc200();

};
