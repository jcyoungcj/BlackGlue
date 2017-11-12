#include "getAdhersiveResult_thresh.h"
#include "getRectRegion.h"
#include <opencv2\opencv.hpp>
using namespace Halcon;
using namespace cv;

// 阈值分割法黑胶检测
/*****************************************************************************
*  @brief    : 黑胶检测结果返回
*****************************************************************************/
int getAdhersiveResult_thresh(
	IN Mat& inImage,                              // 拼接后的大图片
	IN IVRectangle rectangle,                    // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,               // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults      // 输出黑胶检测结果
	){
	int ret = 0;
	//矩形坐标校正
	if (rectangle.topLeftPoint.x > rectangle.bottomRightPoint.x) {
		int tmp1 = rectangle.topLeftPoint.x;
		rectangle.topLeftPoint.x = rectangle.bottomRightPoint.x;
		rectangle.bottomRightPoint.x = tmp1;
	}
	if (rectangle.topLeftPoint.y > rectangle.bottomRightPoint.y) {
		int tmp1 = rectangle.topLeftPoint.y;
		rectangle.topLeftPoint.y = rectangle.bottomRightPoint.y;
		rectangle.bottomRightPoint.y = tmp1;
	}
	//输入参数判断
	if (inImage.empty() || rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
		rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID != 1 ||
		(rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x == 0) ||
		(rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y == 0)
		){
		ret = -1; return ret;
	}
	// Local iconic variables 
	Hobject  Image, LightRegion, Region, rs, blackArea;
	Hobject  inner_whiteArea, inner_whiteArea2, whiteRect, in_white;
	Hobject  white_Rect, rss, rectTest, blackRect, out_black;
	Hobject  black_Rect;


	// Local control variables 
	HTuple  Width, Height, WindowHandle, WindowID, white_num, i;
	HTuple  whiteArea, whiteRow, whiteColumn, white_row1, white_column1, outwhiteArea, outwhiteRow, outwhiteColumn, whiteIndex;
	HTuple  white_row2, white_column2, black_num, row1, column1;
	HTuple  row2, column2, black_Area, blackRow, blackColumn;
	HTuple  black_row1, black_column1, black_row2, black_column2;
	HTuple outblcakArea, outblackRow, outblackColumn, blackIndex;
	//1. 获取矩形区域图像
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);
	if (rectInImage.channels() != 3){
		ret = -1; return ret;
	}


	int width = rectInImage.cols;
	int height = rectInImage.rows;
	int pixel[256] = {0};
	int num = width * height / 8;
	int hi, lo, hnum, lnum;

	hi = lo = hnum = lnum = 0;
	for (int i = 0; i < width;i++){
		for (int j = 0; j < height; j++){
			int a = rectInImage.at<uchar>(j, i);
			pixel[a]++;
		}
	}
	for (int i = 0; i < 256; ++i)
	{
		hi += pixel[i] * i;
		hnum += pixel[i];
		if (hnum >= num) break;
	}

	for (int i = 255; i > 0; --i)
	{
		lo += pixel[i] * i;
		lnum += pixel[i];
		if (lnum >= num) break;
	}

	hi = hi / hnum, lo = lo / lnum;

	int thre = (hi + lo) / 2;
	//2.图像格式转换
	Image = MatToHImage(rectInImage);


	rgb1_to_gray(Image, &Image);
	gauss_image(Image, &Image, 3);
	//threshold(Image, &LightRegion, 5, 27);
	//if (adhesivePara.adhesivePara1.minThreshold <= 0)
	//	return -1;
	threshold(Image, &LightRegion, adhesivePara.adhesivePara1.minThreshold, adhesivePara.adhesivePara1.maxThreshold);


	//先腐蚀膨胀
	opening_circle(LightRegion, &Region, adhesivePara.adhesivePara1.openRadius);
	boundary(Region, &rs, "inner_filled");
	fill_up(rs, &rs);
	reduce_domain(Image, rs, &blackArea);
	difference(rs, Region, &inner_whiteArea);

	connection(inner_whiteArea, &inner_whiteArea2);
	count_obj(inner_whiteArea2, &white_num);
	area_center(inner_whiteArea2, &outwhiteArea, &outwhiteRow, &outwhiteColumn);
	tuple_sort_index(outwhiteArea, &whiteIndex);
	tuple_inverse(whiteIndex, &whiteIndex);

	//空白区域画外接矩形
	shape_trans(inner_whiteArea2, &whiteRect, "rectangle1");

	int w = 0;
	for (i = 1; i <= white_num && i<=100; i += 1)
	{
		select_obj(inner_whiteArea2, &in_white, HTuple(whiteIndex[i - 1]) + 1);
		area_center(in_white, &whiteArea, &whiteRow, &whiteColumn);
		if (0 != (whiteArea > 30)){
			select_obj(whiteRect, &white_Rect, HTuple(whiteIndex[i - 1]) + 1);
			smallest_rectangle1(white_Rect, &white_row1, &white_column1, &white_row2, &white_column2);
			int ii = i[0].I();
			adhesiveResults.adhesiveWhite.pArea[ii-1] = whiteArea[0].D();
			adhesiveResults.adhesiveWhite.pRectangle[ii-1].topLeftPoint.y = white_row1[0].D() + rectangle.topLeftPoint.y;
			adhesiveResults.adhesiveWhite.pRectangle[ii-1].topLeftPoint.x = white_column1[0].D() + rectangle.topLeftPoint.x;
			adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.y = white_row2[0].D() + rectangle.topLeftPoint.y;
			adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.x = white_column2[0].D() + rectangle.topLeftPoint.x;
			w++;
		}
	}
	if (w > 100){
		adhesiveResults.adhesiveWhite.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveWhite.connectionNum = w;
	}


	//---------------------------黑胶区域检测结果----------------------------------//
	connection(rs, &rss);
	count_obj(rss, &black_num);
	area_center(rss, &outblcakArea, &outblackRow, &outblackColumn);
	tuple_sort_index(outblcakArea, &blackIndex);
	tuple_inverse(blackIndex, &blackIndex);
	//黑胶画出外接矩形
	int b = 0;
	shape_trans(rss, &blackRect, "rectangle1");

	for (i = 1; i <= black_num; i += 1)
	{
		select_obj(rss, &out_black, HTuple(blackIndex[i - 1]) + 1);
		area_center(out_black, &black_Area, &blackRow, &blackColumn);
		if (0 != (black_Area>300)){
			select_obj(blackRect, &black_Rect, HTuple(blackIndex[i - 1]) + 1);
			smallest_rectangle1(black_Rect, &black_row1, &black_column1, &black_row2, &black_column2);
			int ii = i[0].I();
			adhesiveResults.adhesiveBlack.pArea[ii - 1] = black_Area[0].D();
			adhesiveResults.adhesiveBlack.pRectangle[ii - 1].topLeftPoint.y = black_row1[0].D() + rectangle.topLeftPoint.y;
			adhesiveResults.adhesiveBlack.pRectangle[ii - 1].topLeftPoint.x = black_column1[0].D() + rectangle.topLeftPoint.x;
			adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.y = black_row2[0].D() + rectangle.topLeftPoint.y;
			adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.x = black_column2[0].D() + rectangle.topLeftPoint.x;
			b++;
		}

	}
	if (b > 100){
		adhesiveResults.adhesiveBlack.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveBlack.connectionNum = b ;
	}

