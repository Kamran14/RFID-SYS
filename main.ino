#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd (7, 6, 5, 4, 3, 2); //Pins for LCD Screen

#define RST_PIN 9           // Configurable, see typical pin layout above
#define SS_PIN 10          // Configurable, see typical pin layout above
byte Kamran = 10000001100101001001101111101011000000110010100100110111110101;
int Shahzaib = 1000001110011111101010011101010110000011100111111010100111010101;
int Johnn = 1001001110001100100011110010010011100011001000111100;
int Janice = 1000101111000001001111110110010001011110000010011111101100;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// Number of known default keys (hard-coded)
// NOTE: Synchronize the NR_KNOWN_KEYS define with the defaultKeys[] array
#define NR_KNOWN_KEYS   8
// Known keys, see: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
  {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
  {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
  {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
  {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
  {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
  {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};
//int val;
/*
 * Initialize.
 */
void setup() {
  lcd.begin(16, 2);
  lcd.print("Scan a card");
  lcd.setCursor(0, 2);
  Serial.begin(9600);         // Initialize serial communications with the PC
  while (!Serial);            // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();         // Init MFRC522 card
  Serial.println(F("Try the most used default keys to print block 0 of a MIFARE PICC."));
}

/*
 * Helper routine to dump a byte array as hex values to Serial.
 */
unsigned long dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte x = 0; x < bufferSize; x++) {
    Serial.print(buffer[x] < 16 ? "0" : ""); //value cannot be bigger than 16 digits
    Serial.print(buffer[x], HEX);
  }

  unsigned long retval = 0;
  // when numbers comebackwards, change this to
  //  for (int i = 3; i >= 0; i--) {
  for (int i = 0; i < 4; i++) {
    retval = retval << 8;
    retval |= buffer[i];
  }
  return retval;
}

//Prints in Serial Monitor
//just incase you dont have a lcd or what not

/*unsigned long dump_byte_array1(byte *buffer, byte bufferSize) {
  for (byte u = 0; u < bufferSize; u++) {
    Serial.print(buffer[u] < 16 ? "0" : ""); //value cannot be bigger than 16 digits
    Serial.print(buffer[u], BIN);
  }
}*/
/*void dump_byte_array1(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i] < 16 ? "0" : "");
    //Serial.print(buffer[i], HEX);
  }
}*/

/*
 * Try using the PICC (the tag/card) with the given key to access block 0.
 * On success, it will show the key details, and dump the block data on Serial.
 *
 * @return true when the given key worked, false otherwise.
 */
boolean try_key(MFRC522::MIFARE_Key *key){
  boolean result = false;
  byte buffer[18];
  byte block = 0;
  byte status;

  // Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    // Serial.print(F("PCD_Authenticate() failed: "));
    // Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  // Read block
  byte byteCount = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
  if (status != MFRC522::STATUS_OK) {
    // Serial.print(F("MIFARE_Read() failed: "));
    // Serial.println(mfrc522.GetStatusCodeName(status));
  }
  else {
    // Successful read
    result = true;
    //Serial.print(F("Success with key:"));
    dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    // Dump block data
    Serial.print(F("Block ")); lcd.print(block); Serial.print(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
  }
  Serial.println();

  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  return result;
}

/*
 * Main loop.
 */
void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;
  //Serial.println(val);

  //else//{

  //lcd.print("You're card could not be found in our database");
  //}
  //  Serial.print(F("Card UID:"));

  //dump_byte_array1(mfrc522.uid.uidByte, mfrc522.uid.size);
  unsigned long valueOnCard = dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  //lcd.println(valueOnCard);
  Serial.println("");
//   Serial.println(valueOnCard);
  delay(1000);
  lcd.clear();
  lcd.print("Scan a card");
  lcd.setCursor(0, 2);
 // Serial.println(valueOnCard);
  lcd.print(valueOnCard);
   if (valueOnCard ==  2173999989) {
     Serial.println("Kam");
     lcd.print("Kam");
     delay(1000);
     lcd.clear();
   } else if (valueOnCard == 2208278997) {
     Serial.println("Shazio");
     lcd.print("Shazi");
     delay(1000);
     lcd.clear();
   }else if (valueOnCard == 2475462400){
     lcd.print("John");
     delay(1000);
     lcd.clear();
   }else if (valueOnCard == 1172348716){
     lcd.print("Janice");
     delay(1000);
     lcd.clear();
   }
}

