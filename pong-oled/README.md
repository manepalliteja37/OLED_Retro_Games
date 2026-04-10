# 🏓 Pong Game on OLED (Arduino Nano)

A fully featured Pong game built using Arduino Nano and 0.96" SSD1306 OLED display.

---

## 🚀 Features

- 🎮 Pong gameplay with increasing difficulty
- ⭐ Difficulty levels (Easy → Extreme)
- 🔊 Sound effects (passive buzzer)
- 🎬 Startup animation (curtain + particles)
- 💤 Auto sleep mode
- 🎮 Pause system
- 📊 Score tracking
- 🔋 Battery indicator UI
- 🧠 Smooth UI using millis (no delay)

---

## 🧰 Hardware Used

- Arduino Nano
- SSD1306 OLED Display (128x64)
- 4 Push Buttons
- Passive Buzzer
- 100 ohm resistor
- Breadboard + wires


---

## 🔌 Wiring

| Component | Arduino Pin |
|----------|------------|
| OLED SDA | A4 |
| OLED SCL | A5 |
| Button LEFT | D5 |
| Button RIGHT | D4 |
| Button UP | D2 |
| Button DOWN | D3 |
| Buzzer | D6 |

---

## 🖼️ Screenshots

![circuit](pong-oled/Images/circuit.png)

---

## 🎥 Demo Links

- ▶ YouTube: https://your-youtube-link
- 🐦 X (Twitter): https://x.com/TejaMane37/status/2041576579902685540?s=20

---

## ⚙️ How to Run

1. Install Arduino IDE
2. Install libraries:
   - Adafruit GFX
   - Adafruit SSD1306
3. Upload `src/pong_game.ino` to Arduino Nano

---

## 🧠 Concepts Used

- Embedded Systems
- State Machines
- Non-blocking code (millis)
- Graphics rendering
- Game logic

---

## 🚀 Future Improvements

- EEPROM high score save
- Multiplayer mode
- AI opponent
- Better animations

---

## 👨‍💻 Author

Built by **Telugu Mad Thinker**
