// Magnification.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"


// MagnificationDLL.cpp : Defines the exported functions for the DLL application.
//
#include "IVMagnificationDLL.h"
#include "Halconcpp.h"
#include "ImgTransform.h"


//using namespace std;
using namespace Halcon;

// ͼ��Ŵ��ʼ���
/*****************************************************************************
*  @brief    : ����ϵͳ��ͼ��Ŵ���
*****************************************************************************/
extern"C" __declspec(dllexport)  int getImageMagnification1(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ���ף�
	IN MagPara1 magPara1,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                       // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	)
{
	if (inImage.empty() || diameter == 0 || magPara1.alpha == NULL || magPara1.alpha <1||
		magPara1.maxLackGirth == NULL || magPara1.maxLackGirth <1 ||
		magPara1.minGirth == NULL || magPara1.minGirth<0 ||
		magPara1.sigma == NULL || magPara1.sigma <1	
		){ return -1; }

	// Local iconic variables 
	Hobject  Image, Edges, SelectedContours, UnionContours;
	// Local control variables 
	HTuple  NumberCircles, Row, Column, Radius, StartPhi;
	HTuple  EndPhi, PointOrder, diameter_h, MaxRadius, i, Mindiameter, MinRadius;
	//��ʽת��
	Image = MatToHImage(inImage);

	rgb1_to_gray(Image, &Image);
//	derivate_gauss(Image, &Image, 2.4, "none");
	derivate_gauss(Image, &Image, magPara1.sigma, "none");
//	edges_sub_pix(Image, &Edges, "canny", 2, 20, 40);
	edges_sub_pix(Image, &Edges, "canny", magPara1.alpha, 20, 40);
	//�������²���ɸѡ���� ���ȣ��������ʣ���գ�������������
//	select_contours_xld(Edges, &SelectedContours, "contour_length", 50, 999999999, -0.5, 0.5);
	select_contours_xld(Edges, &SelectedContours, "contour_length", magPara1.minGirth, 999999999, -0.5, 0.5);
//	select_contours_xld(SelectedContours, &SelectedContours, "closed", 0, 20, 0, 0);
	select_contours_xld(SelectedContours, &SelectedContours, "closed", 0, magPara1.maxLackGirth, 0, 0);
	//�ϲ��ص�����,��Ҫ�ѹ��ߵ������ϲ���һ��
	union_cocircular_contours_xld(SelectedContours, &UnionContours, 1.6, 0.9, 0.9, 30, 40, 40, "true", 1);
	//����Բ�ĸ���
	count_obj(UnionContours, &NumberCircles);
	//��ȡԲ������  �Ե���������Բ�ƽ���fit_eclipse �����Բ
	fit_circle_contour_xld(SelectedContours, "algebraic", -1, 0, 0, 10, 2, &Row, &Column, &Radius, &StartPhi, &EndPhi, &PointOrder);
	//�����λ����
	if (0 != (NumberCircles<1))
	{
		diameter_h = 0;
	}
	else
	{
		tuple_max(Radius, &MaxRadius);
		tuple_min(Radius, &MinRadius);
		for (i = 0; i <= NumberCircles - 1; i += 1)
		{
			if (0 != (HTuple(Radius[i]) == MaxRadius))
			{
				diameter_h = 2 * MaxRadius[0].D();
				Mindiameter = 2 * MinRadius;
			}
		}
	}

	double d = Mindiameter[0].D();
	magnification = (int)d / (diameter*0.001);
	if (magnification == 0){
		return 0;
		magnification = 0;							//�Ŵ��ʳ�ʼ��ֵ����Ҫ�޸�
	}
	//����Ĭ��ֵ����ֹû�м���Բ�����
	return 0;
}
extern"C" __declspec(dllexport)  int getImageMagnification2(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ���ף�
	IN MagPara2 magPara2,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                       // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	)
{
	if (inImage.empty() || diameter == 0 || magPara2.maxThresh == 0 || magPara2.maxThresh <1 ||
		magPara2.minArea == 0 || magPara2.minArea <1 ||
		magPara2.minThresh<0 || magPara2.maxThresh >255 ||
		magPara2.Openradius == 0 || magPara2.Openradius <1 ||
		magPara2.sigma == 0 || magPara2.sigma <1
		){
		return -1;
	}

	// Local iconic variables 
	Hobject  Image, Region, RegionOpening, ConnectedRegions;
	Hobject  SelectedRegions, Contours;

	// Local control variables 
	HTuple  area, row, column, NumberCircles;
	HTuple  Row, Column, Radius, StartPhi, EndPhi, PointOrder;
	HTuple  Maxdiameter, Mindiameter, MaxRadius, MinRadius;
	HTuple  i;

	//��ʽת��
	Image = MatToHImage(inImage);
	derivate_gauss(Image, &Image, magPara2.sigma, "none");

	threshold(Image, &Region, magPara2.minThresh, magPara2.maxThresh);
//	opening_circle(Region, &RegionOpening, 6);
	opening_circle(Region, &RegionOpening, magPara2.Openradius);
	connection(RegionOpening, &ConnectedRegions);
//	select_shape(ConnectedRegions, &SelectedRegions, "area", "and", 500, 3000000);
	select_shape(ConnectedRegions, &SelectedRegions, "area", "and", magPara2.minArea, 300000000);
	gen_contour_region_xld(SelectedRegions, &Contours, "border");
	area_center(SelectedRegions, &area, &row, &column);

	count_obj(Contours, &NumberCircles);
	fit_circle_contour_xld(Contours, "algebraic", -1, 0, 0, 10, 2, &Row, &Column, &Radius,
		&StartPhi, &EndPhi, &PointOrder);

	if (0 != (NumberCircles<1))
	{
		Maxdiameter = 0;
		Mindiameter = 0;
	}
	else
	{
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
	magnification = (int)d / (diameter*0.001);
	if (magnification == 0){
		return 0;
	}

}
extern"C" __declspec(dllexport)  int getImageMagnification3(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ���ף�
	IN MagPara magPara,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                      // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	){
	if (inImage.empty() || diameter == 0 ){
		return -1;
	}

	// Local iconic variables 
	Hobject  Image, Region, RegionOpening, ConnectedRegions;
	Hobject  SelectedRegions, Contours;

	// Local control variables 
	HTuple  area, row, column, NumberCircles;
	HTuple  Row, Column, Radius, StartPhi, EndPhi, PointOrder;
	HTuple  Maxdiameter, Mindiameter, MaxRadius, MinRadius;
	HTuple  i;

	//��ʽת��
	Image = MatToHImage(inImage);
	derivate_gauss(Image, &Image, 2.5, "none");

	//threshold(Image, &Region, magPara2.minThresh, magPara2.maxThresh);
	bin_threshold(Image, &Region);
	//	opening_circle(Region, &RegionOpening, 6);
	opening_circle(Region, &RegionOpening, 6);
	connection(RegionOpening, &ConnectedRegions);
	select_shape(ConnectedRegions, &SelectedRegions, "area", "and", 300, 300000000);
	gen_contour_region_xld(SelectedRegions, &Contours, "border");
	area_center(SelectedRegions, &area, &row, &column);

	count_obj(Contours, &NumberCircles);
	fit_circle_contour_xld(Contours, "algebraic", -1, 0, 0, 10, 2, &Row, &Column, &Radius,
		&StartPhi, &EndPhi, &PointOrder);

	if (0 != (NumberCircles<1))
	{
		Maxdiameter = 0;
		Mindiameter = 0;
	}
	else
	{
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
	magnification = (int)d / (diameter*0.001);
	if (magnification == 0){
		return 0;
	}

}

extern"C" __declspec(dllexport)  int getImageMagnification(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,							   //��֪Բֱ�� ���ף�
	IN MagPara magPara,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                       // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	)
{
	if (inImage.empty() || diameter == 0){ return -1; }
	//���
	switch (magPara.algorithmID)
	{
	case 1:
		getImageMagnification1(inImage, diameter, magPara.magPara1, magnification);
		break;
	case 2:
		getImageMagnification2(inImage, diameter, magPara.magPara2, magnification);
		break;
	case 3 :
		getImageMagnification3(inImage, diameter, magPara, magnification);
	default:
		break;
	}
}