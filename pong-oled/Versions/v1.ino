#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Buttons
#define BTN_LEFT 5
#define BTN_RIGHT 4
#define BTN_UP 2
#define BTN_DOWN 3

#define BUZZER 6

// Game states
enum State {MENU, GAME, PAUSE, GAMEOVER, SOUND_MENU, HELP, DIFFICULTY, SLEEP};
State state = MENU;

// Menu
int menuIndex = 0;
const char* menuItems[] = {"Start", "Difficulty", "Sound", "Help", "Exit"};
bool soundOn = true;

// Ball
int ballX, ballY, dx, dy;

// Paddle
int paddleX = 50;
int paddleWidth = 25;

// Score
int score = 0;
int lastScore = 0;
int highScores[5] = {0};

// Speed
int speedDelay = 20;

// Sleep
unsigned long lastActivityTime = 0;
const unsigned long sleepTimeout = 20000;
// difficulty
int difficulty = 1; // 0=Easy,1=Medium,2=Hard,3=Extreme
// ================= SOUND CORE =================
void playTone(int freq, int duration){
  if(soundOn){
    noTone(BUZZER);               // stop previous sound
    tone(BUZZER, freq, duration);
  }
}

// ================= SOUND TYPES =================
void soundMove(){ playTone(700, 20); }
void soundSelect(){ playTone(1000, 40); }
void soundBack(){ playTone(500, 30); }

void soundWall(){ playTone(800, 40); }
void soundPaddle(){ playTone(1200, 40); }
void soundMiss(){ playTone(300, 200); }

// ================= SPEED =================
void updateSpeed(){

  int baseSpeed;

  if(difficulty == 0) baseSpeed = 25;      // Easy
  else if(difficulty == 1) baseSpeed = 15; // Medium
  else if(difficulty == 2) baseSpeed = 7; // Hard
  else baseSpeed = 2;                      // 🔥 Extreme

  int level = score / 5;

  speedDelay = baseSpeed - (level * 3);

  if(speedDelay < 1) speedDelay = 1; // ultra fast
}
//====startup======
void startupAnimation(){

  // ================= 🎭 CURTAIN OPEN =================
  for(int i = 0; i <= 64; i += 4){
    tone(BUZZER, 600 + i*5, 20);
    display.clearDisplay();

    // Left curtain
    display.fillRect(0, 0, 64 - i, 64, WHITE);

    // Right curtain
    display.fillRect(64 + i, 0, 64 - i, 64, WHITE);

    display.display();
    delay(40);
  }

  // ================= ⚪ BALL SHOWER =================
  for(int frame = 0; frame < 20; frame++){
    tone(BUZZER, random(800,1200), 10);
    display.clearDisplay();

    for(int i = 0; i < 15; i++){
      int x = random(0, 128);
      int y = random(0, 64);
      display.fillCircle(x, y, 1, WHITE); // small balls
    }

    display.display();
    delay(50);
  }

// ================= RANDOM LETTER SCATTER =================

String chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

for(int frame = 0; frame < 15; frame++){
  tone(BUZZER, 1200, 150);
  display.clearDisplay();
  display.setTextColor(WHITE);
  for(int i = 0; i < 20; i++){  // number of random letters

    int x = random(0, 120);
    int y = random(10, 55);

    char c = chars[random(0, chars.length())];

    display.setCursor(x, y);
    display.print(c);
  }

  display.display();
  delay(20);
}

// ================= CLEAR =================
display.clearDisplay();
display.display();
delay(50);

// ================= FINAL TEXT REVEAL =================

String finalText = "TELUGU MAD THINKER";

// center text

display.setTextSize(2);
display.setCursor(0, 20);
display.println(finalText);
display.display();

// ================= HOLD =================
delay(2000); // hold for 2 sec
// ================= CLEAR =================
display.clearDisplay();
display.display();

}
// ================= MENU =================
void drawMenu(){
  display.clearDisplay();

  display.setCursor(2,2);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("PONG GAME");

  display.drawLine(0,16,127,16,WHITE);

  int visibleItems = 4;
  int start = menuIndex - 1;

  if(start < 0) start = 0;
  if(start > 5 - visibleItems) start = 5 - visibleItems;

  for(int i=0;i<visibleItems;i++){
    int itemIndex = start + i;

    display.setCursor(20, 20 + i*12);

    if(itemIndex == menuIndex) display.print("> ");
    else display.print("  ");

    display.println(menuItems[itemIndex]);
  }

  display.display();
}

