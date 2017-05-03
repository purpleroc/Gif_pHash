#include <iostream>
#include <bitset>
#include <string>
#include <iomanip>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

char* getphash(char* filename);
char* getphashFormMem(IplImage* IplImg);
