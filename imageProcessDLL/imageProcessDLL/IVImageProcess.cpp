#include "IVImageProcess.h"
#include "getRectRegion.h"
#include "ImgTransform.h"
#include "BrightLineHelper.h"
#include "getAdhersiveResult_thresh.h"
#include "getAdhersiveResult_binary.h"


using namespace cv;
using namespace std;
using namespace Halcon;

//// ͼ��Ŵ��ʼ���


// Բ��λ�÷���
/*****************************************************************************
*  @brief    : Բ��λ�÷���
*****************************************************************************/
extern"C" __declspec(dllexport)  int getMarkLocation(
	IN Mat& inImage,								// ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,                      // ����ľ�������
	IN MarkPara makePara,					      // Բ�ף�mark������Ӧ���������
	OUT IVPoint & circleCentre		             // ����Բ������
	)
{
	int ret = 0;
	//��������У��
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
	//�����ж�
	if (inImage.empty() || rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
		rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y>inImage.rows
		){
		ret = -1; return ret;
	}

	// Local iconic variables
	Hobject  ho_Image, ho_Edges, ho_SelectedContours;
	Hobject  ho_UnionContours;
	// Local control variables
	HTuple  hv_NumberCircles;
	HTuple  hv_Row, hv_Column, hv_Radius, hv_StartPhi, hv_EndPhi;
	HTuple  hv_PointOrder, hv_MaxRadius, hv_i;
	HTuple  hv_outRaw, hv_outColumn;

	//1. ��ȡ��������ͼ��
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);


	//�ı�����

	//for (int y = 0; y < rectInImage.rows;y++){
	//	for (int x = 0; x < rectInImage.cols;x++){
	//		rectInImage.at<Vec3b>(y,x)[3] = 0;
	//	}
	//
	//}

	//2.ͼ���ʽת��
	ho_Image = MatToHImage(rectInImage);
	rgb1_to_gray(ho_Image, &ho_Image);
	derivate_gauss(ho_Image, &ho_Image, makePara.sigma, "none");
	edges_sub_pix(ho_Image, &ho_Edges, "canny", makePara.alpha, 20, 40);
	//alpha:����ָ��ֵԽС,ƽ��Խǿ��,����ٱ�Եϸ��,cannay �෴0.9

	//�������²���ɸѡ���� ���ȣ��������ʣ���գ�������������
	select_contours_xld(ho_Edges, &ho_SelectedContours, "contour_length", makePara.minGirth, makePara.maxGirth, -0.5, 0.5);
	select_contours_xld(ho_SelectedContours, &ho_SelectedContours, "closed", 0, makePara.maxLackGirth, 0, 0);

	//�ϲ��ص�����,��Ҫ�ѹ��ߵ������ϲ���һ��
	union_cocircular_contours_xld(ho_SelectedContours, &ho_UnionContours, 1.6, 0.9, 0.9, 30, 40, 40, "true", 1);

	//����Բ�ĸ���
	count_obj(ho_UnionContours, &hv_NumberCircles);

	//��ȡԲ������  �Ե���������Բ�ƽ���fit_eclipse �����Բ
	fit_circle_contour_xld(ho_SelectedContours, "algebraic", -1, 0, 0, 10, 2, &hv_Row, &hv_Column, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	//�����λ����
	if (0 != (hv_NumberCircles<1))
	{
		//hv_ResultText = "δ��⵽Բ������������";
		circleCentre.x = 0;
		circleCentre.y = 0;
		ret = -1;
	}
	else
	{
		tuple_max(hv_Radius, &hv_MaxRadius);
		//HTuple end_val58 = hv_NumberCircles;
		//HTuple step_val58 = 1;
		for (hv_i = 0; hv_i <= hv_NumberCircles - 1; hv_i += 1)
		{
			if (0 != (HTuple(hv_Radius[hv_i]) == hv_MaxRadius))
			{
				hv_outRaw = hv_Row[hv_i];
				circleCentre.y = hv_outRaw[0].D() + rectangle.topLeftPoint.y;
				hv_outColumn = hv_Column[hv_i];
				circleCentre.x = hv_outColumn[0].D() + rectangle.topLeftPoint.x;
			}
		}
	}
#ifdef _DEBUG
	Point p(circleCentre.x, circleCentre.y);
	circle(rectInImage, p, 0, Scalar(0, 255, 0));
	//imshow("Բ�׶�λ", rectInImage);
#endif

	return ret;
}

