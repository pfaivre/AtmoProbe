# AtmoProbe

Connected air quality sensor

## Requirements

 * An Arduino compatible board
 * An Ethernet shield or Ethernet interface built-in the board
 * PubSubClient library (can be installed using the library manager)
 * An MQTT broker on your network (a server that will dispatch messages to all the devices)
 * A device that will read messages (using InfluxDB for instance)

On config.h, change the values of `IO_MAC_ADDRESS` and `IO_BROKER_ADDRESS` according to your configuration.

Once started and connected, the device periodically sends its readings on the `probes` topic.
Message format follows the (Influx Line protocol)[https://docs.influxdata.com/influxdb/v1.7/write_protocols/line_protocol_tutorial/].

Examples:
```
probes,location=living_room temperature=21

probes,location=kitchen humidity=71

```