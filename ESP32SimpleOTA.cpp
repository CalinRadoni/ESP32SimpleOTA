/**
This file is part of ESP32SimpleOTA esp-idf component
(https://github.com/CalinRadoni/ESP32SimpleOTA)
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

#include "ESP32SimpleOTA.h"
#include "esp_log.h"

// -----------------------------------------------------------------------------

static const char* TAG = "SimpleOTA";

ESP32SimpleOTA simpleOTA;

// -----------------------------------------------------------------------------

ESP32SimpleOTA::ESP32SimpleOTA(void)
{
    otaHandle = 0;
}

ESP32SimpleOTA::~ESP32SimpleOTA(void)
{
    if (otaHandle != 0) {
        esp_ota_end(otaHandle);
    }
}

esp_err_t ESP32SimpleOTA::CheckApplicationImage(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    if (running == NULL) {
        ESP_LOGE(TAG, "esp_ota_get_running_partition failed");
        return ESP_FAIL;
    }

    esp_ota_img_states_t ota_state;
    esp_err_t err = esp_ota_get_state_partition(running, &ota_state);
    if ( err != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_ota_get_state_partition", err);
        return ESP_FAIL;
    }

    if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
        if (ApplicationImageDiagnose()) {
            ESP_LOGI(TAG, "Application Image Diagnose OK, continuing execution ...");
            esp_ota_mark_app_valid_cancel_rollback();
        }
        else {
            ESP_LOGE(TAG, "Application Image Diagnose failed! Rollback to the previous version ...");
            esp_ota_mark_app_invalid_rollback_and_reboot();
            return ESP_ERR_OTA_ROLLBACK_FAILED;
        }
    }

    return ESP_OK;
}

bool ESP32SimpleOTA::ApplicationImageDiagnose(void)
{
    return true;
}

uint32_t ESP32SimpleOTA::GetMaxImageSize(void)
{
    const esp_partition_t* updatePart = esp_ota_get_next_update_partition(NULL);
    if (updatePart == NULL) {
        ESP_LOGE(TAG, "No usable OTA partition !");
        return 0;
    }
    return updatePart->size;
}

esp_err_t ESP32SimpleOTA::Begin(void) {
    if (otaHandle != 0) {
        esp_ota_end(otaHandle);
        otaHandle = 0;
    }

    const esp_partition_t* runningPart = esp_ota_get_running_partition();
    if (runningPart != NULL) {
        ESP_LOGI(TAG, "Running partition type %d, subtype %d, offset 0x%08x",
                        runningPart->type,
                        runningPart->subtype,
                        runningPart->address);
    }

    const esp_partition_t* updatePart = esp_ota_get_next_update_partition(NULL);
    if (updatePart == NULL) {
        ESP_LOGE(TAG, "No usable OTA partition !");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
                    updatePart->subtype,
                    updatePart->address);

    esp_err_t err = esp_ota_begin(updatePart, OTA_SIZE_UNKNOWN, &otaHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_ota_begin", err);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t ESP32SimpleOTA::Write(char* data, int length)
{
    if (otaHandle == 0) return ESP_FAIL;
    if (data == nullptr) return ESP_FAIL;
    if (length <= 0) return ESP_FAIL;

    esp_err_t err = esp_ota_write(otaHandle, data, length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_ota_write", err);
        return err;
    }
    return ESP_OK;
}

esp_err_t ESP32SimpleOTA::End(void)
{
    esp_err_t err = esp_ota_end(otaHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_ota_end", err);
        return err;
    }
    otaHandle = 0;

    const esp_partition_t* updatePart = esp_ota_get_next_update_partition(NULL);
    if (updatePart == NULL) {
        ESP_LOGE(TAG, "Failed to get update partition");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(updatePart);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%d esp_ota_set_boot_partition", err);
        return err;
    }

    return ESP_OK;
}
