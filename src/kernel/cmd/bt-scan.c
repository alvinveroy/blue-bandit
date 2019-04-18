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


#define TAB_WIDTH 4

// Advertisement packet info See
//  BLE:     BT 4 Core Spec v6 Vol.2 Part B Section 2 --> Air Interface Packets
//  Classic: BT 4 Core Spec
static void
get_advertised_fullname(struct net_buf_simple * ad, char * name)
{
    if (ad->len < 2) return;

    for (s32_t i=0; i<ad->len;)
    {
        // Parse component advertisement structures
        u8_t sublen   = ad->data[i] - 1; // Length also includes type hence the -1
        u8_t subtype  = ad->data[i+1];
        u8_t substart = i+2;
        if (subtype == BT_DATA_NAME_COMPLETE || subtype == BT_DATA_NAME_SHORTENED)
        {
            for (s32_t j=0; j<sublen; j++) name[j] = ad->data[substart + j];
            name[sublen] = '\0';
            i = substart + sublen;
        } else i++;
    }
}

static const char *
strAdType(u8_t ad_type)
{
    switch(ad_type)
    {
        case BT_LE_ADV_IND:                 return "LE Undirected Connectable";
        case BT_LE_ADV_DIRECT_IND:          return "LE Directed Connectable";
        case BT_LE_ADV_SCAN_IND:            return "LE Scannable Non-connectable";
        case BT_LE_ADV_NONCONN_IND:         return "LE Non-connectable";
        case BT_LE_ADV_DIRECT_IND_LOW_DUTY: return "LE Low-duty Directed Connectable";
        default:                            return "LE Unspecified";
    }
}

// See GAP EIR/AD/OOB data type numbers
// https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
static char *
strDataType(u8_t gap_data_type)
{
    switch(gap_data_type)
    {
        case BT_DATA_FLAGS:             return "Flags";
        case BT_DATA_UUID16_SOME:       return "Partial 2B Service UUID List";
        case BT_DATA_UUID16_ALL:        return "2B Service UUID List";
        case BT_DATA_UUID32_SOME:       return "Partial 4B Service UUID List";
        case BT_DATA_UUID32_ALL:        return "4B Service UUID List";
        case BT_DATA_UUID128_SOME:      return "Partial 16B Service UUID List";
        case BT_DATA_UUID128_ALL:       return "16B Service UUID List";
        case BT_DATA_NAME_SHORTENED:    return "Shortened Local Name";
        case BT_DATA_NAME_COMPLETE:     return "Complete Local Name";
        case BT_DATA_TX_POWER:          return "TX Power";
        case BT_DATA_SOLICIT16:         return "Solicit 2B UUID List";
        case BT_DATA_SOLICIT128:        return "Solicit 16b UUID List";
        case BT_DATA_SVC_DATA16:        return "2B Service Data";
        case BT_DATA_GAP_APPEARANCE:    return "GAP Appearance";
        case BT_DATA_SOLICIT32:         return "4B Solicit UUID List";
        case BT_DATA_SVC_DATA32:        return "4B Service Data";
        case BT_DATA_SVC_DATA128:       return "16B Service Data";
        case BT_DATA_MANUFACTURER_DATA: return "Manufacturer Data";
        default:                        return "Unspecifed GAP Data Type";
    }
}

static void
printAdSubTypes(struct net_buf_simple * ad)
{
    printk("    Ad Data:\n");

    if (!ad || ad->len < 3) return;

    for (s32_t i=0; i<ad->len;)
    {
        u8_t sublen  = ad->data[i] - 1; // Don't can't type byte
        u8_t subtype = ad->data[i+1];
        u8_t start   = i+2;

        printk("        %s (0x%x)\n", strDataType(subtype), subtype);
        i += start + sublen;
    }
}

static void
device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t type,
                         struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    char name[32 + 1] = "n/a";

    if (default_conn) return;

    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    get_advertised_fullname(ad, name);

    //TODO: Should the printing ref a specific shell instance?
    printk("%s@%s (RSSI %d)  ---- Rx %d B\n"
           "    Advertising: %s (%d)\n",
           name, addr_str, rssi, ad->len,
           strAdType(type), type);
    printAdSubTypes(ad);
    printk("\n");

    /* connect only to devices in close proximity */
    //if (rssi < -70) return;

}

static void
connected(struct bt_conn *conn, u8_t err)
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

static void
disconnected(struct bt_conn *conn, u8_t reason)
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

//Desc: Register connection callbacks and start an scan. In BT world, this means that the scanning host
//      can message picked up devices to request for more information, whereas passive scan can only receive
//      advertised data.

static int
cmd_scan(const struct shell *shell, size_t argc, char ** argv, u8_t scan_type)
{
    // Parse command inputs, setting default values if necessary
    s8_t scan_duration = strtol(argv[1], 0, 10);
    if (scan_duration <= 0 || scan_duration > 10)
    {
        s32_t default_window = 10;
        shell_print(shell, "Duration Interval: [0,10]. Defaulting to %ds", default_window);
        scan_duration = default_window;
    }

    // Register callback, start then stop command after some time
    bt_conn_cb_register(&conn_callbacks);

    struct bt_le_scan_param scanparam = {
            .type = scan_type,
            .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_ENABLE,
            .interval = BT_GAP_SCAN_FAST_INTERVAL,
            .window = BT_GAP_SCAN_FAST_WINDOW
    };

    s32_t err = bt_le_scan_start(&scanparam, device_found);
    if(err) printk("Scanning failed to start (err %d)\n", err);
    k_sleep(1000 * scan_duration);
    bt_le_scan_stop();
    return err;
}

static int
cmd_scan_passive(const struct shell * shell, size_t argc, char ** argv)
{
    return cmd_scan(shell, argc, argv, BT_HCI_LE_SCAN_PASSIVE);
}

static int
cmd_scan_active(const struct shell * shell, size_t argc, char ** argv)
{
    return cmd_scan(shell, argc, argv, BT_HCI_LE_SCAN_ACTIVE);
}

// TODO: bb_scan list is not using my passed in args
SHELL_STATIC_SUBCMD_SET_CREATE(subcmd_bb_scan,
    SHELL_CMD_ARG(passive, NULL, "Scan for n seconds. Max = 10s (ex. bb_scan passive 3)", cmd_scan_passive, 1, 1),
    SHELL_CMD_ARG(active, NULL, "Scan for n seconds. Max = 10s (ex. bb_scan active 3)", cmd_scan_active, 1, 1),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(bb_scan, &subcmd_bb_scan, "BB Scan Commands", NULL);