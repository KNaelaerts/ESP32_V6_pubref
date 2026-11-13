#include "myPWM.h"

#include <stdbool.h>
#include <stddef.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_check.h"

//___ configuratie van de timer, volgens instellingen uit de h file
ledc_timer_config_t ledc_timer = 
{
    .speed_mode       = LEDC_MODE,
    .duty_resolution  = LEDC_DUTY_RES,
    .timer_num        = LEDC_TIMER,
    .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
    .clk_cfg          = LEDC_CLK_SRC,
};

//configuratie van 1 kanaal volgens h file
ledc_channel_config_t ledc_channel = 
{
    .speed_mode     = LEDC_MODE,
    .channel        = LEDC_CHANNEL,
    .timer_sel      = LEDC_TIMER,
    .gpio_num       = LEDC_OUTPUT_IO,
    .duty           = 1000, // Set duty to 0%
    .hpoint         = 0,
};

void myPWM_init(int pin, int kanaal)
{
    ledc_channel.channel = kanaal;
    ledc_channel.gpio_num = pin;

    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);

}



// Stel de dutycycle in als geheel percentage van 0 tot en met 100.
void myPWM_Duty(int kanaal, int duty_procent)
{
    ledc_set_duty(LEDC_MODE, kanaal, ((duty_procent * (LEDC_DUTY_RES<<13))+50)/100);
    // Update duty to apply the new value
    ledc_update_duty(LEDC_MODE, kanaal);
}

// Wijzig de frequentie in Hz zonder de dutycycle of andere PWM's te wijzigen.
void myPWM_setFreq(int pin, uint32_t frequentie)
{
    ledc_set_freq(LEDC_MODE, LEDC_TIMER, frequentie);
}




