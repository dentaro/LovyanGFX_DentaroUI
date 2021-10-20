//LovyanGFXの設定です。2021年10月時点での動作を確認しています。
#define LGFX_AUTODETECT //デバイスの自動認識 (D-duino-32 XS, PyBadge はパネルID読取りが出来ないため自動認識の対象から外れています)
#define LGFX_USE_V1//新しいバージョンを使う

#include <LovyanGFX.hpp>//LovyanGFXライブラリを読み込む
#include <LGFX_AUTODETECT.hpp>  //クラス"LGFX"を準備します

//ESP32などの自作デバイス派の人は、AUTODETECTを使用せず、LGFX_ESP32_sample.hppにピンアサインなどの設定を書き込んで読み込んでください。
//#include "LGFX_ESP32_sample.hpp"

//でんたろう自作UIライブラリを読み込みます。LovyanGFXがないと動きません。
#include <LovyanGFX_DentaroUI.hpp>

//利用のながれ
//create系の関数で、UIコンテナというボタンのはいる箱を作り、その中を行と列で区切ることで、たくさんのボタンを一気に作ります。

//create時UIコンテナに自動でIDが貼られ以下のようなシリアル出力があるので、UIのID番号情報を確認してからloop内で
//ui.getUiID("SLIDER_0")などのdraw作業をしてください。（基本的に上から0,1,2と順番にふられます）
//These UI IDs are generated automatically. Check out the output of Serial when you create a UIContainer.
//[UI_ID information]
//SLIDER_0=[0]
//BTN_1=[1]

static LGFX lcd;// LGFXのインスタンスを作成
LovyanGFX_DentaroUI ui( &lcd );//lcdを入れる
static std::uint32_t sec, psec;
static std::uint32_t fps = 0, frame_count = 0;
static LGFX_Sprite ui_sprite0( &lcd );//スライダ用
static LGFX_Sprite ui_sprite1( &lcd );//トグル用
int v,r,g,b = 127;
void setup() {
  Serial.begin( 115200 ); delay( 50 );  // Serial Init Wait
  //lcd.init();lcd.begin();lcd.setRotation(1);lcd.setColorDepth(24);//ここでlcd関係を宣言するとなぜかタッチパネルキャリブレーションができます。
  ui.begin( &lcd );//uiを使うための準備
  lcd.init();//initとbeginはどちらかだけでも動くようです。
  lcd.begin();
  lcd.setRotation(0);//描画面の回転ができます。
  lcd.setColorDepth(24);// RGB888の24ビットに設定(表示される色数はパネル性能によりRGB666の18ビットになります)

  ui.createSliders( 0,   140, 180, 180, 1, 4, ui_sprite0, X_VAL, MULTI_EVENT);
  //スライダ用の箱を用意(x,y,w,h)し、x方向1列、y方向4行に区切ったスライダを生成します。
  //X方向の値（X_VAL）を利用します。
  //このスライダはMULTI_EVENT（何かのイベントがあれば）のときに反応するように設定しています。
  
  ui.createToggles( 180, 140,  60, 180, 1, 4, ui_sprite1, TOUCH );
  //トグル用の箱を用意(x,y,w,h)し、x方向1列、y方向4行に区切ったトグルボタンを生成します。
  //このスライダはMULTI_EVENT（何かのイベントがあれば）のときに反応するように設定しています。
  //toggleはTOUCH,WTAPPED,TAPPEDイベントで利用してください。
  
  ui.setBtnName( 0, "Brightness"  ); 
  ui.setBtnName( 1, "RED"        );
  ui.setBtnName( 2, "GREEN"       ); 
  ui.setBtnName( 3, "BLUE"       );
  //スライダボタンのID番号を指定して、スライダに名前をつけます。
  
  ui.setBtnName( 4, "MAX", "OFF"  ); 
  ui.setBtnName( 5, "MAX", "OFF" );
  ui.setBtnName( 6, "MAX", "OFF"  ); 
  ui.setBtnName( 7, "MAX", "OFF" );
  //トグルボタンのID番号を指定して、オン状態、オフ状態の順で名前をつけます。

}
void loop( void ){
  ui.update( &lcd );//この一行でタッチボタンイベントを取得します。

  //タッチされたボタン（スライダ）のID番号を取得し、UIコンテナ内のボタン番号と一致したら、スライダの値を変数に格納します。
  if( ui.getTouchBtnID() == ui.getUiFirstNo( ui.getUiID("SLIDER_0") )   ){ v = int( ui.getSliderVal( ui.getUiID("SLIDER_0"), 0 )*255 ); }
  if( ui.getTouchBtnID() == ui.getUiFirstNo( ui.getUiID("SLIDER_0") )+1 ){ r = int( ui.getSliderVal( ui.getUiID("SLIDER_0"), 1 )*255 ); }
  if( ui.getTouchBtnID() == ui.getUiFirstNo( ui.getUiID("SLIDER_0") )+2 ){ g = int( ui.getSliderVal( ui.getUiID("SLIDER_0"), 2 )*255 ); }
  if( ui.getTouchBtnID() == ui.getUiFirstNo( ui.getUiID("SLIDER_0") )+3 ){ b = int( ui.getSliderVal( ui.getUiID("SLIDER_0"), 3 )*255 ); }
  
  //タッチされたトグルボタンの値がtrueだったら、スライダのX値を1.0に、Y値を0.5に強制的に置き換えます。
  if( ui.getToggleVal( ui.getUiID("BTN_1"), 0 ) == true){ui.setSliderVal( ui.getUiID("SLIDER_0"), 0 , 1.0, 0.5 ); v = 255;}
  if( ui.getToggleVal( ui.getUiID("BTN_1"), 1 ) == true){ui.setSliderVal( ui.getUiID("SLIDER_0"), 1 , 1.0, 0.5 ); r = 255;}
  if( ui.getToggleVal( ui.getUiID("BTN_1"), 2 ) == true){ui.setSliderVal( ui.getUiID("SLIDER_0"), 2 , 1.0, 0.5 ); g = 255;}
  if( ui.getToggleVal( ui.getUiID("BTN_1"), 3 ) == true){ui.setSliderVal( ui.getUiID("SLIDER_0"), 3 , 1.0, 0.5 ); b = 255;}

  //何かしらのイベントが発生したときだけ、矩形を描画し、画面輝度を変えます。
  if( ui.getEvent() != NO_EVENT ){ lcd.fillRect( 0, 40, 240, 100, lcd.color888( r,g,b ) ); lcd.setBrightness( v );};
  
  //スライダボタンを描画します。
  ui.drawSliders( ui.getUiID("SLIDER_0"), &lcd, ui_sprite0 );
  
  //トグルボタンを描画します。
  ui.drawToggles( ui.getUiID("BTN_1"),    &lcd, ui_sprite1 );

  //ボタン情報、フレームレート情報などを表示します。
  showInfo();

  //WDT対策
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
