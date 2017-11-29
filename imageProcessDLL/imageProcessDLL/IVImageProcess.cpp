#include "IVImageProcess.h"
#include "getRectRegion.h"
#include "ImgTransform.h"
#include "BrightLineHelper.h"
#include "getAdhersiveResult_thresh.h"
#include "getAdhersiveResult_binary.h"


using namespace cv;
using namespace std;
using namespace Halcon;

// 图像放大率计算


// 圆孔位置返回
/*****************************************************************************
*  @brief    : 圆孔位置返回
*****************************************************************************/
extern"C" __declspec(dllexport)  int getMarkLocation(
	IN Mat& inImage,								// 拼接后的大图片
	IN IVRectangle rectangle,                      // 输入的矩形区域
	IN MarkPara makePara,					      // 圆孔（mark）检测对应的输入参数
	OUT IVPoint & circleCentre		             // 返回圆心坐标
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
	//参数判断
	if (inImage.empty() || rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.y<0 ||
		rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y>inImage.rows ||
		(rectangle.bottomRightPoint.x - rectangle.topLeftPoint.x == 0) ||
		(rectangle.bottomRightPoint.y - rectangle.topLeftPoint.y == 0)
		){
		ret = -1; return ret;
	}
	// Local iconic variables 
	Hobject  Image, ImageGray, ImageThresh, RegionOpening;
	Hobject  ConnectedRegions, Contours, ContCircle;


	// Local control variables 
	HTuple  Width, Height, area, row, column, NumberCircles;
	HTuple  outRaw, outColumn, Row, Column, Radius, StartPhi;
	HTuple  EndPhi, PointOrder, MaxRadius, i;
	//1. 获取矩形区域图像
	Mat rectInImage, rectInImage1;
	getRectRegion(inImage, rectangle, rectInImage1);
	rectInImage1.copyTo(rectInImage);
	if (rectInImage.channels() != 3){
		ret = -1; return ret;
	}
	Image = MatToHImage(rectInImage);
	get_image_size(Image, &Width, &Height);

	rgb1_to_gray(Image, &ImageGray);
	gauss_image(ImageGray, &ImageGray, 3);
	threshold(ImageGray, &ImageThresh, 5, 50);
	opening_circle(ImageThresh, &RegionOpening, 1);
	connection(RegionOpening, &ConnectedRegions);
	select_shape(ConnectedRegions, &ConnectedRegions, "circularity", "and", 0.7, 1);
	select_shape(ConnectedRegions, &ConnectedRegions, "area", "and", 100, 3000000000);
	area_center(ConnectedRegions, &area, &row, &column);
	gen_contour_region_xld(ConnectedRegions, &Contours, "border");
	select_contours_xld(Contours, &Contours, "contour_length", 100, 9999999, -0.5,0.5);

	count_obj(Contours, &NumberCircles);
	if (0 != (NumberCircles<1))
	{
		outRaw = 0;
		outColumn = 0;
		circleCentre.x = 0;
		circleCentre.y = 0;
		ret = -1;
	}
	else
	{
		fit_circle_contour_xld(Contours, "geotukey", -1, 0, 0, 10, 2, &Row, &Column,&Radius, &StartPhi, &EndPhi, &PointOrder);
		//  ahuber   'algebraic'
		//会报错 算法抄在纸上
		gen_circle_contour_xld(&ContCircle, Row, Column, Radius, 0, HTuple(360).Rad(),
			"positive", 1.0);

		tuple_max(Radius, &MaxRadius);
		for (i = 0; i <= NumberCircles - 1; i += 1)
		{
			if (0 != (HTuple(Radius[i]) == MaxRadius))
			{
				outRaw = Row[i];
				outColumn = Column[i];
				circleCentre.y = outRaw[0].D() + rectangle.topLeftPoint.y;
				circleCentre.x = outColumn[0].D() + rectangle.topLeftPoint.x;
			}
		}
	}
#ifdef _DEBUG
	/*halcon图像显示*/
	//Halcon::get_image_size(ho_Image, &Width, &Height);
	//if (HDevWindowStack::IsOpen())
	//	Halcon::close_window(HDevWindowStack::Pop());
	//Halcon::set_window_attr("background_color", "black");
	//Halcon::open_window(0, 0, Width / 10, Height / 10, 0, "", "", &WindowHandle);
	//HDevWindowStack::Push(WindowHandle);

	//if (HDevWindowStack::IsOpen())
	//	set_color(HDevWindowStack::GetActive(), "red");
	//if (HDevWindowStack::IsOpen())
	//	Halcon::disp_obj(Image, HDevWindowStack::GetActive());
	//	Halcon::disp_obj(ho_SelectedContours, HDevWindowStack::GetActive());
	//Sleep(500);
	//Halcon::close_window(HDevWindowStack::Pop());

	/*opencv显示*/

	//Point p(circleCentre.x, circleCentre.y);
	//circle(rectInImage, p, 0, Scalar(0, 255, 0));
	//imshow("圆孔定位", rectInImage);
	//Sleep(1000);

#endif
	return ret;

}




