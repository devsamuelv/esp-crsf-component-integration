#include "ESP_CRSF.h"
#include "esp_task_wdt.h"
#include "driver/mcpwm.h"
#include "driver/ledc.h"
#include <esp_err.h>
#include <math.h>
#include <rtc_wdt.h>
#include <stdlib.h>
#include <esp_intr_alloc.h>

// Arbitrary
#define DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define LEDC_SPEED LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0

float remap_channel_precent(int value)
{
  if (value == 0)
  {
    return 0;
  }

  return (value - 922.0f) / 818.0f;
}

void main_thread()
{
  for (;;)
  {
    crsf_channels_t channels = {0};

    CRSF_receive_channels(&channels);

    float speed_channel = remap_channel_precent(channels.ch3);

    if (fabs(speed_channel) <= 0.05)
    {
      speed_channel = 0;
    }

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_SPEED, LEDC_CHANNEL, 5046 * fabs(speed_channel)));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_SPEED, LEDC_CHANNEL));

    printf("Channel 3 %f\n", speed_channel);
  }
}

void app_main()
{

  ledc_timer_config_t timer_config = {
      .speed_mode = LEDC_SPEED,
      .timer_num = LEDC_TIMER_0,
      .duty_resolution = DUTY_RESOLUTION,
      .freq_hz = 50};
  ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

  ledc_channel_config_t channel_config = {
      .channel = LEDC_CHANNEL,
      .speed_mode = LEDC_SPEED,
      .gpio_num = 19,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .hpoint = 0,
      .duty = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

  TaskHandle_t handle = NULL;
  crsf_config_t config = {
      .uart_num = UART_NUM_2,
      .tx_pin = 17,
      .rx_pin = 16};

  CRSF_init(&config);

  xTaskCreate(main_thread, "main", 8192, NULL, 12, &handle);

  ESP_ERROR_CHECK(esp_task_wdt_add(handle));
}