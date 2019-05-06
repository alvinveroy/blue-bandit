cmake_minimum_required(VERSION 3.13.1)

set(CONF_FILE kernel.conf)
set(KCONFIG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig)


include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(blue-bandit)


target_include_directories(app PRIVATE
        src)

target_sources(app PRIVATE
        src/kernel/cmd/bt-scan.c
        src/kernel/cmd/bt-pose.c
        src/kernel/cmd/bt-info.c
        src/main.cpp
        src/kernel/ble-terminal/ble-terminal.c
        src/kernel/ble-terminal/service.c
        src/kernel/gatt/bas.c
        src/kernel/gatt/hrs.c
        src/kernel/gatt/cts.c
        )

