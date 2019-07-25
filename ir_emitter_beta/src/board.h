/* board.h - Board-specific hooks */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

static inline void board_output_number(bt_mesh_output_action_t action,
				       u32_t number)
{
	printk("pairing number!: %u\n", number);
}

static inline void board_prov_complete(void)
{
}

static inline void board_init(void)
{
}
