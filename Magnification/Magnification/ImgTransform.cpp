#include "stdafx.h"
#include "ImgTransform.h"

using namespace Halcon;


Hobject MatToHImage(cv::Mat &mat)
{
	Hobject Hobj;
	if (mat.empty()) return Hobj;
	int width = mat.cols;
	int height = mat.rows;
	if (3 == mat.channels()) {
		std::vector<cv::Mat> mv;
		split(mat, mv);
		gen_image3(&Hobj, "byte", width, height, (Hlong)(mv[2].data), (Hlong)(mv[1].data), (Hlong)(mv[0].data));
	}
	if (1 == mat.channels())
	{
		uchar *dataGray = new uchar[width*height];
		for (int i = 0; i<height; i++){
			memcpy(dataGray + width*i, mat.ptr() + mat.step*i, width);
		}
		gen_image1(&Hobj, "byte", width, height, (Hlong)(dataGray));
		delete[] dataGray;
	}
	return Hobj;
}
cv::Mat HImageToMat(Hobject &Hobj)
{
	if (Hobj.Id() == H_EMPTY_REGION) {
		return cv::Mat();
	}
	HTuple htCh = HTuple();
	HTuple cType;
	cv::Mat Image;
	convert_image_type(Hobj, &Hobj, "byte");
	count_channels(Hobj, &htCh);
	HTuple wid;
	HTuple hgt;
	int W, H;
	if (htCh[0].I() == 1)
	{
		HTuple ptr;
		get_image_pointer1(Hobj, &ptr, &cType, &wid, &hgt);
		W = wid[0].L();
		H = hgt[0].L();
		Image.create(H, W, CV_8UC1);
		uchar* pdata = (uchar*)ptr[0].L();
		memcpy(Image.data, pdata, W*H);
	}
	else if (htCh[0].I() == 3)
	{
		HTuple ptrR, ptrG, ptrB;
		get_image_pointer3(Hobj, &ptrR, &ptrG, &ptrB, &cType, &wid, &hgt);
		W = wid[0].L();
		H = hgt[0].L();
		Image.create(H, W, CV_8UC3);
		std::vector<cv::Mat> vecM(3);
		uchar* pr = (uchar*)ptrR[0].L();
		uchar* pg = (uchar*)ptrG[0].L();
		uchar* pb = (uchar*)ptrB[0].L();
		vecM[2] = cv::Mat(H, W, CV_8UC1, pr);
		vecM[1] = cv::Mat(H, W, CV_8UC1, pg);
		vecM[0] = cv::Mat(H, W, CV_8UC1, pb);
		merge(vecM, Image);
	}
	return Image;
}