#include <LovyanGFX_DentaroUI.hpp>
static LGFX lcd;
LovyanGFX_DentaroUI ui;
static LGFX_Sprite ui_sprite0( &lcd );//円形ボタン用
static LGFX_Sprite ui_sprite1( &lcd );//円形ボタン用
int btn_No[4] = {0,2,6,8};//四隅のボタンを消す
String btn_name[9] = { "0", "A", "0", "B", "C", "D", "0", "E", "0" };
int w = 10;
int x,y = 3;
int c = TFT_GREEN;
void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( lcd, 3, 16, 3, true );//lcd, 色深度,回転方向,タッチキャリブレーション
  //中心位置XY、外半径r0、内半径r1、スタート角（右から時計回り）、分割数
  ui.createOBtns( 75,245,  25, 75, 22, 8, ui_sprite0, TOUCH );
  ui.createOBtns( 75,245,  1, 25, 22, 1, ui_sprite1, TOUCH );
  //以下も同じこと
  // ui.setAllBtnAvailableF( ui.getUiID("BTN_0"), false );//一度全部ボタンを無反応に
  // ui.setAllBtnVisibleF(   ui.getUiID("BTN_0"), false );//一度全部ボタンを消す
  // ui.setBtnName( 1, btn_name[1] ); //再描画していく
  // ui.setBtnName( 3, btn_name[3] ); 
  // ui.setBtnName( 4, btn_name[4] );
  // ui.setBtnName( 5, btn_name[5] ); 
  // ui.setBtnName( 7, btn_name[7] ); 
}
void loop( void ){
  ui.update( lcd );
  if( ui.getEvent() != NO_EVENT ){//何かイベントがあれば
    lcd.fillRect(0, 30, 240, 140, TFT_BLUE);
    c = TFT_GREEN;
    if(ui.getTouchBtnID() == 0)     {x++;y++;}
    else if(ui.getTouchBtnID() == 1){    y++;}
    else if(ui.getTouchBtnID() == 2){x--;y++;}
    else if(ui.getTouchBtnID() == 3){x--;    }
    else if(ui.getTouchBtnID() == 4){x--;y--;}
    else if(ui.getTouchBtnID() == 5){    y--;}
    else if(ui.getTouchBtnID() == 6){x++;y--;}
    else if(ui.getTouchBtnID() == 7){x++;    }
    if(ui.getTouchBtnID() == 8)c = TFT_RED;
    if(x > 24)x = 0;if(x < 0)x = 24;
    if(y > 16 )y = 3;if(y < 3)y = 16;
    lcd.fillRect(x*w, y*w, w, w, c);
    
    ui.drawOBtns( ui.getUiID("BTN_0"), lcd, ui_sprite0 );
    ui.drawOBtns( ui.getUiID("BTN_1"), lcd, ui_sprite1 );

  }
  ui.showTouchEventInfo( lcd, lcd.width() - 50, 0 );//タッチイベントを視覚化する
  ui.showInfo( lcd ,0, 48 );//ボタン情報、フレームレート情報などを表示します。
  delay(1);
}
