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

#if IO_NETWORKING == 1
    #include <EthernetClient.h>
    #include <PubSubClient.h>

    EthernetClient eth;
    PubSubClient mqtt(eth);
    bool ethConnected = false;
    unsigned long prevMillisNetwork; // Timer used for the network monitoring
#endif

unsigned long prevMillisProbe; // Timer used for the probe reading

char* location = "living_room";

#define MESSAGE_BUFFER_SIZE 64

void setup() {
    #if LOG >= 1
        Serial.begin(38400);
        Serial.println(F("AtmoProbe"));
    #endif

    #if IO_NETWORKING == 1
        prevMillisNetwork = millis();
        
        // disable SD card
        pinMode(4, OUTPUT);
        digitalWrite(4, HIGH);

        pinMode(13, OUTPUT); // Built-in LED is used to tell about network status
        digitalWrite(13, LOW);

        _connect();
    #endif

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
                _publishReading("temperature", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read temp"));
            #endif
    
            // Humidity
            value = DHTSensor::ReadHumidity();
            if (value != NAN)
                _publishReading("humidity", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read hum"));
            #endif
        #endif

        #if PROBE_MQ135_ENABLED == 1
            // "Air quality" (harmful gaz concentration)
            value = MQ135Sensor::Read();
            if (value != NAN)
                _publishReading("gaz", value);
            #if LOG >=1
                else
                    Serial.println(F("Couldn't read gaz"));
            #endif
        #endif
    }
    
    #if IO_NETWORKING == 1
        if (mqtt.connected()) {
            mqtt.loop();
        }

        // Periodically check network status
        if (millis() - prevMillisNetwork >= 30000) {
            prevMillisNetwork = millis();
            _connect();
        }
    #endif
    
    delay(10);
}

#if IO_NETWORKING == 1

/**
 * Connect to the broker through Ethernet.
 * Does nothing if the connection is already active
 */
void _connect() {
    // Start the ethernet interface and try to get an IP address from the DHCP server.
    if (!ethConnected) {
        #if LOG >= 2
            Serial.println(F("Connecting eth..."));
        #endif
        byte mac[] = IO_MAC_ADDRESS;
        if (Ethernet.begin(mac, IO_ETH_TIMEOUT) == 0) {
            digitalWrite(13, LOW);
            ethConnected = false;
            #if LOG >= 1
                Serial.print(F("eth failed "));
                Serial.println(Ethernet.localIP());
            #endif
        }
        else {
            ethConnected = true;
            #if LOG >= 2
                Serial.print(F("DHCP assigned IP "));
                Serial.println(Ethernet.localIP());
            #endif
        }
    }

    // Connect to the MQTT broker
    if (ethConnected && !mqtt.connected()) {
        #if LOG >= 2
            Serial.println(F("Trying to connect to the broker..."));
        #endif
        mqtt.setServer(IO_BROKER_ADDRESS, 1883);
        mqtt.setCallback(_callback);
        char clientId[18]; // "xx:xx:xx:xx:xx:xx"
        byte mac[] = IO_MAC_ADDRESS;
        sprintf(clientId, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        if (mqtt.connect(clientId, IO_MQTT_USERNAME, IO_MQTT_PASSWORD))
        {
            digitalWrite(13, HIGH);
            mqtt.subscribe(IO_SUBSCRIPTION_TOPIC);
            #if LOG >= 2
                Serial.println(F("Connected to broker"));
            #endif
        }
        else
        {
            digitalWrite(13, LOW);
            #if LOG >= 1
                Serial.println(F("Failed to connect to broker"));
            #endif
        }
    }
}

void _publishReading(char *fieldKey, float value) {
    if (mqtt.connected()) {

        char buffer[MESSAGE_BUFFER_SIZE] = { '\0' };
        int val_int = (int)value; // compute the integer part of the float
        int val_fra = (int)((value - (float)val_int) * 10); // compute 1 decimal places (and convert it to int)
        snprintf(buffer, MESSAGE_BUFFER_SIZE, "%s,location=%s %s=%d.%d", IO_PUBLICATION_TOPIC, location, fieldKey, val_int, val_fra);
        mqtt.publish(IO_PUBLICATION_TOPIC, buffer);
        #if LOG >= 2
            Serial.println(buffer);
        #endif
    }
}

/**
 * Callback for the PubSubClient library
 * Handles incoming messages from the broker
 */
void _callback(char* topic, byte* payload, unsigned int length) {
    char buffer[17] = { 0 };
    strncpy(buffer, (char*)payload, min(16, length));
    
    #if LOG >= 3
        Serial.print(F("In msg ["));
        Serial.print(topic);
        Serial.print(F("] "));
        Serial.print(buffer);
        Serial.println();
    #endif

    // Do something with the message
}

#endif
