#pragma once

#include <opencv2/opencv.hpp> 
#include <vector>
#include <string>

using namespace cv;
using namespace std;

class DocumentScanner {
public:
	DocumentScanner(const Mat& image, const string& winName, int w = 500, int h = 707);

private:
	enum class State { INACTIVE, ACTIVE };
	enum class PointName { UPPERLEFT, UPPERRIGHT, LOWERLEFT, LOWERRIGHT };

	void resetState();
	void setState(PointName name);
	void drawRegion();
	void setMask();
	void rectifyImage();
	void saveOutput();
	static void onMouse(int event, int x, int y, int flags, void* userData);
	
	State _ptUpperLeftState, _ptUpperRightState, _ptLowerLeftState, _ptLowerRightState;
	const int _output_width;
	const int _output_height;
	Mat _img;
	Mat _imgCopy;
	Mat _mask;
	Mat _maskCopy;
	Mat _outputImg;
	string _winName;
	Point _ptUpperLeft;
	Point _ptUpperRight;
	Point _ptLowerRight;
	Point _ptLowerLeft;
	int _ptRadius;
	vector<Point> _pts;
	vector<vector<Point>> _vpts;
};

double euclideanDist(const Point& p, const Point& q);
void help();