// ================= SOUND MENU =================
void drawSoundMenu(){
  display.clearDisplay();

  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("SOUND SETTINGS");

  display.drawLine(0,16,127,16,WHITE);

  display.setCursor(20,30);
  display.print("Sound: ");
  display.println(soundOn ? "ON" : "OFF");

  display.setCursor(10,50);
  display.println("UP: Back");

  display.display();

  if(digitalRead(BTN_LEFT)==LOW || digitalRead(BTN_RIGHT)==LOW){
    soundOn = !soundOn;
    soundSelect();
    delay(200);
  }

  if(digitalRead(BTN_UP)==LOW){
    soundBack();
    state = MENU;
    delay(200);
  }
}

// ================= HELP =================
void drawHelp(){
  display.clearDisplay();

  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("HELP");

  display.drawLine(0,16,127,16,WHITE);

  display.setCursor(0,20);
  display.println("LEFT/RIGHT: Move");

  display.setCursor(0,30);
  display.println("UP: Back");

  display.setCursor(0,40);
  display.println("DOWN: Menu Nav");

  display.display();

  if(digitalRead(BTN_UP)==LOW){
    soundBack();
    state = MENU;
    delay(200);
  }
}
//=======pause======
void drawPause(){

  display.clearDisplay();

  display.setCursor(30,20);
  display.setTextSize(2);
  display.println("PAUSED");

  display.setTextSize(1);
  display.setCursor(20,50);
  display.println("UP: Resume");

  display.display();

  if(digitalRead(BTN_UP)==LOW){
    soundSelect();
    state = GAME;
    delay(300);
  }
}

// ================= SLEEP =================
void drawSleep(){

  static bool screenOff = false;

  if(!screenOff){

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(40,20);
    display.print("Zz");

    display.setTextSize(1);
    display.setCursor(80,10);
    display.print("z");

    display.setCursor(20,50);
    display.print("Sleeping...");

    display.display();

    delay(2000);

    display.ssd1306_command(SSD1306_DISPLAYOFF);
    screenOff = true;
  }

  if(digitalRead(BTN_LEFT)==LOW ||
     digitalRead(BTN_RIGHT)==LOW ||
     digitalRead(BTN_UP)==LOW ||
     digitalRead(BTN_DOWN)==LOW){

    display.ssd1306_command(SSD1306_DISPLAYON);

    lastActivityTime = millis();
    state = MENU;
    screenOff = false;

    delay(300);
  }
}

// ================= GAME =================
void resetGame(){
  ballX = 64;
  ballY = 30;
  dx = 1;
  dy = 1;
  score = 0;
}
//======draw header=======
void drawHeader(){
  display.setCursor(0,0);
  display.print("PONG ");
  display.setCursor(30,0);

if(difficulty == 0){
  display.println("E");
}
else if(difficulty == 1){
  display.println("M");
}
else if(difficulty == 2){
  display.println("H");
}
else{
  display.println("X");
}
  display.setCursor(80,0);
  display.print(score);

  display.drawRect(100,0,16,8,WHITE);
  display.fillRect(102,2,10,4,WHITE);
}

