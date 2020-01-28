#include <stdio.h>
#include <string.h>

#include "TF/TF.h"
#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"
#include "TF/GPIO.h"

//#include "MyClass.hpp"
#include "SerialThread.hpp"
#include "ConsoleThread.hpp"

#ifdef __linux__
extern "C" void __cxa_pure_virtual() { while (1); }
#endif

#ifdef __ZEPHYR__
#include <zephyr.h>
#endif

K_THREAD_STACK_DEFINE(stack_1, 1024);
K_THREAD_STACK_DEFINE(stack_2, 1024);
SerialThread    serialThread;
ConsoleThread   consoleThread;

#define LED0_PORT   "GPIOC"
#define LED0_PIN    13
TF::GPIO    gpio_led(LED0_PORT, LED0_PIN);

extern "C" int main()
//int main()
{
    TF::Log::setLogDebug(false);
    printf("Build: %s %s\n", __DATE__, __TIME__);
//    printf("Port: %s, Pin: %i\n", DT_ALIAS_LED0_GPIOS_CONTROLLER, DT_ALIAS_LED0_GPIOS_PIN);
//    mc.test();


#ifdef __ZEPHYR__
    serialThread.start(stack_1, K_THREAD_STACK_SIZEOF(stack_1));
    consoleThread.start(stack_2, K_THREAD_STACK_SIZEOF(stack_2));
#else
    serialThread.start();
    consoleThread.start();
#endif
//    char *lineptr = NULL;
//    size_t linelen = 0;
#include "TF/Event.h"

    bool led = false;
    while(1) {
        gpio_led.set(led);
        led = !led;
        if(consoleThread.event.is_set())  { consoleThread.event.wait(); printf("Got event!\n"); }
        TF::Thread::sleep_ms(300);
    }

    return 0;
}
