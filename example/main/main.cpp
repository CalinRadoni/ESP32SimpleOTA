/**
This file is part of pax-devices (https://github.com/CalinRadoni/pax-devices)
Copyright (C) 2019+ by Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "ESP32SimpleOTA.h"

const char* TAG = "example";

extern "C" {

    void restart()
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        esp_restart();
    }

    void app_main()
    {
        esp_err_t err = simpleOTA.CheckApplicationImage();
        if (err == ESP_ERR_OTA_ROLLBACK_FAILED) {
            ESP_LOGE(TAG, "Current application image is NOT valid and rollback failed!");
            restart();
        }
        else if (err == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to determine the status of Current application image !");
        }

        ESP_LOGI(TAG, "Max accepted image size is %d bytes", simpleOTA.GetMaxImageSize());

        ESP_LOGW(TAG, "This SHOULD fail because the buffer content is wrong !");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGW(TAG, "This SHOULD fail because the buffer content is wrong !");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGW(TAG, "This SHOULD fail because the buffer content is wrong !");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        const uint32_t bufferLength = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
        char buffer[bufferLength];

        buffer[0] = ESP_IMAGE_HEADER_MAGIC + 1;
        buffer[1] = 0;

        err = simpleOTA.Begin();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "simpleOTA.Begin Failed");
            restart();
        }

        err = simpleOTA.Write(buffer, bufferLength);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "simpleOTA.Write Failed");
            restart();
        }
        err = simpleOTA.End();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "simpleOTA.End Failed");
            restart();
        }

        ESP_LOGI(TAG, "OTA done OK, you should restart");
        restart();
    }

}
