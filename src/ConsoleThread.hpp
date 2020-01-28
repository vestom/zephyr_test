#include "TF/Log.h"
#include "TF/Thread.h"
#include "TF/Timer.h"
#include "TF/Event.h"
#include <string.h>

// Abstract zephyr specific console functions
#ifdef _TF_OS_ZEPHYR_
#include <console/console.h>
#define getchar()   (console_getchar())
#define putchar(c)  (console_putchar(c))
#include <debug/tracing.h>
#endif


class ConsoleThread : public TF::Thread {
    void run() {
        TF::Log::debug("ConsoleThread");
        console_init();

        char ic;
        while(1) {
            ic = getchar();
            switch(ic) {
                case 'c':   cmdCpuStats(); break;
                case 'e':   cmdEvent(); break;
                case 'h':   cmdHelp(); break;
                case 'l':   cmdLoad(); break;
                default:    printf("Unknown command: %c\n", ic);
                            cmdHelp();
                            break;
            }
        }
    }

    void cmdHelp() {
        printf("Console commands:\n");
        printf("c = CPU stats\n");
        printf("e = Event\n");
        printf("h = Help\n");
        printf("l = Load CPU for 1 sek\n");
        printf("\n");
    }

    void cmdCpuStats() {
        struct cpu_stats stats;
        cpu_stats_get_ns(&stats);
        uint64_t allticks = (stats.idle+stats.sched+stats.non_idle);
        uint64_t l_idle   = 1000ULL * (stats.idle) / allticks;
        uint64_t l_nidle  = 1000ULL * (stats.non_idle) / allticks;
        uint64_t l_sched  = 1000ULL * (stats.sched) / allticks;
        printf("CPU stats: Idle %.1f %%, Non-Idle %.1f %%, Sched %.1f %%\n",
            0.1*l_idle, 0.1*l_nidle, 0.1*l_sched);
        //printf("CPU stats: Idle %u, Non-Idle %u, Sched %u\n",
        //    (unsigned)stats.idle, (unsigned)stats.non_idle, (unsigned)stats.sched);
        cpu_stats_reset_counters();
    }

    void cmdEvent() {
        event.set();
    }

    void cmdLoad() {
        volatile int i=0;
        TF::Timer timer(1000);
        printf("Loading CPU... ");
        while(!timer.is_expired()) { i++; }
        printf("Done!\n");
    }

public:
    TF::Event event;


#if 0
//#ifdef __linux__
    public:
        void operator delete(void* v) {};
#endif
};

