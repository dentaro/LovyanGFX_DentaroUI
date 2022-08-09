#ifndef _CBFSBENCH_H_
#define _CBFSBENCH_H_

#include <SPIFFS.h>
#include <SD.h>
#include "MenuCallBack.h"
#include "Header.h"

class CBFSBench : public MenuCallBack
{
  String tmpFile{"/lovyanLauncherBench"};
public:
  bool setup() 
  {
    Header.draw();
    lcd.setTextFont(0);
    lcd.setTextColor(0xFFFF);
    fs::FS& fs(getFS());
    for (int i = 1; i < 16; ++i) {
      lcd.drawFastHLine(0, 10 + i, lcd.width(), (&fs == &SD) ? (i << 1) : (i << 6));
    }
    lcd.drawString(menuItem->title, 10, 10, &fonts::Font2);
    showFSInfo();
    return true;
  }

  bool loop()
  {
    if (cmd == M5TreeView::eCmd::ENTER) {
#ifndef ARDUINO_ODROID_ESP32
      btnDrawer.setText(1, "");
      btnDrawer.draw();
#endif
      ExecBench();
    } else {
      btnDrawer.setText(1, "Execute");
      Header.draw();
      delay(100);
    }
    return true;
  }

protected:
  void showFSInfo() {
    init();
    lcd.setTextFont(0);
    lcd.setTextColor(0xFFFF, 0);
    lcd.setCursor(0, 30);

    lcd.println("total:" + getStrSize(totalBytes()));
    lcd.println("used :" + getStrSize(usedBytes()));
    lcd.println("free :" + getStrSize(totalBytes() - usedBytes()));
  }

  String getStrSize(uint64_t value) {
    int base = 0;
    for (; base < 3 && (value >= (100 * 1024)); ++base) {
      value = value >> 10;
    }
    String space = "";
    int dig = (value == 0) ? 0 : log10(value);
    for (int i = 6 - dig; i > 0; --i) space += " ";
    return space + String((long)value, DEC)
         + ((base == 3) ? " GiB"
          : (base == 2) ? " MiB"
          : (base == 1) ? " KiB"
          : (base == 0) ? " Byte"
                        : " ??");
  }

  void ExecBench() {
    lcd.fillRect(0, 30, lcd.width(), 192, 0);
    showFSInfo();
    lcd.setTextFont(2);

    lcd.drawRect(10, 59, 300, 103, 0x8410);
    lcd.setTextColor(0x8410);
    lcd.drawRightString("FileSize",   100, 60, &fonts::Font2);
    lcd.drawRightString("WriteSpeed", 200, 60, &fonts::Font2);
    lcd.drawRightString("ReadSpeed",  300, 60, &fonts::Font2);
    lcd.setTextColor(0xFFFF);
    for ( int i = 0; i < 5; ++i ) {
      lcd.drawFastHLine(11, 76 + i * 17, 298, 0x4208);
      drawResult( 77 + i * 17, tmpFile,  4 << i,  8 - i);
    }

    lcd.drawRect(10, 162, 300, 35, 0x8410);
    lcd.drawFastHLine(11, 179, 298, 0x4208);
    lcd.setTextColor(0x8410);
    lcd.drawRightString("0ByteCreate", 100, 163, &fonts::Font2);
    lcd.drawRightString("OpenClose",   200, 163, &fonts::Font2);
    lcd.drawRightString("Remove",      300, 163, &fonts::Font2);
    lcd.setTextColor(0xFFFF);

    int loop = 20;
    lcd.drawRightString(String(int((uint64_t)1000000 / getCreateTime(   getFS(), tmpFile, loop)), DEC) + " count/s", 100, 180, &fonts::Font2);
    lcd.drawRightString(String(int((uint64_t)1000000 / getOpenCloseTime(getFS(), tmpFile, loop)), DEC) + " count/s", 200, 180, &fonts::Font2);
    lcd.drawRightString(String(int((uint64_t)1000000 / getRemoveTime(   getFS(), tmpFile, loop)), DEC) + " count/s", 300, 180, &fonts::Font2);
    lcd.drawString("Complete.", 10, 200, &fonts::Font2);
  }

