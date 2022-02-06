# Description

Test project for using TinyFramwork with ZephyrOS.

The project has 3 threads:
1. The main thread: For various simple tests. Prints to stdout.
2. SeralThread: Tests TFs Serial module. Will periodically print out what is received and send a test message.
3. ConsoleThread: Implements a simple console using stdin/stdout.

## Setting up
```
pip3 install --user -U west
pip3 install --user -r ~/Projects/zephyrproject/zephyr/scripts/requirements.txt
```


## Building:

```
source $HOME/Projects/zephyrproject/zephyr/zephyr-env.sh
west build -b stm32f4_disco .
west build -b stm32_min_dev_blue .
west build -b native_posix_64 . -d build_posix

```

Downloading: `west flash`
Debugging: `west debugserver`
Config: `west build -t guiconfig`


## Platforms

### STM32F4Discovery
* Console is on UART2 (TX: PA2, RX: PA3)
* SerialThread is UART1 (TX: PB6, RX: PB7)
