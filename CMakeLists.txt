cmake_minimum_required(VERSION 3.13.1)

set(CONF_FILE kernel.conf)
set(KCONFIG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig)


include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(blue-bandit)


target_include_directories(app PRIVATE src)
target_sources(app PRIVATE
        src/main.cpp
        src/kernel/ble-terminal/ble-terminal.c
        )


