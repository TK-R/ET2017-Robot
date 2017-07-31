#include "BlockMoveManager.h"
#include "BlockMoveStrategy.h"

// 次のステートに切り替える
void BlockMoveStrategy::ChangeState(AbstractMoveState* nextState)
{
	State = nextState;
}

void BlockMoveStrategy::Run()
{
	State->Run();
}

// ブロックまで移動中の動作
void ApproachState::Run()
{
	//BlockMoveManager* manager = BlockMoveManager::GetInstance();
	
	// 仮想ウェイポイント間を移動中の動作


	// 最終ウェイポイントに到達したので、旋回する動作


	// ライントレースする動作

}

void MoveState::Run()
{
	//BlockMoveManager* manager = BlockMoveManager::GetInstance();

}

void PargeState::Run()
{
	//BlockMoveManager* manager = BlockMoveManager::GetInstance();

}