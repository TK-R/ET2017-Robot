#ifndef __BLOCK_MOVE_MANAGER__
#define  __BLOCK_MOVE_MANAGER__

#include "SerialData.h"
#include "Point.h"


class BlockMoveManager {
private:
	BlockMoveManager(){}

	static int MyAtan2(double x, double y);

	BlockMoveRuleData RuleData;

	int CurrentCommandNo = 0;
	int CurrentSrcWaypointNo = 0;
	int CurrentDstWaypointNo = 0;
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

	// ウェイポイントの座標
	Point* WayPointArray[27] = 
	{
		new Point( 775, 395),
		new Point(1554, 395),
		new Point(2334, 395),
		new Point( 580, 508),
		new Point( 970, 508),
		new Point(1359, 508),
		new Point(1749, 508),
		new Point(2139, 508),
		new Point(2528, 508),
		new Point( 467, 702),
		new Point( 970, 702),
		new Point(1359, 702),
		new Point(1749, 702),
		new Point(2139, 702),
		new Point(2641, 702),
		new Point( 662, 815),
		new Point(2446, 815),
		new Point(1052,1040),
		new Point(1277,1040),
		new Point(1831,1040),
		new Point(2056,1040),
		new Point( 745,1122),
		new Point(2364,1122),
		new Point(1164,1235),
		new Point(1554,1235),
		new Point(1944,1235),
		new Point( 446,1168)
	};


public:
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

};

#endif