/*
// Pas de pinnummers aan je schakeling aan. Gebruik voor elke PWM een andere pin.
#define MYPWM_PIN_1 4
#define MYPWM_PIN_2 5
#define MYPWM_PIN_3 6
#define MYPWM_PIN_4 8

#define MYPWM_MODUS      LEDC_LOW_SPEED_MODE
#define MYPWM_RESOLUTIE  LEDC_TIMER_10_BIT
#define MYPWM_DUTY_MAX   (1U << MYPWM_RESOLUTIE)
#define MYPWM_KLOK       LEDC_AUTO_CLK


typedef struct {
    int pin;
    ledc_channel_t kanaal;
    ledc_timer_t timer;
    bool gestart;
} myPWM_instelling_t;

static myPWM_instelling_t pwm[] = {
    { MYPWM_PIN_1, LEDC_CHANNEL_0, LEDC_TIMER_0, false },
    { MYPWM_PIN_2, LEDC_CHANNEL_1, LEDC_TIMER_1, false },
    { MYPWM_PIN_3, LEDC_CHANNEL_2, LEDC_TIMER_2, false },
    { MYPWM_PIN_4, LEDC_CHANNEL_3, LEDC_TIMER_3, false },
};

static const char *TAG = "myPWM";

static myPWM_instelling_t *zoek_pwm(int pin)
{
    for (unsigned int i = 0; i < sizeof(pwm) / sizeof(pwm[0]); i++) {
        if (pwm[i].pin == pin) {
            return &pwm[i];
        }
    }
    return NULL;
}

esp_err_t myPWM_init(int pin, uint32_t frequentie)
{
    myPWM_instelling_t *instelling = zoek_pwm(pin);
    ESP_RETURN_ON_FALSE(instelling != NULL, ESP_ERR_INVALID_ARG, TAG, "Pin %d staat niet in de PWM-instellingen", pin);
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(pin), ESP_ERR_INVALID_ARG, TAG, "Pin %d is geen geldige uitgang", pin);
    ESP_RETURN_ON_FALSE(frequentie > 0, ESP_ERR_INVALID_ARG, TAG, "Frequentie moet groter zijn dan 0");
    ESP_RETURN_ON_FALSE(!instelling->gestart, ESP_ERR_INVALID_STATE, TAG, "Pin %d is al geinitialiseerd", pin);

    ledc_timer_config_t timer = {
        .speed_mode = MYPWM_MODUS,
        .timer_num = instelling->timer,
        .duty_resolution = MYPWM_RESOLUTIE,
        .freq_hz = frequentie,
        .clk_cfg = MYPWM_KLOK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "Timer configureren mislukt");

    ledc_channel_config_t kanaal = {
        .gpio_num = pin,
        .speed_mode = MYPWM_MODUS,
        .channel = instelling->kanaal,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = instelling->timer,
        .duty = 0,
        .hpoint = 0,
    };
    esp_err_t fout = ledc_channel_config(&kanaal);
    if (fout != ESP_OK) {
        ESP_LOGE(TAG, "Kanaal configureren mislukt: %s", esp_err_to_name(fout));
        esp_err_t opruimfout = ledc_timer_pause(MYPWM_MODUS, instelling->timer);
        if (opruimfout == ESP_OK) {
            timer.deconfigure = true;
            opruimfout = ledc_timer_config(&timer);
        }
        if (opruimfout != ESP_OK) {
            ESP_LOGE(TAG, "Timer opruimen mislukt: %s", esp_err_to_name(opruimfout));
        }
        return fout;
    }

    instelling->gestart = true;
    return ESP_OK;
}

esp_err_t myPWM_Duty(int pin, int duty)
{
    myPWM_instelling_t *instelling = zoek_pwm(pin);
    ESP_RETURN_ON_FALSE(instelling != NULL, ESP_ERR_INVALID_ARG, TAG, "Pin %d staat niet in de PWM-instellingen", pin);
    ESP_RETURN_ON_FALSE(instelling->gestart, ESP_ERR_INVALID_STATE, TAG, "Pin %d is niet geinitialiseerd", pin);
    ESP_RETURN_ON_FALSE(duty >= 0 && duty <= 100, ESP_ERR_INVALID_ARG, TAG, "Duty moet tussen 0 en 100 liggen");

    uint32_t waarde = ((uint32_t)duty * MYPWM_DUTY_MAX + 50U) / 100U;
    ESP_RETURN_ON_ERROR(ledc_set_duty(MYPWM_MODUS, instelling->kanaal, waarde), TAG, "Duty instellen mislukt");
    ESP_RETURN_ON_ERROR(ledc_update_duty(MYPWM_MODUS, instelling->kanaal), TAG, "Duty toepassen mislukt");
    return ESP_OK;
}

esp_err_t myPWM_setFreq(int pin, uint32_t frequentie)
{
    myPWM_instelling_t *instelling = zoek_pwm(pin);
    ESP_RETURN_ON_FALSE(instelling != NULL, ESP_ERR_INVALID_ARG, TAG, "Pin %d staat niet in de PWM-instellingen", pin);
    ESP_RETURN_ON_FALSE(instelling->gestart, ESP_ERR_INVALID_STATE, TAG, "Pin %d is niet geinitialiseerd", pin);
    ESP_RETURN_ON_FALSE(frequentie > 0, ESP_ERR_INVALID_ARG, TAG, "Frequentie moet groter zijn dan 0");

    ESP_RETURN_ON_ERROR(ledc_set_freq(MYPWM_MODUS, instelling->timer, frequentie), TAG, "Frequentie instellen mislukt");
    return ESP_OK;
}

*/
/*
#define MYPWM_RESOLUTIE LEDC_TIMER_10_BIT
#define MYPWM_DUTY_MAX (1U << 10)
#define MYPWM_AANTAL ((LEDC_TIMER_MAX < LEDC_CHANNEL_MAX) ? LEDC_TIMER_MAX : LEDC_CHANNEL_MAX)

static const char *TAG = "myPWM";

typedef struct {
    bool gebruikt;
    int pin;
} myPWM_kanaal_t;

static myPWM_kanaal_t kanalen[MYPWM_AANTAL];

static int zoek_pin(int pin)
{
    for (int i = 0; i < MYPWM_AANTAL; i++) {
        if (kanalen[i].gebruikt && kanalen[i].pin == pin) {
            return i;
        }
    }
    return -1;
}

esp_err_t myPWM_init(int pin, uint32_t frequentie)
{
    ESP_RETURN_ON_FALSE(pin >= 0 && GPIO_IS_VALID_OUTPUT_GPIO(pin),
                        ESP_ERR_INVALID_ARG, TAG, "Ongeldige uitgangspin: %d", pin);
    ESP_RETURN_ON_FALSE(frequentie > 0, ESP_ERR_INVALID_ARG, TAG, "Frequentie moet groter zijn dan 0");
    ESP_RETURN_ON_FALSE(zoek_pin(pin) < 0, ESP_ERR_INVALID_STATE, TAG, "Pin %d is al geinitialiseerd", pin);

    int kanaal = 0;
    while (kanaal < MYPWM_AANTAL && kanalen[kanaal].gebruikt) {
        kanaal++;
    }
    ESP_RETURN_ON_FALSE(kanaal < MYPWM_AANTAL, ESP_ERR_NOT_FOUND, TAG, "Geen vrije PWM-timer");

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = (ledc_timer_t)kanaal,
        .duty_resolution = MYPWM_RESOLUTIE,
        .freq_hz = frequentie,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "Timer configureren mislukt");

    ledc_channel_config_t uitgang = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)kanaal,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = (ledc_timer_t)kanaal,
        .duty = 0,
        .hpoint = 0,
    };
    esp_err_t err = ledc_channel_config(&uitgang);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Kanaal configureren mislukt: %s", esp_err_to_name(err));
        esp_err_t cleanup_err = ledc_timer_pause(LEDC_LOW_SPEED_MODE, timer.timer_num);
        if (cleanup_err == ESP_OK) {
            timer.deconfigure = true;
            cleanup_err = ledc_timer_config(&timer);
        }
        if (cleanup_err != ESP_OK) {
            ESP_LOGE(TAG, "Timer opruimen mislukt: %s", esp_err_to_name(cleanup_err));
        }
        return err;
    }

    kanalen[kanaal].pin = pin;
    kanalen[kanaal].gebruikt = true;
    return ESP_OK;
}

esp_err_t myPWM_Duty(int pin, int duty)
{
    ESP_RETURN_ON_FALSE(duty >= 0 && duty <= 100, ESP_ERR_INVALID_ARG, TAG, "Duty moet tussen 0 en 100 liggen");
    int kanaal = zoek_pin(pin);
    ESP_RETURN_ON_FALSE(kanaal >= 0, ESP_ERR_INVALID_STATE, TAG, "Pin %d is niet geinitialiseerd", pin);

    uint32_t waarde = ((uint32_t)duty * MYPWM_DUTY_MAX + 50U) / 100U;
    ESP_RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)kanaal, waarde),
                        TAG, "Duty instellen mislukt");
    ESP_RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)kanaal),
                        TAG, "Duty toepassen mislukt");
    return ESP_OK;
}

esp_err_t myPWM_setFreq(int pin, uint32_t frequentie)
{
    ESP_RETURN_ON_FALSE(frequentie > 0, ESP_ERR_INVALID_ARG, TAG, "Frequentie moet groter zijn dan 0");
    int kanaal = zoek_pin(pin);
    ESP_RETURN_ON_FALSE(kanaal >= 0, ESP_ERR_INVALID_STATE, TAG, "Pin %d is niet geinitialiseerd", pin);

    ESP_RETURN_ON_ERROR(ledc_set_freq(LEDC_LOW_SPEED_MODE, (ledc_timer_t)kanaal, frequentie),
                        TAG, "Frequentie instellen mislukt");
    return ESP_OK;
}
*/