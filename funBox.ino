#include <stdint.h>
#include <LCD.h>
#include <SD.h>
#include <SPI.h>
#include <Streaming.h>
#include <XPT2046.h>
#include <Touch.h>

#include "LCD.h"

#define MAX_BMP         10                      // bmp file num
#define FILENAME_LEN    20                      // max file name length

const int PIN_SD_CS = 5;                        // pin of sd card

const int __Gnbmp_height = 320;                 // bmp hight
const int __Gnbmp_width  = 240;                 // bmp width

unsigned char __Gnbmp_image_offset  = 0;        // offset

int __Gnfile_num = 4;                           // num of file

char __Gsbmp_files[4][FILENAME_LEN] =           // add file name here
{
  "flower.bmp",
  "tiger.bmp",
  "tree.bmp",
  "RedRose.bmp",
};

File bmpFile;

void setup()
{
  Serial.begin(9600);
  //  __SD_CS_DISABLE();
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);
  Sd2Card card;
  card.init(SPI_FULL_SPEED, PIN_SD_CS);
  if (!SD.begin(PIN_SD_CS))  {
    Serial.println("failed!");
    while (1);                              // init fail, die here
  }


  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  Tft.lcd_init();
  Tp.tp_init();
  Tp.tp_adjust();
  Tp.tp_dialog();

}


void loop()
{
  int p = 0;
  Tft.lcd_clear_screen(WHITE);
  choseGameMenu();
  while (true) {
    int chose = 0;
    int x;
    int y;
    int c = 0;
    int touch = true;
    touch = Xpt.xpt2046_twice_read_xy(&x, &y);
    if (!touch) {
      c = checkChosenGame(x, y);
      switch (c) {
        case 1:
          ticTacToeGame();
          break;
        case 2:
          Tft.lcd_clear_screen(BLACK);
          paint();
          break;
        case 3:
          for (unsigned char i = 0; i < __Gnfile_num; i++) {
            bmpFile = SD.open(__Gsbmp_files[i]);
            if (! bmpFile) {
              Serial.println("didnt find image");
              while (1);
            }

            if (! bmpReadHeader(bmpFile)) {
              Serial.println("bad bmp");
              return;
            }

            bmpdraw(bmpFile, 0, 0);
            bmpFile.close();
            delay(1000);
            delay(1000);
            delay(1000);
            delay(1000);
          }
          break;
      }
    }
  }
}

void choseGameMenu()
{
  Tft.lcd_clear_screen(WHITE);
  Tft.lcd_fill_rect(50, 50, 150, 60, RED);
  Tft.lcd_display_string(105, 75, (const uint8_t *)"Game",
                         16, WHITE);

  Tft.lcd_fill_rect(50, 200, 150, 60, BLUE);
  Tft.lcd_display_string(105, 225, (const uint8_t *)"Paint",
                         16, WHITE);
}

int checkChosenGame(int x, int y)
{
  if (x >= 450 && x <= 1550 && y >= 1350 && y <= 1650) {
    return 1;
  }
  if (x >= 450 && x <= 1550 && y >= 500 && y <= 850) {
    return 2;
  }
  // add menu point
  return 3;
  return 0;
}

void drawTicTacToeField()
{
  Tft.lcd_fill_rect(70, 40, 15, 240, WHITE);
  Tft.lcd_fill_rect(150, 40, 15, 240, WHITE);
  Tft.lcd_fill_rect(15, 95, 190, 15, WHITE);
  Tft.lcd_fill_rect(15, 210, 190, 15, WHITE);
}

void checkTicTacToeGame()
{
  int board[3][3] = {
    {5, 6 , 7},
    {8, 9, 10},
    {13, 12 , 11}
  };

  uint16_t xp = 0;
  uint16_t yp = 0;
  int posX = 0;
  int posY = 0;
  int sleep = 1;
  int player = 1;
  while (true) {
    sleep = Xpt.xpt2046_twice_read_xy(&xp, &yp);
    if (!sleep) {
      posX = checkPosX(xp);
      posY = checkPosY(yp);
      if ((posX != 0) && (posY != 0)) {
        board[posY - 1][posX - 1] = player;
        drawFigure(player, posX, posY);
        if (isWin(board)) {
          Tft.lcd_clear_screen(WHITE);
          if (player == 1) {
            Tft.lcd_display_string(60, 120, (const uint8_t *)"CROSS WON", FONT_1608, RED);
          } else {
            Tft.lcd_display_string(60, 120, (const uint8_t *)"ZERO WON", FONT_1608, RED);
          }
          delay(2000);
          return 0;
        }
        if (player == 1) {
          player = 2;
        } else {
          player = 1;
        }
      }
    }
  }
}

