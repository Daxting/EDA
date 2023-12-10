// 8/20 updated
///////////////basic implementation///////////////
//void visualization(string problem_file_location, string answer_file_location, bool DrawAxis, double Extremum[])
//parameter: problem_file_location places Problem.txt, answer_file_location places Silkscreen.txt
///////////////axis implementation////////////////
//if you want to show the axis according to the extremum, DrawAxis places "true", otherwise "false"
//if DrawAxis places "true", Extremum[] should be a matrix with 4 info
//first: xmax , second: xmin, third: ymax, fourth: ymin
//And the function will cout the intersection of two axis
///////////////draw implementation////////////////
//draw(any kind of data type), the color will be purple by default
//if you want to use other colors, places the color on the second parameter
//ex, draw(silkscreen[i], pc::blue);
//colors' list:  pc::white, pc::green, pc::yellow, pc::lightblue, pc::blue, pc::purple
//////////////////////////////////////////////////

#define _USE_MATH_DEFINES
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
using cv::Mat;
using cv::Scalar;
using cv::Point2d;

namespace pc {
	Mat img(5000, 5000, CV_8UC3, Scalar(0, 0, 0));
	int thickness = 6, cthickness = 7, scale = 200, translate = 7;
	int	sthickness = 4, scthickness = 5; //for the function "draw()", narrow the element!
	enum type {
		white, green, yellow, lightblue, blue, purple
	};
	vector<point> vector_point;
	vector<type> vector_point_type;
	vector<segment> vector_segment;
	vector<type> vector_segment_type;
	vector<Arc> vector_arc;
	vector<type> vector_arc_type;
	void visualization(string problem_file_location, string answer_file_location, bool DrawAxis, double Extremum[]);
	void read_problemfile(ifstream& infile, string item, Mat img, int s, int t);
	void read_answerfile(ifstream& ansfile, string item, Mat img, int s, int t);
	double getlim(string item);
	void swap(double& a, double& b);
	void getdata_and_draw(Mat img, string inputstr, int type, int s, int t);
	Scalar color(int type);
	void arc_angle(double x1, double y1, double x2, double y2, double xc, double yc, double radius, double& start, double& end);
	double to_degree(double angle);
	void scaling(Mat& img);
	void delete_outrange(double& num);
	Point2d transform(double x, double y, int s, int t);
	void draw_the_axis_E(Mat img, int s, int t, double Extremum[]);
	void draw_the_axis_O(Mat img, int s, int t, double Extremum[]);
	void pcout();
	void draw_total();


	void visualization(string problem_file_location, string answer_file_location, bool DrawAxis, double Extremum[]) {
		//the function is transform from the main function of "visualization.cpp"
		//the function should call together with the function "silkscreen_output()" in "io.h"
		string location, problem, answer; // 判別需要繪圖的檔案
		string item;
		double assemblygap, coppergap, silkscreenlen;
		ifstream infile, ansfile;
		infile.open(problem_file_location, std::ios::in);
		infile >> item;
		assemblygap = getlim(item);
		infile >> item;
		coppergap = getlim(item);
		infile >> item;
		silkscreenlen = getlim(item);
		read_problemfile(infile, item, img, scale, translate);
		ansfile.open(answer_file_location, std::ios::in);
		read_answerfile(ansfile, item, img, scale, translate);
		if (DrawAxis)
			draw_the_axis_E(img, scale, translate, Extremum);
		draw_total();
		pcout();
		//scaling(img);
		//imshow("assembly", img);
		//waitKey();
		return;
	}

	void pcout() {
		flip(img, img, 0);// to upsidedown the img ,let y+ up, x+ right (原本原點在畫布左上角)
		imwrite("visualpicture.jpg", img);
	}

	void read_problemfile(ifstream& infile, string item, Mat img, int s, int t)
	{
		/*read the assembly part in the infile*/
		infile >> item; // "assembly"
		infile >> item; // first line of assembly
		while (item != "copper") {
			if (item[0] == 'l') {
				item = item.substr(5); // delete "line,"
				getdata_and_draw(img, item, 1, s, t);
			}
			else if (item[0] == 'a') {
				item = item.substr(4); // delete "arc,"
				getdata_and_draw(img, item, 2, s, t);
			}
			if (infile.peek() == EOF)
				break;
			infile >> item;
		}
		/*read the copper part in the infile*/
		while (infile.peek() != EOF) {
			infile >> item;
			do {
				if (item[0] == 'l') {
					item = item.substr(5); // delete "line,"
					getdata_and_draw(img, item, 3, s, t);
				}
				else if (item[0] == 'a') {
					item = item.substr(4); // delete "arc,"
					getdata_and_draw(img, item, 4, s, t);
				}
			} while (infile >> item && item != "copper");
			// if item is null or is "copper", end the loop
		}
	}

