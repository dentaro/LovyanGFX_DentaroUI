#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX_DentaroUI.hpp>

static LGFX lcd;
LovyanGFX_DentaroUI ui( &lcd );
static LGFX_Sprite layoutSprite( &lcd );//レイアウト用
static LGFX_Sprite ui_sprite0( &layoutSprite );//フリック用
static LGFX_Sprite ui_sprite1( &layoutSprite );//左ボタン用
static LGFX_Sprite ui_sprite2( &layoutSprite );//右ボタン用
static LGFX_Sprite flickUiSprite;//フリックパネル用

void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( &lcd , flickUiSprite, SHIFT_NUM ); //フリックパネルshiftの数
  lcd.init(); lcd.begin(); lcd.setRotation( 0 ); lcd.setColorDepth( 24 );
  ui.createLayout(0, 80, 240, 240, layoutSprite, MULTI_EVENT );
  ui.setDentaroKeyPanels();//フリックキーボード用プリセット
  ui.createFlicks( 48, 48,  144, 192, 3, 4, ui_sprite0, MULTI_EVENT );
  flickUiSprite.createSprite( 144, 144 );
  ui.createBtns( 0,   48,  48, 192, 1, 4, ui_sprite1, MULTI_EVENT );
  ui.createBtns( 192, 48,  48, 192, 1, 4, ui_sprite2, MULTI_EVENT );
  ui.setDentaroKeyBtn();//フリックキーボード用プリセット
}
void loop( void ){
  ui.update( &lcd );
  ui.dentaroKeyUpdate( &lcd ,layoutSprite, ui_sprite0, ui_sprite1, ui_sprite2 ,flickUiSprite);
  if( ui.getEvent() == RELEASE ){
    lcd.fillRect(0, 28, 240, 100, TFT_BLACK);
    lcd.setTextSize(1);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.setCursor(0,30);
    lcd.setFont(&lgfxJapanGothicP_20);
    lcd.setTextWrap(true);
    lcd.setTextScroll(true);
    lcd.printf( ui.getDentaroFlickStrings().c_str() );
    lcd.drawString( ui.getDentaroFlickChar(), 150,0 );
  }
  ui.showTouchEventInfo( &lcd, lcd.width() - 50, 0 );//タッチイベントを視覚化する
  ui.showInfo( &lcd );//ボタン情報、フレームレート情報などを表示します。
  delay(1);
}
