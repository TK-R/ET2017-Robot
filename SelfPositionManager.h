#ifndef SELF_POSITION_MANAGER_
#define SELF_POSITION_MANAGER_



class SelfPositionManager
{
	private:
	public:
		SelfPositionData PositionData;

		SelfPositionManager(SelfPositionData p);

		void UpdatePosition(uint8_t leftMotorCount, uint8_t rightMotorCount);
		void ResetPosition(SelfPositionData p);
};



#endif