#ifndef __BLOCK_MOVE_MANAGER__
#define  __BLOCK_MOVE_MANAGER__

#include "SerialData.h"


class BlockMoveManager {
private:
	BlockMoveManager(){}
	BlockMoveRuleData RuleData;

	// 現在の動作状態がブロック運搬中ならTrue
	bool CurrentStatusIsMove;
	int CurrentCommandNo = 0;
	int CurrentWaypointNo = 0;
	bool HasRuleData = false;

public:
	BlockMoveCommandData CurrentCommand;

	// シングルトンのインスタンスを取得する
	static BlockMoveManager* GetInstance();
	// 受信したブロック運搬ルールを登録する
	void SetBlockMoveRule(BlockMoveRuleData data);

	// ウェイポイント到着を通知する
	// trueなら、最終ウェイポイント到着
	bool ArrivalWayPoint();

	// ブロック置き場到着を通知する
	// Trueなら、全ブロック運搬コマンド終了
	bool ArrivalBlockPosition();

	// 現在の座標より、次に向かうべき角度を取得する
	int GetTargetAngle(int x, int y);
};

#endif
