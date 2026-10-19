#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <stdio.h>

#define BLINK_GPIO 15

// added for button example
volatile static bool led = 0;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

 
void hello_task(void *pvParameter)
{
 
	while(1)
	{
	    printf("Hello world!\n");
	    vTaskDelay(100 / portTICK_RATE_MS);
	}
}
 
void blinky(void *pvParameter)
{
 
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_RATE_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

// added for push button switch testing
void IRAM_ATTR button_intr(void* arg) {
    if (!gpio_get_level(GPIO_NUM_0)) {

        for (volatile int i = 0; i < 10000; ++i) {}  // Cheesy debounce.

        if (!gpio_get_level(GPIO_NUM_0)) {
            led = !led;
            gpio_set_level(GPIO_NUM_5, led);
        }
    }
}

void app_main(void)
{
/*
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "access_point_name",
            .password = "password",
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
*/
    //nvs_flash_init();
    //xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
    //xTaskCreate(&blinky, "blinky", 512,NULL,5,NULL );

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);

    gpio_set_intr_type(GPIO_NUM_0, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(GPIO_NUM_0);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL2);

    gpio_isr_handler_add(
        GPIO_NUM_0,
        button_intr,
        NULL
    );

}

