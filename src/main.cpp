#include <stdio.h>
#include <string.h>

#include "TF/TF.h"
#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"
#include "TF/Time.h"
#include "TF/GPIO.h"

//#include "MyClass.hpp"
TF::TimeStamp   maxThreadRunTime=0;     // Ugly global for test...
#include "SerialThread.hpp"
#include "ConsoleThread.hpp"


#ifdef __ZEPHYR__
#include <zephyr.h>
K_THREAD_STACK_DEFINE(stack_1, 1024);
K_THREAD_STACK_DEFINE(stack_2, 1024);

#ifdef __linux__
// Workaround for when running 'native_posix_64' build
extern "C" void __cxa_pure_virtual() { while (1); }
#endif
#endif

SerialThread    serialThread;
ConsoleThread   consoleThread;

// STM32F103 Bluepill
//#define LED0_PORT   "GPIOC"
//#define LED0_PIN    13
// STM32F4_DISCOVERY
#define LED0_PORT   DT_ALIAS_LED0_GPIOS_CONTROLLER
#define LED0_PIN    DT_ALIAS_LED0_GPIOS_PIN

auto gpio_led = TF::GPIO(LED0_PORT, LED0_PIN);

//int main()
extern "C" int main()
{
    TF::Log::setLogDebug(false);
    printf("Build: %s %s\n", __DATE__, __TIME__);

    printf("LED0 Port: %s, Pin: %i\n", DT_ALIAS_LED0_GPIOS_CONTROLLER, DT_ALIAS_LED0_GPIOS_PIN);


#ifdef __ZEPHYR__
//    serialThread.start(stack_1, K_THREAD_STACK_SIZEOF(stack_1));
    consoleThread.start(stack_2, K_THREAD_STACK_SIZEOF(stack_2));
#else
//    serialThread.start();
    consoleThread.start();
#endif

/////////// Benchmark
    TF::Time tic;
    char s[100];
    for (int n=0; n<1000; n++) {
        sprintf(s, "Print med format %u\n",n);
    }
    auto elapsed = tic.get_elapsed_ms();
    printf("%s", s);
    printf("Time: %lu ms\n", elapsed);
//////////

    bool led = false;
    while(1) {
        gpio_led.set(led);
        led = !led;
        if(consoleThread.event.is_set())  { consoleThread.event.wait(); printf("Got event!\n"); }
        TF::Thread::sleep_ms(300);
    }

    return 0;
}