void gameLoop(){
  // 🎮 Pause button
  if(digitalRead(BTN_UP)==LOW){
   soundBack();
   state = PAUSE;
   delay(300);
   return;
  }
  display.clearDisplay();

  drawHeader();

  display.drawLine(0,16,127,16,WHITE);
  display.drawRect(0,17,128,47,WHITE);

  if(digitalRead(BTN_LEFT)==LOW) paddleX -= 4;
  if(digitalRead(BTN_RIGHT)==LOW) paddleX += 4;

  if(paddleX < 0) paddleX = 0;
  if(paddleX > 128-paddleWidth) paddleX = 128-paddleWidth;

  display.fillRect(paddleX,60,paddleWidth,3,WHITE);
  display.fillCircle(ballX, ballY, 2, WHITE);

  ballX += dx;
  ballY += dy;

  if(ballX<=2 || ballX>=126){
    dx = -dx;
    soundWall();
  }

  if(ballY<=18){
    dy = -dy;
    soundWall();
  }

  if(ballY>=58 && ballX>=paddleX && ballX<=paddleX+paddleWidth){
    dy = -dy;
    score++;
    soundPaddle();
  }
if(ballY > 63){

  soundMiss();       // play sound

  lastScore = score;

  // save high score
  for(int i=0;i<5;i++){
    if(score > highScores[i]){
      for(int j=4;j>i;j--) highScores[j] = highScores[j-1];
      highScores[i] = score;
      break;
    }
  }

  state = GAMEOVER;
}

  updateSpeed();

  display.display();
  delay(speedDelay);
}

//=====difficulty=====
void drawDifficulty(){

  display.clearDisplay();

  display.setCursor(0,0);
  display.println("DIFFICULTY");

  display.drawLine(0,16,127,16,WHITE);

  display.setCursor(20,30);

if(difficulty == 0) display.println("Easy");
else if(difficulty == 1) display.println("Medium");
else if(difficulty == 2) display.println("Hard");
else display.println("Extreme");

  display.setCursor(10,50);
  display.println("UP: Back");

  display.display();

  if(digitalRead(BTN_LEFT)==LOW){
    difficulty--;
    if(difficulty < 0) difficulty = 3;
    soundMove();
    delay(200);
  }

  if(digitalRead(BTN_RIGHT)==LOW){
    difficulty++;
    if(difficulty > 3) difficulty = 0;
    soundMove();
    delay(200);
  }

  if(digitalRead(BTN_UP)==LOW){
    soundBack();
    state = MENU;
    delay(200);
  }

}
// ================= GAME OVER =================
void drawGameOver(){
  display.clearDisplay();

  display.setCursor(0,0);
  display.println("GAME OVER");

  display.setCursor(10,20);
  display.print("Score: ");
  display.println(lastScore);

  display.setCursor(10,30);
  display.println("High Scores:");

  for(int i=0;i<3;i++){
    display.setCursor(90,30 + i*8);
    display.println(highScores[i]);
  }

  display.display();

  if(digitalRead(BTN_UP)==LOW){
    state = MENU;
    delay(300);
  }
}

// ================= SETUP =================
void setup(){
  lastActivityTime = millis();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  startupAnimation();  // animation call
}

// ================= LOOP =================
void loop(){

  if(digitalRead(BTN_LEFT)==LOW ||
     digitalRead(BTN_RIGHT)==LOW ||
     digitalRead(BTN_UP)==LOW ||
     digitalRead(BTN_DOWN)==LOW){
    lastActivityTime = millis();
  }

  if(millis() - lastActivityTime > sleepTimeout && state != SLEEP){
    state = SLEEP;
  }

  if(state == MENU){

    drawMenu();

    if(digitalRead(BTN_DOWN)==LOW){
      menuIndex = (menuIndex+1)%5;
      soundMove();
      delay(200);
    }

    if(digitalRead(BTN_UP)==LOW){
      menuIndex = (menuIndex-1+5)%5;
      soundMove();
      delay(200);
    }

    if(digitalRead(BTN_RIGHT)==LOW){
      soundSelect();

        if(menuIndex==0){ resetGame(); state = GAME; }
        else if(menuIndex==1){ state = DIFFICULTY; }
        else if(menuIndex==2){ state = SOUND_MENU; }
        else if(menuIndex==3){ state = HELP; }
        else if(menuIndex==4){ state = SLEEP; }

      delay(300);
    }
  }

  else if(state == SOUND_MENU) drawSoundMenu();
  else if(state == HELP) drawHelp();
  else if(state == GAME) gameLoop();
  else if(state == PAUSE) drawPause(); 
  else if(state == SLEEP) drawSleep();
  else if(state == DIFFICULTY) drawDifficulty();
  else if(state == GAMEOVER) drawGameOver();
  
}