  void drawResult(int y, const String& filepath, int kib, int loop)
  {
    lcd.drawRightString(getStrSize(1024 * kib), 100, y, &fonts::Font2);
    lcd.drawRightString(String(int(1000000 * kib / getWriteTime(getFS(), filepath, 1024 * kib, loop)), DEC) + " KiB/s", 200, y, &fonts::Font2);
    lcd.drawRightString(String(int(1000000 * kib / getReadTime( getFS(), filepath, 1024 * kib, loop)), DEC) + " KiB/s", 300, y, &fonts::Font2);
  }


  uint64_t getWriteTime(fs::FS& fs, const String& filepath, long filesize, int loop) volatile {
    File f;
    uint64_t res = 0;
    uint8_t buf[SPI_FLASH_SEC_SIZE];
    for (int i = 0; i < SPI_FLASH_SEC_SIZE; ++i) { buf[i] = uint8_t(i & 0xFF); }
    for (int i = 0; i < loop; ++i) {
      long rest = filesize;
      uint64_t start = micros();
      f = fs.open(filepath + String(i), FILE_WRITE);
      do {
        long size = rest > SPI_FLASH_SEC_SIZE ? SPI_FLASH_SEC_SIZE : rest;
        f.write(&buf[0], size);
        rest -= size;
      } while (rest > 0);
      f.flush();
      f.close();
      res += micros() - start;
    }
    return res / loop;
  }

  uint64_t getReadTime(fs::FS& fs, const String& filepath, long filesize, int loop) volatile {
    File f;
    uint64_t res = 0;
    uint8_t buf[SPI_FLASH_SEC_SIZE];
    for ( int i = 0; i < loop; ++i) {
      long rest = filesize;
      uint64_t start = micros();
      f = fs.open(filepath + String(i), FILE_READ);
      do {
        long size = rest > SPI_FLASH_SEC_SIZE ? SPI_FLASH_SEC_SIZE : rest;
        f.read(&buf[0], size);
        rest -= size;
      } while (rest > 0);
      f.close();
      res += micros() - start;
      fs.remove(filepath + String(i));
    }
    return res / loop;
  }

  uint64_t getCreateTime(fs::FS& fs, const String& filepath, int loop) volatile {
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      fs.open(filepath + String(i), FILE_WRITE).close();
    }
    return (micros() - start) / loop;
  }

  uint64_t getOpenCloseTime(fs::FS& fs, const String& filepath, int loop) volatile {
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      fs.open(filepath + String(i), FILE_READ).close();
    }
    return (micros() - start) / loop;
  }

  uint64_t getRemoveTime(fs::FS& fs, const String& filepath, int loop) volatile {
    uint64_t start = micros();
    for (int i = 0; i < loop; ++i) {
      fs.remove(filepath + String(i));
    }
    return (micros() - start) / loop;
  }

private:
  virtual fs::FS& getFS() = 0;
  virtual void init() = 0;
  virtual uint64_t totalBytes() { return 0; }
  virtual uint64_t usedBytes()  { return 0; }
};

class CBFSBenchSD : public CBFSBench
{
  fs::FS& getFS() { return SD; }
  uint64_t totalBytes() { return SD.totalBytes(); }
  uint64_t usedBytes()  { return SD.usedBytes(); }
  void init()
  {
    SD.end();
    SD.begin( TFCARD_CS_PIN, SPI, 40000000);
  }
};

class CBFSBenchSPIFFS : public CBFSBench
{
public:
  bool setup() 
  {
    lcd.setTextFont(0);
    lcd.setTextColor(0xFFFF);
    lcd.setCursor(0, 30);
    lcd.print("Format Check...\r\nWait a minute...");
    return CBFSBench::setup();
  }

private:
  fs::FS& getFS() { return SPIFFS; }
  uint64_t totalBytes() { return SPIFFS.totalBytes(); }
  uint64_t usedBytes()  { return SPIFFS.usedBytes(); }
  void init()
  {
    SPIFFS.begin(true);
  }
};
#endif
