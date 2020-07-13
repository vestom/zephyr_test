#include <stdio.h>
#include <string.h>

#include "TF/TF.h"
#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"
#include "TF/Time.h"
#include "TF/GPIO.h"

#ifdef __ZEPHYR__
#include <zephyr.h>
K_THREAD_STACK_DEFINE(stack_1, 1024);
K_THREAD_STACK_DEFINE(stack_2, 1024);
#endif

//#include "MyClass.hpp"
TF::TimeStamp   maxThreadRunTime=0;     // Ugly global for test...
#include "SerialThread.hpp"
#include "ConsoleThread.hpp"

SerialThread    serialThread;
ConsoleThread   consoleThread;

// STM32F103 Bluepill
//#define LED0_PORT   "GPIOC"
//#define LED0_PIN    13
// Devicetree
#define LED0_PORT   DT_GPIO_LABEL(DT_ALIAS(led0), gpios)
#define LED0_PIN    DT_GPIO_PIN(DT_ALIAS(led0), gpios)

auto gpio_led = TF::GPIO(LED0_PORT, LED0_PIN);

//int main()
extern "C" int main()
{
    TF::Log::setLogDebug(false);
    printf("Build: %s %s\n", __DATE__, __TIME__);

    printf("LED0 Port: %s, Pin: %i\n", DT_GPIO_LABEL(DT_ALIAS(led0), gpios), DT_GPIO_PIN(DT_ALIAS(led0), gpios));
//    printf("LED2 Port: %s, Pin: %i\n", DT_GPIO_LABEL(DT_ALIAS(led2), gpios), DT_GPIO_PIN(DT_ALIAS(led2), gpios));

#ifdef __ZEPHYR__
    memset(stack_1, 0xA5, K_THREAD_STACK_SIZEOF(stack_1));  // Fill for debug
    memset(stack_2, 0xA5, K_THREAD_STACK_SIZEOF(stack_2));  // Fill for debug
    printf("stack_1 %p size: %u\n", stack_1, K_THREAD_STACK_SIZEOF(stack_1));
    printf("stack_2 %p size: %u\n", stack_2, K_THREAD_STACK_SIZEOF(stack_2));
    serialThread.start(stack_1, K_THREAD_STACK_SIZEOF(stack_1));  //, TF::Thread::Priority::HIGH);
    consoleThread.start(stack_2, K_THREAD_STACK_SIZEOF(stack_2));
#else
    serialThread.start();
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

    while(1) {
        gpio_led.set(!gpio_led.get());
        if(consoleThread.event.is_set())  {
            consoleThread.event.wait();
            printf("Got event!\n");
        }
        TF::Thread::sleep_ms(300);
    }

    return 0;
}
