//
// Created by 0xDC on 4/23/19.
//

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr.h>
#include <misc/printk.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <shell/shell.h>

#include "bt-scan.h"


static void
cmd_info(const struct shell * shell, size_t argc, char ** argv)
{
    // Misc Info
    shell_print(shell, "Bluetooth: %s", bt_is_enabled()? "ON" : "OFF");

    // Print name and all advertised bluetooth access IDs
    bt_addr_le_t addrs[CONFIG_BT_ID_MAX] = {0};
    size_t count_addrs = sizeof(addrs);
    const char * name  = bt_get_name();

    bt_id_get(&addrs, &count_addrs);
    char addr_strbuf[BT_ADDR_LE_STR_LEN]= {0};
    for (u8_t i=0; i<count_addrs; i++)
    {
        bt_addr_le_to_str(&addrs[i], &addr_strbuf, sizeof(addr_strbuf));
        shell_print(shell, "%s@%s", name, addr_strbuf);
    }
}

SHELL_CMD_REGISTER(bb_info, NULL, "BB Bluetooth Setup", cmd_info);
