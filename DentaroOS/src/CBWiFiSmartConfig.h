#ifndef _CBWIFISMARTCONFIG_H_
#define _CBWIFISMARTCONFIG_H_

#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "MenuCallBack.h"
#include "Header.h"

class CBWiFiSmartConfig : public MenuCallBack
{
public:
  bool setup() {
    Header.draw();
    lcd.setTextFont(0);
    lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), i << 6);
    }
    lcd.drawString("WiFi SmartConfig", 10, 10, &fonts::Font2);

    lcd.drawCentreString("iOS", 70, 26, &fonts::Font2);
    lcd.drawCentreString("Android", 250, 26, &fonts::Font2);
    lcd.qrcode("https://itunes.apple.com/app/id1071176700", 0, 40, 140, 3);                   
    lcd.qrcode("https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch", 180, 40, 140, 4);

    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    lcd.setCursor(0,180);
    lcd.setTextFont(2);
    lcd.print("SmartConfig Started.");
    return true;
  }

  bool loop() {
    if (!(++counter & 0xF)) Header.draw();
    if (WiFi.smartConfigDone() && WiFi.status() == WL_CONNECTED) {
      lcd.setTextFont(0);
      lcd.setTextFont(2);
      lcd.setTextColor(0xFFFF, 0);
      lcd.println(" Done !");
      lcd.println("SSID : " + WiFi.SSID());
      Preferences preferences;
      preferences.begin("wifi-config");
      preferences.putString("WIFI_SSID", WiFi.SSID());
      preferences.putString("WIFI_PASSWD", WiFi.psk());
      preferences.end();
      delay(1000);
      return false;
    } else {
      delay(10);
    }
    return true;
  }

  void close()
  {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.stopSmartConfig();
  }

private:
  long counter = 0;
};
#endif
