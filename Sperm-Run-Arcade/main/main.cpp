#include <cmath>
#include <algorithm>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "LGFX_ESP32S3.h"  // Game-local ST7789/LovyanGFX configuration

static LGFX gfx;
static constexpr int W = 320, H = 240;
static constexpr int HUD_H = 25, FLOOR_Y = 228;
static constexpr int PLAYER_X = 72, PLAYER_R = 8;
static constexpr int PIPE_W = 25, GAP_H = 88;

// Buttons are active-low (button between GPIO and GND).
// These avoid the known TFT pins (9,10,11,12,13,46),
// INMP441 pins (4,5,6) and MAX98357A pins (15,16,17).
static constexpr gpio_num_t BTN_UP    = GPIO_NUM_1;
static constexpr gpio_num_t BTN_DOWN  = GPIO_NUM_2;
static constexpr gpio_num_t BTN_BOOST = GPIO_NUM_8;
static constexpr gpio_num_t BTN_PAUSE = GPIO_NUM_14;
static constexpr gpio_num_t BTN_MENU  = GPIO_NUM_18;

static constexpr uint16_t BLACK=0x0000, WHITE=0xFFFF, CYAN=0x07FF;
static constexpr uint16_t GREEN=0x07E0, YELLOW=0xFFE0, RED=0xF800;
static constexpr uint16_t GREY=0x7BEF, BLUE=0x001F;

enum State { TITLE, PLAYING, PAUSED, GAME_OVER };
struct Obstacle { float x; int gapY; bool scored; };
static State state = TITLE;
static Obstacle pipes[3];
static float playerY=120.0f, playerV=0.0f, anim=0.0f, bgOffset=0.0f;
static float speed=105.0f;
static int score=0, best=0, lives=3;
static bool prevPause=false, prevMenu=false;
static int64_t lastFrame=0;
static uint32_t rng=0xA5317B29;

static void configureButton(gpio_num_t pin) {
  gpio_config_t c{};
  c.pin_bit_mask = 1ULL << pin;
  c.mode = GPIO_MODE_INPUT;
  c.pull_up_en = GPIO_PULLUP_ENABLE;
  c.pull_down_en = GPIO_PULLDOWN_DISABLE;
  c.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&c);
}
static bool pressed(gpio_num_t p) { return gpio_get_level(p)==0; }
static uint32_t rnd() { rng^=rng<<13; rng^=rng>>17; rng^=rng<<5; return rng; }
static int newGap() { return 76 + (rnd()%90); }

