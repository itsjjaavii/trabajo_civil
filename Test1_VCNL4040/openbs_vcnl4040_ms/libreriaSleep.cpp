#include "libreriaSleep.hpp"

void sleepESP(uint64_t time_in_s)
{
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); //Inhabilito todas las formas de despertar
  esp_sleep_enable_timer_wakeup(time_in_s * uS_TO_S_FACTOR); //Habilito la forma por timer
  
  //Proximas lineas no se usarlas, se supone que podría mejorarse el consumo, actualmente está en 76uA.

  //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF); 
  //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  //esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);

  esp_deep_sleep_start();
  return;
}