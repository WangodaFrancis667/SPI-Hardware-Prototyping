/*
 * DIY SPI LCD Interface (Hardware SPI)
 * * HARDWARE CONNECTIONS (Arduino Uno):
 * - 595 Pin 14 (Data/DS)   -> Arduino Pin 11 (MOSI)
 * - 595 Pin 11 (Clock/SH)  -> Arduino Pin 13 (SCK)
 * - 595 Pin 12 (Latch/ST)  -> Arduino Pin 10 (SS)
 * * 595 OUTPUT MAPPING:
 * - Q0 -> LCD RS
 * - Q1 -> LCD Enable
 * - Q4-Q7 -> LCD D4-D7
 */

#include <SPI.h>

// Define the Slave Select (Latch) pin
const int SLAVE_SELECT_PIN = 10;

// SPI settings: 1MHz is reliable on breadboard wiring, MSB First, Mode 0
const SPISettings SPI_SETTINGS(1000000, MSBFIRST, SPI_MODE0);

// Forward declarations
void send_nibble_spi(byte nibble, byte rs);
void lcd_send_data(byte data, byte rs);
void lcd_command(byte cmd);
void lcd_print(const char* str);
void lcd_setCursor(byte col, byte row);
void lcd_init();

void setup() {
  // 1. Configure Pins
  pinMode(SLAVE_SELECT_PIN, OUTPUT);
  
  // 2. Initialize SPI
  SPI.begin();

  // 3. Initialize LCD
  lcd_init();

  // 4. Test Output
  lcd_print("SPI Interface");
  lcd_setCursor(0, 1);
  lcd_print("By Francis");
}

void loop() {
  // Nothing to loop
}

// --- LOW LEVEL SPI FUNCTIONS ---

void lcd_send_data(byte data, byte rs) {
  /* * LOGIC:
   * We need to send 4 bits of data (nibble) + RS bit + Enable Pulse
   * Mapping: [D7 D6 D5 D4] [X] [X] [E] [RS]
   */
   
  byte highNibble = (data & 0xF0);        // Get top 4 bits
  byte lowNibble  = ((data << 4) & 0xF0); // Get bottom 4 bits & shift them up

  send_nibble_spi(highNibble, rs); // Send upper nibble
  send_nibble_spi(lowNibble, rs);  // Send lower nibble
}

void send_nibble_spi(byte nibble, byte rs) {
  byte packet = nibble;
  
  // Set RS bit (Bit 0) if sending text/data
  if (rs == 1) packet |= 0x01; 

  // Acquire SPI bus for this nibble transfer
  SPI.beginTransaction(SPI_SETTINGS);

  // 1. Set data with Enable LOW (data setup before rising edge)
  packet &= ~0x02;
  digitalWrite(SLAVE_SELECT_PIN, LOW);
  SPI.transfer(packet);
  digitalWrite(SLAVE_SELECT_PIN, HIGH);
  delayMicroseconds(2);
  
  // 2. Pulse Enable HIGH (rising edge, data already stable)
  packet |= 0x02;
  digitalWrite(SLAVE_SELECT_PIN, LOW);
  SPI.transfer(packet);
  digitalWrite(SLAVE_SELECT_PIN, HIGH);
  delayMicroseconds(2);

  // 3. Pulse Enable LOW (falling edge triggers the LCD to read)
  packet &= ~0x02;
  digitalWrite(SLAVE_SELECT_PIN, LOW);
  SPI.transfer(packet);
  digitalWrite(SLAVE_SELECT_PIN, HIGH);

  // Release SPI bus
  SPI.endTransaction();
  
  delayMicroseconds(50); // Wait for LCD to process
}

// --- STANDARD LCD FUNCTIONS ---

void lcd_command(byte cmd) {
  lcd_send_data(cmd, 0); // RS = 0 for Command
  if (cmd == 0x01 || cmd == 0x02) {
    delay(5);  // Clear/Home need 1.52ms min
  } else {
    delayMicroseconds(50); // Extra settle time after commands
  }
}

void lcd_print(const char* str) {
  while (*str) {
    lcd_send_data(*str++, 1); // RS = 1 for Data
  }
}

void lcd_setCursor(byte col, byte row) {
  byte offsets[] = {0x00, 0x40, 0x14, 0x54};
  lcd_command(0x80 | (col + offsets[row]));
}

void lcd_init() {
  delay(50); // Power up wait

  // Clear 595 outputs to a known state (all LOW, especially Enable)
  SPI.beginTransaction(SPI_SETTINGS);
  digitalWrite(SLAVE_SELECT_PIN, LOW);
  SPI.transfer(0x00);
  digitalWrite(SLAVE_SELECT_PIN, HIGH);
  SPI.endTransaction();
  delay(10); // Let LCD settle with all lines LOW

  // Force reset into 4-bit mode (Standard HD44780 Sequence)
  send_nibble_spi(0x30, 0); delay(5);
  send_nibble_spi(0x30, 0); delayMicroseconds(200);
  send_nibble_spi(0x30, 0); delayMicroseconds(200);
  
  send_nibble_spi(0x20, 0); // Set 4-bit mode interface
  delayMicroseconds(200);
  
  // Configure Options
  lcd_command(0x28); // Function: 4-bit, 2 Line, 5x8 Dots
  lcd_command(0x0C); // Display On, Cursor Off
  lcd_command(0x06); // Entry Mode: Auto Increment
  lcd_command(0x01); // Clear Display
}