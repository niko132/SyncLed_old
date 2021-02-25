#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <FS.h>
#include <ESPAsyncWebServer.h>

#include "Logger.h"
#include "MessageManager.h"
#include "DeviceManager.h"
#include "TimeManager.h"

#include "PaletteManager.h"

#include "VirtualDeviceManager.h"


#ifndef STASSID
#define STASSID "WGLan"
#define STAPSK  "94384322823429699220"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

IPAddress ip(192, 168, 178, 113);

IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


String msg = "";


void setup() {
    Serial.begin(115200);
    Logger.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Logger.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    setupOTA();


    if(!SPIFFS.begin()){
        Serial.println("An Error has occurred while mounting SPIFFS");
    }


    server.on("/millis", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String(millis()));
    });

    server.on("/millis1", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String((int)(millis() / 1000.0f)));
    });

    server.on("/free_heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String(ESP.getFreeHeap()));
    });

    /*
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger.println("Request to index.html");
		request->send(SPIFFS, "/index.html", String());
	});

    server.on("/grapick.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger.println("Request to Grapick JS");
		request->send(SPIFFS, "/grapick.min.js", String());
	});

    server.on("/grapick.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger.println("Request to Grapick CSS");
		request->send(SPIFFS, "/grapick.min.css", String());
	});
    */

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");





    ws.onEvent([msg](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if(type == WS_EVT_CONNECT){
            Logger.printf("ws[%s][%u] connect", server->url(), client->id());
            Logger.println();

            DynamicJsonDocument doc(2048);
            JsonObject root = doc.to<JsonObject>();
            DeviceManager.writeConfig(root);
            VirtualDeviceManager.writeConfig(root);
            PaletteManager.writeConfig(root);

            String output = "";
            serializeJson(doc, output);

            client->text(output);
        } else if(type == WS_EVT_DISCONNECT){
            Logger.printf("ws[%s][%u] disconnect", server->url(), client->id());
            Logger.println();
        } else if(type == WS_EVT_ERROR){
            Logger.printf("ws[%s][%u] error(%u): %s", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
            Logger.println();
        } else if(type == WS_EVT_PONG){
            Logger.printf("ws[%s][%u] pong[%u]: %s", server->url(), client->id(), len, (len)?(char*)data:"");
            Logger.println();
        } else if(type == WS_EVT_DATA){
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if(info->final && info->index == 0 && info->len == len){
                //the whole message is in a single frame and we got all of it's data
                Logger.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
                Logger.println();
                msg = "";

                if(info->opcode == WS_TEXT) { // we only care about text messages
                    for(size_t i=0; i < info->len; i++) {
                        msg += (char) data[i];
                    }

                    // TODO: message received
                    handleConfig(msg);
                }
            } else {
                //message is comprised of multiple frames or the frame is split into multiple packets
                if(info->index == 0){
                    if(info->num == 0) {
                        Logger.printf("ws[%s][%u] %s-message start", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                        Logger.println();
                        msg = "";
                    }

                    Logger.printf("ws[%s][%u] frame[%u] start[%llu]", server->url(), client->id(), info->num, info->len);
                    Logger.println();
                }

                Logger.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
                Logger.println();

                if(info->opcode == WS_TEXT) { // we only care about text messages
                    for(size_t i=0; i < len; i++) {
                        msg += (char) data[i];
                    }

                    if ((info->index + len) == info->len && info->final) {
                        // TODO: message received
                        handleConfig(msg);
                    }
                }
            }
        }
    });

    server.addHandler(&ws);


    server.begin();
    MessageManager.begin();
    DeviceManager.begin();
    TimeManager.begin();


    // create at least 1 default palette
    Palette *blueAndRedPalette = PaletteManager.createPalette("BlueAndRed");
    blueAndRedPalette->addColorKey(0.0, rgbToColor(0, 0, 255));
    blueAndRedPalette->addColorKey(1.0, rgbToColor(255, 0, 0));

    Palette *rainbowPalette = PaletteManager.createPalette("Rainbow");
    rainbowPalette->addColorKey(1.0 / 6.0, rgbToColor(255, 0, 0));
    rainbowPalette->addColorKey(0.5, rgbToColor(0, 255, 0));
    rainbowPalette->addColorKey(5.0 / 6.0, rgbToColor(0, 0, 255));



    VirtualDeviceManager.begin(180);
    unsigned long id1 = VirtualDeviceManager.addVirtualDevice(0, 60);
    delay(2);
    unsigned long id2 = VirtualDeviceManager.addVirtualDevice(120, 180);



    VirtualDevice *dev1 = VirtualDeviceManager.getVirtualDevice(id1);
    dev1->setPosOffsetStart(0.0);
    dev1->setPosOffsetEnd(0.25);

    VirtualDevice *dev2 = VirtualDeviceManager.getVirtualDevice(id2);
    dev2->setPosOffsetStart(0.25);
    dev2->setPosOffsetEnd(0.5);





    Logger.println("Ready");
    Logger.print("IP address: ");
    Logger.println(WiFi.localIP());
}

void handleConfig(String json) {
    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, json);
    JsonObject root = doc.as<JsonObject>();
    bool success1 = PaletteManager.fromConfig(root);
    bool success2 = DeviceManager.fromConfig(root);
    bool success3 = VirtualDeviceManager.fromConfig(root);

    Logger.println("Got config: " + String(success1) + " " + String(success2) + " " + String(success3));
}

void setupOTA() {
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);

        SPIFFS.end();
    });
    ArduinoOTA.onEnd([]() {
        Logger.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Logger.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Logger.printf("Error[%u]: ", error);

        if (error == OTA_AUTH_ERROR) {
            Logger.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Logger.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Logger.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Logger.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Logger.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}

void loop() {
    ArduinoOTA.handle();

    DeviceManager.update();
    TimeManager.update();

    VirtualDeviceManager.update();
}