// ͼ��У��
/*****************************************************************************
*  @brief    : ͼ��У��
*****************************************************************************/
extern"C" __declspec(dllexport)  int affineImage(
	INOUT Mat & inImage,						      // Բ�ף�mark��ͼƬ
	IN AffinePara affinePara						 // ����任�Ĳ�������
	)
{
	//check the param
	if (inImage.empty()) {
		return -1;
	}
	if ((affinePara.originPoint2.y - affinePara.originPoint1.y) *
		(affinePara.originPoint3.x - affinePara.originPoint1.x) ==
		(affinePara.originPoint3.y - affinePara.originPoint1.y)*
		(affinePara.originPoint2.x - affinePara.originPoint1.x)) {
		return -1;
	}
	if ((affinePara.newPoint2.y - affinePara.newPoint1.y) *
		(affinePara.newPoint3.x - affinePara.newPoint1.x) ==
		(affinePara.newPoint3.y - affinePara.newPoint1.y)*
		(affinePara.newPoint2.x - affinePara.newPoint1.x)) {
		return -1;
	}
	// Local iconic variables
	Hobject  ho_Image, ho_ImageAffinTrans;
	// Local control variables
	HTuple  hv_RowSource, hv_ColumnSource;
	HTuple  hv_RowTarget, hv_ColumnTarget, hv_HomMat2D;
	//Input affine transformation
	hv_RowSource[0] = affinePara.originPoint1.y;
	hv_RowSource[1] = affinePara.originPoint2.y;
	hv_RowSource[2] = affinePara.originPoint3.y;
	hv_ColumnSource[0] = affinePara.originPoint1.x;
	hv_ColumnSource[1] = affinePara.originPoint2.x;
	hv_ColumnSource[2] = affinePara.originPoint3.x;
	hv_RowTarget[0] = affinePara.newPoint1.y;
	hv_RowTarget[1] = affinePara.newPoint2.y;
	hv_RowTarget[2] = affinePara.newPoint3.y;
	hv_ColumnTarget[0] = affinePara.newPoint1.x;
	hv_ColumnTarget[1] = affinePara.newPoint2.x;
	hv_ColumnTarget[2] = affinePara.newPoint3.x;
	ho_Image = MatToHImage(inImage);
	//Determine affine transformation from input points
	vector_to_hom_mat2d(hv_RowSource, hv_ColumnSource, hv_RowTarget,
		hv_ColumnTarget, &hv_HomMat2D);
	affine_trans_image(ho_Image, &ho_ImageAffinTrans, hv_HomMat2D, "constant",
		"true");
	inImage = HImageToMat(ho_ImageAffinTrans);
	return 0;
}

// �ڽ����������
/*****************************************************************************
*  @brief    : �ڽ������������
*****************************************************************************/
extern"C" __declspec(dllexport)  int getAdhesiveResults(
	IN Mat& inImage,                               // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,                     // ���кڽ��ľ�������
	IN AdhesivePara adhesivePara,                // �ڽ�����Ӧ���������
	OUT AdhesiveResults & adhesiveResults       // ����ڽ���ͼƬ�еı���
	)
{
	int ret = 0;
	//��������У��
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
	//��������ж�
	if (inImage.empty() || rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
		rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID<1
		){
		ret = -1; return ret;
	}

	//���
	switch (adhesivePara.algorithmID)
	{
	case 1:
		getAdhersiveResult_thresh(inImage, rectangle, adhesivePara, adhesiveResults);
		break;
	case 2:
		getAdhersiveResult_binary(inImage, rectangle, adhesivePara, adhesiveResults);
		break;
	default:
		break;
	}

	return 0;
}

// ���߼��
/*****************************************************************************
*  @brief    : ���߼����
*****************************************************************************/
extern"C" __declspec(dllexport)  int getBrightLineResults(
	IN cv::Mat& inImage,											// ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,							           // ���кڽ��ľ�������
	IN BrightLinePara brightLinePara,				              // �ڽ�����Ӧ���������
	OUT BrightLineResults & brightLineResults                    // ����ڽ���ͼƬ�еı���
	)
{
	int ret = 0;
	switch (brightLinePara.algorithmID)
	{
	case 1:
		ret = DetectBrightLine_1(inImage, rectangle, brightLinePara.brightLinePara1, brightLineResults);
		break;
	case 2:
		ret = DetectBrightLine_2(inImage, rectangle, brightLinePara.brightLinePara2, brightLineResults);
		break;
	default:
		break;
	}

	return ret;
}

// DM��ά��ʶ��
/*****************************************************************************
*  @brief    : DM��ά��ʶ��
*****************************************************************************/
extern"C" __declspec(dllexport)  int DMDecode(
	IN Mat& inImage,								  // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,                        // ���ж�ά��ľ�������
	IN DMCodePara dmCodePara,                       // DM��ά����������
	OUT char * outImage,                          // �ڴ����㷨���룬��λ�����ͷ�
	OUT int & charCount                           // �ַ�����
	)
{
	if (inImage.empty() || dmCodePara.time_out <= 0 ||
		rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.x>inImage.cols ||
		rectangle.topLeftPoint.y<0 || rectangle.topLeftPoint.y>inImage.rows ||
		rectangle.bottomRightPoint.x<0 || rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y<0 || rectangle.bottomRightPoint.y>inImage.cols ||
		outImage == NULL)
		return -1;
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
	Mat img_crop;
	getRectRegion(inImage, rectangle, img_crop);
	Hobject  ho_Image = MatToHImage(img_crop);
	Hobject ho_SymbolXLDs;
	// Local control variables
	HTuple  hv_DataCodeHandle, hv_ResultHandles;
	HTuple  hv_DecodedDataStrings;
	//creat the 2d data code model
	create_data_code_2d_model("Data Matrix ECC 200", HTuple(), HTuple(), &hv_DataCodeHandle);
	set_data_code_2d_param(hv_DataCodeHandle, "timeout", dmCodePara.time_out);
	//set_data_code_2d_param(hv_DataCodeHandle, "",)
	find_data_code_2d(ho_Image, &ho_SymbolXLDs, hv_DataCodeHandle, HTuple(), HTuple(),
		&hv_ResultHandles, &hv_DecodedDataStrings);
	//If no data code could be found
	if (hv_DecodedDataStrings.Num() == 0)	{
		outImage = NULL;
		charCount = 0;
		return -1;
	}
	HTuple str_len;
	tuple_strlen(hv_DecodedDataStrings, &str_len);
	charCount = str_len[0].I();

	//outImage = new char[charCount+1];
	//memset(outImage, 0, charCount+1);
	memcpy(outImage, hv_DecodedDataStrings[0].S(), charCount);
	//Clear the 2d data code model
	clear_data_code_2d_model(hv_DataCodeHandle);
	return 0;
}