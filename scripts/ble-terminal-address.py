#!/usr/bin/env python3
from bluetooth.ble import DiscoveryService

try:
    service = DiscoveryService()
    devices  = service.discover(3)

    for addr, name in devices.items():
        if 'blue-bandit' == name:
            print("blue-bandit@%s" % addr)
except RuntimeError:
    print('[FAILED]: Make sure you are running with as super user. \n'
          '          Additionally, you may need to call the script multiple times for the correct\n'
          '          user rights to go through for BLE discovery. I had this issue sometimes')

