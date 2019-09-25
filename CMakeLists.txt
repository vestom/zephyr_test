# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(ztest)


target_include_directories(app PRIVATE include)
target_include_directories(app PRIVATE src)

# TinyFramework
set(TF_TARGET app)
add_subdirectory(modules/TinyFramework)


target_sources(app PRIVATE src/main.cpp)

