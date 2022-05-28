#include <RGBmatrixPanel.h>
#include <misakiUTF16.h>

#define CLK  8
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2

//まぶしすぎるので少し暗めのRGB
#define RGB4(r,g,b)  matrix.Color333(r,g,b)

//色の諸設定
#define BLACK   RGB4(0,0,0)
#define BLUE    RGB4(0,0,3)
#define RED     RGB4(3,0,0)
#define GREEN   RGB4(0,3,0)
#define CYAN    RGB4(0,3,3)
#define MAGENTA RGB4(3,0,3)
#define YELLOW  RGB4(3,3,0)
#define WHITE   RGB4(3,3,3)

//フレーム
#define FPS 12

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

int16_t n = 0;
byte buf[40][8];

//全角文字を描く
void drawMisakiZenkaku ( int16_t x, int16_t y, char * text, uint16_t color )  {
  n = 0;

  while (*text)
    text = getFontData(&buf[n++][0], text);

  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < n; j++) {
      for (byte k = 0; k < 8; k++) {
        if (bitRead(buf[j][i], 7 - k))matrix.drawPixel(x + j * 8 + k, y + i, color);
      }
    }
  }
}

//半角文字を描く
void drawMisakiHankaku ( int16_t x, int16_t y, char * text, uint16_t color )  {
  n = 0;

  while (*text)
    text = getFontData(&buf[n++][0], text);

  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < n; j++) {
      for (byte k = 0; k < 4; k++) {
        if (bitRead(buf[j][i], 7 - k))matrix.drawPixel(x + j * 4 + k, y + i, color);
      }
    }
  }
}

//全角文字のスクロール
void scrollMisakiZenkaku ( int16_t x, int16_t y, int16_t w, char * text, uint16_t color, uint32_t frame ) {
  n = 0;

  while (*text)
    text = getFontData(&buf[n++][0], text);

  int16_t scrollLoop = frame;


  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < n; j++) {
      for (byte k = 0; k < 8; k++) {
        if (x <= x + j * 8 + k + w - scrollLoop && x + j * 8 + k + w - scrollLoop < x + w && bitRead(buf[j][i], 7 - k))matrix.drawPixel(x + j * 8 + k + w - scrollLoop, y + i, color);
      }
    }
  }
}

//半角文字のスクロール
void scrollMisakiHankaku ( int16_t x, int16_t y, int16_t w, char * text, uint16_t color, uint32_t frame ) {
  n = 0;

  while (*text)
    text = getFontData(&buf[n++][0], text);

  int16_t scrollLoop = frame;


  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < n; j++) {
      for (byte k = 0; k < 4; k++) {
        if (x <= x + j * 4 + k + w - scrollLoop && x + j * 4 + k + w - scrollLoop < x + w && bitRead(buf[j][i], 7 - k))matrix.drawPixel(x + j * 4 + k + w - scrollLoop, y + i, color);
      }
    }
  }
}

//カラフルな枠
void colorfulFrame( uint16_t color1, uint16_t color2, uint32_t frame ){
  const uint8_t interval = 2;
  //上
  for(uint8_t i = 0; i < 32; i++){
    (frame / interval + i) % 2 ? matrix.drawPixel(i, 0, color1) :  matrix.drawPixel(i, 0, color2);
  }
  //下
  for(uint8_t i = 0; i < 32; i++){
    (frame / interval + i) % 2 ? matrix.drawPixel(i, 15, color2) :  matrix.drawPixel(i, 15, color1);
  }
  //左
  for(uint8_t i = 0; i < 16; i++){
    (frame / interval + i) % 2 ? matrix.drawPixel(0, i, color1) :  matrix.drawPixel(0, i, color2);
  }
  //右
  for(uint8_t i = 0; i < 16; i++){
    (frame / interval + i) % 2 ? matrix.drawPixel(31, i, color2) :  matrix.drawPixel(31, i, color1);
  }
}

//初期化
void initialize() {
  matrix.begin();//マトリクスの開始
  matrix.setRotation(0);
  matrix.fillScreen(BLACK);//黒で埋める
}

