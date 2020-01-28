# Description

Building:

```
source $HOME/Projects/zephyrproject/zephyr/zephyr-env.sh
west build -b stm32f4_disco .
west build -b stm32_min_dev_blue .
west build -b native_posix_64 . -d build_posix

```
