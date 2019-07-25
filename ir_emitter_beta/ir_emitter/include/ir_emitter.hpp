#pragma once
#include "ir_encoder.hpp"

namespace ero {
	namespace ir_emitter {
		constexpr uint32_t IR_EMITTER_MAX_SEQ_SIZE = 64;

		encoder::static_ir_seq_t<IR_EMITTER_MAX_SEQ_SIZE>& seq();

		void init(uint32_t pin);


		bool in_use();

		void _trigger();


		bool send_seq();

		void set_repeats(uint32_t times, uint32_t delay);

		void disable();

		bool is_carrier_on();

		encoder::encoder_t& get_encoder(encoder::ir_encoding_t encoding);

		extern const bt_mesh_model_op model_handlers[];
	}
}