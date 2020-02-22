#include "TF/Log.h"
#include "TF/Thread.h"
#include "TF/Time.h"
#include "TF/Event.h"
#include "TF/Mutex.h"
#include "TF/Zephyr/zephyr_extras.h"
#include <string.h>
#include <console/console.h>
#include <stdlib.h>
#include <ctype.h>

// Use of tracing require a Zephyr fix to declare functions 'extern "C" {}'
#include <debug/tracing.h>

//// Helpers Begin

// Pretty print of a block of memory
void printMem(void *p, size_t s) {
    static const int WIDTH = 32;
    int row=0, col=0, columns;
    int rows = (s+WIDTH-1)/WIDTH;   // Round up
    char c;
    for(row=0; row<rows; row++) {
        printf("*%10p : ", (char*)p+row*WIDTH);
        columns = MIN(WIDTH, s-row*WIDTH);
        for (col=0; col<columns; ++col) {
            printf("%02X", ((char*)p)[row*WIDTH+col]);
        }
        printf(" [");
        for (col=0; col<columns; ++col) {
            c = ((char*)p)[row*WIDTH+col];
            c = (isprint(c) ? c : '.');
            printf("%c", c);
        }
        printf("]\n");
    }
}

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
    char str_input[32];     // Object variable to hold string to parse

    void run() {
        TF::Log::debug("ConsoleThread");
        console_init();
        const char *s;

        while(1) {
            fgets(str_input, sizeof(str_input), stdin);
            printMutex.lock();
            printf("*%s", str_input);
            if         (startsWith(str_input, "cl"))  { cmdCpuLoad(); }
            else if    (startsWith(str_input, "cs"))  { cmdCpuStats(); }
            else if    (startsWith(str_input, "d"))   { cmdDebug(); }
            else if    (startsWith(str_input, "e"))   { cmdEvent(); }
            else if    (startsWith(str_input, "h"))   { cmdHelp(); }
            else if ((s=startsWith(str_input, "md"))) { cmdMemDisplay(s); }
            else if ((s=startsWith(str_input, "mt"))) { cmdMemTest(s); }
            else if    (startsWith(str_input, "s"))   { cmdStatistics(); }
            else if ((s=startsWith(str_input, "t")))  { cmdTest(s); }
            else    { printf("*Error! 'h' for help\n"); }
            printMutex.unlock();
        }
    }

    void cmdHelp() {
        printf("*Console commands:\n");
        printf("* cl = Load CPU for 1 sec\n");
        printf("* cs = CPU stats\n");
        printf("* d = Debug on/off\n");
        printf("* e = Event\n");
        printf("* h = Help\n");
        printf("* md <addr> [<size>] = Display memory\n");
        printf("* mt <size> = Test memory allocation\n");
        printf("* s = Thread statistics\n");
        printf("* t <parm> = Test\n");
    }

    void cmdCpuLoad() {
        volatile int i=0;
        TF::Time time(1000);
        printf("*Loading CPU... ");
        while(!time.is_expired()) { i++; }
        printf("*Done!\n");
    }

    void cmdCpuStats() {
/* This function only works with C++ fix in zephyr
        struct cpu_stats stats;
        cpu_stats_get_ns(&stats);
        uint64_t allticks = (stats.idle+stats.sched+stats.non_idle);
        uint64_t l_idle   = 1000ULL * (stats.idle) / allticks;
        uint64_t l_nidle  = 1000ULL * (stats.non_idle) / allticks;
        uint64_t l_sched  = 1000ULL * (stats.sched) / allticks;
//        printf("*CPU stats: Idle %u, Non-Idle %u, Sched %u\n",
//            (unsigned)stats.idle, (unsigned)stats.non_idle, (unsigned)stats.sched);
        printf("*CPU stats: Idle %.1f %%, Non-Idle %.1f %%, Sched %.1f %%\n",
            0.1*l_idle, 0.1*l_nidle, 0.1*l_sched);
        cpu_stats_reset_counters();
*/
    }

    void cmdDebug() {
        bool newstate = !TF::Log::getLogDebug();
        printf("*Debug %s\n", (newstate ? "Enabled" : "Disabled"));
        TF::Log::setLogDebug(newstate);
    }

    void cmdEvent() {
        event.set();
    }

    void cmdMemDisplay(const char *param) {
        char *str;
        void* p = (void*)strtoul(param, &str, 0); // Get address
        size_t s = strtoul(str, &str, 0);   // Get size
        s = (s ? s : 256);  // Default size
        printf("*Memory @ %p, size: %u...\n", p, s);
        printMem(p, s);
    }

    class Test {
    public:
        Test() { printf("* %s\n", __PRETTY_FUNCTION__); }
        ~Test() { printf("* %s\n", __PRETTY_FUNCTION__); }
        char str[64];
    };

    void cmdMemTest(const char *param) {
        int size = atoi(param);
        printf("*Allocating %i bytes...\n", size);
        char* str = (char*) malloc(size);
        if(!str) printf("* malloc(%i) failed!\n", size);
        free(str);
        Test *p = new Test;
        delete p;
    }

    void cmdStatistics(void) {
        printf("*Thread max run time\n");
        printf("*  SerialThread = %lu ms\n", maxThreadRunTime);
    }

    void cmdTest(const char *param) {
        printf("*Received param: %s\n", param);
        const char str[] = "*Test print of long sentence..\n";
//        size_t len = sizeof(str);
        TF::Time time(0);
        printf(str);
//        console_write(NULL, str, len);
        unsigned elapsed = time.get_elapsed_ms();
        printf("*Time spent: %u ms\n", elapsed);
    }
};

