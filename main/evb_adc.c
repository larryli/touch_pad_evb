/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <stdio.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "evb.h"
static const char* TAG = "evb_adc";

#if CONFIG_TOUCH_EB_V1
#define TOUCH_SLIDE_THRESH_UPPER     600
#define TOUCH_SLIDE_THRESH_LOWER     300
#define TOUCH_MATRIX_THRESH_UPPER    200
#define TOUCH_MATRIX_THRESH_LOWER    0
#elif CONFIG_TOUCH_EB_V2
#define TOUCH_SLIDE_THRESH_UPPER     350
#define TOUCH_SLIDE_THRESH_LOWER     300
#define TOUCH_MATRIX_THRESH_UPPER    500
#define TOUCH_MATRIX_THRESH_LOWER    450
#define TOUCH_SPRING_THRESH_UPPER    80
#define TOUCH_SPRING_THRESH_LOWER    30
#define TOUCH_CIRCLE_THRESH_UPPER    820
#define TOUCH_CIRCLE_THRESH_LOWER    780
#endif

void evb_adc_init()
{
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_11db);
}

int evb_adc_get_mode()
{
    esp_adc_cal_characteristics_t characteristics;
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_0db, ADC_WIDTH_12Bit, &characteristics);

    while (1) {
        uint32_t voltage = adc1_to_voltage(ADC1_TEST_CHANNEL, &characteristics);
        ESP_LOGI(TAG, "%d mV", voltage);
        if (voltage < TOUCH_SLIDE_THRESH_UPPER && voltage > TOUCH_SLIDE_THRESH_LOWER) {
            ESP_LOGI(TAG, "SLIDE MODE...");
            return TOUCH_EVB_MODE_SLIDE;
        } else if (voltage < TOUCH_MATRIX_THRESH_UPPER && voltage > TOUCH_MATRIX_THRESH_LOWER) {
            ESP_LOGI(TAG, "MATRIX MODE...");
            return TOUCH_EVB_MODE_MATRIX;
        }
#if CONFIG_TOUCH_EB_V2
        else if (voltage < TOUCH_SPRING_THRESH_UPPER && voltage > TOUCH_SPRING_THRESH_LOWER) {
            ESP_LOGI(TAG, "SPRING MODE...");
            return TOUCH_EVB_MODE_SPRING;
        } else if (voltage < TOUCH_CIRCLE_THRESH_UPPER && voltage > TOUCH_CIRCLE_THRESH_LOWER) {
            ESP_LOGI(TAG, "CIRCLE MODE...");
            return TOUCH_EVB_MODE_CIRCLE;
        }

#endif
        else {
            ESP_LOGE(TAG, "Unexpected ADC value...");
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