int isWin(int board[3][3])
{
  for (int  i = 0; i < 3; i++) {
    bool flag = true;
    for (int j = 1; j < 3; j++) {
      if (board[i][0] != board[i][j]) {
        flag = false;
        break;
      }
    }
    if (flag == true) {
      return true;
    }
  }

  for (int  i = 0; i < 3; i++) {
    bool flag = true;
    for (int j = 1; j < 3; j++) {
      if (board[0][i] != board[j][i]) {
        flag = false;
        break;
      }
    }
    if (flag == true) {
      return true;
    }
  }

  if (board[0][0] ==  board[1][1] && board[1][1] == board[2][2]) {
    return true;
  }
  if (board[0][2] ==  board[1][1] && board[1][1] == board[2][1]) {
    return true;
  }
  return false;
}

void drawFigure(int player, int x, int y)
{
  x = x - 1;
  int fy = y;
  y = y * 60;
  if (fy == 2) {
    y += 40;
  }
  if (fy == 3) {
    y += 72;
  }
  if (player == 2) {
    Tft.lcd_draw_circle(x * 80 + 40, y, 25, RED);
  } else {
    Tft.lcd_draw_line(x * 80 + 25, y, x * 80 + 40, y + 25 , BLUE);
    Tft.lcd_draw_line(x * 80 + 40, y, x * 80 + 25, y + 25, BLUE);
  }
}

int checkPosX(uint16_t x)
{
  if (x <= 1850 && x >= 1450) {
    return 1;
  }

  if (x <= 1250 && x >= 800) {
    return 2;
  }

  if (x <= 650 && x >= 200) {
    return 3;
  }
  return 0;
}

int checkPosY(uint16_t y)
{
  if (y <= 1850 && y >= 1450) {
    return 1;
  }

  if (y <= 1250 && y >= 800) {
    return 2;
  }

  if (y <= 650 && y >= 200) {
    return 3;
  }
  return 0;
}

void ticTacToeGame()
{
  Tft.lcd_clear_screen(BLACK);
  drawTicTacToeField();
  checkTicTacToeGame();
}

void paint()
{
  while (true) {
    Tp.tp_draw_board();
  }
}


#define BUFFPIXEL       60                      // must be a divisor of 240 
#define BUFFPIXEL_X3    180                     // BUFFPIXELx3

void bmpdraw(File f, int x, int y)
{
  bmpFile.seek(__Gnbmp_image_offset);

  uint32_t time = millis();

  uint8_t sdbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

  for (int i = 0; i < __Gnbmp_height; i++) {
    for (int j = 0; j < (240 / BUFFPIXEL); j++) {
      bmpFile.read(sdbuffer, BUFFPIXEL_X3);

      uint8_t buffidx = 0;
      int offset_x = j * BUFFPIXEL;
      unsigned int __color[BUFFPIXEL];

      for (int k = 0; k < BUFFPIXEL; k++) {
        __color[k] = sdbuffer[buffidx + 2] >> 3;                    // read
        __color[k] = __color[k] << 6 | (sdbuffer[buffidx + 1] >> 2); // green
        __color[k] = __color[k] << 5 | (sdbuffer[buffidx + 0] >> 3); // blue

        buffidx += 3;
      }

      Tft.lcd_set_cursor(offset_x, i);
      Tft.lcd_write_byte(0x22, LCD_CMD);
      __LCD_DC_SET();
      __LCD_CS_CLR();
      for (int m = 0; m < BUFFPIXEL; m ++) {
        __LCD_WRITE_BYTE(__color[m] >> 8);
        __LCD_WRITE_BYTE(__color[m] & 0xFF);
      }
      __LCD_CS_SET();
    }
  }

  Serial.print(millis() - time, DEC);
  Serial.println(" ms");
}

boolean bmpReadHeader(File f)
{
  // read header
  uint32_t tmp;
  uint8_t bmpDepth;

  if (read16(f) != 0x4D42) {
    // magic bytes missing
    return false;
  }

  // read file size
  tmp = read32(f);
  Serial.print("size 0x");
  Serial.println(tmp, HEX);

  // read and ignore creator bytes
  read32(f);

  __Gnbmp_image_offset = read32(f);
  Serial.print("offset ");
  Serial.println(__Gnbmp_image_offset, DEC);

  // read DIB header
  tmp = read32(f);
  Serial.print("header size ");
  Serial.println(tmp, DEC);

  int bmp_width = read32(f);
  int bmp_height = read32(f);

  if (bmp_width != __Gnbmp_width || bmp_height != __Gnbmp_height)  {   // if image is not 320x240, return false
    return false;
  }

  if (read16(f) != 1)
    return false;

  bmpDepth = read16(f);
  Serial.print("bitdepth ");
  Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }

  Serial.print("compression ");
  Serial.println(tmp, DEC);

  return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f)
{
  uint16_t d;
  uint8_t b;
  b = f.read();
  d = f.read();
  d <<= 8;
  d |= b;
  return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f)
{
  uint32_t d;
  uint16_t b;

  b = read16(f);
  d = read16(f);
  d <<= 16;
  d |= b;
  return d;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
