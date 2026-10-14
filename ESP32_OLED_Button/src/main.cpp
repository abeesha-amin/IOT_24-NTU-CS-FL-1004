#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const int BUTTON_PIN = 18;
const int SDA_PIN    = 21;
const int SCL_PIN    = 22;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const unsigned long DEBOUNCE_MS = 50;  
int lastRawState = HIGH;               
int stableState  = HIGH;               
unsigned long lastChangeTime = 0;      

int pressCount = 0;                    


void drawScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32 Button Counter");

  display.setCursor(0, 20);
  display.print("Button: ");
  display.println(stableState == LOW ? "PRESSED" : "RELEASED");

  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print("Count: ");
  display.print(pressCount);

  display.display();   // send the buffer to the screen
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED not found! Check wiring and address.");
    while (true) { delay(1000); }
  }

  Serial.println("System started. Press the button.");
  drawScreen();   // title, RELEASED, count = 0
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  
  if (reading != lastRawState) {
    lastChangeTime = millis();
    lastRawState = reading;
  }

  
  if ((millis() - lastChangeTime) >= DEBOUNCE_MS && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {          
      pressCount++;
      Serial.print("Accepted press count: ");
      Serial.println(pressCount);
    }
    drawScreen();                     
  }
}