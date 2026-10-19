/* I2C Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_wifi.h"
#include "xi2c.h"
#include "fonts.h"
#include "ssd1306.h"
#include "nvs_flash.h"


#include <string.h>
#include <stdlib.h>
#include "freertos/queue.h"

// #define BLINK_GPIO 2
#define GPIO_INPUT_IO_0    16 
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_IO_2    16
#define GPIO_INPUT_IO_3    17
#define GPIO_INPUT_IO_4    18

static xQueueHandle gpio_evt_queue = NULL;

/**
 * TEST CODE BRIEF
 *
 * This example will show you how to use I2C SSD1306 display module using the i2c bus:
 *
 * - Use one I2C port(master mode) to write to I2C port on one SSD1306 OLED display.
 *
 * Pin assignment:
 *
 * - master:
 *    GPIO21 is assigned as the data signal of i2c master port
 *    GPIO22 is assigned as the clock signal of i2c master port
 *
 * Connection:
 *
 * - connect GPIO21 with SDA
 * - connect GPIO22 with SCL
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 *
 * Test items:
 *
 * - write to the display, if connected.
 * 
 */

#define I2C_EXAMPLE_MASTER_SCL_IO    22    /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO    21    /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

xSemaphoreHandle print_mux;


#define ESP_INTR_FLAG_DEFAULT 0

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

static void gpio_task(void* arg)
{
  uint32_t io_num;
  printf("GPIO task created\n");
  for(;;) {
    if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
      printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
      SSD1306_GotoXY(5, 4);
      SSD1306_Puts("Button pressed!", &Font_7x10, SSD1306_COLOR_WHITE);
      printf("Button pressed...\n");
         /* Update screen, send changes to LCD */
    SSD1306_UpdateScreen();

    while (1) {
        /* Invert pixels */
        SSD1306_ToggleInvert();
        
        /* Update screen */
        SSD1306_UpdateScreen();
        
        /* Make a little delay */
        vTaskDelay(50);
    }
    }
  }
}

static void i2c_test(void)
{
    SSD1306_GotoXY(5, 4);
    SSD1306_Puts("ESP32 Goth pARty!", &Font_7x10, SSD1306_COLOR_WHITE);
    
    /* Go to location X = 20, Y = 25 */
    SSD1306_GotoXY(8, 25);
    SSD1306_Puts("I2C SSD1306 OLED", &Font_7x10, SSD1306_COLOR_WHITE);
    
    /* Go to location X = 15, Y = 45 */
    SSD1306_GotoXY(15, 45);
    SSD1306_Puts("DEFCON XXVI", &Font_7x10, SSD1306_COLOR_WHITE);
    
    /* Update screen, send changes to LCD */
    SSD1306_UpdateScreen();

    while (1) {
        /* Invert pixels */
        SSD1306_ToggleInvert();
        
        /* Update screen */
        SSD1306_UpdateScreen();
        
        /* Make a little delay */
        vTaskDelay(50);
    }

}

/**
 * @brief i2c master initialization
 */
static void i2c_example_master_init()
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}

void app_main()
{
  print_mux = xSemaphoreCreateMutex();
  i2c_example_master_init();
  SSD1306_Init();

  //xTaskCreate(i2c_test, "i2c_test", 1024, NULL, 10, NULL);


  //change gpio intrrupt type for one pin
  gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);
  gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_ANYEDGE);    
  gpio_set_intr_type(GPIO_INPUT_IO_2, GPIO_INTR_ANYEDGE);
  gpio_set_intr_type(GPIO_INPUT_IO_3, GPIO_INTR_ANYEDGE);
  gpio_set_intr_type(GPIO_INPUT_IO_4, GPIO_INTR_ANYEDGE);
  //create a queue to handle gpio event from isr
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  //start gpio task
  xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
  //hook isr handler for specific gpio pin
  //gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
}
