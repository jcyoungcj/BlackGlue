#pragma once
// ʹ��opencv������opencv���ͷ�ļ�
#include <core/core.hpp>
#include <highgui/highgui.hpp>

// �������������ʶ
#define IN
#define OUT
#define INOUT


// Point
typedef struct _IVPoint
{
	int x;                    // x ����
	int y;                    // y ����
}IVPoint;

// ��������
typedef struct _IVRectangle
{
	IVPoint topLeftPoint;                         // �������Ͻǵĵ�
	IVPoint bottomRightPoint;					 // �������Ͻǵĵ�
}IVRectangle;



// ��λ��λ�ü��㣬�������
typedef struct _MarkPara
{
	double alpha;									// canny����   Ĭ��ֵ�� 2.0 ,1.0  ����ɾ��
	double sigma;									// gauss	   Ĭ��ֵ��2.4     ����ɾ��
	int minGirth;									// ��С�ܳ�	   Ĭ��ֵ��50     ����ɾ��
	int maxGirth;									// ����ܳ�    Ĭ��ֵ��99999  ����ɾ��
	int maxLackGirth;								// Բ���ȱ��  Ĭ��ֵ��20    ����ɾ��
}MarkPara;

/* ��ϸmark ����
typedef struct _MarkPara
{
	double circularity;								// Բ������	   Ĭ��ֵ�� 0.70

}MarkPara;

*/


// ͼ��У������
typedef struct _AffinePara
{
	IVPoint originPoint1;				 // ԭʼ��׼ͼƬ��λ��1����λ��
	IVPoint originPoint2;				 // ԭʼ��׼ͼƬ��λ��2����λ��
	IVPoint originPoint3;				 // ԭʼ��׼ͼƬ��λ��3����λ��
	IVPoint newPoint1;					 // ��ͼƬ��λ��1����λ��
	IVPoint newPoint2;					 // ��ͼƬ��λ��2����λ��
	IVPoint newPoint3;					 // ��ͼƬ��λ��3����λ��
}AffinePara;

// �ڽ�������
typedef struct _AdhesiveThreshPara1
{
	int minThreshold;                      // ��С��ֵ      Ĭ��ֵ��	 ����ɾ��
	int maxThreshold;					  // �����ֵ		Ĭ��ֵ��42
	double openRadius;					 // ������뾶		Ĭ��ֵ��3.5  ����ɾ��
}AdhesiveThreshPara1;

typedef struct _AdhesiveThreshPara2			//����ȫ��ɾ��
{
	double sigma;						  // ��˹�˲�����   Ĭ��ֵ��1
	double openRadius;					 // ������뾶      Ĭ��ֵ��3.5
}AdhesiveThreshPara2;

typedef struct _AdhesivePara
{
	int algorithmID;                  // �㷨ID��1��ʼ����������
	AdhesiveThreshPara1 adhesivePara1;
	AdhesiveThreshPara2 adhesivePara2;
}AdhesivePara;


// �ڽ���������
typedef struct _AdhesiveArea
{
	int connectionNum;					      // ��ͨ�����
	IVRectangle pRectangle[100];
	int pArea[100];
}AdhesiveArea;

typedef struct _AdhesiveResults
{
	AdhesiveArea adhesiveBlack;						   //  �ڽ������
	AdhesiveArea adhesiveWhite;						  //  �ڽ��ڲ��հ׼����
}AdhesiveResults;

//7�����߼��

// ���߼��
/*****************************************************************************
*  @brief    : ���߼����
*****************************************************************************/


typedef struct _BrightLinePara1
{
	// ***������㷨�������������****
	//********************************/
	int		thre;					//�����ֵ
	int		radius;					//�˲��뾶
}BrightLinePara1;

typedef struct _BrightLinePara2
{
	// ***������㷨�������������****
	//********************************/
	int		minDiffThre;			//��С�����ֵ
	int		matchRadius;			//ƥ��뾶
	int		blurRadius;				//�˲��뾶
}BrightLinePara2;
// ���߼�����
typedef struct _BrightLinePara
{
	int algorithmID;                  // �㷨ID��1��ʼ����������
	BrightLinePara1 brightLinePara1;
	BrightLinePara2 brightLinePara2;
}BrightLinePara;

// ���߼��������
typedef struct _BrightLineResults
{
	char algorithmName[50];
	// ***������㷨�������������****
	//********************************/
	const static int maxSize = 100;
	struct _contourArea {
		cv::Rect	rectBounding;			//����������
		float	    fArea;					//���������
	}arrContours[maxSize];
	int arrSize;
}BrightLineResults;

// DM��ά��ʶ���������
typedef struct _DMCodePara
{
	int time_out;                                // ��ʱ��ֵ����λms,����200ms
}DMCodePara; 




// Բ��λ�÷���
/*****************************************************************************
*  @brief    : Բ��λ�÷���
*****************************************************************************/
extern"C" __declspec(dllexport)  int getMarkLocation(
	IN cv::Mat& inImage,								// ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,               // ����ľ�������
	IN MarkPara makePara,					// Բ�ף�mark������Ӧ���������
	OUT IVPoint & circleCentre		    // ����Բ������ Ϊ0��û�м�⵽Բ
	);

// ͼ��У��
/*****************************************************************************
*  @brief    : ͼ��У��
*****************************************************************************/
extern"C" __declspec(dllexport)  int affineImage(
	INOUT cv::Mat & inImage,						 // Բ�ף�mark��ͼƬ
	IN AffinePara affinePara						 // ����任�Ĳ�������
	);

// �ڽ����������
/*****************************************************************************
*  @brief    : �ڽ������������
*****************************************************************************/
extern"C" __declspec(dllexport)  int getAdhesiveResults(
	IN cv::Mat& inImage,												 // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,								 // ���кڽ��ľ�������
	IN AdhesivePara adhesivePara,					     // �ڽ�����Ӧ���������
	OUT AdhesiveResults & adhesiveResults        // ����ڽ����Ľ��
	);

// ���߼��
/*****************************************************************************
*  @brief    : ���߼����
*****************************************************************************/
extern"C" __declspec(dllexport)  int getBrightLineResults(
	IN cv::Mat& inImage,											    // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,							      // ��������
	IN BrightLinePara brightLinePara,				      // ���߼����������
	OUT BrightLineResults & brightLineResults     // ���߼����� 
	);

// DM��ά��ʶ��
/*****************************************************************************
*  @brief    : DM��ά��ʶ��
*****************************************************************************/
extern"C" __declspec(dllexport)  int DMDecode(
	IN cv::Mat& inImage,								  // ƴ�Ӻ�Ĵ�ͼƬ
	IN IVRectangle rectangle,                 // ���ж�ά��ľ�������
	IN DMCodePara dmCodePara,         // DM��ά����������
	OUT char * outImage,                       // �ڴ����㷨���룬��λ�����ͷ�
	OUT int & charCount                        // �ַ�����
	);

