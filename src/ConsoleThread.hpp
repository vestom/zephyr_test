#include "TF/Log.h"
#include "TF/Thread.h"
#include "TF/Timer.h"
#include "TF/Event.h"
#include "TF/Mutex.h"
#include "TF/Zephyr/zephyr_extras.h"
#include <string.h>

//// Helpers Begin

// Test if string starts with pre string
// Returns pointer to next character or NULL if not found
const char * startsWith(const char *str, const char *pre) {
    while(*pre) {
        if(*pre++ != *str++)
            return NULL;
    }
    return str;
}

// Return pointer to next parameter delimited by whitespace or '/'
const char * findNextParm(const char *str) {
    while(*str) {   // Find first delimiter
        if(isspace(*str) || *str=='/') {
            str++;  // Skip delimiter
            break;
        }
        str++;
    }
    while(isspace(*str)) { str++; } // Skip extra whitespace
    if(*str)    return str;
    else        return NULL;
}

//// Helpers End

class ConsoleThread : public TF::Thread {
public:
    TF::Event event;
    TF::Mutex printMutex;

private:
    char str_input[16];     // Object variable to hold string to parse

    void run() {
        TF::Log::debug("ConsoleThread");
        console_init();
        const char *s;

        while(1) {
            fgets(str_input, sizeof(str_input), stdin);
            printMutex.lock();
            printf("*%s", str_input);
            if         (startsWith(str_input, "c"))  { cmdCpuStats(); }
            else if    (startsWith(str_input, "d"))  { cmdDebug(); }
            else if    (startsWith(str_input, "e"))  { cmdEvent(); }
            else if    (startsWith(str_input, "h"))  { cmdHelp(); }
            else if    (startsWith(str_input, "l"))  { cmdLoad(); }
            else if ((s=startsWith(str_input, "t"))) { cmdTest(s); }
            else    { printf("*Error! 'h' for help\n"); }
            printMutex.unlock();
        }
    }

    void cmdHelp() {
        printf("*Console commands:\n");
        printf("* c = CPU stats\n");
        printf("* d = Debug on/off\n");
        printf("* e = Event\n");
        printf("* h = Help\n");
        printf("* l = Load CPU for 1 sek\n");
        printf("* t <parm> = Test\n");
    }

    void cmdDebug() {
        bool newstate = !TF::Log::getLogDebug();
        printf("*Debug %s\n", (newstate ? "Enabled" : "Disabled"));
        TF::Log::setLogDebug(newstate);
    }

    void cmdCpuStats() {
        struct cpu_stats stats;
        cpu_stats_get_ns(&stats);
        uint64_t allticks = (stats.idle+stats.sched+stats.non_idle);
        uint64_t l_idle   = 1000ULL * (stats.idle) / allticks;
        uint64_t l_nidle  = 1000ULL * (stats.non_idle) / allticks;
        uint64_t l_sched  = 1000ULL * (stats.sched) / allticks;
        printf("*CPU stats: Idle %.1f %%, Non-Idle %.1f %%, Sched %.1f %%\n",
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
        printf("*Loading CPU... ");
        while(!timer.is_expired()) { i++; }
        printf("*Done!\n");
    }

    void cmdTest(const char *param) {
        printf("*Received param: %s\n", param);
        const char str[] = "*Test print of long sentence..\n";
//        size_t len = sizeof(str);
        TF::Timer timer(0);
        printf(str);
//        console_write(NULL, str, len);
        unsigned time = timer.get_elapsed_ms();
        printf("*Time spent: %u ms\n", time);
    }


#if 0
//#ifdef __linux__
    public:
        void operator delete(void* v) {};
#endif
};

