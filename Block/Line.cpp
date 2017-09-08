#include "Line.h"
#include "Point.h"

Line::Line(int no, int startPlaceNo, int endPlaceNo, int angle, Point* wayPoint)
{
	No = no;
	StartPlaceNo = startPlaceNo;
	EndPlaceNo = endPlaceNo;
	Angle = angle;
	WayPoint = wayPoint;
}

int Line::GetAngle(int PlaceNo)
{
	if(PlaceNo == StartPlaceNo) return Angle;
	else if(PlaceNo == EndPlaceNo) return Angle + 180;
	else return 0;	// ありえないパターン
}

int Line::GetAngleWithSource(int SourcePlaceNo)
{
	if(SourcePlaceNo == StartPlaceNo) return Angle + 180;
	else if(SourcePlaceNo == EndPlaceNo) return Angle;
	else return 0;	// ありえないパターン
}
