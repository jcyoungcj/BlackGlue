#pragma once
// 使用opencv，调用opencv相关头文件
#include <core/core.hpp>
#include <highgui/highgui.hpp>

// 定义输入输出标识
#define IN
#define OUT
#define INOUT


// Point
typedef struct _IVPoint
{
	int x;                    // x 坐标
	int y;                    // y 坐标
}IVPoint;

// 矩形区域
typedef struct _IVRectangle
{
	IVPoint topLeftPoint;                         // 矩形左上角的点
	IVPoint bottomRightPoint;					 // 矩形右上角的点
}IVRectangle;



// 定位孔位置计算，输入参数
typedef struct _MarkPara
{
	double alpha;									// canny算子   默认值： 2.0 ,1.0  可以删除
	double sigma;									// gauss	   默认值：2.4     可以删除
	int minGirth;									// 最小周长	   默认值：50     可以删除
	int maxGirth;									// 最大周长    默认值：99999  可以删除
	int maxLackGirth;								// 圆最大缺口  默认值：20    可以删除
}MarkPara;

/* 最细mark 参数
typedef struct _MarkPara
{
	double circularity;								// 圆完整度	   默认值： 0.70

}MarkPara;

*/


// 图像校正参数
typedef struct _AffinePara
{
	IVPoint originPoint1;				 // 原始标准图片定位孔1所在位置
	IVPoint originPoint2;				 // 原始标准图片定位孔2所在位置
	IVPoint originPoint3;				 // 原始标准图片定位孔3所在位置
	IVPoint newPoint1;					 // 新图片定位孔1所在位置
	IVPoint newPoint2;					 // 新图片定位孔2所在位置
	IVPoint newPoint3;					 // 新图片定位孔3所在位置
}AffinePara;

// 黑胶检测参数
typedef struct _AdhesiveThreshPara1
{
	int minThreshold;                      // 最小阈值      默认值：	 可以删除
	int maxThreshold;					  // 最大阈值		默认值：42
	double openRadius;					 // 开运算半径		默认值：3.5  可以删除
}AdhesiveThreshPara1;

typedef struct _AdhesiveThreshPara2			//可以全部删除
{
	double sigma;						  // 高斯滤波算子   默认值：1
	double openRadius;					 // 开运算半径      默认值：3.5
}AdhesiveThreshPara2;

typedef struct _AdhesivePara
{
	int algorithmID;                  // 算法ID从1开始，往上增加
	AdhesiveThreshPara1 adhesivePara1;
	AdhesiveThreshPara2 adhesivePara2;
}AdhesivePara;


// 黑胶区域属性
typedef struct _AdhesiveArea
{
	int connectionNum;					      // 连通域个数
	IVRectangle pRectangle[100];
	int pArea[100];
}AdhesiveArea;

typedef struct _AdhesiveResults
{
	AdhesiveArea adhesiveBlack;						   //  黑胶检测结果
	AdhesiveArea adhesiveWhite;						  //  黑胶内部空白检测结果
}AdhesiveResults;

//7、亮线检测

// 亮线检测
/*****************************************************************************
*  @brief    : 亮线检测结果
*****************************************************************************/


typedef struct _BrightLinePara1
{
	// ***下面的算法需求的其他参数****
	//********************************/
	int		thre;					//检测阈值
	int		radius;					//滤波半径
}BrightLinePara1;

typedef struct _BrightLinePara2
{
	// ***下面的算法需求的其他参数****
	//********************************/
	int		minDiffThre;			//最小差分阈值
	int		matchRadius;			//匹配半径
	int		blurRadius;				//滤波半径
}BrightLinePara2;
// 亮线检测参数
typedef struct _BrightLinePara
{
	int algorithmID;                  // 算法ID从1开始，往上增加
	BrightLinePara1 brightLinePara1;
	BrightLinePara2 brightLinePara2;
}BrightLinePara;

// 亮线检测结果返回
typedef struct _BrightLineResults
{
	char algorithmName[50];
	// ***下面的算法需求的其他参数****
	//********************************/
	const static int maxSize = 100;
	struct _contourArea {
		cv::Rect	rectBounding;			//轮廓的区域
		float	    fArea;					//轮廓的面积
	}arrContours[maxSize];
	int arrSize;
}BrightLineResults;

// DM二维码识别输入参数
typedef struct _DMCodePara
{
	int time_out;                                // 超时阈值，单位ms,建议200ms
}DMCodePara; 




// 圆孔位置返回
/*****************************************************************************
*  @brief    : 圆孔位置返回
*****************************************************************************/
extern"C" __declspec(dllexport)  int getMarkLocation(
	IN cv::Mat& inImage,								// 拼接后的大图片
	IN IVRectangle rectangle,               // 输入的矩形区域
	IN MarkPara makePara,					// 圆孔（mark）检测对应的输入参数
	OUT IVPoint & circleCentre		    // 返回圆心坐标 为0则没有检测到圆
	);

// 图像校正
/*****************************************************************************
*  @brief    : 图像校正
*****************************************************************************/
extern"C" __declspec(dllexport)  int affineImage(
	INOUT cv::Mat & inImage,						 // 圆孔（mark）图片
	IN AffinePara affinePara						 // 仿射变换的参数输入
	);

// 黑胶检测结果返回
/*****************************************************************************
*  @brief    : 黑胶区域比例返回
*****************************************************************************/
extern"C" __declspec(dllexport)  int getAdhesiveResults(
	IN cv::Mat& inImage,												 // 拼接后的大图片
	IN IVRectangle rectangle,								 // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,					     // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults        // 输出黑胶检测的结果
	);

// 亮线检测
/*****************************************************************************
*  @brief    : 亮线检测结果
*****************************************************************************/
extern"C" __declspec(dllexport)  int getBrightLineResults(
	IN cv::Mat& inImage,											    // 拼接后的大图片
	IN IVRectangle rectangle,							      // 亮线区域
	IN BrightLinePara brightLinePara,				      // 亮线检测的输入参数
	OUT BrightLineResults & brightLineResults     // 亮线检测输出 
	);

// DM二维码识别
/*****************************************************************************
*  @brief    : DM二维码识别
*****************************************************************************/
extern"C" __declspec(dllexport)  int DMDecode(
	IN cv::Mat& inImage,								  // 拼接后的大图片
	IN IVRectangle rectangle,                 // 带有二维码的矩形区域
	IN DMCodePara dmCodePara,         // DM二维码的输入参数
	OUT char * outImage,                       // 内存由算法申请，上位机来释放
	OUT int & charCount                        // 字符长度
	);

