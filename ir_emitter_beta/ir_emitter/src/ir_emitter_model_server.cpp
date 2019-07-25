//
// Created by Andrew on 7/6/2019.
//
#include "ir_emitter.hpp"
#include <bluetooth/mesh.h>

struct emit_ir_message_t {
	uint16_t bit_len;
	uint16_t carrier_freq;
	ero::ir_emitter::encoder::ir_encoding_t encoding;
	uint8_t data[];
} __attribute__((packed));
struct status_message_t {

};
static ero::ir_emitter::emit_ir_status_t send_ir_message(const emit_ir_message_t& msg, uint32_t byte_len) {
	using ero::ir_emitter::emit_ir_status_t;
	ero::ir_emitter::seq().clear();
	if (ero::BitQueue::bit_len_to_byte(msg.bit_len)>(byte_len-sizeof(emit_ir_message_t)))
		return emit_ir_status_t::INVALID_BIT_LEN; //Invalid bit_len
	if (!ero::ir_emitter::seq().set_carrier(msg.carrier_freq))
		return emit_ir_status_t::INVALID_CARRIER_FREQ;
	const ero::BitQueue bit_queue{msg.data, msg.bit_len, 0};
	if (ero::ir_emitter::get_encoder(msg.encoding).encode(bit_queue.get_cursor()))
		return emit_ir_status_t::INVALID_ENCODING;
	if (ero::ir_emitter::seq().pwm_seq.overflowed)
		return emit_ir_status_t::INVALID_BIT_LEN;
	//TODO: check to make sure not busy
	if (!ero::ir_emitter::send_seq())
		return emit_ir_status_t::INVALID_SENDING;
	return emit_ir_status_t::OK;
}
static void handle_emit_ir_cb(bt_mesh_model* model, bt_mesh_msg_ctx* message, net_buf_simple* data) {
	if(data->size==0)
		return;
	const emit_ir_message_t& ir_message = *reinterpret_cast<const emit_ir_message_t*>(data->data);
	ero::ir_emitter::emit_ir_status_t status = send_ir_message(ir_message, data->len);
	data->size = sizeof(status);
	*reinterpret_cast<ero::ir_emitter::emit_ir_status_t*>(data->data) = status;
	message->send_ttl = BT_MESH_TTL_DEFAULT;
	bt_mesh_model_send(model, message, data, nullptr, nullptr);
}
using namespace ero::ir_emitter;
const bt_mesh_model_op ero::ir_emitter::model_handlers[] { {model_server_opcodes::emit_ir, sizeof(emit_ir_message_t), handle_emit_ir_cb} };
