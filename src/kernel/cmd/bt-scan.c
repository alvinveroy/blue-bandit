//
// Created by 0xDC on 4/11/19.
//

#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr.h>
#include <misc/printk.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <misc/byteorder.h>
#include <shell/shell.h>

#include "bt-scan.h"

static struct bt_conn *default_conn;

void
get_advertised_fullname(struct net_buf_simple * ad, char * name)
{
    if (ad->len < 2) return;

    for (s32_t i=0; i<ad->len; i++)
    {
        // Parse component advertisement structures
        u8_t sublen  = ad->data[i];
        u8_t subtype = ad->data[i+1];
        if (subtype == BT_DATA_NAME_COMPLETE)
        {
            for (s32_t j=0; j < sublen; j++) name[j] = ad->data[j];
            name[sublen] = '\0';
            i += sublen;
        }
    }
}

static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    char name[32 + 1] = "n/a";

    if (default_conn) return;

    /* We're only interested in connectable events */
    //if (type != BT_LE_ADV_IND && type != BT_LE_ADV_DIRECT_IND) {
    //    return;
    //}

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    get_advertised_fullname(ad, name);
    //TODO: Should the printing ref a specific shell instance?
    printk("LE-Ad: %d %s@%s (RSSI %d)  ---- Rx %d B\n", type, name, addr_str, rssi, ad->len);

    /* connect only to devices in close proximity */
    //if (rssi < -70) return;

}

static void connected(struct bt_conn *conn, u8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        printk("Failed to connect to %s (%u)\n", addr, err);
        return;
    }

    if (conn != default_conn) {
        return;
    }

    printk("Connected: %s\n", addr);

    bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int err;

    if (conn != default_conn) {
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printk("Disconnected: %s (reason %u)\n", addr, reason);

    bt_conn_unref(default_conn);
    default_conn = NULL;

    /* This demo doesn't require active scan */
    err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found);
    if (err) {
        printk("Scanning failed to start (err %d)\n", err);
    }
}

static struct bt_conn_cb conn_callbacks = {
        .connected = connected,
        .disconnected = disconnected,
};

//Desc: Register connection callbacks and start an 'active' scan. In BT world, this means that the scanning host
//      can message picked up devices to request for more information, whereas passive scan can only receive
//      advertised data.
void
start_active_scan(void)
{
    bt_conn_cb_register(&conn_callbacks);

    struct bt_le_scan_param scantype = {
        .type = BT_HCI_LE_SCAN_ACTIVE,
        .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_ENABLE,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,
        .window = BT_GAP_SCAN_FAST_WINDOW

    };

    int err = bt_le_scan_start(&scantype, device_found);
    if (err) printk("Scanning failed to start (err %d)\n", err);
    else     printk("Scanning successfully started\n");
}


static int
bb_scan_list(const struct shell *shell, size_t argc, char ** argv)
{
    s8_t scan_duration = strtol(argv[1], 0, 10);

    if (scan_duration <= 0 || scan_duration > 10)
    {
        s32_t default_window = 10;
        shell_print(shell, "Duration Interval: [0,10]. Defaulting to %ds", default_window);
        scan_duration = default_window;
    }

    start_active_scan();
    k_sleep(625 * scan_duration);
    bt_le_scan_stop();

    return 0;
}

// TODO: bb_scan list is not using my passed in args
SHELL_STATIC_SUBCMD_SET_CREATE(subcmd_bb_scan,
    SHELL_CMD_ARG(list, NULL, "Scan for n seconds. Max = 10s (ex. bb_scan list 3)", bb_scan_list, 1, 1),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(bb_scan, &subcmd_bb_scan, "BB Scan Commands", NULL);