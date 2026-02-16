#include <Arduino.h>
/*
 * ROBUST REAL HARDWARE SPI LCD (74HC595)
 * Stable version for Arduino + 74HC595 + 16x2 LCD
 */

#include <SPI.h>

// ---------------- PIN CONFIG ----------------
const int LATCH_PIN = 10;   // STCP (RCLK)

// ---------------- SPI SETTINGS ----------------
const SPISettings SPI_SETTINGS(500000, MSBFIRST, SPI_MODE0);

// ---------------- TIMING (REAL HARDWARE) ----------------
const int ENABLE_PULSE_US = 2;      // Min Enable pulse width (µs)
const int NIBBLE_DELAY_US = 100;    // Delay between high & low nibbles (µs)
const int CHAR_DELAY_US   = 50;     // Delay after full character (µs)
const int CMD_DELAY_MS    = 2;      // Delay after command (ms)
const int CLEAR_DELAY_MS  = 3;      // Delay for clear/home commands (ms)

// ---------------- LOW LEVEL SEND ----------------
// Sends 4-bit nibble via SPI to 74HC595 shift register
// Data layout: Q0=RS, Q1=Enable, Q2-Q3=unused, Q4-Q7=LCD Data (D4-D7)
void sendNibble(byte nibble, bool rs) {
  // Extract bits 4-7 (the actual data for D4-D7)
  byte packet = nibble & 0xF0;

  // Set RS bit (0=command, 1=data)
  if (rs) packet |= 0x01;

  // Step 1: Set data with EN=0 (setup phase)
  SPI.beginTransaction(SPI_SETTINGS);
  digitalWrite(LATCH_PIN, LOW);
  SPI.transfer(packet);  // Q4-Q7 have data, Q1=0 (EN disabled)
  digitalWrite(LATCH_PIN, HIGH);
  SPI.endTransaction();
  delayMicroseconds(10);

  // Step 2: Pulse EN high (enable the LCD to capture data)
  SPI.beginTransaction(SPI_SETTINGS);
  digitalWrite(LATCH_PIN, LOW);
  SPI.transfer(packet | 0x02);  // Set Q1 (Enable) high
  digitalWrite(LATCH_PIN, HIGH);
  SPI.endTransaction();
  delayMicroseconds(ENABLE_PULSE_US);

  // Step 3: Return EN to low (LCD latches data on this falling edge)
  SPI.beginTransaction(SPI_SETTINGS);
  digitalWrite(LATCH_PIN, LOW);
  SPI.transfer(packet);  // Q1 back to 0 (EN disabled) - DATA CAPTURED HERE
  digitalWrite(LATCH_PIN, HIGH);
  SPI.endTransaction();
  delayMicroseconds(NIBBLE_DELAY_US);
}

// Sends a full 8-bit byte as two 4-bit nibbles (HD44780 4-bit mode)
void sendByte(byte value, bool rs) {
  // High nibble: bits 7-4 stay in positions 7-4
  byte highNibble = value & 0xF0;
  // Low nibble: bits 3-0 shifted to positions 7-4
  byte lowNibble  = (value << 4) & 0xF0;
  
  sendNibble(highNibble, rs);
  delayMicroseconds(10);
  sendNibble(lowNibble, rs);
  delayMicroseconds(CHAR_DELAY_US);
}

void lcdCommand(byte cmd) {
  sendByte(cmd, false);
  if (cmd == 0x01 || cmd == 0x02) {
    delay(CLEAR_DELAY_MS);
  }
}

void lcdWrite(char c) {
  sendByte(c, true);
}

// ---------------- HIGH LEVEL ----------------
void lcdPrint(const char* str) {
  while (*str) lcdWrite(*str++);
}

void lcdSetCursor(byte col, byte row) {
  byte offsets[] = {0x00, 0x40};
  lcdCommand(0x80 | (col + offsets[row]));
}

// ---------------- LCD INIT ----------------
void lcdInit() {
  delay(50); // Power-up wait

  // Force 4-bit mode
  sendNibble(0x30, false); delay(5);
  sendNibble(0x30, false); delay(5);
  sendNibble(0x30, false); delay(1);
  sendNibble(0x20, false); delay(1);

  lcdCommand(0x28); // 4-bit, 2-line
  lcdCommand(0x0C); // Display ON
  lcdCommand(0x06); // Entry mode
  lcdCommand(0x01); // Clear
}

// ---------------- SETUP ----------------
void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  SPI.begin();

  lcdInit();

  lcdPrint("SPI Interface");
  lcdSetCursor(0, 1);
  lcdPrint("By Francis");
}

void loop() {}
