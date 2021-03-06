#pragma once
// 使用opencv，调用opencv相关头文件
#include <core/core.hpp>
#include <highgui/highgui.hpp>


// 定义输入输出标识
#define IN
#define OUT
#define INOUT

typedef struct _MagPara1
{
	float sigma;			//高斯算子 默认值：2.5
	int minThresh;			//最低阈值 默认值：0
	int maxThresh;			//最高阈值 默认值：65
	int Openradius;			//开运算半径 默认值：6
	int minArea;			//最小面积  默认值：500
}MagPara1;
typedef struct _MagPara
{
	int algorithmID;                  // 算法ID从1开始，往上增加(1,2)
	MagPara1 magPara1;

}MagPara;
// 图像放大率计算
/*****************************************************************************
*  @brief    : 计算系统的图像放大率
*****************************************************************************/
extern"C" __declspec(dllexport)  int getImageMagnification(
	IN cv::Mat& inImage,                            // 用来测定放大率的源图片
	IN int diameter,						       // 已知圆实际直径 （微米）
	IN MagPara magPara,							  // 放大率计算算法的输入参数
	OUT int & magnification                      // 系统的图像放大率，单位：像素/米
	);
