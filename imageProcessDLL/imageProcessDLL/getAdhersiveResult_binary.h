#include "ImgTransform.h"
#include "IVImageProcess.h"

// 阈值分割法黑胶检测
/*****************************************************************************
*  @brief    : 黑胶检测结果返回
*****************************************************************************/
int getAdhersiveResult_binary(
	IN cv::Mat& inImage,                   // 拼接后的大图片
	IN IVRectangle rectangle,               // 带有黑胶的矩形区域
	IN AdhesivePara adhesivePara,           // 黑胶检测对应的输入参数
	OUT AdhesiveResults & adhesiveResults   // 输出黑胶检测结果
	);
