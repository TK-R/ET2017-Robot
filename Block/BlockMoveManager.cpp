#include <math.h>

#include "ev3api.h"
#include "BlockMoveManager.h"

BlockMoveManager* BlockMoveManager::GetInstance(){
	static BlockMoveManager manager;
	return &manager;
}

// y軸方向を0度とした、0～360の角度を求める
int BlockMoveManager::MyAtan2(double x, double y)
{	
	int val = (int)(atan2(x , y) * 180 / M_PI);
	return (val + 270) % 360;
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
	CurrentSrcWaypointCount++;
							
	// 最終ウェイポイントの場合
	if(CurrentSrcWaypointCount == 
			CurrentCommand.ApproachWaypointCount) 
	{
		return true;
	}

	return false;
}

// 運搬先ブロック置き場に向かうためのウェイポイント到着を通知する
// trueなら、最終ウェイポイント到着
bool BlockMoveManager::ArrivalDstWayPoint()
{
	CurrentDstWaypointCount++;

	// 最終ウェイポイントの場合
	if(CurrentDstWaypointCount == 
			CurrentCommand.BlockMoveWaypointCount) 
	{
		return true;
	}

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
	CurrentCommandNo++;
	
	// 最終コマンド終了の場合
	if(CurrentCommandNo == RuleData.CommandSize){
		return true;
	}

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
	return MyAtan2(wayPoint->X - x, wayPoint->Y -y);
}

// 運搬先ブロック置き場に到達するため経由する、
// 次のウェイポイントの角度を取得する
int BlockMoveManager::GetDstWaypointAngle(double x, double y)
{
	Point* wayPoint = GetDstWaypoint();
	return MyAtan2(wayPoint->X - x, wayPoint->Y -y);
}

// 次に向かうべき運搬元ブロック置き場の角度を取得する
int BlockMoveManager::GetSrcBlockAngle(double x, double y)
{
	Point* blockPoint = GetSrcBlockPoint();
	return MyAtan2(blockPoint->X - x, blockPoint->Y -y);
}

// 次に向かうべき運搬先ブロック置き場の角度を取得する
int BlockMoveManager::GetDstBlockAngle(double x, double y)
{
	Point* blockPoint = GetDstBlockPoint();
	return MyAtan2(blockPoint->X - x, blockPoint->Y -y);
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
	