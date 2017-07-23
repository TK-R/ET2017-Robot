#ifndef __SERIAL_SEND_TASK_H__
#define __SERIAL_SEND_TASK_H__

#include "SerialData.h"
// 入力信号電文のデータ領域
extern uint8_t buff_input_signal[sizeof(InputSignalData)];
extern uint8_t buff_output_signal[sizeof(OutputSignalData)];

#endif