#include "getAdhersiveResult_thresh.h"
#include "getRectRegion.h"
#include <opencv2\highgui\highgui.hpp> 
#include <opencv2\opencv.hpp>

using namespace Halcon;
using namespace cv;
// 阈值分割法黑胶检测
/*****************************************************************************
*  @brief    : 黑胶检测结果返回
*****************************************************************************/
int getAdhersiveResult_binary(
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
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID != 2
		){
		ret = -1; return ret;
	}

	// 方法一:直接调用 brightLine 
	//二： 利用直方图
	// Local iconic variables 
	Hobject  Image, Red, Green, Blue, ImageGrayGaus;
	Hobject  DarkRegion, minblackArea, blackBoundary, blackBoundary2;
	Hobject  Bigblack, innerWhite, innerWhiteImage, DarkRegion2;
	Hobject  blackRect, out_black, Black_Rect, innerWhiteImage2;
	Hobject  whiteRect, out_white, White_Rect;


	// Local control variables 
	HTuple  WindowID, AbsoluteHisto, RelativeHisto;
	HTuple  MinThresh, MaxThresh, RelativeHiso, PeakGray, black_num;
	HTuple  outBlackArea, outBlackRow, outBlackColumn, BlackIndex;
	HTuple  i, BlackArea, BlackRow, BlackColumn, Black_row1;
	HTuple  Black_column1, Black_row2, Black_column2, white_num;
	HTuple  outWhiteArea, outWhiteRow, outWhiteColumn, WhiteIndex;
	HTuple  MeanGray, DeviationGray, WhiteArea, WhiteRow, WhiteColumn;
	HTuple  White_row1, White_column1, White_row2, White_column2;
	//1. 获取矩形区域图像
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);

	//2.图像格式转换
	Image = MatToHImage(rectInImage);
	decompose3(Image, &Red, &Green, &Blue);
	derivate_gauss(Red, &ImageGrayGaus, 2.4, "none");
	gray_histo(ImageGrayGaus, ImageGrayGaus, &AbsoluteHisto, &RelativeHisto);
	histo_to_thresh(AbsoluteHisto, 4, &MinThresh, &MaxThresh);
	threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), HTuple(MaxThresh[0]));
	
	//获取灰度最大值
	//gray_histo(ImageGrayGaus, ImageGrayGaus, &AbsoluteHisto, &RelativeHiso);
	//PeakGray = (AbsoluteHisto.SortIndex())[255];
	//threshold(ImageGrayGaus, &DarkRegion, 0, PeakGray + 25);


	reduce_domain(Red, DarkRegion, &minblackArea);
	boundary(minblackArea, &blackBoundary, "inner_filled");

	fill_up(blackBoundary, &blackBoundary2);
	reduce_domain(Red, blackBoundary2, &Bigblack);

	difference(Bigblack, minblackArea, &innerWhite);
	reduce_domain(Red, innerWhite, &innerWhiteImage);


	//*******************黑胶区域********************************
	connection(DarkRegion, &DarkRegion2);
	select_shape(DarkRegion2, &DarkRegion2, "area", "and", 400, 999999999999);
	count_obj(DarkRegion2, &black_num);
	area_center(DarkRegion2, &outBlackArea, &outBlackRow, &outBlackColumn);
	//**********************************************排序有问题,zhengque
	tuple_sort_index(outBlackArea, &BlackIndex);
	tuple_inverse(BlackIndex, &BlackIndex);
	//先筛选，画出外接矩形

	shape_trans(DarkRegion2, &blackRect, "rectangle1");
	int b = black_num[0].D();
	if (b>100){
		adhesiveResults.adhesiveBlack.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveBlack.connectionNum = b;
	}
	for (i = 1; i <= black_num&& i<=100; i += 1)
	{
		select_obj(DarkRegion2, &out_black, HTuple(BlackIndex[i - 1]) + 1);
		area_center(out_black, &BlackArea, &BlackRow, &BlackColumn);
		select_obj(blackRect, &Black_Rect, i);
		smallest_rectangle1(Black_Rect, &Black_row1, &Black_column1, &Black_row2, &Black_column2);
		int ii = i[0].I();
		adhesiveResults.adhesiveBlack.pArea[ii - 1] = BlackArea[0].D();
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].topLeftPoint.y = Black_row1[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].topLeftPoint.x = Black_column1[0].D() + rectangle.topLeftPoint.x;
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.y = Black_row2[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.x = Black_column2[0].D() + rectangle.topLeftPoint.x;
	
	}

	//*******************画内部空白区域****************************
	connection(innerWhiteImage, &innerWhiteImage2);
	select_shape(innerWhiteImage2, &innerWhiteImage2, "area", "and", 10, 100000);
	count_obj(innerWhiteImage2, &white_num);
	area_center(innerWhiteImage2, &outWhiteArea, &outWhiteRow, &outWhiteColumn);
	tuple_sort_index(outWhiteArea, &WhiteIndex);
	tuple_inverse(WhiteIndex, &WhiteIndex);
	//画出外接矩形
	shape_trans(innerWhiteImage2, &whiteRect, "rectangle1");
	int w = white_num[0].D();
	if (w>100){
		adhesiveResults.adhesiveWhite.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveWhite.connectionNum = w;
	}
	for (i = 1; i <= white_num&& i<=100; i += 1)
	{
		select_obj(innerWhiteImage2, &out_white, HTuple(WhiteIndex[i - 1]) + 1);
		area_center(out_white, &WhiteArea, &WhiteRow, &WhiteColumn);
		select_obj(whiteRect, &White_Rect, i);
		smallest_rectangle1(White_Rect, &White_row1, &White_column1, &White_row2, &White_column2);
		int ii = i[0].I();
		adhesiveResults.adhesiveWhite.pArea[ii - 1] = WhiteArea[0].D();
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].topLeftPoint.y = White_row1[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].topLeftPoint.x = White_column1[0].D() + rectangle.topLeftPoint.x;
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.y = White_row2[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.x = White_column2[0].D() + rectangle.topLeftPoint.x;

	}

