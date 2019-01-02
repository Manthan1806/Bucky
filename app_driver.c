/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <iot_button.h>

#include "app_priv.h"

/* This is the button that is used for toggling the output */
#define BUTTON_GPIO          0
#define BUTTON_ACTIVE_LEVEL  0
/* This is the GPIO on which the output will be set */
#define OUTPUT_GPIO    27
#define OUTPUT_GPIO2    14
#define ENABLE_GPIO    12


//button can be pressed after five seconds have passed from the previous button press
#define DEBOUNCE_TIME  500

static bool g_output_state;
static void push_btn_cb(void* arg)
{
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        printf("BHAI BHAI");
        previous = current;
        app_driver_toggle_state();
    }
}

static void configure_push_button(int gpio_num, void (*btn_cb)(void *))
{
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        printf("CONFIGURING?");
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, btn_cb, "RELEASE");
    }
}

static void set_output_state(bool target)
{
    gpio_set_level(OUTPUT_GPIO, target);
    gpio_set_level(OUTPUT_GPIO2, !target);
}

void app_driver_init()
{
    configure_push_button(BUTTON_GPIO, push_btn_cb);

    printf("APP DRIVER INIT");

    /* Configure output */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf.pin_bit_mask = ((uint64_t)1 << OUTPUT_GPIO);

    gpio_config_t io_conf2 = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf2.pin_bit_mask = ((uint64_t)1 << OUTPUT_GPIO2);


    gpio_config_t io_conf3 = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf3.pin_bit_mask = ((uint64_t)1 << ENABLE_GPIO); 


    /* Configure the GPIO */
    gpio_config(&io_conf);
    gpio_config(&io_conf2);
    gpio_config(&io_conf3);

    gpio_set_level(ENABLE_GPIO, 1);
    printf("ENABLE_GPIO set");
}

int IRAM_ATTR app_driver_toggle_state(void)
{
    printf("\nTOGGLE KIYA");
    g_output_state = ! g_output_state;
    set_output_state(g_output_state);
    return ESP_OK;
}

bool app_driver_get_state(void)
{

    return g_output_state;
}
