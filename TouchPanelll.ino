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

  Tft.lcd_init();                                      // init TFT library
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
          Tft.lcd_clear_screen(RED);
          break;
        case 2:
          Tft.lcd_clear_screen(BLUE);
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

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
