
// ShowPicDemoDlg.h : 头文件

#pragma once
#include <opencv2\opencv.hpp>
#include <mutex>

#include "IVImageProcess.h"
#include "IVMagnificationDLL.h"

// CShowPicDemoDlg 对话框
class CShowPicDemoDlg : public CDialogEx
{
// 构造
public:
	CShowPicDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHOWPICDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpen();
	cv::Mat mSource;
	cv::Mat mShow;
	CRect mRectShow;		//图像显示的区域位置，它的原点是PIC控件的左上角点。
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
