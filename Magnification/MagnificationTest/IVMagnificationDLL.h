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
	double sigma;									//gauss		  默认值：2.4
	double alpha;									// canny算子  默认值： 2.0 ,1.0
	int minGirth;									// 最小周长	  默认值：50
	int maxLackGirth;								// 圆最大缺口  默认值：20
}MagPara1;
typedef struct _MagPara2
{
	float sigma;			//高斯算子 默认值：2.5
	int minThresh;			//最低阈值 默认值：0
	int maxThresh;			//最高阈值 默认值：65
	int Openradius;			//开运算半径 默认值：6
	int minArea;			//最小面积  默认值：500
}MagPara2;
typedef struct _MagPara
{
	int algorithmID;                  // 算法ID从1开始，往上增加
	MagPara1 magPara1;
	MagPara2 magPara2;
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