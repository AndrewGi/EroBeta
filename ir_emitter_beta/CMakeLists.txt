# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
set(QEMU_EXTRA_FLAGS -s)
set(BOARD "nrf52_pca10040")
set(ZEPHYR_TOOLCHAIN_VARIANT "gnuarmemb")
set(GNUARMEMB_TOOLCHAIN_PATH "E:/arm_gcc/7 2018-q2-update")
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(ero_zephyr)
zephyr_include_directories(ir_emitter/include)
target_sources(app PRIVATE
		src/main.cpp
		ir_emitter/src/ir_emitter.cpp
		ir_emitter/src/ir_emitter_model_server.cpp  )
#target_sources_ifdef(CONFIG_BOARD_BBC_MICROBIT app PRIVATE src/microbit.c)
