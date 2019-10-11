#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"
#include <string.h>

class SerialThread : public TF::Thread {
    void run() {
        TF::Log::debug("SerialThread");

        serial.open();
        serial.flushRxBuffer();
        char tx_buffer[32];
        char rx_buffer[32];
        char str[32];

        int n=0;

        while(1) {
            TF::Thread::sleep_ms(500);
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
//			queue.send((uint8_t*)"MSG1", 256);
//			queue.send((uint8_t*)"MSG2", 8);
    }

    TF::Serial serial;

#if 0
//#ifdef __linux__
    public:
        void operator delete(void* v) {};
#endif
};

