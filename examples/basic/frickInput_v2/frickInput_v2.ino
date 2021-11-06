#include <LovyanGFX_DentaroUI.hpp>
LGFX lcd;
LovyanGFX_DentaroUI ui( &lcd );
static LGFX_Sprite layoutSprite( &lcd );//レイアウト用
static LGFX_Sprite ui_sprite0( &layoutSprite );//フリック用
static LGFX_Sprite ui_sprite1( &layoutSprite );//左ボタン用
static LGFX_Sprite ui_sprite2( &layoutSprite );//右ボタン用
static LGFX_Sprite flickUiSprite;//フリックパネル用
void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( &lcd, flickUiSprite, SHIFT_NUM, 24, 0, true );//lcd, フリックパネル, shiftの数, 色深度, 回転方向,タッチキャリブレーション
  ui.createLayout( 0, 80, 240, 240, layoutSprite, MULTI_EVENT );
  ui.setFlickPanels();//フリックキーボード用プリセット
  ui.createFlicks( 48, 48,  144, 192, 3, 4, ui_sprite0, MULTI_EVENT );
  flickUiSprite.createSprite( 144, 144 );
  ui.createBtns( 0,   48,  48, 192, 1, 4, ui_sprite1, MULTI_EVENT );
  ui.createBtns( 192, 48,  48, 192, 1, 4, ui_sprite2, MULTI_EVENT );
  ui.setFlick( JP );//立ち上げ時のパネル指定　JP/EN/NUMERIC
}
void loop( void ){
  ui.update( &lcd );
  ui.flickUpdate( &lcd ,layoutSprite, ui_sprite0, ui_sprite1, ui_sprite2 ,flickUiSprite);
  if( ui.getEvent() == RELEASE ){
    lcd.fillRect(0, 28, 240, 100, TFT_BLACK);
    lcd.setTextSize(1);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.setCursor(0,30);
    lcd.setFont(&lgfxJapanGothicP_20);
    lcd.setTextWrap(true);
    lcd.setTextScroll(true);
    lcd.printf( ui.getFlickString().c_str() );
//    lcd.printf( ui.getFlickString(INVISIBLE).c_str() );//インビジブルモード（暗号化していません。）
    lcd.drawString( ui.getFlickChar(), 150,0 );
  }
  ui.showTouchEventInfo( &lcd, lcd.width() - 50, 0 );//タッチイベントを視覚化する
  ui.showInfo( &lcd );//ボタン情報、フレームレート情報などを表示します。
  delay(1);
}
   