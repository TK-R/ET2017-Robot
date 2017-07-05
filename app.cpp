/**
 * This sample program balances a two-wheeled Segway type robot such as Gyroboy in EV3 core set.
 *
 * References:
 * http://www.hitechnic.com/blog/gyro-sensor/htway/
 * http://www.cs.bgu.ac.il/~ami/teaching/Lejos-2013/classes/src/lejos/robotics/navigation/Segoway.java
 */

#include "ev3api.h"
#include "app.h"
#include "SerialData.h"
#include "SerialReceiveTask.h"
#include "libcpp-test.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

class TestClass {
public:
    TestClass() {
        //static char buf[256];
        //sprintf(buf, "Object has been created.");
        //ev3_lcd_draw_string(buf, 0, 16);
        member = 0x12345678;
    }

    void testMethod() {
        static char buf[256];
        sprintf(buf, "Member is 0x%08x.", member);
   //   ev3_lcd_draw_string(buf, 0, 32);
    }
private:
    int member;
};

auto obj2 = new TestClass();

void main_task(intptr_t unused) {
    // Test global constructor
    obj2->testMethod();

    // Test function in static library
    libcpp_test_c_echo_function(777);

    // Test class in static library
    LibSampleClass a;
    a.draw();
    
    static char buf[256];
    int i = 0;

    ev3_motor_config(EV3_PORT_A, LARGE_MOTOR);
    ev3_motor_config(EV3_PORT_B, LARGE_MOTOR);

    while(1)
    {
        sprintf(buf, " %d, Power: %4d, %4d",i, OutputData.LeftMotorPower, OutputData.RightMotorPower);
        ev3_lcd_draw_string(buf, 0, 32);

        ev3_motor_set_power(EV3_PORT_A, OutputData.LeftMotorPower);
        ev3_motor_set_power(EV3_PORT_B, OutputData.RightMotorPower);

        i++;
		dly_tsk(200);

    }

    //auto obj = new TestClass();
}
