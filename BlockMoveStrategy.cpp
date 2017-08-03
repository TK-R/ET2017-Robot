

#include "SelfPositionManager.h"
#include "BlockMoveManager.h"
#include "BlockMoveStrategy.h"
#include "InOutManager.h"

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
	InOutManager* IoManager = InOutManager::GetInstance();			
	int currentAngle = SpManager->Angle;
	Point* waypoint = BtManager->GetSrcWaypoint();

	switch(SubState){
	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 旋回動作を実行
		if(FirstTargetAngle > currentAngle)
		{
			// 反時計回りに旋回
			IoManager->TurnCCW(10);

		}
		else if(FirstTargetAngle < currentAngle)
		{
			// 時計回りに旋回
			IoManager->TurnCW(10);
		}
		else
		{
			// 角度が一致したため、次のサブステートに遷移
			SubState = ImaginaryWaypoint;
		}
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// ラインを認識した場合には、ライントレース前の旋回
		
		// ウェイポイント座標と自身の座標が離れている場合
		if(SpManager->GetDistance(waypoint) > 100)
		{
			
		}else
		{
			// ウェイポイント付近の場合
			IoManager->Forward(10);
		}

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