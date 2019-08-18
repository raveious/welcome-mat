/**
 *  @filename   :   epd7in5b.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd7in5b.h"

int epd7in5b_init(void) {

    if (ifinit() != 0) {
        return -1;
    }
    epd7in5b_reset();

    epd7in5b_send_command(POWER_SETTING); 
    epd7in5b_send_data(0x37);
    epd7in5b_send_data(0x00);

    epd7in5b_send_command(PANEL_SETTING);
    epd7in5b_send_data(0xCF);
    epd7in5b_send_data(0x08);
    
    epd7in5b_send_command(BOOSTER_SOFT_START);
    epd7in5b_send_data(0xc7);     
    epd7in5b_send_data(0xcc);
    epd7in5b_send_data(0x28);

    epd7in5b_send_command(POWER_ON);
    epd7in5b_wait_until_idle();

    epd7in5b_send_command(PLL_CONTROL);
    epd7in5b_send_data(0x3c);        

    epd7in5b_send_command(TEMPERATURE_CALIBRATION);
    epd7in5b_send_data(0x00);

    epd7in5b_send_command(VCOM_AND_DATA_INTERVAL_SETTING);
    epd7in5b_send_data(0x77);

    epd7in5b_send_command(TCON_SETTING);
    epd7in5b_send_data(0x22);

    epd7in5b_send_command(TCON_RESOLUTION);
    epd7in5b_send_data(0x02);     //source 640
    epd7in5b_send_data(0x80);
    epd7in5b_send_data(0x01);     //gate 384
    epd7in5b_send_data(0x80);

    epd7in5b_send_command(VCM_DC_SETTING);
    epd7in5b_send_data(0x1E);      //decide by LUT file

    epd7in5b_send_command(0xe5);           //FLASH MODE            
    epd7in5b_send_data(0x03);  

    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void epd7in5b_send_command(unsigned char command) {
    digital_write(DC_PIN, 0);
    spi_transfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void epd7in5b_send_data(unsigned char data) {
    digital_write(DC_PIN, 1);
    spi_transfer(data);
}

/**
 *  @brief: Wait until the BUSY_PIN goes HIGH
 */
void epd7in5b_wait_until_idle(void) {
    while(digital_read(BUSY_PIN) == 0) {      //0: busy, 1: idle
        delay_ms(100);
    }      
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see \ref epd7in5b_sleep();
 */
void epd7in5b_reset(void) {
    digital_write(RST_PIN, 0);                //module reset    
    delay_ms(200);
    digital_write(RST_PIN, 1);
    delay_ms(200);    
}

void epd7in5b_display_frame(const unsigned char* frame_black, const unsigned char* frame_red)
{
    if (frame_black != NULL) {
        epd7in5b_send_command(DATA_START_TRANSMISSION_1);
        delay_ms(2);
        for (int i = 0; i < EPD_WIDTH / 8 * EPD_HEIGHT; i++) {
            epd7in5b_send_data(frame_black[i]);
        }
        delay_ms(2);
    }
    if (frame_red != NULL) {
        epd7in5b_send_command(IMAGE_PROCESS);
        delay_ms(2);
        for (int i = 0; i < EPD_WIDTH / 8 * EPD_HEIGHT; i++) {
            epd7in5b_send_data(frame_red[i]);
        }
        delay_ms(2);
    }
    epd7in5b_send_command(DISPLAY_REFRESH);
    epd7in5b_wait_until_idle();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5. 
 *          You can use \ref epd7in5b_reset() to awaken
 */
void epd7in5b_sleep(void) {
    epd7in5b_send_command(POWER_OFF);
    epd7in5b_wait_until_idle();
    epd7in5b_send_command(DEEP_SLEEP);
    epd7in5b_send_command(0xa5);
}

/* END OF FILE */


