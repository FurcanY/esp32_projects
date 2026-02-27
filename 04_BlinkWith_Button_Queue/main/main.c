#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define LED_GPIO 4
#define BUTTON_GPIO 5


// queue oluşturma
QueueHandle_t button_queue = NULL;

/* --- Task A ---*/ 
// bu task içerisinde button ile veri üretilip queue'ya gönderilecek
void task_a (void *pvParameters)
{
    // kolay olsun diye polling yapacağız
    int last_state = 1,state;

    while (true)
    {
        state = gpio_get_level (BUTTON_GPIO);

        // eğer basıldıysa son durumdan farklıdır.
        if (state != last_state) {
            //debounce için küçük bekleme
            vTaskDelay (20 / portTICK_PERIOD_MS);

            // tüm durumları queue atarız
            // queue doluysa bloklama
            xQueueSend(button_queue,&state,( TickType_t ) 0);
            last_state = state;
        }
        // task schedule için küçük bir bekleme
        vTaskDelay (50/portTICK_PERIOD_MS);
    }
    

}

/* --- Task B ---*/ 
// bu task içerisinde led yakılır.
void task_b (void *pvParameters)
{

    int button_state;
    while(true){
        // portMAX_DELAY ile veri gelesiye kadar bekle 
        if (xQueueReceive (button_queue,&button_state,portMAX_DELAY)){
            // veri geldiği zaman buraya girer

            // pullup olduğundan basıldığında 0 gelir
            if (!button_state) {
                gpio_set_level (LED_GPIO,1);
            }
            else {
                gpio_set_level (LED_GPIO,0);
            }
        }
        // task schedule için küçük bir bekleme
        vTaskDelay (50/portTICK_PERIOD_MS);
    }




}

void app_main(void)
{
    // LED ve BUTTON Ayarları
    // button
    gpio_reset_pin     (BUTTON_GPIO                     );
    gpio_set_direction (BUTTON_GPIO, GPIO_MODE_INPUT    );
    gpio_set_pull_mode (BUTTON_GPIO, GPIO_PULLUP_ONLY   ); // pull-up direnci
    // led
    gpio_reset_pin     (LED_GPIO                    );
    gpio_set_direction (LED_GPIO,GPIO_MODE_OUTPUT   );
    gpio_set_level     (LED_GPIO,0                  );
    
    // queue oluşturma
    button_queue = xQueueCreate(5,sizeof(int));

    // task1 oluşturma
    xTaskCreate(
        task_a,          // task fonksiyonu
        "button_task",   // task adı
        2048,            // stack boyutu
        NULL,     // parametre
        2,               // öncelik
        NULL             // task handle
    );

    // task2 oluşturma
    xTaskCreate(
        task_b,
        "led_task",
        2048,
        NULL,
        1,
        NULL
    );

    // main task silinebilir.
    
}