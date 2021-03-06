APPL_COBJS += image.o bmp_simple.o 
APPL_CXXOBJS += SerialSendTask.o PIDDataManager.o SerialReceiveState.o SerialReceiveTask.o HSLColor.o InOutManager.o StrategyManager.o  LineTraceStrategy.o BlockMoveStrategy.o SelfPositionManager.o BlockMoveManager.o SoundPlayTask.o ColorDecision.o FieldMap.o ParticleFilter.o ETTrainStrategy.o ETSumoStrategy.o PrizeStrategy.o Line.o MotorTestStrategy.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-test/Makefile
include $(EV3RT_SDK_LIB_DIR)/lib2/Makefile
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile


endif
