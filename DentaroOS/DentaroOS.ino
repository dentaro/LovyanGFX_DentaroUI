#include <LovyanGFX_DentaroUI.hpp>
static LGFX lcd;
LovyanGFX_DentaroUI ui;
static LGFX_Sprite ui_sprite0( &lcd );//円形ボタン用

#include <vector>
#include <M5Stack.h>
#include <M5StackUpdater.h>     //lcd渡せていないので後で修正// https://github.com/tobozo/M5Stack-SD-Updater/
#include <M5TreeView.h>         // https://github.com/lovyan03/M5Stack_TreeView/
#include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/
#include <MenuItemSD.h>
#include <MenuItemSPIFFS.h>
#include <MenuItemToggle.h>
#include <MenuItemWiFiClient.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <esp_partition.h>
#include <nvs_flash.h>

void cleanup();

#include "src/MenuItemSDUpdater.h"
#include "src/Header.h"
#include "src/SystemInfo.h"
#include "src/I2CScanner.h"
//  #include "src/WiFiWPS.h"
#include "src/BinaryViewer.h"
#include "src/CBImageViewer.h"
#include "src/CBFTPserver.h"
#include "src/CBArduinoOTA.h"
#include "src/CBSDUpdater.h"
#include "src/CBFSBench.h"
#include "src/CBWiFiSetting.h"
#include "src/CBWiFiSmartConfig.h"
#include "src/GlobalParams.h"

//======================================================================//
typedef std::vector<MenuItem*> vmi;
M5TreeView treeView;
M5OnScreenKeyboard osk;
constexpr uint8_t NEOPIXEL_pin = 15;
int ui_x = 35;
int ui_y = 150;
int rotateNum = 0;
int preRotateNum = 0;
uint8_t loopcnt = 0xF;
long lastctrl = millis();
int touchDiffAngle = 0;//タッチした時から現在までの角度差
int touchStartUnitAngle = 0;//タッチした時の角度
int touchStartAngle = 0;//タッチした時の角度
int unitAngle = 10;//20度ごとにイベントを発生させる
int modUnitAngle = 0;

