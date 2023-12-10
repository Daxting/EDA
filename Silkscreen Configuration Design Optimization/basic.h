#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <time.h>
#include <algorithm>

using std::cin;
using std::cout;
using std::abs;
using std::endl;
using std::vector;
using std::string;
using std::ofstream;
using std::ifstream;
#define ERROR 1e-5
#define M_PI 3.14159265358979323846

namespace db {//for double
    inline bool equal(double a, double b) {
        if (a >= b - ERROR && a <= b + ERROR)
            return true;
        else
            return false;
    }

    inline bool less(double a, double b) {
        if (a < b - ERROR)
            return true;
        else
            return false;
    }

    inline bool greater(double a, double b) {
        if (a > b + ERROR)
            return true;
        else
            return false;
    }

    inline bool less_or_equal(double a, double b) {
        if (a <= b + ERROR)
            return true;
        else
            return false;
    }

    inline bool greater_or_equal(double a, double b) {
        if (a >= b - ERROR)
            return true;
        else
            return false;
    }
    string to_string_io(double d) {
        string str;
        bool positive = 1;
        if (d < 0) {
            positive = 0;
            d *= -1;
        }
        double temp = d * 10000;
        int n = round(temp);
        while (n != 0) {
            if (n % 10 == 0) {
                str += "0";
            }
            else if (n % 10 == 1) {
                str += "1";
            }
            else if (n % 10 == 2) {
                str += "2";
            }
            else if (n % 10 == 3) {
                str += "3";
            }
            else if (n % 10 == 4) {
                str += "4";
            }
            else if (n % 10 == 5) {
                str += "5";
            }
            else if (n % 10 == 6) {
                str += "6";
            }
            else if (n % 10 == 7) {
                str += "7";
            }
            else if (n % 10 == 8) {
                str += "8";
            }
            else if (n % 10 == 9) {
                str += "9";
            }
            n /= 10;
        }
        string ansstr;
        if (positive == 0)
            ansstr += "-";
        if (str.size() > 4) {
            for (int i = str.size() - 1; i >= 4; i--) {
                ansstr += str[i];
            }
            ansstr += ".";
            for (int i = 3; i >= 0; i--) {
                ansstr += str[i];
            }
        }
        else {
            ansstr += "0.";
            for (int i = 0; i < 4 - str.size(); i++) {
                ansstr += "0";
            }
            for (int i = str.size() - 1; i >= 0; i--) {
                ansstr += str[i];
            }
        }
        return ansstr;
    }
    inline double rounding(double x) {
        x = round(x * 10000.0);
        return x / 10000.0;
    }
}

class point {
public:
    double x;
    double y;
    point(double input_x = 0, double input_y = 0) {
        x = input_x;
        y = input_y;
    }
    bool collinear(point p, point q) {
        double cross_product = (p.y - y) * (q.x - p.x) - (p.x - x) * (q.y - p.y);
        return db::equal(cross_product, 0);
    }
    inline bool operator == (point p) {
        if (db::equal(x, p.x) && db::equal(y, p.y))
            return true;
        else
            return false;
    }
    inline bool operator != (point p) {
        if (!db::equal(x, p.x) || !db::equal(y, p.y))
            return true;
        else
            return false;
    }
    inline void operator = (point p) {
        x = p.x;
        y = p.y;
    }
    void swap(point& p) {
        point temp(x, y);
        x = p.x;
        y = p.y;
        p = temp;
    }
    double distance(point p) {
        return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
    }
    int orientation(point p, point q) {
        // To find orientation of ordered triplet (p, q, r), r is the point itself
        // The function returns following values
        // 0 --> p, q and r are collinear
        // 1 --> Clockwise
        // 2 --> Counterclockwise
        double val = (q.y - p.y) * (x - q.x) - (q.x - p.x) * (y - q.y);

        if (db::equal(val, 0)) return 0;  // collinear

        else if (db::greater(val, 0)) return 1;//  clock wise

        else if (db::less(val, 0)) return 2; //  counterclock wise
    }
};

struct point_data {
    point pt;
    bool type; // (0) extreme, (1) silk
    int number; // on which element
    int direction; // (0) CW, (1) CCW, (2) No
    double distance; // distance from the element's start point
    double angle; // angle from the element's start point
};

