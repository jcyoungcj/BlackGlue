// IVTestFrame.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "IVImageProcess.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <time.h>
#include<windows.h> 
using namespace std;
using namespace cv;

void testBlack(){
	clock_t start = clock();
	Mat ho_Image = imread("E:/������ʿ����·����/Բ��ͼ��/�ڽ�/3.png");
	clock_t read = clock();
	cout << "��ȡ�ڽ�ͼƬʱ�䣺" << (double)(read - start) << endl;
	//IVPoint point1 = { 844, 1200};
	//IVPoint point2 = { 1622, 1386 };
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { ho_Image.cols, ho_Image.rows };

	IVRectangle rectangle = { point1, point2 };
	AdhesiveResults adhesiveResults;
	//---------------------------�ڽ������----------------------------------//
	AdhesivePara adhesivePara1 = { 1, { 2, 42, 3.5 }, {} }; //thresh �н����
	AdhesivePara adhesivePara2 = { 2, {}, { 1.0, 3.5 } };//bina  �޽����
	int i = 10;
	//while (1)
//	{

		clock_t b = clock();
		getAdhesiveResults(ho_Image, rectangle, adhesivePara2, adhesiveResults);
		clock_t black = clock();
		cout << "�ڽ����ʱ�䣺" << (double)(black -b) << endl;
		//i--;
//	}
	
	clock_t black2 = clock();
	waitKey();
}

void test_DMDecode() {
	Mat img_ = imread("E:\\������ʿ����·����\\20000.bmp");
	IVRectangle rect{ IVPoint{ 1000, 900 }, IVPoint{ 1500, 1400 } };
	DMCodePara code_para{ 200 };
	char* out_str = new char[100];
	int str_length = 0;
	std::cout << "��ʼ" << clock() << endl;
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
	std::cout << "����" << clock() << endl;
	
	delete[] out_str;
	return;
}

//��������

void testBrightLine()
{
	BrightLinePara blp;

	//��һ���㷨
	blp.algorithmID = 1;
	blp.brightLinePara1.radius = 15;
	blp.brightLinePara1.thre = 40;

	BrightLineResults blr;
	string path = R"(E:/������ʿ����·����/Բ��ͼ��/�ڽ�/2.png)";
	Mat src = imread(path, 1);
	IVPoint point1 = { 0, 0 };
	IVPoint point2 = { src.cols, src.rows };

	IVRectangle rect = { point1, point2 };
	//IVRectangle rect = { 417, 1380, 417 + 233, 1380 + 83 };
	
	

	int ret = getBrightLineResults(src, rect, blp, blr);

	Mat tmp = src.clone();
	for (int i = 0; i < blr.arrSize; ++i)
	rectangle(tmp, blr.arrContours[i].rectBounding, Scalar(0, 255, 0));
	imwrite("brightline.bmp", tmp);
}
void mark(){

		//-------------------------------Բ�׶�λ----------------------------------//
		MarkPara makePara = {3.0,3.0,100,99999,20};
		IVPoint circleCentre;
		Mat ho_Image = imread("E:\\������ʿ����·����\\Բ��ͼ��\\Mark15153545902.bmp");
		IVPoint point1 = {0,0};
		IVPoint point2 = { ho_Image.cols, ho_Image.rows };
		IVRectangle rectangle = { point1, point2 };
	    clock_t read = clock();
		getMarkLocation(ho_Image, rectangle, makePara, circleCentre);
		clock_t mark = clock();
		Point p(circleCentre.x, circleCentre.y);
		circle(ho_Image, p, 1, Scalar(0, 255, 0));
		imshow("Բ�׶�λ",ho_Image);
		cout << "markʱ�䣺" << (double)(mark - read) << endl;
		waitKey();
}

int main()
{
	//---------------------------���߼��----------------------------------//

//	testBrightLine();


	//---------------------------�ڽ������----------------------------------//
	testBlack();

//	//-------------------------------Բ�׶�λ----------------------------------//
//	mark();
	//---------------------------------ͼ��Ŵ���-----------------------------------//
//	int magnification;
//	int mag = 3;				//��ֱ֪����΢��
//	getImageMagnification(ho_Image, mag, magnification);
//	clock_t magni = clock();
//	cout << "�Ŵ��ʼ��ʱ�䣺" << (double)(magni - mark) << endl;	
	//------------------------------------------��ά��----------------------------------------//
	//test_DMDecode();

	system("pause");
    return 0;
}