	/*read the ansfile*/
	void read_answerfile(ifstream& ansfile, string item, Mat img, int s, int t)
	{
		ansfile >> item;  // "silkscreen" (at the top of the ansfile)
		while (ansfile.peek() != EOF) {
			ansfile >> item;
			do {
				if (item[0] == 'l') {
					item = item.substr(5); // delete "line,"
					getdata_and_draw(img, item, 5, s, t);
				}
				else if (item[0] == 'a') {
					item = item.substr(4); // delete "arc,"
					getdata_and_draw(img, item, 6, s, t);
				}
			} while (ansfile >> item && item != "silkscreen");
		}
	}

	/*get limit information*/
	double getlim(string item)
	{
		return stod(item.substr(item.find(",") + 1));
	}

	/*swap*/
	void swap(double& a, double& b)
	{
		double item = a;
		a = b;
		b = item;
	}

	void draw_the_axis_E(Mat img, int s, int t, double Extremum[]) {
		int type = 7, type5 = 8;
		double length = 0.1;
		Point2d p, q;
		int xmax = ceil(Extremum[0]);
		int xmin = floor(Extremum[1]);
		int ymax = ceil(Extremum[2]);
		int ymin = floor(Extremum[3]);
		p = transform(xmin - 1, ymin - 2, s, t), q = transform(xmin - 1, ymax + 2, s, t);
		line(img, p, q, color(type), thickness);
		p = transform(xmin - 2, ymin - 1, s, t), q = transform(xmax + 2, ymin - 1, s, t);
		line(img, p, q, color(type), thickness);
		cout << "Axis intersection: (" << xmin - 1 << "," << ymin - 1 << ")" << endl;
		for (int i = xmin - 2; i < xmax + 2; i++) {
			p = transform(i, ymin - 1 - length, s, t), q = transform(i, ymin - 1 + length, s, t);
			if (i % 5 != 0)
				line(img, p, q, color(type), thickness);
			else
				line(img, p, q, color(type5), thickness);
		}
		for (int i = ymin - 2; i < ymax + 2; i++) {
			p = transform(xmin - 1 - length, i, s, t), q = transform(xmin - 1 + length, i, s, t);
			if (i % 5 != 0)
				line(img, p, q, color(type), thickness);
			else
				line(img, p, q, color(type5), thickness);
		}
	}

	void draw_the_axis_O(Mat img, int s, int t, double Extremum[]) {
		int  type = 7, type5 = 8;
		Point2d p, q;
		int xmax = ceil(Extremum[0]);
		int xmin = floor(Extremum[1]);
		int ymax = ceil(Extremum[2]);
		int ymin = floor(Extremum[3]);
		// y axis
		p = transform(0, round(ymin - 2), s, t), q = transform(0, round(ymax + 2), s, t);
		line(img, p, q, color(type), thickness);
		for (int i = round(ymin - 2); i < ymax + 2; i++) {
			p = transform(0, i, s, t);
			if (i % 5 != 0)
				circle(img, p, thickness, color(type), thickness + 5);
			else
				circle(img, p, thickness, color(type5), thickness + 5);
		}
		// x axis
		p = transform(round(xmin - 2), 0, s, t), q = transform(round(xmax + 2), 0, s, t);
		line(img, p, q, color(type), thickness);
		for (int i = xmin - 2; i < xmax + 2; i++) {
			p = transform(i, 0, s, t);
			if (i % 5 != 0)
				circle(img, p, thickness, color(type), thickness + 5);
			else
				circle(img, p, thickness, color(type5), thickness + 5);
		}

	}

	Point2d transform(double x, double y, int s = scale, int t = translate) {
		Point2d p((x + t) * s, (y + t) * s);
		return p;
	}

