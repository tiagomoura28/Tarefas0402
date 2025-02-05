#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

const uint LED_GPIN = 11;
const uint LED_BPIN = 12;
const uint LED_RPIN = 13;
const uint BUTTON_PIN = 5;

int contador = 0;
bool led_active = false;
bool led_on = false;
bool led_fon = false;

// Função de callback para desligar os LEDs após um tempo programado.
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    if (led_active) {
        gpio_put(LED_RPIN, false);
        gpio_put(LED_BPIN, true);
        gpio_put(LED_GPIN, true);
        printf("LED Vermelho desligado \n");
        led_active = false;
        
        if (led_on || led_fon) {
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
        }
        return 0;
    }
    
    if (led_on) {
        gpio_put(LED_RPIN, false);
        gpio_put(LED_BPIN, false);
        gpio_put(LED_GPIN, true);
        printf("LED Azul Desligado\n");
        led_on = false;
        
        if (led_fon) {
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
        }
       return 0;
    }
    
    if (led_fon) {
        gpio_put(LED_RPIN, false);
        gpio_put(LED_BPIN, false);
        gpio_put(LED_GPIN, false);
        printf("LED Verde Desligado\n");
        led_fon = false;
    }
   return 0;
}

int main() {
    stdio_init_all();
    
    gpio_init(LED_RPIN);
    gpio_set_dir(LED_RPIN, GPIO_OUT);
    gpio_init(LED_BPIN);
    gpio_set_dir(LED_BPIN, GPIO_OUT);
    gpio_init(LED_GPIN);
    gpio_set_dir(LED_GPIN, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        // Só permite o botão ser pressionado se todos os LEDs estiverem apagados
        if (!led_active && !led_on && !led_fon) {
            if (gpio_get(BUTTON_PIN) == 0) {
                sleep_ms(50);  // Debounce
                if (gpio_get(BUTTON_PIN) == 0) {
                    
                    sleep_ms(50);

                    // Liga os LEDs e define estados
                    gpio_put(LED_RPIN, true);
                    gpio_put(LED_BPIN, true);
                    gpio_put(LED_GPIN, true);
                    printf("Botão pressionado, LEDs ligados\n");
                    
                    led_active = true;
                    led_on = true;
                    led_fon = true;

                    // Inicia o primeiro alarme
                    add_alarm_in_ms(3000, turn_off_callback, NULL, false);
                }
            }
        }
        sleep_ms(50);
         // Introduz uma pequena pausa de 50 ms para reduzir o uso da CPU.
        // Isso evita que o loop seja executado muito rapidamente e consuma recursos desnecessários.

    }
    return 0;
}
