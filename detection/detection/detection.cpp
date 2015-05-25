#include <iostream>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include "IntImg_d.h"
#include "Soldier_d.h"
#include <conio.h>


using namespace cv;
using namespace std;

#define FACTOR 1.25
#define SHIFT 1.5

Mat SrcImg, SrcImg_gray;
int stageNum;
vector<double> th_stage;
vector<vector<Soldier>> s; 
vector<int> s_num;
IntImg img, img_square;
FILE *output;
vector<Point> pt;
Mat OutImg_click;
int scan_flag;

void click_judge()
{
	int subWindow_x1 = pt[0].x, subWindow_y1 = pt[0].y, subWindow_x2 = pt[1].x, subWindow_y2 = pt[1].y;
	int area = (subWindow_x2 - subWindow_x1 + 1) * (subWindow_y2 - subWindow_y1 + 1);
	long double sigma_square = (long double)img_square.data[subWindow_y2 - 1][subWindow_x2 - 1] / area - ((long double)img.data[subWindow_y2 - 1][subWindow_x2 - 1] / area) * ((long double)img.data[subWindow_y2 - 1][subWindow_x2 - 1] / area);
	double sigma;
	bool isFace = FALSE;

	if (sigma_square < 0)
		sigma_square *= -1;
	sigma = sqrt(sigma_square);
	if (sigma < 1e-15 && sigma > -1e-15){
		printf("(%d, %d) to (%d, %d)'s sigma == 0.\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2);
	}
	else{
		double scale = (double)(subWindow_x2 - subWindow_x1 + 1) / 24;
		for (int i = 0; i < stageNum; i++)
			for (int j = 0; j < s_num[i]; j++)
				s[i][j].setScale(scale);

		for (int i = 0; i < stageNum; i++){
			double sum = 0;

			for (int j = 0; j < s_num[i]; j++){
				double alpha = log((1 - s[i][j].getE()) / s[i][j].getE());

				s[i][j].setPosition(subWindow_x1, subWindow_y1, sigma);
				sum += alpha * s[i][j].judge(&img);
			}
			if (sum >= th_stage[i])
				isFace = TRUE;
			else{
				isFace = FALSE;
				break;
			}
		}
		if (isFace){
			Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);
			rectangle(OutImg_click, rect, Scalar(255, 0, 0));
			imshow("Output_Click", OutImg_click);
			waitKey(1);
		}
	}

	printf("\n(%d, %d) to (%d, %d)\narea = %d * %d = %d\nsigma = %lf\nisFace = %d\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2, (subWindow_x2 - subWindow_x1 + 1), (subWindow_y2 - subWindow_y1 + 1), (subWindow_x2 - subWindow_x1 + 1) * (subWindow_y2 - subWindow_y1 + 1), sigma, isFace);
	return;
}

void on_mouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		switch (pt.size())
		{
		case 0:
			pt.push_back(Point(x, y));
			circle(OutImg_click, pt[pt.size() - 1], 5, CV_RGB(255, 0, 0), -1);
			imshow("Output_Click", OutImg_click);
			break;
		case 1:
			if (x <= pt[0].x || y <= pt[0].y)
				break;
			if (x - pt[0].x < y - pt[0].y)
				y = pt[0].y + x - pt[0].x;
			else
				x = pt[0].x + y - pt[0].y;
			pt.push_back(Point(x, y));		
			circle(OutImg_click, pt[pt.size() - 1], 5, CV_RGB(255, 0, 0), -1);
			imshow("Output_Click", OutImg_click);
			waitKey(1);
			click_judge();
			break;
		case 2:
			pt.clear();
			OutImg_click = SrcImg.clone();
			imshow("Output_Click", OutImg_click);
			break;
		}
	}
	return;
}

void click()
{
	system("CLS");
	cout << "�Хηƹ��I��subWindow�����W�Υk�U�I�y��" << endl;
	pt.clear();
	OutImg_click = SrcImg.clone();
	imshow("Output_Click", OutImg_click);
	setMouseCallback("Output_Click", on_mouse, 0);
	waitKey(0);
	return;
}

