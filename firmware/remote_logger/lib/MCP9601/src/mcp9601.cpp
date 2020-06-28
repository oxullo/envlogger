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
#include <Wire.h>

#include "mcp9601.h"

#define MCP9601_NUM_DEVS        2

static const uint8_t MCP9601A_I2C_ADDRESS = 0x60;
static const uint8_t MCP9601B_I2C_ADDRESS = 0x67;

static const uint8_t MCP9601_ADDRESSES[MCP9601_NUM_DEVS] = {MCP9601A_I2C_ADDRESS, MCP9601B_I2C_ADDRESS};

static const uint8_t MCP9601_REG_HJTH = 0x00;
static const uint8_t MCP9601_REG_CJTC = 0x02;
static const uint8_t MCP9601_REG_STATUS = 0x04;
static const uint8_t MCP9601_REG_DEVID = 0x20;

static const uint8_t MCP9601_INVALID_ERR_THRESHOLD = 5;

typedef struct TCStore {
    float cj_temp;
    float hj_temp;
    uint8_t status;
    uint8_t update_error_count;
} TC_store;

static TCStore tc_store[2] = {0};
static TCData tc_data[2] = {0};


static int8_t mcp9601_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* buffer, uint8_t len)
{
    Wire.beginTransmission(dev_addr);
    Wire.write(reg_addr);
    Wire.endTransmission();
    
    if (Wire.requestFrom(dev_addr, len) == len) {
        for (uint8_t i=0 ; i < len ; ++i) {
            buffer[i] = Wire.read();
        }
        return 0;
    } else {
        return -1;
    }
}

static int8_t mcp9601_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* buffer, uint8_t len)
{
    Wire.beginTransmission(dev_addr);
    Wire.write(reg_addr);
    
    for (uint8_t i=0 ; i < len ; ++i) {
        Wire.write(buffer[i]);
    }
    Wire.endTransmission();

    return 0;
}

static uint8_t mcp9601_read_devid(uint8_t dev_id)
{
    uint8_t buffer[2];
    uint8_t dev_addr = MCP9601_ADDRESSES[dev_id];
    
    if (mcp9601_read_reg(dev_addr, MCP9601_REG_DEVID, buffer, 2) == 0) {
        Serial.print("MCP9601: dev_id=");
        Serial.print(dev_id);
        Serial.print(" dev_addr=");
        Serial.print(dev_addr, HEX);
        Serial.print(" id=");
        Serial.print(buffer[0], HEX);
        Serial.print(" rev maj=");
        Serial.print(buffer[1] >> 4, HEX);
        Serial.print(" rev min=");
        Serial.println(buffer[1] & 0x0f, HEX);
        
        return buffer[0];
    } else {
        Serial.print("Failed to init MCP9601 dev_addr=");
        Serial.println(dev_addr, HEX);
        
        return 0;
    }
}

static float mcp9601_calculate_hjth(uint8_t msb, uint8_t lsb)
{
    float th = (msb & 0x7f) * 16.0 + (float) lsb / 16.0;
    
    if (msb & 0x80) {
        // less than zero
        return th - 4096;
    } else {
        return th;
    }
}

static float mcp9601_calculate_cjtc(uint8_t msb, uint8_t lsb)
{
    float th = (msb & 0x0f) * 16.0 + (float) lsb / 16.0;
    
    if (msb & 0x08) {
        // less than zero
        return th - 4096;
    } else {
        return th;
    }
}

void mcp9601_init()
{
    for (uint8_t dev_id=0 ; dev_id < MCP9601_NUM_DEVS ; ++dev_id) {
        mcp9601_read_devid(dev_id);
    }
}

void mcp9601_update()
{
    uint8_t buffer[2];
    for (uint8_t dev_id=0 ; dev_id < MCP9601_NUM_DEVS ; ++dev_id) {
        uint8_t dev_addr = MCP9601_ADDRESSES[dev_id];
        TCStore* tcs = &tc_store[dev_id];
        TCData* tcd = &tc_data[dev_id];

        uint8_t valid = 1;

        if (mcp9601_read_reg(dev_addr, MCP9601_REG_STATUS, buffer, 1) == 0) {
            tcs->status = buffer[0];
        } else {
            valid = 0;
        }

        if (!(tcs->status & MCP9601_STATUS_THU)
                || tcs->status & MCP9601_STATUS_SC
                || tcs->status & MCP9601_STATUS_OC) {
            valid = 0;
        } else {
            // Clear ThUPDATE
            buffer[0] = 0;
            mcp9601_write_reg(dev_addr, MCP9601_REG_STATUS, buffer, 1);

            if (mcp9601_read_reg(dev_addr, MCP9601_REG_HJTH, buffer, 2) == 0) {
                tcs->hj_temp = mcp9601_calculate_hjth(buffer[0], buffer[1]);
            } else {
                valid = 0;
            }

            if (mcp9601_read_reg(dev_addr, MCP9601_REG_CJTC, buffer, 2) == 0) {
                tcs->cj_temp = mcp9601_calculate_cjtc(buffer[0], buffer[1]);
            }
        }

        if (valid) {
            tcd->temperature = tcs->hj_temp;
            tcd->valid = 1;
            tcs->update_error_count = 0;
        } else {
            if (tcs->update_error_count > MCP9601_INVALID_ERR_THRESHOLD) {
                tcd->valid = 0;
            } else {
                ++tcs->update_error_count;
            }
        }
    }
}

void mcp9601_dump()
{
    for (uint8_t dev_id=0 ; dev_id < MCP9601_NUM_DEVS ; ++dev_id) {
        uint8_t dev_addr = MCP9601_ADDRESSES[dev_id];
        TCStore* tcs = &tc_store[dev_id];
        TCData* tcd = &tc_data[dev_id];

        Serial.print("MCP9601: dev_id=");
        Serial.print(dev_id);
        Serial.print(" dev_addr=");
        Serial.print(dev_addr, HEX);

        Serial.print(" uec=");
        Serial.print(tcs->update_error_count);

        Serial.print(" valid=");
        Serial.print(tcd->valid);

        Serial.print(" status=");
        Serial.print(tcs->status, HEX);

        Serial.print(" hj/th=");
        Serial.print(tcs->hj_temp);
        Serial.print("C ");

        Serial.print(" cj/tc=");
        Serial.print(tcs->cj_temp);
        Serial.print("C ");
        Serial.println();
    }
}

float mcp9601_get_cj(uint8_t dev_id)
{
    return tc_store[dev_id].cj_temp;
}

float mcp9601_get_hj(uint8_t dev_id)
{
    return tc_store[dev_id].hj_temp;
}

TCData* mcp9601_get_tcdata(uint8_t dev_id)
{
    return &tc_data[dev_id];
}
