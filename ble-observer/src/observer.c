#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/byteorder.h> // Include this if sys_get_le16 is declared here
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include <string.h>

static const struct device *const uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

char tx_buffer[128];



#define NAME_LEN 30
static uint16_t manufacturer_id = 0;
static uint8_t manufacturer_data_len = 0;
static const uint8_t *manufacturer_data = NULL;
static char name[NAME_LEN] = {0}; // Ensure the name is initialized to prevent undefined behavior.

/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

const char* get_manufacturer_name(uint16_t manufacturer_id) {
    switch (manufacturer_id) {
    case 76:
        return "Apple";
    case 734:
    case 117:
        return "Samsung";
    case 224:
        return "Google";
    default:
        return "Unknown";
    }
}

static bool parse_advertising_data(struct bt_data *data, void *user_data)
{
    switch (data->type) {
    case BT_DATA_NAME_COMPLETE:
    case BT_DATA_NAME_SHORTENED:
        // Ensure not to overflow the name buffer
        size_t copy_len = MIN(data->data_len, NAME_LEN - 1);
        memcpy(name, data->data, copy_len);
        name[copy_len] = '\0';
        return true;
    case BT_DATA_MANUFACTURER_DATA:
        if (data->data_len >= 2) {
            manufacturer_id = sys_get_le16(data->data);
            manufacturer_data = &data->data[2];
            manufacturer_data_len = data->data_len - 2;
        }
        return true;
    default:
        return false;
    }
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type, struct net_buf_simple *ad)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));

    // Reset name and manufacturer data before parsing to avoid displaying stale information.
    memset(name, 0, sizeof(name));
    manufacturer_id = 0;
    manufacturer_data_len = 0;
    manufacturer_data = NULL;

    bt_data_parse(ad, parse_advertising_data, NULL);

    // printk("Device found: %s (RSSI %d)\n", addr_str, rssi);
    if (manufacturer_id != 0 && manufacturer_data_len > 0) {
        const char* manufacturer_name = get_manufacturer_name(manufacturer_id);

		memset(tx_buffer, 0, sizeof(tx_buffer));
		snprintf(tx_buffer, sizeof(tx_buffer), "Manufacturer ID: %u device name (%s)  address: %s (RSSI %d)\n", manufacturer_id, manufacturer_name, addr_str, rssi);
		print_uart(tx_buffer);
    }
}


#if defined(CONFIG_BT_EXT_ADV)
static bool data_cb(struct bt_data *data, void *user_data)
{
	char *name = user_data;
	uint8_t len;

	switch (data->type) {
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
		len = MIN(data->data_len, NAME_LEN - 1);
		(void)memcpy(name, data->data, len);
		name[len] = '\0';
		return false;
	default:
		return true;
	}
}

static const char *phy2str(uint8_t phy)
{
	switch (phy) {
	case BT_GAP_LE_PHY_NONE: return "No packets";
	case BT_GAP_LE_PHY_1M: return "LE 1M";
	case BT_GAP_LE_PHY_2M: return "LE 2M";
	case BT_GAP_LE_PHY_CODED: return "LE Coded";
	default: return "Unknown";
	}
}

static void scan_recv(const struct bt_le_scan_recv_info *info,
		      struct net_buf_simple *buf)
{
	char le_addr[BT_ADDR_LE_STR_LEN];
	char name[NAME_LEN];
	uint8_t data_status;
	uint16_t data_len;

	(void)memset(name, 0, sizeof(name));

	data_len = buf->len;
	bt_data_parse(buf, data_cb, name);

	data_status = BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS(info->adv_props);

	bt_addr_le_to_str(info->addr, le_addr, sizeof(le_addr));
	printk("[DEVICE]: %s, AD evt type %u, Tx Pwr: %i, RSSI %i "
	       "Data status: %u, AD data len: %u Name: %s "
	       "C:%u S:%u D:%u SR:%u E:%u Pri PHY: %s, Sec PHY: %s, "
	       "Interval: 0x%04x (%u ms), SID: %u\n",
	       le_addr, info->adv_type, info->tx_power, info->rssi,
	       data_status, data_len, name,
	       (info->adv_props & BT_GAP_ADV_PROP_CONNECTABLE) != 0,
	       (info->adv_props & BT_GAP_ADV_PROP_SCANNABLE) != 0,
	       (info->adv_props & BT_GAP_ADV_PROP_DIRECTED) != 0,
	       (info->adv_props & BT_GAP_ADV_PROP_SCAN_RESPONSE) != 0,
	       (info->adv_props & BT_GAP_ADV_PROP_EXT_ADV) != 0,
	       phy2str(info->primary_phy), phy2str(info->secondary_phy),
	       info->interval, info->interval * 5 / 4, info->sid);
}

static struct bt_le_scan_cb scan_callbacks = {
	.recv = scan_recv,
};
#endif /* CONFIG_BT_EXT_ADV */

int observer_start(void)
{


	if (!device_is_ready(uart_dev)) {
		return 0;
	}


	struct bt_le_scan_param scan_param = {
		.type       = BT_LE_SCAN_TYPE_PASSIVE,
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
		.interval   = BT_GAP_SCAN_FAST_INTERVAL,
		.window     = BT_GAP_SCAN_FAST_WINDOW,
	};
	int err;

#if defined(CONFIG_BT_EXT_ADV)
	bt_le_scan_cb_register(&scan_callbacks);
	printk("Registered scan callbacks\n");
#endif /* CONFIG_BT_EXT_ADV */

	err = bt_le_scan_start(&scan_param, device_found);
	if (err) {
		//print_uart("Start scanning failed (err %d)\n", err);
		return err;
	}

	return 0;
}
