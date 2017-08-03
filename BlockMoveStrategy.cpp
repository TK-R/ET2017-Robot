

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
	int ForwardPower = 15;
	int TurnPower = 10;

	int targetWaypointAngle = BtManager->GetSrcWaypointAngle(SpManager->PositionX, SpManager->PositionY);
	int targetBlockAngle = BtManager->GetSrcBlockAngle(SpManager->PositionX, SpManager->PositionY);

	switch(SubState){
	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 旋回動作を実行
		if(targetWaypointAngle > currentAngle) {
			// 反時計回りに旋回
			IoManager->TurnCCW(TurnPower);
		} else if(targetWaypointAngle < currentAngle) {
			// 時計回りに旋回
			IoManager->TurnCW(TurnPower);
		} else {
			// 角度が一致したため、仮想ウェイポイント間の移動に遷移
			SubState = ImaginaryWaypoint;
		}
		break;
	// ウェイポイントに向かって移動する動作
	case ImaginaryWaypoint:
		// ラインを認識した場合には、直進ステートに遷移
		if(IoManager->InputData.ReflectLight < 40) {
			// ラインをまたぐまで直進
			SubState = OverLine;
		} else {
			IoManager->Forward(ForwardPower);
		}
		break;
	// ラインをまたぐまでの処理
	case OverLine:
		if(IoManager->InputData.ReflectLight > 120) {	
			// 白認識（ラインを跨いだ）ので、ライントレース前の旋回動作に遷移
			SubState = LineTurn;
			// TODO 位置補正
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(ForwardPower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		// 旋回動作を実行
		if(targetBlockAngle > currentAngle) {
			// 反時計回りに旋回
			IoManager->TurnCW(TurnPower);
		} else if(targetBlockAngle < currentAngle) {
			// 時計回りに旋回
			IoManager -> TurnCCW(TurnPower);
		} else {
			// 角度が一致したため、ライントレースに遷移
			SubState = LineTrace;
		}
		break;	
	// ライントレースする動作	
	case LineTrace:
		// 置き場色を取得
		

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