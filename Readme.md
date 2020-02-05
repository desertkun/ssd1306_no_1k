# SSD1306 display driver for stm32 with no pixel buffer
Regular implementation of ssd1306 driver require to hold 1k pixel buffer in SRAM,
which might be way to expensive.

This implementation uses "character map" which requires only 128 bytes in SRAM.
It allows only placing 8x8 characters at specific cursor positions. 

A font is defined by an array of characters starting from character 32 (space), each
character requires exactly 8 bytes.

## How To Use
1. Add this repo as a submodule.
    ```bash
        git submodule add ssd1306 https://github.com/desertkun/ssd1306_no_1k.git
    ```
2. Add this in your CMake file:
    ```cmake
        # include your stuff beforehand
        include_directories(
            Inc
            Drivers/STM32F1xx_HAL_Driver/Inc
            Drivers/CMSIS/Include
            Drivers/CMSIS/Device/ST/STM32F1xx/Include)
    
        add_subdirectory(ssd1306)
        target_link_libraries(${PROJECT_NAME} ssd1306)
    ```
3. Create file `ssd1306_config.h` in your project and configure the library there.
3. Generate new font:
    ```bash
    python generate_font.py font_example.png
    ```
   (it will produce a file `ssd1306_font.c`)
5. Refer to `font_example.c` for example font