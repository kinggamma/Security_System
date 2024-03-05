#include "mbed.h"
#include "MFRC522.h" // Library for RFID
#include "hcsr04.h" // Library for Ultrasonic Sensor
#include "BufferedSerial.h" // Library for Buffered Serial communication

// Define byte type
typedef unsigned char byte;


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
BufferedSerial bluetooth(D1, D0); // TX, RX pins for Buffered Serial

bool isRFIDTriggered = false;

// Expected RFID tag UID (modify according to your requirements)
byte expectedUID[] = {0x11, 0x22, 0x33, 0x44};

void setup() {
    // Initialize RFID and Ultrasonic Sensor
    RfChip.PCD_Init();
    ultrasonicSensor.start();
}

bool compareUID(byte* uid1, byte* uid2, int length) {
    for (int i = 0; i < length; i++) {
        if (uid1[i] != uid2[i]) {
            return false;
        }
    }
    return true;
}


void loop() {
    // Check RFID
    if (RfChip.PICC_IsNewCardPresent()) {
        if (RfChip.PICC_ReadCardSerial()) {
            // Compare UID with expected UID
            byte* uid = RfChip.uid.uidByte;
            if (compareUID(uid, expectedUID, RfChip.uid.size)) {
                isRFIDTriggered = true;
                // Disable Ultrasonic Sensor when RFID is triggered
                ultrasonicSensor.stop();
                // Send message to app
                bluetooth.write("RFID tag detected and authorized.\r\n", 34); // 34 is the length of the message
            }
        }
    }


    // Check Ultrasonic Sensor
    if (!isRFIDTriggered && ultrasonicSensor.get_dist_cm() < 100) {
        // Send warning to app
         bluetooth.write("Unauthorized access detected!\r\n", 31); // 31 is the length of the message
    }

    // Check button press
    if (button) {
        led = !led; // Toggle LED
        if (led)
            bluetooth.write("LED is ON\r\n", 11); // 11 is the length of the message
        else
            bluetooth.write("LED is OFF\r\n", 12); // 12 is the length of the message
    }
    
    // Check for Bluetooth commands from app
    if (bluetooth.readable()) {
        char cmd;
        bluetooth.read(&cmd, 1);
        if (cmd == 'R') { // Reset
            isRFIDTriggered = false;
            ultrasonicSensor.start(); // Enable Ultrasonic Sensor
            bluetooth.write("System reset\r\n", 14); // 14 is the length of the message
        }
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



// code to get the uid of the nfc tag before running the code
/*
#include "mbed.h"
#include "MFRC522.h" // Library for RFID

// Pin Definitions
#define RFID_SS_PIN D10
#define RFID_RST_PIN D9

// Initialize objects
MFRC522 RfChip(D13, D10, D12, D11, D9); // MFRC522 RfChip(SPI_MOSI, SPI_MISO, SPI_SCLK, SPI_CS, MF_RESET)

void setup() {
    // Initialize RFID
    RfChip.PCD_Init();
}

void loop() {
    // Check if a new card is present
    if (RfChip.PICC_IsNewCardPresent()) {
        // Read the card's UID
        if (RfChip.PICC_ReadCardSerial()) {
            printf("UID: ");
            for (byte i = 0; i < RfChip.uid.size; i++) {
                printf("%X", RfChip.uid.uidByte[i]);
            }
            printf("\r\n");
        }
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
*/
