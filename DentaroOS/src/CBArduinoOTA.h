#ifndef _CBARDUINOOTA_H_
#define _CBARDUINOOTA_H_

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "MenuCallBack.h"
#include "Header.h"
class CBArduinoOTA : public MenuCallBack
{
public:
  bool setup(){
    lcd.setTextFont(0);
    lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), (i << 12) | (i << 6));
    }
    lcd.drawString(menuItem->title, 10, 10, &fonts::Font2);
    _started = false;
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.mode(WIFI_MODE_STA);
      WiFi.begin();
      lcd.drawString("WiFi waiting...", 10, 60, &fonts::Font2);
    }
    counter = 0xFF;
    return true;
  }

  bool loop()
  {
    if (_started) {
      ArduinoOTA.handle();
    } else {
      if (WiFi.status() == WL_CONNECTED) {
        startOTA();
      }
    }
    if (++counter == 0) Header.draw();
    return true;
  }

  void close()
  {
    if (_started) {
      ArduinoOTA.end();
    }
  }

private:
  uint8_t counter = 0;
  bool _started = false;

  void startOTA()
  {
    ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        lcd.println("Start updating " + type);
      })
      .onEnd([]() {
        lcd.println("\nEnd");
        cleanup();
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        // lcd.progressBar( 110, 112, 100, 20, progress / (total / 100));
      })
      .onError([](ota_error_t error) {
        lcd.setTextColor(0xFFFF, 0);
        lcd.setTextFont(1);
        lcd.printf("Error[%u]: ", error);
        switch (error) {
        case OTA_AUTH_ERROR:    lcd.println("Auth Failed");   break;
        case OTA_BEGIN_ERROR:   lcd.println("Begin Failed");  break;
        case OTA_CONNECT_ERROR: lcd.println("Connect Failed");break;
        case OTA_RECEIVE_ERROR: lcd.println("Receive Failed");break;
        case OTA_END_ERROR:     lcd.println("End Failed");    break;
        }
      });

    //ArduinoOTA.setRebootOnSuccess(false);
    ArduinoOTA.begin();

    lcd.setTextColor(0xFFFF, 0);
    lcd.setCursor(0,60);
    lcd.setTextFont(1);
    lcd.setTextSize(2);
    lcd.println("host : " + WiFi.localIP().toString());
    //lcd.drawString("host : " + WiFi.localIP().toString(), 0, 60, &fonts::Font2);
    _started = true;
  }
};
#endif
