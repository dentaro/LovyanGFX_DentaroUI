#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX_DentaroUI.hpp>
static LGFX lcd;
LovyanGFX_DentaroUI ui( &lcd );
static LGFX_Sprite ui_sprite0( &lcd );//スライダ用
void setup(){
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( &lcd ); lcd.init(); lcd.begin(); lcd.setRotation( 0 ); lcd.setColorDepth( 24 );
  ui.createSliders( 0, 140, 240, 160, 1, 1, ui_sprite0, XY_VAL, MULTI_EVENT );
  ui.setBtnName( ui.getUiID("SLIDER_0"), "2DSlider" );
  lcd.setBrightness( 127 ); 
}
void loop( void ){
  ui.update( &lcd );
  if( ui.getEvent() != NO_EVENT ){ 
    lcd.fillRect( 0, 0, 240, 160, TFT_BLACK);
    lcd.fillRect(
      ui.getSliderVal(ui.getUiID("SLIDER_0"), SLIDER_NO, X_VAL)*240,
      ui.getSliderVal(ui.getUiID("SLIDER_0"), SLIDER_NO, Y_VAL)*160,
      20,20,TFT_RED); 
  }
  ui.drawSliders( ui.getUiID("SLIDER_0"), &lcd, ui_sprite0 ); 
}//26行で完成！
