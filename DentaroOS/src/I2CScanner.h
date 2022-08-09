#ifndef _I2CSCANNER_H_
#define _I2CSCANNER_H_

#include <vector>
#include <algorithm>
#include "MenuCallBack.h"
#include "Header.h"
#include "I2CRegistry.h"

class I2CScanner : public MenuCallBack
{
public:
  bool setup() 
  {
    lcd.fillScreen(0);
    lcd.setTextColor(TFT_WHITE);
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), i << 1);
    }
    lcd.drawString(menuItem->title, 10, 10, &fonts::Font2);
    btnDrawer.setText("Back/Prev","Ok","Next");
    btnDrawer.draw(true);
    return true;
  }

  bool loop()
  {
    Header.draw();
    lcd.setTextFont(1);
    lcd.setTextSize(1);
    lcd.setTextColor(TFT_WHITE, 0);
    byte dummy = 0;
    uint8_t exist = false;
    std::vector<uint8_t> ex;
    for (uint8_t adr = 0; adr <= 0x7F; adr++) {//0x7F
      int x = (adr % 16) * 20;
      int y = 40 + (adr / 16) * 20;
      lcd.setCursor(4 + x, 6 + y);
      // Wire.beginTransmission(adr);
      // Wire.write(&dummy, 0);
      // exist = Wire.endTransmission() == 0;
      if (exist) ex.push_back(adr);
      uint16_t color = exist ? 0xFFFF : 0x39E7;
      lcd.setTextColor(color, 0);
      if (exist && _addr == adr) {
        color = 0x421F;
      }
      lcd.drawRect(x,y,19,19, color);
      lcd.printf("%02X", adr);
    }
    delay(10);

    uint8_t idx = std::distance(ex.begin(), std::lower_bound(ex.begin(), ex.end(), _addr));
    if (cmd != M5TreeView::eCmd::NONE) {
      if (cmd == M5TreeView::eCmd::ENTER && idx < ex.size() && ex[idx] == _addr) {
        I2CRegistry re;
        re.addr = _addr;
        re(menuItem);
        setup();
        return true;
      } else if (cmd == M5TreeView::eCmd::NEXT) {
        idx++;
        if (idx >= ex.size()) idx = 0;
      } else if (cmd == M5TreeView::eCmd::PREV) {
        idx--;
        if (idx >= ex.size()) idx = ex.size() - 1;
      }
    }
    if (idx < ex.size()) _addr = ex[idx];

    return true;
  }

private:
  uint8_t _addr = 0;
};

#endif