#ifdef _DEBUG
	int bb = adhesiveResults.adhesiveBlack.connectionNum;
	while (bb){
		Rect r( cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(rectInImage, r, Scalar(0, 255, 255), 1);
		bb--;
	}


	int ww = adhesiveResults.adhesiveWhite.connectionNum;
	while (ww){
		Rect r(cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(rectInImage, r, Scalar(0, 255, 0), 1);
		ww--;
	}

	imwrite("D:\\ou_bina.jpg", rectInImage);

#endif
	return 0;

}



int getAdhersiveResult_binary2(
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
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID!=2
		){
		ret = -1; return ret;
	}
	// Local iconic variables 
	Hobject  Image, LightRegion, Region, rs, blackArea;
	Hobject  inner_whiteArea, inner_whiteArea2, whiteRect, in_white;
	Hobject  white_Rect, rss, rectTest, blackRect, out_black;
	Hobject  black_Rect;


	// Local control variables 
	HTuple  Width, Height, WindowID, white_num, i;
	HTuple  whiteArea, whiteRow, whiteColumn, white_row1, white_column1;
	HTuple  white_row2, white_column2, black_num, row1, column1;
	HTuple  row2, column2, black_Area, blackRow, blackColumn;
	HTuple  black_row1, black_column1, black_row2, black_column2;
	HTuple  outwhiteArea, outwhiteRow, outwhiteColumn, whiteIndex;
	HTuple  outblcakArea, outblackRow, outblackColumn, blackIndex;
	//1. 获取矩形区域图像
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);

	//2.图像格式转换
	Image = MatToHImage(rectInImage);
	rgb1_to_gray(Image, &Image);
	derivate_gauss(Image, &Image, adhesivePara.adhesivePara2.sigma, "none");
	bin_threshold(Image, &LightRegion);
	//先腐蚀膨胀
	opening_circle(LightRegion, &Region, adhesivePara.adhesivePara2.openRadius);

	boundary(Region, &rs, "inner_filled");
	fill_up(rs, &rs);
	reduce_domain(Image, rs, &blackArea);
	difference(rs, Region, &inner_whiteArea);

	connection(inner_whiteArea, &inner_whiteArea2);
	//结果显示
	//if (HDevWindowStack::IsOpen())
	//	disp_obj(inner_whiteArea2, HDevWindowStack::GetActive());
	//
	count_obj(inner_whiteArea2, &white_num);
	area_center(inner_whiteArea2, &outwhiteArea, &outwhiteRow, &outwhiteColumn);
	tuple_sort_index(outwhiteArea, &whiteIndex);
	tuple_inverse(whiteIndex, &whiteIndex);
	//空白区域画外接矩形
	shape_trans(inner_whiteArea2, &whiteRect, "rectangle1");
	int w = white_num[0].D();
	if (w>100){
		adhesiveResults.adhesiveWhite.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveWhite.connectionNum = w;
	}

	for (i = 1; i <= white_num && i<100; i += 1)
	{
		select_obj(inner_whiteArea2, &in_white, HTuple(whiteIndex[i - 1]) + 1);
		area_center(in_white, &whiteArea, &whiteRow, &whiteColumn);
		select_obj(whiteRect, &white_Rect, HTuple(whiteIndex[i - 1]) + 1);
		smallest_rectangle1(white_Rect, &white_row1, &white_column1, &white_row2, &white_column2);
		int ii = i[0].I();
		adhesiveResults.adhesiveWhite.pArea[ii-1] = whiteArea[0].D();
		adhesiveResults.adhesiveWhite.pRectangle[ii-1].topLeftPoint.y = white_row1[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveWhite.pRectangle[ii-1].topLeftPoint.x = white_column1[0].D() + rectangle.topLeftPoint.x;
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.y = white_row2[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveWhite.pRectangle[ii - 1].bottomRightPoint.x = white_column2[0].D() + rectangle.topLeftPoint.x;

	}

	//---------------------------黑胶区域检测结果----------------------------------//
	connection(rs, &rss);
	count_obj(rss, &black_num);
	area_center(rss, &outblcakArea, &outblackRow, &outblackColumn);
	tuple_sort_index(outblcakArea, &blackIndex);
	tuple_inverse(blackIndex, &blackIndex);
	//黑胶画出外接矩形
	int b = black_num[0].D();
	if (b>100){
		adhesiveResults.adhesiveBlack.connectionNum = 100;
	}
	else{
		adhesiveResults.adhesiveBlack.connectionNum = b;
	}
	
	shape_trans(rss, &blackRect, "rectangle1");

	for (i = 1; i <= black_num && i<=100; i += 1)
	{
		select_obj(rss, &out_black, HTuple(blackIndex[i - 1]) + 1);
		area_center(out_black, &black_Area, &blackRow, &blackColumn);
		select_obj(blackRect, &black_Rect, HTuple(blackIndex[i - 1]) + 1);
		smallest_rectangle1(black_Rect, &black_row1, &black_column1, &black_row2, &black_column2);
		
		int ii = i[0].I();
		adhesiveResults.adhesiveBlack.pArea[ii-1] = black_Area[0].D();
		adhesiveResults.adhesiveBlack.pRectangle[ii-1].topLeftPoint.y = black_row1[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveBlack.pRectangle[ii-1].topLeftPoint.x = black_column1[0].D() + rectangle.topLeftPoint.x;
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.y = black_row2[0].D() + rectangle.topLeftPoint.y;
		adhesiveResults.adhesiveBlack.pRectangle[ii - 1].bottomRightPoint.x = black_column2[0].D() + rectangle.topLeftPoint.x;
	}

#ifdef _DEBUG
	int bb = adhesiveResults.adhesiveBlack.connectionNum;
	while (bb){

		Rect r(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.y - rectangle.topLeftPoint.y,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.y - rectangle.topLeftPoint.y);
		cv::rectangle(rectInImage, r, Scalar(0, 255, 255), 1);		
		bb--;
		
	}


	int ww = adhesiveResults.adhesiveWhite.connectionNum;
	while (ww){
		Rect r(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.y - rectangle.topLeftPoint.y,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.y - rectangle.topLeftPoint.y);
		cv::rectangle(rectInImage, r, Scalar(0, 255, 0), 1);	
		ww--;
	}

	namedWindow("黑胶检测结果", 0);
	resizeWindow("黑胶检测结果", 800, 600);
	imshow("黑胶检测结果", rectInImage);
	waitKey(100);
	cv::destroyWindow("黑胶检测结果");

#endif

	return 0;
}