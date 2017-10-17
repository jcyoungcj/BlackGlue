#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include "IVImageProcess.h"
using cv::Mat;
using std::vector;
using cv::Rect;
using cv::Point;

int DetectBrightLine_1(Mat& src, IVRectangle rect, BrightLinePara1& para, BrightLineResults& res);
int DetectBrightLine_2(Mat& src, IVRectangle rect, BrightLinePara2& para, BrightLineResults& res);