//
// Created by Andrew on 6/26/2019.
//

#include "nrf52.h"
struct ero_timer_t {
	NRF_TIMER_Type timer;
	void start() {
		timer.TASKS_START =
	}
};
