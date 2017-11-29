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
	float sigma;			//��˹���� Ĭ��ֵ��3   �ò�������ɾ��
	int minThresh;			//�����ֵ Ĭ��ֵ��5
	int maxThresh;			//�����ֵ Ĭ��ֵ��50
	int Openradius;			//������뾶 Ĭ��ֵ��1   ����ɾ��
	int minArea;			//��С���  Ĭ��ֵ��100
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