void scan()
{
	system("CLS");

	Mat OutImg = SrcImg.clone();

	double scale = 1, move = SHIFT;
	double sigma;
	int subWindow_x1 = 0, subWindow_y1 = 0, subWindow_x2 = 23, subWindow_y2 = 23;
	bool isFace = FALSE;

	for (int c = 0; c < 10; c++, scale *= FACTOR, move *= FACTOR){
		subWindow_x2 *= FACTOR;
		subWindow_y2 *= FACTOR;

		for (int i = 0; i < stageNum; i++)
			for (int j = 0; j < s_num[i]; j++)
				s[i][j].setScale(scale);

		int tmpy2 = subWindow_y2, tmpx2 = subWindow_x2;
		int FrameSum = 0, P = 0;
		vector<int> FrameStage, Ps;
		FrameStage.resize(stageNum);
		Ps.resize(stageNum);
		while (subWindow_y2 < SrcImg_gray.rows){
			while (subWindow_x2 < SrcImg_gray.cols){
				int area = (subWindow_x2 - subWindow_x1 + 1) * (subWindow_y2 - subWindow_y1 + 1);
				long double sigma_square = (long double)img_square.data[subWindow_y2 - 1][subWindow_x2 - 1] / area - ((long double)img.data[subWindow_y2 - 1][subWindow_x2 - 1] / area) * ((long double)img.data[subWindow_y2 - 1][subWindow_x2 - 1] / area);

				if (sigma_square < 0)
					sigma_square *= -1;
				sigma = sqrt(sigma_square);
				if (sigma < 1e-15 && sigma > -1e-15){
					printf("(%d, %d) to (%d, %d)'s sigma == 0.\n", subWindow_x1, subWindow_y1, subWindow_x2, subWindow_y2);
				}
				else{
					for (int i = 0; i < stageNum; i++){
						double sum = 0;

						for (int j = 0; j < s_num[i]; j++){
							double alpha = log((1 - s[i][j].getE()) / s[i][j].getE());

							s[i][j].setPosition(subWindow_x1, subWindow_y1, sigma);
							sum += alpha * s[i][j].judge(&img);
						}
						FrameStage[i]++;
						if (sum >= th_stage[i]){
							isFace = TRUE;
							Ps[i]++;
						}
						else{
							isFace = FALSE;
							break;
						}
					}
					if (isFace){
						Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);
						rectangle(OutImg, rect, Scalar(255, 0, 0));
						isFace = FALSE;
						P++;
					}
				}
				FrameSum++;

				if (scan_flag){
					Mat tmp = OutImg.clone();
					Rect rect = Rect(subWindow_x1, subWindow_y1, subWindow_x2 - subWindow_x1 + 1, subWindow_y2 - subWindow_y1 + 1);

					rectangle(tmp, rect, Scalar(255, 0, 0));
					imshow("Output_Scan", tmp);
					waitKey(1);
				}

				subWindow_x1 += move;
				subWindow_x2 += move;
			}
			subWindow_x1 = 0;
			subWindow_x2 = tmpx2;
			subWindow_y1 += move;
			subWindow_y2 += move;
		}
		subWindow_x1 = 0;
		subWindow_x2 = tmpx2;
		subWindow_y1 = 0;
		subWindow_y2 = tmpy2;

		printf("Scale: %lf, FrameSum: %d, P: %d, N: %d, PFrate: %lf\n", scale, FrameSum, P, FrameSum - P, (double)P / FrameSum);
		fprintf_s(output, "Scale: %lf, FrameSum: %d, P: %d, N: %d, PFrate: %lf\n", scale, FrameSum, P, FrameSum - P, (double)P / FrameSum);
		for (int i = 0; i < stageNum; i++){
			printf("Stage %d => Total: %d, Pass: %d, noPass: %d, PassRate: %lf\n", i + 1, FrameStage[i], Ps[i], FrameStage[i] - Ps[i], (double)Ps[i] / FrameStage[i]);
			fprintf_s(output, "Stage %d => Total: %d, Pass: %d, noPass: %d, PassRate: %lf\n", i + 1, FrameStage[i], Ps[i], FrameStage[i] - Ps[i], (double)Ps[i] / FrameStage[i]);
		}
		printf("\n");
		fprintf_s(output, "\n");
	}
	imshow("Output_Scan", OutImg);
	fclose(output);
	waitKey(0);
	return;
}

