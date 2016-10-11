#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<windows.h>
#include"screenCapture.h"
#include "main.h"

//hind console windows
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using namespace cv;
using namespace std;

int testLoadimg() {
	Mat img = imread("C:/Users/v-chuqi/Pictures/ce2eacc8c94038be04af1fa3d134530d.jpg", IMREAD_COLOR);
	if (!img.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("Demo", WINDOW_AUTOSIZE);
	imshow("Demo", img);
	waitKey(0);
	return 0;
}

Mat screencut(double rowsMin_p,double rowsMax_p,double colsMin_p,double colsMax_p)
{
	const wchar_t processName[] = L"lync.exe";
	vector<HWND> list;
	bool visibleOnly = true;

	//LPCWSTR pszNewWindowTitle = L"Skype for Business ";
	//HWND hWnd = FindWindow(NULL, pszNewWindowTitle);

	/*getHwndsByProcessName(processName, list, visibleOnly);
	HWND hWnd = list[0];*/
	HWND hWnd = GetDesktopWindow();
	WindowCapture cap(hWnd, true);
	Mat dst;
	cap.captureFrame(dst);
	int rows = dst.rows;
	int cols = dst.cols;
	Mat temp = dst(Range(rows*rowsMin_p, rows*rowsMax_p), Range(cols*colsMin_p, cols*colsMax_p));
	//imshow("Demo", temp);
	return temp;
}

void MatchingMethod(Mat src, Mat templ, int method, double threshold){
	int x = src.type();
	int y = templ.type();

	Mat src_2;
	Mat templ_2;

	src_2.create(src.size(), src.type());
	int srcnl = src.cols*src.channels();
	for (int i = 0; i < src.rows; i++) {
		const uchar* inData = src.ptr<uchar>(i);
		uchar* outData = src_2.ptr<uchar>(i);
		for (int j = 0; j < srcnl; j++)
		{
			if (inData[j] >= 128)
				outData[j] = 255;
			else
				outData[j] = 1;
		}
	}


	templ_2.create(templ.size(), templ.type());
	int templnl = templ.cols*templ.channels();
	for (int i = 0; i < templ.rows; i++) {
		const uchar* inData = templ.ptr<uchar>(i);
		uchar* outData = templ_2.ptr<uchar>(i);
		for (int j = 0; j < templnl; j++)
		{
			if (inData[j] >= 128)
				outData[j] = 255;
			else
				outData[j] = 1;
		}
	}


	/*cvtColor(src, src, CV_32FC3);
	x = src.type();*/
	//cvtColor(templ, templ, CV_32FC3);
	Mat img_display;
	src_2.copyTo(img_display);
	int result_cols = src.cols - templ.cols + 1;
	int result_rows = src.rows - templ.rows + 1;
	Mat result;
	result.create(result_rows, result_cols, CV_32FC3);
	//"Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED"
	
	bool method_accepts_mask = (CV_TM_SQDIFF == method || method == CV_TM_CCORR_NORMED);

	matchTemplate(src_2,templ_2,result,method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	Point matchLocList[10000];
	int Count = 0;
	int m = result.channels();
	int nl = result.cols*result.channels();
	for (int i = 0; i < result.rows; i++) {
		const float* inData = result.ptr<float>(i);
		for (int j = 0; j < nl; j++)
		{
			if (!(method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)) {
				
				if ((1 - inData[j]) <= threshold) {
					matchLocList[Count] = Point(j, i);
					Count++;
				}
			}
			else {
				if (inData[j] <= threshold) {
					if (Count == 450)
					{
						matchLocList[Count];
						Point(j, i);
					}
					matchLocList[Count] = Point(j, i);
					Count++;
				}
			}

		}
	}

	for (int i = 0; i < Count; i++) {
		rectangle(img_display, Point(matchLocList[i].x, matchLocList[i].y), Point(matchLocList[i].x + templ.cols, matchLocList[i].y + templ.rows), Scalar::all(0), 2, 8, 0);
		rectangle(result, Point(matchLocList[i].x, matchLocList[i].y), Point(matchLocList[i].x + templ.cols, matchLocList[i].y + templ.rows), Scalar::all(0), 2, 8, 0);
	}
	

	imshow("src", img_display);
	imshow("result", result);
	waitKey(0);
}

int main()
{

	Mat src = screencut(0, 1, 0, 1);
	
	//Mat temp = screencut(0, 0.05, 0.004, 0.033);
	//imwrite("C:/Users/v-chuqi/Pictures/tmp.png", temp);
	//imwrite("C:/Users/v-chuqi/Pictures/src.png", src);

	//flag value: -1,0,1,2,4
	//Mat src = imread("C:/Users/v-chuqi/Pictures/src.jpg", -1);
	Mat temp = imread("C:/Users/v-chuqi/Pictures/ri.jpg", -1); 
	
	
	/*imshow("src", src);
	imshow("result", temp);
	waitKey(0);*/
	MatchingMethod(src, temp, 4, 0);

}