#define LGFX_AUTODETECT
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX_DentaroUI.hpp>
static LGFX lcd;
LovyanGFX_DentaroUI ui( &lcd );
static LGFX_Sprite ui_sprite0( &lcd );//スライダ用
static std::uint32_t sec, psec;
static std::uint32_t fps = 0, frame_count = 0;
int btn_No[4] = {0,2,6,8};//四隅のボタンを消す
String btn_name[9] = { "0", "A", "0", "B", "C", "D", "0", "E", "0" };
void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  ui.begin( &lcd ); lcd.init(); lcd.begin(); lcd.setRotation( 0 ); lcd.setColorDepth( 24 );
  ui.createBtns( 45, 170,  150, 150, 3, 3, ui_sprite0, TOUCH );
  for( int i=0; i < 4; i++ ){ui.setAvailableF( ui.getUiID("BTN_0"), btn_No[i], false ); }//四隅のボタンを消す
//以下も同じこと
//  ui.setAllBtnAvailableF( ui.getUiID("BTN_0"), false );//一度全部ボタンを消して
//  ui.setBtnName( 1, btn_name[1] ); //再描画していく
//  ui.setBtnName( 3, btn_name[3] ); 
//  ui.setBtnName( 4, btn_name[4] );
//  ui.setBtnName( 5, btn_name[5] ); 
//  ui.setBtnName( 7, btn_name[7] ); 
}
void loop( void ){
  ui.update( &lcd );
  if( ui.getEvent() != NO_EVENT ){//何かイベントがあれば
    ui.drawBtns( ui.getUiID("BTN_0"), &lcd, ui_sprite0 ); 
  }
  //ボタン情報、フレームレート情報などを表示します。
  showInfo();
  delay(1);
}
void showInfo(){
  ui.showTouchEventInfo( &lcd, lcd.width() - 100, 0 );//タッチイベントを視覚化する
  //フレームレート情報などを表示します。
  lcd.fillRect( 0,0,100,10,TFT_BLACK );
  lcd.setTextColor( TFT_WHITE );
  lcd.setCursor( 1,1 );
  lcd.print( fps );lcd.print( ":" );lcd.print( String( ui.getEvent() ) );lcd.print( "[" );lcd.print( String( ui.getPreEvent() ) );lcd.print( "]:BTN" );
  lcd.println( String( ui.getTouchBtnID() ) );
  ++frame_count;sec = millis() / 1000;
  if ( psec != sec ) { psec = sec; fps = frame_count; frame_count = 0; lcd.setAddrWindow( 0, 0, lcd.width(), lcd.height() ); }
}
