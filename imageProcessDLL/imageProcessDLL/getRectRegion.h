#pragma once
// ʹ��opencv������opencv���ͷ�ļ�
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include "Halconcpp.h"
#include "IVImageProcess.h"





// ͼ����������ȡ
/*****************************************************************************
*  @brief    : ��ȡ�����ͼ��
*****************************************************************************/
extern"C" __declspec(dllexport)  int getRectRegion(
	IN cv::Mat& inImage,								// ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,               // ����ľ�������
	OUT cv::Mat & outImage		    // ���ؾ��������ڵ�ͼ��
	);