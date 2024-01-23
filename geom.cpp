


#define PI 3.141592653589793238463
#define debug1

#include <iostream>
#include <iomanip>
#include <utility>
#include <bits/stdc++.h>


#include "geom.hh"
using namespace std;


Point operator+(Point a, Point b){
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
Point operator-(Point a, Point b){
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
double operator*(Point a, Point b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
Point operator/(Point a,double b){
    return {a.x/b, a.y/b, a.z/b};
}

// helper methods -> TODO: move to static methods in class
// angle of points used in sorting
double angleXY(Point p){
    double angle = atan(p.y/p.x);
#ifdef debug
    cout << " ( " << angle*180/PI << " ) ";
#endif
    double add = 0;
        // second quadrant
    if (p.x == 0 && p.y == 0) {return 0;}
    if (p.y == 0) return (p.x > 0) ? 0 : PI;
    if (p.x == 0) return (p.y >0) ? PI/2 : 3*PI/2;

    if (p.y >= 0 && p.x <= 0){
        return PI + angle;
    } else if (p.y <= 0 && p.x < 0){
        // third quadrant
        return angle + PI;
    } else if (p.y <= 0 && p.x >= 0){
        //third quadrant
        add = 2 * PI;
        return add + angle;
    }
    return angle;
}
// for sorting the points
bool comparePoint(Point p1, Point p2){
    return angleXY(p1) > angleXY(p2);
}


// class methods
Cell :: Cell(vector<Point> p_pts, Point p_charPt, int p_numPts, int CharId,Point p_B_field) :
      pts(std::move(p_pts)),
      charPt(p_charPt),
      numPts(p_numPts),
      charId(CharId),
      B_field(p_B_field)
    {
        area = 0;
        areaComputed = false;
        getArea();
    }

Cell :: ~Cell() = default;

void Cell :: _translate() {
    // implement and test
    for(Point &pt : this->pts){
        pt = pt - this->charPt;
    }
}

void Cell :: _orderPts(){
    //implement and test
    #ifdef debug
    cout << "Before sort" << endl;
    for (Point pt : pts){
        cout << pt.x << " " << pt.y << " " << angleXY(pt) * 180/PI << endl;
    }
    cout << "After sort" << endl;
    #endif

    sort(pts.begin(),pts.end(), comparePoint);

    #ifdef debug
    for (Point pt : pts){
        cout << pt.x << " " << pt.y << " " << angleXY(pt) * 180/PI << endl;
    }
    #endif
}

double Cell :: _computeArea(){
    //implement and test
    double sum1, sum2 = 0;
    // calculate area given ordered points
    for (int i = 0; i < numPts; i++){
        int sindex = (i+1) % numPts;
        sum1 += pts.at(i).x*pts.at(sindex).y;
        sum2 += pts.at(sindex).x*pts.at(i).y;
    }
    
    double tot = 1/2.0 * abs(sum1 - sum2);
    return tot;
}

void Cell :: _translateBack(){
    for (Point &pt : this->pts){
        pt = pt + this->charPt;
    }
}


// getters and setters 
double Cell :: getArea() {
    if (areaComputed) return area;

    this->_translate();
    this->_orderPts();
    double area = this->_computeArea();
    this->area = area;
    areaComputed = true;
    this->_translateBack();
    return area;
}

double Cell :: getNumPts(){
    const double res = numPts;
    return res;
}

Point Cell :: getCharPt() {
    return this->charPt;
}

int Cell :: getCharPtId(){
    return this->charId;
}



//static methods
void Cell :: test() {
    cout << "test";
}


Point Cell :: MeanPoints(const vector<Point>& pts){
    auto n = (double) pts.size();
    Point avg = {0,0,0};
    for (Point pt : pts){
        avg = avg + pt;
    }
    return avg/n;
}

Point Cell :: vtkPtsToPoint(vtkPoints *PointArray, int pid){
    double *ps = PointArray->GetPoint(pid);
    return {1E6*ps[0],1E6*ps[1],1E6*ps[2],pid};
}


Point Cell :: MeanPoints(Point pt1, Point pt2){
    return (pt1 + pt2)/2;
}

Point Cell :: GetPoint(int index){
    return pts[index];
}


// to string

std::string Cell :: toString() {
        return "(size: " +  to_string(this->getNumPts()) + " Area: " + to_string(this->getArea()) + " Charpt id: " + to_string(this->getCharPtId()) + ")";
    }

Point Cell::getB() {
    return B_field;
}


std::ostream& operator<<(std::ostream &s, Point pt) {
    return s << "(x: " << pt.x << "\t y: " << pt.y << "\t z:" << pt.z<< ")";
}