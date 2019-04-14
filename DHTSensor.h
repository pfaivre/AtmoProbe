/**
 * AtmoProbe
 *
 * Copyright (C) 2019 Pierre Faivre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "config.h"

#define DHTTYPE DHT22


/**
 * Reads metrics from a DHT22 sensor
 * Detects level of humidity and temperature
 */
class DHTSensor {
public:
    /**
     * Initialize the DHT sensor
     */
    static void Begin();

    /**
     * Read level of humidity
     * @return Atmospheric humidity as percentage
     */
    static float ReadHumidity();

    /**
     * Read level of atmospheric temperature
     * @return Temperature value as Celsius degrees
     */
    static float ReadTemperature();
};
