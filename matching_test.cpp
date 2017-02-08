#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

std::vector<std::vector<Point>> TwoD_points(64, std::vector<Point>());
vector<vector<Point>> matching_pt = TwoD_points;
std::vector<std::vector<Point>> contours(32, std::vector<Point>());
vector<vector<vector<Point>>> contour_matching(32, vector<vector<Point>>(1200, vector<Point>()));
int ch = 10;
int vlp_2d_max_X = 0; int vlp_2d_min_X = INT_MAX;
int vlp_2d_max_Y = 0; int vlp_2d_min_Y = INT_MAX;
double f_width=0;
int min_a; int min_b;
Point before_a, before_b, after_a, after_b;
void pointcloud();
void MyMouse(int event, int x, int y, int flag, void* param);
void check_keypoint_minmax();
void scale_init(int minX, int maxX, int minY, int maxY, int width, int height);
void scale_init_re(int minX, int maxX, int minY, int maxY, int width, int height);
Mat draw_keypoint(Mat image);
void start_point(int minX, int minY, int x, int y);
void matching_start(int minX, int minY, int s_width, int s_height, int width, int height, Mat image, Mat m_image);
double calculate_cost(int x, int y);
double round(double value);
clock_t begin_t, end_t;

void main()
{
	int vlp_x=0, vlp_y=0, vlp_width=0, vlp_height=0;
	Mat matching_image = imread("images/human.png");
	Mat image_c = matching_image.clone();
	cvtColor(image_c, image_c, CV_BGR2GRAY);
	Mat Point_image = imread("images/points.png");
	threshold(image_c, image_c, 0, 240, CV_THRESH_BINARY_INV);

	findContours(image_c, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(matching_image, contours, -1, Scalar(0, 0, 255), 2);	
	Mat m_image = matching_image.clone();
	Rect r = boundingRect(Mat(contours[0]));
	//rectangle(matching_image, r, Scalar(0, 255, 0), 2);
	
	cout << "contours width : " << r.width << "   contours height : " << r.height << endl;

	for (int a = 0; a <= 0; a++)
	{
		sort(contours[a].begin(), contours[a].end(), [](const Point &a, Point &b) {
			return a.y < b.y;
		});
		for (int b = 0; b < contours[a].size(); b++)
		{
			contour_matching[a][contours[a][b].y].push_back(contours[a][b]);
		}	
	}

	for (int a = 0; a <= 0; a++)
	{
		for (int b = 0; b < contour_matching[a].size(); b++)
		{
			sort(contour_matching[a][b].begin(), contour_matching[a][b].end(), [](const Point &a, Point &b) {
				return a.x < b.x;
			});
		}
	}

	//for (int a = 0; a <= 0; a++)
	//{
	//	for (int b = 0; b < contours[a].size(); b++)
	//	{
	//		cout << "contours[" << a << "]" << "[" << b << "]" << " : " << contours[a][b] << endl;
	//	}
	//}
	//
	//for (int a = 0; a <= 0; a++)
	//{
	//	for (int b = 0; b < 600; b++)
	//	{
	//		cout << "contour_matching[" << a << "]" << "[" << b << "]" << " : " << contour_matching[a][b] << endl;
	//	}
	//}
	pointcloud();
	
	while(1)
	{	
		//setMouseCallback("point", MyMouse, (void*)&Point_image);	
		
		//imshow("original", image);
		//imshow("contour", image_c);
		
		imshow("point", Point_image);

		bool bKey = false;
		waitKey(0);
		int nKey;
		while (!bKey)
		{
			imshow("matching image", matching_image);
			nKey = waitKey(0);
			switch (nKey)
			{
			case 'q':
				for (int a = 0; a <= ch; a++)
				{
					for (int b = 0; b < TwoD_points[a].size(); b++)
					{
						cout << "TwoD_points[" << a << "]" << "[" << b << "]" << " : " << TwoD_points[a][b] << endl;
					}
				}
				draw_keypoint(matching_image);
				break;
			case 'w':
				matching_image = m_image;
				cout << "matching start" << endl;
				check_keypoint_minmax();
				before_a = Point(vlp_2d_min_X, vlp_2d_min_Y); before_b = Point(vlp_2d_max_X, vlp_2d_max_Y);
				cout << "vlp keypoint image x min : " << vlp_2d_min_X << " x max : " << vlp_2d_max_X << " y min : " << vlp_2d_min_Y << " y max : " << vlp_2d_max_Y << endl;
				scale_init(vlp_2d_min_X, vlp_2d_max_X, vlp_2d_min_Y, vlp_2d_max_Y, r.width, r.height);
				check_keypoint_minmax();
				scale_init_re(vlp_2d_min_X, vlp_2d_max_X, vlp_2d_min_Y, vlp_2d_max_Y, r.width, r.height);
				vlp_x = vlp_2d_min_X; vlp_y = vlp_2d_min_Y; vlp_width = vlp_2d_max_X - vlp_2d_min_X; vlp_height = vlp_2d_max_Y - vlp_2d_min_Y;
				draw_keypoint(matching_image);
				break;
			case 'e':
				check_keypoint_minmax();
				cout << "scaled vlp keypoint image x min : " << vlp_2d_min_X << " x max : " << vlp_2d_max_X << " y min : " << vlp_2d_min_Y << " y max : " << vlp_2d_max_Y << endl;
				break;
			case 'r':
				cout << "draw" << endl;
				draw_keypoint(matching_image);
				break;
			case 'a':
				matching_image = imread("images/human.png");
				cout << "move to start point" << endl;
				start_point(vlp_2d_min_X, vlp_2d_min_Y, r.x, r.y);
				draw_keypoint(matching_image);
				break;
			case 's':
				bKey = true;
				matching_image = imread("images/human.png");
				cout << "start matching" << endl;
				begin_t = clock();
				matching_start(r.x, r.y, vlp_width, vlp_height, r.width, r.height, matching_image, m_image);
				end_t = clock();
				cout << "Processing time(sec) : " << (double)(end_t - begin_t) / CLOCKS_PER_SEC << endl;
				break;
			case 'c':
				cout << "erase" << endl;
				matching_image = imread("images/human.png");
				break;
			
			}
		}

		for (int x = 0; x <= ch; x++)
		{
			for (int y = 0; y < matching_pt[x].size(); y++)
			{
				circle(matching_image, Point(matching_pt[x][y].x + min_a, matching_pt[x][y].y + min_b), 2, Scalar(0, 0, 255), -1);
			}
		}

		imshow("matching image", matching_image);
		waitKey(1);
	}	
}

void pointcloud()
{
	TwoD_points[0].push_back(Point(44, 316));
	TwoD_points[0].push_back(Point(65, 316));
	TwoD_points[0].push_back(Point(119, 316));
	TwoD_points[0].push_back(Point(156, 316));
	TwoD_points[1].push_back(Point(43, 288));
	TwoD_points[1].push_back(Point(90, 288));
	TwoD_points[1].push_back(Point(107, 288));
	TwoD_points[1].push_back(Point(149, 288));
	TwoD_points[2].push_back(Point(65, 260));
	TwoD_points[2].push_back(Point(139, 260));
	TwoD_points[3].push_back(Point(92, 233));
	TwoD_points[3].push_back(Point(136, 233));
	TwoD_points[4].push_back(Point(87, 206));
	TwoD_points[4].push_back(Point(144, 205));
	TwoD_points[5].push_back(Point(51, 178));
	TwoD_points[5].push_back(Point(83, 178));
	TwoD_points[5].push_back(Point(139, 178));
	TwoD_points[6].push_back(Point(38, 150));
	TwoD_points[6].push_back(Point(80, 150));
	TwoD_points[6].push_back(Point(134, 150));
	TwoD_points[6].push_back(Point(148, 150));
	TwoD_points[6].push_back(Point(194, 150));
	TwoD_points[7].push_back(Point(56, 123));
	TwoD_points[7].push_back(Point(174, 123));
	TwoD_points[8].push_back(Point(75, 95));
	TwoD_points[8].push_back(Point(132, 95));
	TwoD_points[9].push_back(Point(83, 68));
	TwoD_points[9].push_back(Point(116, 68));
	TwoD_points[10].push_back(Point(70, 40));
	TwoD_points[10].push_back(Point(132, 40));
}

void MyMouse(int event, int x, int y, int flag, void* param)
{
	switch (event){

	case CV_EVENT_LBUTTONDOWN:
		cout << "Left button down X = " << x << ", Y = " << y << endl;
		TwoD_points[ch].push_back(Point(x, y));
		break;
	case CV_EVENT_RBUTTONDOWN:
		ch++;
		cout << "ch : " << ch << endl;
		break;
	}
}

void check_keypoint_minmax()
{
	vlp_2d_max_X = 0; vlp_2d_min_X = INT_MAX;
	vlp_2d_max_Y = 0; vlp_2d_min_Y = INT_MAX;

	for (int a = 0; a <= ch; a++)
	{
		for (int b = 0; b < TwoD_points[a].size(); b++)
		{
			if (a == 0)
			{
				if (TwoD_points[a][b].y > vlp_2d_max_Y)
				{
					vlp_2d_max_Y = TwoD_points[a][b].y;
				}
			}
			if (a == ch)
			{
				if (TwoD_points[a][b].y < vlp_2d_min_Y)
				{
					vlp_2d_min_Y = TwoD_points[a][b].y;
				}
			}
			if (b == 0)
			{
				if (TwoD_points[a][b].x < vlp_2d_min_X)
				{
					vlp_2d_min_X = TwoD_points[a][b].x;
				}
				if (TwoD_points[a][b].x > vlp_2d_max_X)
				{
					vlp_2d_max_X = TwoD_points[a][b].x;
				}
			}
			if (b == TwoD_points[a].size() - 1)
			{
				if (TwoD_points[a][b].x < vlp_2d_min_X)
				{
					vlp_2d_min_X = TwoD_points[a][b].x;
				}
				if (TwoD_points[a][b].x > vlp_2d_max_X)
				{
					vlp_2d_max_X = TwoD_points[a][b].x;
				}
			}
		}
	}
}

void scale_init(int minX, int maxX, int minY, int maxY, int width, int height)
{
	if ((maxX - minX) > width)
	{
		cout << "width scale adjusting..." << endl;
		for (int a = 0; a <= ch; a++)
		{
			for (int b = 0; b < TwoD_points[a].size(); b++)
			{
				TwoD_points[a][b].x = (TwoD_points[a][b].x - minX)*width / (maxX - minX) + minX;
				TwoD_points[a][b].y = (TwoD_points[a][b].y - minY)*width / (maxX - minX) + minY;
			}
		}
	}
}

void scale_init_re(int minX, int maxX, int minY, int maxY, int width, int height)
{
	if ((maxY - minY) > height)
	{
		cout << "height scale adjusting..." << endl;
		for (int a = 0; a <= ch; a++)
		{
			for (int b = 0; b < TwoD_points[a].size(); b++)
			{
				TwoD_points[a][b].x = (TwoD_points[a][b].x - minX)*height / (maxY - minY) + minX;
				TwoD_points[a][b].y = (TwoD_points[a][b].y - minY)*height / (maxY - minY) + minY;
			}
		}
	}
}

Mat draw_keypoint(Mat image)
{
	for (int a = 0; a <= ch; a++)
	{
		for (int b = 0; b < TwoD_points[a].size(); b++)
		{
			circle(image, TwoD_points[a][b], 3, Scalar(255, 0, 255), -1);
		}
	}
	return image;
}

void start_point(int minX, int minY, int x, int y)
{
	for (int a = 0; a <= ch; a++)
	{
		for (int b = 0; b < TwoD_points[a].size(); b++)
		{
			TwoD_points[a][b].x = (TwoD_points[a][b].x + (x - minX));
			TwoD_points[a][b].y = (TwoD_points[a][b].y + (y - minY));
		}
	}
}

void matching_start(int minX, int minY, int s_width, int s_height, int width, int height, Mat image, Mat m_image)
{
	double min_cost = INT_MAX;
	double temp_height = s_height; double temp_width = s_width;
	while (temp_width >= width * 0.97)
	{
		//scale adjusting	
		f_width = double(temp_width* (temp_height) / (s_height));
		temp_height -= 1; temp_width = round(f_width);
		for (int a = 0; a <= ch; a++)
		{
			for (int b = 0; b < TwoD_points[a].size(); b++)
			{
				TwoD_points[a][b].x = round((TwoD_points[a][b].x - minX)*(temp_height) / (s_height)+minX);
				TwoD_points[a][b].y = round((TwoD_points[a][b].y - minY)*(temp_height) / (s_height)+minY);
			}
		}
		
		//cout <<"  s_width : " << temp_width << "  width : " << width << endl;
		for (int y = minY, b=0; (y + temp_height) < (minY + height); y++,b++)
		{
			//cout << "y : " << y << "  s_height : " << temp_height << "  minY : " << minY << "  height : " << height << endl;
			for (int x = minX, a=0; (x + temp_width) <= (minX + width); x++,a++)
			{
				bool bEscKey = true;
				//waitKey(0);
				int nKey;
				while (!bEscKey)
				{
					nKey = waitKey(0);
					switch (nKey)
					{
						case 27:
							bEscKey = true;
							break;
					}
				}
				//cout << "a : " <<a<<"  b : "<<b<< endl;				
				image = imread("images/human.png");
				double cost = calculate_cost(a,b);
				//cout << "cost : " << cost << endl;
				if (cost < min_cost)
				{
					min_a = a; min_b = b;
					after_a = Point(x, y); after_b = Point(x + temp_width, y + temp_height);

					cout << "min cost : " << min_cost << endl;
					min_cost = cost;
					matching_pt = TwoD_points;
				}
				
				for (int x = 0; x <= ch; x++)
				{
					for (int y = 0; y < matching_pt[x].size(); y++)
					{
						circle(image, Point(matching_pt[x][y].x + min_a, matching_pt[x][y].y + min_b), 2, Scalar(0, 0, 255), -1);
					}
				}

				for (int x = 0; x <= ch; x++)
				{
					for (int y = 0; y < TwoD_points[x].size(); y++)
					{
						circle(image, Point(TwoD_points[x][y].x + a, TwoD_points[x][y].y + b), 3, Scalar(0, 255, 0), -1);
					}
				}
				imshow("matching image", image);
			}
			
		}
	}
	cout << endl << "    matching complate    " << endl;
	cout << "before scaling size : x_min : " << before_a.x << "  x_max : " << before_b.x << "  y_min : " << before_a.y << "  y_max : " << before_b.y << endl;
	cout << "after scaling size : x_min : " << after_a.x << "  x_max : " << after_b.x << "  y_min : " << after_a.y << "  y_max : " << after_b.y << endl;
	imshow("matching image", image);
}

double calculate_cost(int x, int y)
{
	double cost = 0; double dist = 0;
	for (int a = 0; a <= ch; a++)
	{
		if (cost >= 35)
		{
			//cout << "cost is too high" << endl;
			return cost;
		}
			
		if (TwoD_points[a].size() > 0)
		{
			dist = abs(TwoD_points[a][0].x + x - contour_matching[0][TwoD_points[a][0].y + y][0].x); +
				abs(TwoD_points[a][TwoD_points[a].size() - 1].x + x - contour_matching[0]
				[TwoD_points[a][TwoD_points[a].size() - 1].y + y][contour_matching[0][TwoD_points[a][TwoD_points[a].size() - 1].y + y].size() - 1].x);
			//cout << "dist : " << dist << endl;
			cost += dist;
		}
		else
			cout << "TwoD_points[a].size() < 0 " << endl;

	}
	return cost;
}

double round(double value)
{
	value += 0.5;
	value = floor(value); 
	return value;
}