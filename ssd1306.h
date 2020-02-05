/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stddef.h>

#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#else
 #error "SSD1306 library was tested only on STM32F1, STM32F3, STM32F4, STM32L0, STM32L4, STM32H7 MCU families. Please modify ssd1306.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif

#include "ssd1306_config.h"

/* vvv I2C config vvv */

#ifndef SSD1306_I2C_PORT
#define SSD1306_I2C_PORT		hi2c1
#endif

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

/* ^^^ I2C config ^^^ */

/* vvv SPI config vvv */

#ifndef SSD1306_SPI_PORT
#define SSD1306_SPI_PORT        hspi2
#endif

#ifndef SSD1306_CS_Port
#define SSD1306_CS_Port         GPIOB
#endif
#ifndef SSD1306_CS_Pin
#define SSD1306_CS_Pin          GPIO_PIN_12
#endif

#ifndef SSD1306_DC_Port
#define SSD1306_DC_Port         GPIOB
#endif
#ifndef SSD1306_DC_Pin
#define SSD1306_DC_Pin          GPIO_PIN_14
#endif

#ifndef SSD1306_Reset_Port
#define SSD1306_Reset_Port      GPIOA
#endif
#ifndef SSD1306_Reset_Pin
#define SSD1306_Reset_Pin       GPIO_PIN_8
#endif

/* ^^^ SPI config ^^^ */

#if defined(SSD1306_USE_I2C)
extern I2C_HandleTypeDef SSD1306_I2C_PORT;
#elif defined(SSD1306_USE_SPI)
extern SPI_HandleTypeDef SSD1306_SPI_PORT;
#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro!"
#endif

// SSD1306 OLED height in characters
#define SSD1306_HEIGHT          8
// SSD1306 OLEDwidth in characters
#define SSD1306_WIDTH           16

typedef struct {
    // an array of normal characters 8x8 (ord 32 thru 127)
    // each character is encoded by 8 bytes, each byte encodes one column of 8 pixels
    // from left to right
    const uint8_t* characters;
    // amount of characters we have, there could be only 32 of them
    const uint8_t characters_count;
} font_def_t;

// Procedure definitions
void ssd1306_init(font_def_t* font);
void ssd1306_set_font(font_def_t* font);
void ssd1306_update_screen();
void ssd1306_set_cursor(uint8_t x, uint8_t y);
void ssd1306_write_string(const char* str);
void ssd1306_write_char(char c);
void ssd1306_fill(char c);

// Low-level procedures
void ssd1306_reset(void);
void ssd1306_write_command(uint8_t byte);
void ssd1306_write_data(uint8_t* buffer, size_t buff_size);

#endif // __SSD1306_H__
