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

#ifndef MCP9601_H
#define MCP9601_H

const uint8_t MCP9601_STATUS_THU = 1 << 6;
const uint8_t MCP9601_STATUS_SC = 1 << 5;
const uint8_t MCP9601_STATUS_OC = 1 << 4;

typedef struct TCData {
    float temperature;
    uint8_t valid;
} TCData;


void mcp9601_init();
void mcp9601_update();
void mcp9601_dump();
float mcp9601_get_cj(uint8_t dev_id);
float mcp9601_get_hj(uint8_t dev_id);
TCData* mcp9601_get_tcdata(uint8_t dev_id);

#endif
