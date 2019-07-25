//
// Created by Andrew on 6/26/2019.
//

#include "ir_emitter.hpp"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_ppi.h"
static bool _is_carrier_ready = false;
static bool _is_carrier_used = false;
static uint32_t _ir_pins[4] = {NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};

static ero::pwm_t pwm(NRF_PWM0);
using namespace ero;
using namespace ero::ir_emitter;
static ero::ir_emitter::encoder::static_ir_seq_t<IR_EMITTER_MAX_SEQ_SIZE> _ir_seq;

bool ero::ir_emitter::in_use() {
	return _is_carrier_used;
}
ero::ir_emitter::encoder::static_ir_seq_t<IR_EMITTER_MAX_SEQ_SIZE>& ero::ir_emitter::seq() {
	return _ir_seq;
}
bool ero::ir_emitter::send_seq() {
	if (in_use())
		return false;
	_is_carrier_used = true;
	disable();
	pwm.pins_set(_ir_pins);
	pwm.enable();
	pwm.set_freq(_ir_seq.carrier_freq);
	pwm.sequence_set(0, _ir_seq.pwm_seq.seq_ptr());
	pwm.decoder_set(nrf_pwm_dec_load_t::NRF_PWM_LOAD_COMMON, nrf_pwm_dec_step_t::NRF_PWM_STEP_AUTO);
	pwm.shorts_set(nrf_pwm_short_mask_t::NRF_PWM_SHORT_SEQEND0_STOP_MASK);
	pwm.start(0);
	return true;
}
void ero::ir_emitter::init(uint32_t pin) {
	_ir_pins[0] = pin;
	_is_carrier_ready = true;
}

void ero::ir_emitter::disable() {
	pwm.disable();
}
namespace encoders {
	encoder::raw_encoder_t raw{seq()};
	encoder::nec_encoder_t nec{seq()};
}
encoder::encoder_t& ero::ir_emitter::get_encoder(encoder::ir_encoding_t encoding) {
	switch (encoding) {
		case encoder::ir_encoding_t::RAW:
			return encoders::raw;
		case encoder::ir_encoding_t::NEC:
			return encoders::nec;
		default:
			//UNRECOGNIZED IR ENCODING
			return encoders::raw;
	}
}