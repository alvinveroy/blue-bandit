#!/usr/bin/env python3

import dbus
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject
import sys

from dbus.mainloop.glib import DBusGMainLoop

bus = None
mainloop = None

BLUEZ_SERVICE_NAME = 'org.bluez'
DBUS_OM_IFACE =      'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE =    'org.freedesktop.DBus.Properties'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE =    'org.bluez.GattCharacteristic1'


HR_SVC_UUID =        '0000180d-0000-1000-8000-00805f9b34fb'
HR_MSRMT_UUID =      '00002a37-0000-1000-8000-00805f9b34fb'
BODY_SNSR_LOC_UUID = '00002a38-0000-1000-8000-00805f9b34fb'
HR_CTRL_PT_UUID =    '00002a39-0000-1000-8000-00805f9b34fb'

# The objects that we interact with.
hr_service = None
hr_msrmt_chrc = None
body_snsr_loc_chrc = None
hr_ctrl_pt_chrc = None



SERVICE_UART_UUID           = '0000bb00-0000-1000-8000-00805f9b34fb'
CHARCTR_UART_SERVER_RX_UUID = '0000dc01-0000-1000-8000-00805f9b34fb'
CHARCTR_UART_SERVER_TX_UUID = '0000dc02-0000-1000-8000-00805f9b34fb'

uart_service = None
chrc_client_rx = None
chrc_client_tx = None


SERVICE_BATTERY_UUID     = '0000180f-0000-1000-8000-00805f9b34fb'
CHARCTR_BATTERY_LVL_UUID = '00002a19-0000-1000-8000-00805f9b34fb'
chrc_battery_lvl = None
battery_service  = None



def generic_error_cb(error):
    print('D-Bus call failed: ' + str(error))
    mainloop.quit()


def body_sensor_val_to_str(val):
    if val == 0:
        return 'Other'
    if val == 1:
        return 'Chest'
    if val == 2:
        return 'Wrist'
    if val == 3:
        return 'Finger'
    if val == 4:
        return 'Hand'
    if val == 5:
        return 'Ear Lobe'
    if val == 6:
        return 'Foot'

    return 'Reserved value'


def sensor_contact_val_to_str(val):
    if val == 0 or val == 1:
        return 'not supported'
    if val == 2:
        return 'no contact detected'
    if val == 3:
        return 'contact detected'

    return 'invalid value'


def body_sensor_val_cb(value):
    if len(value) != 1:
        print('Invalid body sensor location value: ' + repr(value))
        return

    print('Body sensor location value: ' + body_sensor_val_to_str(value[0]))


def hr_msrmt_start_notify_cb():
    print('HR Measurement notifications enabled')


def hr_msrmt_changed_cb(iface, changed_props, invalidated_props):
    if iface != GATT_CHRC_IFACE:
        return

    if not len(changed_props):
        return

    value = changed_props.get('Value', None)
    if not value:
        return

    print('New HR Measurement')

    flags = value[0]
    value_format = flags & 0x01
    sc_status = (flags >> 1) & 0x03
    ee_status = flags & 0x08

    if value_format == 0x00:
        hr_msrmt = value[1]
        next_ind = 2
    else:
        hr_msrmt = value[1] | (value[2] << 8)
        next_ind = 3

    print('\tHR: ' + str(int(hr_msrmt)))
    print('\tSensor Contact status: ' +
          sensor_contact_val_to_str(sc_status))

    if ee_status:
        print('\tEnergy Expended: ' + str(int(value[next_ind])))



def response_battery_level():
    print("Battery level notifications enabled")


def propertyChange_battery_level(iface, changed_props, invalidated_props):
    if iface != GATT_CHRC_IFACE:
        return

    if not len(changed_props):
        return

    value = changed_props.get('Value', None)
    if not value:
        return

    print('Battery @ %d%%' % value[0])


def response_client_rx():
    print("Client RX updated")

def response_client_tx():
    print("Client TX updated")

def propertyChange_client_rx(iface, changed_props, invalidated_props):
    if iface != GATT_CHRC_IFACE:
        return

    if not len(changed_props):
        return

    value = changed_props.get('Value', None)
    if not value:
        return

    print('Client RX: %s' % ''.join([str(c) for c in value]))



def sendMessage(msg):
    data = dbus.ByteArray(msg.encode('utf8'))
    chrc_client_tx[0].WriteValue(data, {}, dbus_interface=GATT_CHRC_IFACE)

