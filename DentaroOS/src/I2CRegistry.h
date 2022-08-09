#ifndef _I2CREGISTRY_H_
#define _I2CREGISTRY_H_

#include "Header.h"

class I2CRegistry
{
public:
  uint8_t addr = 0;
  uint8_t regMax = 0x80;

  void operator()(MenuItem* mi) {
    menuItem = mi;
    treeView = ((M5TreeView*)(mi->topItem()));
    lcd.fillScreen(0);
    btnDrawer.setText("Back","Mode","Mode");
    btnDrawer.draw(true);
    if (setup()) {
      while (loop());
      close();
      lcd.fillScreen(MenuItem::backgroundColor);
    }
  }

  bool setup() 
  {
    lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), i << 1);
    }
    lcd.drawString(getTitle(), 10, 10, &fonts::Font2);

    lcd.setTextFont(1);
    lcd.setTextSize(1);
    lcd.setTextColor(0xFF00, 0);

    for (byte i = 0; i < regMax/16; ++i) {
      lcd.setCursor(2, 42 + i*14);
      lcd.printf("%01Xx", i);
    }
    for (byte i = 0; i < 0x10; ++i) {
      lcd.setCursor(20+i*19, 30);
      lcd.printf("x%01X", i);
    }
    step = 0;

    return true;
  }

  bool loop()
  {
    M5.update();
    if (M5.BtnA.wasReleased()) return false;
    if (M5.BtnB.wasReleased() || M5.BtnC.wasReleased() || step == 0) {
      step = M5.BtnB.wasReleased() ? step << 1 : step >> 1;
      if (step == 0) step = 16;
      else if (step >= 32) step = 1;
      lcd.setTextFont(2);
      lcd.setTextSize(1);
      lcd.setCursor(32, 160);
      lcd.setTextColor(0xFFFF, 0);
      lcd.printf("%2d Byte read mode ", step);
    }
    Header.draw();
    btnDrawer.draw();

    lcd.setTextFont(1);
    lcd.setTextSize(1);
    bool enabled = false;
    for (byte row = 0; row < regMax/16; ++row) {
      for (byte col = 0; col < 16; ++col) {
        if ((col % step) == 0) {
          Wire.beginTransmission(addr);
          Wire.write(row * 16 + col);
          enabled = (Wire.endTransmission(false) == 0 && Wire.requestFrom(addr, (uint8_t)(step ? step:16)));
        }
        int x = 20 + col * 19;
        int y = 42 + row * 14;
        uint16_t color = 0;
        lcd.setCursor(x, y);

        if (enabled) {
          uint8_t dat = Wire.read();
          lcd.setTextColor((dat ? 0xFFFF : 0xF9E7), color);
          lcd.printf("%02X", dat);
        } else {
          lcd.setTextColor(0xF9E7, color);
          lcd.print("--");
        }
      }
    }

    return true;
  }

  void close()
  {
  }

protected:
  M5ButtonDrawer btnDrawer;
  M5TreeView* treeView;
  MenuItem* menuItem;
  uint8_t step = 0;

  virtual String getTitle() {
    return "I2C 0x" + String(addr,HEX) + " REGISTRY ";
  }
};

#endif
