

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

void AbstractMoveState::LineTraceAction()
{
	auto InOut = InOutManager::GetInstance();
	double pk = CurrentPID.PGain, pd = CurrentPID.DGain, power = CurrentPID.BasePower;
	int center = 65;

	int light = InOut->InputData.ReflectLight;

	int diff = LeftEdge ? (int)light - center :  (int)center - light;
	int steering = pk * diff + (diff - PrevDiff) * pd;
	
	InOut->Forward(power, steering);
}

// ブロックまで移動中の動作
void ApproachState::Run()
{
	BlockMoveManager* BtManager = BlockMoveManager::GetInstance();
	SelfPositionManager* SpManager = SelfPositionManager::GetInstance();
	InOutManager* IoManager = InOutManager::GetInstance();			
	int currentAngle = SpManager->RobotAngle;
	int ForwardPower = 10;
	int TurnPower = 10;

	int targetWaypointAngle = BtManager->GetSrcWaypointAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);
	int targetBlockAngle = BtManager->GetSrcBlockAngle(SpManager->RobotPoint.X, SpManager->RobotPoint.Y);

	switch(SubState){
	// 仮想ウェイポイント間を移動中の動作
	case FirstTurn:
		// 旋回動作を実行
		if(targetWaypointAngle != currentAngle) {
			IoManager->Turn(currentAngle, targetWaypointAngle, TurnPower);
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
			// 白認識（ラインを跨いだ）ので、次の状態に繊維
			bool last = BtManager->ArrivalSrcWayPoint();
			
			// 最終ウェイポイントの場合
			if(last){
				// ライントレース前の旋回動作に遷移
				SubState = LineTurn;
			} else {
				// 初回旋回に遷移
				SubState = FirstTurn;
			}
			// TODO 位置補正
		} else {	
			// ラインをまたぐまでは直進
			IoManager->Forward(ForwardPower);
		}
		break;
	// ライントレース前の旋回動作
	case LineTurn:
		// 旋回動作を実行
		if(abs(targetBlockAngle - currentAngle) > 20 || IoManager->InputData.ReflectLight > 40){
			IoManager->Turn(currentAngle, targetBlockAngle, TurnPower);
		} else {
			// 角度が一致したため、ライントレースに遷移
			SubState = LineTrace;

			// 旋回方向によって、ラインのどちら側を走行するか決定	
			if(targetBlockAngle > currentAngle) LeftEdge = false;
			else LeftEdge = true;
		}
		break;	
	// ライントレースする動作	
	case LineTrace:
		LineTraceAction();

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