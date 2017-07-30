#include "ev3api.h"
#include "BlockMoveManager.h"

BlockMoveManager* BlockMoveManager::GetInstance(){
	static BlockMoveManager manager;
	return &manager;
}

void BlockMoveManager::SetBlockMoveRule(BlockMoveRuleData data){
	RuleData = data;
	CurrentCommandNo = 0;
	CurrentWaypointNo = 0;
	CurrentCommand = RuleData.Command[CurrentCommandNo];

	HasRuleData = true;
}

// ウェイポイント到着を通知する
// trueなら、最終ウェイポイント到着
bool BlockMoveManager::ArrivalWayPoint()
{
	CurrentWaypointNo++;

	// 現在の動作状況におけるウェイポイントの数を取得
	int wayPointCount = CurrentStatusIsMove == true ? 
								CurrentCommand.BlockMoveWaypointCount : CurrentCommand.ApproachWaypointCount;

	// 最終ウェイポイントの場合
	if(CurrentWaypointNo == wayPointCount) {
		return true;
	}

	return false;
}

// ブロック置き場到着を通知する
// Trueなら、全ブロック運搬コマンド終了
bool BlockMoveManager::ArrivalBlockPosition(){
	// 現在の状態がブロック確保中だった場合
	if(CurrentStatusIsMove == false){
		// ブロック運搬状態に切替
		CurrentStatusIsMove = true;
		CurrentWaypointNo = 0;
		return false;
	}else{ // 現在の状態がブロック運搬中だった場合
		
		CurrentCommandNo++;
		CurrentStatusIsMove = false;
		
		// 最終コマンド終了の場合
		if(CurrentCommandNo == RuleData.CommandSize){
			return true;
		}

		CurrentWaypointNo = 0;
		CurrentCommand = RuleData.Command[CurrentCommandNo];
	}

	return false;
}

int BlockMoveManager::GetTargetAngle(int x, int y)
{

	return 0;
}