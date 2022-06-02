# Socket.Io ESP8266 Client Integration

## Used libraries

[SocketIo](https://github.com/Links2004/arduinoWebSockets)\
[OneWire](https://github.com/PaulStoffregen/OneWire)\
[Temperature Control Library](https://github.com/milesburton/Arduino-Temperature-Control-Library)\
[ESP8266](https://github.com/khoih-prog/ESP_AT_Lib )

## Installation

Install all libraries above


## Usage
Set IP of a SocketIo server and port
```cpp
// Select the IP address of your SocketIo server according to your local network
IPAddress serverIP(192, 168, 1, 190);
uint16_t serverPort = 80;
```
Provide WiFi SSID and Password
```cpp
WiFiMulti.addAP("SSID", "password");
```
Set name of the emitting event and json key name
```cpp
array.add("event_name");
param1["json_key"] = sensors.getTempCByIndex(0);
//this will create websocket > event_name : { json_key : 20.00 }
``` 

