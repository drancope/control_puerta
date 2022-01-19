//#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Servo.h>

#define ABIERTA (1)
#define CERRADA (0)

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (12)
#define PN532_MOSI (10)
#define PN532_SS   (9)
#define PN532_MISO (11)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
Servo servoMotor;
long int tiempo = millis();
void puerta();

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
int estado_puerta = CERRADA;
int orden_puerta = CERRADA;

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero
  Serial.println("Buenos días, Dave");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("No se ha encontrado un módulo PN53x.");
    while (1); // halt
    }
  servoMotor.attach(6);

  // Got ok data, print it out!
  Serial.print("Encontrado un chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Esperando una tarjeta ISO14443A");
}

void loop(void) {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100);
  uint8_t autorizado[] = { 4, 112, 29, 34, 151, 60, 128 };

  bool exito = true;
  if (success) {
    Serial.println("Tarjeta encontrada!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");

    for (uint8_t i=0; i < uidLength; i++)
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
      if (uid[i] != autorizado[i]) {
        exito = false;
      }
    }
    Serial.println("");
    if (exito) {
      Serial.print("exito. Puerta en estado ");
      Serial.println((int) estado_puerta);
      orden_puerta = ABIERTA;
   }
  }
  else
  {
    // PN532 probably timed out waiting for a card
    Serial.println("Tiempo de espera cumplido.");
  }
  puerta();

  digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
  digitalWrite(LED_BUILTIN, LOW);
	delay(900);

}

void puerta() {
  if (orden_puerta == CERRADA && estado_puerta == ABIERTA) { //no hay que abrir, pero está abierta
     if ((millis() - tiempo) >4000) {  //No hay que abrir, ya ha pasado tiempo, estará cerrada
       estado_puerta = CERRADA;
       //tiempo = millis();
     }
     else if ((millis() - tiempo) >2000) { //No hay que abrir, está abierta, y hay que cerrarla
       Serial.println("Estoy cerrando");
       servoMotor.write(-90);
     }
  } else if (orden_puerta == 1) { //Hay que abrir
    estado_puerta = ABIERTA;
    tiempo = millis();
    Serial.println("Estoy abriendo");
    servoMotor.write(90);
    orden_puerta = CERRADA;
  } else { //No hay que abrir, y está cerrada, no se hace nada
  }
}
