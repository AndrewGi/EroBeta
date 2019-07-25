/* main.c - Application main entry point */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <misc/printk.h>
#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include "ir_emitter.hpp"

#include "board.h"
static constexpr bt_mesh_cfg_srv _make_cfgsrv() {
	bt_mesh_cfg_srv out {};
	out.net_transmit = BT_MESH_TRANSMIT(2, 20),
	out.relay = BT_MESH_RELAY_DISABLED,
	out.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
	out.beacon = BT_MESH_BEACON_ENABLED,
	#if defined(CONFIG_BT_MESH_GATT_PROXY)
		out.gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
	#else
		out.gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
	#endif
	#if defined(CONFIG_BT_MESH_FRIEND)
		out.frnd = BT_MESH_FRIEND_ENABLED,
	#else
		out.frnd = BT_MESH_FRIEND_NOT_SUPPORTED,
	#endif
	out.default_ttl = 7;
	return out;
}
static struct bt_mesh_cfg_srv cfg_srv = _make_cfgsrv();
static struct bt_mesh_health_srv health_srv = {
};
static constexpr uint32_t max_faults = 0;
NET_BUF_SIMPLE_DEFINE_STATIC(bt_mesh_pub_msg_health_pub, 1 + 3 + max_faults);
static constexpr bt_mesh_model_pub _make_healthpub() {
	bt_mesh_model_pub out {};
	out.update = nullptr;
	out.msg = &bt_mesh_pub_msg_health_pub;
	return out;
}
static struct bt_mesh_model_pub health_pub = _make_healthpub();

static struct bt_mesh_model_pub gen_level_pub;
static struct bt_mesh_model_pub gen_onoff_pub;

static void gen_onoff_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
}

static void gen_onoff_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op gen_onoff_op[] = {
	{ BT_MESH_MODEL_OP_2(0x82, 0x01), 0, gen_onoff_get },
	{ BT_MESH_MODEL_OP_2(0x82, 0x02), 2, gen_onoff_set },
	{ BT_MESH_MODEL_OP_2(0x82, 0x03), 2, gen_onoff_set_unack },
	BT_MESH_MODEL_OP_END,
};

static void gen_level_get(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
}

static void gen_level_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
}

static void gen_level_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf)
{
}

static void gen_delta_set(struct bt_mesh_model *model,
			  struct bt_mesh_msg_ctx *ctx,
			  struct net_buf_simple *buf)
{
}

static void gen_delta_set_unack(struct bt_mesh_model *model,
				struct bt_mesh_msg_ctx *ctx,
				struct net_buf_simple *buf)
{
}

static void gen_move_set(struct bt_mesh_model *model,
			 struct bt_mesh_msg_ctx *ctx,
			 struct net_buf_simple *buf)
{
}

static void gen_move_set_unack(struct bt_mesh_model *model,
			       struct bt_mesh_msg_ctx *ctx,
			       struct net_buf_simple *buf)
{
}

static const struct bt_mesh_model_op gen_level_op[] = {
	{ BT_MESH_MODEL_OP_2(0x82, 0x05), 0, gen_level_get },
	{ BT_MESH_MODEL_OP_2(0x82, 0x06), 3, gen_level_set },
	{ BT_MESH_MODEL_OP_2(0x82, 0x07), 3, gen_level_set_unack },
	{ BT_MESH_MODEL_OP_2(0x82, 0x09), 5, gen_delta_set },
	{ BT_MESH_MODEL_OP_2(0x82, 0x0a), 5, gen_delta_set_unack },
	{ BT_MESH_MODEL_OP_2(0x82, 0x0b), 3, gen_move_set },
	{ BT_MESH_MODEL_OP_2(0x82, 0x0c), 3, gen_move_set_unack },
	BT_MESH_MODEL_OP_END,
};
static constexpr bt_mesh_model make_bt_model(u16_t id, const bt_mesh_model_op* op, bt_mesh_model_pub* pub, void* user_data) {
	return bt_mesh_model {
			{id},
			0,
			0,
			0,
			pub,
			{},
			{},
			op,
			user_data
	};
}
static constexpr bt_mesh_model make_bt_vender_model(uint16_t company_id, u16_t id, const bt_mesh_model_op* op, bt_mesh_model_pub* pub, void* user_data) {

	bt_mesh_model out {
			{},
			0,
			0,
			0,
			pub,
			{},
			{},
			op,
			user_data
	};
	out.vnd.company = company_id;
	out.vnd.id=id;
	return out;
}

static struct bt_mesh_model root_models[] = {
	make_bt_model(BT_MESH_MODEL_ID_CFG_SRV, bt_mesh_cfg_srv_op, nullptr, &cfg_srv),
	make_bt_model(BT_MESH_MODEL_ID_HEALTH_SRV, bt_mesh_health_srv_op, &health_pub, &health_srv),
	make_bt_model(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op,
		      &gen_onoff_pub, NULL),
	make_bt_model(BT_MESH_MODEL_ID_GEN_LEVEL_SRV, gen_level_op,
		      &gen_level_pub, NULL),
	make_bt_vender_model(ero::ir_emitter::COMPANY_ID, ero::ir_emitter::MODEL_SERVER_ID, ero::ir_emitter::model_handlers, nullptr, nullptr)
};

template<uint32_t mod_size, uint32_t vnd_mod_size>
static constexpr bt_mesh_elem make_bt_element(u16_t loc, bt_mesh_model (&models)[mod_size], bt_mesh_model (&vnd_models)[vnd_mod_size]) {
	return {.loc = loc, .model_count=mod_size,.vnd_model_count=vnd_mod_size, .models=models,.vnd_models=vnd_models};
}
static bt_mesh_elem elements[] = {
		bt_mesh_elem {
			0,
			0,
			ARRAY_SIZE(root_models),
			0,
			root_models,
			nullptr
		}
};

static const struct bt_mesh_comp comp = {
	BT_COMP_ID_LF,
	0,
	0,
	ARRAY_SIZE(elements),
	elements,
};

static int output_number(bt_mesh_output_action_t action, u32_t number)
{
	printk("OOB Number: %u\n", number);

	board_output_number(action, number);

	return 0;
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
	board_prov_complete();
}

static void prov_reset(void)
{
	bt_mesh_prov_enable(static_cast<bt_mesh_prov_bearer_t>(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT));
}

static const uint8_t dev_uuid[16] = { 0xdd, 0xdd };
static const uint8_t oob[] = {7, 7, 3, 7};
static const struct bt_mesh_prov prov = {
		dev_uuid,
		nullptr,
		bt_mesh_prov_oob_info_t::BT_MESH_PROV_OOB_NUMBER, //OOB info
		oob, //OOB static value
		sizeof(oob), //OOB static value length
		4, //output size
		BT_MESH_NO_OUTPUT, //output actions
		0,
		BT_MESH_NO_INPUT,
		output_number,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		prov_complete,
		prov_reset,
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	board_init();

	err = bt_mesh_init(&prov, &comp);
	if (err) {
		printk("Initializing mesh failed (err %d)\n", err);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	/* This will be a no-op if settings_load() loaded provisioning info */
	bt_mesh_prov_enable(static_cast<bt_mesh_prov_bearer_t>(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT));

	printk("Mesh initialized\n");
}

void main(void)
{
	int err;

	printk("Initializing...\n");
	printk("Starting ir emitter\n");
	ero::ir_emitter::init(2);
	printk("Ero ready!\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
	printk("Node ready!\n");
}
