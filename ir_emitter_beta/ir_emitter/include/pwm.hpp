#pragma once

#include "nrf_pwm.h"
#include "nrf_gpio.h"
#include <string.h>
namespace ero {
	template<class T, uint32_t _max_raw_length> struct static_pwm_seq_t {
		static constexpr uint32_t max_raw_length() {
			return _max_raw_length;
		}
		static constexpr uint32_t period_size() {
			return sizeof(T)/sizeof(uint16_t);
		}
		static_assert(max_raw_length()>0);
		uint16_t values[max_raw_length()];
		nrf_pwm_sequence_t sequence;
		bool overflowed = false;
		static_pwm_seq_t(uint16_t length, uint16_t repeats) : sequence{{(nullptr)}, length, repeats, 0} {
			sequence.values.p_raw = values;
		};
		static_pwm_seq_t() : static_pwm_seq_t(0, 0) {};
		uint16_t* end() {
			return &values[sequence.length];
		}
		void clear() {
			//TODO: Do we need to zero the values?
			memset(values, 0, sizeof(uint16_t)*max_raw_length());
			sequence.length = 0;
			overflowed = false;
		}
		bool insert(T value) {
			if (overflowed || (sequence.length+period_size()) > max_raw_length()) {
				overflowed = true;
				return false; //Out of bounds
			}
			*reinterpret_cast<T*>(end()) = value;
			sequence.length += period_size();
			return true;
		}
		const nrf_pwm_sequence_t* seq_ptr() const {
			return &sequence;
		}
	};
	struct pwm_t {
		NRF_PWM_Type* reg;
		nrf_pwm_clk_t _clk;
		nrf_pwm_mode_t _mode = nrf_pwm_mode_t::NRF_PWM_MODE_UP;
		bool is_enabled = false;
		constexpr pwm_t(NRF_PWM_Type* reg, nrf_pwm_clk_t clk = nrf_pwm_clk_t::NRF_PWM_CLK_1MHz) : reg(reg), _clk(clk) {}
		void configure(uint16_t top_value) {
			nrf_pwm_configure(reg, _clk, _mode, top_value);
		}
		void pins_set(uint32_t pins[NRF_PWM_CHANNEL_COUNT]) {
			uint32_t* start = pins;
			while (start!=&pins[NRF_PWM_CHANNEL_COUNT]) {
				if (*start != NRF_PWM_PIN_NOT_CONNECTED) {
					nrf_gpio_pin_clear(*start);
					nrf_gpio_cfg_output(*start);
				}
				start++;
			}
			nrf_pwm_pins_set(reg, pins);
		}
		void sequence_set(uint8_t seq_id, nrf_pwm_sequence_t const * sequence) {
			nrf_pwm_sequence_set(reg, seq_id, sequence);
		}
		void decoder_set(nrf_pwm_dec_load_t load, nrf_pwm_dec_step_t step) {
			nrf_pwm_decoder_set(reg, load, step);
		}
		void shorts_set(nrf_pwm_short_mask_t mask) {
			nrf_pwm_shorts_set(reg, mask);
		}
		void enable() {
			if (is_enabled)
				return;
			nrf_pwm_enable(reg);
			is_enabled = true;
		}
		void disable() {
			if (!is_enabled)
				return;
			nrf_pwm_disable(reg);
			is_enabled = false;
		}
		void loop_set(uint16_t count) {
			nrf_pwm_loop_set(reg, count);
		}
		void start(uint8_t seq_id) {
			if (seq_id > 2)
				return;
			task_trigger(seq_id==0?nrf_pwm_task_t::NRF_PWM_TASK_SEQSTART0:nrf_pwm_task_t::NRF_PWM_TASK_SEQSTART1);
		}
		void stop() {
			task_trigger(nrf_pwm_task_t::NRF_PWM_TASK_STOP);
		}
		void next_step() {
			task_trigger(nrf_pwm_task_t::NRF_PWM_TASK_NEXTSTEP);
		}
		void task_trigger(nrf_pwm_task_t task) {
			nrf_pwm_task_trigger(reg, task);
		}
		void set_freq(uint32_t freq) {
			uint32_t clk_freq = (16000000ul)>>static_cast<uint32_t>(_clk);
			configure(clk_freq/freq);
		}
		void transmit(const nrf_pwm_sequence_t* seq) {
			uint8_t seq_id = 0;
			nrf_pwm_shorts_set(reg, nrf_pwm_short_mask_t::NRF_PWM_SHORT_LOOPSDONE_STOP_MASK);
			sequence_set(seq_id, seq);
			start(seq_id);
		}
	};
}