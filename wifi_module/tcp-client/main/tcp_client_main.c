/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * 
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE (1024)
static QueueHandle_t uartQueue;


extern void tcp_client(void);


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
    uartQueue = xQueueCreate(10, sizeof(uint8_t *));

    // Start UART task
    xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
    // Start TCP task
    xTaskCreate(tcp_client, "tcp_task", 4096, NULL, 5, NULL);
}
