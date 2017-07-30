#ifndef __BLOCK_MOVE_MANAGER__
#define  __BLOCK_MOVE_MANAGER__

#include "SerialData.h"


class BlockMoveManager {
private:
	BlockMoveManager(){}
	BlockMoveRuleData RuleData;
	int CurrentCommand = 1;
	bool HasRuleData = false;
public:
	static BlockMoveManager* GetInstance();
	void SetBlockMoveRule(BlockMoveRuleData data);
	int CurrentBlockColor();
	int CurrentSourcePosition();
	int CurrentDestinationPosition();
	int CurrentTargetWayPoint();
};

#endif
