#include "BrightLineHelper.h"
using namespace cv;

const char* METHOD_1 = "METHOD_1"; //算法的名字
const char* METHOD_2 = "METHOD_2"; //算法的名字

int DetectBrightLine_1(Mat& src, IVRectangle rect, BrightLinePara1& para, BrightLineResults& res)
{
	if (src.empty())
		return -1;
	if (rect.topLeftPoint.x > rect.bottomRightPoint.x || rect.topLeftPoint.y > rect.bottomRightPoint.y)
		return -1;
	if (para.radius < 3 || para.thre < 5)
		return -1;

	res.arrSize = 0;
	strcpy(res.algorithmName, METHOD_1);

	//bright line
	vector<vector<Point>> vcs;
	Rect roi = { rect.topLeftPoint.x, rect.topLeftPoint.y, rect.bottomRightPoint.x - rect.topLeftPoint.x, rect.bottomRightPoint.y - rect.topLeftPoint.y };
	Mat bin;
	Mat mroi = src(roi);
	vector<Mat> mv;
	split(mroi, mv);
	Mat G = mv[1];
	if (para.thre > 0)
		threshold(G, bin, para.thre, 255, THRESH_BINARY);
	else
		threshold(G, bin, 128, 255, THRESH_OTSU);
	bin = ~bin;

	int gker = para.radius;
	gker = gker % 2 == 1 ? gker : gker + 1;
	GaussianBlur(bin, bin, { gker, gker }, 2.5);
	threshold(bin, bin, 80, 255, THRESH_BINARY);
	gker = gker / 2;
	gker = gker % 2 == 1 ? gker : gker + 1;
	auto ele = getStructuringElement(MORPH_RECT, { gker, gker });
	erode(bin, bin, ele, Point(-1, -1), 3);
	dilate(bin, bin, ele, Point(-1, -1), 6);
	erode(bin, bin, ele, Point(-1, -1), 3);
	findContours(bin, vcs, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//trans
	for (int i = 0; i < vcs.size(); i++)
	{
		for (auto& pt : vcs[i])
			pt += Point(roi.x, roi.y);
	}
	//output
	if (vcs.size() > 0)
	{
		std::sort(vcs.begin(), vcs.end(), [](const vector<Point>& left, const vector<Point>& right) {
			return contourArea(left) > contourArea(right);
		});
		res.arrContours[0].rectBounding = boundingRect(vcs[0]);
		res.arrContours[0].fArea = contourArea(vcs[0]);
		res.arrSize = 1;
	}
	return 0;
}

int DetectBrightLine_2(Mat& src, IVRectangle rect, BrightLinePara2& para, BrightLineResults& res)
{
	res.arrSize = 0;
	strcpy(res.algorithmName, METHOD_2);


	////bright line
	//vector<vector<Point>> vcs;
	//Rect roi = { rect.topLeftPoint.x , rect.topLeftPoint.y, rect.bottomRightPoint.x - rect.topLeftPoint.x, rect.bottomRightPoint.y - rect.topLeftPoint.y };
	//DetectGlue(src, roi, vcs, para.thre);
	//if (vcs.size() > 0)
	//{
	//	std::sort(vcs.begin(), vcs.end(), [](const vector<Point>& left, const vector<Point>& right) {
	//		return contourArea(left) > contourArea(right);
	//	});
	//	res.arrContours[0].rectBounding = boundingRect(vcs[0]);
	//	res.arrContours[0].fArea = contourArea(vcs[0]);
	//	res.arrSize = 1;
	//}
	return 0;
}
