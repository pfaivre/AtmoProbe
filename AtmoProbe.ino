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

#include <Arduino.h>
#include "config.h"

#if PROBE_DHT_ENABLED == 1
    #include "DHTSensor.h"
#endif

#if PROBE_MQ135_ENABLED == 1
    #include "MQ135Sensor.h"
#endif

#include "Io.h"

unsigned long prevMillisProbe; // Timer used for the probe reading

void setup() {
    #if LOG >= 1
        Serial.begin(38400);
        Serial.println(F("AtmoProbe"));
    #endif

    Io::Begin();
    
    prevMillisProbe = millis();

    #if PROBE_DHT_ENABLED == 1
        DHTSensor::Begin();
    #endif
}

void loop() {
    if (millis() - prevMillisProbe >= PROBE_DELAY) {
        prevMillisProbe = millis();

        #if PROBE_DHT_ENABLED == 1
            float value = NAN;
            
            // Temperature
            value = DHTSensor::ReadTemperature();
            if (value != NAN)
                Io::PublishReading("temperature", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read temp"));
            #endif
    
            // Humidity
            value = DHTSensor::ReadHumidity();
            if (value != NAN)
                Io::PublishReading("humidity", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read hum"));
            #endif
        #endif

        #if PROBE_MQ135_ENABLED == 1
            // "Air quality" (harmful gaz concentration)
            value = MQ135Sensor::Read();
            if (value != NAN)
                Io::PublishReading("gaz", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read gaz"));
            #endif
        #endif
    }
    
    Io::Loop();
    
    delay(10);
}
