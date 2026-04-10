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

// States
enum State {MENU, GAME, PAUSE, GAMEOVER, SOUND_MENU, HELP, DIFFICULTY, SLEEP};
State state = MENU;

// Menu
int menuIndex = 0;
const char* menuItems[] = {"Start","Difficulty","Sound","Help","Exit"};
const int totalItems = 5;

// Game
int ballX=64, ballY=30, dx=1, dy=1;
int paddleX=50, paddleWidth=25;
int score=0, lastScore=0;

int difficulty=1;
bool soundOn=true;

// Timing
unsigned long lastBtnTime=0;
unsigned long lastFrame=0;
int speedDelay=20;

// ================= BUTTON =================
bool pressed(int pin){
  if(digitalRead(pin)==LOW && millis()-lastBtnTime>150){
    lastBtnTime=millis();
    return true;
  }
  return false;
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
  int base;
  if(difficulty==0) base=30;
  else if(difficulty==1) base=20;
  else if(difficulty==2) base=8;
  else base=2;

  speedDelay = base - (score/5)*2;
  if(speedDelay<2) speedDelay=2;
}

// ================= STARTUP =================
void startupAnimation(){

  // curtain
  for(int i=0;i<=64;i+=4){
    tone(BUZZER, 600 + i*5, 20);
    display.clearDisplay();
    display.fillRect(0,0,64-i,64,WHITE);
    display.fillRect(64+i,0,64-i,64,WHITE);
    display.display();
    delay(30);
  }

  // particles
  for(int f=0;f<15;f++){
    tone(BUZZER, random(800,1200), 10);
    display.clearDisplay();
    for(int i=0;i<20;i++){
      display.fillCircle(random(0,128),random(0,64),1,WHITE);
    }
    display.display();
    delay(40);
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
  delay(30);
}

  // text
  display.clearDisplay();
  display.setCursor(10,20);
  display.setTextColor(WHITE);
  display.println("TELUGU MAD");
  display.setCursor(20,35);
  display.println("THINKER");
  display.display();
  delay(1500);

  display.clearDisplay();
}

// ================= HEADER =================
void drawHeader(){
  display.setCursor(0,0);
  display.print("PONG ");

  display.setCursor(35,0);
  if(difficulty==0) display.print("E");
  else if(difficulty==1) display.print("M");
  else if(difficulty==2) display.print("H");
  else display.print("X");

  display.setCursor(80,0);
  display.print(score);

  // battery icon
  display.drawRect(100,0,16,8,WHITE);
  display.fillRect(102,2,10,4,WHITE);
}

// ================= MENU =================
void drawMenu(){
  display.clearDisplay();

  display.setCursor(0,0);
  display.println("PONG GAME");

  display.drawLine(0,16,127,16,WHITE);

  int visible=4;
  int start = menuIndex-1;
  if(start<0) start=0;
  if(start>totalItems-visible) start=totalItems-visible;

  for(int i=0;i<visible;i++){
    int idx=start+i;
    display.setCursor(20,20+i*10);
    if(idx==menuIndex) display.print("> ");
    else display.print("  ");
    display.println(menuItems[idx]);
  }

  display.display();
}

// ================= GAME =================
void resetGame(){
  ballX=64; ballY=30; dx=1; dy=1; score=0;
}
// game loop
void gameLoop(){

  if(millis()-lastFrame < speedDelay) return;
  lastFrame = millis();

  display.clearDisplay();

  drawHeader();

  // boundary
  display.drawLine(0,16,127,16,WHITE);
  display.drawRect(0,17,128,47,WHITE);

  // paddle
  if(digitalRead(BTN_LEFT)==LOW) paddleX-=4;
  if(digitalRead(BTN_RIGHT)==LOW) paddleX+=4;

  if(paddleX<0) paddleX=0;
  if(paddleX>128-paddleWidth) paddleX=128-paddleWidth;

  display.fillRect(paddleX,60,paddleWidth,3,WHITE);

  // ball
  display.fillCircle(ballX,ballY,2,WHITE);

  ballX+=dx;
  ballY+=dy;

  if(ballX<=2 || ballX>=126){ dx=-dx; playTone(800,20); }
  if(ballY<=18){ dy=-dy; playTone(900,20); }

  if(ballY>=58 && ballX>=paddleX && ballX<=paddleX+paddleWidth){
    dy=-dy; score++; playTone(1200,20);
  }

  if(ballY>63){
    playTone(300,200);
    lastScore=score;
    state=GAMEOVER;
  }

  updateSpeed();

  display.display();
}

