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

#ifndef ESP32SimpleOTA_H
#define ESP32SimpleOTA_H

#include "freertos/FreeRTOS.h"
#include "esp_ota_ops.h"

class ESP32SimpleOTA
{
public:
    ESP32SimpleOTA(void);
    virtual ~ESP32SimpleOTA(void);

    /**
     * @brief Check if current application image is OK
     *
     * If current application image is in the ESP_OTA_IMG_PENDING_VERIFY state
     * this function will check the validity of appication image by calling
     * the ApplicationImageDiagnose function.
     *
     * If ApplicationImageDiagnose returns true the image is marked as valid
     * and rollback is canceled.
     *
     * If ApplicationImageDiagnose returns false the function will call the
     * initiate the rollback and reboot functionality. In case of rollback failure
     * ESP_ERR_OTA_ROLLBACK_FAILED will be returned
     *
     * @return
     *  - ESP_OK if image is OK
     *  - ESP_FAIL if the status could not be determined
     *  - ESP_ERR_OTA_ROLLBACK_FAILED if rollback failed
     */
    esp_err_t CheckApplicationImage(void);

    /**
     * @brief Return the maximum allowed image size
     *
     * @return
     *  - the size of the next update partition
     *  - zero in case of any error
     */
    uint32_t GetMaxImageSize(void);

    /**
     * @brief The Begin from Begin -> Write -> ... -> Write -> End
     *
     * @return ESP_OK or ESP_FAIL
     */
    esp_err_t Begin(void);

    /**
     * @brief The Write from Begin -> Write -> ... -> Write -> End
     *
     * @return ESP_OK or ESP_FAIL
     */
    esp_err_t Write(char* data, int length);

    /**
     * @brief The End from Begin -> Write -> ... -> Write -> End
     *
     * If the return value is ESP_OK, the updated partition has
     * been set as the new boot partition.
     *
     * @return ESP_OK or ESP_FAIL
     */
    esp_err_t End(void);

protected:

    esp_ota_handle_t otaHandle;

    /**
     * @brief Check validity of current application image
     *
     * Currently this function just returns true.
     */
    virtual bool ApplicationImageDiagnose(void);

private:
};

extern ESP32SimpleOTA simpleOTA;

#endif
