
// stdafx.cpp : 只包括标准包含文件的源文件
// ShowPicDemo.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

//
//#ifdef _DEBUG
//#pragma comment(lib, "opencv_aruco320d.lib")
//#pragma comment(lib, "opencv_bgsegm320d.lib")
//#pragma comment(lib, "opencv_bioinspired320d.lib")
//#pragma comment(lib, "opencv_calib3d320d.lib")
//#pragma comment(lib, "opencv_ccalib320d.lib")
//#pragma comment(lib, "opencv_core320d.lib")
//#pragma comment(lib, "opencv_datasets320d.lib")
//#pragma comment(lib, "opencv_dnn320d.lib")
//#pragma comment(lib, "opencv_dpm320d.lib")
//#pragma comment(lib, "opencv_face320d.lib")
//#pragma comment(lib, "opencv_features2d320d.lib")
//#pragma comment(lib, "opencv_flann320d.lib")
//#pragma comment(lib, "opencv_fuzzy320d.lib")
//#pragma comment(lib, "opencv_hdf320d.lib")
//#pragma comment(lib, "opencv_highgui320d.lib")
//#pragma comment(lib, "opencv_imgcodecs320d.lib")
//#pragma comment(lib, "opencv_imgproc320d.lib")
//#pragma comment(lib, "opencv_line_descriptor320d.lib")
//#pragma comment(lib, "opencv_ml320d.lib")
//#pragma comment(lib, "opencv_objdetect320d.lib")
//#pragma comment(lib, "opencv_optflow320d.lib")
//#pragma comment(lib, "opencv_phase_unwrapping320d.lib")
//#pragma comment(lib, "opencv_photo320d.lib")
//#pragma comment(lib, "opencv_plot320d.lib")
//#pragma comment(lib, "opencv_reg320d.lib")
//#pragma comment(lib, "opencv_rgbd320d.lib")
//#pragma comment(lib, "opencv_saliency320d.lib")
//#pragma comment(lib, "opencv_shape320d.lib")
//#pragma comment(lib, "opencv_stereo320d.lib")
//#pragma comment(lib, "opencv_stitching320d.lib")
//#pragma comment(lib, "opencv_structured_light320d.lib")
//#pragma comment(lib, "opencv_superres320d.lib")
//#pragma comment(lib, "opencv_surface_matching320d.lib")
//#pragma comment(lib, "opencv_text320d.lib")
//#pragma comment(lib, "opencv_tracking320d.lib")
//#pragma comment(lib, "opencv_ts320d.lib")
//#pragma comment(lib, "opencv_video320d.lib")
//#pragma comment(lib, "opencv_videoio320d.lib")
//#pragma comment(lib, "opencv_videostab320d.lib")
//#pragma comment(lib, "opencv_xfeatures2d320d.lib")
//#pragma comment(lib, "opencv_ximgproc320d.lib")
//#pragma comment(lib, "opencv_xobjdetect320d.lib")
//#pragma comment(lib, "opencv_xphoto320d.lib")
//#else
//#pragma comment(lib, "opencv_aruco320.lib")
//#pragma comment(lib, "opencv_bgsegm320.lib")
//#pragma comment(lib, "opencv_bioinspired320.lib")
//#pragma comment(lib, "opencv_calib3d320.lib")
//#pragma comment(lib, "opencv_ccalib320.lib")
//#pragma comment(lib, "opencv_core320.lib")
//#pragma comment(lib, "opencv_datasets320.lib")
//#pragma comment(lib, "opencv_dnn320.lib")
//#pragma comment(lib, "opencv_dpm320.lib")
//#pragma comment(lib, "opencv_face320.lib")
//#pragma comment(lib, "opencv_features2d320.lib")
//#pragma comment(lib, "opencv_flann320.lib")
//#pragma comment(lib, "opencv_fuzzy320.lib")
//#pragma comment(lib, "opencv_hdf320.lib")
//#pragma comment(lib, "opencv_highgui320.lib")
//#pragma comment(lib, "opencv_imgcodecs320.lib")
//#pragma comment(lib, "opencv_imgproc320.lib")
//#pragma comment(lib, "opencv_line_descriptor320.lib")
//#pragma comment(lib, "opencv_ml320.lib")
//#pragma comment(lib, "opencv_objdetect320.lib")
//#pragma comment(lib, "opencv_optflow320.lib")
//#pragma comment(lib, "opencv_phase_unwrapping320.lib")
//#pragma comment(lib, "opencv_photo320.lib")
//#pragma comment(lib, "opencv_plot320.lib")
//#pragma comment(lib, "opencv_reg320.lib")
//#pragma comment(lib, "opencv_rgbd320.lib")
//#pragma comment(lib, "opencv_saliency320.lib")
//#pragma comment(lib, "opencv_shape320.lib")
//#pragma comment(lib, "opencv_stereo320.lib")
//#pragma comment(lib, "opencv_stitching320.lib")
//#pragma comment(lib, "opencv_structured_light320.lib")
//#pragma comment(lib, "opencv_superres320.lib")
//#pragma comment(lib, "opencv_surface_matching320.lib")
//#pragma comment(lib, "opencv_text320.lib")
//#pragma comment(lib, "opencv_tracking320.lib")
//#pragma comment(lib, "opencv_ts320.lib")
//#pragma comment(lib, "opencv_video320.lib")
//#pragma comment(lib, "opencv_videoio320.lib")
//#pragma comment(lib, "opencv_videostab320.lib")
//#pragma comment(lib, "opencv_xfeatures2d320.lib")
//#pragma comment(lib, "opencv_ximgproc320.lib")
//#pragma comment(lib, "opencv_xobjdetect320.lib")
//#pragma comment(lib, "opencv_xphoto320.lib")
//#endif








