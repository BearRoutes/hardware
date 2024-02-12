#include "tcp_client.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"

#define SERVER_PORT 3000

static const char *TAG = "TCP_CLIENT";

void tcp_client(void *args) {
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return; // No value is returned here because the function is void.
    }
    ESP_LOGI(TAG, "Socket created");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); // Corrected variable name here.

    // Convert IPv4 addresses from text to binary form
    if (inet_pton(AF_INET, "172.31.63.135", &server_addr.sin_addr) <= 0) {
        ESP_LOGE(TAG, "Invalid address/ Address not supported"); // Use logging instead of printf.
        // No return -1; here, just return because the function is void.
        close(sock); // Ensure resources are cleaned up.
        vTaskDelete(NULL);
        return;
    }

    int err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Successfully connected");

    char rx_buffer[128];
    char tx_buffer[20] = "Hello, server";

    send(sock, tx_buffer, strlen(tx_buffer), 0);
    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

    // Check for errors or no data read.
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
    } else {
        rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
        ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
    }

    close(sock);
}
