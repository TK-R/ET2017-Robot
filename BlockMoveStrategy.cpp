

#include "SelfPositionManager.h"
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
	BlockMoveManager* BtManager = BlockMoveManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
			
	int currentAngle = SpManager->Angle;

	switch(SubState){
	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 旋回動作を実行
		if(FirstTargetAngle > currentAngle)
		{
			// 反時計回りに旋回

		}else if(FirstTargetAngle < currentAngle){
			// 時計回りに旋回

		}else{
			// 角度が一致したため、次のサブステートに遷移
			SubState = ImaginaryWaypoint; 
		}
		break;
	// 最終ウェイポイントに到達したので、旋回する動作
	case ImaginaryWaypoint:
		break;

	// ライントレース前の旋回動作
	case LineTurn:
		break;

	// ライントレースする動作	
	case LineTrace:
		break;
	case Back:
		break;
	}
	
}

void MoveState::Run()
{
	//BlockMoveManager* manager = BlockMoveManager::GetInstance();

}

void PargeState::Run()
{
	//BlockMoveManager* manager = BlockMoveManager::GetInstance();

}