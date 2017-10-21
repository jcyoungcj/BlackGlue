#pragma once
// ʹ��opencv������opencv���ͷ�ļ�
#include <core/core.hpp>
#include <highgui/highgui.hpp>


// �������������ʶ
#define IN
#define OUT
#define INOUT


typedef struct _MagPara1
{
	double sigma;									//gauss		  Ĭ��ֵ��2.4
	double alpha;									// canny����  Ĭ��ֵ�� 2.0 ,1.0
	int minGirth;									// ��С�ܳ�	  Ĭ��ֵ��50
	int maxLackGirth;								// Բ���ȱ��  Ĭ��ֵ��20
}MagPara1;
typedef struct _MagPara2
{
	float sigma;			//��˹���� Ĭ��ֵ��2.5
	int minThresh;			//�����ֵ Ĭ��ֵ��0
	int maxThresh;			//�����ֵ Ĭ��ֵ��65
	int Openradius;			//������뾶 Ĭ��ֵ��6
	int minArea;			//��С���  Ĭ��ֵ��500
}MagPara2;
typedef struct _MagPara
{
	int algorithmID;                  // �㷨ID��1��ʼ����������
	MagPara1 magPara1;
	MagPara2 magPara2;
}MagPara;
// ͼ��Ŵ��ʼ���
/*****************************************************************************
*  @brief    : ����ϵͳ��ͼ��Ŵ���
*****************************************************************************/
extern"C" __declspec(dllexport)  int getImageMagnification(
	IN cv::Mat& inImage,                            // �����ⶨ�Ŵ��ʵ�ԴͼƬ
	IN int diameter,						       // ��֪Բʵ��ֱ�� ��΢�ף�
	IN MagPara magPara,							  // �Ŵ��ʼ����㷨���������
	OUT int & magnification                      // ϵͳ��ͼ��Ŵ��ʣ���λ������/��
	);