count = 0
def propertyChange_client_tx(iface, changed_props, invalidated_props):
    if iface != GATT_CHRC_IFACE:
        return


    if not len(changed_props):
        print('Client TX[0]')
        return


    value = changed_props.get('Value', None)
    if not value:
        print('Client TX: n/a')

        # Initialize the client TX data on the server
        data = dbus.ByteArray('Client TX Initialized'.encode('utf8'))
        chrc_client_tx[0].WriteValue(data, {},
                                     dbus_interface=GATT_CHRC_IFACE)

        return


    # Print the current Client TX as noted on the server
    print('Client TX: %s' % ''.join([str(c) for c in value]))

    # Update what the client TX on the server
    global count
    data = dbus.ByteArray('Count = %d'.encode('utf8') % count)
    chrc_client_tx[0].WriteValue(data, {}, dbus_interface=GATT_CHRC_IFACE)
    count += 1



def start_client():
    '''
    # Read the Body Sensor Location value and print it asynchronously.
    body_snsr_loc_chrc[0].ReadValue({}, reply_handler=body_sensor_val_cb,
                                    error_handler=generic_error_cb,
                                    dbus_interface=GATT_CHRC_IFACE)

    # Listen to PropertiesChanged signals from the Heart Measurement
    # Characteristic.
    hr_msrmt_prop_iface = dbus.Interface(hr_msrmt_chrc[0], DBUS_PROP_IFACE)
    hr_msrmt_prop_iface.connect_to_signal("PropertiesChanged",
                                          hr_msrmt_changed_cb)

    # Subscribe to Heart Rate Measurement notifications.
    hr_msrmt_chrc[0].StartNotify(reply_handler=hr_msrmt_start_notify_cb,
                                 error_handler=generic_error_cb,
                                 dbus_interface=GATT_CHRC_IFACE)
    '''
    # Subscribe to battery notificatios
    if False and battery_service:
        propertyIF_battery_level = dbus.Interface(chrc_battery_lvl[0], DBUS_PROP_IFACE)
        propertyIF_battery_level.connect_to_signal("PropertiesChanged", propertyChange_battery_level)
        chrc_battery_lvl[0].StartNotify(reply_handler=response_battery_level,
                                        error_handler=generic_error_cb,
                                        dbus_interface=GATT_CHRC_IFACE)
    if uart_service:
        propertyIF_client_rx = dbus.Interface(chrc_client_rx[0], DBUS_PROP_IFACE)
        propertyIF_client_rx.connect_to_signal("PropertiesChanged", propertyChange_client_rx)
        chrc_client_rx[0].StartNotify(reply_handler=response_client_rx,
                                      error_handler=generic_error_cb,
                                      dbus_interface=GATT_CHRC_IFACE)

        propertyIF_client_tx = dbus.Interface(chrc_client_tx[0], DBUS_PROP_IFACE)
        propertyIF_client_tx.connect_to_signal("PropertiesChanged", propertyChange_client_tx)

        chrc_client_tx[0].StartNotify(reply_handler=response_client_tx,
                                      error_handler=generic_error_cb,
                                      dbus_interface=GATT_CHRC_IFACE)

def process_heartrate_chrc(chrc_path):
    chrc = bus.get_object(BLUEZ_SERVICE_NAME, chrc_path)
    chrc_props = chrc.GetAll(GATT_CHRC_IFACE,
                             dbus_interface=DBUS_PROP_IFACE)

    uuid = chrc_props['UUID']

    if uuid == HR_MSRMT_UUID:
        global hr_msrmt_chrc
        hr_msrmt_chrc = (chrc, chrc_props)
    elif uuid == BODY_SNSR_LOC_UUID:
        global body_snsr_loc_chrc
        body_snsr_loc_chrc = (chrc, chrc_props)
    elif uuid == HR_CTRL_PT_UUID:
        global hr_ctrl_pt_chrc
        hr_ctrl_pt_chrc = (chrc, chrc_props)
    else:
        print('Unrecognized characteristic: ' + uuid)

    return True

def process_uart_chrc(chrc_path):
    chrc = bus.get_object(BLUEZ_SERVICE_NAME, chrc_path)
    chrc_props = chrc.GetAll(GATT_CHRC_IFACE, dbus_interface=DBUS_PROP_IFACE)

    uuid = chrc_props['UUID']

    if uuid == CHARCTR_UART_SERVER_RX_UUID:
        global chrc_client_tx
        chrc_client_tx = (chrc, chrc_props)
        print('processed server rx characteristics')

    if uuid == CHARCTR_UART_SERVER_TX_UUID:
        global chrc_client_rx
        chrc_client_rx = (chrc, chrc_props)
        print('processed server tx characteristics')

    return True