char* cstring_to_char(CString& str)
{
	int n = str.GetLength();     //
	//获取宽字节字符的大小，大小是按字节计算的
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char * pFileName = new char[len + 1];   //以字节为单位
	//宽字节编码转换成多字节编码
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), pFileName, len, NULL, NULL);
	pFileName[len] = '\0';   //多字节字符以'\0'结束

	return pFileName;
}
void DrawCImageCenter(ATL::CImage& image, CWnd* pwnd, CRect& dstRect, COLORREF bkColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (image.IsNull())
		return;
	CRect rect;
	pwnd->GetClientRect(&rect);
	int img_width = image.GetWidth();
	int img_height = image.GetHeight();
	//计算水平点
	int cx0 = (rect.right - img_width) / 2;
	int cy0 = (rect.bottom - img_height) / 2;
	CClientDC dc(pwnd);
	HDC hdc = dc.GetSafeHdc();
	SetStretchBltMode(hdc, HALFTONE);
	SetBrushOrgEx(hdc, 0, 0, NULL);
	dstRect = rect;
	if (cx0 >= 0 && cy0 >= 0)
	{
		dstRect.left = cx0;
		dstRect.top = cy0;
		dstRect.right = cx0 + img_width;
		dstRect.bottom = cy0 + img_height;
	}
	//填充背景色
	FillRect(hdc, &rect, CBrush(bkColor));
	image.Draw(hdc, dstRect, CRect(0, 0, image.GetWidth(), image.GetHeight()));
	return;
}


void Mat2CImage(void* data, int width, int height, int channels, int step, ATL::CImage& cimg)
{
	if (data == nullptr)
		return;
	if (cimg.IsNull())
		cimg.Create(width, height, 8 * channels);
	else if (cimg.GetWidth() != width ||
		cimg.GetHeight() != height ||
		cimg.GetBPP() / 8 != channels)
	{
		cimg.Destroy();
		cimg.Create(width, height, 8 * channels);
	}
	unsigned char* ps;
	unsigned char* pd = (unsigned char*)data;
	unsigned char* pimg = (unsigned char*)cimg.GetBits(); //获取CImage的像素存贮区的指针 
	int cimg_step = cimg.GetPitch();//每行的字节数,注意这个返回值有正有负

	// 如果是1个通道的图像(灰度图像) DIB格式才需要对调色板设置   
	// CImage中内置了调色板，我们要对他进行赋值： 
	if (1 == channels)
	{
		RGBQUAD* ColorTable;
		int MaxColors = 256;
		//这里可以通过CI.GetMaxColorTableEntries()得到大小(如果你是CI.Load读入图像的话)   
		ColorTable = new RGBQUAD[MaxColors];
		cimg.GetColorTable(0, MaxColors, ColorTable);//这里是取得指针   
		for (int i = 0; i< MaxColors; i++)
		{
			ColorTable[i].rgbBlue = (BYTE)i;
			//BYTE和uchar一回事，但MFC中都用它   
			ColorTable[i].rgbGreen = (BYTE)i;
			ColorTable[i].rgbRed = (BYTE)i;
		}
		cimg.SetColorTable(0, MaxColors, ColorTable);
		delete[]ColorTable;
	}
	for (int y = 0; y < height; y++)
	{
		ps = pd + y * step;
		for (int x = 0; x < width; x++)
		{
			if (1 == channels)
			{
				*(pimg + y * cimg_step + x) = ps[x];
			}
			else if (3 == channels)
			{
				*(pimg + y* cimg_step + x * 3 + 0) = ps[x * 3];
				*(pimg + y* cimg_step + x * 3 + 1) = ps[x * 3 + 1];
				*(pimg + y* cimg_step + x * 3 + 2) = ps[x * 3 + 2];
			}
		}
	}
}
