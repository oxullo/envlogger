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

static const uint32_t BACKLIGHT_OFF_PERIOD = 10000;

#define ST7565_HSPI


static ST7565 glcd(LCD_SI_PIN, LCD_SCL_PIN, LCD_A0_PIN, LCD_RST_PIN, LCD_CS1_PIN);
static uint32_t ts_last_press = 0;


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

void check_backlight()
{
    if (ts_last_press && millis() - ts_last_press > BACKLIGHT_OFF_PERIOD) {
        ui_backlight_enable(false);
        ts_last_press = 0;
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

    ts_last_press = millis();
}

void ui_update()
{
    glcd.clear();
    String s;
    TCData* tcd;

    s = String("Ta=") + String(envsensor_get_ta(), 2) + "C Pa=" + String(envsensor_get_pa(), 2) + "hPa"; 
    glcd.drawstring(0, 0, s.c_str());

    s = String("RH=") + String(envsensor_get_rh(), 2) + "% Vr=" + String(envsensor_get_vocr(), 2) + "k"; 
    glcd.drawstring(0, 1, s.c_str());

    s = String("TC1=");    
    tcd = mcp9601_get_tcdata(0);
    s += tcd->valid ? (String(tcd->temperature, 2) + "C") : "N/A";
    s += " TC2=";
    tcd = mcp9601_get_tcdata(1);
    s += tcd->valid ? (String(tcd->temperature, 2) + "C") : "N/A";
    glcd.drawstring(0, 2, s.c_str());

    s = String("B1=") + digitalRead(BUTTON1_PIN) + " B2=" + digitalRead(BUTTON2_PIN);
    glcd.drawstring(0, 3, s.c_str());

    glcd.display();

    if (digitalRead(BUTTON1_PIN) == LOW || digitalRead(BUTTON2_PIN) == LOW) {
        if (ts_last_press == 0) {
            ui_backlight_enable(true);
        }
        ts_last_press = millis();
    }

    check_backlight();
}
