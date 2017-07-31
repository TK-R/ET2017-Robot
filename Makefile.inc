APPL_COBJS += 
APPL_CXXOBJS += SerialSendTask.o SerialReceiveState.o SerialReceiveTask.o HSLColor.o InOutManager.o StrategyManager.o  LineTraceStrategy.o BlockMoveStrategy.o SelfPositionManager.o BlockMoveManager.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-test/Makefile
include $(EV3RT_SDK_LIB_DIR)/lib2/Makefile
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile


endif
