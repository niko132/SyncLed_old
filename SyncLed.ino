#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <ESPAsyncWebServer.h>

#include "Logger.h"
#include "MessageManager.h"
#include "DeviceManager.h"
#include "TimeManager.h"

#include "VirtualDeviceManager.h"


#ifndef STASSID
#define STASSID "WGLan"
#define STAPSK  "94384322823429699220"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

IPAddress ip(192, 168, 178, 116);

IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);


AsyncWebServer server(80);

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

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String(millis()));
    });

    server.on("/1", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String((int)(millis() / 1000.0f)));
    });

    server.on("/free_heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", String(ESP.getFreeHeap()));
    });

    server.begin();
    MessageManager.begin();
    DeviceManager.begin();
    TimeManager.begin();

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