// 图像校正
/*****************************************************************************
*  @brief    : 图像校正
*****************************************************************************/
extern"C" __declspec(dllexport)  int affineImage(
	INOUT Mat & inImage,						      // 圆孔（mark）图片
	IN AffinePara affinePara						 // 仿射变换的参数输入
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
	hv_RowTarget[0] = affinePara.originPoint1.y;
	hv_RowTarget[1] = affinePara.originPoint2.y;
	hv_RowTarget[2] = affinePara.originPoint3.y;
	hv_ColumnTarget[0] = affinePara.originPoint1.x;
	hv_ColumnTarget[1] = affinePara.originPoint2.x;
	hv_ColumnTarget[2] = affinePara.originPoint3.x;
	hv_RowSource[0] = affinePara.newPoint1.y;
	hv_RowSource[1] = affinePara.newPoint2.y;
	hv_RowSource[2] = affinePara.newPoint3.y;
	hv_ColumnSource[0] = affinePara.newPoint1.x;
	hv_ColumnSource[1] = affinePara.newPoint2.x;
	hv_ColumnSource[2] = affinePara.newPoint3.x;
	ho_Image = MatToHImage(inImage);
	//Determine affine transformation from input points
	vector_to_hom_mat2d(hv_RowSource, hv_ColumnSource, hv_RowTarget,
		hv_ColumnTarget, &hv_HomMat2D);
	affine_trans_image(ho_Image, &ho_ImageAffinTrans, hv_HomMat2D, "constant",
		"true");
	inImage = HImageToMat(ho_ImageAffinTrans);
	return 0;
}

// 黑胶检测结果返回
/*****************************************************************************
*  @brief    : 黑胶区域返回
*****************************************************************************/
extern"C" __declspec(dllexport)  int getAdhesiveResults(
	IN Mat& inImage,                               // 拼接后的大图片
	IN IVRectangle rectangle,                     // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,                // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults       // 输出黑胶在图片中的比例
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
		rectangle.bottomRightPoint.y>inImage.rows || adhesivePara.algorithmID<1
		){
		ret = -1; return ret;
	}

	//检测
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

// 亮线检测
/*****************************************************************************
*  @brief    : 亮线检测结果
*****************************************************************************/
extern"C" __declspec(dllexport)  int getBrightLineResults(
	IN cv::Mat& inImage,											// 拼接后的大图片
	IN IVRectangle rectangle,							           // 带有黑胶的矩形区域
	IN BrightLinePara brightLinePara,				              // 黑胶检测对应的输入参数
	OUT BrightLineResults & brightLineResults                    // 输出黑胶在图片中的比例
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

// DM二维码识别
/*****************************************************************************
*  @brief    : DM二维码识别
*****************************************************************************/
extern"C" __declspec(dllexport)  int DMDecode(
	IN Mat& inImage,								  // 拼接后的大图片
	IN IVRectangle rectangle,                        // 带有二维码的矩形区域
	IN DMCodePara dmCodePara,                       // DM二维码的输入参数
	OUT char * outImage,                          // 内存由算法申请，上位机来释放
	OUT int & charCount                           // 字符长度
	)
{
	if (inImage.empty() || dmCodePara.time_out <= 0 ||
		rectangle.topLeftPoint.x<0 || rectangle.topLeftPoint.x>inImage.cols ||
		rectangle.topLeftPoint.y<0 || rectangle.topLeftPoint.y>inImage.rows ||
		rectangle.bottomRightPoint.x<0 || rectangle.bottomRightPoint.x>inImage.cols ||
		rectangle.bottomRightPoint.y<0 || rectangle.bottomRightPoint.y>inImage.rows ||
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