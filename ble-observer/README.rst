.. _bluetooth-observer-sample:

Bluetooth: Observer
###################

Overview
********

A simple application demonstrating Bluetooth Low Energy Observer role
functionality. The application will periodically scan for devices nearby.
If any found, prints the address of the device, the RSSI value, the Advertising
type, and the Advertising data length to the console.

If the used Bluetooth Low Energy Controller supports Extended Scanning, you may
enable `CONFIG_BT_EXT_ADV` in the project configuration file. Refer to the
project configuration file for further details.

Requirements
************

* A board with Bluetooth Low Energy support

Building and Running
********************

This sample can be found under :zephyr_file:`samples/bluetooth/observer` in the
Zephyr tree.

See :ref:`Bluetooth samples section <bluetooth-samples>` for details.



UUID (Universally Unique Identifier): This is a 128-bit identifier that distinguishes one beacon from another. 
Typically, a company or organization will use the same UUID for all its beacons.
estimate its distance from the beacon based on the RSSI value.

structure of an iBeacon advertisement:

4 bytes: Apple's Bluetooth SIG Company Identifier (0x004C).
2 bytes: Beacon type, typically 0x0215 for iBeacons.
16 bytes: UUID.
2 bytes: Major.
2 bytes: Minor.
1 byte: Signal power calibration value.