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
#include <Adafruit_BME680.h>

#include "envsensor.h"

const uint8_t BME680_I2C_ADDRESS = 0x76;


Adafruit_BME680 bme;


void envsensor_init(bool init_vco)
{
    while (!bme.begin(BME680_I2C_ADDRESS)) {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        delay(1000);
    }

    // Slowest filtering, reducing jitters
    bme.setTemperatureOversampling(BME680_OS_16X);
    bme.setHumidityOversampling(BME680_OS_16X);
    bme.setPressureOversampling(BME680_OS_16X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_127);

    if (init_vco) {
        bme.setGasHeater(320, 150); // 320*C for 150 ms
    } else {
        bme.setGasHeater(0, 0); // Disable VOC sampling
    }
}

void envsensor_update()
{
    if (!bme.performReading()) {
        Serial.println("BME680: Failed to perform reading");
        return;
    }
}

void envsensor_dump()
{
    String s;
    
    s += "BME680: Ta=" + String(bme.temperature);
    s += "C Pa=" + String(bme.pressure / 100.0);

    s += "hPa RH=" + String(bme.humidity);
    s += "% VCOR=" + String(bme.gas_resistance / 1000.0) + "kOhm";

    Serial.println(s);
}

float envsensor_get_ta()
{
    return bme.temperature;
}

float envsensor_get_pa()
{
    return bme.pressure / 100.0;
}

float envsensor_get_rh()
{
    return bme.humidity;
}

float envsensor_get_vocr()
{
    return bme.gas_resistance / 1000.0;
}
