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

#include <Particle.h>

#include "envsensor.h"
#include "mcp9601.h"
#include "ui.h"

const uint8_t VRU_ENABLE_PIN = 4;


SYSTEM_MODE(MANUAL);

void enable_system_power()
{
    pinMode(VRU_ENABLE_PIN, OUTPUT);
    digitalWrite(VRU_ENABLE_PIN, HIGH);
    delay(500);
}

void setup()
{
    Wire.begin();
    Serial.begin(9600);
    
    enable_system_power();

    envsensor_init(false);
    mcp9601_init();
    ui_init();

    delay(1000);
}

void loop()
{
    envsensor_update();

    mcp9601_update();
    mcp9601_dump();

    Serial.println("--------");

    ui_update();
}
