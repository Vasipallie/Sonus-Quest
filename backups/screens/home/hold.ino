#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "FreeSans9pt7b.h"

#define TFT_CS   10
#define TFT_RST  14
#define TFT_DC   16
#define TFT_MOSI 13
#define TFT_SCLK 11
#define TFT_BL   17

const int btndown = 1;
constexpr int SD_CS = 4;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; 

class LopakaST7735 : public Adafruit_ST7735 {
 public:
  using Adafruit_ST7735::Adafruit_ST7735;

  void setFreeFont(const GFXfont* f) {
    setFont(f);
  }

  int16_t drawString(const char* text, int16_t x, int16_t y) {
    int16_t x1, y1;
    uint16_t w, h;

    getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    setCursor(x, y - y1);
    print(text);
    return static_cast<int16_t>(w);
  }

  int16_t drawString(const String& text, int16_t x, int16_t y) {
    return drawString(text.c_str(), x, y);
  }
};

LopakaST7735 tft = LopakaST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Bluetooth icon
static const unsigned char PROGMEM image_bluetooth_bits[] = {
0x01,0x00,0x02,0x80,0x02,0x40,0x22,0x20,
0x12,0x20,0x0a,0x40,0x06,0x80,0x03,0x00,
0x06,0x80,0x0a,0x40,0x12,0x20,0x22,0x20,
0x02,0x40,0x02,0x80,0x01,0x00,0x00,0x00
};

static const unsigned char PROGMEM image_wifi_2_bars_bits[] = {0x01,0xf0,0x00,0x06,0x0c,0x00,0x18,0x03,0x00,0x21,0xf0,0x80,0x46,0x0c,0x40,0x88,0x02,0x20,0x10,0xe1,0x00,0x23,0x18,0x80,0x04,0x04,0x00,0x08,0x42,0x00,0x01,0xf0,0x00,0x03,0xb8,0x00,0x01,0x50,0x00,0x00,0xe0,0x00,0x00,0x40,0x00,0x00,0x00,0x00};

// Signal icon
static const unsigned char PROGMEM image_network_3_bars_bits[] = {
0x00,0x0e,0x00,0x0a,0x00,0x0a,0x00,0x0a,
0x00,0xea,0x00,0xea,0x00,0xea,0x00,0xea,
0x0e,0xea,0x0e,0xea,0x0e,0xea,0x0e,0xea,
0xee,0xea,0xee,0xea,0xee,0xee,0x00,0x00
};
const int BUTTON_PIN = 2;
void setup() {

  // BUTTON 
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(0);
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);

  drawUI();
}
int numeri = 1000;
void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    Serial.println("Button is being pressed!");
    tft.fillRect(0, 0, 128, 100, ST77XX_GREEN);
    drawsyn();
  }
  delay(50);
  // TESTING OUT TIMER
  while(true){
    delay(1000);
    tft.fillRect(0, 149, 50, 19, ST77XX_GREEN);
    str
    tft.setTextColor(0xEF7D);
    tft.drawString(, -1, 144);
  }
}

void drawsyn(){

tft.setTextColor(0xE459);
tft.setTextSize(1);
tft.setFreeFont(&FreeSans9pt7b);
tft.drawString("music", 1, 2);
tft.drawString("videos", 2, 19);
tft.setTextColor(0xFFFF);
tft.setTextSize(2);
tft.drawString("synk", 1, 32);
tft.setTextColor(0xE459);
tft.setTextSize(1);
tft.drawString("connect", 1, 66);
tft.drawLine(0, 141, 127, 141, 0xE459);
tft.setTextColor(0xEF7D);
tft.drawString("settings", 1, 81);
}

void drawUI() {
  tft.fillScreen(ST77XX_BLACK);
  // [BEGIN lopaka generated]
tft.setTextColor(0xFFFF);
tft.setTextSize(2);
tft.setFreeFont(&FreeSans9pt7b);
tft.drawString("music", 1, 0);
tft.setTextColor(0xE459);
tft.setTextSize(1);
tft.drawString("videos", 2, 31);
tft.drawString("synk", 1, 48);
tft.drawString("connect", 1, 64);
tft.drawLine(0, 141, 127, 141, 0x9370);
tft.setTextColor(0xEF7D);
tft.drawString("10:20p", -1, 144);
tft.drawBitmap(97, 144, image_network_3_bars_bits, 15, 16, 0xEF7D);
tft.drawBitmap(114, 144, image_bluetooth_bits, 14, 16, 0xEF7D);
tft.setTextColor(0xE459);
tft.drawString("settings", 1, 79);
  // [END lopaka generated]
}