static void resetGame() {
  playerY=120; playerV=0; anim=0; score=0; lives=3; speed=105;
  pipes[0]={360,95,false}; pipes[1]={555,155,false}; pipes[2]={750,115,false};
  state=PLAYING;
}
static void drawBackground(float dt) {
  bgOffset += 20*dt; if(bgOffset>=48) bgOffset-=48;
  for(int i=0;i<25;i++) {
    int x=(i*47-(int)bgOffset+W)%W;
    int y=HUD_H+8+(i*31)%190;
    gfx.drawPixel(x,y,(i%3==0)?BLUE:GREY);
    if(i%6==0) gfx.drawCircle((x+17)%W,y+5,3,BLUE);
  }
}
static void drawPlayer() {
  int x=PLAYER_X, y=(int)playerY;
  int px=x-PLAYER_R, py=y;
  for(int i=1;i<=8;i++) {
    float t=i/8.0f;
    int tx=x-PLAYER_R-(int)(t*31);
    int ty=y+(int)(sinf(anim*10-t*5.5f)*(2+t*5));
    gfx.drawLine(px,py,tx,ty,CYAN);
    gfx.drawLine(px,py+1,tx,ty+1,CYAN);
    px=tx; py=ty;
  }
  gfx.fillCircle(x,y,PLAYER_R,CYAN);
  gfx.fillCircle(x+3,y-2,2,BLACK);
  gfx.drawPixel(x-2,y-4,WHITE);
}
static void drawObstacle(const Obstacle &p) {
  int x=(int)p.x, top=p.gapY-GAP_H/2, bottom=p.gapY+GAP_H/2;
  if(top>HUD_H) {
    gfx.fillRect(x,HUD_H,PIPE_W,top-HUD_H,GREEN);
    gfx.drawRect(x,HUD_H,PIPE_W,top-HUD_H,WHITE);
    gfx.drawLine(x+4,HUD_H+5,x+4,top-4,YELLOW);
  }
  if(bottom<FLOOR_Y) {
    gfx.fillRect(x,bottom,PIPE_W,FLOOR_Y-bottom,GREEN);
    gfx.drawRect(x,bottom,PIPE_W,FLOOR_Y-bottom,WHITE);
    gfx.drawLine(x+4,bottom+4,x+4,FLOOR_Y-4,YELLOW);
  }
}
static bool hit(const Obstacle &p) {
  if(PLAYER_X+PLAYER_R<p.x || PLAYER_X-PLAYER_R>p.x+PIPE_W) return false;
  int top=p.gapY-GAP_H/2, bottom=p.gapY+GAP_H/2;
  return playerY-PLAYER_R<top || playerY+PLAYER_R>bottom;
}
static void drawHUD() {
  gfx.fillRect(0,0,W,HUD_H,BLACK);
  gfx.setTextColor(WHITE,BLACK); gfx.setTextSize(2);
  gfx.drawString("SPERM RUN",7,4);
  gfx.setTextSize(1);
  gfx.drawString("SCORE",175,3); gfx.drawNumber(score,175,13);
  gfx.drawString("BEST",238,3); gfx.drawNumber(best,238,13);
  gfx.drawString("LIVES",282,3); gfx.drawNumber(lives,282,13);
  gfx.drawLine(0,HUD_H,W,HUD_H,WHITE);
}
static void drawTitle() {
  gfx.fillScreen(BLACK);
  gfx.setTextColor(CYAN,BLACK); gfx.setTextSize(4);
  gfx.drawString("SPERM RUN",48,48);
  gfx.setTextColor(WHITE,BLACK); gfx.setTextSize(2);
  gfx.drawString("MICRO ARCADE",83,100);
  drawPlayer();
  gfx.setTextSize(1);
  gfx.drawString("HOLD UP / DOWN TO SWIM",76,162);
  gfx.drawString("BOOST: SPEED UP",101,180);
  gfx.drawString("MENU BUTTON: START",91,198);
  
}
static void drawGameOver() {
  gfx.fillScreen(BLACK); gfx.setTextColor(RED,BLACK); gfx.setTextSize(4);
  gfx.drawString("GAME OVER",53,57);
  gfx.setTextColor(WHITE,BLACK); gfx.setTextSize(2);
  gfx.drawString("SCORE:",95,115); gfx.drawNumber(score,185,115);
  gfx.drawString("BEST:",95,145); gfx.drawNumber(best,185,145);
  gfx.setTextSize(1); gfx.drawString("MENU BUTTON TO RESTART",75,195);
  
}
static void drawGame(float dt) {
  gfx.fillScreen(BLACK); drawBackground(dt);
  for(auto &p:pipes) drawObstacle(p);
  drawPlayer(); gfx.drawLine(0,FLOOR_Y,W,FLOOR_Y,CYAN); drawHUD();
  if(state==PAUSED) {
    gfx.fillRect(55,82,210,72,BLACK); gfx.drawRect(55,82,210,72,YELLOW);
    gfx.setTextColor(YELLOW,BLACK); gfx.setTextSize(3); gfx.drawString("PAUSED",94,94);
    gfx.setTextSize(1); gfx.setTextColor(WHITE,BLACK); gfx.drawString("PRESS PAUSE TO RESUME",82,130);
  }
  
}
static void updateGame(float dt) {
  bool up=pressed(BTN_UP), down=pressed(BTN_DOWN), boost=pressed(BTN_BOOST);
  float target=0;
  if(up && !down) target=-155;
  if(down && !up) target=155;
  playerV += (target-playerV)*std::min(1.0f,7.0f*dt);
  playerY += playerV*dt;
  anim += dt;
  speed=boost?170.0f:105.0f+std::min(score*1.2f,28.0f);

  if(playerY-PLAYER_R<HUD_H+1 || playerY+PLAYER_R>FLOOR_Y-1) {
    if(--lives<=0) { state=GAME_OVER; best=std::max(best,score); return; }
    playerY=120; playerV=0;
  }
  for(int i=0;i<3;i++) {
    pipes[i].x-=speed*dt;
    if(hit(pipes[i])) {
      if(--lives<=0) { state=GAME_OVER; best=std::max(best,score); return; }
      pipes[i].x=-100; playerY=120; playerV=0;
    }
    if(!pipes[i].scored && pipes[i].x+PIPE_W<PLAYER_X-PLAYER_R) {
      pipes[i].scored=true; score++; best=std::max(best,score);
    }
    if(pipes[i].x+PIPE_W<0) {
      float rightmost=std::max({pipes[0].x,pipes[1].x,pipes[2].x});
      pipes[i].x=rightmost+195; pipes[i].gapY=newGap(); pipes[i].scored=false;
    }
  }
}
extern "C" void app_main(void) {
  gfx.init(); gfx.setRotation(1); gfx.setColorDepth(16); gfx.fillScreen(BLACK);
  configureButton(BTN_UP); configureButton(BTN_DOWN); configureButton(BTN_BOOST);
  configureButton(BTN_PAUSE); configureButton(BTN_MENU);
  lastFrame=esp_timer_get_time();
  while(true) {
    int64_t now=esp_timer_get_time();
    float dt=(now-lastFrame)/1000000.0f; lastFrame=now;
    if(dt>0.04f) dt=0.04f;
    bool pauseNow=pressed(BTN_PAUSE), menuNow=pressed(BTN_MENU);
    bool pauseEdge=pauseNow&&!prevPause, menuEdge=menuNow&&!prevMenu;
    prevPause=pauseNow; prevMenu=menuNow;
    if(state==TITLE) {
      anim+=dt*5;
      if(menuEdge) resetGame(); else drawTitle();
    } else if(state==PLAYING) {
      if(pauseEdge) state=PAUSED; else updateGame(dt);
      if(state==PLAYING) drawGame(dt);
      else if(state==PAUSED) drawGame(dt);
      else drawGameOver();
    } else if(state==PAUSED) {
      if(pauseEdge) state=PLAYING;
      drawGame(dt);
    } else {
      if(menuEdge) resetGame(); else drawGameOver();
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}