#include "halconcpp.h"
#include "core/core.hpp"

Halcon::Hobject MatToHImage(cv::Mat &mat);
cv::Mat HImageToMat(Halcon::Hobject &Hobj);