#if !defined(ESP8266)
#error This code is intended to run only on the ESP8266 boards !
#endif

#define _WEBSOCKETS_LOGLEVEL_ 2

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#include <WebSocketsClient_Generic.h>
#include <SocketIOclient_Generic.h>

#include <Hash.h>

// define digital pin for ESP8266
#define ONE_WIRE_BUS D2

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
ESP8266WiFiMulti WiFiMulti;
SocketIOclient socketIO;

// Select the IP address of your SocketIo server according to your local network
IPAddress serverIP(192, 168, 1, 190);
uint16_t serverPort = 80;

void socketIOEvent(const socketIOmessageType_t &type, uint8_t *payload, const size_t &length)
{
    switch (type)
    {
    case sIOtype_DISCONNECT:
        Serial.println("[IOc] Disconnected");
        break;
    case sIOtype_CONNECT:
        Serial.print("[IOc] Connected to url: ");
        Serial.println((char *)payload);

        // join default namespace (no auto join in Socket.IO V3)
        socketIO.send(sIOtype_CONNECT, "/");

        break;
    case sIOtype_EVENT:
        Serial.print("[IOc] Get event: ");
        Serial.println((char *)payload);

        break;
    case sIOtype_ACK:
        Serial.print("[IOc] Get ack: ");
        Serial.println(length);

        hexdump(payload, length);
        break;
    case sIOtype_ERROR:
        Serial.print("[IOc] Get error: ");
        Serial.println(length);

        hexdump(payload, length);
        break;
    case sIOtype_BINARY_EVENT:
        Serial.print("[IOc] Get binary: ");
        Serial.println(length);

        hexdump(payload, length);
        break;
    case sIOtype_BINARY_ACK:
        Serial.print("[IOc] Get binary ack: ");
        Serial.println(length);

        hexdump(payload, length);
        break;

    case sIOtype_PING:
        Serial.println("[IOc] Get PING");

        break;

    case sIOtype_PONG:
        Serial.println("[IOc] Get PONG");

        break;

    default:
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.print("\nStart ESP8266_WebSocketClientSocketIO on ");
    Serial.println(ARDUINO_BOARD);
    Serial.println(WEBSOCKETS_GENERIC_VERSION);

    // disable AP
    if (WiFi.getMode() & WIFI_AP)
    {
        WiFi.softAPdisconnect(true);
    }

    WiFiMulti.addAP("SSID", "password");

    // WiFi.disconnect();
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }

    Serial.println();

    // Client address
    Serial.print("WebSockets Client started @ IP address: ");
    Serial.println(WiFi.localIP());

    // server address, port and URL
    Serial.print("Connecting to WebSockets Server @ IP address: ");
    Serial.print(serverIP);
    Serial.print(", port: ");
    Serial.println(serverPort);
    sensors.begin();
    // setReconnectInterval to 10s, new from v2.5.1 to avoid flooding server. Default is 0.5s
    socketIO.setReconnectInterval(10000);

    // server address, port and URL
    socketIO.begin(serverIP, serverPort);

    // event handler
    socketIO.onEvent(socketIOEvent);
}

unsigned long messageTimestamp = 0;

void loop()
{
    sensors.requestTemperatures();
    socketIO.loop();

    uint64_t now = millis();

    if (now - messageTimestamp > 5000)
    {
        messageTimestamp = now;

        // creat JSON message for Socket.IO (event)
        DynamicJsonDocument doc(1024);
        JsonArray array = doc.to<JsonArray>();

        // add evnet name
        // Hint: socket.on('event_name', ....
        array.add("event_name");

        // add payload (parameters) for the event
        JsonObject param1 = array.createNestedObject();
        param1["json_key"] = sensors.getTempCByIndex(0);

        // JSON to String (serializion)
        String output;
        serializeJson(doc, output);

        // Send event
        socketIO.sendEVENT(output);

        // Print JSON for debugging
        Serial.println(output);
    }
}