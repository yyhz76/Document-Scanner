#include "DocumentScanner.h"

DocumentScanner::DocumentScanner(const Mat& image, const string& winName, int w, int h) : _output_width(w), _output_height(h)
{
	resetState();
	_outputImg.create(_output_width, _output_height, CV_8UC3);

	_img = image;
	_mask = Mat(_img.size(), CV_8UC1, Scalar(0));
	_imgCopy = _img.clone();
	_maskCopy = _mask.clone();
	_winName = winName;
	int width = _img.cols;
	int height = _img.rows;
	int leftX = width / 4;
	int rightX = width / 4 * 3;
	int upperY = height / 4;
	int lowerY = height / 4 * 3;
	_ptUpperLeft = Point(leftX, upperY);
	_ptUpperRight = Point(rightX, upperY);
	_ptLowerRight = Point(rightX, lowerY);
	_ptLowerLeft = Point(leftX, lowerY);
	_ptRadius = width / 150;
	_pts = vector<Point>{ _ptUpperLeft, _ptUpperRight, _ptLowerRight, _ptLowerLeft };
	
	drawRegion();
	cv::setMouseCallback(_winName, onMouse, this);
	imshow(_winName, _img);
	waitKey();
}

// reset all 4 vertices of the region to inactive
void DocumentScanner::resetState() {
	_ptUpperLeftState = State::INACTIVE;
	_ptLowerLeftState = State::INACTIVE;
	_ptLowerRightState = State::INACTIVE;
	_ptUpperRightState = State::INACTIVE;
}

// set one vertex to active for moving the vertex around
void DocumentScanner::setState(PointName name) {
	resetState();
	if (name == PointName::UPPERLEFT) {
		_ptUpperLeftState = State::ACTIVE;
	}
	else if (name == PointName::UPPERRIGHT) {
		_ptUpperRightState = State::ACTIVE;
	}
	else if (name == PointName::LOWERLEFT) {
		_ptLowerLeftState = State::ACTIVE;
	}
	else if (name == PointName::LOWERRIGHT) {
		_ptLowerRightState = State::ACTIVE;
	}
}

// draw the region enclosed by the 4 vertices
void DocumentScanner::drawRegion() {
	_imgCopy.copyTo(_img);
	circle(_img, _ptUpperLeft, _ptRadius, Scalar(50, 255, 100), -1);
	circle(_img, _ptLowerLeft, _ptRadius, Scalar(50, 255, 100), -1);
	circle(_img, _ptLowerRight, _ptRadius, Scalar(50, 255, 100), -1);
	circle(_img, _ptUpperRight, _ptRadius, Scalar(50, 255, 100), -1);
	polylines(_img, _pts, true, Scalar(50, 255, 100), 4);
}

// set the mask for extracting the region of interest (i.e. the region of the document)
void DocumentScanner::setMask() {
	_vpts.clear();
	_vpts.push_back(_pts);
	_maskCopy.copyTo(_mask);
	fillPoly(_mask, _vpts, Scalar(1));
}

// rectify the document to front-facing view
void DocumentScanner::rectifyImage() {
	const vector<Point> outputVertices{ Point(0, 0), Point(_output_width, 0), Point(_output_width, _output_height), Point(0, _output_height) };
	Mat h = findHomography(_pts, outputVertices);
	Mat roi;
	_imgCopy.copyTo(roi, _mask);
	warpPerspective(roi, _outputImg, h, Size(_output_width, _output_height));
}

void DocumentScanner::saveOutput() {
	imwrite("result.jpg", _outputImg);
	namedWindow("result", WINDOW_AUTOSIZE);
	imshow("result", _outputImg);
	waitKey();
}

// the callback function interacting with the mouse, including drawing / warping document region
void DocumentScanner::onMouse(int event, int x, int y, int flags, void* userData) {
	DocumentScanner* pThis = static_cast<DocumentScanner*>(userData);
	Point p(x, y);

	switch (event) {
		case EVENT_LBUTTONDOWN: {
			if (euclideanDist(p, pThis->_ptUpperLeft) < 2 * static_cast<double>(pThis->_ptRadius)) {
				pThis->_ptUpperLeft = p;
				pThis->setState(PointName::UPPERLEFT);
			}
			else if (euclideanDist(p, pThis->_ptLowerLeft) < 2 * static_cast<double>(pThis->_ptRadius)) {
				pThis->_ptLowerLeft = p;
				pThis->setState(PointName::LOWERLEFT);
			}
			else if (euclideanDist(p, pThis->_ptLowerRight) < 2 * static_cast<double>(pThis->_ptRadius)) {
				pThis->_ptLowerRight = p;
				pThis->setState(PointName::LOWERRIGHT);
			}
			else if (euclideanDist(p, pThis->_ptUpperRight) < 2 * static_cast<double>(pThis->_ptRadius)) {
				pThis->_ptUpperRight = p;
				pThis->setState(PointName::UPPERRIGHT);
			}
		}
		break;
		case EVENT_MOUSEMOVE:
		{
			if ((flags & EVENT_FLAG_LBUTTON) && pThis->_ptUpperLeftState == State::ACTIVE) {
				pThis->_ptUpperLeft = p;
				pThis->_pts[0] = p;
			}
			else if ((flags & EVENT_FLAG_LBUTTON) && pThis->_ptLowerLeftState == State::ACTIVE) {
				pThis->_ptLowerLeft = p;
				pThis->_pts[3] = p;
			}
			else if ((flags & EVENT_FLAG_LBUTTON) && pThis->_ptLowerRightState == State::ACTIVE) {
				pThis->_ptLowerRight = p;
				pThis->_pts[2] = p;
			}
			else if ((flags & EVENT_FLAG_LBUTTON) && pThis->_ptUpperRightState == State::ACTIVE) {
				pThis->_ptUpperRight = p;
				pThis->_pts[1] = p;
			}
			pThis->drawRegion();
			pThis->setMask();
			imshow(pThis->_winName, pThis->_img);
		}
		break;
		case EVENT_LBUTTONUP:
		{
			pThis->resetState();
		}
		break;
		case EVENT_RBUTTONUP:
		{
			pThis->rectifyImage();
			pThis->saveOutput();
		}
		break;
	}
}

double euclideanDist(const Point& p, const Point& q) {
	Point diff = p - q;
	return cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void help()
{
	cout << "========================================================================"
		"\n Interactive Document Scanner.\n"
		"\nUSAGE :\n"
		"\t./docScan <filename>\n"
		"README FIRST :\n"
		"\tDrag the four points and select the area around the document you want to scan.\n"
		"\tThen right-click to segment the document.\n"
		"=======================================================================" << endl;
}