//HSVをRGBに変換
//一部改造 https://ja.wikipedia.org/wiki/HSV%E8%89%B2%E7%A9%BA%E9%96%93#%E3%82%BD%E3%83%95%E3%83%88%E3%82%A6%E3%82%A7%E3%82%A2%E3%81%A7%E3%81%AE%E5%A4%89%E6%8F%9B%E5%87%A6%E7%90%86_2
uint16_t hsv(float h, float s, float v) {
  float r = v;
  float g = v;
  float b = v;
  if (s > 0.0f) {
    h *= 6.0f;
    int16_t i = (int16_t) h;
    float f = h - (float) i;
    switch (i) {
      default:
      case 0:
        g *= 1 - s * (1 - f);
        b *= 1 - s;
        break;
      case 1:
        r *= 1 - s * f;
        b *= 1 - s;
        break;
      case 2:
        r *= 1 - s;
        b *= 1 - s * (1 - f);
        break;
      case 3:
        r *= 1 - s;
        g *= 1 - s * f;
        break;
      case 4:
        r *= 1 - s * (1 - f);
        g *= 1 - s;
        break;
      case 5:
        g *= 1 - s;
        b *= 1 - s * f;
        break;
    }
  }
  return RGB4(r * 4, g * 4, b * 4);
}

void setup() {
  initialize();
}

const bool arrow[14][18] PROGMEM = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, },
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
  { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
};

const bool apcLogo[14][30] PROGMEM = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
  { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, },
  { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, },
  { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, },
  { 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, },
  { 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
  { 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, },
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, },
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, },
  { 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, },
};

void loop() {
  matrix.fillScreen(BLACK);
  //入口
  for(uint16_t i = 0; i < 7 * FPS; i++) {
    drawMisakiZenkaku(8, 4, "入口", hsv((i % 20) * 0.05f, 1.0f, 1.0f));
    colorfulFrame(YELLOW, RED, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //APClogo
  for(uint16_t i = 0; i < 7 * FPS; i++) {
    for (uint8_t j = 0; j < 14; j++ ){
      for (uint8_t k = 0; k < 18; k++ ){
        if (pgm_read_byte(&(arrow[j][k])) == 1){
          matrix.drawPixel(k + 7, j + 1, hsv((i % 20) * 0.05f, 1.0f, 1.0f));
        }
      }
    }
    colorfulFrame(YELLOW, RED, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //物理部へようこそ
  for(uint16_t i = 0; i < 8 * (9 + 4); i++) {
    matrix.fillRect(1, 4, 30, 8, BLACK);//フレームを黒に塗る
    scrollMisakiZenkaku(1, 4, 30, "物理部へようこそ!", RGB4(3, 1, 0), i);
    colorfulFrame(CYAN, BLUE, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //Welcome to APC!
  for(uint16_t i = 0; i < 4 * (15 + 8); i++) {
    matrix.fillRect(1, 5, 30, 8, BLACK);//フレームを黒に塗る
    scrollMisakiHankaku(1, 5, 30, "Welcome to APC!", RGB4(3, 1, 0), i);
    colorfulFrame(CYAN, BLUE, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //APClogo
  for(uint16_t i = 0; i < 7 * FPS; i++) {
    for (uint8_t j = 0; j < 14; j++ ){
      for (uint8_t k = 0; k < 30; k++ ){
        if (pgm_read_byte(&(apcLogo[j][k])) == 1){
          matrix.drawPixel(k + 1, j + 1, hsv((i % 20) * 0.05f, 1.0f, 1.0f));
        }
      }
    }
    colorfulFrame(GREEN, YELLOW, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //プログラミング・電子工作
  for(uint16_t i = 0; i < 8 * (12 + 4); i++) {
    matrix.fillRect(1, 4, 30, 8, BLACK);//フレームを黒に塗る
    scrollMisakiZenkaku(1, 4, 30, "プログラミング・電子工作", RGB4(1, 3, 1), i);
    colorfulFrame(RED, MAGENTA, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
  matrix.fillScreen(BLACK);
  //浅野でいちばんたのしい「ものづくり」
  for(uint16_t i = 0; i < 8 * (18 + 4); i++) {
    matrix.fillRect(1, 4, 30, 8, BLACK);//フレームを黒に塗る
    scrollMisakiZenkaku(1, 4, 30, "浅野でいちばんたのしい「ものづくり」", RGB4(0, 1, 3), i);
    colorfulFrame(WHITE, YELLOW, i);
    delay(1000 / FPS);//フレームを変えるまで待つ
  }
}
