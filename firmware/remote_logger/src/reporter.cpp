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

#include "reporter.h"

static const uint32_t UPDATE_MIN_PERIOD = 1000;
static const IPAddress INFLUX_SERVER_IP(192, 168, 20, 16);
static const uint16_t INFLUX_SERVER_PORT = 8089;
static const char* DEVICE_IDENT = "proto-01";


static UDP udp_client;
static uint32_t ts_last_update = 0;

void reporter_update()
{
    if (millis() - ts_last_update > UPDATE_MIN_PERIOD) {
        String s;
        TCData* tcd;

        s += "envlogger,device=" + String(DEVICE_IDENT) + " ";
        s += "tamb=" + String(envsensor_get_ta(), 2);
        s += ",atmp=" + String(envsensor_get_pa(), 2);
        s += ",rh=" + String(envsensor_get_rh(), 2);
        s += ",vocr=" + String(envsensor_get_vocr(), 2);
        
        tcd = mcp9601_get_tcdata(0);
        if (tcd->valid) {
            s += ",tc1=" + String(tcd->temperature);
        }

        tcd = mcp9601_get_tcdata(1);
        if (tcd->valid) {
            s += ",tc2=" + String(tcd->temperature);
        }

        udp_client.begin((int)INFLUX_SERVER_PORT);
        if (udp_client.sendPacket(s, s.length(), INFLUX_SERVER_IP, INFLUX_SERVER_PORT) < 0) {
            Serial.println("Cannot send update packet");
        }
        udp_client.stop();

        ts_last_update = millis();
    }
}
