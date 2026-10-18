#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// I2C Configuration for OLED 
#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          400000

// Motor GPIO Mapping (L293D Inputs) [cite: 27]
#define M1_PWM 13  // Motor 1 Speed
#define M1_IN1 12  // Motor 1 Dir A
#define M1_IN2 14  // Motor 1 Dir B

// PWM Settings for DC Motors
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0

static const char *TAG = "ROVER_MAIN";

void init_motors() {
    // Configure Direction Pins as Output
    gpio_set_direction((gpio_num_t)M1_IN1, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)M1_IN2, GPIO_MODE_OUTPUT);

    // Configure PWM Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HS_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT, // 0-255 speed range
        .timer_num        = LEDC_HS_TIMER,
        .freq_hz          = 5000, // 5kHz frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configure PWM Channel for Motor 1
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = M1_PWM,
        .speed_mode     = LEDC_HS_MODE,
        .channel        = LEDC_HS_CH0_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_HS_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
    
    ESP_LOGI(TAG, "Motor GPIOs initialized.");
}

void init_i2c_oled() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = { .clk_speed = I2C_MASTER_FREQ_HZ },
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    ESP_LOGI(TAG, "I2C initialized for OLED.");
}

extern "C" void app_main() {
    init_motors();
    init_i2c_oled();

    while (1) {
        ESP_LOGI(TAG, "Moving Motor 1 Forward...");
        gpio_set_level((gpio_num_t)M1_IN1, 1);
        gpio_set_level((gpio_num_t)M1_IN2, 0);
        ledc_set_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL, 200); // Set speed
        ledc_update_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Stopping Motor 1...");
        ledc_set_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL, 0);
        ledc_update_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}