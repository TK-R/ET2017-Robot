#ifndef __BLOCK_MOVE_STRATEGY__
#define __BLOCK_MOVE_STRATEGY__

#include "Strategy.h"

enum BlockMoveStateEnum
{
	Initialize = 0, 		// 初回処理
	FirstTurn, 			// 初回旋回中
	FirstStraight,		// 初回直進（ブロック運搬のみ）
	FirstLineTrace,		// 初回ライントレース（ブロック運搬のみ）
	ImaginaryWaypoint, // 仮想ウェイポイント移動中
	OverLine,	// ラインをまたぐまで直進中
	LineTurn, // ライン上で旋回中
	LineTrace, // ライントレース移動中
	Back, // 後退中（ブロック運搬のみ）
};

enum BlockPlaceColor
{
	None = 0,
	Black = 1,
	Red,
	Blue,
	Yellow,
	Green
};

class BlockMoveStrategy;

class AbstractMoveState
{
protected:
	// サブステートの最初は必ず初回旋回
	BlockMoveStateEnum SubState = Initialize;
	BlockMoveStrategy* ParentStrategy;

public:
	// 次の旋回が時計回りか示す
	bool CW;
		
	// 左エッジを走行する場合にTrueにセットする
	bool LeftEdge;
	
	AbstractMoveState(BlockMoveStrategy* strategy)
	{
		ParentStrategy = strategy;
	}

	int CurrentWayPointNo = 26;		
	virtual void Run(){}
	virtual ~AbstractMoveState(){}
	
};

class ApproachState: public AbstractMoveState
{
	
public:
	void Run();
	using AbstractMoveState::AbstractMoveState;
	
};

class MoveState: public AbstractMoveState 
{
public:
	int ColorDetectCount = 0;
	
	void Run();
	using AbstractMoveState::AbstractMoveState;
	
};


// 戦略管理クラスから実行される戦略クラス;
class BlockMoveStrategy: public AbstractStrategy
{
private:
	AbstractMoveState* State = new ApproachState(this);
public:
	void ChangeState(AbstractMoveState* nextState);
	void Run();
	using AbstractStrategy::AbstractStrategy;
};

#endif