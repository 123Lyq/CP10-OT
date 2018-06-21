#include <Eigen/Dense>  
#include <iostream>  
#include <string>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/objdetect/objdetect.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <opencv2/core/eigen.hpp>  
#include <opencv2/opencv.hpp>  
using namespace std;
using namespace Eigen;
using namespace cv;
Mat OMP(Mat& dic, Mat& signal, int sparsity);
struct mats
{
	Mat image;
};
struct Rec
{
	Point2i tl;
	Point2i br;
};
int main(int argc, char* argv[])
{
	int num,size=30;
	float sd[225][30];
	Mat dic;
	Rec s_position[30];
	Mat signal = Mat_<float>(225, 1);
	int store_length;
	int op = 0;//target1
	int pp = 1;//target2
	int qp = 2;//target3
	float min;
	float fmin_o, fmin_p, fmin_q;
	int i, j;
	for (int num = 100; num <150; num++)
	{

		string r_path, w_path;
		r_path = "E:\\original\\" + to_string(num) + ".jpg";//modify your path here
		Mat cimg = imread(r_path);
		Mat img = imread(r_path, CV_LOAD_IMAGE_GRAYSCALE);
		HOGDescriptor hog;
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		vector<Rect> found, found_filtered;
		hog.detectMultiScale(img, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);	
		for (i = 0; i < found.size(); i++)
		{
			Rect r = found[i];
			for (j = 0; j < found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}
		Rec *dr = new Rec[size];
		int *fclass=new int[size];
		float *ff = new float[size];
		float *fres=new float[size];
		mats *Mats=new mats[size];
		///////////////////////////////////////////////////////
		cout << "frame:" << num << endl;
		///////////////////////////////////////////////////////
		for (i = 0; i < found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			Rect Crect = Rect(r.tl(), r.br());
			Mat Cimage;
			Cimage = img(Crect);
///////////////////////////////////////////////////////////////
			Mats[i].image = Cimage;
			dr[i].tl = r.tl();
			dr[i].br = r.br();
//////////////////////////////////////////////////////////////
			Mat gCimage;
			resize(Cimage, gCimage, Size(15, 15));
			if (num == 100)
			{
			
				MatrixXd object;
				cv2eigen(gCimage, object);
				ofstream ot;
				ot.open("col.txt");
				ot << object;
				ot.close();
				ifstream io;
				io.open("col.txt");
				for (int m = 0; m < 225; m++)
				{
					float a;
					io >> a;
					sd[m][i] = a;
				}
				io.close();
			}
			else
			{
				/////////////////////////////////////////////////////////////////////////////		
				MatrixXd sobject;
				cv2eigen(gCimage, sobject);
				ofstream ot;
				ot.open("signal.txt");
				ot << sobject;
				ot.close();
				ifstream it;
				it.open("signal.txt");
				for (int l = 0; l < 225; l++)
				{
					float a;
					it >> a;
					sd[l][i] = a;
					signal.row(l).col(0) = a;
				}
				it.close();
				/////////////////////////////////////////////////////////////////////////////
				Mat temp(1, dic.cols, CV_32F);  
				for (int it = 0; it < dic.cols; it++)
				{
					temp.col(it) = norm(dic.col(it));  
				}
				divide(dic, repeat(temp, dic.rows, 1), dic); 
				signal = signal / norm(signal);  
				Mat A = OMP(dic, signal, 2); 
				min = 100;
				for (int ip = 0; ip < store_length; ip++)
				{
					Mat B = Mat_<float>(store_length, 1);
					for (int j = 0; j < store_length; j++)
					{
						if (j == ip)
						{
							B.row(j).col(0) = A.row(j).col(0) + 0;
						}
						else
						{
							B.row(j).col(0) = 0;
						}
					}
					float res = (float)norm(signal - dic*B) / 2; 
					double d = d = sqrt((dr[i].br.x - s_position[ip].br.x)*(dr[i].br.x - s_position[ip].br.x)
						+ (dr[i].br.y - s_position[ip].br.y)*(dr[i].br.y - s_position[ip].br.y));
					if (res < min&&d<30)
					{
						min = res;
						fclass[i] = ip;
						ff[i] = res;
					}
				}
			}
		}
		fmin_o= 1, fmin_p = 1, fmin_q = 1;
///////////////////////////////////////////////////////////////////////////////
		string tpath0 = "E:\\0\\" + to_string(num) + ".jpg";//modify your path here
		string tpath1 = "E:\\1\\" + to_string(num) + ".jpg";//modify your path here
		string tpath2 = "E:\\2\\" + to_string(num) + ".jpg";//modify your path here
		if (num == 100)
		{
			imwrite(tpath0, Mats[op].image); rectangle(cimg, dr[op].tl, dr[op].br, Scalar(255, 0, 0), 3);
			imwrite(tpath1, Mats[pp].image); rectangle(cimg, dr[pp].tl, dr[pp].br, Scalar(0, 255, 0), 3);
			imwrite(tpath2, Mats[qp].image); rectangle(cimg, dr[qp].tl, dr[qp].br, Scalar(0, 0, 255), 3);
		}
		else
		{
			int fg,ig,kg;
			for (int g = 0; g < found_filtered.size(); g++)
			{
				if (fclass[g] == op)
				{
					if (ff[g] < fmin_o)
					{
						fmin_o = ff[g];
						fg = g;
					}
				}
				if (fclass[g] == pp)
				{
					if (ff[g] < fmin_p)
					{
						fmin_p = ff[g];
						ig = g;
					}
				}
				if (fclass[g] == qp)
				{
					if (ff[g] < fmin_q)
					{
						fmin_q= ff[g];
						kg = g;
					}
				}
			}
			imwrite(tpath0, Mats[fg].image); op = fg; rectangle(cimg, dr[fg].tl, dr[fg].br, Scalar(255, 0, 0), 3);//modify your path here
			imwrite(tpath1, Mats[ig].image); pp = ig; rectangle(cimg, dr[ig].tl, dr[ig].br, Scalar(0, 255, 0), 3);//modify your path here
			imwrite(tpath2, Mats[kg].image); qp = kg; rectangle(cimg, dr[kg].tl, dr[kg].br, Scalar(0, 0, 255), 3);//modify your path here
		}
		/////////////////////////////////////////////////////////////////////////////////
		dic = Mat_<float>(225, found_filtered.size());
		for (int p = 0; p <found_filtered.size(); p++)
		{
			for (int m = 0; m < 225; m++)
			{
				dic.row(m).col(p) = sd[m][p];
			}
		}
		/////////////////////////////////////////////////////////////////////////////////
		for (int count = 0; count < found_filtered.size(); count++)
		{
			s_position[count].tl = dr[count].tl;
			s_position[count].br = dr[count].br;
		}
		store_length = found_filtered.size();
		/////////////////////////////////////////////////////////////////////////////////
		delete ff;
		delete fclass;
		delete fres;
		delete []Mats;
		delete []dr;
		string fpath="E:\\Final1\\"+to_string(num)+".jpg";//modify your path here
		imwrite(fpath,cimg);
		/////////////////////////////////////////////////////////////////////////////////
	}
	system("pause");
	return 0;
}
Mat OMP(Mat& dic, Mat& signal, int sparsity)
{
	if (signal.cols>1)
	{
		cout << "wrong signal" << endl;
		exit(-1);
	}
	vector<int> selectedAtomOrder;   
	Mat coef(dic.cols, 1, CV_32F, Scalar::all(0));    
	Mat residual = signal.clone();  
	Mat indx(0, 1, CV_32F);
	Mat phi;
	float max_coefficient;
	unsigned int atomOrder; 
	for (;;)
	{
		max_coefficient = 0;
		for (int i = 0; i <dic.cols; i++)
		{
			float coefficient = (float)dic.col(i).dot(residual);

			if (abs(coefficient) > abs(max_coefficient))
			{
				max_coefficient = coefficient;
				atomOrder = i;
			}
		}
		selectedAtomOrder.push_back(atomOrder);         
		Mat& temp_atom = dic.col(atomOrder); 
		if (phi.cols == 0)
			phi = temp_atom;
		else
			hconcat(phi, temp_atom, phi); 
		indx.push_back(0.0f);    
		solve(phi, signal, indx, DECOMP_SVD); 
		residual = signal - phi*indx;  
		float res_norm = (float)norm(residual);
		if (indx.rows >= sparsity || res_norm <= 1e-6) 
		{
			for (int k = 0; k < selectedAtomOrder.size(); k++)
			{
				coef.row(selectedAtomOrder[k]).setTo(indx.row(k));  
			}
			return coef;
		}
	}
}


