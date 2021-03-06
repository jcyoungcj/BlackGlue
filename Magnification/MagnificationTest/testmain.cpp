#include "IVMagnificationDLL.h"
#include <iostream>
#include <time.h>
using namespace std;
using namespace cv;

void main(){
		//clock_t start = clock();
		Mat ho_Image = imread("E:/滨江富士康电路板检测/圆孔图像/fangda.bmp");
		clock_t mark = clock();
		int magnification;
		int mag = 10.2;				//已知直径，微米		
		int algorithmID = 2;
		MagPara1 magPara1 = { 3.0, 5, 50, 1, 100 };
		
		MagPara magPara = { algorithmID, magPara1 };
		getImageMagnification(ho_Image, mag, magPara, magnification);
		clock_t magni = clock();
		cout << "放大率检测时间：" << (double)(magni - mark) << endl;	
		waitKey(0);

}