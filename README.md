# SPI LCD Hardware Prototyping 🔧

A proof-of-concept DIY project demonstrating how to control an HD44780 LCD display using the Serial Peripheral Interface (SPI) protocol on an Arduino Uno microcontroller.

## 📋 Table of Contents
- [Overview](#overview)
- [Key Features](#key-features)
- [Hardware Setup](#hardware-setup)
- [How It Works](#how-it-works)
- [Circuit Diagram](#circuit-diagram)
- [Visual Guide](#visual-guide)
- [Project Structure](#project-structure)
- [Code Architecture](#code-architecture)
- [Getting Started](#getting-started)
- [Technical Details](#technical-details)
- [Future Improvements](#future-improvements)

---

## 📝 Overview

This project serves as an educational prototype for interfacing with an HD44780 LCD display through the Serial Peripheral Interface (SPI) protocol rather than the traditional parallel interface. 

**What makes this unique?**
- Uses SPI (3 wires: MOSI, MISO, SCK) instead of 8-10 parallel wires
- Demonstrates low-level hardware communication
- Completely DIY implementation without existing LCD libraries
- Optimized timing specifications for reliable operation

**Target Hardware**: Arduino Uno (ATMega328P)

---

## ✨ Key Features

- ✅ **SPI-Based Communication**: Sends data to LCD display via SPI protocol at 500kHz
- ✅ **4-Bit Mode Operation**: Efficient communication using 4-bit data transfer
- ✅ **HD44780 Compatible**: Works with standard 16x2 character LCD displays
- ✅ **Custom Timing Control**: Precise microsecond-level timing for reliable operation
- ✅ **PlatformIO Compatible**: Fully configured for the PlatformIO build system
- ✅ **Well-Documented Code**: Clear comments explaining hardware protocol details

---

## 🔌 Hardware Setup

### Required Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| Arduino Uno | 1 | Main microcontroller |
| HD44780 LCD Display (16x2) | 1 | Output display |
| 8-bit Shift Register (74HC595) | 1 | Interface between SPI and LCD |
| 10kΩ Potentiometer | 1 | LCD contrast adjustment |
| 10kΩ Resistor | 1 | Pull-up for contrast |
| Jumper Wires | Multiple | Connections |
| 5V Power Supply | 1 | Power for Arduino and LCD |

### Pin Connections

| Arduino Pin | Function | Connected To |
|-------------|----------|--------------|
| Pin 10 (SS) | Slave Select | 74HC595 Latch |
| Pin 11 (MOSI) | SPI Data Out | 74HC595 Data |
| Pin 13 (SCK) | SPI Clock | 74HC595 Clock |
| GND | Ground | Common ground |
| 5V | Power | 74HC595 Power |

---

## 🧠 How It Works

### The Big Picture

Instead of connecting the LCD directly to your microcontroller with 8+ wires, this project uses a **shift register** (74HC595) to reduce the number of wires needed. Here's the flow:

```
Arduino Uno (SPI) 
    ↓
74HC595 Shift Register 
    ↓
HD44780 LCD Display
```

### Step-by-Step Process

1. **Preparation**: The Arduino prepares a byte with control signals (RS for Register Select, Enable pulse)
2. **Serial Transmission**: Data is sent via SPI one bit at a time
3. **Shift Register**: The 74HC595 captures 8 bits from the SPI line
4. **Latch Signal**: A pulse on the SS (Slave Select) pin latches the data
5. **LCD Response**: The LCD interprets the signals and performs the requested action

### Communication Protocol

The project uses **4-bit mode**, meaning:
- Data sent in two 4-bit chunks (high nibble, then low nibble)
- Control bits included in the same byte:
  - **Bit 0**: RS (Register Select) - Command (0) or Data (1)
  - **Bit 1**: Enable - Pulse signal for LCD
  - **Bits 4-7**: Data lines (D4-D7)

---

## 📐 Circuit Diagram

![Circuit Schematic](public/images/schematic.png)

**Key Connection Points:**
- The 74HC595 shift register acts as a bridge between the Arduino's SPI and the LCD
- Pin 12 (MISO) is not used in this configuration
- Potentiometer adjusts LCD contrast for better readability

---

## 🎨 Visual Guide

![Hardware Setup](public/images/visual.png)

This shows the physical layout and connections of the complete hardware setup.

---

## 📁 Project Structure

```
SPI-Hardware-Prototyping/
├── src/
│   └── main.cpp              # Main sketch with all LCD functions
├── include/
│   └── README                # Header files location
├── lib/
│   └── README                # External libraries
├── public/
│   └── images/
│       ├── schematic.png     # Circuit diagram
│       └── visual.png        # Visual hardware setup
├── platformio.ini            # PlatformIO configuration
├── Makefile                  # Build configuration
└── README.md                 # This file
```

---

## 🏗️ Code Architecture

### Main Functions

#### **`lcd_send_nibble(byte nibble, byte rs)`**
Sends a 4-bit nibble to the LCD with proper SPI signaling.
- Prepares the data packet with RS and Enable bits
- Toggles enable signal to latch data into LCD
- Handles 3-step handshake with LCD

#### **`lcd_send_byte(byte value, byte rs)`**
Sends a full 8-bit byte by sending two nibbles (high then low).
- Splits byte into high and low nibbles
- Calls `lcd_send_nibble()` twice
- Applies appropriate timing delays

#### **`lcd_command(byte cmd)`**
Sends a command to the LCD (RS=0).
- Examples: Clear display (0x01), Home (0x02), Function Set (0x28)
- Special timing for home and clear commands

#### **`lcd_print(const char* str)`**
Writes text to the LCD (RS=1).
- Iterates through each character
- Sends each character as data (RS=1)

#### **`lcd_setCursor(byte col, byte row)`**
Positions the cursor at specified row and column.
- Handles multiple rows with correct DDRAM addressing
- Row offsets: [0x00, 0x40, 0x14, 0x54]

#### **`lcd_init()`**
Initializes the LCD display.
- Power-up delay for stability
- 3-step reset sequence for 4-bit mode
- Configures display settings (2-line, 5x8 dots)
- Clears display and sets entry mode

### Timing Constants

```cpp
TIME_PULSE_WIDTH = 1 µs    // Enable pulse duration
TIME_CHAR_EXEC = 50 µs     // Character execution time
TIME_CMD_EXEC = 50 µs      // Command execution time
TIME_HOME_EXEC = 2000 µs   // Home command delay
```

These timings ensure the LCD has enough time to process each instruction.

---

## 🚀 Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- Arduino Uno board
- USB cable for uploading
- All hardware components listed above

### Installation Steps

1. **Clone or Download the Project**
   ```bash
   git clone <repository-url>
   cd SPI-Hardware-Prototyping
   ```

2. **Open in VS Code with PlatformIO**
   - Open the project folder
   - PlatformIO should detect the `platformio.ini` file

3. **Connect Hardware**
   - Follow the pin connections table above
   - Ensure all power and ground connections are secure

4. **Upload the Code**
   ```bash
   platformio run --target upload
   ```
   Or use the VS Code PlatformIO button

5. **Verify**
   - You should see "Welcome home" on the first line
   - "By Frank" should appear on the second line

### Customizing the Display

Edit the `setup()` function in `main.cpp`:

```cpp
void setup() {
  // ... initialization code ...
  
  lcd_print("Your text here");     // Change first line
  lcd_setCursor(0, 1);               // Move to row 2
  lcd_print("Second line");          // Change second line
}
```

---

## 🔬 Technical Details

### HD44780 LCD Command Set (Quick Reference)

| Command | Hex | Function |
|---------|-----|----------|
| Clear Display | 0x01 | Clears all text |
| Home | 0x02 | Returns cursor to top-left |
| Entry Mode | 0x06 | Sets cursor movement direction |
| Display ON/OFF | 0x0C | Turns display on, cursor off |
| Function Set | 0x28 | 4-bit mode, 2 lines, 5x8 font |
| Set DDRAM Address | 0x80+ | Positions the cursor |

### Why SPI Instead of Parallel?

**Parallel (Traditional)**
- Requires 8-10 wires
- Faster data transfer
- Uses more pins

**SPI (This Project)**
- Requires only 3-4 wires (MOSI, SCK, SS, GND)
- Slightly slower but sufficient for LCD speeds
- More flexible for multiple devices
- Better for Arduino Uno (limited pins)

### SPI Settings Used

```cpp
SPISettings SPI_SETTINGS(500000, MSBFIRST, SPI_MODE0);
```

- **Frequency**: 500 kHz (fast enough for LCD)
- **Bit Order**: MSB First (most significant bit first)
- **Clock Mode**: SPI_MODE0 (clock low at idle, data captured on rising edge)

---

## 🎯 Future Improvements

This project can be extended in several ways:

### Immediate Enhancements
- [ ] Add `lcd_clear()` function for easy display clearing
- [ ] Create `lcd_backlight()` function to control backlight
- [ ] Add scrolling text support
- [ ] Implement custom character creation

### Advanced Features
- [ ] Add button input (read analog inputs, display menu)
- [ ] Build a temperature sensor interface
- [ ] Create a clock display
- [ ] Implement EEPROM storage for messages

### Code Improvements
- [ ] Convert to a reusable library
- [ ] Add error handling
- [ ] Create unit tests
- [ ] Document timing diagrams

### Hardware Expansion
- [ ] Multi-line displays (20x4, 40x4)
- [ ] Multiple shift registers for more outputs
- [ ] Add I2C interface instead of SPI
- [ ] Create PCB design

---

## 📚 References & Resources

### Documentation
- [Arduino SPI Library Reference](https://www.arduino.cc/reference/en/language/functions/communication/spi/)
- [HD44780 LCD Datasheet](https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller)
- [74HC595 Shift Register Reference](https://en.wikipedia.org/wiki/74HC595)

### Useful Tools
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Arduino Official IDE](https://www.arduino.cc/en/software)
- [Online Circuit Simulator](https://www.tinkercad.com/)

---

## 📄 License

This is a proof-of-concept project shared for educational purposes.

---

## 👤 Author

**Frank**  
A DIY electronics enthusiast exploring low-level hardware communication protocols.

---

## 💡 Notes

- This is a **work in progress** and a learning tool to understand SPI communication
- The code prioritizes clarity over optimization for educational value
- Always test timing specifications on your specific hardware, as component variations may exist
- Consider using pull-up resistors if communication becomes unreliable

---

**Last Updated**: February 2026  
**Status**: POC / Experimental
