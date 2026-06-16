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

  for (uint8_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
    uint32_t speed = speeds[i];
    Serial.print("Trying SD on CS=");
    Serial.print(SD_CS);
    Serial.print(" @ ");
    Serial.print(speed);
    Serial.println(" Hz");

    if (SD.begin(SD_CS, SPI, speed)) {
      Serial.print("SD mounted at ");
      Serial.print(speed);
      Serial.println(" Hz");
      return true;
    }

    delay(50);
  }

  return false;
}

void showSdListing() {
  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open root directory");
    return;
  }

  File file = root.openNextFile();
  if (!file) {
    Serial.println("(no files)");
  }
  while (file) {
    Serial.print("FILE: ");
    Serial.print(file.name());
    Serial.print(" SIZE: ");
    Serial.println(file.size());
    file = root.openNextFile();
  }

  root.close();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  Serial.println("Initializing SD card...");
  delay(500);

  if (!mountSdCard()) {
    Serial.println("SD Card Mount Failed");
    return;
  }

  Serial.println("SD Card Mounted!");
  showSdListing();

}

void loop() {
}