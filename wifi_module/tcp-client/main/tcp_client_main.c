/*
 * TCP Client for University Routing Project
 *  Authour: Abdulaziz Issa , aissa1@ualberta.ca
 * 
 * 
 */
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT
#define UART_NUM UART_NUM_1
#define BUF_SIZE (1024)
static const char *TAG = "Dev";
static QueueHandle_t uartQueue;


void tcp_client(void *pvParameters);

void uart_task(void *pvParameters);


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

     // Create a queue capable of containing pointers to uint8_t arrays (messages)
    uartQueue = xQueueCreate(20, sizeof(uint8_t *));

    // Start UART task
    xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
    // Start TCP task
    xTaskCreate(tcp_client, "tcp_task", 4096, NULL, 5, NULL);
}



// UART reading task
void uart_task(void *pvParameters) {
    const int uart_num = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(uart_num, &uart_config);
    uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    while (true) {
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, pdMS_TO_TICKS(20));
        if (len > 0) {
            // Ideally, create a copy or use a ring buffer for the data to be sent to the queue
            // For simplicity, we're directly sending the pointer to the data buffer
            xQueueSend(uartQueue, (void *)&data, (TickType_t)0);
        }
    }
}

void tcp_client(void *pvParameters)
{
    
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {

        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        // Create socket
        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);
        // Connect to server
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        // Send data to the server
        uint8_t *data_to_send = NULL;
        while (1) {

            if (xQueueReceive(uartQueue, &(data_to_send), portMAX_DELAY)) {
                // Wait for data from UART queue
                int err = send(sock, data_to_send, strlen((char *)data_to_send), 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
                
                 // Free the dynamically allocated memory after sending
                free(data_to_send);
              
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
