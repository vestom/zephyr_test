#include <stdio.h>
#include <string.h>

#include "zephyr.h"

#include "TF/TF.h"
#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"

#include "MyClass.hpp"

#ifdef __linux__
extern "C" void __cxa_pure_virtual() { while (1); }
#endif

class MyThread1 : public TF::Thread {
    void run() {
        while(1) {
            TF::Log::debug("MyThread1");
//			queue.send((uint8_t*)"MSG1", 256);
//			queue.send((uint8_t*)"MSG2", 8);
            TF::Thread::sleep_ms(200);
        }
    }

#ifdef __linux__
    public:
        void operator delete(void* v) {};
#endif
};


TF::Serial serial;
MyClass     mc;

K_THREAD_STACK_DEFINE(stack_1, 1024);
MyThread1   myThread1;


extern "C" int main()
//int main()
{
    printf("printf() Hello World! %s\n", CONFIG_BOARD);
    printf("Port: %s, Pin: %i\n", DT_ALIAS_LED0_GPIOS_CONTROLLER, DT_ALIAS_LED0_GPIOS_PIN);
//    mc.test();

//    myThread1.start(stack_1, K_THREAD_STACK_SIZEOF(stack_1));


    serial.open();
    serial.flushRxBuffer();
    char tx_buffer[32];
    char rx_buffer[32];
    char str[32];

    int n=0;
    while(1) {
        TF::Thread::sleep_ms(300);
        // TX
        sprintf(tx_buffer, "Testing %2i", n++);
        TF::Log::debug("Sending: %s", tx_buffer);
        serial.write((uint8_t*) tx_buffer, strlen(tx_buffer));
        TF::Thread::sleep_ms(10);
        // RX
        int bytes = serial.getRxBytes();
        if(bytes) {
            serial.read((uint8_t *)rx_buffer, bytes);
            rx_buffer[bytes] = 0;

            // Bin2Hex
            for (int n=0; n<bytes; ++n) {
                sprintf(str+n*3, "%02X ", (unsigned)rx_buffer[n]);
            }
            TF::Log::debug("Received: %s, [%u] %s", rx_buffer, bytes, str);
        }
    }

    return 0;
}
