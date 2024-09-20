
#include "UpdateManager.h"
#include "HTTPClient.h"
#include "esp_log.h"

static const char *TAG = "UpdateManager";

/**
 * @brief Construct a new UpdateManager::UpdateManager object
 */

//---------------------------------------------------------------------------------------------
UpdateManager::UpdateManager()
{
	_feedback = UPDATE_FEEDBACK_OK;
	ArduinoOTA.onStart(std::bind(&UpdateManager::onStart, this));
	ArduinoOTA.onProgress(std::bind(&UpdateManager::onProgress, this, std::placeholders::_1, std::placeholders::_2));
	ArduinoOTA.onEnd(std::bind(&UpdateManager::onEnd, this));
	ArduinoOTA.onError(std::bind(&UpdateManager::onError, this, std::placeholders::_1));
	ArduinoOTA.begin();
}

/**
 * @brief Set the progress callback
 *
 * @param callback the callback function
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::setProgressCallback(ProgressCallback callback) 
{
	_progressCallback = callback;
}

/**
 * @brief Update the firmware
 *
 * @param url the url of the firmware
 * @param callback the callback function (optional)
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::updateFirmware(const char *url, ProgressCallback callback) 
{
	if (callback) setProgressCallback(callback);

	_url = strdup(url);
	startUpdate();
}

/* PRIVATE METHODS */

/**
 * @brief Start the update
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::startUpdate() 
{
	ESP_LOGI(TAG, "Update: Updating firmware: %s\n", _url);
	Serial.printf("(%s) Update: Updating firmware: %s\n", __FUNCTION__, _url);

	_lastPercentage = 0;

	if (httpUpdate(_url)) 
    {
		ESP_LOGI(TAG, "Update done!");
		Serial.printf("(%s) Update done!\r\n", __FUNCTION__);
		_feedback = UPDATE_FEEDBACK_UPDATE_OK;
	}
}

/**
 * @brief On start of the update
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::onStart() 
{
}

/**
 * @brief On progress of the update
 *
 * @param progress the progress
 * @param total the total
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::onProgress(unsigned int progress, unsigned int total) 
{
	float percent = progress / (total / 100.0);

	if ((u_int8_t)percent > _lastPercentage) 
    {
		_lastPercentage = percent;
		if (_progressCallback) _progressCallback((int)percent);
	}
}

/**
 * @brief On end of the update
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::onEnd() 
{
	ESP_LOGI(TAG, "Update Done!");
	Serial.printf("(%s) Update Done!\r\n", __FUNCTION__);
	_feedback = UPDATE_FEEDBACK_UPDATE_OK;

}

/**
 * @brief On error of the update
 *
 * @param error the error
 */
//---------------------------------------------------------------------------------------------
void UpdateManager::onError(ota_error_t error) 
{
	ESP_LOGE(TAG, "Error[%u]: ", error);
	Serial.printf("(%s) Error[%u]: ", __FUNCTION__, error);
	_feedback = UPDATE_FEEDBACK_UPDATE_ERROR;

}

/**
 * @brief Update the firmware via http
 *
 * @param url the url
 * @return true if the update was successful
 * @return false if the update was not successful
 */
//---------------------------------------------------------------------------------------------
bool UpdateManager::httpUpdate(const char *url) 
{
	HTTPClient httpClient;
	httpClient.useHTTP10(true);
	httpClient.setTimeout(5000);

	httpClient.begin(url);

	int result = httpClient.GET();
	if (result != HTTP_CODE_OK) 
    {
		ESP_LOGE(TAG, "HTTP ERROR CODE: %d\n", result);
		Serial.printf("(%s) HTTP ERROR CODE: %d\n", __FUNCTION__, result);
		_feedback = UPDATE_FEEDBACK_UPDATE_ERROR;
		return false;
	}
	int httpSize = httpClient.getSize();

	if (!Update.begin(httpSize, U_FLASH)) 
	{
		ESP_LOGE(TAG, "ERROR: %s\n", httpClient.errorToString(httpClient.GET()));
		Serial.printf("(%s) ERROR: %s\n", __FUNCTION__, httpClient.errorToString(httpClient.GET()));
		_feedback = UPDATE_FEEDBACK_UPDATE_ERROR;
		return false;
	}

	uint8_t buff[1024] = {0};
	size_t sizePack;

	WiFiClient *stream = httpClient.getStreamPtr();
	while (httpClient.connected() && (httpSize > 0 || httpSize == -1)) 
    {
		sizePack = stream->available();
		if (sizePack) 
        {
			int c = stream->readBytes(buff, ((sizePack > sizeof(buff)) ? sizeof(buff) : sizePack));
			Update.write(buff, c);
			if (httpSize > 0) httpSize -= c;
		}
		int percent = int(Update.progress() * 100 / httpClient.getSize());
		if (percent > _lastPercentage) 
        {
			_lastPercentage = percent;
			if (_progressCallback) _progressCallback(percent);
		}
	}
	if (!Update.end()) 
    {
		ESP_LOGE(TAG, "ERROR: %s\n", Update.getError());
		Serial.printf("(%s) ERROR: %s\n", __FUNCTION__, Update.getError());
		_feedback = UPDATE_FEEDBACK_UPDATE_ERROR;
		return false;
	}
	httpClient.end();
	Serial.printf("(%s) OK!\r\n", __FUNCTION__);
	_feedback = UPDATE_FEEDBACK_UPDATE_OK;

	return true;
}


//--------------------------------------------------------------------
bool UpdateManager::feedback(int8_t check) 
{
  if (check == _feedback)
  {
    _feedback = UPDATE_FEEDBACK_OK;
    return true;
  }
  return false;
}

