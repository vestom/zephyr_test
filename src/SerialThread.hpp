#include "TF/Log.h"
#include "TF/Serial.h"
#include "TF/Thread.h"
#include "TF/Time.h"
#include <string.h>

#define LED2_PORT   DT_ALIAS_LED2_GPIOS_CONTROLLER
#define LED2_PIN    DT_ALIAS_LED2_GPIOS_PIN
#define SERIAL_UART "UART_1"

class SerialThread : public TF::Thread {
    void run() {
        char tx_buffer[32];
        char rx_buffer[32];
        char str[32];
        int n=0;

        TF::Log::debug("SerialThread");

        serial.open(SERIAL_UART, 115200);
        serial.flushRxBuffer();

        auto gpio_led = TF::GPIO(LED2_PORT, LED2_PIN);

        run_period.set_timeout_ms(100);  // 10Hz
        run_period.reset();

        while(1) {
            // Periodic loop
            maxThreadRunTime = MAX(maxThreadRunTime, run_period.get_elapsed_ms());
            while(run_period.get_remaining_ms() > 0) {
                TF::Thread::sleep_ms(run_period.get_remaining_ms());
            }
            run_period.reset();

            gpio_led.set(!gpio_led.get());

            // RX
            int bytes = serial.getRxBytes();
            if(bytes) {
                serial.read((uint8_t *)rx_buffer, bytes);
                rx_buffer[bytes] = 0;   // safe string termination..

                // Bin2Hex
                for (int n=0; n<bytes; ++n) {
                    snprintf(str+n*2, sizeof(str)-n*2, "%02X ", (unsigned)rx_buffer[n]);
                }
                //TF::Log::debug("Received: %s, [%u] %s", rx_buffer, bytes, str);
                TF::Log::debug("Received: %s, [%u]", rx_buffer, bytes);
            }

            // TX
            snprintf(tx_buffer, sizeof(tx_buffer), "Test %2i", n++);
            TF::Log::debug("Sending: %s", tx_buffer);
            serial.write((uint8_t*) tx_buffer, strlen(tx_buffer));
        }
        // Test queue
//      queue.send((uint8_t*)"MSG1", 256);
//      queue.send((uint8_t*)"MSG2", 8);
    }

    TF::Serial serial;
    TF::Time run_period;

#if 0
//#ifdef __linux__
    public:
        void operator delete(void* v) {};
#endif
};

