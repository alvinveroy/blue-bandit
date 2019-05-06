//
// Created by 0xDC on 4/21/19.
//


#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/printk.h>
#include <misc/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <kernel/ble-terminal/service.h>

#define BT_UUID_SERVICE_UART  BT_UUID_DECLARE_16(0xbb00)
#define BT_UUID_CHARCTR_RX    BT_UUID_DECLARE_16(0xdc01)
#define BT_UUID_CHARCTR_TX    BT_UUID_DECLARE_16(0xdc02)


// As defined in bt core spec. See Long Attributes
#define MAX_LEN_CHARCTR 512
#define BUF_SIZE 20
static u8_t rx_buf[BUF_SIZE] = {0};
static u8_t tx_buf[BUF_SIZE] = {0};


static struct bt_gatt_ccc_cfg  server_rx_cfg[BT_GATT_CCC_MAX] = {};
static struct bt_gatt_ccc_cfg  server_tx_cfg[BT_GATT_CCC_MAX] = {};

static u8_t server_rx_active = true;
static u8_t server_tx_active = true;

// TODO: Allow client to change these? Or get rid of CCC
static void server_rx_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    server_rx_active = true;//(value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

// TODO: Allow client to change these?
static void server_tx_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    server_tx_active = true;//(value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}


static ssize_t server_rx_process(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                void *buf, u16_t len, u16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

static ssize_t server_tx_process(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                void *buf, u16_t len, u16_t offset)
{
    const char *value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

/* Battery Service Declaration */
static struct bt_gatt_attr attrs_uart[] = {
        BT_GATT_PRIMARY_SERVICE(BT_UUID_SERVICE_UART),
        BT_GATT_CHARACTERISTIC(BT_UUID_CHARCTR_RX,
                               BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                               BT_GATT_PERM_WRITE,
                               NULL,
                               server_rx_process,
                               &rx_buf),
        BT_GATT_CCC(server_rx_cfg, server_rx_cfg_changed),
        BT_GATT_CHARACTERISTIC(BT_UUID_CHARCTR_TX,
                               BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                               BT_GATT_PERM_READ,
                               server_tx_process,
                               NULL,
                               &tx_buf),
        BT_GATT_CCC(server_tx_cfg, server_tx_cfg_changed),
};

static struct bt_gatt_service service_uart = BT_GATT_SERVICE(attrs_uart);

void
server_uart_init(void)
{
    bt_gatt_service_register(&service_uart);
}

static u8_t mcount = 0;
int
server_uart_tx_notify(void)
{
    if (!server_tx_active) return -1;

    sprintf(tx_buf, "hello %d\n", mcount++);
    return bt_gatt_notify(NULL, &attrs_uart[4], tx_buf, sizeof(tx_buf));
}
