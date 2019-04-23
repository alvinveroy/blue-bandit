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

static struct bt_gatt_ccc_cfg hrmc_ccc_cfg[BT_GATT_CCC_MAX] = {};
static u8_t simulate_hrm;
static u8_t heartrate = 90U;
static u8_t hrs_blsc;




static void hrmc_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                 u16_t value)
{
    simulate_hrm = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, u16_t len, u16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &hrs_blsc,
                             sizeof(hrs_blsc));
}

/* Heart Rate Service Declaration */
static struct bt_gatt_attr attrs[] = {
        BT_GATT_PRIMARY_SERVICE(BT_UUID_HRS),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
                               BT_GATT_PERM_NONE, NULL, NULL, NULL),
        BT_GATT_CCC(hrmc_ccc_cfg, hrmc_ccc_cfg_changed),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_BODY_SENSOR, BT_GATT_CHRC_READ,
                               BT_GATT_PERM_READ, read_blsc, NULL, NULL),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_CONTROL_POINT, BT_GATT_CHRC_WRITE,
                               BT_GATT_PERM_NONE, NULL, NULL, NULL),
};

static struct bt_gatt_service hrs_svc = BT_GATT_SERVICE(attrs);

void hrs_init(u8_t blsc)
{
    hrs_blsc = blsc;

    bt_gatt_service_register(&hrs_svc);
}

void hrs_notify(void)
{
    static u8_t hrm[2];

    /* Heartrate measurements simulation */
    if (!simulate_hrm) {
        return;
    }

    heartrate++;
    if (heartrate == 160U) {
        heartrate = 90U;
    }

    hrm[0] = 0x06; /* uint8, sensor contact */
    hrm[1] = heartrate;

    bt_gatt_notify(NULL, &attrs[1], &hrm, sizeof(hrm));
}





//----------------------------------------------





#define LEN_BUF 256
static u8_t buf_tx_ping[LEN_BUF] = {0}; // Double buffers. One sends while other bufs upcoming sends
static u8_t buf_tx_pong[LEN_BUF] = {0};

/*
static void hrmc_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                 u16_t value)
{
    simulate_hrm = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}



static ssize_t read_blsc(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, u16_t len, u16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &hrs_blsc,
                             sizeof(hrs_blsc));
}
*/



#define BB_BT_UART_SERVICE 0xdc00
#define BB_BT_TX_
#define BB_BT_UART_CHAR
/* Heart Rate Service Declaration */
static struct bt_gatt_attr bt_uart_attrs[] = {
        BT_GATT_PRIMARY_SERVICE(BB_BT_UART_SERVICE),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
                               BT_GATT_PERM_NONE, NULL, NULL, NULL),
        BT_GATT_CCC(hrmc_ccc_cfg, hrmc_ccc_cfg_changed),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_BODY_SENSOR, BT_GATT_CHRC_READ,
                               BT_GATT_PERM_READ, read_blsc, NULL, NULL),
        BT_GATT_CHARACTERISTIC(BT_UUID_HRS_CONTROL_POINT, BT_GATT_CHRC_WRITE,
                               BT_GATT_PERM_NONE, NULL, NULL, NULL),};

static struct bt_gatt_service bt_uart_service = BT_GATT_SERVICE(bt_uart_attrs);

void tx_init(void)
{
    memset(&buf_tx_ping, 0, LEN_BUF);
    memset(&buf_tx_ping, 0, LEN_BUF);
}

// Notifies the client that new data is ready
void tx_notify(void)
{
    static char * hello = "Hi there David\n";

}

void hrs_notify(void)
{
    static u8_t hrm[2];

    /* Heartrate measurements simulation */
    if (!simulate_hrm) {
        return;
    }

    heartrate++;
    if (heartrate == 160U) {
        heartrate = 90U;
    }

    hrm[0] = 0x06; /* uint8, sensor contact */
    hrm[1] = heartrate;

    bt_gatt_notify(NULL, &attrs[1], &hrm, sizeof(hrm));
}

