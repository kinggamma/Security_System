#include "mbed.h"
#include "MFRC522.h" // Library for RFID
#include "hcsr04.h" // Library for Ultrasonic Sensor

// Pin Definitions
#define RFID_SS_PIN D10
#define RFID_RST_PIN D9
#define ULTRASONIC_TRIGGER_PIN D5
#define ULTRASONIC_ECHO_PIN D6
#define LED_PIN D7
#define BUTTON_PIN D8

// Initialize objects
MFRC522 RfChip(D13, D10, D12, D11, D9); // MFRC522    RfChip   (SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS, MF_RESET)
HCSR04 ultrasonicSensor(D5, D6); // Trigger, Echo
DigitalOut led(D7); // LED pin
DigitalIn button(D8); // Button pin

bool isRFIDTriggered = false;

void setup() {
    // Initialize RFID and Ultrasonic Sensor
    RfChip.PCD_Init();
    ultrasonicSensor.start();
}

void loop() {
    // Check RFID
    if (RfChip.PICC_IsNewCardPresent() && RfChip.PICC_ReadCardSerial()) {
        isRFIDTriggered = true;
        // Disable Ultrasonic Sensor when RFID is triggered
        ultrasonicSensor.stop();

    }

    // Check Ultrasonic Sensor
    if (!isRFIDTriggered && ultrasonicSensor.get_dist_cm() < 100) {
        // Send warning to app
        // Note: Bluetooth code removed
    }

    // Check button press
    if (button) {
        led = !led; // Toggle LED
        // Note: Bluetooth code removed
    }

    // Optional: Add delay to reduce CPU usage
    ThisThread::sleep_for(100ms);
}

int main() {
    setup();
    while (true) {
        loop();
    }
}