// ================= OTHER SCREENS =================
void drawPause(){
  display.clearDisplay();
  display.setCursor(30,20);
  display.println("PAUSED");
  display.setCursor(20,50);
  display.println("UP Resume");
  display.display();

  if(pressed(BTN_UP)) state=GAME;
}

void drawGameOver(){
  display.clearDisplay();
  display.setCursor(20,20);
  display.println("GAME OVER");
  display.setCursor(20,35);
  display.print("Score:");
  display.print(lastScore);
  display.display();

  if(pressed(BTN_UP)) state=MENU;
}
// difficulty
void drawDifficulty(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("DIFFICULTY");

  display.setCursor(20,30);
  if(difficulty==0) display.println("Easy");
  else if(difficulty==1) display.println("Medium");
  else if(difficulty==2) display.println("Hard");
  else display.println("Extreme");

  display.display();

  if(pressed(BTN_LEFT)){ difficulty--; if(difficulty<0)difficulty=3; }
  if(pressed(BTN_RIGHT)){ difficulty++; if(difficulty>3)difficulty=0; }
  if(pressed(BTN_UP)) state=MENU;
}
//sound
void drawSound(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("SOUND");

  display.setCursor(20,30);
  display.println(soundOn?"ON":"OFF");

  display.display();

  if(pressed(BTN_LEFT)||pressed(BTN_RIGHT)) soundOn=!soundOn;
  if(pressed(BTN_UP)) state=MENU;
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

// ================= HELP =================
void drawHelp(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("HELP");

  display.setCursor(0,20);
  display.println("LEFT/RIGHT Move");
  display.setCursor(0,30);
  display.println("UP Pause");

  display.display();

  if(pressed(BTN_UP)) state=MENU;
}

// ================= SETUP =================
void setup(){
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);

  pinMode(BTN_LEFT,INPUT_PULLUP);
  pinMode(BTN_RIGHT,INPUT_PULLUP);
  pinMode(BTN_UP,INPUT_PULLUP);
  pinMode(BTN_DOWN,INPUT_PULLUP);
  pinMode(BUZZER,OUTPUT);

  randomSeed(analogRead(0));

  startupAnimation(); // 🔥 kept
}

// ================= LOOP =================
void loop(){

  if(state==MENU){

    drawMenu();

    if(pressed(BTN_DOWN)) menuIndex=(menuIndex+1)%totalItems;
    if(pressed(BTN_UP)) menuIndex=(menuIndex-1+totalItems)%totalItems;

    if(pressed(BTN_RIGHT)){
      if(menuIndex==0){ resetGame(); state=GAME; }
      else if(menuIndex==1) state=DIFFICULTY;
      else if(menuIndex==2) state=SOUND_MENU;
      else if(menuIndex==3) state=HELP;
      else if(menuIndex==4) state=SLEEP;
    }
  }

  else if(state==GAME){
    if(pressed(BTN_UP)) state=PAUSE;
    gameLoop();
  }
  else if(state==PAUSE) drawPause();
  else if(state==GAMEOVER) drawGameOver();
  else if(state==DIFFICULTY) drawDifficulty();
  else if(state==SOUND_MENU) drawSound();
  else if(state==HELP) drawHelp();
}