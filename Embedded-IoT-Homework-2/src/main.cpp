// Homework 2 - Task 1: Dual LED PWM fading (different PWM frequencies)
// Name: Abeesha Amin   Reg#: 24-NTU-CS-FL-1004


#include <Arduino.h>

#define LED1_PIN   18
#define LED2_PIN   19

// Core 2.x only: use channels on DIFFERENT timers (0-1 share timer 0, 2-3 share timer 1)
#define LED1_CH    0
#define LED2_CH    2

#define LED1_FREQ  1000      // Hz
#define LED2_FREQ  5000      // Hz
#define PWM_RES    8         // 8-bit -> duty 0..255
#define MAX_DUTY   255

int duty1 = 0,        step1 = 2;    // LED1 fades up first, slower
int duty2 = MAX_DUTY, step2 = -3;   // LED2 fades down first, faster
unsigned long lastFade = 0, lastPrint = 0;
const unsigned long FADE_INTERVAL_MS = 30;

void setDuty(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? LED1_CH : LED2_CH, duty);
#endif
}

void setup() {
  Serial.begin(115200);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED1_PIN, LED1_FREQ, PWM_RES);
  ledcAttach(LED2_PIN, LED2_FREQ, PWM_RES);
#else
  ledcSetup(LED1_CH, LED1_FREQ, PWM_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);
  ledcSetup(LED2_CH, LED2_FREQ, PWM_RES);
  ledcAttachPin(LED2_PIN, LED2_CH);
#endif
  setDuty(1, duty1);
  setDuty(2, duty2);
}

void loop() {
  if (millis() - lastFade >= FADE_INTERVAL_MS) {
    lastFade = millis();

    duty1 += step1;
    if (duty1 >= MAX_DUTY) { duty1 = MAX_DUTY; step1 = -step1; }
    if (duty1 <= 0)        { duty1 = 0;        step1 = -step1; }

    duty2 += step2;
    if (duty2 >= MAX_DUTY) { duty2 = MAX_DUTY; step2 = -step2; }
    if (duty2 <= 0)        { duty2 = 0;        step2 = -step2; }

    setDuty(1, duty1);
    setDuty(2, duty2);

    if (millis() - lastPrint >= 250) {   // print 4 times per second, not every step
      lastPrint = millis();
      Serial.printf("LED1 %3d%% @ %d Hz | LED2 %3d%% @ %d Hz\n",
                  duty1 * 100 / MAX_DUTY, LED1_FREQ,
                  duty2 * 100 / MAX_DUTY, LED2_FREQ);
    }
  }
}