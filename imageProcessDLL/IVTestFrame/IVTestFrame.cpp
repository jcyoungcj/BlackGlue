// IVTestFrame.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "IVImageProcess.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

void testBlack(){
	clock_t start = clock();
	Mat ho_Image = imread("E:/滨江富士康电路板检测/圆孔图像/黑胶/3.bmp");
	clock_t read = clock();
	cout << "读取黑胶图片时间：" << (double)(read - start) << endl;
	IVPoint point1 = { 844, 1200};
//	IVPoint point2 = { ho_Image.cols, ho_Image.rows };
	IVPoint point2 = { 1622, 1386 };
	IVRectangle rectangle = { point1, point2 };
	AdhesiveResults adhesiveResults;
	//---------------------------黑胶检测结果----------------------------------//
	AdhesivePara adhesivePara = { 1, { 22, 196, 3.5 }, {} };
//	getAdhesiveResults(ho_Image, rectangle, adhesivePara, adhesiveResults);
	clock_t black = clock();
	cout << "黑胶检测1时间：" << (double)(black - read) << endl;
	adhesivePara = { 2, {}, {1.0, 3.5} };
	getAdhesiveResults(ho_Image, rectangle, adhesivePara, adhesiveResults);
	clock_t black2 = clock();
	int b = adhesiveResults.adhesiveBlack.connectionNum;
	
	
	while (b){
		Rect r(adhesiveResults.adhesiveBlack.pRectangle[b - 1].topLeftPoint.x, adhesiveResults.adhesiveBlack.pRectangle[b - 1].topLeftPoint.y,
			adhesiveResults.adhesiveBlack.pRectangle[b - 1].bottomRightPoint.x, adhesiveResults.adhesiveBlack.pRectangle[b - 1].bottomRightPoint.y);
		cv::rectangle(ho_Image, r, Scalar(0, 255, 255), 1);
		b--;
	}

	int w = adhesiveResults.adhesiveWhite.connectionNum;
	while (w){
		Rect r(adhesiveResults.adhesiveWhite.pRectangle[w - 1].topLeftPoint.x, adhesiveResults.adhesiveWhite.pRectangle[w - 1].topLeftPoint.y,
			adhesiveResults.adhesiveWhite.pRectangle[w - 1].bottomRightPoint.x, adhesiveResults.adhesiveWhite.pRectangle[w - 1].bottomRightPoint.y);
		cv::rectangle(ho_Image, r, Scalar(0, 255, 0), 1);
		w--;
	}
	imshow("黑胶检测结果", ho_Image);
	cout << "黑胶检测2时间：" << (double)(black2 - black) << endl;
	waitKey();
}

void test_DMDecode() {
	Mat img_ = imread("E:\\滨江富士康电路板检测\\20000.bmp");
	IVRectangle rect{ IVPoint{ 1000, 900 }, IVPoint{ 1500, 1400 } };
	DMCodePara code_para{ 200 };
	char* out_str = new char[100];
	int str_length = 0;
	std::cout << "开始" << clock() << endl;
	int ret = DMDecode(img_, rect, code_para, out_str, str_length);

	if (ret == -1) {
		cout << "can't decode the ECC200" << endl;
	}
	else {
		cout << "the ECC200 is ";
		for (int i = 0; i < str_length; i++) {
			std::cout << out_str[i];
		}
	}
	std::cout << endl;
	std::cout << "结束" << clock() << endl;
	
	delete[] out_str;
	return;
}

//测试亮线

void testBrightLine()
{
	BrightLinePara blp;

	//第一种算法
	blp.algorithmID = 1;
	blp.brightLinePara1.radius = 15;
	blp.brightLinePara1.thre = 30;

	BrightLineResults blr;
	IVRectangle rect = { 417, 1380, 417 + 233, 1380 + 83 };
	string path = R"(C:\Users\jcyoung\Desktop\Image_20170919141411537.bmp)";
	Mat src = imread(path, 1);

	int ret = getBrightLineResults(src, rect, blp, blr);

	Mat tmp = src.clone();
	rectangle(tmp, blr.arrContours[0].rectBounding, Scalar(0, 255, 0));
	imwrite("brightline.bmp", tmp);
}
void mark(){

		//-------------------------------圆孔定位----------------------------------//
		MarkPara makePara = {2.0,2.4,50,99999,20};
		IVPoint circleCentre;
		Mat ho_Image = imread("C:/Users/jcyoung/Desktop/20171009/2.bmp");
	
		IVPoint point1 = {0,0};
		IVPoint point2 = { ho_Image.cols, ho_Image.rows };
		IVRectangle rectangle = { point1, point2 };
	    clock_t read = clock();
		getMarkLocation(ho_Image, rectangle, makePara, circleCentre);
		clock_t mark = clock();
		Point p(circleCentre.x, circleCentre.y);
		circle(ho_Image, p, 0, Scalar(0, 255, 0));
		imshow("圆孔定位",ho_Image);
		cout << "mark时间：" << (double)(mark - read) << endl;
		waitKey();
}

int main()
{
	//---------------------------亮线检测----------------------------------//
	testBrightLine();
//	//HObject  ho_Image;
//	clock_t start = clock();
//	Mat ho_Image = imread("C:/Users/jcyoung/Desktop/20171009/2.bmp");
//	clock_t read = clock();
//	cout << "读取圆孔图片时间：" << (double)(read-start)<< endl;	
//	IVPoint point1 = {0,0};
//	IVPoint point2 = { ho_Image.cols, ho_Image.rows };
//	IVRectangle rectangle = { point1, point2 };
//	AdhesiveResults adhesiveResults;
//
	//---------------------------黑胶检测结果----------------------------------//
	testBlack();

//	//-------------------------------圆孔定位----------------------------------//
	mark();
	//---------------------------------图像放大率-----------------------------------//
//	int magnification;
//	int mag = 3;				//已知直径，微米
//	getImageMagnification(ho_Image, mag, magnification);
//	clock_t magni = clock();
//	cout << "放大率检测时间：" << (double)(magni - mark) << endl;	
	//------------------------------------------二维码----------------------------------------//
	test_DMDecode();

	system("pause");
    return 0;
}

