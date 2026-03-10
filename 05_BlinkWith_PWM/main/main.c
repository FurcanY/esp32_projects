#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

// gpio tanımlama
#define GPIO_LED 4
// ledc tanımlamaları
#define LEDC_TIMER      LEDC_TIMER_0         // timer 0 seçilir
#define LEDC_MODE       LEDC_HIGH_SPEED_MODE // yüksek hız modu seçilir
#define LEDC_RESOLUTION LEDC_TIMER_12_BIT    // 12 bit çözünürlük
#define LEDC_FREQ       5000                 // 5000 Hz ferkans
#define LEDC_CHANNEL    LEDC_CHANNEL_0       // Kanal 0 seçilir

// fonskiyon deklarasyonları
void timer_init ();
void led_task (void *param);

void led_1();
void led_2();

void app_main (void)
{
    // timer initialize edelim
    timer_init();

    // task oluşturma
    xTaskCreate(
        led_task  ,
        "led task 1",
        4096        ,
        NULL        ,
        1           ,
        NULL
    );

}


void timer_init () {

    // Timer Yapılandırması
    ledc_timer_config_t timer_conf = {
        .speed_mode         = LEDC_MODE       ,
        .duty_resolution    = LEDC_RESOLUTION ,
        .timer_num          = LEDC_TIMER      ,
        .freq_hz            = LEDC_FREQ       ,
        .clk_cfg            = LEDC_AUTO_CLK
    };

    // yapılandırmayı ayarlayalım
    ledc_timer_config(&timer_conf);

    // Kanal Yapılandırması
    ledc_channel_config_t channel_conf = {
        .gpio_num       = GPIO_LED          ,
        .speed_mode     = LEDC_MODE         ,
        .channel        = LEDC_CHANNEL      ,
        .intr_type      = LEDC_INTR_DISABLE ,
        .timer_sel      = LEDC_TIMER        ,
        .duty           = 0                 ,
        .hpoint         = 0                     
    };

    //yapılandırmayı ayarlayalım
    ledc_channel_config(&channel_conf);
}

void led_task (void *param){

    while (true)
    {
        led_1();
        vTaskDelay (100/portTICK_PERIOD_MS);
        led_2();
        vTaskDelay (100/portTICK_PERIOD_MS);
    }
    
}


void led_1 (){

    // Duty cycle ile parlaklık ayarı yapalım
    int duty = 0;
    int step = 100;

    for ( size_t i = 0; i < 3; ++i) {
        // parlaklığı artıralım
        for (duty = 0; duty < 4096; duty += step )
        {   
            // duty değiştir
            ledc_set_duty (
                LEDC_MODE       ,
                LEDC_CHANNEL    ,
                duty
            );
            // değişikliği uygula
            ledc_update_duty (
                LEDC_MODE,
                LEDC_CHANNEL
            );

            // küçük bir bekleme
            vTaskDelay (20/portTICK_PERIOD_MS);
        }

        // parlaklığı azaltalım
        for (duty = 4096; duty >= 0; duty -= step )
        {
            // duty değiştir
            ledc_set_duty (
                LEDC_MODE       ,
                LEDC_CHANNEL    ,
                duty
            );
            // değişikliği uygula
            ledc_update_duty (
                LEDC_MODE,
                LEDC_CHANNEL
            );

            // küçük bir bekleme
            vTaskDelay (20/portTICK_PERIOD_MS);
        }
    }
}
void led_2 (){
    // sert bir geçiş ile pwm'i gözlemleyelim
    int duty= 0;
    int step = 4096/7;

    for (size_t i = 0; i < 3; i++)
    {
        for (duty = 0; duty < 4096; duty += step)
        {
            // duty değiştir
            ledc_set_duty (
                LEDC_MODE       ,
                LEDC_CHANNEL    ,
                duty
            );
            // değişikliği uygula
            ledc_update_duty (
                LEDC_MODE,
                LEDC_CHANNEL
            );

            // küçük bir bekleme
            vTaskDelay (300/portTICK_PERIOD_MS);
        }

        for (duty = 4096; duty >= 0; duty -= step)
        {
            // duty değiştir
            ledc_set_duty (
                LEDC_MODE       ,
                LEDC_CHANNEL    ,
                duty
            );
            // değişikliği uygula
            ledc_update_duty (
                LEDC_MODE,
                LEDC_CHANNEL
            );

            // küçük bir bekleme
            vTaskDelay (300/portTICK_PERIOD_MS);
        }
        
    }
    
}

// idf.py -p /dev/ttyUSB0 flash ile cihaza kodu atarız.