void setup() {
  lcd.init();
  ui.begin( lcd, 5, 16, 3, true );//lcd, 色深度,回転方向,タッチキャリブレーション

  //円形Uiを格納できる一番大きなスプライトを用意
  ui_sprite0.setPsram(USE_PSRAM);
  ui_sprite0.setColorDepth( COL_DEPTH );
  ui_sprite0.createSprite( 170, 170 );
  //円形ボタンをセット
  ui.createOBtns( 40, 85, 0, 1, TOUCH );//外半径r0、内半径r1、スタート角（右から時計回り）、分割数、再描画イベント(位置は描画の時に決める)
  ui.createOBtns( 1, 40, 0, 1, MULTI_EVENT );

//  ui.createBtns(220, 100,  100, 120, 2, 2, ui_sprite0, TOUCH );
//  ui.setBtnName(0, "x");
//  ui.setBtnName(1, "^");
//  ui.setBtnName(2, "o");
//  ui.setBtnName(3, "v");

    M5.begin();
    M5.Power.begin();

// #ifdef ARDUINO_ODROID_ESP32
//   M5.battery.begin();
// #else
  M5.Speaker.begin();
  M5.Speaker.mute();
//   Wire.begin();
// #endif
// // for M5GO Bottom LED off
  // pinMode(NEOPIXEL_pin, OUTPUT);
  // setNeoPixelAll(0);

  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *nextupdate = esp_ota_get_next_update_partition(NULL);
  const char* menubinfilename PROGMEM {MENU_BIN} ;
  if (!nextupdate)
  {
    lcd.setTextFont(4);
    lcd.print("! WARNING !\r\nNo OTA partition.\r\nCan't use SD-Updater.");
    delay(3000);
  }
  else if (running && running->label[3] == '0' && nextupdate->label[3] == '1') 
  {
    lcd.setTextFont(2);
    lcd.println("LovyanLauncher on app0");
    size_t sksize = ESP.getSketchSize();
    if (!comparePartition(running, nextupdate, sksize))
    {
      bool flgSD = SD.begin( TFCARD_CS_PIN, SPI, 40000000);
      lcd.print(" copy to app1");
      File dst;
      if (flgSD) {
        dst = (SD.open(menubinfilename, FILE_WRITE ));
        lcd.print(" and SD menu.bin");
      }
      if (copyPartition( flgSD ? &dst : NULL, nextupdate, running, sksize)) {
        lcd.println("\r\nDone.");
      }
      if (flgSD) dst.close();
    }
    SDUpdater::updateNVS();      
    lcd.println("Rebooting app1...");
    if (Update.canRollBack()) {
      Update.rollBack();
      ESP.restart();
    }
  }

  lcd.fillScreen(0);
  // M5ButtonDrawer::width = 106;
  treeView.clientRect.x = 2;
  treeView.clientRect.y = 16;
  treeView.clientRect.w = 236;
  treeView.clientRect.h = 130;
  treeView.itemWidth = 216;

  treeView.useFACES       = true;
  treeView.useCardKB      = true;
  treeView.useJoyStick    = true;
  treeView.usePLUSEncoder = true;
  treeView.useFACESEncoder= true;
  
treeView.setItems(vmi
               { 
                  new MenuItem("WiFi", vmi
                  { new MenuItemWiFiClient("WiFi Client", callBackWiFiClient)
                  , new MenuItem("WiFi Off", callBackWiFiOff)
                  } )

                 , new MenuItem("Tools", vmi
                 { new MenuItem("System Info", callBackExec<SystemInfo>)
                 , new MenuItem("I2C Scanner", callBackExec<I2CScanner>)
                 , new MenuItem("FTP Server (SDCard)", callBackExec<CBFTPserverSD>)
                 , new MenuItem("FTP Server (SPIFFS)", callBackExec<CBFTPserverSPIFFS>)
                 , new MenuItem("Benchmark (SDCard)", callBackExec<CBFSBenchSD>)
                 , new MenuItem("Benchmark (SPIFFS)", callBackExec<CBFSBenchSPIFFS>)
                 , new MenuItem("Format SPIFFS", vmi
                   { new MenuItem("Format Execute", callBackFormatSPIFFS)
                   } )
                 , new MenuItem("Erase NVS(Preferences)", vmi
                   { new MenuItem("Erase Execute", callBackFormatNVS)
                   } )
                 , new MenuItem("Style ", callBackStyle, vmi
                   { new MenuItem("FreeSans9pt7b", 2)
                   , new MenuItem("Font 2" , 1)
                   , new MenuItem("Font 1", 0)
                   } )

                  } )

                , new MenuItem("Binary Viewer", vmi
                 { new MenuItemSD(    "SDCard", callBackExec<BinaryViewerFS>)
                 , new MenuItemSPIFFS("SPIFFS", callBackExec<BinaryViewerFS>)
                 , new MenuItem("FLASH", vmi
                   { new MenuItem("2nd boot loader", 0, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("partition table", 1, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("nvs",         0x102, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("otadata",     0x100, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("app0",        0x010, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("app1",        0x011, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("eeprom",      0x199, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("spiffs",      0x182, callBackExec<BinaryViewerFlash>)
                   } )
                 } )

                 , new MenuItem("Image Viewer", vmi
                 { new MenuItemSD(    "SDCard", callBackExec<CBImageViewer>)
                 , new MenuItemSPIFFS("SPIFFS", callBackExec<CBImageViewer>)
                 } )

#ifndef ARDUINO_ODROID_ESP32
               , new MenuItem("Power", vmi
                 {
                //    new MenuItemToggle("BatteryCharge" , getIP5306REG(0) & 0x10, 0x10, callBackBatteryIP5306CTL0)
                //  , new MenuItemToggle("BatteryOutput" , getIP5306REG(0) & 0x20, 0x20, callBackBatteryIP5306CTL0)
                //  , new MenuItemToggle("Boot on load"  , getIP5306REG(0) & 0x04, 0x04, callBackBatteryIP5306CTL0)
                   new MenuItemToggle("M5GO Bottom LED", false, callBackFIRELED)
                 , new MenuItem("DeepSleep", callBackDeepSleep)
                 })
#endif
               , new MenuItem("OTA", vmi
                 { new MenuItem("Arduino OTA", callBackExec<CBArduinoOTA>)
                 , new MenuItem("OTA Rollback", vmi
                   { new MenuItem("Rollback Execute", callBackRollBack)
                   } )
                 } )
               } );

  treeView.setLcd(lcd);
  treeView.begin(lcd);
}                                                         // Construct a triangle with (x1,y1) (x2,y2) (x3,y3) as its vertices

void loop(){
  ui.update( lcd );
  if (NULL != treeView.update(lcd)) {
    lastctrl = millis();
  }
  if (treeView.isRedraw()) {
    drawFrame();
    loopcnt = 0xF;
  }
  if (0 == (++loopcnt & 0xF)) {
      Header.draw();//中身を修正
  #ifndef ARDUINO_ODROID_ESP32
      if ( 600000 < millis() - lastctrl ) {
        Serial.println( "goto sleep" );
        callBackDeepSleep(NULL);
      }
  #endif
    }

  if( ui.getEvent() != NO_EVENT ){//何かイベントが起きたら
    ui.updateOBtnSlider(ui.getUiID("BTN_0"), ui_sprite0, ui_x, ui_y );
    if( ui.getEvent() == TOUCH )
    {
      modUnitAngle = ui.getCurrentAngle(ui.getUiID("BTN_0")) % unitAngle;
      touchStartUnitAngle = ui.getCurrentAngle(ui.getUiID("BTN_0")) - modUnitAngle;
      touchStartAngle = ui.getCurrentAngle(ui.getUiID("BTN_0"));
    }
    //treeView.setBtnID(int no)はtreeviewクラスのcheckInputUI()関数で実行されます。
    //  TREE_EVENT_NONE  0
    //  TREE_EVENT_BACK  1
    //  TREE_EVENT_NEXT  2
    //  TREE_EVENT_PREV  3
    //  TREE_EVENT_HOLD  4
    //  TREE_EVENT_ENTER 5
    //で定義されています。
    if( ui.getEventBit(TAPPED) ){
      if(ui.getTouchBtnID() == 1)treeView.setBtnID(TREE_EVENT_ENTER);//開く
    }
    else if( ui.getEventBit(UP_FLICK) ){
      if(ui.getTouchBtnID() == 1)treeView.setBtnID(TREE_EVENT_BACK);//外円 UP_FLICKで閉じる
    }
    if( ui.getEvent() == RELEASE ){
      ui.resetEventBits();
    }

    touchDiffAngle = touchStartAngle - ui.getCurrentAngle(ui.getUiID("BTN_0"));//タッチ時の単位角から現在の角度の差を調べる
    if(abs(touchDiffAngle) > unitAngle){//タッチ時の単位角から現在の角度までの差が、１単位角を超えたら

      Serial.print(touchStartUnitAngle);
      Serial.print(":");
      Serial.print(touchDiffAngle);
      Serial.println("");

            if(touchDiffAngle>0){treeView.setBtnID(TREE_EVENT_NEXT);}
      else if(touchDiffAngle<0){treeView.setBtnID(TREE_EVENT_PREV);}
      modUnitAngle = ui.getCurrentAngle(ui.getUiID("BTN_0")) % unitAngle;
      touchStartAngle = ui.getCurrentAngle(ui.getUiID("BTN_0"));
      // touchStartUnitAngle = ui.getCurrentAngle(ui.getUiID("BTN_0")) - (ui.getCurrentAngle(ui.getUiID("BTN_0")%unitAngle));//タッチ角に近い単位角を登録
      // touchStartUnitAngle = ui.getPreAngle(ui.getUiID("BTN_0")) - (ui.getPreAngle(ui.getUiID("BTN_0")%unitAngle));//タッチ角に近い単位角を登録
    }
    //ui_sprite0スプライトに描画
    ui_sprite0.setColor(TFT_BLUE);//円形ボタンは1分割の時だけボタンの色変えられます(分割すると白)
    ui_sprite0.setTextColor(TFT_BLACK);//ボタン名の色
    ui.drawOBtns( ui.getUiID("BTN_0"), lcd, ui_sprite0, ui_x, ui_y );

    ui_sprite0.setColor(TFT_RED);//円形ボタンは1分割の時だけボタンの色変えられます(分割すると白)
    ui_sprite0.setTextColor(TFT_BLACK);//ボタン名の色
    ui.drawOBtns( ui.getUiID("BTN_1"), lcd, ui_sprite0, ui_x, ui_y );

    ui_sprite0.pushSprite( ui_x, ui_y );//一つのスプライトui_sprite0にまとめて出力
  }
  ui.showTouchEventInfo( lcd, lcd.width() - 50, 0 );//タッチイベントを視覚化する
  ui.showInfo( lcd ,0, 48 );//ボタン情報、フレームレート情報などを表示します。
  delay(1);
}

void callBackDeepSleep(MenuItem* sender)
{
  lcd.setBrightness(0);
  lcd.sleep();
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_B_PIN, LOW);
  esp_deep_sleep_start();
}

void callBackWiFiClient(MenuItem* sender)
{
  MenuItemWiFiClient* mi = static_cast<MenuItemWiFiClient*>(sender);
  if (!mi) return;

  if (mi->ssid == "") return;

  Preferences preferences;
  preferences.begin("wifi-config");
  preferences.putString("WIFI_SSID", mi->ssid);
  String wifi_passwd = preferences.getString(mi->ssid.c_str());
  if (wifi_passwd == "") wifi_passwd = preferences.getString("WIFI_PASSWD");

  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  if (mi->auth != WIFI_AUTH_OPEN) {
    osk.setup(wifi_passwd);
    while (osk.loop()) { delay(1); }
    wifi_passwd = osk.getString();
    osk.close();
    WiFi.begin(mi->ssid.c_str(), wifi_passwd.c_str());
    preferences.putString("WIFI_PASSWD", wifi_passwd);
    preferences.putString(mi->ssid.c_str(), wifi_passwd);
  } else {
    WiFi.begin(mi->ssid.c_str(), "");
    preferences.putString("WIFI_PASSWD", "");
  }
  preferences.end();
  while (M5.BtnA.isPressed()) M5.update();
}

void callBackWiFiOff(MenuItem* sender)
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(true);
}

void callBackFormatSPIFFS(MenuItem* sender)
{
  lcd.fillRect(20, 100, 160, 30, 0);
  lcd.drawRect(23, 103, 154, 24, 0xFFFF);
  lcd.setTextFont(0);
  lcd.setTextColor(0xFFFF, 0);
  lcd.drawCentreString("SPIFFS Format...", 90, 106, &fonts::Font2);
  SPIFFS.begin();
  SPIFFS.format();
  SPIFFS.end();
}

const char* preferKeyIP5306 PROGMEM( "IP5306CTL0" );
const char* preferKeyStyle  PROGMEM( "TVStyle" );
void drawFrame() {
  Rect16 r = treeView.clientRect;
  r.inflate(1);
  lcd.drawRect(r.x -1, r.y, r.w +2, r.h, MenuItem::frameColor[1]);
  lcd.drawRect(r.x, r.y -1, r.w, r.h +2, MenuItem::frameColor[1]);
  // lcd.setTextFont(0);
  // lcd.setTextColor(0x8410,0);
  // lcd.drawString("- LovyanLauncher -", 207, 191);
  // lcd.drawString("@lovyan03    v0.2.3", 204, 201);
  // lcd.drawString("http://git.io/fhdJV", 204, 211);
  // lcd.drawString("- LovyanLauncher -", 207, 191, &fonts::Font2);
  // lcd.drawString("@lovyan03    v0.2.3", 204, 201, &fonts::Font2);
  // lcd.drawString("http://git.io/fhdJV", 204, 211, &fonts::Font2);
}

void setStyle(int tag)
{
  switch (tag) {
  default: return;

  // lcd.setTextColor(0xFFFF00U);//フォントの色を指定
  lcd.setTextColor(0x00FFFFU, 0xFF0000U);
  lcd.setFont(&fonts::Font4);//フォントを指定
  // lcd.setFont(&fonts::lgfxJapanGothic_8);//フォントを指定

  case 0:
    // M5ButtonDrawer::height = 14;
    // M5ButtonDrawer::setTextFont(1);
    // treeView.setTextFont(1);
    treeView.itemHeight = 18;
    osk.keyHeight = 14;
    // osk.setTextFont(1);
    break;

  case 1:
    // M5ButtonDrawer::height = 18;
    // M5ButtonDrawer::setTextFont(2);
    // treeView.setTextFont(2);
    treeView.itemHeight = 20;
    osk.keyHeight = 18;
    // osk.setTextFont(2);
    break;

  case 2:
    // M5ButtonDrawer::height = 18;
    // M5ButtonDrawer::setTextFont(2);
    // treeView.setFreeFont(&FreeSans9pt7b);
    // treeView.setTextFont(2);
    treeView.itemHeight = 24;
    osk.keyHeight = 18;
    // osk.setTextFont(2);
    break;
  }
  treeView.updateDest();
  lcd.fillRect(0, 218, lcd.width(), 22, 0);
}

void callBackStyle(MenuItem* sender)
{
  setStyle(sender->tag);
  Preferences p;
  p.begin(GlobalParams::preferName);
  p.putUChar(preferKeyStyle, sender->tag);
  p.end();
}

void callBackFormatNVS(MenuItem* sender)
{
  lcd.fillRect(20, 100, 160, 30, 0);
  lcd.drawRect(23, 103, 154, 24, 0xFFFF);
  lcd.setTextFont(0);
  lcd.setTextColor(0xFFFF, 0);
  lcd.drawCentreString("NVS erase...", 90, 106, &fonts::Font2);
  nvs_flash_init();
  nvs_flash_erase();
  nvs_flash_deinit();
  nvs_flash_init();
  delay(1000);
}

uint8_t getIP5306REG(uint8_t reg, uint8_t defaultValue = 0)
{
  Wire.beginTransmission(0x75);
  Wire.write(reg);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(0x75, 1)) {
    return Wire.read();
  }
  return defaultValue;
}

void setIP5306REG(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(0x75);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void callBackBatteryIP5306CTL0(MenuItem* sender)
{
  MenuItemToggle* mi((MenuItemToggle*)sender); 
  uint8_t data = getIP5306REG(0, 0x35);
  data = mi->value ? (data | mi->tag) : (data & ~(mi->tag));
  Preferences p;
  p.begin(GlobalParams::preferName);
  p.putUChar(GlobalParams::preferName, data);
  p.end();
  setIP5306REG(0, data);
}

void sendNeoPixelBit(bool flg) {
  for (uint8_t i = 0; i < 2; ++i) digitalWrite(NEOPIXEL_pin, HIGH);
  for (uint8_t i = 0; i < 6; ++i) digitalWrite(NEOPIXEL_pin, flg);
  for (uint8_t i = 0; i < 3; ++i) digitalWrite(NEOPIXEL_pin, LOW);
}
void sendNeoPixelColor(uint32_t color) { // 24bit GRB
  for (uint8_t i = 0; i < 24; ++i) {
    sendNeoPixelBit(color & 0x800000);
    color = color << 1;
  }
}
void setNeoPixelAll(uint32_t color) {
  sendNeoPixelBit(false);
  delay(1);
  for (uint8_t i = 0; i < 10; ++i) {
    sendNeoPixelColor(color);
  }
}
void callBackFIRELED(MenuItem* sender)
{
  MenuItemToggle* mi((MenuItemToggle*)sender); 
  setNeoPixelAll(mi->value ? 0x555555 : 0);
}

void callBackRollBack(MenuItem* sender)
{
  if( Update.canRollBack() )  {
    cleanup();
    Update.rollBack();
    ESP.restart();
  }
}

template <class T>
void callBackExec(MenuItem* sender)
{
  T menucallback;
  menucallback(sender);
}

bool comparePartition(const esp_partition_t* src1, const esp_partition_t* src2, size_t length)
{
    size_t lengthLeft = length;
    const size_t bufSize = SPI_FLASH_SEC_SIZE;
    std::unique_ptr<uint8_t[]> buf1(new uint8_t[bufSize]);
    std::unique_ptr<uint8_t[]> buf2(new uint8_t[bufSize]);
    uint32_t offset = 0;
    size_t i;
    while( lengthLeft > 0) {
      size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
      if (!ESP.flashRead(src1->address + offset, reinterpret_cast<uint32_t*>(buf1.get()), (readBytes + 3) & ~3)
       || !ESP.flashRead(src2->address + offset, reinterpret_cast<uint32_t*>(buf2.get()), (readBytes + 3) & ~3)) {
          return false;
      }
      for (i = 0; i < readBytes; ++i) if (buf1[i] != buf2[i]) return false;
      lengthLeft -= readBytes;
      offset += readBytes;
    }
    return true;
}

bool copyPartition(File* fs, const esp_partition_t* dst, const esp_partition_t* src, size_t length)
{
    lcd.fillRect( 110, 112, 100, 20, 0);
    size_t lengthLeft = length;
    const size_t bufSize = SPI_FLASH_SEC_SIZE;
    std::unique_ptr<uint8_t[]> buf(new uint8_t[bufSize]);
    uint32_t offset = 0;
    uint32_t progress = 0, progressOld = 0;
    while( lengthLeft > 0) {
      size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
      if (!ESP.flashRead(src->address + offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)
       || !ESP.flashEraseSector((dst->address + offset) / bufSize)
       || !ESP.flashWrite(dst->address + offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)) {
          return false;
      }
      if (fs) fs->write(buf.get(), (readBytes + 3) & ~3);
      lengthLeft -= readBytes;
      offset += readBytes;
      progress = 100 * offset / length;
      if (progressOld != progress) {
        progressOld = progress;
        //lcd.progressBar( 110, 112, 100, 20, progress);
      }
    }
    return true;
}

void cleanup() {
  M5.Speaker.end();
}

//onEnter
//onAfterDraw
//LGFX &lcd
