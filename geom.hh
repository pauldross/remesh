/*
Author: Paul Rossouw
Description: 
Class that calculates the area of an n-gon given that it is parralell with the xy plane.
Used in msfn calculation to calculate node areas.
Usage: 
Pass it points of type Point all except the point at which the area is being calculated. That point is passed to charpt
numPoints is the number of points in the polygon (excluding the characteristic point)

*/

/*
 TODO: Gotta make this work in 3D man. Current solution is getting the normal and detecting when the
 object is oriented in the x-z or y-z plane. These are calculated with the normal algorithm except x or y is replaced by z.
 If object is not perfectly oriented in one of these planes project onto xy plane and use formula to calculate are when not projected.
*/

#ifndef GEOM_
#define GEOM_



#define B_PT(v) {v[0], v[1], v[3]}
#define PT(pt) pt.x , pt.y, pt.z

#include <vtkGenericDataObjectReader.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <vector>
#include <math.h>
#include <string>


typedef struct Point { 
    double x, y, z;
    int pid = 0;
    double norm() { return std::sqrt(x*x + y*y + z*z); }
} Point;


Point operator+(Point a, Point b);
Point operator-(Point a, Point b);
double operator*(Point a, Point b);
Point operator/(Point a, double b);
std::ostream& operator<<(std::ostream &s, Point pt);

class Cell {
    private:
        double area;
        bool areaComputed;
        std::vector<Point> pts;
        Point charPt;
        int numPts;
        int charId;
        Point B_field;

        void _orderPts();
        void _translate();
        void _translateBack();
        double _computeArea();


    public:
        Cell(std::vector<Point> p_pts, Point p_charPt, int p_numPts, int CharId, Point p_B_field);
        ~Cell();
        double getArea();
        double getNumPts();
        Point getCharPt();
        Point GetPoint(int index);
        int getCharPtId();
        std::string toString();
        static void test();
        static Point MeanPoints(const std::vector<Point>& points);
        static Point MeanPoints(Point pt1, Point pt2);
        static Point vtkPtsToPoint(vtkPoints *PointArray, int pid);
        Point getB();
};

#endif