#include "ESP_CRSF.h"
#include "esp_task_wdt.h"
#include "driver/mcpwm.h";
#include "driver/ledc.h";
#include <esp_err.h>

// Arbitrary
#define DUTY_RESOLUTION 50

void main_thread()
{
  for (;;)
  {
    crsf_channels_t channels = {0};

    CRSF_receive_channels(&channels);

    printf("Channel 1 %d\n", channels.ch1);
  }
}

void app_main()
{

  ledc_timer_config_t timer_config = {
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .duty_resolution = DUTY_RESOLUTION,
      .freq_hz = 1000};

  ledc_channel_config_t channel_config = {
      .channel = LEDC_CHANNEL_0,
      .gpio_num = 21,
      .duty = (2 * DUTY_RESOLUTION),
  };

  TaskHandle_t handle = NULL;
  crsf_config_t config = {
      .uart_num = UART_NUM_2,
      .tx_pin = 17,
      .rx_pin = 16};

  ledc_timer_config(&timer_config);
  ledc_channel_config(&channel_config);

  CRSF_init(&config);

  xTaskCreate(main_thread, "main", 3072, NULL, 12, NULL);
  esp_task_wdt_add(handle);
}