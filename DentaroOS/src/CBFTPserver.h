#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include "MenuCallBack.h"
#include "Header.h"
#include "ESP32FtpServer.h"
#include <SD.h>
#include <SPIFFS.h>
class CBFTPserver : public MenuCallBack
{
public:
  bool setup(){
    lcd.setTextFont(0);
    bool flgSD = isSD();
    lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), i << (flgSD ? 1 : 6));
    }
    lcd.drawString(menuItem->title, 10, 10, &fonts::Font2);
    _started = false;
    if (flgSD) {
      SD.end();
      SD.begin( TFCARD_CS_PIN, SPI, 40000000);
    } else {
      SPIFFS.begin();
    }
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
      getFTPServ()->handleFTP();
    } else {
      if (WiFi.status() == WL_CONNECTED) {
        startFTP();
      }
    }
    if (++counter == 0) Header.draw();
    return true;
  }

  void close()
  {
    if (_started) {
      getFTPServ()->end();
    }
  }

private:
  uint8_t counter = 0;
  bool _started = false;

  void startFTP()
  {
    String user = "esp32";
    String pass = "esp32";
    getFTPServ()->begin(user, pass);
    lcd.setTextColor(0xFFFF, 0);
    lcd.setCursor(0,60);
    lcd.setTextFont(1);
    lcd.setTextSize(2);
    lcd.println("host : " + WiFi.localIP().toString());
    lcd.println("user : " + user);
    lcd.println("pass : " + pass);
    lcd.setTextSize(1);
    lcd.setTextFont(2);
    lcd.print("\r\nftp://" + user + ":" + pass + "@" + WiFi.localIP().toString() + "/");
    _started = true;
  }
  virtual bool isSD() { return false; }
  virtual FtpServer* getFTPServ() = 0;
};
class CBFTPserverSD : public CBFTPserver
{
  FtpServer ftpSrv;
  bool isSD() override { return true; }
  FtpServer* getFTPServ() {
    return &ftpSrv;
  }
};
class CBFTPserverSPIFFS : public CBFTPserver
{
  FtpServerSPIFFS ftpSrv;
  FtpServer* getFTPServ() {
    return &ftpSrv;
  }
};
#endif
