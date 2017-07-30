#include "ev3api.h"
#include "BlockMoveManager.h"

BlockMoveManager* BlockMoveManager::GetInstance(){
	static BlockMoveManager manager;
	return &manager;
}

void BlockMoveManager::SetBlockMoveRule(BlockMoveRuleData data){
	RuleData = data;
	HasRuleData = true;
}

int BlockMoveManager::CurrentBlockColor()
{
	if(HasRuleData == false) return -1;
	return RuleData.Command[CurrentCommand].BlockColor;
}

// 現在の運搬コマンドにおける運搬元ブロック置き場の番号を返す
// 不正な場合は-1を返す
int BlockMoveManager::CurrentSourcePosition()
{
	if(HasRuleData == false) return -1;
	return RuleData.Command[CurrentCommand].SourceBlockPosition;
}

int BlockMoveManager::CurrentDestinationPosition()
{
	if(HasRuleData == false) return -1;

	return RuleData.Command[CurrentCommand].DestinationBlockPosition;
}

int BlockMoveManager::CurrentTargetWayPoint()
{
	if(HasRuleData == false) return -1;
	return  RuleData.Command[CurrentCommand].ApproachWayPoint[0];

}