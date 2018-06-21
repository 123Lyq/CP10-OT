#include <iostream>  
#include <highgui.h>  
#include <cv.h>  
using namespace std;
int main()
{
	IplImage *img;
	char filename[100];
	CvVideoWriter *writer = NULL;
	for (int i = 1; i <= 154; i++)
	{
		sprintf(filename, "%s%d%s", "image (", i, ").jpg");//Modify here according to your path
		img = cvLoadImage(filename);
		if (img == NULL)
			continue;
		if (writer == NULL)
		{
			writer = cvCreateVideoWriter("out1.avi", CV_FOURCC('D', 'I', 'V', 'X'), 7, cvGetSize(img));
		}
		if (writer == NULL)
		{
			cout << "Fail£¡" << endl;
			exit(0);
		}
		cvWriteFrame(writer, img);
		cvReleaseImage(&img);
	}
	cvReleaseVideoWriter(&writer);
	return 0;
}