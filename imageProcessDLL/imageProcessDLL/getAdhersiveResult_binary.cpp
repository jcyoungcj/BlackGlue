#include "getAdhersiveResult_thresh.h"
#include "getRectRegion.h"
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include "direct.h"
using namespace Halcon;
using namespace cv;
using namespace std;

#define MIN_NON_UF_PATCH_AREA (100)
void non_uf_detect(const Mat& image,Rect roi,uchar thr,vector<Rect>& uf_rgn,vector<float>& uf_area)
{
	vector<vector<Point>> vcs;
	Mat non_uf_gray;
	Mat non_uf_bin, non_uf_normal_bin;
	Mat non_uf_color = image(roi);
	cvtColor(non_uf_color,non_uf_gray,CV_RGB2GRAY);
	threshold(non_uf_gray,non_uf_bin,thr,255,THRESH_BINARY_INV);
	erode(non_uf_bin,non_uf_bin,getStructuringElement(MORPH_RECT,Size(5,5)));
	dilate(non_uf_bin, non_uf_bin, getStructuringElement(MORPH_RECT, Size(5, 5)));
	findContours(non_uf_bin,vcs,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
	if (vcs.size()>0){
		std::sort(vcs.begin(), vcs.end(), [](const vector<Point>&left, const vector<Point>&right){
			return contourArea(left) > contourArea(right); });
	

#if _DEBUG
	_mkdir("uf");
	Mat partimage = image(roi).clone();
#endif
    
		for (int i = 0; i < (int)vcs.size();++i){ 
			Rect non_uf = boundingRect(vcs[i]);
			float non_uf_arae = (float)contourArea(vcs[i]);
			if (non_uf.width * non_uf.height < MIN_NON_UF_PATCH_AREA){ break; }
			if (i == 100){ break; }
			Rect uf_1;
			uf_1.x = roi.x + non_uf.x;
			uf_1.y = roi.y + non_uf.y;
			uf_1.width = non_uf.width, uf_1.height = non_uf.height;
			uf_rgn.push_back(uf_1);
			uf_area.push_back(non_uf_arae);
#if _DEBUG
			drawContours(partimage,vcs,i,Scalar(0,0,255),CV_FILLED);
#endif
		}

#if _DEBUG
		//imshow("white", partimage);
		//waitKey();
#endif

	}

	return;
}

/*****************************************************************************
*  @brief    : 无胶检测结果返回
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
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID != 2  ||
		(rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x == 0) ||
		(rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y == 0)
		){
		ret = -1; return ret;
	}

	vector<Rect> rgn;
	vector<float> area;
	Rect roi(rectangle.topLeftPoint.x,rectangle.topLeftPoint.y,
		rectangle.bottomRightPoint.x-rectangle.topLeftPoint.x,rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y);
	non_uf_detect((const Mat&)inImage,roi,25,rgn,area);
	adhesiveResults.adhesiveBlack.connectionNum = (int)rgn.size();
	for(int i=0;i<(int)rgn.size();i++){
		adhesiveResults.adhesiveBlack.pRectangle[i].topLeftPoint.x = rgn[i].x;
		adhesiveResults.adhesiveBlack.pRectangle[i].topLeftPoint.y = rgn[i].y;
		adhesiveResults.adhesiveBlack.pRectangle[i].bottomRightPoint.x = rgn[i].x + rgn[i].width;
		adhesiveResults.adhesiveBlack.pRectangle[i].bottomRightPoint.y = rgn[i].y + rgn[i].height;
		adhesiveResults.adhesiveBlack.pArea[i] = area[i];
	}
#if 0
	// 方法一:直接调用 brightLine 
	//二： 利用直方图
	// Local iconic variables 
	Hobject  Image, ImageGrayGaus;
	Hobject  DarkRegion, minblackArea, blackBoundary, blackBoundary2;
	Hobject  Bigblack, innerWhite, innerWhiteImage, DarkRegion2;
	Hobject  blackRect, out_black, Black_Rect, innerWhiteImage2;
	Hobject  whiteRect, out_white, White_Rect;


	// Local control variables 
	HTuple  WindowID, AbsoluteHisto, RelativeHisto;
	HTuple  Width, Height, WindowHandle;
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
	if (rectInImage.channels() != 3){
		ret = -1; return ret;
	}


	//2.图像格式转换
	Image = MatToHImage(rectInImage);
	//decompose3(Image, &Red, &Green, &Blue);
	gauss_image(Image, &ImageGrayGaus, 3);
	/*直方图*/
	gray_histo(ImageGrayGaus, ImageGrayGaus, &AbsoluteHisto, &RelativeHisto);
	histo_to_thresh(AbsoluteHisto, 2, &MinThresh, &MaxThresh);
	if (0 != (HTuple(HTuple(MaxThresh[0])>70).And(HTuple(MinThresh[0])<40)))
	{
		threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), 40);
	}
	else
	{
		threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), HTuple(MaxThresh[0]));
	}
	threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]),42);
	opening_circle(DarkRegion, &DarkRegion, 10);
	reduce_domain(Image, DarkRegion, &minblackArea);
	boundary(minblackArea, &blackBoundary, "inner_filled");

	fill_up(blackBoundary, &blackBoundary2);
	reduce_domain(Image, blackBoundary2, &Bigblack);

	difference(Bigblack, minblackArea, &innerWhite);
	reduce_domain(Image, innerWhite, &innerWhiteImage);


	//*******************黑胶区域********************************
	connection(DarkRegion, &DarkRegion2);
//	select_shape(DarkRegion2, &DarkRegion2, "area", "and", 400, 999999999999);
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

	/*halcon图像显示*/

	//Halcon::get_image_size(Image, &Width, &Height);
	//if (HDevWindowStack::IsOpen())
	//	Halcon::close_window(HDevWindowStack::Pop());
	//Halcon::set_window_attr("background_color", "black");
	//Halcon::open_window(0, 0, Width , Height , 0, "", "", &WindowHandle);
	//HDevWindowStack::Push(WindowHandle);
	//
	//if (HDevWindowStack::IsOpen())
	//	set_color(HDevWindowStack::GetActive(), "red");
	//if (HDevWindowStack::IsOpen())
	//	Halcon::disp_obj(Image, HDevWindowStack::GetActive());
	//	Halcon::disp_obj(DarkRegion, HDevWindowStack::GetActive());

	//Sleep(3000);
	//Halcon::close_window(HDevWindowStack::Pop());
	
	/*opencv画圆*/
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


	//cv::imwrite("D://glue_bin.jpg", rectInImage);

#endif
#endif
	return 0;

}


