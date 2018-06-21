#include <opencv2/highgui/highgui.hpp>     
#include <opencv2/imgproc/imgproc.hpp>
#include <string>   
#include <iostream>   
#include<fstream>
using namespace cv;
using namespace std;
void Divide(string filename)
{
	cv::VideoCapture capture(filename);
	if (!capture.isOpened())
	{
		cout << "open video error";
	}
	int frame_number = capture.get(CV_CAP_PROP_FRAME_COUNT);
	int num = 0;
	string img_name;
	cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
	while (true)
	{
		cv::Mat frame;
		bool bSuccess = capture.read(frame);
		char key = cvWaitKey(20);
		imshow("Video", frame);
		img_name = "";//your path and name of image
		imwrite(img_name, frame);
		if (num == frame_number)
		{
			break;
		}
	}
	capture.release();
}
void main(int argc, char** argv)
{
	Mat img;
	FILE* f = 0;
	string video_name = "";//your path
	Divide(video_name);
}