	Scalar color(type ctype) //BGR
	{
		if (ctype == white)
			return Scalar(255, 255, 255);
		else if (ctype == green)
			return Scalar(0, 255, 0);
		else if (ctype == yellow)
			return Scalar(0, 255, 255);
		else if (ctype == lightblue)
			return Scalar(255, 100, 0);
		else if (ctype == blue)
			return Scalar(255, 255, 0);
		else if (ctype == purple)
			return Scalar(255, 0, 255);
	}

	void draw(point p, type color = purple) {
		vector_point.push_back(p);
		vector_point_type.push_back(color);
	}

	void draw(segment seg, type color = purple) {
		vector_segment.push_back(seg);
		vector_segment_type.push_back(color);
	}

	void draw(Arc arc, type color = purple) {
		vector_arc.push_back(arc);
		vector_arc_type.push_back(color);
	}

	void draw(element ele, type color = purple) {
		if (ele.is_arc == 0) {
			draw(ele.seg, color);
		}
		else {
			draw(ele.arc, color);
		}
	}

	void draw(component com, type color = purple) {
		for (int i = 0; i < com.material.size(); i++) {
			pc::draw(com.material.at(i), color);
		}
	}

	void draw_total() {
		//draw the element at last for being more eyecatching
		for (int i = 0; i < vector_point.size(); i++) {
			cv::circle(img, transform(vector_point[i].x, vector_point[i].y), sthickness, pc::color(vector_point_type[i]), scthickness);
		}
		for (int i = 0; i < vector_segment.size(); i++) {
			cv::circle(img, transform(vector_segment[i].first.x, vector_segment[i].first.y), sthickness, pc::color(vector_segment_type[i]), scthickness);
			cv::circle(img, transform(vector_segment[i].second.x, vector_segment[i].second.y), sthickness, pc::color(vector_segment_type[i]), scthickness);
			cv::line(img, transform(vector_segment[i].first.x, vector_segment[i].first.y), transform(vector_segment[i].second.x, vector_segment[i].second.y)
				, pc::color(vector_segment_type[i]), sthickness);
		}
		for (int i = 0; i < vector_arc.size(); i++) {
			if (vector_arc[i].p == vector_arc[i].c) {
				cv::circle(img, transform(vector_arc[i].c.x, vector_arc[i].c.y), sthickness, pc::color(vector_arc_type[i]), scthickness);
			}
			else if (vector_arc[i].p == vector_arc[i].q) {
				cv::circle(img, transform(vector_arc[i].p.x, vector_arc[i].p.y), sthickness, pc::color(vector_arc_type[i]), scthickness);
				double r = vector_arc[i].radius() * scale;
				cv::ellipse(img, transform(vector_arc[i].c.x, vector_arc[i].c.y), cv::Size(r, r), 0, 0, 360, pc::color(vector_arc_type[i]), sthickness);
			}
			else {
				cv::circle(img, transform(vector_arc[i].p.x, vector_arc[i].p.y), sthickness, pc::color(vector_arc_type[i]), scthickness);
				cv::circle(img, transform(vector_arc[i].q.x, vector_arc[i].q.y), sthickness, pc::color(vector_arc_type[i]), scthickness);
				double angle1 = vector_arc[i].start_angle(), angle2 = vector_arc[i].end_angle(), r = vector_arc[i].radius() * scale;
				if (!vector_arc[i].direction) // turn the arc into CCW
					std::swap(angle1, angle2);
				if (angle1 > angle2) // make angle1 the smaller angle
					angle1 -= 360;
				cv::ellipse(img, transform(vector_arc[i].c.x, vector_arc[i].c.y), cv::Size(r, r), 0, angle1, angle2, pc::color(vector_arc_type[i]), sthickness);
			}
		}
	}

