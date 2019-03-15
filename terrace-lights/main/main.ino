#include <SoftwareSerial.h>
#include <FastLED.h>

#define G_LED_PIN 13
#define G_RX_PIN  10
#define G_TX_PIN  11

#define CHANDELIER_COUNT 10
#define LAMP_COUNT_PER_CHANDELIER 40
#define DIODS_COUNT (CHANDELIER_COUNT * LAMP_COUNT_PER_CHANDELIER)

#define UP_SIN_DIRECTION 1
#define DOWN_SIN_DIRECTION -1
#define MAX_BRIGHTNESS 100
#define MIN_BRIGHTNESS 0

#define STEP_SIZE_FOR_SIN_MODE 10

#define LED_PIN     7
#define NUM_LEDS    20

enum lighting_mode {
  ALWAYS,
  BLINK,
  SIN
};

CRGB randomColor();
void fillAllDiodsByCurrentColor();
void randomizeColors();
void fillSinMode();
void drawLight();

lighting_mode current_lighting_mode = ALWAYS;
CRGB current_color = randomColor();
int current_brightness = MAX_BRIGHTNESS;
int current_blink_saturation = 50;
int current_sin_direction = UP_SIN_DIRECTION;

CRGB diods_colors[DIODS_COUNT];

SoftwareSerial BTSerial(G_RX_PIN, G_TX_PIN);

void setup() {
  BTSerial.begin(38400);
  Serial.begin(9600);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(diods_colors, DIODS_COUNT);

  fillAllDiodsByCurrentColor();

  delay(500);
}

CRGB randomColor() {
  return CRGB(random(255), random(255), random(255));
};

void fillAllDiodsByCurrentColor() {
  for(int index = 0; index < DIODS_COUNT; index++) {
    diods_colors[index] = current_color;
  }
};

void randomizeColors() {
  for(int index = 0; index < DIODS_COUNT; index++) {
    int current_led_must_be_filled = random(100) < current_blink_saturation;

    if(current_led_must_be_filled == true) {
      diods_colors[index] = randomColor();
    } else {
      diods_colors[index] = CRGB(0, 0, 0);
    }
  }
};

void fillSinMode() {
  current_brightness += current_sin_direction * STEP_SIZE_FOR_SIN_MODE;

  if(current_brightness == MAX_BRIGHTNESS || current_brightness == MIN_BRIGHTNESS) {
    current_sin_direction ^= UP_SIN_DIRECTION;

    if(current_sin_direction == UP_SIN_DIRECTION) {
      current_color = randomColor();
    }
  }

  fillAllDiodsByCurrentColor();
};

void drawLight() {
  switch(current_lighting_mode) {
    case ALWAYS: {
      fillAllDiodsByCurrentColor();
      break;
    }

    case BLINK: {
      randomizeColors();
      break;
    }

    case SIN: {
      fillSinMode();
      break;
    }
  }

  FastLED.setBrightness(current_brightness);
  FastLED.show();
  delay(400);
};

void loop() {
  //if (BTSerial.available()) {
  //    Serial.write(BTSerial.read());
  //}
  //if (Serial.available()) {
  //  BTSerial.write(Serial.read());
  //}

  //readNewMode(); // in one thread

  drawLight();
}
