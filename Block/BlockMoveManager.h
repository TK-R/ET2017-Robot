#ifndef __BLOCK_MOVE_MANAGER__
#define  __BLOCK_MOVE_MANAGER__

#include "SerialData.h"
#include "Point.h"
#include "ColorDecision.h"
#include "Line.h"

class BlockMoveManager {
private:

	Line* LineArray[27] = { 
		new Line(0,		0, 	1,	0,  	new Point( 775, 395)),	
		new Line(1,		1, 	2, 	0,		new Point(1554, 395)),
		new Line(2, 	2, 	3, 	0, 		new Point(2334, 395)),
		new Line(3, 	0, 	4, 	150,	new Point( 580, 508)),
		new Line(4, 	1,	4, 	30,		new Point( 970, 508)),
		new Line(5, 	1, 	5, 	150,	new Point(1359, 508)),
		new Line(6, 	2, 	5, 	30, 	new Point(1749, 508)),
		new Line(7, 	2, 	6, 	150, 	new Point(2139, 508)),
		new Line(8, 	3, 	6, 	30,		new Point(2528, 508)),
		new Line(9, 	0, 	9, 	105,	new Point( 467, 702)),
		new Line(10, 	4, 	7, 	150,	new Point( 970, 702)),
		new Line(11, 	5, 	7, 	30, 	new Point(1359, 702)),
		new Line(12, 	5, 	8, 	150,	new Point(1749, 702)),
		new Line(13, 	6, 	8, 	30, 	new Point(2139, 702)),
		new Line(14, 	3, 	10, 75,		new Point(2641, 702)),
		new Line(15, 	4, 	9, 	60, 	new Point( 662, 815)),
		new Line(16, 	6, 	10, 120,	new Point(2446, 815)),
		new Line(17, 	7, 	11, 60, 	new Point(1052,1040)),
		new Line(18, 	7, 	12, 120,	new Point(1277,1040)),
		new Line(19, 	8, 	13, 60,		new Point(1831,1040)),
		new Line(20, 	8, 	14, 120,	new Point(2056,1040)),
		new Line(21, 	9, 	11, 150,	new Point( 745,1122)),
		new Line(22, 	10, 14, 30,		new Point(2364,1122)),
		new Line(23, 	11, 12, 0,		new Point(1164,1235)),
		new Line(24, 	12, 13, 0,		new Point(1554,1235)),
		new Line(25, 	13, 14, 0,		new Point(1944,1235)),
		new Line(26, 	9, 	9, 	60,		new Point( 446,1168))
	};

	BlockMoveManager(){
	};
 

	static int MyAtan2(double x, double y);

	BlockMoveRuleData RuleData;

	bool HasRuleData = false;

	// ブロック置き場の座標
	Point* BlockPlaceArray[15] =
	{
		new Point( 385, 395),
		new Point(1164, 395),
		new Point(1944, 395),
		new Point(2723, 395),
		new Point( 775, 620),
		new Point(1554, 620),
		new Point(2334, 620),
		new Point(1164, 845),
		new Point(1944, 845),
		new Point( 550,1010),
		new Point(2559,1010),
		new Point( 939,1235),
		new Point(1389,1235),
		new Point(1719,1235),
		new Point(2169,1235)
	};

	// ブロック置き場色
	HSLColorKind BlockPositionColors[15] = 
	{
		HSLRed,
		HSLBlue,
		HSLYellow,
		HSLBlue,
		HSLYellow,
		HSLGreen,
		HSLRed,
		HSLRed,
		HSLBlue,
		HSLGreen,
		HSLGreen,
		HSLBlue,
		HSLYellow,
		HSLRed,
		HSLYellow
	};


public:
	int CurrentSrcWaypointCount = 0;
	int CurrentDstWaypointCount = 0;

	int CurrentCommandNo = 0;
	
	BlockMoveCommandData CurrentCommand;

	// シングルトンのインスタンスを取得する
	static BlockMoveManager* GetInstance();
	// 受信したブロック運搬ルールを登録する
	void SetBlockMoveRule(BlockMoveRuleData data);

	// 運搬元ブロック置き場に向かうためのウェイポイント到着を通知する
	// trueなら、最終ウェイポイント到着
	bool ArrivalSrcWayPoint();

	// 運搬先ブロック置き場に向かうためのウェイポイント到着を通知する
	// trueなら、最終ウェイポイント到着
	bool ArrivalDstWayPoint();

	// 運搬元ブロック置き場到着を通知する
	// Trueなら、全ブロック運搬コマンド終了
	bool ArrivalSrcBlockPosition();

	// 運搬先ブロック置き場到達を通知する
	// Trueなら、全ブロック運搬コマンド終了
	bool ArrivalDstBlockPosition();

	// 運搬元ブロック置き場に到達するため経由する、
	// 次のウェイポイントの角度を取得する
	int GetSrcWaypointAngle(double x, double y);

	// 運搬先ブロック置き場に到達するため経由する、
	// 次のウェイポイントの角度を取得する
	int GetDstWaypointAngle(double x, double y);

	// 次に向かうべき運搬元ブロック置き場の角度を取得する
	int GetSrcBlockAngle(double x, double y);
	
	// 次に向かうべき運搬先ブロック置き場の角度を取得する
	int GetDstBlockAngle(double x, double y);
	
	// 運搬元ブロック置き場に到達するため経由する、
	// 次のウェイポイントの座標を取得する
	Point* GetSrcWaypoint();

	// 運搬先ブロック置き場に到達するため経由する、
	// 次のウェイポイントの座標を取得する
	Point* GetDstWaypoint();

	// 次に向かうべき運搬元ブロック置き場の座標を取得する
	Point* GetSrcBlockPoint();

	// 次に向かうべき運搬先ブロック置き場の座標を取得する
	Point* GetDstBlockPoint();

	Line* GetLine(int No) { return LineArray[No]; }

	// ブロックにアプローチする際の、現在のウェイポイントNoを取得する
	int GetSrcWayPointNo();

	// ブロックを運搬する際の、現在のウェイポイントNoを取得する
	int GetDstWayPointNo();
	
	// 現在の運搬元ブロック置き場色を取得する
	HSLColorKind GetSrcBlockPositionColor();

	// 現在の運搬先ブロック置き場色を取得する
	HSLColorKind GetDstBlockPositionColor();


};

#endif