#ifdef _DEBUG
	/*halcon图像显示*/
	//Halcon::get_image_size(Image, &Width, &Height);
	//if (HDevWindowStack::IsOpen())
	//	Halcon::close_window(HDevWindowStack::Pop());
	//Halcon::set_window_attr("background_color", "black");
	//Halcon::open_window(0, 0, Width, Height, 0, "", "", &WindowHandle);
	//HDevWindowStack::Push(WindowHandle);

	//if (HDevWindowStack::IsOpen())
	//	set_color(HDevWindowStack::GetActive(), "red");
	//if (HDevWindowStack::IsOpen())
	//	Halcon::disp_obj(Image, HDevWindowStack::GetActive());
	//	Halcon::disp_obj(rs, HDevWindowStack::GetActive());

	//Sleep(1500);
	//Halcon::close_window(HDevWindowStack::Pop());
	
	/* opencv画圆 */
	int bb = adhesiveResults.adhesiveBlack.connectionNum;			// adhesiveResults.adhesiveBlack.connectionNum;
	while (bb){
		Rect r(cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(rectInImage, r, Scalar(0, 255, 255), 2);

		bb--;
	}

	int ww = adhesiveResults.adhesiveWhite.connectionNum;
	while (ww){
		Rect r(cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(rectInImage, r, Scalar(0, 255, 0), 2);
		ww--;
	}
	
//	cv::imwrite("D://glue_thresh.bmp", rectInImage);

#endif

	return 0;
}