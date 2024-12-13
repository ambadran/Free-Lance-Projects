#include "project-defs.h"

unsigned int _counter;

// List of init commands according to datasheet SSD1306
const uint8_t __code INIT_SSD1306[] = {
  17,                                                             // number of initializers
  SSD1306_DISPLAY_OFF, 0,                                         // 0xAE = Set Display OFF
  SSD1306_SET_MUX_RATIO, 1, 0x3F,                                 // 0xA8 - 0x3F for 128 x 64 version (64MUX)
                                                                  //      - 0x1F for 128 x 32 version (32MUX)
  SSD1306_MEMORY_ADDR_MODE, 1, 0x00,                              // 0x20 = Set Memory Addressing Mode
                                                                  // 0x00 - Horizontal Addressing Mode
                                                                  // 0x01 - Vertical Addressing Mode
                                                                  // 0x02 - Page Addressing Mode (RESET)
  SSD1306_SET_START_LINE, 0,                                      // 0x40
  SSD1306_DISPLAY_OFFSET, 1, 0x00,                                // 0xD3
  SSD1306_SEG_REMAP_OP, 0,                                        // 0xA0 / remap 0xA1
  SSD1306_COM_SCAN_DIR_OP, 0,                                     // 0xC0 / remap 0xC8
  SSD1306_COM_PIN_CONF, 1, 0x12,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
                                                                  //       0x12 - for 128 x 64 version
                                                                  //       0x02 - for 128 x 32 version
  SSD1306_SET_CONTRAST, 1, 0x7F,                                  // 0x81, 0x7F - reset value (max 0xFF)
  SSD1306_DIS_ENT_DISP_ON, 0,                                     // 0xA4
  SSD1306_DIS_NORMAL, 0,                                          // 0xA6
  SSD1306_SET_OSC_FREQ, 1, 0x80,                                  // 0xD5, 0x80 => D=1; DCLK = Fosc / D <=> DCLK = Fosc
  SSD1306_SET_PRECHARGE, 1, 0xc2,                                 // 0xD9, higher value less blinking
                                                                  // 0xC2, 1st phase = 2 DCLK,  2nd phase = 13 DCLK
  SSD1306_VCOM_DESELECT, 1, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  SSD1306_SET_CHAR_REG, 1, 0x14,                                  // 0x8D, Enable charge pump during display on
  SSD1306_DEACT_SCROLL, 0,                                        // 0x2E
  SSD1306_DISPLAY_ON, 0                                           // 0xAF = Set Display ON
};

// array Chache memory Lcd 8 * 128 = 1024
static char cacheMemLcd[CACHE_SIZE_MEM];

void SSD1306_Init(void) { 

  // initializing the I2C Drivers
  i2cInitialiseMaster(0, I2C_CLOCK_100kHz);

  /* // Sending Initializations */
  /* int i; */
  /* i2cStartCommand(address, I2C_WRITE); */
  /* for(i=0 ; i<44 ; i++) { */
  /*   i2cSendByte(INIT_SSD1306_ADAFRUIT[i]); */
  /* } */

  const uint8_t * list = INIT_SSD1306;
  uint8_t arguments;
  uint8_t commands = *list++;

  // Sending Initializations
  i2cStartCommand(SSD1306_ADDRESS, I2C_WRITE);
  while (commands--) {
    i2cSendByte(SSD1306_COMMAND);
    i2cSendByte(*list++);
    arguments = *list++;
    while (arguments--) {
      i2cSendByte(SSD1306_COMMAND);
      i2cSendByte(*list++); // argument
    }
  }

  i2cStop();

}

void SSD1306_NormalScreen(void) {
  i2cStartCommand(SSD1306_ADDRESS, I2C_WRITE);
  i2cSendByte(SSD1306_COMMAND);
  i2cSendByte(SSD1306_DIS_NORMAL);
  i2cStop();
}

void SSD1306_InverseScreen(void) {

  i2cStartCommand(SSD1306_ADDRESS, I2C_WRITE);
  i2cSendByte(SSD1306_COMMAND);
  i2cSendByte(SSD1306_DIS_INVERSE);
  i2cStop();

}

uint8_t SSD1306_UpdateScreen(void) {
  uint16_t i = 0;

  i2cStartCommand(SSD1306_ADDRESS, I2C_WRITE);
  i2cSendByte(SSD1306_DATA_STREAM);
  while (i < CACHE_SIZE_MEM) {
    i2cSendByte(cacheMemLcd[i++]);  // send data
  }
  i2cStop();

  return SSD1306_SUCCESS;
}

