#include <SPI.h>
#include <SD.h>

#define SD_CS    15
#define SD_SCK   11
#define SD_MISO  12
#define SD_MOSI  13

bool mountSdCard() {
  const uint32_t speeds[] = {400000, 1000000, 4000000};

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  for (uint8_t i = 0; i < 3; i++) {
    uint32_t speed = speeds[i];

    Serial.print("Trying SD @ ");
    Serial.println(speed);

    if (SD.begin(SD_CS, SPI, speed)) {
      Serial.println("SD mounted");
      return true;
    }

    delay(50);
  }

  return false;
}

void readTextFile() {
  File file = SD.open("/test.txt");

  if (!file) {
    Serial.println("Failed to open test.txt");
    return;
  }

  Serial.println("---- test.txt START ----");

  while (file.available()) {
    char c = file.read();
    Serial.print(c);
  }

  Serial.println("\n---- test.txt END ----");

  file.close();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!mountSdCard()) {
    Serial.println("SD Failed");
    return;
  }

  Serial.println("SD OK");

  readTextFile();
}

void loop() {
}