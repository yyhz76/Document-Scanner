#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "DocumentScanner.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	// Reading file from command line
	cv::CommandLineParser parser(argc, argv, "{help h||}{@input||}");
	if (parser.has("help"))
	{
		help();
		return 0;
	}
	string filename = parser.get<string>("@input");

	if (filename.empty())
	{
		cout << "\n Empty filename \n" << endl;
		return EXIT_FAILURE;
	}

	Mat image = imread(filename);
	
	if (image.empty())
	{
		cout << "\n Couldn't read image filename " << filename << endl;
		return EXIT_FAILURE;
	}

	help();

	const string winName = "image";
	namedWindow(winName, WINDOW_AUTOSIZE);
	DocumentScanner app(image, winName);

	return 0;
}