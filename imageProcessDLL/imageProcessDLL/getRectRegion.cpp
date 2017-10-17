
#include "getRectRegion.h"


// 图像矩形区域截取
/*****************************************************************************
*  @brief    : 截取区域的图像
*****************************************************************************/
extern"C" __declspec(dllexport)  int getRectRegion(
	IN cv::Mat& inImage,								// 拼接后的大图片
	IN IVRectangle rectangle,               // 输入的矩形区域
	OUT cv::Mat & outImage		    // 返回矩形区域内的图像
	)
{
	//if (rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
	//	rectangle.bottomRightPoint.x>inImage.cols || rectangle.bottomRightPoint.y>inImage.rows
	//	)
	//{
	//	return -1;
	//}
	int width = rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x;
	int height = rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y;
	outImage = inImage(cv::Rect(rectangle.topLeftPoint.x, rectangle.topLeftPoint.y, width, height));
	outImage.copyTo(outImage);
	return 0;
}
