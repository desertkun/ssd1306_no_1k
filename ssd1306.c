#include "ssd1306.h"
#include <string.h>

// Screen buffer
static char display_buffer[SSD1306_WIDTH * SSD1306_HEIGHT];
static uint8_t display_position;
static const font_def_t* display_font;

#if defined(SSD1306_USE_I2C)

void ssd1306_Reset(void) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
void ssd1306_write_command(uint8_t byte) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

// Send data
void ssd1306_write_data(uint8_t* buffer, size_t buff_size) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

#elif defined(SSD1306_USE_SPI)

void ssd1306_reset(void) {
	// CS = High (not selected)
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);

	// Reset the OLED
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

// Send a byte to the command register
void ssd1306_write_command(uint8_t byte) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET); // command
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

// Send data
void ssd1306_write_data(uint8_t* buffer, size_t buff_size) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET); // data
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif


// Initialize the oled screen
void ssd1306_init(const font_def_t* font)
{
    display_position = 0;
    display_font = font;

	// Reset OLED
	ssd1306_reset();

    // Wait for the screen to boot
    HAL_Delay(100);
    
    // Init OLED
    ssd1306_write_command(0xAE); //display off

    ssd1306_write_command(0x20); //Set Memory Addressing Mode   
    ssd1306_write_command(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                 // 10b,Page Addressing Mode (RESET); 11b,Invalid

    ssd1306_write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_write_command(0xC0); // Mirror vertically
#else
    ssd1306_write_command(0xC8); //Set COM Output Scan Direction
#endif

    ssd1306_write_command(0x00); //---set low column address
    ssd1306_write_command(0x10); //---set high column address

    ssd1306_write_command(0x40); //--set start line address - CHECK

    ssd1306_write_command(0x81); //--set contrast control register - CHECK
    ssd1306_write_command(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_write_command(0xA0); // Mirror horizontally
#else
    ssd1306_write_command(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_write_command(0xA7); //--set inverse color
#else
    ssd1306_write_command(0xA6); //--set normal color
#endif

    ssd1306_write_command(0xA8); //--set multiplex ratio(1 to 64) - CHECK
    ssd1306_write_command(0x3F); //

    ssd1306_write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_write_command(0xD3); //-set display offset - CHECK
    ssd1306_write_command(0x00); //-not offset

    ssd1306_write_command(0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_write_command(0xF0); //--set divide ratio

    ssd1306_write_command(0xD9); //--set pre-charge period
    ssd1306_write_command(0x22); //

    ssd1306_write_command(0xDA); //--set com pins hardware configuration - CHECK
    ssd1306_write_command(0x12);

    ssd1306_write_command(0xDB); //--set vcomh
    ssd1306_write_command(0x20); //0x20,0.77xVcc

    ssd1306_write_command(0x8D); //--set DC-DC enable
    ssd1306_write_command(0x14); //
    ssd1306_write_command(0xAF); //--turn on SSD1306 panel

}

void ssd1306_set_font(const font_def_t* font)
{
    display_font = font;
}

void ssd1306_update_screen()
{
    ssd1306_write_command(0xB0);
    ssd1306_write_command(0x00);
    ssd1306_write_command(0x10);

    char *ch = display_buffer;

    for(uint8_t y = 0; y < SSD1306_HEIGHT; y++)
    {
        for(uint8_t x = 0; x < SSD1306_WIDTH; x++)
        {
            const uint8_t* data;

            if (*ch < 32 || *ch > display_font->characters_count)
            {
                // refer to the first character, which is a space
                data = display_font->characters;
            }
            else
            {
                uint8_t c = *ch - 32;
                data = display_font->characters + c * 8;
            }

            // send one character
            ssd1306_write_data((uint8_t*)data, 8);
            ch++;
        }
    }
}

void ssd1306_set_cursor(uint8_t x, uint8_t y)
{
    display_position = y * SSD1306_WIDTH + x;
}

void ssd1306_write_string(const char* str)
{
    uint8_t len = strlen(str);
    memcpy(display_buffer + display_position, str, len);
    display_position += len;
}

void ssd1306_write_char(char c)
{
    display_buffer[display_position++] = c;
}

void ssd1306_fill(char c)
{
    memset(display_buffer, c, sizeof(display_buffer));
}
