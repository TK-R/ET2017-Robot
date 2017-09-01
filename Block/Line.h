#ifndef __LINE_H_
#define __LINE_H_

#include "Point.h"

class Line {

public:
	int No;
	int Angle;
	int StartPlaceNo;
	int EndPlaceNo;
	Point* WayPoint;

	// 向かっているブロック置き場Noから、補正座標を取得する
	int GetAngle(int PlaceNo);

	// コンストラクタ
	Line(int no, int startPlaceNo, int endPlaceNo, int angle, Point* wayPoint);
};


#endif
