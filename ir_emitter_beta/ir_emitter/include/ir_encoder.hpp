#pragma once


#include "ir_emitter_defines.hpp"
#include "bit_queue.hpp"
#include "pwm.hpp"
namespace ero {
	namespace ir_emitter {
		namespace encoder {
			struct ir_seq_t {
				static constexpr uint16_t blank_pulse = 0x8000;
				static constexpr uint32_t min_freq = 1000;
				static constexpr uint32_t max_freq = 100000;
				uint32_t carrier_freq;
				uint16_t mark_duty_cycle;
				uint16_t space_duty_cycle = blank_pulse;
				ir_seq_t(uint32_t carrier) : carrier_freq(carrier), mark_duty_cycle(IR_PWM_FREQ/(carrier*2)) {}
				bool set_carrier(uint32_t new_carrier_freq) {
					if (new_carrier_freq < min_freq || new_carrier_freq > max_freq) {
						return false;
					}
					carrier_freq = new_carrier_freq;
					mark_duty_cycle = IR_PWM_FREQ/(carrier_freq*2); //50% duty cycle
					return true;
				}
				void mark() {
					insert(mark_duty_cycle);
				}
				void space() {
					insert(space_duty_cycle);
				}
				virtual void set_pulse_per_markspace(uint16_t pulses) = 0;
				virtual bool insert(uint16_t duty_cycle) = 0;
				virtual void clear() = 0;

			};
			template<uint32_t _max_raw_length> struct static_ir_seq_t : ir_seq_t {
				static constexpr uint16_t blank_pulse = 0x8000;
				static_pwm_seq_t<uint16_t, _max_raw_length> pwm_seq;
				static_ir_seq_t(uint16_t pulses_per_markspace, uint32_t carrier_freq) : ir_seq_t(carrier_freq), pwm_seq(0, pulses_per_markspace) {
				}
				static_ir_seq_t() : static_ir_seq_t(32, 36000) {}
				void set_pulse_per_markspace(uint16_t pulses) override {
					if (pulses == 0)
						return;
					pwm_seq.sequence.repeats = pulses;
					pwm_seq.sequence.end_delay = pulses;
				}
				bool insert(uint16_t duty_cycle) override {
					return pwm_seq.insert(duty_cycle);
				}
				void clear() override {
					pwm_seq.clear();
				}
			};
			enum class ir_encoding_t : uint8_t {
				RAW = 0,
				NEC = 1,
				END = 2
			};
			struct bit_t {
				uint16_t marks = 0;
				uint16_t spaces = 0;
				bool start_low = false; //marks first = false, spaces first = true
				bool is_zero() const {
					return marks == 0 && spaces == 0;
				}
			};
			struct encoder_t {
				ir_seq_t& seq;
				encoder_t(ir_seq_t& seq) : seq(seq) {}
				virtual bool encode(BitQueue::cursor_t cursor) {
					while(!cursor.is_done()) {
						if (cursor.next())
							mark();
						else
							space();
					}
					space();
					return true;
				}
				void mark() {
					seq.mark();
				}
				void space() {
					seq.space();
				}
			};
			template<class T>
			T make_encoder(ir_seq_t& seq) {
				T {encoder_t{seq}};
			}
			struct raw_encoder_t : encoder_t {
				raw_encoder_t(ir_seq_t& seq) : encoder_t(seq) {}
				bool encode(BitQueue::cursor_t cursor) override {
					uint8_t first_byte = cursor.aligned_byte();
					if (first_byte==0)
						return false;
					seq.set_pulse_per_markspace(first_byte);
					encoder_t::encode(cursor);
				}
			};
			struct nec_encoder_t : encoder_t {
				constexpr static uint32_t nec_carrier() {
					return 38000;
				}
				nec_encoder_t(ir_seq_t& seq) : encoder_t(seq) {}
			private:
				void leader() {
					for(uint32_t i = 0; i<16; i++)
						seq.mark();

					for(uint32_t i = 0; i<8; i++)
						seq.space();
				}
				void one() {
					seq.mark();
					seq.space();
					seq.space();
				}
				void zero() {
					seq.mark();
					seq.space();
				}
			public:
				bool encode(BitQueue::cursor_t cursor) override {
					seq.set_pulse_per_markspace(21);
					//seq.set_carrier(nec_carrier());
					leader();
					while(!cursor.is_done()) {
						if (cursor.next()) {
							one();
						} else {
							zero();
						}
						//Trailing mark after each bit
						seq.mark();
					}
					seq.space();
					return true;
				}
				bool encode_command(uint8_t address, uint8_t command) {
					uint8_t out[4] = {address, static_cast<uint8_t>(~address), command, static_cast<uint8_t>(~command)};
					auto queue = make_bit_queue(out);
					return encode(queue.get_cursor());
				}
			};
		}
	}
}