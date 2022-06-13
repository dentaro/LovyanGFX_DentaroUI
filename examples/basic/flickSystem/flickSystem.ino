#include <LovyanGFX_DentaroUI.hpp>
LGFX lcd;
//LovyanGFX_DentaroUI ui( &lcd );
LovyanGFX_DentaroUI ui;

static LGFX_Sprite layoutSprite( &lcd );//レイアウト用
static LGFX_Sprite ui_sprite0( &layoutSprite );//フリック用
static LGFX_Sprite ui_sprite1( &layoutSprite );//左ボタン用
static LGFX_Sprite ui_sprite2( &layoutSprite );//右ボタン用
static LGFX_Sprite ui_sprite3( &layoutSprite );//上ボタン用

static LGFX_Sprite flickUiSprite;//フリックパネル用

void setup() 
{
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( lcd, SHIFT_NUM, 16, 3, true );//lcd, フリックパネル, shiftの数, 色深度, 回転方向,タッチキャリブレーション//アドレス渡し
  ui.flickSetup( layoutSprite, ui_sprite0, ui_sprite1, ui_sprite2, ui_sprite3 ,flickUiSprite);//flickでBTN_4まで使います。//アドレス渡し
  //これ以降、ボタンを配置する場合はBTN_5となります。
}
void loop( void ){
   ui.update( lcd );//参照渡し
   ui.flickUpdate( lcd, layoutSprite, ui_sprite0, ui_sprite1, ui_sprite2, ui_sprite3 ,flickUiSprite);//参照渡し
   
   if( ui.getEvent() == RELEASE ){
     lcd.fillRect(0, 20, 240, 60, TFT_BLACK);
     lcd.setTextSize(1);
     lcd.setTextColor(TFT_WHITE, TFT_BLACK);
     lcd.setCursor(0,20);
     lcd.setFont(&lgfxJapanGothicP_20);
     lcd.setTextWrap(true);
     lcd.setTextScroll(true);
     lcd.printf( ui.getFlickString().c_str() );
    //lcd.printf( ui.getFlickString(INVISIBLE).c_str() );//インビジブルモード（暗号化していません。）
    //lcd.drawString( ui.getFlickChar(), 150,0 );
    
   }
  ui.showTouchEventInfo( &lcd, lcd.width() - 50, 0 );//タッチイベントを視覚化する//ポインタ渡し
  ui.showInfo( &lcd , 0, 48 );//ボタン情報、フレームレート情報などを表示します。//ポインタ渡し
  delay(1);
}
