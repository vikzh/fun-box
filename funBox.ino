#include <stdint.h>
#include <LCD.h>
#include <SPI.h>
#include <XPT2046.h>
#include <Touch.h>

void setup()
{
  Serial.begin(9600);
  __SD_CS_DISABLE();

  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
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

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
