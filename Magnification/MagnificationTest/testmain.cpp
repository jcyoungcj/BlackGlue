#include "IVMagnificationDLL.h"
#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

void main(){
		//clock_t start = clock();
		Mat ho_Image = imread("E:/������ʿ����·����/Բ��ͼ��/10.png");
		clock_t mark = clock();
		int magnification;
		int mag = 3;				//��ֱ֪����΢��		
		int algorithmID = 3;
		MagPara1 magPara1 = { 2.4, 2.0, 50, 20 };
		MagPara2 magPara2 = { 2.5, 0, 65, 6, 500 };
		MagPara magPara = { algorithmID, magPara1, magPara2 };
		getImageMagnification(ho_Image, mag, magPara, magnification);
		clock_t magni = clock();
		cout << "�Ŵ��ʼ��ʱ�䣺" << (double)(magni - mark) << endl;	
		waitKey(0);
}