#pragma once
// 使用opencv，调用opencv相关头文件
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include "Halconcpp.h"
#include "IVImageProcess.h"





// 图像矩形区域截取
/*****************************************************************************
*  @brief    : 截取区域的图像
*****************************************************************************/
extern"C" __declspec(dllexport)  int getRectRegion(
	IN cv::Mat& inImage,								// 拼接后的大图片
	IN IVRectangle rectangle,               // 输入的矩形区域
	OUT cv::Mat & outImage		    // 返回矩形区域内的图像
	);