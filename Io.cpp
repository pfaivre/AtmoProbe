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

#include "Io.h"
#include "config.h"

#include <EthernetClient.h>
#include <PubSubClient.h>

EthernetClient eth;
PubSubClient mqtt(eth);
bool ethConnected = false;
unsigned long prevMillisNetwork; // Timer used for the network monitoring

#define MESSAGE_BUFFER_SIZE 64

char* location = "living_room";

void Io::Begin() {
    prevMillisNetwork = millis();
        
    // disable SD card
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    pinMode(13, OUTPUT); // Built-in LED is used to tell about network status
    digitalWrite(13, LOW);

    Io::_connect();
}

void Io::Loop() {
    // Periodically check network status
    if (millis() - prevMillisNetwork >= 30000) {
        prevMillisNetwork = millis();
        Io::_connect();
    }

    if (mqtt.connected()) {
        mqtt.loop();
    }
}

void Io::_connect() {
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
        mqtt.setCallback(Io::Callback);
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

void Io::PublishReading(char *fieldKey, float value) {
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

void Io::Callback(char* topic, byte* payload, unsigned int length) {
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
