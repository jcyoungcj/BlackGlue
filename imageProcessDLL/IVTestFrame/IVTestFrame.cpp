// IVTestFrame.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "IVImageProcess.h"

#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

void testBlack(){
	clock_t start = clock();
	Mat ho_Image = imread("E:/������ʿ����·����/Բ��ͼ��/�ڽ�/3.bmp");
	clock_t read = clock();
	cout << "��ȡ�ڽ�ͼƬʱ�䣺" << (double)(read - start) << endl;
	IVPoint point1 = { 844, 1200};
//	IVPoint point2 = { ho_Image.cols, ho_Image.rows };
	IVPoint point2 = { 1622, 1386 };
	IVRectangle rectangle = { point1, point2 };
	AdhesiveResults adhesiveResults;
	//---------------------------�ڽ������----------------------------------//
	AdhesivePara adhesivePara = { 1, { 22, 196, 3.5 }, {} };
//	getAdhesiveResults(ho_Image, rectangle, adhesivePara, adhesiveResults);
	clock_t black = clock();
	cout << "�ڽ����1ʱ�䣺" << (double)(black - read) << endl;
	adhesivePara = { 2, {}, {1.0, 3.5} };
	getAdhesiveResults(ho_Image, rectangle, adhesivePara, adhesiveResults);
	clock_t black2 = clock();
	cout << "�ڽ����2ʱ�䣺" << (double)(black2 - black) << endl;
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
#include <opencv2\opencv.hpp>
void testBrightLine()
{
	BrightLinePara blp;

	//��һ���㷨
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


int main()
{
	testBrightLine();
//	//HObject  ho_Image;
//	clock_t start = clock();
//	Mat ho_Image = imread("C:/Users/jcyoung/Desktop/20171009/2.bmp");
//	clock_t read = clock();
//	cout << "��ȡԲ��ͼƬʱ�䣺" << (double)(read-start)<< endl;	
//	IVPoint point1 = {0,0};
//	IVPoint point2 = { ho_Image.cols, ho_Image.rows };
//	IVRectangle rectangle = { point1, point2 };
//	AdhesiveResults adhesiveResults;
//
//	//---------------------------�ڽ������----------------------------------//
	testBlack();
//	clock_t black = clock();
//	//-------------------------------Բ�׶�λ----------------------------------//
//	MarkPara makePara = {2.0,2.4,50,99999,20};
//	IVPoint circleCentre;
//	getMarkLocation(ho_Image, rectangle, makePara, circleCentre);
//	clock_t mark = clock();
//	cout << "markʱ�䣺" << (double)(mark - black) << endl;
//	//---------------------------------ͼ��Ŵ���-----------------------------------//
//	int magnification;
//	int mag = 3;				//��ֱ֪����΢��
//	getImageMagnification(ho_Image, mag, magnification);
//	clock_t magni = clock();
//	cout << "�Ŵ��ʼ��ʱ�䣺" << (double)(magni - mark) << endl;	
//	//--------------------------------//

//	test_DMDecode();
	system("pause");
    return 0;
}
