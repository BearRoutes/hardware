#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "HTTP_CLIENT";

// Function to send HTTP POST request
void http_post_function() {
    char *post_data = "{\"message\": \"Hello from ESP32\"}";
    esp_http_client_config_t config = {
        .url = "http://172.31.63.135:5000/post-endpoint", // Update with your server's URL
        .method = HTTP_METHOD_POST,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Set header for indicating content type as JSON
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Set the POST field data
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Perform the HTTP POST request
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %" PRId64,
         esp_http_client_get_status_code(client),
         esp_http_client_get_content_length(client));

    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // Cleanup after the request
    esp_http_client_cleanup(client);
}