int main(void)
{
	system("CLS");

	FILE *soldier_cascade;
	int s_total = 0;

	fopen_s(&output, "detection_output.txt", "w");
	fopen_s(&soldier_cascade, "soldier_cascade.txt", "r");
	if (soldier_cascade == NULL){
		cout << "ERROR: can't find soldier_cascade.txt" << endl;
		return 0;
	}
	fscanf_s(soldier_cascade, "%d", &stageNum);
	s.resize(stageNum);
	s_num.resize(stageNum);
	th_stage.resize(stageNum);
	for (int i = 0; i < stageNum; i++){
		int x1, y1, x2, y2, type, p;
		double e;
		long double th;

		fscanf_s(soldier_cascade, "%d", &s_num[i]);
		s_total += s_num[i];
		s[i].resize(s_num[i]);
		for (int j = 0; j < s_num[i]; j++){
			fscanf_s(soldier_cascade, "%02d %02d %02d %02d %d %2d %8lf %lf", &x1, &y1, &x2, &y2, &type, &p, &th, &e);
			s[i][j] = Soldier(x1, y1, x2, y2, type, p, th, e);
		}
		fscanf_s(soldier_cascade, "%lf", &th_stage[i]);
	}
	fclose(soldier_cascade);

	unsigned char *p;

	SrcImg = imread("test.jpg");
	imshow("SrcImg", SrcImg);
	waitKey(1);
	cvtColor(SrcImg, SrcImg_gray, CV_RGB2GRAY);

	img = IntImg(SrcImg_gray.rows, SrcImg_gray.cols);
	img_square = IntImg(SrcImg_gray.rows, SrcImg_gray.cols);
	double sigma;

	for (int i = 0; i < SrcImg_gray.rows; i++){
		p = SrcImg_gray.ptr<unsigned char>(i);
		for (int j = 0; j < SrcImg_gray.cols; j++){
			if (i == 0 && j == 0){
				img.data[i][j] = p[j];
				img_square.data[i][j] = p[j] * p[j];
			}
			else if (i == 0){
				img.data[i][j] = p[j] + img.data[i][j - 1];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i][j - 1];
			}
			else if (j == 0){
				img.data[i][j] = p[j] + img.data[i - 1][j];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i - 1][j];
			}
			else{
				img.data[i][j] = p[j] + img.data[i][j - 1] + img.data[i - 1][j] - img.data[i - 1][j - 1];
				img_square.data[i][j] = p[j] * p[j] + img_square.data[i][j - 1] + img_square.data[i - 1][j] - img_square.data[i - 1][j - 1];
			}		
		}
	}
	
	sigma = sqrt((long double)img_square.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols) - ((long double)img.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols)) * ((long double)img.data[SrcImg_gray.rows - 1][SrcImg_gray.cols - 1] / (SrcImg_gray.rows * SrcImg_gray.cols)));
	if (sigma < 1e-15 && sigma > -1e-15){
		cout << "This image's sigma == 0." << endl;
		system("PAUSE");
		return 0;
	}

	while (1){
		scan_flag = 0;
		system("CLS");
		cout << "Stage number: " << stageNum << endl << "Total feature number: " << s_total << endl;
		cout << endl << "Menu: (please input 1 or 2 to run, and 3 to exit)" << endl;
		cout << "1. Scan mode" << endl;
		cout << "2. Click mode" << endl;
		int c;
		cin >> c;

		if (c == 3)
			break;
		else if (c == 1){
			cout << "�O�_�L�X�C��subWindow? (Y/N)\n";
			char ch;
			cin >> ch;
			if (ch == 'Y' || ch == 'y')
				scan_flag = 1;
			scan();
		}
		else if (c == 2){
			click();
		}
	}
	
	return 0;
}