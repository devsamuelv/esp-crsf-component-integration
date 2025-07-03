#include "ESP_CRSF.h"
#include "esp_task_wdt.h"
#include "driver/mcpwm.h";
#include "driver/ledc.h";
#include <esp_err.h>
#include <math.h>

// Arbitrary
#define DUTY_RESOLUTION 5

#define LEDC_SPEED LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0

float remap_channel_precent(int value)
{
  return (value - 922.0f) / 818.0f;
}

void main_thread()
{
  for (;;)
  {
    crsf_channels_t channels = {0};

    CRSF_receive_channels(&channels);

    // uint32_t desired_resolution = (channels.ch1);

    float speed_channel = remap_channel_precent(channels.ch3);

    // Not a thread-safe method
    ledc_set_duty(LEDC_SPEED, LEDC_CHANNEL, DUTY_RESOLUTION * speed_channel);

    printf("Channel 3 %f\n", speed_channel);
  }
}

void app_main()
{

  ledc_timer_config_t timer_config = {
      .speed_mode = LEDC_SPEED,
      .duty_resolution = DUTY_RESOLUTION,
      .freq_hz = 50};

  ledc_channel_config_t channel_config = {
      .channel = LEDC_CHANNEL,
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