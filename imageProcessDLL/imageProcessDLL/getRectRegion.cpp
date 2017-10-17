
#include "getRectRegion.h"


// ͼ����������ȡ
/*****************************************************************************
*  @brief    : ��ȡ�����ͼ��
*****************************************************************************/
extern"C" __declspec(dllexport)  int getRectRegion(
	IN cv::Mat& inImage,								// ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,               // ����ľ�������
	OUT cv::Mat & outImage		    // ���ؾ��������ڵ�ͼ��
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
