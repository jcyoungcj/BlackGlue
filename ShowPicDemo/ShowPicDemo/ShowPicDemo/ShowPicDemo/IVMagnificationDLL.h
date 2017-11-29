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
	float sigma;			//��˹���� Ĭ��ֵ��2.5
	int minThresh;			//�����ֵ Ĭ��ֵ��0
	int maxThresh;			//�����ֵ Ĭ��ֵ��65
	int Openradius;			//������뾶 Ĭ��ֵ��6
	int minArea;			//��С���  Ĭ��ֵ��500
}MagPara1;
typedef struct _MagPara
{
	int algorithmID;                  // �㷨ID��1��ʼ����������(1,2)
	MagPara1 magPara1;

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