def process_battery_chrc(chrc_path):
    chrc = bus.get_object(BLUEZ_SERVICE_NAME, chrc_path)
    chrc_props = chrc.GetAll(GATT_CHRC_IFACE, dbus_interface=DBUS_PROP_IFACE)

    uuid = chrc_props['UUID']

    if uuid == CHARCTR_BATTERY_LVL_UUID:
        global chrc_battery_lvl
        chrc_battery_lvl = (chrc, chrc_props)
    else:
        print('Unrecognized battery characteristic: ' + uuid)

    return True

def process_hr_service(service_path, chrc_paths):
    service = bus.get_object(BLUEZ_SERVICE_NAME, service_path)
    service_props = service.GetAll(GATT_SERVICE_IFACE,
                                   dbus_interface=DBUS_PROP_IFACE)

    uuid = service_props['UUID']

    if uuid != HR_SVC_UUID:
        return False

    print('Heart Rate Service found: ' + service_path)

    # Process the characteristics.
    for chrc_path in chrc_paths:
        process_heartrate_chrc(chrc_path)

    global hr_service
    hr_service = (service, service_props, service_path)

    return True

def process_uart_service(service_path, chrc_paths):
    service = bus.get_object(BLUEZ_SERVICE_NAME, service_path)
    service_props = service.GetAll(GATT_SERVICE_IFACE, dbus_interface=DBUS_PROP_IFACE)

    uuid = service_props['UUID']

    if uuid != SERVICE_UART_UUID:
        return False

    print('Blue-Bandit Service found: ' + service_path)

    # Process the characteristics.
    for chrc_path in chrc_paths:
        process_uart_chrc(chrc_path)

    global uart_service
    uart_service = (service, service_props, service_path)

    return True


def process_battery_service(service_path, chrc_paths):
    service = bus.get_object(BLUEZ_SERVICE_NAME, service_path)
    print(service)
    service_props = service.GetAll(GATT_SERVICE_IFACE, dbus_interface=DBUS_PROP_IFACE)

    uuid = service_props['UUID']

    if uuid != SERVICE_BATTERY_UUID:
        return False

    print('Blue-Bandit Service found: ' + service_path)

    # Process the characteristics.
    for chrc_path in chrc_paths:
        process_battery_chrc(chrc_path)

    global battery_service
    battery_service = (service, service_props, service_path)

    return True

def interfaces_removed_cb(object_path, interfaces):
    if not hr_service:
        return

    if object_path == hr_service[2]:
        print('Service was removed')
        mainloop.quit()




def on_input(sender, event):
    # Send the message over via bluetooth server
    msg = chr(event.keyval)
    sendMessage(msg)


def main():
    # Set up the main loop.
    DBusGMainLoop(set_as_default=True)
    global bus
    bus = dbus.SystemBus()
    global mainloop
    mainloop = GObject.MainLoop()

    om = dbus.Interface(bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
    om.connect_to_signal('InterfacesRemoved', interfaces_removed_cb)

    print('Getting objects...')
    objects = om.GetManagedObjects()
    chrcs = []

    # List characteristics found
    for path, interfaces in objects.items():
        if GATT_CHRC_IFACE not in interfaces.keys():
            continue
        chrcs.append(path)

    # List sevices found
    for path, interfaces in objects.items():
        # Skip anything that doesn't offer GATT services
        if GATT_SERVICE_IFACE not in interfaces.keys():
            continue
        

        print('service path: %s' % str(path))
        #print(interfaces)
        chrc_paths = [d for d in chrcs if d.startswith(path + "/")]

        if process_battery_service(path, chrc_paths):
            print('battery service processed')

        if process_hr_service(path, chrc_paths):
            print('Heartrate processed')

        if process_uart_service(path, chrc_paths):
            print('uart service processed')
            break


    if not battery_service:
        print('No battery service found')


    if not uart_service:
        print('No UART service found')
        sys.exit(1)

    start_client()


    # Connect a keyboard listener
    from gi.repository import Gtk
    window = Gtk.Window()
    window.connect('key-release-event', on_input)
    window.show_all()

    mainloop.run()

if __name__ == '__main__':
    main()
