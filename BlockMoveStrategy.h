#ifndef __BLOCK_MOVE_STRATEGY__
#define __BLOCK_MOVE_STRATEGY__

#include "Strategy.h"

enum BlockMoveStateEnum
{
	FirstTurn = 0, // 初回旋回中
	ImaginaryWaypoint, // 仮想ウェイポイント移動中
	LineTurn, // ライン上で旋回中
	LineTrace, // ライントレース移動中
	Back, // 後退中
};


class AbstractMoveState
{
protected:
	// サブステートの最初は必ず初回旋回
	BlockMoveStateEnum SubState = FirstTurn;
public:
	// 初回旋回時の目標角度
	int FirstTargetAngle;
	virtual void Run(){}
};

class ApproachState: public AbstractMoveState
{
public:
	void Run();


};

class MoveState: public AbstractMoveState 
{
public:
	void Run();

};

class PargeState: public AbstractMoveState
{
public:
	void Run();
};


// 戦略管理クラスから実行される戦略クラス;
class BlockMoveStrategy: public AbstractStrategy
{
private:
	AbstractMoveState* State = new ApproachState();

public:
	BlockMoveStrategy();
	void ChangeState(AbstractMoveState* nextState);
	void Run();
	using AbstractStrategy::AbstractStrategy;

};

#endif