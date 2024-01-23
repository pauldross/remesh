#include <iostream>
#include <iomanip>
#include "geom.hh"


using namespace std;

int main(){
        Point p1 = {0,0,0 }; Point p2 = {2,2,0}; Point p3 = {3,0,0};
        double X_sol =  (p1.x*p1.x*p2.y - p1.x*p1.x*p3.y - p2.x*p2.x*p1.y + p2.x*p2.x*p3.y + p3.x*p3.x*p1.y - p3.x*p3.x*p2.y + p1.y*p1.y*p2.y - p1.y*p1.y*p3.y - p1.y*p2.y*p2.y + p1.y*p3.y*p3.y + p2.y*p2.y*p3.y - p2.y*p3.y*p3.y)/(2*(p1.x*p2.y - p2.x*p1.y - p1.x*p3.y + p3.x*p1.y + p2.x*p3.y - p3.x*p2.y));
        double Y_sol = (- p1.x*p1.x*p2.x + p1.x*p1.x*p3.x + p1.x*p2.x*p2.x - p1.x*p3.x*p3.x + p1.x*p2.y*p2.y - p1.x*p3.y*p2.y - p2.x*p2.x*p3.x + p2.x*p3.x*p3.x - p2.x*p1.y*p1.y + p2.x*p3.y*p3.y + p3.x*p1.y*p1.y - p3.x*p2.y*p2.y)/(2*(p1.x*p2.y - p2.x*p1.y - p1.x*p3.y + p3.x*p1.y + p2.x*p3.y - p3.x*p2.y));
        double Z_sol = p1.z;

        cout << X_sol << " " << Y_sol << " " << Z_sol;

        return 1;
}

