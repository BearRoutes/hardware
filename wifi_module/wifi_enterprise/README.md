# Understanding different WiFi enterprise modes:

**WPA2 Enterprise**: WPA2-Enterprise is an advanced Wi-Fi security mode primarily used in business environments. It employs a RADIUS server for user-based authentication, supporting various EAP methods like EAP-TLS and EAP-PEAP. This mode enhances security by requiring individual user credentials, establishes secure encryption keys, and allows for efficient user management. It's a scalable and robust solution ideal for large-scale networks seeking strong protection against unauthorized access.

**WPA3 Enterprise**: WPA2-Enterprise + PMF mandatory + CA certificate validaion(required)

**WPA3 Enterprise(192 bit)**: WPA3 Enterprise + AES256 Keys(GCMP256/CCMP256) + BIP256 + RSA3096/EC certs + NSA SuiteB ciphers in EAP authentication.

# This application is built on top of the WiFi Enterprise Example given in ESP sdk 

This example shows how ESP32 connects to AP with Wi-Fi enterprise encryption. The example does the following steps:

1. Install CA certificate which is optional.
2. Install client certificate and client key which is required in TLS method and optional in PEAP and TTLS methods.
3. Set identity of phase 1 which is optional.
4. Set user name and password of phase 2 which is required in PEAP and TTLS methods.
5. Enable WiFi enterprise mode.
6. Connect to AP using esp_wifi_connect().

*Note:* 1. The certificates currently are generated and are present in examples/wifi/wifi_enterprise/main folder.
        2. The expiration date of the certificates is 2027/06/05.
        3. In case using suite-b, please use appropriate certificates such as RSA-3072 or p384 EC certificates.

The steps to create new certificates are given below.

## The file ca.pem, ca.key, server.pem, server.crt and server.key can be used to configure AP with enterprise encryption.

## How to use Example

### Configuration

```
idf.py menuconfig
```
* Set SSID of Access Point to connect in Example Configuration.
* Select EAP method (TLS, TTLS or PEAP).
* Select Phase2 method (only for TTLS).
* Enter EAP-ID.
* Enter Username and Password (only for TTLS and PEAP).
* Enable or disable Validate Server option.

### Build and Flash the project.

```
idf.py -p PORT flash monitor
```

## how it works


### Procedure Explanation:
- **Wi-Fi Configuration and Connection**:
   - Initializes the ESP32 Wi-Fi system to connect to a specified Wi-Fi network with support for advanced security protocols, such as WPA2/WPA3 Enterprise, through configurable settings like SSID, EAP method, and credentials.

- **Wi-Fi Setup**:
  - The `initialise_wifi` function configures the Wi-Fi connection details, including security settings and credentials, based on preprocessor directives that allow for compile-time configuration.
- **Secure Connection**:
  - Depending on the defined EAP method, it sets up the necessary authentication details, such as CA certificates for server validation, client certificates and keys for EAP-TLS, or usernames and passwords for PEAP and TTLS.
- **Event Handling and Connection**:
  - Registers event handlers to manage the Wi-Fi connection lifecycle, automatically reconnecting on disconnection and signaling upon successful connection and IP address assignment.
- **Monitoring and Logging**:
  - After initializing the Wi-Fi system, it creates a task that periodically checks and logs the device’s IP configuration, ensuring the connection is maintained and providing visibility into the network status.

This setup showcases the ESP32's capabilities in securely connecting to enterprise Wi-Fi networks, suitable for environments requiring advanced authentication methods and encryption standards.