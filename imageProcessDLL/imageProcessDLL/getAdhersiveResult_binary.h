#include "ImgTransform.h"
#include "IVImageProcess.h"

// ��ֵ�ָ�ڽ����
/*****************************************************************************
*  @brief    : �ڽ����������
*****************************************************************************/
int getAdhersiveResult_binary(
	IN cv::Mat& inImage,                   // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,               // ���кڽ��ľ�������
	IN AdhesivePara adhesivePara,           // �ڽ�����Ӧ���������
	OUT AdhesiveResults & adhesiveResults   // ����ڽ������
	);
