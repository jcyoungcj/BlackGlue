#include "getAdhersiveResult_thresh.h"
#include "getRectRegion.h"
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include "direct.h"
using namespace Halcon;
using namespace cv;
using namespace std;

/*****************************************************************************
*  @brief    : 黑胶区域检测结果返回
1.处理彩色区域，
2.处理转换后的黑胶区域
*****************************************************************************/
int getAdhersiveResult_thresh2(
	IN Mat& inImage,                              // 拼接后的大图片
	IN IVRectangle rectangle,                    // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,               // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults      // 输出黑胶检测结果
	);

#define CLU_NUM (4)
void cluster(const Mat img,Scalar cc[])
{
	vector<Point> vps;
	vector<Point> vs[CLU_NUM];
	Scalar ncc[CLU_NUM] = { 0 };
	for (int i = 0; i < img.rows;i++){
		for (int j = 0; j < img.cols;j++){
			vps.push_back(Point(j,i));
		}
	}
	int iter = 0;
	float eps = 0xFFFF;
	while(1){
		if (eps<0.1F || iter>8)break;
		for (int i = 0; i < CLU_NUM; i++) vs[i].clear();
		for (int i = 0; i < (int)vps.size();i++){
			int x = vps[i].x, y = vps[i].y;
			uchar r, g, b;
			b = img.data[3*y*img.cols + 3*x];
			g = img.data[3 * y*img.cols + 3 * x +1];
			r = img.data[3 * y*img.cols + 3 * x +2];
			int mind = 0xFFFF, ind = 0;
			for (int j = 0; j < CLU_NUM;j++){
				int d = fabs((cc[j][0] - b)) + fabs((cc[j][1] - b)) + fabs((cc[j][2] - b));
				if (d<mind)
				{
					mind = d; ind = j;
				}
			}
			vs[mind].push_back(Point(x,y));
		}
		for (int i = 0; i < CLU_NUM; i++){
			int _rc, _gc,_bc;
			_rc = _gc = _bc = 0;
			for (int j = 0; j < (int)vs[i].size();j++){
				int x = vs[i][j].x, y = vs[i][j].y;
				uchar r, g, b;
				b = img.data[3*y*img.cols + 3*x];
				g = img.data[3 * y*img.cols + 3 * x + 1];
				r = img.data[3 * y*img.cols + 3 * x + 2];
				_bc += b, _gc += g, _rc += r;
			}
			ncc[i][0] = (float)_bc / (vs[i].size()+0.0001F);
			ncc[i][1] = (float)_gc / (vs[i].size() + 0.0001F);
			ncc[i][2] = (float)_rc / (vs[i].size() + 0.0001F);
		}
		float dc = 0;
		for (int k = 0; k < CLU_NUM;++k){
			dc += (ncc[k][0] - cc[k][0])*(ncc[k][0] - cc[k][0]) +
				(ncc[k][1] - cc[k][1])*(ncc[k][1] - cc[k][1]) +
				(ncc[k][2] - cc[k][2])*(ncc[k][2] - cc[k][2]);
		}
		dc /= (3*CLU_NUM);
		eps = dc;
		for (int i = 0; i < CLU_NUM; i++) cc[i] = ncc[i];
		iter++;
	}

	
	return;
}
#define MIN_UF_PATCH_AREA (400)
static void uf_threshold(const Mat &src,Mat &dst,Mat mask,uchar thr)
{
	dst.zeros(dst.size(),CV_8UC1);
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols;j++)
		{
			uchar m = mask.data[i*src.cols+j];
			if (!m)continue;
			uchar v = src.data[i*src.cols + j];
			dst.data[i*src.cols + j] = v>thr? 255 : 0;
		}
	}
	return;
}
void uf_detect(const Mat& image,Rect roi,uchar thr,Mat& processed)
{
	vector<vector<Point>> vcs;
	Mat uf_gray;
	Mat uf_bin, uf_normal_bin;
	Mat uf_color = image(roi);
	Mat uf_color_draw = image(roi);
	cvtColor(uf_color,uf_gray,CV_RGB2GRAY);
	threshold(uf_gray,uf_bin,thr,255,THRESH_BINARY_INV);
	erode(uf_bin,uf_bin,getStructuringElement(MORPH_RECT,Size(11,11)));
	dilate(uf_bin, uf_bin, getStructuringElement(MORPH_RECT, Size(11, 11)));
	findContours(uf_bin, vcs, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	if (vcs.size()>0){
		std::sort(vcs.begin(), vcs.end(), [](const vector<Point>&left, const vector<Point>&right){
			return contourArea(left) > contourArea(right); });
			for (int i = 0; i < (int)vcs.size(); i++)
			{
				Rect uf = boundingRect(vcs[i]);
				if (uf.width * uf.height < MIN_UF_PATCH_AREA) { break; }
				Mat uf_part = uf_gray(uf), uf_part_bin;
				threshold(uf_part,uf_part_bin,thr+20,255,THRESH_BINARY);
				vector<vector<Point>> vcs_in;
				findContours(uf_part_bin, vcs_in, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
				if (vcs_in.size()>0){
					std::sort(vcs_in.begin(), vcs_in.end(), [](const vector<Point>&left, const vector<Point>&right){
						return contourArea(left) > contourArea(right); });
						for (int i = 0; i < (int)vcs_in.size(); i++)
						{
							Rect ri = boundingRect(vcs_in[i]);//-----------------需要确认
							if (ri.width*ri.height < MIN_UF_PATCH_AREA) continue;
							Scalar bc[4] = { Scalar(0, 0, 0), Scalar(217, 114, 34), Scalar(80, 130, 48), Scalar(32, 110, 211) };
							Scalar cc[4] = { Scalar(0, 0, 0), Scalar(217, 114, 34), Scalar(80, 130, 48), Scalar(32, 110, 211) };
							Mat bow = uf_color(Rect(uf.x + ri.x, uf.y + ri.y, ri.width, ri.height)).clone(); //-----------------需要确认
							cluster(bow, bc);
							float dc = 0;
							for (int k = 1; k < 4; k++)
							{
								dc += (bc[k][0] - cc[k][0])*(bc[k][0] - cc[k][0]) +
									(bc[k][1] - cc[k][1]) * (bc[k][1] - cc[k][1]) +
									(bc[k][2] - cc[k][2]) * (bc[k][2] - cc[k][2]);
							}
							dc /= (3 * CLU_NUM);
						//	if (dc < 1000)
						//	{
								for (int m = 0; m < vcs_in[i].size(); ++m) //---------------------
								{
									vcs_in[i][m].x += uf.x;
									vcs_in[i][m].y += uf.y;
								}
								drawContours(uf_color_draw, vcs_in, i, Scalar(15), CV_FILLED);							
							//}
						}
				}
			}
	}
	//----------------
	processed = uf_color_draw.clone();
 	return;
}
int getAdhersiveResult_thresh(
	IN Mat& inImage,                              // 拼接后的大图片
	IN IVRectangle rectangle,                    // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,               // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults      // 输出黑胶检测结果
	)
{
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
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID != 2 ||
		(rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x == 0) ||
		(rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y == 0) 
		){
		ret = -1; return ret;
	}
	//1. 获取矩形区域图像
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);
	if (rectInImage.channels() != 3){
		ret = -1; return ret;
	}
	vector<Rect> uf_region;
	vector<float> uf_area;
	Mat processed;
	uf_detect(rectInImage, Rect(0, 0, rectInImage.cols, rectInImage.rows), 50, processed);
	getAdhersiveResult_thresh2(processed, rectangle, adhesivePara, adhesiveResults);

	return 0;

}