class segment {
public:
    point first;
    point second;
    segment(point input_first = (0, 0), point input_second = (0, 0)) {
        first = input_first;
        second = input_second;
    }
    bool collinear(point input_point) {
        double cross_product = (first.y - input_point.y) * (second.x - first.x) - (first.x - input_point.x) * (second.y - first.y);
        return db::equal(cross_product, 0);
    }
    bool collinear(segment input_seg) {
        if (collinear(input_seg.first) && collinear(input_seg.second))
            return true;
        else
            return false;
    }
    bool on_segment(point input_point) {
        if (collinear(input_point) &&
            db::less_or_equal(input_point.x, rightmostValue()) &&
            db::greater_or_equal(input_point.x, leftmostValue()) &&
            db::less_or_equal(input_point.y, ceilValue()) &&
            db::greater_or_equal(input_point.y, groundValue()))
            return true;
        return false;
    }
    bool intersects(segment input_seg) {
        // solutions quoted from gfg
        // Find the four orientations needed for general and special cases
        point p1 = first, q1 = second, p2 = input_seg.first, q2 = input_seg.second;
        int o1 = p2.orientation(p1, q1);
        int o2 = q2.orientation(p1, q1);
        int o3 = p1.orientation(p2, q2);
        int o4 = q1.orientation(p2, q2);

        // General case
        if (o1 != o2 && o3 != o4)
            return true;

        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && segment(p1, q1).on_segment(p2)) return true;

        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && segment(p1, q1).on_segment(q2)) return true;

        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && segment(p2, q2).on_segment(p1)) return true;

        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && segment(p2, q2).on_segment(q1)) return true;

        return false; // Doesn't fall in any of the above cases
    }
    double length() {
        return sqrt((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
    }
    point midpoint() {
        return point((first.x + second.x) / 2, (first.y + second.y) / 2);
    }
    double ceilValue() {
        if (first.y > second.y)
            return first.y;
        else
            return second.y;
    }
    double groundValue() {
        if (first.y < second.y)
            return first.y;
        else
            return second.y;
    }
    double rightmostValue() {
        if (first.x > second.x)
            return first.x;
        else
            return second.x;
    }
    double leftmostValue() {
        if (first.x < second.x)
            return first.x;
        else
            return second.x;
    }
    bool IsSameDirection(segment input_seg) {
        point vec1 = { input_seg.second.x - input_seg.first.x,  input_seg.second.y - input_seg.first.y };
        point vec2 = { second.x - first.x,  second.y - first.y };
        if (vec1.x * vec2.x + vec1.y * vec2.y > 0)
            return true;
        else
            return false;
    }
    inline bool operator == (segment input_seg) {
        if (first == input_seg.first && second == input_seg.second)
            return true;
        else
            return false;
    }
    inline bool operator != (segment input_seg) {
        if (first != input_seg.first || second != input_seg.second)
            return true;
        else
            return false;
    }
    inline void operator = (segment input_seg) {
        first = input_seg.first;
        second = input_seg.second;
    }
};

class Arc {
public:
    point p, q, c; // start, end, center
    bool direction; // (0) CW , (1) CCW
    Arc(point input_p = (0, 0), point input_q = (0, 0), point input_c = (0, 0), bool dir = 0) {
        p = input_p;
        q = input_q;
        c = input_c;
        direction = dir;
    }
    double radius() {
        return sqrt((p.x - c.x) * (p.x - c.x) + (p.y - c.y) * (p.y - c.y));
    }
    double start_angle() {
        return find_angle(p);
    }
    double end_angle() {
        return find_angle(q);
    }
    double length() {
        if (p == q) {
            return 2 * radius() * M_PI;
        }
        double portion = arc_angle() / 360;
        return radius() * 2 * M_PI * portion;
    }
    double ceilValue() {
        if (in_angle(90))
            return c.y + radius();
        if (p.y > q.y)
            return p.y;
        else
            return q.y;
    }
    double groundValue() {
        if (in_angle(270))
            return c.y - radius();
        if (p.y < q.y)
            return p.y;
        else
            return q.y;
    }
    double rightmostValue() {
        if (in_angle(0))
            return c.x + radius();
        if (p.x > q.x)
            return p.x;
        else
            return q.x;
    }
    double leftmostValue() {
        if (in_angle(180))
            return c.x - radius();
        if (p.x < q.x)
            return p.x;
        else
            return q.x;
    }
    bool in_angle(double input_angle) {
        if (p == q)
            return true;
        double angle1, angle2;
        angle1 = start_angle();
        angle2 = end_angle();
        if (!direction) {//cw
            if (angle1 > angle2) {
                if (input_angle > angle2 && input_angle < angle1)
                    return true;
                else
                    return false;
            }
            else if (angle2 > angle1) {
                if (input_angle<angle1 || input_angle>angle2)
                    return true;
                else
                    return false;
            }
        }
        else if (direction) {//ccw
            if (angle2 > angle1) {
                if (input_angle > angle1 && input_angle < angle2)
                    return true;
                else
                    return false;
            }
            else if (angle1 > angle2) {
                if (input_angle > angle1 || input_angle < angle2)
                    return true;
                else
                    return false;
            }
        }
    }
    double find_angle(point input_point) {
        double cos1 = (input_point.x - c.x) / c.distance(input_point);
        if (cos1 > 1)  cos1 = 1;
        else if (cos1 < -1)  cos1 = -1;
        if (input_point.y >= c.y) {
            return (acos(cos1)) * 180 / M_PI;
        }
        else {
            return  360 - (acos(cos1)) * 180 / M_PI;
        }
    }
    bool in_angle(point input_point) {
        if (p == q)
            return true;
        double angle1, angle2, angle3;
        angle1 = start_angle();
        angle2 = end_angle();
        angle3 = find_angle(input_point);
        return in_angle(angle3);
    }
    double arc_angle() {
        double angle1, angle2;
        angle1 = start_angle();
        angle2 = end_angle();
        if (direction) // turn the arc into CCW
            std::swap(angle1, angle2);
        if (angle1 > angle2) // make angle1 the smaller angle
            return angle1 - angle2;
        else
            return angle1 + 360 - angle2;
    }
    point angle_to_point(double angle) {
        double angle_in_rad = angle * M_PI / 180;
        return point(c.x + radius() * cos(angle_in_rad), c.y + radius() * sin(angle_in_rad));
    }
    bool in_across_angle(point input_point) {
        if (p == q)
            return true;
        double angle1, angle2, angle3;
        angle1 = start_angle();
        angle2 = end_angle();
        angle3 = find_angle(input_point);
        angle3 += 180;
        if (angle3 >= 360)
            angle3 -= 360;
        if (direction)
            std::swap(angle1, angle2);
        return in_angle(angle3);
    }
    point midpoint() {
        double x = 0, y = 0, r = radius();
        double a = q.x - p.x, b = q.y - p.y;
        if (a == 0 && b == 0 && p != c)
            return point(p.x + 2 * (c.x - p.x), p.y + 2 * (c.y - p.y));
        x = c.x + r * b / sqrt(a * a + b * b);
        y = c.y - r * a / sqrt(a * a + b * b);
        if (on_arc(point(x, y)))
            return point(x, y);
        else {
            x = c.x - r * b / sqrt(a * a + b * b);
            y = c.y + r * a / sqrt(a * a + b * b);
            return point(x, y);
        }
    }
    bool IsSameDirection(Arc input_arc) {
        if (input_arc.c != c)
            cout << "ERROR! the center is not consistent(IsSameDirection)" << endl;
        if (input_arc.direction == direction)
            return true;
        else
            return false;
    }
    bool on_arc(point input_point) {
        if (in_angle(input_point) && db::equal(c.distance(input_point), radius()))
            return true;
        else
            return false;
    }
    bool operator == (Arc input_arc) {
        if (p == input_arc.p && q == input_arc.q && c == input_arc.c && direction == input_arc.direction)
            return true;
        else
            return false;
    }
    bool operator != (Arc input_arc) {
        if (p != input_arc.p || q != input_arc.q || c != input_arc.c || direction != input_arc.direction)
            return true;
        else
            return false;
    }
    void operator = (Arc input_arc) {
        p = input_arc.p;
        q = input_arc.q;
        c = input_arc.c;
        direction = input_arc.direction;
    }
};

class element {
public:
    segment seg;
    Arc arc;
    bool is_arc;
    int numbers; //record the buffered element which is corresponded to assembly element
    element() {
        is_arc = 0;
        numbers = 0;
        seg = segment();
        arc = Arc();
    }
    // line constructor
    element(point p, point q) {
        is_arc = 0;
        numbers = 0;
        seg.first = p;
        seg.second = q;
    }
    // arc constructor
    element(point p, point q, point c, bool dir) {
        is_arc = 1;
        numbers = 0;
        arc.p = p;
        arc.q = q;
        arc.c = c;
        arc.direction = dir;
    }
    point getStartPoint() {
        if (is_arc)
            return arc.p;
        else
            return seg.first;
    }
    point getEndPoint() {
        if (is_arc)
            return arc.q;
        else
            return seg.second;
    }
    void reverseArcDirection() {
        if (is_arc) {
            arc.direction = abs(arc.direction - 1);
        }
    }
    void setStartPoint(point p) {
        if (is_arc)
            arc.p = p;
        else
            seg.first = p;
    }
    void setEndPoint(point p) {
        if (is_arc)
            arc.q = p;
        else
            seg.second = p;
    }
    double length() {
        if (is_arc)
            return arc.length();
        else
            return seg.length();
    }
    double ceilValue() {
        if (is_arc)
            return arc.ceilValue();
        else
            return seg.ceilValue();
    }
    double groundValue() {
        if (is_arc)
            return arc.groundValue();
        else
            return seg.groundValue();
    }
    double rightmostValue() {
        if (is_arc)
            return arc.rightmostValue();
        else
            return seg.rightmostValue();
    }
    double leftmostValue() {
        if (is_arc)
            return arc.leftmostValue();
        else
            return seg.leftmostValue();
    }
    string info() {
        string str;
        if (is_arc) {
            str = "arc," + db::to_string_io(arc.p.x) + "," + db::to_string_io(arc.p.y) + ","
                + db::to_string_io(arc.q.x) + "," + db::to_string_io(arc.q.y) + "," +
                db::to_string_io(arc.c.x) + "," + db::to_string_io(arc.c.y) + ",";
            if (arc.direction == 0)
                str += "CW";
            else
                str += "CCW";
        }
        else {
            str = "line," + db::to_string_io(seg.first.x) + "," + db::to_string_io(seg.first.y) + ","
                + db::to_string_io(seg.second.x) + "," + db::to_string_io(seg.second.y);
        }
        return str;
    }
    point midpoint()
    {
        if (is_arc)
            return arc.midpoint();
        else
            return seg.midpoint();
    }
    bool IsSameDirection(element ele) {
        if (ele.is_arc != is_arc) {
            cout << "ERROR! the two element are not the same type(IsSameDirection)" << endl;
            return false;
        }
        if (ele.is_arc) {
            return seg.IsSameDirection(ele.seg);
        }
        else if (!ele.is_arc) {
            return arc.IsSameDirection(ele.arc);
        }
    }
    bool operator == (element input_ele) {
        if (is_arc == input_ele.is_arc && seg == input_ele.seg && arc == input_ele.arc)
            return true;
        else
            return false;
    }
    bool operator != (element input_ele) {
        if (is_arc != input_ele.is_arc || seg != input_ele.seg || arc != input_ele.arc)
            return true;
        else
            return false;
    }
    void operator = (element input_ele) {
        is_arc = input_ele.is_arc;
        seg = input_ele.seg;
        arc = input_ele.arc;
        numbers = input_ele.numbers;
    }
};

class component {
public:
    vector<element> material;
    string name;
    double xmax, xmin, ymax, ymin;
    vector<point_data> ep;
    component(string str = "Unset") {
        name = str;
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        ep.clear();
        material.clear();
    }
    void clear() {
        xmax = 0;
        xmin = 0;
        ymax = 0;
        ymin = 0;
        ep.clear();
        material.clear();
    }
    point getStartPoint() {
        return material[0].getStartPoint();
    }
    point getEndPoint() {
        return material[material.size() - 1].getEndPoint();
    }
    void log() {
        cout << "object: " << name << endl;
        cout << "----start----" << endl;
        for (int i = 0; i < material.size(); i++) {
            cout << material[i].info() << endl;
        }
        cout << "----end----" << endl;
    }
    double shoelace() {
        //if the result >0 means the element sequence is CCW, <0 CW
        double result = 0;
        for (int i = 0; i < material.size() - 1; i++) {
            result += 0.5 * (material[i].getStartPoint().x * material[i + 1].getStartPoint().y
                - material[i].getStartPoint().y * material[i + 1].getStartPoint().x);
        }
        result += 0.5 * (material[material.size() - 1].getStartPoint().x * material[0].getStartPoint().y
            - material[material.size() - 1].getStartPoint().y * material[0].getStartPoint().x);
        return result;
    }
    bool IsCcwSequence() {
        //if (any extremum is arc)&& (the arc direction is ccw )&& (extremum is not on the arc.p || arc.q)     return true;
        element xmaxele = material[0];
        element xminele = material[0];
        element ymaxele = material[0];
        element yminele = material[0];
        xmax = material[0].rightmostValue();
        xmin = material[0].leftmostValue();
        ymax = material[0].ceilValue();
        ymin = material[0].groundValue();
        for (int i = 1; i < material.size(); i++) {
            if (db::greater(material[i].rightmostValue(), xmax)) {
                xmax = material[i].rightmostValue();
                xmaxele = material[i];
            }
            if (db::greater(xmin, material[i].leftmostValue())) {
                xmin = material[i].leftmostValue();
                xminele = material[i];
            }
            if (db::greater(material[i].ceilValue(), ymax)) {
                ymax = material[i].ceilValue();
                ymaxele = material[i];
            }
            if (db::greater(ymin, material[i].groundValue())) {
                ymin = material[i].groundValue();
                yminele = material[i];;
            }
        }
        if (xmaxele.is_arc && xmaxele.arc.direction && !db::equal(xmax, xmaxele.arc.p.x) && !db::equal(xmax, xmaxele.arc.q.x))
            return true;
        else if (xmaxele.is_arc && !xmaxele.arc.direction && !db::equal(xmax, xmaxele.arc.p.x) && !db::equal(xmax, xmaxele.arc.q.x))
            return false;
        if (xminele.is_arc && xminele.arc.direction && !db::equal(xmin, xminele.arc.p.x) && !db::equal(xmin, xminele.arc.q.x))
            return true;
        else if (xminele.is_arc && !xminele.arc.direction && !db::equal(xmin, xminele.arc.p.x) && !db::equal(xmin, xminele.arc.q.x))
            return false;
        if (ymaxele.is_arc && ymaxele.arc.direction && !db::equal(ymax, ymaxele.arc.p.y) && !db::equal(ymax, ymaxele.arc.q.y))
            return true;
        else if (ymaxele.is_arc && !ymaxele.arc.direction && !db::equal(ymax, ymaxele.arc.p.y) && !db::equal(ymax, ymaxele.arc.q.y))
            return false;
        if (yminele.is_arc && yminele.arc.direction && !db::equal(ymin, yminele.arc.p.y) && !db::equal(ymin, xminele.arc.q.y))
            return true;
        else if (yminele.is_arc && !yminele.arc.direction && !db::equal(ymin, yminele.arc.p.y) && !db::equal(ymin, xminele.arc.q.y))
            return false;
        if (shoelace() > 0)
            return true;
        else
            return false;
    }
    double area() {
        double area = abs(shoelace());
        for (int i = 0; i < material.size(); i++) {
            if (material[i].is_arc) {
                double sectorArea = material[i].arc.radius() * material[i].arc.radius() * (double)M_PI * (material[i].arc.arc_angle() / 360.0);
                double triangleArea = material[i].arc.radius() * material[i].arc.radius() * abs(sin(material[i].arc.arc_angle() / 180.0 * (double)M_PI)) / 2.0;
                if (!material[i].arc.direction) {//add
                    if (material[i].arc.arc_angle() > 180)
                        area += sectorArea + triangleArea;
                    else
                        area += sectorArea - triangleArea;
                }
                else {//subtract
                    if (material[i].arc.arc_angle() > 180)
                        area -= sectorArea + triangleArea;
                    else
                        area -= sectorArea - triangleArea;
                }
            }
        }
        return area;
    }
    double length() {
        double len = 0;
        for (int i = 0; i < material.size(); i++) {
            len += material[i].length();
        }
        return len;
    }
    void set_extreme_value() {
        if (!material.size())
            return;
        xmax = material[0].rightmostValue();
        xmin = material[0].leftmostValue();
        ymax = material[0].ceilValue();
        ymin = material[0].groundValue();
        for (int i = 1; i < material.size(); i++) {
            if (db::greater(material[i].rightmostValue(), xmax))
                xmax = material[i].rightmostValue();
            if (db::greater(xmin, material[i].leftmostValue()))
                xmin = material[i].leftmostValue();
            if (db::greater(material[i].ceilValue(), ymax))
                ymax = material[i].ceilValue();
            if (db::greater(ymin, material[i].groundValue()))
                ymin = material[i].groundValue();
        }
    }
    void reverse_material() {
        for (int i = 0; i < material.size(); i++) {
            if (material[i].is_arc) {
                material[i].arc.direction = abs(material[i].arc.direction - 1);
                material[i].arc.p.swap(material[i].arc.q);
            }
            else
                material[i].seg.first.swap(material[i].seg.second);
        }
        vector<element> temp;
        for (int i = material.size() - 1; i >= 0; i--) {
            temp.push_back(material[i]);
        }
        material = temp;
    }
    void eliminateCcwSmallArc(double gap) {
        int record = 0; // record the used one
        if (material.size() == 1)
            return;
        for (int i = material.size() - 1; i >= 0; i--) { // for the convenience of deletion, loop from the last one
            if (material[i].is_arc && material[i].arc.direction == 1 && db::less_or_equal(material[i].arc.radius(), gap)) {
                if (material[record].is_arc == 0 && material[record].seg.second.collinear(material[i].arc.p, material[i].arc.q)) {
                    // colliner line to small arc p,q
                    material[record].seg.first = material[i].arc.p;
                    material.erase(material.begin() + i);
                }
                else {
                    material[i].is_arc = 0;
                    material[i].seg.first = material[i].arc.p;
                    material[i].seg.second = material[i].arc.q;
                }
            }
            else if (material[i].is_arc == 0 && material[record].is_arc == 0) {
                if (material[record].seg.second.collinear(material[i].seg.first, material[i].seg.second)) {
                    //two collinear line
                    material[record].seg.first = material[i].seg.first;
                    material.erase(material.begin() + i);
                }
            }
            // record won't change from 0 to comp.material.size() - 1, if the deletion happened in the first loop
            if (record == 0 && i == material.size() - 1)
                record = material.size() - 1;
            else if (record != 0)
                record--;
        }
    }
    void eliminate_duplicate_arc() {
        if (material.size() == 1)
            return;
        // connect the continous arcs with same center
        for (int i = 0; i < material.size() - 1; i++) {
            if (material[i].is_arc && material[i + 1].is_arc) {
                if (material[i].arc.c == material[i + 1].arc.c) {
                    material[i].arc.q = material[i + 1].arc.q;
                    material.erase(material.begin() + i + 1);
                    i--;
                }
            }
            if (material.size() == 1)
                return;
        }
        if (material[material.size() - 1].is_arc && material[0].is_arc) {
            if (material[material.size() - 1].arc.c == material[0].arc.c) {
                material[material.size() - 1].arc.q = material[0].arc.q;
                material.erase(material.begin());
            }
        }
    }
    void eleminate_collinear_seg() {
        //connect continuously collinear segments
        for (int i = 0; i < material.size() - 1; i++) {
            if (!material[i].is_arc && !material[i + 1].is_arc) {
                if (material[i].seg.collinear(material[i + 1].seg)) {
                    material[i].seg.second = material[i + 1].seg.second;
                    material.erase(material.begin() + i + 1);
                    i--;
                }
            }
            if (material.size() == 1)
                return;
        }
        if (!material[material.size() - 1].is_arc && !material[0].is_arc) {
            if (material[material.size() - 1].seg.collinear(material[0].seg)) {
                material[material.size() - 1].seg.second = material[0].seg.second;
                material.erase(material.begin());
            }
        }
    }
    void silkscreen_output(string file_name = "Result.txt") {
        //the function will detect the discontinuity and add the word "silkscreen"
        //comp will be the silkscreen location data
        //file_name will be the output file name
        ofstream outfile(file_name);
        int endpoint = material.size() - 1;
        for (int i = 0; i < material.size() - 2; i++) {
            if (material[i].getEndPoint() != material[i + 1].getStartPoint()) {
                endpoint = i;
                break;
            }
        }
        int i = endpoint + 1;
        outfile << "silkscreen" << endl;
        do {
            if (i == material.size()) {
                i = 0;
            }
            outfile << material[i].info() << endl;
            if (i == material.size() - 1 && material[i].getEndPoint() != material[0].getStartPoint() ||
                i != material.size() - 1 && material[i].getEndPoint() != material[i + 1].getStartPoint()) {
                if (i != endpoint)
                    outfile << "silkscreen" << endl;
            }
            i++;
        } while (i != endpoint + 1);
    }
    bool operator == (component input_comp) {
        if (input_comp.material.size() != material.size())
            return false;
        for (int i = 0; i < material.size(); i++) {
            if (material[i] != input_comp.material[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator != (component input_comp) {
        if (input_comp.material.size() != material.size())
            return true;
        for (int i = 0; i < material.size(); i++) {
            if (material[i] != input_comp.material[i]) {
                return true;
            }
        }
        return false;
    }
    void operator = (component input_comp) {
        material.clear();
        for (int i = 0; i < input_comp.material.size(); i++)
            material.push_back(input_comp.material[i]);
    }
    void RoundingCopy(component comp) {
        material.clear();
        for (int i = 0; i < comp.material.size(); i++) {
            if (comp.material[i].is_arc) {
                material.push_back(element(point(db::rounding(comp.material[i].arc.p.x), db::rounding(comp.material[i].arc.p.y)),
                    point(db::rounding(comp.material[i].arc.q.x), db::rounding(comp.material[i].arc.q.y)),
                    point(db::rounding(comp.material[i].arc.c.x), db::rounding(comp.material[i].arc.c.y)), comp.material[i].arc.direction));
            }
            else {
                material.push_back(element(point(db::rounding(comp.material[i].seg.first.x), db::rounding(comp.material[i].seg.first.y)),
                    point(db::rounding(comp.material[i].seg.second.x), db::rounding(comp.material[i].seg.second.y))));
            }

        }
    }
};

class extremum_silkscreens {
public:
    //containers place the silkscreen collected by extremum process and clarity
    vector<component> xmax_container;
    vector<component> xmin_container;
    vector<component> ymax_container;
    vector<component> ymin_container;
    //boxes place the silkscreen will be processed by area optimization
    vector<component> xmax_box;
    vector<component> xmin_box;
    vector<component> ymax_box;
    vector<component> ymin_box;
    //ans_silk place the correct answer
    vector<component> ans_silk;
    vector<component> key_silk;
    //four extremums from assembly
    double assxmax, assxmin, assymax, assymin;
    extremum_silkscreens(component ass) {
        xmax_container.clear();
        xmin_container.clear();
        ymax_container.clear();
        ymin_container.clear();
        xmax_box.clear();
        xmin_box.clear();
        ymax_box.clear();
        ymin_box.clear();
        ans_silk.clear();
        key_silk.clear();
        ass.set_extreme_value();
        assxmax = ass.xmax;
        assxmin = ass.xmin;
        assymax = ass.ymax;
        assymin = ass.ymin;
    }
    void clear() {
        xmax_container.clear();
        xmin_container.clear();
        ymax_container.clear();
        ymin_container.clear();
        xmax_box.clear();
        xmin_box.clear();
        ymax_box.clear();
        ymin_box.clear();
        ans_silk.clear();
        key_silk.clear();
    }
    void showbox() {
        cout << "------------------------------------------" << endl;
        cout << "show box mode: " << endl;
        cout << "xmax: " << xmax_box.size() << endl;
        for (int i = 0; i < xmax_box.size(); i++) {
            xmax_box[i].log();
        }
        cout << "xmin: " << xmin_box.size() << endl;
        for (int i = 0; i < xmin_box.size(); i++) {
            xmin_box[i].log();
        }
        cout << "ymax:" << ymax_box.size()<< endl;
        for (int i = 0; i < ymax_box.size(); i++) {
            ymax_box[i].log();
        }
        cout << "ymin:" << ymin_box.size()<< endl;
        for (int i = 0; i < ymin_box.size(); i++) {
            ymin_box[i].log();
        }
        cout << "------------------------------------------" << endl;
    }
    void showcontainer() {
        cout << "------------------------------------------" << endl;
        cout << "show container mode: " << endl;
        cout << "xmax:" << xmax_container.size()<< endl;
        for (int i = 0; i < xmax_container.size(); i++) {
            xmax_container[i].log();
        }
        cout << "xmin:" << xmin_container.size()<< endl;
        for (int i = 0; i < xmin_container.size(); i++) {
            xmin_container[i].log();
        }
        cout << "ymax:" << ymax_container.size()<< endl;
        for (int i = 0; i < ymax_container.size(); i++) {
            ymax_container[i].log();
        }
        cout << "ymin:" << ymin_container.size() << endl;
        for (int i = 0; i < ymin_container.size(); i++) {
            ymin_container[i].log();
        }
        cout << "------------------------------------------" << endl;
    }
    void classify_extremum(vector<component> silkscreen) {
        for (int i = 0; i < silkscreen.size(); i++) {
            silkscreen[i].set_extreme_value();
            if (db::greater_or_equal(silkscreen[i].xmax, assxmax))
                xmax_box.push_back(silkscreen[i]);
            if (db::less_or_equal(silkscreen[i].xmin, assxmin))
                xmin_box.push_back(silkscreen[i]);
            if (db::greater_or_equal(silkscreen[i].ymax, assymax))
                ymax_box.push_back(silkscreen[i]);
            if (db::less_or_equal(silkscreen[i].ymin, assymin))
                ymin_box.push_back(silkscreen[i]);
        }
    }
    void classify_key_silk() {
        for (int i = 0; i < key_silk.size(); i++) {
            key_silk[i].set_extreme_value();
            if (db::greater_or_equal(key_silk[i].xmax, assxmax))
                xmax_container.push_back(key_silk[i]);
            if (db::less_or_equal(key_silk[i].xmin, assxmin))
                xmin_container.push_back(key_silk[i]);
            if (db::greater_or_equal(key_silk[i].ymax, assymax))
                ymax_container.push_back(key_silk[i]);
            if (db::less_or_equal(key_silk[i].ymin, assymin))
                ymin_container.push_back(key_silk[i]);
        }
    }
    void minlength_checker(double silklen_limit) {
        cout << "-------start of minlength_checker-------" << endl;
        minlength_process(key_silk, silklen_limit);
        cout << "--------end of minlength_checker--------" << endl;
    }
    void minlength_process(vector<component>& silk, double limit) {
        for (int i = 0; i < silk.size(); i++) {
            cout << "length: " << silk[i].length() << endl;
            silk[i].log();
            if (db::equal(silk[i].length(), 0)) {
                silk.erase(silk.begin() + i);
                i--;
                continue;
            }
            if (silk[i].length() < limit) {
                element new_ele;
                if (db::greater(silk[i].material[0].length() + silk[i].length(), limit)) {
                    inverse_element(new_ele, silk[i].material[0]);
                    silk[i].material.insert(silk[i].material.begin(), new_ele);
                }
                else if (db::greater(silk[i].material[silk[i].material.size() - 1].length() + silk[i].length(), limit)) {
                    inverse_element(new_ele, silk[i].material[silk[i].material.size() - 1]);
                    silk[i].material.insert(silk[i].material.end(), new_ele);
                }
                else {
                    int maxele_pos = 0;
                    for (int j = 0; j < silk[i].material.size(); j++) {
                        if (db::greater(silk[i].material[j].length(), silk[i].material[maxele_pos].length()))
                            maxele_pos = j;
                    }
                    do {
                        inverse_element(new_ele, silk[i].material[maxele_pos]);
                        silk[i].material.insert(silk[i].material.begin() + maxele_pos, new_ele);
                        silk[i].material.insert(silk[i].material.begin() + maxele_pos, silk[i].material[maxele_pos + 1]);
                    } while (silk[i].length()< limit);
                }
            }
        }
    }
    void inverse_element(element& new_ele, element org_ele) {
        new_ele = org_ele;
        if (new_ele.is_arc == 0) {
            new_ele.seg.first = org_ele.seg.second;
            new_ele.seg.second = org_ele.seg.first;
        }
        else {
            new_ele.arc.p = org_ele.arc.q;
            new_ele.arc.q = org_ele.arc.p;
            new_ele.arc.direction = !org_ele.arc.direction;
        }
    }
    void pick(int i, int j, int k, int w) {
        ans_silk.clear();
        ans_silk.push_back(xmax_container[i]);
        ans_silk.push_back(xmin_container[j]);
        ans_silk.push_back(ymax_container[k]);
        ans_silk.push_back(ymin_container[w]);
    }
};

class timer {
private:
    double startTime;
    double endTime;
public:
    timer() {
        startTime = clock();
        endTime = 0;
    }
    double getExecutionTime(bool IfPrint = 0) {
        endTime = clock();
        if (IfPrint) {
            cout << "Duration: " << (endTime - startTime) / CLOCKS_PER_SEC << " secs" << endl;
        }
        return (endTime - startTime) / CLOCKS_PER_SEC;
    }
};
#pragma once