#include "IVMagnificationDLL.h"
#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

void main(){
		//clock_t start = clock();
		Mat ho_Image = imread("E:/滨江富士康电路板检测/圆孔图像/10.png");
		clock_t mark = clock();
		int magnification;
		int mag = 3;				//已知直径，微米		
		int algorithmID = 3;
		MagPara1 magPara1 = { 2.4, 2.0, 50, 20 };
		MagPara2 magPara2 = { 2.5, 0, 65, 6, 500 };
		MagPara magPara = { algorithmID, magPara1, magPara2 };
		getImageMagnification(ho_Image, mag, magPara, magnification);
		clock_t magni = clock();
		cout << "放大率检测时间：" << (double)(magni - mark) << endl;	
		waitKey(0);
}