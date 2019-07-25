#pragma once
#include <bluetooth/mesh.h>
#include "pwm.hpp"
namespace ero {
	namespace ir_emitter {
		enum class emit_ir_status_t : uint16_t {
			OK,
			BUSY,
			INTERRUPTED,
			OTHER,

			INVALID_BIT_LEN,
			INVALID_CARRIER_FREQ,
			INVALID_ENCODING,
			INVALID_SENDING,
		};
		constexpr uint16_t MODEL_SERVER_ID = 0x0012;
		constexpr uint16_t COMPANY_ID = 0x7737;
		namespace model_server_opcodes {
			constexpr static u32_t opcode(uint8_t op) {
				return BT_MESH_MODEL_OP_3(op, COMPANY_ID);
			}
			constexpr static u32_t emit_ir = opcode(0x02);
			constexpr static u32_t emit_ir_status = opcode(0x03);
		}

		static constexpr nrf_pwm_clk_t pwm_clk = nrf_pwm_clk_t::NRF_PWM_CLK_1MHz;
		static constexpr uint32_t IR_PWM_FREQ = (16000000ul)>>static_cast<uint32_t>(pwm_clk);

	}
}