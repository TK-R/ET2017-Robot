#include <math.h>

#include "ev3api.h"
#include "BlockMoveManager.h"

BlockMoveManager* BlockMoveManager::GetInstance(){
	static BlockMoveManager manager;
	return &manager;
}

// y軸方向を0度とした、0～360の角度を求める
int BlockMoveManager::MyAtan2(double x1, double x2, double y1, double y2)
{	
	int val = (int)(atan2(y1 - y2 , x2 - x1) * 180 / M_PI);
	return (val + 360) % 360;
}

// 運搬ルールを初期化する
void BlockMoveManager::SetBlockMoveRule(BlockMoveRuleData data){
	RuleData = data;
	CurrentCommandNo = 0;

	CurrentSrcWaypointCount = 0;
	CurrentDstWaypointCount = 0;
	CurrentCommand = RuleData.Command[CurrentCommandNo];

	HasRuleData = true;
}

// 運搬元ブロック置き場に向かうためのウェイポイント到着を通知する
// trueなら、最終ウェイポイント到着
bool BlockMoveManager::ArrivalSrcWayPoint()
{					
	// 最終ウェイポイントの場合
	if((CurrentSrcWaypointCount + 1)  == 
			CurrentCommand.ApproachWaypointCount) 
	{
		return true;
	}
	
	CurrentSrcWaypointCount++;
	
	return false;
}

// 運搬先ブロック置き場に向かうためのウェイポイント到着を通知する
// trueなら、最終ウェイポイント到着
bool BlockMoveManager::ArrivalDstWayPoint()
{
	// 最終ウェイポイントの場合
	if((CurrentDstWaypointCount + 1) == 
			CurrentCommand.BlockMoveWaypointCount) 
	{
		return true;
	}

	CurrentDstWaypointCount++;	
	return false;
}

// 運搬元ブロック置き場到着を通知する
// Trueなら、全ブロック運搬コマンド終了
bool BlockMoveManager::ArrivalSrcBlockPosition()
{	
	// ウェイポイント値をクリア	
	CurrentSrcWaypointCount = 0;
	CurrentDstWaypointCount = 0;
	return false;
}

// 運搬先ブロック置き場到達を通知する
// Trueなら、全ブロック運搬コマンド終了
bool BlockMoveManager::ArrivalDstBlockPosition()
{
	// 最終コマンド終了の場合
	if((CurrentCommandNo + 1) == RuleData.CommandSize){
		return true;
	}

	CurrentCommandNo++;
	
	CurrentSrcWaypointCount = 0;
	CurrentDstWaypointCount = 0;

	CurrentCommand = RuleData.Command[CurrentCommandNo];
	return false;
}

// 運搬元ブロック置き場に到達するため経由する、
// 次のウェイポイントの角度を取得する
int BlockMoveManager::GetSrcWaypointAngle(double x, double y)
{	
	Point* wayPoint = GetSrcWaypoint();
	return MyAtan2(x, wayPoint->X, y,  wayPoint->Y);
}

// 運搬先ブロック置き場に到達するため経由する、
// 次のウェイポイントの角度を取得する
int BlockMoveManager::GetDstWaypointAngle(double x, double y)
{
	Point* wayPoint = GetDstWaypoint();
	return MyAtan2(x, wayPoint->X, y,  wayPoint->Y);
}

// 次に向かうべき運搬元ブロック置き場の角度を取得する
int BlockMoveManager::GetSrcBlockAngle(double x, double y)
{
	Point* blockPoint = GetSrcBlockPoint();
	return MyAtan2(x, blockPoint->X, y,  blockPoint->Y);
}

// 次に向かうべき運搬先ブロック置き場の角度を取得する
int BlockMoveManager::GetDstBlockAngle(double x, double y)
{
	Point* blockPoint = GetDstBlockPoint();
	return MyAtan2(x, blockPoint->X, y,  blockPoint->Y);
}

// 運搬元ブロック置き場に到達するため経由する、
// 次のウェイポイントの座標を取得する
Point* BlockMoveManager::GetSrcWaypoint()
{
	return  LineArray[GetSrcWayPointNo()]->WayPoint;
}

// 運搬先ブロック置き場に到達するため経由する、
// 次のウェイポイントの座標を取得する
Point* BlockMoveManager::GetDstWaypoint()
{
	return  LineArray[GetDstWayPointNo()]->WayPoint;
}

// 次に向かうべき運搬元ブロック置き場の座標を取得する
Point* BlockMoveManager::GetSrcBlockPoint()
{
	int blockNo = CurrentCommand.SourceBlockPosition;
	return BlockPlaceArray[blockNo];
}

// 次に向かうべき運搬先ブロック置き場の座標を取得する
Point* BlockMoveManager::GetDstBlockPoint()
{
	int blockNo = CurrentCommand.DestinationBlockPosition;
	return BlockPlaceArray[blockNo];
}

// ブロックにアプローチする際の、現在のウェイポイントNoを取得する
int BlockMoveManager::GetSrcWayPointNo()
{
	return CurrentCommand.ApproachWayPoint[CurrentSrcWaypointCount];
}

// ブロックを運搬する際の、現在のウェイポイントNoを取得する
int BlockMoveManager::GetDstWayPointNo()
{
	return CurrentCommand.BlockMoveWayPoint[CurrentDstWaypointCount];
}

// 現在の運搬元ブロック置き場色を取得する
HSLColorKind BlockMoveManager::GetSrcBlockPositionColor()
{
	int blockNo = CurrentCommand.SourceBlockPosition;
	return BlockPositionColors[blockNo];
}
	
// 現在の運搬先ブロック置き場色を取得する
HSLColorKind BlockMoveManager::GetDstBlockPositionColor()
{
	int blockNo = CurrentCommand.DestinationBlockPosition;
	return BlockPositionColors[blockNo];
}
	