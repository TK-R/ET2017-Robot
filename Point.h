#ifndef __POINT__
#define  __POINT__


class Point {
private:
	// なし
public:
	int X;  // X座標
	int Y;  // Y座標
    Point(int X, int Y) : X(X),Y(Y) {}
    Point() : Point(0,0) {}
};

Point* WayPointArray[] = {
    new Point( 385, 395),
    new Point(1164, 395)};


//Point[] BlockPlaceArray;

#endif