int getAdhersiveResult_thresh2(
	IN Mat& inImage,                              // 拼接后的大图片
	IN IVRectangle rectangle,                    // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,               // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults      // 输出黑胶检测结果
	){
	int ret = 0;
	////矩形坐标校正
	//if (rectangle.topLeftPoint.x > rectangle.bottomRightPoint.x) {
	//	int tmp1 = rectangle.topLeftPoint.x;
	//	rectangle.topLeftPoint.x = rectangle.bottomRightPoint.x;
	//	rectangle.bottomRightPoint.x = tmp1;
	//}
	//if (rectangle.topLeftPoint.y > rectangle.bottomRightPoint.y) {
	//	int tmp1 = rectangle.topLeftPoint.y;
	//	rectangle.topLeftPoint.y = rectangle.bottomRightPoint.y;
	//	rectangle.bottomRightPoint.y = tmp1;
	//}
	////输入参数判断
	//if (inImage.empty() || rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
	//	rectangle.bottomRightPoint.x>inImage.cols ||
	//	rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID != 2 ||
	//	(rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x == 0) ||
	//	(rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y == 0)
	//	){
	//	ret = -1; return ret;
	//}

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
	//Mat rectInImage, rectInImage1;
	//getRectRegion(inImage, rectangle, rectInImage1);
	//rectInImage1.copyTo(rectInImage);
	//if (rectInImage.channels() != 3){
	//	ret = -1; return ret;
	//}


	//2.图像格式转换
	Image = MatToHImage(inImage);
	//decompose3(Image, &Red, &Green, &Blue);
	gauss_image(Image, &ImageGrayGaus, 3);
	/*直方图*/
	gray_histo(ImageGrayGaus, ImageGrayGaus, &AbsoluteHisto, &RelativeHisto);
	histo_to_thresh(AbsoluteHisto, 2, &MinThresh, &MaxThresh);
	//if (0 != (HTuple(HTuple(MaxThresh[0])>70).And(HTuple(MinThresh[0])<40)))
	//{
	//	threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), 40);
	//}
	//else
	//{
	//	threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), HTuple(MaxThresh[0]));
	//}
	threshold(ImageGrayGaus, &DarkRegion, HTuple(MinThresh[0]), 50);

	opening_circle(DarkRegion, &DarkRegion, 10);
	reduce_domain(Image, DarkRegion, &minblackArea);
	boundary(minblackArea, &blackBoundary, "inner_filled");

	fill_up(blackBoundary, &blackBoundary2);
	reduce_domain(Image, blackBoundary2, &Bigblack);

	difference(Bigblack, minblackArea, &innerWhite);
	reduce_domain(Image, innerWhite, &innerWhiteImage);


	//*******************黑胶区域********************************
	connection(blackBoundary2, &DarkRegion2);
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
	for (i = 1; i <= black_num&& i <= 100; i += 1)
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
	for (i = 1; i <= white_num&& i <= 100; i += 1)
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
		Rect r(cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveBlack.pRectangle[bb - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(inImage, r, Scalar(0, 255, 255), 1);
		bb--;
	}


	int ww = adhesiveResults.adhesiveWhite.connectionNum;
	while (ww){
		Rect r(cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].topLeftPoint.y - rectangle.topLeftPoint.y),
			cv::Point(adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.x - rectangle.topLeftPoint.x,
			adhesiveResults.adhesiveWhite.pRectangle[ww - 1].bottomRightPoint.y - rectangle.topLeftPoint.y));
		cv::rectangle(inImage, r, Scalar(0, 255, 0), 1);
		ww--;
	}


	//cv::imwrite("D://glue_bin.jpg", rectInImage);

#endif

	return 0;
}