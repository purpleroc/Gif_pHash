
#include <iostream>
#include <bitset>
#include <string>
#include <iomanip>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "phash.h"

using namespace std;
using namespace cv;

#define PI 3.1415926
#define hashLength 64

/*
	功能：获取DCT系数
	n：矩阵大小
	quotient: 系数
	quotientT: 系数转置
*/
void coefficient(const int &n, double **quotient, double **quotientT){
	double sqr = 1.0/sqrt(n+0.0);
	for(int i = 0; i < n; i++){
		quotient[0][i] = sqr;
		quotientT[i][0] =  sqr;
	}

	for(int i = 1; i < n; i++){
		for(int j = 0; j < n; j++){
			quotient[i][j] = sqrt(2.0/n)*cos(i*(j+0.5)*PI/n);  // 由公式得到
			quotientT[j][i] = quotient[i][j];
		}
	}

}
/*
	功能：两矩阵相乘
	A和B：源输入矩阵
	result：输出矩阵
*/
void matrixMultiply(double **A, double **B, int n, double **result){  
	double t = 0;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			t = 0;
			for(int k = 0; k < n; k++)
				t += A[i][k]*B[k][j];   
			result[i][j] = t;
		}
	}
}


void DCT(Mat_<uchar> image, const int &n, double **iMatrix){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			iMatrix[i][j] = (double)image(i,j);
		}
	}

	// 为系数分配空间
	double **quotient = new double*[n];
	double **quotientT = new double*[n];
	double **tmp = new double*[n];
	for(int i = 0; i < n; i++){
		quotient[i] = new double[n];
		quotientT[i] = new double[n]; 
		tmp[i] = new double[n];
	}
	// 计算系数矩阵
	coefficient(n, quotient, quotientT);
	matrixMultiply(quotient, iMatrix, n, tmp);  // 由公式成绩结果
	matrixMultiply(tmp, quotientT, n, iMatrix);

	for(int i = 0; i < n; i++){
		delete []tmp[i];
		delete []quotient[i];
		delete []quotientT[i];
	}
	delete []tmp;
	delete []quotient;
	delete []quotientT;
}

// 计算8*8图像的平均灰度
float calcAverage(double **iMatrix, const int &size){
	float sum = 0;
	for(int i = 0 ; i < size; i++){
		for(int j = 0; j < size; j++){
			sum += iMatrix[i][j];
		}
	}
	return sum/(size*size);
}

/* 计算hash值
	image:8*8的灰度图像
	size: 图像大小  8*8
	phash:存放64位hash值
	averagePix: 灰度值的平均值
*/
void fingerPrint(double **iMatrix, const int &size, char* phash, const float &averagePix){
	for(int i = 0; i < size; i++){
		int pos = i * size;
		for(int j = 0; j < size; j++){
			phash[pos+j] = iMatrix[i][j] >= averagePix ? '1':'0';
		}
	}
        //cout << endl;
}


void DCT(double **image, const int &n, double **iMatrix){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			iMatrix[i][j] = image[i][j];
		}
	}
	double **quotient = new double*[n];
	double **quotientT = new double*[n];
	double **tmp = new double*[n];
	for(int i = 0; i < n; i++){
		quotient[i] = new double[n];
		quotientT[i] = new double[n]; 
		tmp[i] = new double[n];
	}
	coefficient(n, quotient, quotientT);
	matrixMultiply(quotient, iMatrix, n, tmp);  // 由公式成绩结果
	matrixMultiply(tmp, quotientT, n, iMatrix);

	for(int i = 0; i < n; i++){
		delete []tmp[i];
		delete []quotient[i];
		delete []quotientT[i];
	}
	delete []tmp;
	delete []quotient;
	delete []quotientT;
}

int bin2int(char* bin){
    int i,sum;
    for(sum=i=0;bin[i];(sum*=2)+=bin[i++]-'0');
    return sum;
}

char* getphash(char *filename){
    int size   = 32;  // 图片缩放后大小
    int sim_re = 10;  // 感知哈希长度 sim_re^2
    double **iMatrix = new double*[size];
    for(int i = 0; i < size; i++)
        iMatrix[i] = new double[size];

    char *phash = (char*)malloc(sim_re*sim_re);
    memset(phash, '\0', sim_re*sim_re + 1);

    Mat img = imread(filename, 1);
    if(!img.data){
        cout << "the image is not exist" << endl;
        return NULL;
    }

    cvtColor(img, img, COLOR_BGR2GRAY);       // 灰度化
    resize(img, img, Size(size,size), 0,0, CV_INTER_CUBIC);      // 缩放到32*32

    DCT(img, size, iMatrix);   // 离散余弦变换
    DCT(iMatrix, size, iMatrix);


    double averagePix = calcAverage(iMatrix, sim_re);

    fingerPrint(iMatrix, sim_re, phash, averagePix);
    phash[sim_re*sim_re +1] = '\0';

    delete[] iMatrix;
    img.release();

    return phash;
}

char* getphashFormMem(IplImage* IplImg){
    int size   = 32;  // 图片缩放后大小
    int sim_re = 10;  // 感知哈希长度 sim_re^2
    double **iMatrix = new double*[size];
    for(int i = 0; i < size; i++)
        iMatrix[i] = new double[size];

    char *phash = (char*)malloc(sim_re*sim_re);
    memset(phash, '\0', sim_re*sim_re + 1);

    Mat img(IplImg, true);
    //cout << img << endl;
    //flip(img, img, 0);   // 反转 

    if(!img.data){
         cout << "the image is not exist" << endl;
         return NULL;
    }

    cvtColor(img, img, COLOR_BGR2GRAY);       // 灰度化
    resize(img, img, Size(size,size), 0,0, CV_INTER_CUBIC);      // 缩放到32*32

    DCT(img, size, iMatrix);   // 离散余弦变换
    DCT(iMatrix, size, iMatrix);

    double averagePix = calcAverage(iMatrix, sim_re);

    fingerPrint(iMatrix, sim_re, phash, averagePix);
    phash[sim_re*sim_re +1] = '\0';

    delete[] iMatrix;
    cvReleaseImage(&IplImg);
    img.release();

    return phash;
}
