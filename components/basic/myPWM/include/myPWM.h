#pragma once

#include <stdint.h>
#include "esp_err.h"

#define LEDC_TIMER              0                           //je hebt 4 timers dus hier kies je er 1
#define LEDC_MODE               LEDC_LOW_SPEED_MODE         //je neemt normaal altijd low speed mode
#define LEDC_OUTPUT_IO          4                           //hier kies je de pin die je gebruikt, maar die stel je normaal ook in bij init
#define LEDC_CHANNEL            0                           //je hebt 8 mogelijke kanalen(0..7), dit is dus je referentie voor het vervolg
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT           // Set duty resolution to 13 bits ()
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_CLK_SRC            LEDC_AUTO_CLK
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

// Configureer een uitgang met een frequentie in Hz en een beginduty van 0%.
// Stel de pinnen vooraf in bovenaan myPWM.c. Elke pin krijgt een eigen timer.
void myPWM_init(int pin, int kanaal);

// Stel de dutycycle in als geheel percentage van 0 tot en met 100.
void myPWM_Duty(int kanaal, int duty_procent);

// Wijzig de frequentie in Hz zonder de dutycycle of andere PWM's te wijzigen.
void myPWM_setFreq(int pin, uint32_t frequentie);

// C-majeur, vierde octaaf; frequenties afgerond op hele hertz.
#define MYPWM_TOON_DO    262
#define MYPWM_TOON_RE    294
#define MYPWM_TOON_MI    330
#define MYPWM_TOON_FA    349
#define MYPWM_TOON_SOL   392
#define MYPWM_TOON_LA    440
#define MYPWM_TOON_SI    494
#define MYPWM_TOON_DO_HOOG 523
