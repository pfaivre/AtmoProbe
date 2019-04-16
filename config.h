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


/*
 * Logging verbosity level:
 * - 0 None (does not begin Serial)
 * - 1 Error and Warning
 * - 2 Info
 * - 3 Debug
 */
#define LOG 0


// ----------------------
// PROBE: reading metrics
// ----------------------

#define PROBE_DHT_ENABLED 1 // 1 activates readings from the DHT sensor. 0 disables it.
#define PROBE_DHT_PIN 5

#define PROBE_MQ135_ENABLED 1 // 1 activates readings from the MQ135 sensor. 0 disables it.
#define PROBE_MQ135_PIN A0
#define PROBE_MQ135_CORRECTION 100 // Value to add to the given metric to get around 100/150 in good air condition

#define PROBE_DELAY 60000 // in milliseconds. Time to wait between two readings.


// -----------------------
// ALERT: Threshold values
// -----------------------

#define ALERT_TEMPERATURE_MIN 10 // °C
#define ALERT_TEMPERATURE_MAX 35 // °C

#define ALERT_HUMIDITY_MIN 20 // %
#define ALERT_HUMIDITY_MAX 60 // %

#define ALERT_GAZ_MAX 250 // ppm


// -------------------------------
// DISPLAY: Print info on a screen
// -------------------------------

#define DISPLAY_ENABLED 1


// ----------------
// IO: Input/Output
// ----------------

#define IO_MAC_ADDRESS { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 } // Mac address of the device (should be written on your ethernet board)
#define IO_ETH_TIMEOUT 5000 // in milliseconds
#define IO_BROKER_ADDRESS "192.168.0.1" // Address of the MQTT broker (e.g. "192.168.0.1").
#define IO_MQTT_USERNAME ""
#define IO_MQTT_PASSWORD ""
#define IO_PUBLICATION_TOPIC "probes"
#define IO_SUBSCRIPTION_TOPIC "probes/com"
