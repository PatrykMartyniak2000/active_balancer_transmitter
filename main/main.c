#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/timer.h"
#include "driver/gptimer.h"

#define SAMPLE_RATE_HZ 0.05
#define NUM_CHANNELS_ADC1 5
#define NUM_CHANNELS_ADC2 5

// ADC1 CHANNELS (GPIO36, GPIO37, GPIO38, GPIO39, GPIO32)
adc1_channel_t adc1_channels[NUM_CHANNELS_ADC1] = {
    ADC1_CHANNEL_0, ADC1_CHANNEL_1, ADC1_CHANNEL_2, ADC1_CHANNEL_3, ADC1_CHANNEL_4
};

// ADC2 CHANNELS (GPIO4, GPIO0, GPIO2, GPIO15, GPIO13)
adc2_channel_t adc2_channels[NUM_CHANNELS_ADC2] = {
    ADC2_CHANNEL_0, ADC2_CHANNEL_1, ADC2_CHANNEL_2, ADC2_CHANNEL_3, ADC2_CHANNEL_4
};

// DATA BUFFERS
#define BUFFER_SIZE 1
uint16_t adc1_buffer[NUM_CHANNELS_ADC1][BUFFER_SIZE];
uint16_t adc2_buffer[NUM_CHANNELS_ADC2][BUFFER_SIZE];

// FOR DEBUG
static const char *TAG = "ADC_";

void IRAM_ATTR timer_isr(void *arg) {
    static int idx = 0;

    // Odczyt ADC1 dla wszystkich kanałów
    for (int ch = 0; ch < NUM_CHANNELS_ADC1; ch++) {
        adc1_buffer[ch][idx] = adc1_get_raw(adc1_channels[ch]);
    }

    // Odczyt ADC2 dla wszystkich kanałów
    for (int ch = 0; ch < NUM_CHANNELS_ADC2; ch++) {
        int raw_value = 0;
        adc2_get_raw(adc2_channels[ch], ADC_WIDTH_BIT_12, &raw_value);
        adc2_buffer[ch][idx] = raw_value;
    }

    idx = (idx + 1) % BUFFER_SIZE; // Przejdź do kolejnej pozycji w buforze

    // Wyczyść flagę przerwania za pomocą funkcji ESP-IDF
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    // Opcjonalnie, ponownie włącz alarm (dla niektórych konfiguracji)
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}


// Funkcja konfiguracji timera
void configure_timer() {
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = true,
        .divider = 80 // Timer taktowany 1 MHz (80 MHz / 80)
    };

    // Konfiguracja Timer0
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000 / SAMPLE_RATE_HZ);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

void app_main() {
    ESP_LOGI(TAG, "Configuring ADC...");

    // Konfiguracja ADC1
    adc1_config_width(ADC_WIDTH_BIT_12);
    for (int ch = 0; ch < NUM_CHANNELS_ADC1; ch++) {
        adc1_config_channel_atten(adc1_channels[ch], ADC_ATTEN_DB_0);
    }

    // Konfiguracja ADC2
    for (int ch = 0; ch < NUM_CHANNELS_ADC2; ch++) {
        adc2_config_channel_atten(adc2_channels[ch], ADC_ATTEN_DB_0);
    }

    // Konfiguracja i uruchomienie timera
    configure_timer();

    // Główna pętla przetwarzania
    while (1) {
        // Przetwarzanie danych z buforów
        for (int i = 0; i < BUFFER_SIZE; i++) {
            for (int ch = 0; ch < NUM_CHANNELS_ADC1; ch++) {
                ESP_LOGI(TAG, "1[%d][%d]: %d", ch, i, adc1_buffer[ch][i]);
            }
            for (int ch = 0; ch < NUM_CHANNELS_ADC2; ch++) {
                ESP_LOGI(TAG, "2[%d][%d]: %d", ch, i, adc2_buffer[ch][i]);
            }
            printf("\n");
        }

        // Opóźnienie (lub inne operacje między odczytami)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
