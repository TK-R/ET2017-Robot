#ifndef __POINT__
#define  __POINT__


class Point {
private:
	// なし
public:
	double X;  // X座標
	double Y;  // Y座標
    Point(double X, double Y) : X(X),Y(Y) {}
    Point() : Point(0,0) {}
};


#endif