void SSD1306_ClearScreen(void) {
  memset (cacheMemLcd, 0x00, CACHE_SIZE_MEM);                     // null cache memory lcd
}

void SSD1306_SetPosition(uint8_t x, uint8_t y) {
  _counter = x + (y << 7);                                        // update counter
}

uint8_t SSD1306_UpdatePosition(void) {
  uint8_t y = _counter >> 7;                                      // y / 8
  uint8_t x = _counter - (y << 7);                                // y % 8
  uint8_t x_new = x + CHARS_COLS_LENGTH + 1;                      // x + character length + 1
  
  if (x_new > END_COLUMN_ADDR) {                                  // check position
    if (y > END_PAGE_ADDR) {                                      // if more than allowable number of pages
      return SSD1306_ERROR;                                       // return out of range
    } else if (y < (END_PAGE_ADDR-1)) {                           // if x reach the end but page in range
      _counter = ((++y) << 7);                                    // update
    }
  }
 
  return SSD1306_SUCCESS;
}

uint8_t SSD1306_DrawChar(char character) {
  uint8_t i = 0;

  if (SSD1306_UpdatePosition () == SSD1306_ERROR) {
    return SSD1306_ERROR;
  }
  while (i < CHARS_COLS_LENGTH) {
    cacheMemLcd[_counter++] = FONTS[character-32][i++];
  }
  _counter++;

  return SSD1306_SUCCESS;
}

void SSD1306_DrawString(char *str) {
  int i = 0;
  while (str[i] != '\0') {
    SSD1306_DrawChar(str[i++]);
  }
}

uint8_t SSD1306_DrawPixel(uint8_t x, uint8_t y) {
  uint8_t page = 0;
  uint8_t pixel = 0;
  
  if ((x > MAX_X) || (y > MAX_Y)) {                               // if out of range
    return SSD1306_ERROR;                                         // out of range
  }
  page = y >> 3;                                                  // find page (y / 8)
  pixel = 1 << (y - (page << 3));                                 // which pixel (y % 8)
  _counter = x + (page << 7);                                     // update counter
  cacheMemLcd[_counter++] |= pixel;                               // save pixel

  return SSD1306_SUCCESS;
}

uint8_t SSD1306_DrawLine (uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2) {
  int16_t D;                                                      // determinant
  int16_t delta_x, delta_y;                                       // deltas
  int16_t trace_x = 1, trace_y = 1;                               // steps

  delta_x = x2 - x1;                                              // delta x
  delta_y = y2 - y1;                                              // delta y
  
  if (delta_x < 0) {                                              // check if x2 > x1
    delta_x = -delta_x;                                           // negate delta x
    trace_x = -trace_x;                                           // negate step x
  }
  
  if (delta_y < 0) {                                              // check if y2 > y1
    delta_y = -delta_y;                                           // negate detla y
    trace_y = -trace_y;                                           // negate step y
  }

  // Bresenham condition for m < 1 (dy < dx)
  // -------------------------------------------------------------------------------------
  if (delta_y < delta_x) {
    D = (delta_y << 1) - delta_x;                                 // calculate determinant
    SSD1306_DrawPixel (x1, y1);                                   // draw first pixel
    while (x1 != x2) {                                            // check if x1 equal x2
      x1 += trace_x;                                              // update x1
      if (D >= 0) {                                               // check if determinant is positive
        y1 += trace_y;                                            // update y1
        D -= 2*delta_x;                                           // update determinant
      }
      D += 2*delta_y;                                             // update deteminant
      SSD1306_DrawPixel (x1, y1);                                 // draw next pixel
    }
  // for m > 1 (dy > dx)    
  // -------------------------------------------------------------------------------------
  } else {
    D = delta_y - (delta_x << 1);                                 // calculate determinant
    SSD1306_DrawPixel (x1, y1);                                   // draw first pixel
    while (y1 != y2) {                                            // check if y2 equal y1
      y1 += trace_y;                                              // update y1
      if (D <= 0) {                                               // check if determinant is positive
        x1 += trace_x;                                            // update y1
        D += 2*delta_y;                                           // update determinant
      }
      D -= 2*delta_x;                                             // update deteminant
      SSD1306_DrawPixel (x1, y1);                                 // draw next pixel
    }
  }

  return SSD1306_SUCCESS;
}
