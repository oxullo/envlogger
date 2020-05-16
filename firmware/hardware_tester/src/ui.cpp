// Copyright (c) 2020 OXullo Intersecans <x@brainrapers.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdint.h>
#include <Particle.h>

#include <ADAFRUIT_ST7565.h>

#include "ui.h"
#include "envsensor.h"
#include "mcp9601.h"

static const uint8_t LCD_SI_PIN = A5;
static const uint8_t LCD_SCL_PIN = A3;
static const uint8_t LCD_A0_PIN = RX;
static const uint8_t LCD_RST_PIN = WKP;
static const uint8_t LCD_CS1_PIN = A2;

static const uint8_t LCD_BLIGHT_PIN = TX;

static const uint8_t BUTTON1_PIN = 5;
static const uint8_t BUTTON2_PIN = 6;

#define ST7565_HSPI


static ST7565 glcd(LCD_SI_PIN, LCD_SCL_PIN, LCD_A0_PIN, LCD_RST_PIN, LCD_CS1_PIN);

void ui_backlight_enable(bool on)
{
    if (on) {
        for (uint8_t i=0 ; i < 255 ; ++i) {
            analogWrite(LCD_BLIGHT_PIN, i);
            delay(1);
        }
        analogWrite(LCD_BLIGHT_PIN, 255);
    } else {
        for (uint8_t i=255 ; i > 0 ; --i) {
            analogWrite(LCD_BLIGHT_PIN, i);
            delay(1);
        }
        analogWrite(LCD_BLIGHT_PIN, 0);
    }
}

void ui_init()
{
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);

    pinMode(LCD_BLIGHT_PIN, OUTPUT);
    ui_backlight_enable(true);

    glcd.begin(0x25);
    glcd.clear();
    glcd.drawstring(0, 0, "ENVLOGGER v1");
    glcd.drawstring(0, 1, "Hardware test");
    glcd.display();
}

void ui_update()
{
    glcd.clear();
    String s;

    s = String("Ta=") + String(envsensor_get_ta(), 2) + "C Pa=" + String(envsensor_get_pa(), 2) + "hPa"; 
    glcd.drawstring(0, 0, s.c_str());

    s = String("RH=") + String(envsensor_get_rh(), 2) + "% Vr=" + String(envsensor_get_vocr(), 2) + "k"; 
    glcd.drawstring(0, 1, s.c_str());

    s = String("TC1=") + String(mcp9601_get_hj(0), 2) + "C TC2=" + String(mcp9601_get_hj(1), 2) + "C"; 
    glcd.drawstring(0, 2, s.c_str());

    s = String("B1=") + digitalRead(BUTTON1_PIN) + " B2=" + digitalRead(BUTTON2_PIN);
    glcd.drawstring(0, 3, s.c_str());

    glcd.display();
}
