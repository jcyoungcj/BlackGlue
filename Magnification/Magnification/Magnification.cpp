// Magnification.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"


// MagnificationDLL.cpp : Defines the exported functions for the DLL application.
//
#include "IVMagnificationDLL.h"
#include "Halconcpp.h"
#include "ImgTransform.h"
#include "opencv2/opencv.hpp"

//using namespace std;
using namespace Halcon;

// ͼ��Ŵ��ʼ���
/*****************************************************************************
*  @brief    : ����ϵͳ��ͼ��Ŵ���
*****************************************************************************/

extern"C" __declspec(dllexport)  int getImageMagnification1(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ��΢�ף�
	IN MagPara1 magPara1,						  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                      // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	)
{
	if (inImage.empty() || diameter == 0 || magPara1.maxThresh == 0 || magPara1.maxThresh <1 ||
		magPara1.minArea == 0 || magPara1.minArea <1 ||
		magPara1.minThresh<0 || magPara1.maxThresh >255 ||
		magPara1.Openradius == 0 || magPara1.Openradius <1 ||
		magPara1.sigma == 0 || magPara1.sigma <1
		){
		return -1;
	}

	// Local iconic variables 
	Hobject  Image, Region, RegionOpening, ConnectedRegions;
	Hobject  SelectedRegions, Contours,  ContCircle;

	// Local control variables 
	HTuple  area, row, column, NumberCircles;
	HTuple  Row, Column, Radius, StartPhi, EndPhi, PointOrder;
	HTuple  Maxdiameter, Mindiameter, MaxRadius, MinRadius;
	HTuple  i;
	HTuple  Width, Height, WindowHandle, WindowID;

	//��ʽת��
	Image = MatToHImage(inImage);
	derivate_gauss(Image, &Image, magPara1.sigma, "none");

//	threshold(Image, &Region, magPara1.minThresh, magPara1.maxThresh);
	threshold(Image, &Region, 5,50);
//	opening_circle(Region, &RegionOpening, 1);
	opening_circle(Region, &RegionOpening, magPara1.Openradius);
	connection(RegionOpening, &ConnectedRegions);
	select_shape(ConnectedRegions, &ConnectedRegions, "circularity", "and", 0.70, 1);
	select_shape(ConnectedRegions, &SelectedRegions, "area", "and", area.Max(), (area.Max()) + 999999999);
	area_center(ConnectedRegions, &area, &row, &column);
	gen_contour_region_xld(SelectedRegions, &Contours, "border");
	/*area_center(SelectedRegions, &area, &row, &column);*/
	select_contours_xld(Contours, &Contours, "contour_length", 100, 9999999, -0.5, 0.5);

	count_obj(Contours, &NumberCircles);
	// geotukey 'algebraic', 'ahuber', 'atukey', 'geometric', 'geohuber', 'geotukey' 

	if (0 != (NumberCircles<1))
	{
		Maxdiameter = 0;
		Mindiameter = 0;
	}
	else
	{
		fit_circle_contour_xld(Contours, "geotukey", -1, 0, 0, 10, 2, &Row, &Column, &Radius,&StartPhi, &EndPhi, &PointOrder);
		gen_circle_contour_xld(&ContCircle, Row, Column, Radius, 0, HTuple(360).Rad(),"positive", 1.0);
		tuple_max(Radius, &MaxRadius);
		tuple_min(Radius, &MinRadius);
		for (i = 0; i <= NumberCircles - 1; i += 1)
		{
			if (0 != (HTuple(Radius[i]) == MaxRadius))
			{
				Maxdiameter = 2 * MaxRadius;
				Mindiameter = 2 * MinRadius;
			}
		}
	}
	double d = Mindiameter[0].D();
	magnification = (int)d / (diameter*0.000001);
#ifdef _DEBUG
	/*halconͼ����ʾ*/
	//get_image_size(Image, &Width, &Height);
	//if (HDevWindowStack::IsOpen())
	//	close_window(HDevWindowStack::Pop());
	//set_window_attr("background_color", "black");
	//open_window(0, 0, Width/10, Height/10, 0, "", "", &WindowHandle);
	//HDevWindowStack::Push(WindowHandle);

	//if (HDevWindowStack::IsOpen())
	//	set_color(HDevWindowStack::GetActive(), "red");
	//if (HDevWindowStack::IsOpen())
	//	disp_obj(Image, HDevWindowStack::GetActive());
	//	disp_obj(ContCircle, HDevWindowStack::GetActive());
	//Sleep(2000);
	//close_window(HDevWindowStack::Pop());

	/*opencv��Բ*/
	cv::circle(inImage, cvPoint(Column[0].I(), Row[0].I()), d / 2, cv::Scalar(0, 0, 255), 10);
	//cv::namedWindow("circle", CV_WINDOW_NORMAL);
	//cv::resizeWindow("circle", 200, 200);
	//cv::imshow("circle", inImage);
	//Sleep(3000);
	cv::imwrite("D://circle.bmp", inImage);

#endif

	if (magnification == 0){
		return 0;
	}

}
extern"C" __declspec(dllexport)  int getImageMagnification2(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ��΢�ף�
	IN MagPara magPara,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                      // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	){
	if (inImage.empty() || diameter == 0 ){
		return -1;
	}

	// Local iconic variables 
	Hobject  Image, Region, RegionOpening, ConnectedRegions;
	Hobject  SelectedRegions, Contours, ContCircle;

	// Local control variables 
	HTuple  area, row, column, NumberCircles;
	HTuple  Row, Column, Radius, StartPhi, EndPhi, PointOrder;
	HTuple  Maxdiameter, Mindiameter, MaxRadius, MinRadius;
	HTuple  i;
	HTuple  Width, Height, WindowHandle, WindowID;
	//��ʽת��
	Image = MatToHImage(inImage);
	derivate_gauss(Image, &Image, 2.5, "none");

	//threshold(Image, &Region, magPara2.minThresh, magPara2.maxThresh);
	bin_threshold(Image, &Region);
	//	opening_circle(Region, &RegionOpening, 6);
	opening_circle(Region, &RegionOpening, 6);
	connection(RegionOpening, &ConnectedRegions);
	area_center(ConnectedRegions, &area, &row, &column);
	select_shape(ConnectedRegions, &SelectedRegions, "area", "and", area.Max(), (area.Max()) + 999999999);
	//select_shape(ConnectedRegions, &SelectedRegions, "area", "and", 300, 300000000);
	gen_contour_region_xld(SelectedRegions, &Contours, "border");
	area_center(SelectedRegions, &area, &row, &column);

	count_obj(Contours, &NumberCircles);

	if (0 != (NumberCircles<1))
	{
		Maxdiameter = 0;
		Mindiameter = 0;
	}
	else
	{
		fit_circle_contour_xld(Contours, "geotukey", -1, 0, 0, 10, 2, &Row, &Column, &Radius,&StartPhi, &EndPhi, &PointOrder);
		gen_circle_contour_xld(&ContCircle, Row, Column, Radius, 0, HTuple(360).Rad(),"positive", 1.0);
		tuple_max(Radius, &MaxRadius);
		tuple_min(Radius, &MinRadius);
		for (i = 0; i <= NumberCircles - 1; i += 1)
		{
			if (0 != (HTuple(Radius[i]) == MaxRadius))
			{
				Maxdiameter = 2 * MaxRadius;
				Mindiameter = 2 * MinRadius;
			}
		}
	}
	double d = Mindiameter[0].D();
	int dd = Mindiameter[0].I();
	magnification = (int)d / (diameter*0.000001);
#ifdef _DEBUG
	/*halconͼ����ʾ*/
	//get_image_size(Image, &Width, &Height);
	//if (HDevWindowStack::IsOpen())
	//	close_window(HDevWindowStack::Pop());
	//set_window_attr("background_color", "black");
	//open_window(0, 0, Width / 10, Height / 10, 0, "", "", &WindowHandle);
	//HDevWindowStack::Push(WindowHandle);

	//if (HDevWindowStack::IsOpen())
	//	set_color(HDevWindowStack::GetActive(), "red");
	//if (HDevWindowStack::IsOpen())
	//	disp_obj(Image, HDevWindowStack::GetActive());
	//disp_obj(ContCircle, HDevWindowStack::GetActive());
	//Sleep(1000);
	//close_window(HDevWindowStack::Pop());

	/*opencv��Բ*/
	cv::circle(inImage, cvPoint(Column[0].I(), Row[0].I()), d / 2, cv::Scalar(0, 0, 255), 10);
	//cv::namedWindow("circle", CV_WINDOW_NORMAL);
	//cv::resizeWindow("circle", 200, 200);
	//cv::imshow("circle", inImage);
	//Sleep(3000);
	cv::imwrite("D://circle.bmp", inImage);

#endif

	if (magnification == 0){
		return 0;
	}
}


extern"C" __declspec(dllexport)  int getImageMagnification(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ��΢�ף�
	IN MagPara magPara,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                      // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	)
{
	if (inImage.empty() || diameter == 0){ return -1; }
	//���
	switch (magPara.algorithmID)
	{
	case 1:
		getImageMagnification1(inImage, diameter, magPara.magPara1, magnification);//ɸѡԲ����ͨ��
		break;
	case 2:
		getImageMagnification2(inImage, diameter, magPara, magnification); //����Ӧ
		break;
	default:
		break;
	}
}