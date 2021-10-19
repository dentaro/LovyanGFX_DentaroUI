#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX_DentaroUI.hpp>
#define SLIDER_ID_0 0
#define BTN_ID_1 1
static LGFX lcd;
LovyanGFX_DentaroUI ui( &lcd );
static LGFX_Sprite ui_sprite0( &lcd );//スライダ用
static LGFX_Sprite ui_sprite1( &lcd );//トグル用
int c_val[4] = { 127, 127, 127, 127 };
String btn_name[4] = { "Brightness", "RED", "GREEN", "BLUE" };
void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( &lcd ); lcd.init(); lcd.begin(); lcd.setRotation( 0 ); lcd.setColorDepth( 24 );
  ui.createSliders( 0,   140, 180, 180, 1, 4, ui_sprite0, X_VAL, MULTI_EVENT );
  ui.createToggles( 180, 140,  60, 180, 1, 4, ui_sprite1, TOUCH );
  for( int i=0; i < 4; i++ ){ ui.setBtnName( i, btn_name[i] ); ui.setBtnName( 4 + i, "MAX", "OFF" ); }
}
void loop( void ){
  ui.update( &lcd );
  for( int i=0; i < 4; i++ ){
    if( ui.getTouchBtnID() == ui.getUiFirstNo( SLIDER_ID_0 )+i  ){ c_val[i] = int( ui.getSliderVal( SLIDER_ID_0, i )*255 ); }
    if( ui.getToggleVal( BTN_ID_1, i ) == true){ui.setSliderVal( SLIDER_ID_0, i , 1.0, 0.5 ); c_val[i] = 255; }
  }
  if( ui.getEvent() != NO_EVENT ){ lcd.setBrightness( c_val[0] ); lcd.fillRect( 0, 0, 240, 140, lcd.color888( c_val[1], c_val[2], c_val[3] ) ); };
  ui.drawSliders( SLIDER_ID_0, &lcd, ui_sprite0 ); 
  ui.drawToggles( BTN_ID_1,    &lcd, ui_sprite1 ); delay(1);
}//30行で完成！