	/*read the information in the file*/
	void getdata_and_draw(Mat img, string inputstr, int type, int s, int t)
	{
		//type:1=assline, 2=assarc, 3=copline, 4=coparc, 5=silkline, 6=silkarc

		// s:放大縮小比例, t:位移量(防止數據有負號無法顯示在畫布上)
		// thickness:線條粗細
		string sx1, sy1;
		double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		//對小數後四位四捨五入後恢復原值,但仍發現有儲存數據些微差距
		//因此後面仍需要對超出範圍的值做限制並直接改動(delete_outrange)
		if (type == 1 || type == 3 || type == 5) //line
		{
			x1 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x2 in inputstr
			y1 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y2 in inputstr
			x2 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x2 in inputstr
			y2 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y2 in inputstr
			Point2d p(x1, y1), q(x2, y2);
			circle(img, p, thickness, color(type), cthickness);
			circle(img, q, thickness, color(type), cthickness);
			line(img, p, q, color(type), thickness);
		}
		else if (type == 2 || type == 4 || type == 6) //arc
		{
			string sxc, syc;
			sx1 = inputstr.substr(0, inputstr.find(','));
			x1 = (round(stod(sx1) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x1 in inputstr
			sy1 = inputstr.substr(0, inputstr.find(','));
			y1 = (round(stod(sy1) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y1 in inputstr
			x2 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x2 in inputstr
			y2 = (round(stod(inputstr.substr(0, inputstr.find(','))) * 10000) / 10000 + t) * s;
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y2 in inputstr
			sxc = inputstr.substr(0, inputstr.find(','));
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete xc in inputstr
			syc = inputstr.substr(0, inputstr.find(','));
			inputstr = inputstr.substr(inputstr.find(",") + 1); // delete yc in inputstr
			Point2d p(x1, y1), q(x2, y2);

			// 用string比對的方式,若只是一個點,用circle繪製點
			if (sx1 == sxc && sy1 == syc)
			{
				circle(img, p, 1, color(type), thickness);
			}
			else
			{
				circle(img, p, thickness, color(type), cthickness);
				circle(img, q, thickness, color(type), cthickness);
				double xc, yc;
				xc = (round(stod(sxc) * 10000) / 10000 + t) * s;
				yc = (round(stod(syc) * 10000) / 10000 + t) * s;
				cv::Point c(xc, yc);

				// 因為計算角度是逆時針, 所以將順時針的資料swap
				if (inputstr == "CW") {
					swap(x1, x2);
					swap(y1, y2);
				}
				else;

				double radius = sqrt(pow((x1 - xc), 2) + pow((y1 - yc), 2));
				double startangle, endangle;
				arc_angle(x1, y1, x2, y2, xc, yc, radius, startangle, endangle);
				ellipse(img, c, cv::Size(radius, radius), 0, startangle, endangle, color(type), thickness);
			}
		}
	}

	/*color(數字依序為BGR,非RGB)*/
	Scalar color(int type)
	{
		//ass=>white(12), cop=>green(34), ans=>yellow(56)
		if (type == 1 || type == 2)
			return Scalar(255, 255, 255);
		else if (type == 3 || type == 4)
			return Scalar(0, 255, 0);
		else if (type == 5 || type == 6)
			return Scalar(0, 255, 255);
		else if (type == 7)
			return Scalar(255, 100, 0);
		else if (type == 8)
			return Scalar(255, 255, 0);
	}

	/*calculate the angle of arc*/
	void arc_angle(double x1, double y1, double x2, double y2, double xc, double yc, double radius, double& start, double& end)  //d=0,CW;d=1,CCW
	{
		//以acos算出arc起終點所在圓心角,
		double cos1 = (x1 - xc) / radius, cos2 = (x2 - xc) / radius;
		//將因為數據儲存誤差導致acos的參數有可能>1或<-1的情況去除
		//acos的domain為-1<=x<=1
		delete_outrange(cos1);
		delete_outrange(cos2);
		//因為acos的range為0<=y<=(pi/2)
		//所以除了以to_degree將角度換成畫arc函式所需的角度外
		//也將角度範圍轉為0<=起始角<360,0<結束角<=360
		start = (y1 >= yc) ? to_degree(acos(cos1)) : 360 - to_degree(acos(cos1));
		end = (y2 > yc) ? to_degree(acos(cos2)) : 360 - to_degree(acos(cos2));
		//確保起始角度永遠小於結束角度
		if (start >= end)
			start -= 360;
	}

	/*radian change to degree*/
	double to_degree(double angle)
	{
		return angle * 180 / M_PI;
	}

	/*scale the img*/
	void scaling(Mat& img)
	{
		//乘以一個2*3的矩陣
		double tx = img.cols / 4, ty = img.rows / 4; //放大比例
		double value[] = { 1, 0, tx, 0, 1, ty };
		Mat scaling_matrix = (cv::Mat_<double>(2, 3) << 1, 0, tx, 0, 1, ty);
		warpAffine(img, img, scaling_matrix, img.size());
	}

	/*使數據落在-1<=x<=1*/
	void delete_outrange(double& num)
	{
		if (num > 1)
			num = 1;
		else if (num < -1)
			num = -1;
		else
			num;
	}

};