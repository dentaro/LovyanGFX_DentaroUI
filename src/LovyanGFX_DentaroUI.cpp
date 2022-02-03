#include "LovyanGFX_DentaroUI.hpp"
using namespace std;

#define FLICK_DIST 3
// #include <vector>
//LovyanGFX_DentaroUI::LovyanGFX_DentaroUI(LGFX& _lcd)
//{
// lcd = _lcd;
//}

void LovyanGFX_DentaroUI::touchCalibration (bool _calibF){
  touchCalibrationF = _calibF;
}

void LovyanGFX_DentaroUI::begin( LGFX* _lcd, String _host, int _shiftNum, int _colBit, int _rotateNo, bool _calibF )
{
  host = _host;
   SD.end();
  // SDカードがマウントされているかの確認

  if(!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 20000000)){
    Serial.println("Card Mount Failed");
    while (1) {}
  }

  // カードタイプの取得
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    // 画面に案内文章を描画します。
    _lcd->setTextDatum(textdatum_t::middle_center);
    _lcd->drawString("No SD card attached.", _lcd->width()>>1, _lcd->height() >> 1);
    _lcd->setTextDatum(textdatum_t::top_left);
    while (1) {}
  }

  // for(int i=0; i<18;i++){
  //   uiBoxes.push_back(*new UiContainer);
  // }

  _lcd->init();
  _lcd->begin(); 
  _lcd->startWrite();//CSアサート開始
  _lcd->setColorDepth( _colBit );
  touchCalibrationF = _calibF;
  shiftNum = _shiftNum;
  for(int i=0; i<shiftNum;i++){
    flickPanels[i] = new FlickPanel;
  }

  use_flickUiSpriteF = true;
  // flickUiSprite = _flickUiSprite;
  begin(_lcd);
  _lcd->setRotation( _rotateNo );

  
    for(int i = 0; i < BUF_PNG_NUM; i++)
  {
    createLayout( 0, 0, 256, 256, layoutSprite_list[i], MULTI_EVENT );//レイアウト用のスプライトを作る
  }
  
  for(int i = 0; i < BUF_PNG_NUM; i++)
  {
    createTile( layoutSprite_list[i], i, MULTI_EVENT ,i );//スプライト位置とタッチ座標もここでずらす
  }

  for(int objId = 0; objId < BUF_PNG_NUM; objId++){
    preReadXtileNo[objId] = -1;
    preReadYtileNo[objId] = -1;
  }
  

}

void LovyanGFX_DentaroUI::begin( LGFX* _lcd)
{
 lcdPos.x = 0;
 lcdPos.y = 0;
 
//  uiMode = _mode;
  if(uiMode == TOUCH_MODE){
    
  }else if(uiMode == PHYSICAL_MODE){
    selectBtnID = 0;
  }
  //btnNum = _btnNum;

  // タッチが使用可能な場合のキャリブレーションを行います。（省略可）
  if(touchCalibrationF == true){
    if (_lcd->touch())
    {
      //renameFile(SPIFFS, "/config.txt", fileName);
      if (_lcd->width() < _lcd->height()) _lcd->setRotation(_lcd->getRotation() ^ 1);

      // 画面に案内文章を描画します。
      _lcd->setTextDatum(textdatum_t::middle_center);
      _lcd->drawString("touch the arrow marker.", _lcd->width()>>1, _lcd->height() >> 1);
      _lcd->setTextDatum(textdatum_t::top_left);

      // タッチを使用する場合、キャリブレーションを行います。画面の四隅に表示される矢印の先端を順にタッチしてください。
      uint16_t fg = TFT_WHITE;
      uint16_t bg = TFT_BLACK;
      
      if (_lcd->isEPD()) swap(fg, bg);
      _lcd->calibrateTouch(nullptr, fg, bg, max(_lcd->width(), _lcd->height()) >> 3);
    
      _lcd->fillScreen(TFT_BLACK);
      _lcd->setColorDepth(TILE_COL_DEPTH);

      if(use_flickUiSpriteF){ 
        createFlickBtns(_lcd, flickUiSprite);//フリック用のボタンを生成
      }

          Serial.printf("heap_caps_get_free_size(MALLOC_CAP_SPIRAM)            : %6d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) );
          Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
          Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
          Serial.printf("Width:%d, Height:%d\n", 256, 256);
      //    void *p = sprite1.createSprite(256, 256);
      //    if ( p == NULL ) {
      //      Serial.println("メモリが足りなくて確保できない");
      //    }
          Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
          Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
    }

  }

  //9枚分のメモリを確保
  for(int i=0; i<9; i++)
  {
    MapTiles[i] = new MapTile;
    getTilePos(35.667995, 139.7532971887966, 15);//適当な座標を入れておく
    m_url = "/"+host+"/"+String(ztile)+"/"+String(xtile)+"/"+String(ytile + i)+".png";
    MapTiles[i]->begin(i, xtile, ytile, ztile, m_url);
  }

  Serial.println("");
  Serial.println("[UI_ID information]");
}

String LovyanGFX_DentaroUI::getPngUrl(int addNo){
  m_url = "/"+host+"/"+String(ztile)+"/"+String(xtile)+"/"+String(ytile + addNo)+".png";
  return m_url;
}

void LovyanGFX_DentaroUI::setLatLonPos(double _lat, double _lon){
  // _lat = lat;
  // _lon = lon;
}

//経緯度からタイル座標を求める
void LovyanGFX_DentaroUI::getTilePos(double _lat, double _lon, int zoom_level)
{
  //setLatLonPos(lat, lon);
  // _lat = lat;
  // _lon = lon;

  // //座標を含むタイル番号を計算
  // double lat_rad = _lat * (M_PI/180);
  // double n = _lcdpow(2, zoom_level);
  // xtileNo = int((_lon + 180.0) / 360.0 * n);
  // ytileNo = int((1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n);
  // ztile = zoom_level;

  // 緯度経度からタイル座標に変換
  double x = (_lon / 180 + 1) * pow(2, zoom_level) / 2;
  xtile = int(x*255);
  double y = ((-log(tan((45 + _lat / 2) * M_PI / 180)) + M_PI) * pow(2, zoom_level) / (2 * M_PI));
  ytile = int(y*255);
  ztile = zoom_level;

  // //座標を含むタイル番号を計算
  //x = (_lon / 180 + 1) * pow(2, zoom_level-1) / 2;
  // xtileNo = int(x);
  //y = ((-log(tan((45 + _lat / 2) * M_PI / 180)) + M_PI) * pow(2, zoom_level-1) / (2 * M_PI));
  // ytileNo = int(y);


// //現在地を含むタイル番号を計算
  double lat_rad = _lat * (M_PI/180);
  double n = pow(2, zoom_level);
  xtileNo = int((_lon + 180.0) / 360.0 * n);
  ytileNo = int((1.0 - log(tan(lat_rad) + (1 / cos(lat_rad))) / M_PI) / 2.0 * n);
}

void LovyanGFX_DentaroUI::update( LGFX* _lcd )
{

  preXtile = getXtile();
  preYtile = getYtile();
  
  preXtileNo = getXtileNo();
  preYtileNo = getYtileNo();

  // preDirID = dirID;

  _lcd->getTouch(&tp);
  for(int i = 0; i < 4; i++)
  {
    clist[i+1] = clist[i];
  }
  if(eventState != NO_EVENT) preEventState = eventState;//前のイベント状態を保持

  //以下イベント取得処理
  eventState = NO_EVENT;//イベント初期化
  flickState = NO_EVENT;//-1で初期化
  obj_ret.reset();
  
  //1ビット左にシフト
  touchState = touchState<<1;

  clist[0] = TFT_WHITE;
  
  if ( _lcd->getTouch( &tp ) )
  {
    bitSet(touchState, 0);//右０ビット目を１（Set）に
  }else{
    bitClear(touchState, 0);//右０ビット目を0（Clear）に
  }
  //下位２ビットをみてイベントを振り分け
  touchState &= B00000011;
  if (touchState == B_TOUCH){
    sp = tp; sTime =  micros();
    eventState = TOUCH;//6 
  }
  else if (touchState == B_MOVE) {
    clist[0] = TFT_GREEN;
    unsigned long touchedTime = micros() - sTime;
    eventState = WAIT;
    if(touchedTime > 160000){
      clist[0] = TFT_YELLOW;
      eventState = MOVE;//8
    }
  }
  else if (touchState == B_RELEASE) 
  {
    clist[0] = TFT_BLUE;
    unsigned long touchedTime = micros() - sTime;
    float dist = sqrt( pow((tp.x - sp.x),2) + pow((tp.y - sp.y),2) );

    if(touchedTime > 120000&&touchedTime <= 160000){
      if(dist <= FLICK_DIST){
        eventState = WAIT;//5
      }
    }
    else if(touchedTime > 160000)
    {
      eventState = DRAG;//2
    }
    else if(dist <= FLICK_DIST){
      eventState = TAPPED;//4
      
      float dist2 = sqrt( pow((tp.x - sp2.x),2) + pow((tp.y - sp2.y),2) );
      if(sTime - tappedTime < 200000 && dist2 < FLICK_DIST ){//ダブルタップの間の時間調整, 位置判定
        eventState = WTAPPED;//5
      }
      tappedTime = micros();
      sp2 = tp;
    }
    else if(dist > FLICK_DIST)
    {
       if(touchedTime <= 160000){
        float angle = getAngle(sp, tp);
  //      Serial.println(angle);
          
          if(angle <= 22.5 || angle > 337.5){
            eventState = RIGHT_FLICK;//0
            flickState = RIGHT_FLICK;//0
          }else if(angle <= 67.5 && angle > 22.5){
            eventState = R_D_FLICK;//7
            flickState = R_D_FLICK;//7
          }else if(angle <= 112.5 && angle > 67.5){
            eventState = DOWN_FLICK;//6
            flickState = DOWN_FLICK;//6
          }else if(angle <= 157.5 && angle > 112.5){
            eventState = D_L_FLICK;//5
            flickState = D_L_FLICK;//5
          }else if(angle <= 202.5 && angle > 157.5){
            eventState = LEFT_FLICK;//4
            flickState = LEFT_FLICK;//4
          }else if(angle <= 247.5 && angle > 202.5){
            eventState = L_U_FLICK;//3
            flickState = L_U_FLICK;//3
          }else if(angle <= 292.5 && angle > 247.5){
            eventState = UP_FLICK;//2
            flickState = UP_FLICK;//2
          }else if(angle <= 337.5 && angle > 292.5){
            eventState = U_R_FLICK;//1
            flickState = U_R_FLICK;//1
          }

       }
    }
    eventState = RELEASE;//9
  }else if (touchState == B_UNTOUCH) {
    clist[0] = TFT_DARKGREEN;
  }

  for(int i = 0; i < uiBoxes_num; i++){
    for(int j = 0; j<uiBoxes[i].b_num; j++){
      addHandler(uiBoxes[i].b_sNo + j, j, ret0_DG, uiBoxes[i].eventNo, uiBoxes[i].parentID);
      addHandler(uiBoxes[i].b_sNo + j, j, ret1_DG, uiBoxes[i].eventNo, uiBoxes[i].parentID);
    }
  }
}

void LovyanGFX_DentaroUI::flickSetup(LGFX* _lcd, LGFX_Sprite& _layoutSprite, 
                                        LGFX_Sprite& _ui_sprite0, LGFX_Sprite& _ui_sprite1, LGFX_Sprite& _ui_sprite2, LGFX_Sprite& _ui_sprite3, LGFX_Sprite& _flickUiSprite){
  createLayout( 0, 80, 240, 240, _layoutSprite,  MULTI_EVENT );//LAYOUT_0
  createBtns( 0,    18, 240, 30,  1, 1, _ui_sprite3, MULTI_EVENT );//BTN_1
  setFlickPanels();//フリックキーボード用プリセット
  createFlicks( 48, 48,  144, 192, 3, 4, _ui_sprite0, MULTI_EVENT );//FLICK_2
  //ここでフリックパネルのカスタマイズができます。
  //引数は(フリックパネル番号、ボタン番号、登録したい５文字、または９文字)になっています。
//  setFlickPanel(0, 0, "あいうえおかきくけ");
//  setFlickPanel(0, 1, "臨兵闘者皆陣列前行");
  _flickUiSprite.createSprite( 144, 144 );
  createBtns( 0,   48,  48, 192, 1, 4, _ui_sprite1, MULTI_EVENT );//BTN_3
  createBtns( 192, 48,  48, 192, 1, 4, _ui_sprite2, MULTI_EVENT );//BTN_4
  setFlick( JP , getUiID("BTN_1"), getUiID("FLICK_2"), getUiID("BTN_3"), getUiID("BTN_4"));//立ち上げ時のパネル指定　JP/EN/NUMERIC
  setBtnName( 0, "↓");
}
void LovyanGFX_DentaroUI::flickUpdate( LGFX* _lcd, LGFX_Sprite& _layoutSprite, 
                                        LGFX_Sprite& _ui_sprite0, LGFX_Sprite& _ui_sprite1, LGFX_Sprite& _ui_sprite2, LGFX_Sprite& _ui_sprite3, LGFX_Sprite& _flickUiSprite){

    if( getEvent() != NO_EVENT){ 
    if( getEvent() == TOUCH ){

      if( getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo + 2 )//フリックパネルのシフト uiBoxes[LeftBtnUiID].b_sNo+2 = 14
      { 
          fpNo++;
          fpNo %= SHIFT_NUM;//フリックパネルのシフト数
          if(fpNo==0||fpNo==1)setCharMode(CHAR_3_BYTE);//日本語パネル（３バイト）
          else if(fpNo==2||fpNo==3)setCharMode(CHAR_1_BYTE);//英語パネル（１バイト）
          setUiLabels( FlickUiID, fpNo);
          drawFlicks( FlickUiID, &_layoutSprite, _ui_sprite0 );
      }
      else if(getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo+ 1 )
      { //大小文字モード切替 uiBoxes[LeftBtnUiID].b_sNo+1 = 13
        if(fpNo!=2&&fpNo!=3)fpNo=2;//英語モードでなければ切り替える
          else if(fpNo==2)fpNo=3;
          else if(fpNo==3)fpNo=2;
          if(fpNo==2||fpNo==3){
            setCharMode( CHAR_1_BYTE );//英語パネル（１バイト）
          }
          setUiLabels( FlickUiID, fpNo );
          drawFlicks( FlickUiID, &_layoutSprite, _ui_sprite0 );
      }

      _layoutSprite.fillScreen(TFT_BLACK);
      drawBtns( TopBtnUiID, &_layoutSprite, _ui_sprite3 );
      drawBtns( LeftBtnUiID, &_layoutSprite, _ui_sprite1 );
      drawBtns( RightBtnUiID, &_layoutSprite, _ui_sprite2 );
      drawFlicks( FlickUiID, &_layoutSprite, _ui_sprite0 );
      drawFlickBtns( &_layoutSprite, _flickUiSprite, getTouchBtnID(), getTouchBtnNo(), true, true );
      drawLayOut( _layoutSprite );
    }

    if( getEvent() == RELEASE )
    {
      _layoutSprite.fillScreen(TFT_BLACK);
      // uiBoxes[TopBtnUiID].b_sNo = 0;
      // if( getTouchBtnID() == uiBoxes[1].b_sNo) 
      if( getTouchBtnID() == uiBoxes[TopBtnUiID].b_sNo) 
      // if( getTouchBtnID() == 0 ) 
      {
        if(uiOpenF){
          setLayoutPos( 0, 80 );
          }
        else if(!uiOpenF){setLayoutPos( 0, 290 ); }
        _lcd->fillScreen(TFT_BLACK);
        uiOpenF = !uiOpenF;
      }
      
      // _layoutSprite.fillScreen(TFT_BLACK);
      drawBtns( TopBtnUiID, &_layoutSprite, _ui_sprite3 );
      drawBtns( LeftBtnUiID, &_layoutSprite, _ui_sprite1 );
      drawBtns( RightBtnUiID, &_layoutSprite, _ui_sprite2 );
      drawFlicks( FlickUiID, &_layoutSprite, _ui_sprite0 );

      if(getTouchBtnID() >= getUiFirstNo(FlickUiID) && getTouchBtnID() < getUiFirstNo(FlickUiID) + 12 ){
       curKanaRowNo = getTouchBtnID() - getUiFirstNo(FlickUiID);//押されたボタンを行番号として渡す。
      }

      if(getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo)
      {//次へNxt uiBoxes[LeftBtnUiID].b_sNo = 12
        selectModeF =false;
        if(charMode == CHAR_3_BYTE ){//日本語入力の時
          finalChar = "";
          curKanaColNo++;
          curKanaColNo%=5;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);

          while(String("　") == finalChar){
            curKanaColNo++;
            curKanaColNo%=5;
            finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);
          }

          if(finalChar!="無"){
            flickStr = delEndChar(flickStr, 3);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }else if(charMode == CHAR_1_BYTE ){//英語入力の時
          finalChar = "";
          curKanaColNo++;
          curKanaColNo%=5;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,0);
          while(String(" ") == finalChar){
            curKanaColNo++;
            curKanaColNo%=5;
            finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,0);
          }
          if(finalChar!="無"){
            delChar();
            // flickStr = delEndChar(flickStr, 1);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }
      }
      if( getTouchBtnID() == uiBoxes[LeftBtnUiID].b_sNo + 3 )
      {//変換
        selectModeF =true;
        if( charMode == CHAR_3_BYTE ){
          kanaShiftNo++; 
          kanaShiftNo%=3;
          finalChar = getKana( showFlickPanelNo,curKanaRowNo,curKanaColNo,kanaShiftNo);
          if(finalChar!="無"){
            flickStr = delEndChar(flickStr, 3);
            flickStr += finalChar;
            flickStrDel += finalChar+"\n";
          }
        }
      }
      else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo)//delete
      {
        //最後の文字のバイト数を判定する
        setlocale(LC_ALL, "");
        vector<string> ret = split_mb(flickStrDel.c_str(),"\n");
        if(ret.size() >= 1)
        {
          //バイト数分消去
          flickStr = delEndChar(flickStr, ret[ret.size()-1].length());//一字分のバイト数削る
          //flickStrDelも更新（最後の字+ "\n"を削除）
          flickStrDel = "";
          for (size_t i = 0; i < ret.size()-1; i++) {
            flickStrDel += String(ret[i].c_str()) + "\n";
          }
        }

      }else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo+1){
        if( charMode == CHAR_3_BYTE ){//全角空白（日本語のとき）
          finalChar = "　";
          flickStr += finalChar;
          }
        else if( charMode == CHAR_1_BYTE ){//半角空白
          finalChar = " ";
          flickStr += finalChar;
        }
        // flickStrDel = flickStrDel + finalChar + "\n";//通らない
        flickStrDel = String(flickStrDel) + finalChar + "\n";//通る
        // flickStrDel += finalChar+"\n";//なぜか通らない
       
      }else if(getTouchBtnID() == uiBoxes[RightBtnUiID].b_sNo+2){flickStr = ""; flickStrDel = "";//クリアfinalStr = "";finalStrDel = "";
      }else if(flickStr.length() >= 72){flickStr = "";//24文字を超えたらfinalStr = "";
      }else 
      {
        if(selectModeF){
          kanaShiftNo = 0;
        }
        finalChar = getFlickStr();
        flickStr += finalChar;
        flickStrDel += finalChar+"\n";
        drawLayOut(_layoutSprite);
      }
    }
  }
}

void LovyanGFX_DentaroUI::delChar(){
  //最後の文字のバイト数を判定する
  setlocale(LC_ALL, "");
  vector<string> ret = split_mb(flickStrDel.c_str(),"\n");

  if(ret.size() >= 1){
  //バイト数分消去
  flickStr = delEndChar(flickStr, ret[ret.size()-1].length());//一字分のバイト数削る

  //flickStrDelも更新（最後の字+ "\n"を削除）
    flickStrDel = "";
    for (size_t i = 0; i < ret.size()-1; i++) {
      flickStrDel += String(ret[i].c_str()) + "\n";
    }
  }
}

String LovyanGFX_DentaroUI::getKana(int _panelID, int _rowID, int _colID, int _transID){
  String str="";
  string stdstr = "";
  stdstr = flickPanels[showFlickPanelNo]->text_list[_rowID].c_str();

  if( charMode == CHAR_3_BYTE )
  {
    String planeStr ="";
    
    planeStr = stdstr.substr(_colID*3, 3).c_str();
    str = stdstr.substr(_colID*3, 3).c_str();

    if(str != "ま"||planeStr != "な"){
      if(_transID == 1||_transID == 2){
        int i = 0;
        while(planeStr!= getHenkanChar(i, 0)||i>HENKAN_NUM-1){
          i++;
          if(i>HENKAN_NUM-1)break;
        }
        if(i < HENKAN_NUM && getHenkanChar(i, _transID).c_str()!=NULL)str = getHenkanChar(i, _transID).c_str();
        else str = "無";
      }
    }
  
  }
  else if( charMode == CHAR_1_BYTE )
  {
    str = stdstr.substr(_colID, 1).c_str();
  }

  return str;
}


//次のマルチバイト文字へのポインタを取得
const char* LovyanGFX_DentaroUI::next_c_mb(const char* c) {
  int L = mblen(c, 10);
  return c + L;
}

//マルチバイト文字を一文字取り出す
void LovyanGFX_DentaroUI::ngetc(char* const dst,const char* src) {
  int L = mblen(src, 10);
  memcpy(dst, src, L);
  dst[L] = '\0';
}

//マルチバイト文字を比較する
bool LovyanGFX_DentaroUI::nchr_cmp(const char* c1, const char* c2) {
  int K = mblen(c1, 10);
  int L = mblen(c2, 10);

  if (K != L)
    return false;

  bool issame = (strncmp(c1, c2, K) == 0);
  return issame;
}

vector<string> LovyanGFX_DentaroUI::split_mb(const char* src, const char* del) {

  char tmp[10];

  vector<string> result;

  string tmps;
  while (*src) {

    //デリミタを飛ばす
    // const char* p = src;
    while (nchr_cmp(src, del) == true && *src != '\0')
      src= next_c_mb(src);

    //デリミタに遭遇するまで文字を追加し続ける
    while (nchr_cmp(src, del) != true && *src != '\0') {
      ngetc(tmp, src);//一文字取り出す
      tmps += tmp;
      src = next_c_mb(src);
    }
    if (tmps.size()) {
      result.push_back(tmps);
    }
    tmps.clear();
  }

  return result;
}

void LovyanGFX_DentaroUI::updateSelectBtnID(int _selectBtnID){
 selectBtnID = _selectBtnID;
}

void LovyanGFX_DentaroUI::showTouchEventInfo(LovyanGFX* _lgfx, int _x, int _y){
  for(int i = 0; i < 5; i++){
    _lgfx->fillRect(_x + 1 + 10*i, _y, 9,9,clist[0]);
  }
}

void LovyanGFX_DentaroUI::createToggles(int _x, int _y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo){
  toggle_mode = true;
  createBtns( _x, _y, _w, _h, _row, _col, _uiSprite, _eventNo);
  toggle_mode = false;
}

void LovyanGFX_DentaroUI::createFlickBtns(LGFX* _lgfx, LGFX_Sprite& _flickUiSprite)
{
  flickPanel.b_sNo = 0;
  flickPanel.id  = 0;
  flickPanel.x   = 0;
  flickPanel.y   = 0;
  // flickPanel.w   = 144;
  // flickPanel.h   = 144;
  flickPanel.row = 3;
  flickPanel.col = 3;
  // flickPanel.eventNo = _eventNo;
  
  _flickUiSprite.setPsram( USE_PSRAM );
  //_uiSprite.setPsram(false);//UNUSE_PSRAM
  _flickUiSprite.setColorDepth( COL_DEPTH );
  _flickUiSprite.createSprite( flickPanel.w, flickPanel.h );
  
  
  //btnNum = _row * _col ;
  int b_w = 48;
  int b_h = 48;
  
  for(int j= 0; j < 3; j++)
  {
    for(int i= 0; i < 3; i++)
    {
      flick_touch_btn_list[ i*flickPanel.col + j ] = NULL;
      flick_touch_btn_list[ i*flickPanel.col + j ] = new TouchBtn( _lgfx ); 
      
      flick_touch_btn_list[ i*flickPanel.col + j ]->initBtn( i * flickPanel.col + j,"a",
      i*b_w, 
      j*b_h, 
      b_w, 
      b_h, 
      String( i*flickPanel.col + j ), 
      getTouchPoint(0, 0),
      getTouchPoint(0, 0),
      _flickUiSprite,
      TOUCH_FLICK_MODE);

      //ベータ版は基本true
      flick_touch_btn_list[ i * flickPanel.col + j ]->setVisibleF( true );
      flick_touch_btn_list[ i * flickPanel.col + j ]->setAvailableF( true );
    }
  }
  flickPanel.b_num =  9;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::changeBtnMode(int _uiID, int _btnID, int _btn_mode){
  //flick_touch_btn_list[ _btnID ]->btn_mode = _btn_mode;
}

// void LovyanGFX_DentaroUI::setFlickPanel(int _flickPanelID, int _btnID, String _btnsString, int _btn_mode){
//   // flick_touch_btn_list[ _btnID ]->btn_mode = _btn_mode;
//   // setFlickPanel(_flickPanelID, _btnID, _btnsString);
// }

void LovyanGFX_DentaroUI::setFlickPanel( int _flickPanelID, int _btnID, String _btnsString ){
  flickPanels[_flickPanelID]->text_list[_btnID] = _btnsString;
}

String LovyanGFX_DentaroUI::getFlickStr(){
  if(getTouchBtnID()!= NO_EVENT)
  {
    //押されたボタンのモードがFLICKだったら
    if(touch_btn_list[getTouchBtnID()]->getBtnMode() == TOUCH_FLICK_MODE){

      string gettingText = flickPanels[showFlickPanelNo]->text_list[getTouchBtnID() - getUiFirstNo(FlickUiID)].c_str();
      string trimText = "";
      flickString ="";
      trimText = gettingText;

      float angle = getAngle(sp, tp);
  //      Serial.println(angle);
      float dist = getDist(sp, tp);
      if(charMode == CHAR_3_BYTE){//日本語3ビットの場合
        //５文字の場合
        if(dist>24){

          if(gettingText.size() == 15)//5文字以下
          {
            charNumMode  = CHAR_3_BYTE_5;//5文字
            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(3*3,3).c_str();//え
              curKanaColNo = 3;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(4*3,3).c_str();//お
              curKanaColNo = 4;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1*3,3).c_str();//い
              curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(2*3,3).c_str();//う
              curKanaColNo = 2;
            }
          }
          else if(gettingText.size() == 27)//9文字
          {
            charNumMode  = CHAR_3_BYTE_9;//9文字
            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(5*3,3).c_str();//か
              curKanaColNo = 5;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(7*3,3).c_str();//く
              curKanaColNo = 7;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1*3,3).c_str();//い
              curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(3*3,3).c_str();//え
              curKanaColNo = 3;
            }else if(angle <= 67.5 && angle > 22.5){
              flickString = trimText.substr(6*3,3).c_str();//き
              curKanaColNo = 6;
            }else if(angle <= 157.5 && angle > 112.5){
              flickString = trimText.substr(8*3,3).c_str();//け
              curKanaColNo = 8;
            }else if(angle <= 247.5 && angle > 202.5){
              flickString = trimText.substr(2*3,3).c_str();//う
              curKanaColNo = 2;
            }else if(angle <= 337.5 && angle > 292.5){
              flickString = trimText.substr(4*3,3).c_str();//お
              curKanaColNo = 4;
            }
          }

        }else if(dist<=24){
          flickString = trimText.substr(0,3).c_str();//あ
          curKanaColNo = 0;
        }
        return flickString;
      }
      else if(charMode == CHAR_1_BYTE){//英数１ビットの場合
        if(dist>24){

          if(gettingText.size() <= 5)//1バイト文字5文字（英語）以下の場合
          {
            charNumMode  = CHAR_1_BYTE_5;//5文字
            flickString = "";
              if(angle <= 22.5 || angle > 337.5){
                if(trimText.length()>=4)
                flickString = trimText.substr(3,1).c_str();//D
              }
              
              if(angle <= 112.5 && angle > 67.5&&trimText.length()>=5){
                flickString = trimText.substr(4,1).c_str();//E
              }

              if(angle <= 202.5 && angle > 157.5&&trimText.length()>=2){
                flickString = trimText.substr(1,1).c_str();//B
              }

              if(angle <= 292.5 && angle > 247.5&&trimText.length()>=3){
                flickString = trimText.substr(2,1).c_str();//C
              }
          }

          if(gettingText.size() == 9)//1バイト文字5文字（英語）以下の場合
          {
            charNumMode  = CHAR_1_BYTE_9;//9文字
            flickString = "";
              // if(angle <= 22.5 || angle > 337.5){
              //   if(trimText.length()>=4)
              //   flickString = trimText.substr(3,1).c_str();//D
              // }
              
              // if(angle <= 112.5 && angle > 67.5&&trimText.length()>=5){
              //   flickString = trimText.substr(4,1).c_str();//E
              // }

              // if(angle <= 202.5 && angle > 157.5&&trimText.length()>=2){
              //   flickString = trimText.substr(1,1).c_str();//B
              // }

              // if(angle <= 292.5 && angle > 247.5&&trimText.length()>=3){
              //   flickString = trimText.substr(2,1).c_str();//C
              // }

            if(angle <= 22.5 || angle > 337.5){
              flickString = trimText.substr(5,1).c_str();//かF
              // curKanaColNo = 5;
            }else if(angle <= 112.5 && angle > 67.5){
              flickString = trimText.substr(7,1).c_str();//くH
              // curKanaColNo = 7;
            }else if(angle <= 202.5 && angle > 157.5){
              flickString = trimText.substr(1,1).c_str();//いB
              // curKanaColNo = 1;
            }else if(angle <= 292.5 && angle > 247.5){
              flickString = trimText.substr(3,1).c_str();//えD
              // curKanaColNo = 3;
            }else if(angle <= 67.5 && angle > 22.5){
              flickString = trimText.substr(6,1).c_str();//きG
              // curKanaColNo = 6;
            }else if(angle <= 157.5 && angle > 112.5){
              flickString = trimText.substr(8,1).c_str();//けI
              // curKanaColNo = 8;
            }else if(angle <= 247.5 && angle > 202.5){
              flickString = trimText.substr(2,1).c_str();//うC
              // curKanaColNo = 2;
            }else if(angle <= 337.5 && angle > 292.5){
              flickString = trimText.substr(4,1).c_str();//おE
              // curKanaColNo = 4;
            }

          }

        }else if(dist<=24){
          flickString = trimText.substr(0,1).c_str();//A
        }
        return flickString;
        
      }
    }
  }
  return "";
  
}

void LovyanGFX_DentaroUI::drawFlickBtns( LovyanGFX* _lgfx, LGFX_Sprite& _flickUiSprite, int _btnID, int _btnNo, bool _visible, bool _available )
{

  if(_btnID != NO_EVENT)
  {
    //押されたボタンのモードがFLICKだったら
    if(touch_btn_list[getTouchBtnID()]->getBtnMode() == TOUCH_FLICK_MODE)
    {
      //if(_visible){
      _flickUiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

      _flickUiSprite.fillScreen(TFT_RED);

      string panelText ="０１２３４５６７８";//デフォルトの文字セット
      //string prePanelText ="";
      string gettingText = flickPanels[showFlickPanelNo]->text_list[_btnNo].c_str();//ArduinoのString型をc_str()で、string型に直してから、渡す
      string trimText = "";
      if(gettingText.size() <= 5)//1バイト文字5文字（英語）以下の場合
      {
        panelText ="";
        for(int i=0; i<9; i++){
          int swapArray[9] = 
          {5,2,6,
           1,0,3,
           8,4,7};
          if(swapArray[i] < gettingText.size())
          {
            trimText = gettingText; panelText += trimText.substr(swapArray[i],1);
            panelText += " ";panelText += " ";
          }else{
            panelText += "　";
          }
        }
      }
      else if(gettingText.size() == 9)//1バイト文字9文字（英語）以下の場合
      {
        panelText ="";
        for(int i=0; i<9; i++){
          int swapArray[9] = 
          {2,3,4,
           1,0,5,
           8,7,6};
          if(swapArray[i] < gettingText.size())
          {
            trimText = gettingText; panelText += trimText.substr(swapArray[i],1);
            panelText += " ";panelText += " ";
          }else{
            panelText += "　";
          }
        }
      }
      else if(gettingText.size() == 15)//２バイト文字（日本語）5文字の場合
      {
        panelText ="";

        panelText += "　";//"５";//gettingText.substr(5*3,3).c_str();か
        trimText = gettingText; panelText += trimText.substr(2*3,3);//う
        panelText += "　";//"８";//gettingText.substr(8*3,3).c_str();け
      
        trimText = gettingText; panelText += trimText.substr(1*3,3);//い
        trimText = gettingText; panelText += trimText.substr(0,3);//あ
        trimText = gettingText; panelText += trimText.substr(3*3,3);//え

        panelText += "　";//"６";//gettingText.substr(6*3,3).c_str();き
        trimText = gettingText; panelText += trimText.substr(4*3,3);//お
        panelText += "　";//"７";//gettingText.substr(7*3,3).c_str();く
      }
      else if(gettingText.size() == 27)//9文字の場合
      {
        panelText ="";

        trimText = gettingText; panelText += trimText.substr(2*3,3);//う
        trimText = gettingText; panelText += trimText.substr(3*3,3);//え
        trimText = gettingText; panelText += trimText.substr(4*3,3);//お

        trimText = gettingText; panelText += trimText.substr(1*3,3);//い
        trimText = gettingText; panelText += trimText.substr(0,3);//あ
        trimText = gettingText; panelText += gettingText.substr(5*3,3);//か

        trimText = gettingText; panelText += gettingText.substr(8*3,3);//け
        trimText = gettingText; panelText += gettingText.substr(7*3,3);//く
        trimText = gettingText; panelText += gettingText.substr(6*3,3);//き
      }

      //panelText.replace( 0, gettingText.size(), gettingText );//　"あああ"　がきたら　"あああ３４５６７８"　のように置き換える

      //以下なぜか動かず、、、
      // if(panelText.size()<27){//3バイト*9文字以下なら
      //   panelText.replace(0,gettingText.size(), gettingText);//　"あああ"　がきたら　"あああ３４５６７８"　のように置き換える
      // }else{
      //   panelText.substr(0,27);//3バイト*9文字のみ切り出す
      // }

      //フリックパネルの文字を描画
        for(int j = 0; j < 3; j++){
          for(int i = 0; i < 3; i++){
            _flickUiSprite.setTextSize(1);

            flick_touch_btn_list[ j*3+i ]->setBtnName(panelText.substr((j*3+i)*3,3).c_str());//string型の状態で一文字切り出して、Stringに渡す
            // String btn_name = flickPanels[showFlickPanelNo]->btn_name.c_str();
            String btn_name = flick_touch_btn_list[ j*3+i ]->getBtnName();

            //flick_touch_btn_list[i*3+j]->btn_mode = BTN_MODE_FLICK;
            flick_touch_btn_list[j*3+i]->flickDraw( _flickUiSprite ); 
            _flickUiSprite.fillRoundRect(
              // flick_touch_btn_list[i]->getBtnPos().x, 
              // flick_touch_btn_list[i]->getBtnPos().y, 
                48*i, 
                48*j, 
                48,//flick_touch_btn_list[i]->getBtnSize().w, 
                48,//flick_touch_btn_list[i]->getBtnSize().h, 
                10, TFT_WHITE);
            if(btn_name == NULL){
              btn_name = String(j*3+i);
            }
            _flickUiSprite.setTextColor( TFT4_BLACK );
            _flickUiSprite.setFont(&lgfxJapanGothicP_20);
            int b_hw = 48/2;
            int b_hh = 48/2;
            float b_str_hw = _flickUiSprite.textWidth( btn_name ) / 2;
            _flickUiSprite.drawString( flick_touch_btn_list[ j*3+i ]->getBtnName(), 48*i + b_hw - b_str_hw , 48*j + b_hh - 4 );
          }
        }

      // _flickUiSprite.fillRoundRect(0, 0, 50, 50, 10, TFT_WHITE);
      // _flickUiSprite.pushSprite(_lgfx, _x, _y);
    // }
    // else if(!_visible){
      //_flickUiSprite.fillRoundRect(flick_touch_btn_list[i]->, 0, 48, 48, 10, TFT_WHITE);
      _flickUiSprite.pushSprite(_lgfx, getBtnPos(_btnID).x, getBtnPos(_btnID).y);
    // }
    }

  }

}

void LovyanGFX_DentaroUI::setLayoutSpritePos(int _LayoutSpritePosx, int _LayoutSpritePosy){
  layoutSpritePos = getTouchPoint(_LayoutSpritePosx, _LayoutSpritePosy);
}

void LovyanGFX_DentaroUI::createBtns(//修正中
  int _x, int _y, 
  int _w,int _h,
  int _row, int _col, 
  LGFX_Sprite& _uiSprite, int _eventNo)
  {//縦方向に並ぶ
  uiBoxes.push_back(*new UiContainer);

  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "BTN_" + String(uiID);
  Serial.println("BTN_" + String(uiID)  + "=[" + String(uiID) + "]" + String(btnID)+"~");
  
  int _startId = btnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = btnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;
  // uiBoxes[uiID].toggle_mode = toggle_mode;
  // uiBoxes[uiID].parentID = _parentID;
  
  _uiSprite.setPsram(USE_PSRAM);
  //_uiSprite.setPsram(false);//UNUSE_PSRAM
  _uiSprite.setColorDepth( COL_DEPTH );
  _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );
  

  int b_w = int(uiBoxes[uiID].w/uiBoxes[uiID].row);
  int b_h = int(uiBoxes[uiID].h/uiBoxes[uiID].col);

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      int p_btnID = _startId + j * uiBoxes[uiID].row + i;//事前に計算

      touch_btn_list[p_btnID] = NULL;
      touch_btn_list[p_btnID] = new TouchBtn( lcd ); 

      if(toggle_mode == false){
        touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
        i*b_w,
        j*b_h,
        b_w,
        b_h, 
        String(p_btnID), 
        layoutSpritePos, 
        getTouchPoint(_x, _y), 
        _uiSprite,
        TOUCH_BTN_MODE);
      }
      else if(toggle_mode == true){
        touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
        i*b_w,
        j*b_h,
        b_w,
        b_h, 
        String(p_btnID), 
        layoutSpritePos, 
        getTouchPoint(_x, _y), 
        _uiSprite,
        TOUCH_TOGGLE_MODE);
      }

      

      touch_btn_list[ p_btnID ]->setVisibleF( true );
      touch_btn_list[ p_btnID ]->setAvailableF( true );

      btnID++;//ボタンを更新
    }
  }
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::setAvailableF(int uiID, int _btnID, bool _available){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  touch_btn_list[_startId + _btnID]->setAvailableF(_available);
}

void LovyanGFX_DentaroUI::setAllBtnAvailableF(int uiID, bool _available){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
      for(int j= 0; j < uiBoxes[uiID].col; j++)
      {
        touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setAvailableF(_available);
      }
    }
}

void LovyanGFX_DentaroUI::setVisibleF(int uiID, int _btnID, bool _visible){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  touch_btn_list[_startId + _btnID]->setVisibleF(_visible);
}

void LovyanGFX_DentaroUI::setAllBtnVisibleF(int uiID, bool _visible){
  int _startId = uiBoxes[uiID].b_sNo;//スタート時のボタンIDをセット
  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
      for(int j= 0; j < uiBoxes[uiID].col; j++)
      {
        touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setVisibleF(_visible);
      }
    }
}

void LovyanGFX_DentaroUI::createBtns( 
  int _x, int _y, 
  int _w,int _h,int 
  _row, int _col, 
  LGFX_Sprite& _uiSprite, int _eventNo, bool _colMode)
  {//縦方向に並ぶ
  if(!_colMode)
  {
    createBtns( _x, _y, _w, _h, _row, _col, _uiSprite, _eventNo);
  }
  else if(!_colMode)
  {
    uiBoxes.push_back(*new UiContainer);
    uiBoxes_num++;
    uiID++;
    uiBoxes[uiID].label = "BTN_" + String(uiID);
    Serial.println("BTN_"+String(uiID)  + "=[" + String(uiID) + "]");
    int _startId = btnID;//スタート時のボタンIDをセット
    uiBoxes[uiID].b_sNo = btnID;
    uiBoxes[uiID].id  = uiID;
    uiBoxes[uiID].x   = _x;
    uiBoxes[uiID].y   = _y;
    uiBoxes[uiID].w   = _w;
    uiBoxes[uiID].h   = _h;
    uiBoxes[uiID].row = _row;
    uiBoxes[uiID].col = _col;
    uiBoxes[uiID].eventNo = _eventNo;
    // uiBoxes[uiID].parentID = _parentID;
    
    _uiSprite.setPsram( USE_PSRAM );
    //_uiSprite.setPsram(false);//UNUSE_PSRAM
    _uiSprite.setColorDepth( COL_DEPTH );
    _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );
    
    
    //btnNum = _row * _col ;
    int b_w = int( uiBoxes[uiID].w / uiBoxes[uiID].row );
    int b_h = int( uiBoxes[uiID].h / uiBoxes[uiID].col );
    
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      for(int i= 0; i < uiBoxes[uiID].row; i++)
      {
        //int p_btnID = _startId + j * uiBoxes[uiID].row + i;//事前に計算
        int p_btnID = _startId + i * uiBoxes[uiID].col + j;
        touch_btn_list[p_btnID] = NULL;
        touch_btn_list[p_btnID] = new TouchBtn( lcd ); 
        
        touch_btn_list[p_btnID]->initBtn( p_btnID, String(p_btnID),
        i*b_w, 
        j*b_h, 
        b_w, 
        b_h, 
        String(_startId), 
        layoutSpritePos, 
        getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y),
        _uiSprite, 
        TOUCH_BTN_MODE);

        // touch_btn_list[ _startId + i * uiBoxes[uiID].col + j ]->setVisibleF( _btnVisibleF );
        // touch_btn_list[ _startId + i * uiBoxes[uiID].col + j ]->setAvailableF( _btnAvailableF );
        
        //ベータ版は基本true
        touch_btn_list[ p_btnID ]->setVisibleF( true );
        touch_btn_list[ p_btnID ]->setAvailableF( true );
        btnID++;
      }
    }
    uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
  }
}

void LovyanGFX_DentaroUI::setLayoutPos( int _x, int _y )
{
  // uiBoxes[uiID].x   = _x;//タッチ座標
  // uiBoxes[uiID].y   = _y;

  layoutSpritePos.x = _x;//表示用レイアウト座標
  layoutSpritePos.y = _y;
  layoutSprite_w =240;
  layoutSprite_h =240;

  setLayoutPosToAllBtn(getTouchPoint(_x, _y ));
}

void LovyanGFX_DentaroUI::createLayout( int _x, int _y, int _w, int _h, LGFX_Sprite& _layoutSprite, int _eventNo = MULTI_EVENT){ 
  uiBoxes.push_back(*new UiContainer);
    uiBoxes_num++;
    uiID++;
    Serial.println("LAYOUT_" + String(uiID)  + "=[" + String(uiID) + "]" + String(btnID)+"~");
    uiBoxes[uiID].label = "LAYOUT_" + String(uiID);
    uiBoxes[uiID].b_sNo = 0;
    uiBoxes[uiID].id  = uiID;
    uiBoxes[uiID].x   = _x;
    uiBoxes[uiID].y   = _y;
    uiBoxes[uiID].w   = _w;
    uiBoxes[uiID].h   = _h;

    uiBoxes[uiID].eventNo = _eventNo;
    // uiBoxes[uiID].parentID = _parentID;
  
    layoutSpritePos.x = _x;
    layoutSpritePos.y = _y;
    layoutSprite_w = _w;
    layoutSprite_h = _h;
    
    // _layoutSprite.setPsram(false);
    _layoutSprite.setPsram(true);
    _layoutSprite.setColorDepth(TILE_COL_DEPTH);//親スプライトの色深度１６
    _layoutSprite.createSprite(layoutSprite_w, layoutSprite_h);
    _layoutSprite.setPaletteGrayscale();
}

void LovyanGFX_DentaroUI::createSliders(int _x, int _y, int _w, int _h,  int _row, int _col,
LGFX_Sprite& _uiSprite, 
int _xy_mode, 
int _eventNo)
{ 
  uiBoxes.push_back(*new UiContainer);
  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "SLIDER_" + String(uiID);
  Serial.println("SLIDER_" + String(uiID) + "=[" + String(uiID) + "]");
  int _startId = btnID;
  uiBoxes[uiID].b_sNo = btnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;
  //uiBoxes[uiID].parentID = _parentID;
  
  _uiSprite.setPsram( USE_PSRAM );
  // _uiSprite.setPsram(false);//UNUSE_PSRAM
  _uiSprite.setColorDepth( COL_DEPTH );
  _uiSprite.createSprite( _w, _h );
  

  //sliderNum = _row * _col ;
  int s_w = int( _w/_row );
  int s_h = int( _h/_col );
  
  for( int i= 0; i < _row; i++ )
    {
    for( int j= 0; j < _col; j++ )
    {
    btnID = _startId + j*_row + i;
    touch_btn_list[_startId + j*_row + i] = NULL;
    touch_btn_list[_startId + j*_row + i] = new TouchBtn( lcd ); 
    touch_btn_list[_startId + j*_row + i]->initSlider( _startId + j*_row + i,
    i*s_w,
    j*s_h,
    s_w,
    s_h, 
    String( _startId + j*_row + i ), 
    layoutSpritePos, 
    getTouchPoint(_x, _y), 
    _uiSprite,
    _xy_mode);
    
    btnID++;
    }
  }
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;
}



void LovyanGFX_DentaroUI::setQWERTY(int uiID, String _btnsString, LGFX_Sprite& _sprite)
{
  int charsNum = _btnsString.length();
  const char* btnChars = _btnsString.c_str();
  int _id = uiBoxes[uiID].b_sNo;
  
  for(int i=0; i<charsNum; i++){
    touch_btn_list[_id + i]->setVisibleF(true);
    // touch_btn_list[_id + i]->setAvailableF(true);
    touch_btn_list[_id + i]->setBtnName(String(btnChars[i]));
    //touch_btn_list[_id + i]->setColor(_bgColIndex);
  }
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName, String _btnNameFalse){
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setBtnName(_btnName);
  touch_btn_list[_btnId]->setBtnNameFalse(_btnNameFalse);//トグルのfalse状態の時のラベルを設定
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName)
{
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setAvailableF(true);
  touch_btn_list[_btnId]->setVisibleF(true);
  // touch_btn_list[_btnId]->setAvailableF(true);
  touch_btn_list[_btnId]->setBtnName(_btnName);
  //touch_btn_list[_btnId]->setColor(_bgColIndex);
}

// void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX *_lgfx, LGFX_Sprite& _uiSprite)
// {
//   drawBtns(_uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
// }

void LovyanGFX_DentaroUI::drawBtns( int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _x, int _y)
{
  lgfx = _lgfx;
  if( getEvent() != NO_EVENT ){
  if(getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
  {
    int _id = uiBoxes[uiID].b_sNo;
    int _btnNum = uiBoxes[uiID].b_num;
    _uiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

    for(int i= _id; i < _id + _btnNum; i++)
    {
      touch_btn_list[i]->setSelectBtnF(false);
      
      if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      else touch_btn_list[i]->setSelectBtnF(false);
      touch_btn_list[i]->btnDraw(_uiSprite); 
      
      // if(_showMethod == SHOW_ALL)
      // {
      //   if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      //   else touch_btn_list[i]->setSelectBtnF(false);
      //   touch_btn_list[i]->btnDraw(_uiSprite); 
      // }
      // else if(_showMethod == SHOW_NAMED)
      // {
      //   if(touch_btn_list[i]->getAvailableF() == true){
      //     if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      //     else touch_btn_list[i]->setSelectBtnF(false);
      //     touch_btn_list[i]->btnDraw(_uiSprite);
      //   }
      // }
    }
    _uiSprite.pushSprite(_lgfx, uiBoxes[uiID].x, uiBoxes[uiID].y);
    
    }
  }
}

void LovyanGFX_DentaroUI::drawSliders(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite)
{
  drawSliders( _uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawSliders(int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _x, int _y)
{
  if( getEvent() != NO_EVENT ){
    if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
    int _id = uiBoxes[uiID].b_sNo;
    int _btnNum = uiBoxes[uiID].b_num;
    _uiSprite.setPivot(0, 0);//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

    for(int i = _id; i < _id + _btnNum; i++)
    {
  //    touch_btn_list[i].setSelectBtnF(false);
      touch_btn_list[i]->sliderDraw(_uiSprite, tp); 
      
  //    if(_showMethod == SHOW_ALL)
  //    {
  //      if(selectBtnID == i)touch_btn_list[i].setSelectBtnF(true);
  //      else touch_btn_list[i].setSelectBtnF(false);
  //      touch_btn_list[i].sliderDraw(_uiSprite); 
  //    }
  //    else if(_showMethod == SHOW_NAMED)
  //    {
  //      if(touch_btn_list[i].getAvailableF()==true){
  //        if(selectBtnID == i)touch_btn_list[i].setSelectBtnF(true);
  //        else touch_btn_list[i].setSelectBtnF(false);
  //        touch_btn_list[i].sliderDraw(_uiSprite);
  //      }
  //    }
      
    }
    _uiSprite.pushSprite(_lgfx, _x, _y);
    }
  }
  //_layoutSprite.pushSprite(layoutSpritePos.x, layoutSpritePos.y);//最終的な出力
}

void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite){
  toggle_mode = false;
  drawBtns( _uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

// void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _x, int _y){
//   toggle_mode = false;
//   drawBtns( _uiID, _lgfx, _uiSprite, _x, _y);
// }

void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite){
  drawToggles(_uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _x, int _y){
  toggle_mode = true;
  drawBtns(_uiID, _lgfx, _uiSprite, _x, _y);
  toggle_mode = false;
}

void LovyanGFX_DentaroUI::createTile( LGFX_Sprite& _layoutSprite, int _layoutUiID, int _eventNo, int _spriteNo ){//横方向に並ぶ
  uiBoxes.push_back(*new UiContainer);
  uiBoxes_num++;
  uiID++;
  Serial.println("TILE_" + String(uiID) + "=[" + String(uiID) + "]");
  int _startId = btnID;
  uiBoxes[uiID].b_sNo = btnID;
  uiBoxes[uiID].id  = uiID;//いる？
  uiBoxes[uiID].x   = uiBoxes[_layoutUiID].x;
  uiBoxes[uiID].y   = uiBoxes[_layoutUiID].y;
  uiBoxes[uiID].w   = uiBoxes[_layoutUiID].w;
  uiBoxes[uiID].h   = uiBoxes[_layoutUiID].h;
  uiBoxes[uiID].row = 1;
  uiBoxes[uiID].col = 1;
  uiBoxes[uiID].eventNo = _eventNo;
  // uiBoxes[uiID].parentID = _parentID;

  touch_btn_list[_spriteNo] = NULL;
  touch_btn_list[_spriteNo] = new TouchBtn(lcd);

  touch_btn_list[_spriteNo] -> initTile(
    _startId, 
    String(_startId), 
    getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y),
    uiBoxes[uiID].w,
    uiBoxes[uiID].h, 
    _layoutSprite, 
    *MapTiles[_spriteNo]->getSpritePtr());
    // g_basic_sprite_list[_spriteNo]);

   uiBoxes[uiID].b_num =  1;
}

void LovyanGFX_DentaroUI::createFlicks(int _x, int _y, int _w,int _h,int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo){//縦方向に並ぶ
uiBoxes.push_back(*new UiContainer);
  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "FLICK_" + String(uiID);
  Serial.println("FLICK_" + String(uiID)  + "=[" + String(uiID) + "]" + String(btnID)+"~");
  
  int _startId = btnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = btnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _x;
  uiBoxes[uiID].y   = _y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;
  // uiBoxes[uiID].toggle_mode = toggle_mode;
  // uiBoxes[uiID].parentID = _parentID;
  
  _uiSprite.setPsram(USE_PSRAM);
  //_uiSprite.setPsram(false);//UNUSE_PSRAM
  _uiSprite.setColorDepth( COL_DEPTH );
  _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );
  

  int b_w = int(uiBoxes[uiID].w/uiBoxes[uiID].row);
  int b_h = int(uiBoxes[uiID].h/uiBoxes[uiID].col);

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      int p_btnID = _startId + j * uiBoxes[uiID].row + i;
      string stdstr = flickPanels[showFlickPanelNo]->text_list[j * uiBoxes[uiID].row + i].c_str();
      String str = stdstr.substr(0,3).c_str();//先頭文字をラベルにする
      
      touch_btn_list[p_btnID] = NULL;
      touch_btn_list[p_btnID] = new TouchBtn( lcd ); 
      touch_btn_list[p_btnID]->initBtn(p_btnID, String(p_btnID),
      i*b_w,
      j*b_h,
      b_w,
      b_h, 
      str, //先頭文字をラベルにする
      layoutSpritePos, 
      getTouchPoint(_x, _y), 
      _uiSprite,
      TOUCH_FLICK_MODE);
      

      // touch_btn_list[ p_btnID ]->setVisibleF( _btnVisibleF );
      // touch_btn_list[ p_btnID ]->setAvailableF( _btnAvailableF );
      
      //ベータ版は基本true
      touch_btn_list[ p_btnID ]->setVisibleF( true );
      touch_btn_list[ p_btnID ]->setAvailableF( true );

      btnID++;//ボタンを更新
    }
  }
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::drawFlicks(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite){
  toggle_mode = false;
  
  drawFlicks( _uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawFlicks( int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _x, int _y)
{
  lgfx = _lgfx;
  if( getEvent() != NO_EVENT ){
    if(getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
    int _id = uiBoxes[uiID].b_sNo;
    int _btnNum = uiBoxes[uiID].b_num;
    _uiSprite.setPivot( 0, 0 );//setPivot()で回転する場合の原点を指定します。初期値は左上の(0, 0)だと思います

    for(int i= _id; i < _id + _btnNum; i++)
    {
      touch_btn_list[i]->setSelectBtnF(false);
      if(selectBtnID == i)touch_btn_list[i]->setSelectBtnF(true);
      else touch_btn_list[i]->setSelectBtnF(false);
      touch_btn_list[i]->btnDraw(_uiSprite);
    }
    _uiSprite.pushSprite(lgfx, uiBoxes[uiID].x, uiBoxes[uiID].y);
    }
  }
}

void LovyanGFX_DentaroUI::setupMaps(LGFX* _lcd)
{
 
}

void LovyanGFX_DentaroUI::setPngTile( fs::FS &fs, String _m_url, int _spriteNo )
{
  MapTiles[_spriteNo]->getSpritePtr()->drawPngFile(fs, _m_url,
                                0, 0,
                                256, 256,
                                0, 0, 1.0, 1.0,
                                datum_t::top_left);

  // this->g_basic_sprite_list[_spriteNo].drawPngFile(fs, _m_url,
  //                               0, 0,
  //                               256, 256,
  //                               0, 0, 1.0, 1.0,
  //                               datum_t::top_left);
}

void LovyanGFX_DentaroUI::drawTile(int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex, int _spriteNo )//スプライトに格納
{
  if( getEvent() != NO_EVENT )
  {
    if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
    {
      int _id = uiBoxes[uiID].b_sNo;
      touch_btn_list[_id]->setVisibleF(true);
      touch_btn_list[_id]->tileDraw( _lgfx, _layoutSprite, layoutSpritePos, sp, _bgColIndex, *MapTiles[_spriteNo]->getSpritePtr());
    }

  }
}
void LovyanGFX_DentaroUI::drawTileAuto(int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _layoutSprite, int _bgColIndex, int _spriteNo )//スプライトに格納
{
  int _id = uiBoxes[uiID].b_sNo;
  // touch_btn_list[_id]->setVisibleF(false);
  touch_btn_list[_id]->setVisibleF(true);
  
   touch_btn_list[_id]->setDrawFinishF(false);

  touch_btn_list[_id]->tileDraw( _lgfx, _layoutSprite, layoutSpritePos, sp, _bgColIndex, *MapTiles[_spriteNo]->getSpritePtr());

  while(touch_btn_list[_id]->getDrawFinishF() == false)
          {
          //読み込み完了を待つだけ
            delay(1);
          }
  touch_btn_list[_id]->setDrawFinishF(true);

    
  
}

void LovyanGFX_DentaroUI::drawLayOut(LGFX_Sprite& _layoutSprite){
  // if(getEvent() == uiBoxes[uiID].eventNo)
  // {


    _layoutSprite.pushSprite(layoutSpritePos.x, layoutSpritePos.y);//最終的な出力
  // }
}

bool LovyanGFX_DentaroUI::isAvailable( int _btnID ){
  return touch_btn_list[_btnID]->getAvailableF();
}

void LovyanGFX_DentaroUI::addHandler( int _btnID, int _btnNo, DelegateBase2* _func, uint16_t _runEventNo, int _parentID){
  
  runEventNo = _runEventNo;
  //btnID = _btnID;
  _parentID = 0;//ベータ版ではとりあえず強制的にLCDのみのイベントをとる
  
  touch_btn_list[_btnID]->addHandler(_func);

  if( parentID == PARENT_LCD ) touch_btn_list[_btnID]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
  else if( parentID == PARENT_SPRITE ) touch_btn_list[_btnID]->run2( _btnID, _btnNo, sp, tp, eventState, runEventNo);//ボタン内のイベントかチェック
  
  touch_btn_list[_btnID]->delHandlers2();
}

float LovyanGFX_DentaroUI::getAngle(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b ){

  float r = atan2( b.y - a.y, b.x - a.x );
  if (r < 0) {
        r = r + 2 * M_PI;
    }
    return r * 360 / (2 * M_PI);
}

float LovyanGFX_DentaroUI::getDist(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b ){
   float d = sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
    return d;
}

int LovyanGFX_DentaroUI::getPreEvent()
{
  return preEventState;
}

void LovyanGFX_DentaroUI::circle(LovyanGFX* _lgfx,  uint16_t c, int fillF) 
{
  if(fillF){
    _lgfx->fillCircle(tp.x, tp.y, 10, c);
    _lgfx->fillCircle(tp.x, tp.y, 10, c);
  }else{
    _lgfx->drawCircle(tp.x, tp.y, 10, c);
    _lgfx->drawCircle(tp.x, tp.y, 10, c);
  }
}

void LovyanGFX_DentaroUI::rect(LovyanGFX* _lgfx, uint16_t c, int fillF) 
{
  if(fillF){
    _lgfx->fillRect(tp.x, tp.y, 10, 10, c);
    _lgfx->fillRect(tp.x, tp.y, 10, 10, c);
  }else{
    _lgfx->drawRect(tp.x, tp.y, 10, 10, c);
    _lgfx->drawRect(tp.x, tp.y, 10, 10, c);
  }
}

void LovyanGFX_DentaroUI::nowLoc(LovyanGFX* _lgfx){
    _lgfx->fillTriangle(110, 106, 130, 106, 120, 120, TFT_RED);
    _lgfx->fillCircle(120, 120 - 18, 10, TFT_RED);
    _lgfx->fillCircle(120, 120 - 18, 6, TFT_WHITE);
    }

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getPos()
{
  return tp;
  
}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getStartPos()
{
  return sp;
}

void LovyanGFX_DentaroUI::setPos(int _x, int _y){
  tp.x = _x;
  tp.y = _y;
}

void LovyanGFX_DentaroUI::setStartPos(int _x, int _y){
  sp.x = _x;
  sp.y = _y;
}

int LovyanGFX_DentaroUI::getTouchBtnID(){
  return obj_ret.btnID;
}

int LovyanGFX_DentaroUI::getTouchBtnNo(){
  return getTouchBtnID() - getUiFirstNo(FlickUiID);
}

int LovyanGFX_DentaroUI::getEvent(){
  return eventState;
}

int LovyanGFX_DentaroUI::getFlickEvent(){
  return flickState;
}

// LGFX_Sprite LovyanGFX_DentaroUI::getTileSprite(int _btnID)
// {
//   return g_basic_sprite;
// }

void LovyanGFX_DentaroUI::setTilePos(int _id, int _x, int _y)
{
  touch_btn_list[_id]->setTilePos(getTouchPoint(_x - layoutSpritePos.x, _y - layoutSpritePos.y));
}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getTouchPoint(int _x, int _y){
  lgfx::v1::touch_point_t tp;
  tp.x = _x;
  tp.y = _y;
  return tp;
}

float LovyanGFX_DentaroUI::getSliderVal(int uiID, int _btnNo){
  float retVal = 1.0;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(touch_btn_list[_btnID]->get_xy_mode() == X_VAL){retVal = touch_btn_list[_btnID]->getSliderValx();}
  else if(touch_btn_list[_btnID]->get_xy_mode() == Y_VAL){retVal = touch_btn_list[_btnID]->getSliderValy();}
  return retVal;
}

float LovyanGFX_DentaroUI::getSliderVal(int uiID,int _btnNo, int _xy){
  float retVal = 1.0;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(_xy == X_VAL){retVal = touch_btn_list[_btnID]->getSliderValx();}
  else if(_xy == Y_VAL){retVal = touch_btn_list[_btnID]->getSliderValy();}
  return retVal;
}

Vec2 LovyanGFX_DentaroUI::getSliderVec2(int uiID, int _btnNo){//XYを一気に取得
  Vec2  retPos;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(touch_btn_list[_btnID]->get_xy_mode() == XY_VAL){
    retPos.x = touch_btn_list[_btnID]->getSliderValx();
    retPos.y = touch_btn_list[_btnID]->getSliderValy();
  }
  return retPos;
}

void LovyanGFX_DentaroUI::setSliderVal(int uiID, int _btnNo, float _x, float _y){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  touch_btn_list[_btnID]->setSliderVal(_x, _y);
}


void LovyanGFX_DentaroUI::setCharMode(int _charMode){
  charMode = _charMode;
}

int LovyanGFX_DentaroUI::getCharMode(){
  return charMode;
}

bool LovyanGFX_DentaroUI::getToggleVal(int _uiID, int _btnNo){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  return touch_btn_list[_btnID]->getToggleVal();
}

int LovyanGFX_DentaroUI::getUiFirstNo(int uiID){
  return uiBoxes[uiID].b_sNo;
}

int LovyanGFX_DentaroUI::getflickPanelBtnNo(int uiID){
  return flickPanels[uiID]->b_No;
}

int LovyanGFX_DentaroUI::getUiBtnNum(int uiID){
  return uiBoxes[uiID].b_num;
}

int LovyanGFX_DentaroUI::getUiBoxNum(){
//  return uiBoxes.size();
  return uiBoxes_num;
}

int LovyanGFX_DentaroUI::getAllBtnNum(){
  return btnID;
}

int LovyanGFX_DentaroUI::getParentID(){
  return parentID;
}

int LovyanGFX_DentaroUI::getUiID( const char* _uiLabel){
  int i = 0;
  while(uiBoxes[i].label != String(_uiLabel)){
    i++;
  }
  return i;
}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getBtnPos(int _btnID){
  return getTouchPoint(touch_btn_list[_btnID]->getBtnPos().x, touch_btn_list[_btnID]->getBtnPos().y); 
}

void LovyanGFX_DentaroUI::setUiLabels(int uiID, int _showFlickPanelNo)
{
  showFlickPanelNo = _showFlickPanelNo;

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
      // int p_btnID = uiBoxes[uiID].b_sNo + j * uiBoxes[uiID].row + i;
      int p_btnID = uiBoxes[uiID].b_sNo + j * uiBoxes[ uiID ].row + i;
      string stdstr = flickPanels[ showFlickPanelNo ]->text_list[ j * uiBoxes[uiID].row + i ].c_str();
      String str = stdstr.substr( 0, 3 ).c_str();//先頭文字をラベルにする
      touch_btn_list[ p_btnID ]->setBtnName( str );
    }
  }
}

void LovyanGFX_DentaroUI::showInfo(LGFX& _lgfx , int _infox, int _infoy){
  //showTouchEventInfo( _lgfx, _lgfx.width() - 100, 0 );//タッチイベントを視覚化する
  //フレームレート情報などを表示します。
  _lgfx.fillRect( _infox, _infoy, 100, 10, TFT_BLACK );
  _lgfx.setTextSize(2);
  _lgfx.setTextColor( TFT_WHITE );
  _lgfx.setCursor( _infox + 1, _infoy + 1 );
  _lgfx.print( fps );
  _lgfx.print( ":" );
  _lgfx.print( String( getEvent() ) );
  _lgfx.print( "[" );
  _lgfx.print( String( getPreEvent() ) );
  _lgfx.print( "]:BTN" );
  _lgfx.println( String( getTouchBtnID() ) );
  ++frame_count;sec = millis() / 1000;
  if ( psec != sec ) {
    psec = sec; fps = frame_count; 
    frame_count = 0; 
    _lgfx.setAddrWindow( 0, 0, _lgfx.width(), _lgfx.height() ); 
    //_lgfx.setAddrWindow( 0, 0, 240, 320 ); 
  }
}


void LovyanGFX_DentaroUI::showInfo(LovyanGFX* _lgfx , int _infox, int _infoy){
  //フレームレート情報などを表示します。
  _lgfx->setTextSize(1);
  _lgfx->setFont(&lgfxJapanGothicP_8);
  _lgfx->fillRect( 0, 0, 150, 10, TFT_BLACK );
  _lgfx->setTextColor( TFT_WHITE );
  _lgfx->setCursor( 1,1 );
  _lgfx->print( fps );
  _lgfx->print( ":" );
  _lgfx->print( String( getEvent() ) );
  _lgfx->print( "[" );
  _lgfx->print( String( getPreEvent() ) );
  _lgfx->print( "]:BTN" );
  _lgfx->print( "[" );
  _lgfx->print( String( getFlickEvent() ) );
  _lgfx->print( "]:BTN" );
  _lgfx->println( String( getTouchBtnID() ) );
  ++frame_count;sec = millis() / 1000;
  if ( psec != sec ) {
    psec = sec; fps = frame_count; 
    frame_count = 0; 
    _lgfx->setAddrWindow( 0, 0, _lgfx->width(), _lgfx->height() ); 
  }
}

String LovyanGFX_DentaroUI::getHenkanChar(int _henkanListNo, int _kanaShiftNo){ 
return kanalist[_henkanListNo][_kanaShiftNo];
} 


void LovyanGFX_DentaroUI::setFlickPanels(){
  //パネル番号、ボタン番号、表示文字９個
  //setFlickPanel(0, 0, "あいうえおかきくけ");//9文字はいまのところ未対応

  int _b_sNo = uiBoxes[FlickUiID].b_sNo;

  setCharMode(CHAR_3_BYTE);
  //全角5文字になるように空白を入れて文字を登録
  setFlickPanel(0, _b_sNo, "あいうえお");//第一引数はSHIFT_ID
  setFlickPanel(0, _b_sNo+1, "かきくけこ");
  setFlickPanel(0, _b_sNo+2, "さしすせそ");
  setFlickPanel(0, _b_sNo+3, "たちつてと");
  setFlickPanel(0, _b_sNo+4, "なにぬねの");
  setFlickPanel(0, _b_sNo+5, "はひふへほ");
  setFlickPanel(0, _b_sNo+6, "まみむめも");
  setFlickPanel(0, _b_sNo+7, "や「ゆ」よ");
  setFlickPanel(0, _b_sNo+8, "らりるれろ");
  setFlickPanel(0, _b_sNo+9, "～（・）　");
  setFlickPanel(0, _b_sNo+10, "わをんー　");
  setFlickPanel(0, _b_sNo+11, "、。？！　");

  setFlickPanel(1, _b_sNo+0, "アイウエオ");
  setFlickPanel(1, _b_sNo+1, "カキクケコ");
  setFlickPanel(1, _b_sNo+2, "サシスセソ");
  setFlickPanel(1, _b_sNo+3, "タチツテト");
  setFlickPanel(1, _b_sNo+4, "ナニヌネノ");
  setFlickPanel(1, _b_sNo+5, "ハヒフヘホ");
  setFlickPanel(1, _b_sNo+6, "マミムメモ");
  setFlickPanel(1, _b_sNo+7, "ヤ「ユ」ヨ");
  setFlickPanel(1, _b_sNo+8, "ラリルレロ");
  setFlickPanel(1, _b_sNo+9, "％＆￥―　");
  setFlickPanel(1, _b_sNo+10, "ワヲンー　");
  setFlickPanel(1, _b_sNo+11, "、。？！　");

  setCharMode(CHAR_1_BYTE);
  //半角5文字になるように空白を入れて文字を登録
  setFlickPanel(2, _b_sNo+0, "@#|&_");
  setFlickPanel(2, _b_sNo+1, "ABC  ");
  setFlickPanel(2, _b_sNo+2, "DEF  ");
  setFlickPanel(2, _b_sNo+3, "GHI  ");
  setFlickPanel(2, _b_sNo+4, "JKL  ");
  setFlickPanel(2, _b_sNo+5, "MNO  ");
  setFlickPanel(2, _b_sNo+6, "PQRS ");
  setFlickPanel(2, _b_sNo+7, "TUV  ");
  setFlickPanel(2, _b_sNo+8, "WXYZ ");
  setFlickPanel(2, _b_sNo+9, "^[$] ");//toggleボタンに
  setFlickPanel(2, _b_sNo+10, "'<\"> ");
  setFlickPanel(2, _b_sNo+11, ".,?! ");

  setFlickPanel(3, _b_sNo+0, "@#%&_");
  setFlickPanel(3, _b_sNo+1, "abc");
  setFlickPanel(3, _b_sNo+2, "def  ");
  setFlickPanel(3, _b_sNo+3, "ghi  ");
  setFlickPanel(3, _b_sNo+4, "jkl  ");
  setFlickPanel(3, _b_sNo+5, "mno  ");
  setFlickPanel(3, _b_sNo+6, "pqrs ");
  setFlickPanel(3, _b_sNo+7, "tuv  ");
  setFlickPanel(3, _b_sNo+8, "wxyz ");
  setFlickPanel(3, _b_sNo+9, "-(/) ");
  setFlickPanel(3, _b_sNo+10, ":;'\" ");
  setFlickPanel(3, _b_sNo+11, ".,?! ");

  setFlickPanel(4, _b_sNo+0, "0    ");
  setFlickPanel(4, _b_sNo+1, "1    ");
  setFlickPanel(4, _b_sNo+2, "2    ");
  setFlickPanel(4, _b_sNo+3, "3    ");
  setFlickPanel(4, _b_sNo+4, "4    ");
  setFlickPanel(4, _b_sNo+5, "5    ");
  setFlickPanel(4, _b_sNo+6, "6    ");
  setFlickPanel(4, _b_sNo+7, "7    ");
  setFlickPanel(4, _b_sNo+8, "8    ");
  setFlickPanel(4, _b_sNo+9, "9    ");
  setFlickPanel(4, _b_sNo+10, "+-*/ ");
  setFlickPanel(4, _b_sNo+11, ".(=) ");
  }

  void LovyanGFX_DentaroUI::setFlick( int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID )
  {
    setFlick( CHAR_3_BYTE, _TopBtnUiID, _FlickUiID, _LeftBtnUiID, _RightBtnUiID );
  }

  void LovyanGFX_DentaroUI::setFlick( int _charMode, int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID )
  {
    TopBtnUiID = _TopBtnUiID;
    FlickUiID = _FlickUiID;
    LeftBtnUiID = _LeftBtnUiID;
    RightBtnUiID = _RightBtnUiID;
    
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo,   "Nxt"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+1, "a/A"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+2, "SFT"  );
    setBtnName( uiBoxes[LeftBtnUiID].b_sNo+3, "゛小" );

    setBtnName( uiBoxes[RightBtnUiID].b_sNo,   "del" );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+1, "_"   );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+2, "Clr" );
    setBtnName( uiBoxes[RightBtnUiID].b_sNo+3, "Ret" );

    if(_charMode == CHAR_1_BYTE){
      fpNo=2;//英語モードに切り替える
    }else if(_charMode == NUMERIC){
      fpNo=4;//数値モードに切り替える
    }else{
      setCharMode(_charMode);//指定のパネルモードに切り替える
    }
    setUiLabels( FlickUiID, fpNo );
    // drawFlicks( getUiID("FLICK_1"), &_layoutSprite, _ui_sprite0 );
  }

String LovyanGFX_DentaroUI::getInvisibleFlickStrings(){
  String invisibleStr = "";
  //最後の文字のバイト数を判定する
  setlocale(LC_ALL, "");
  vector<string> ret = split_mb(flickStrDel.c_str(),"\n");

  if(ret.size() >= 1){

  for (size_t i = 0; i < ret.size()-1; i++) {
    invisibleStr += "*";
  }

  invisibleStr += String(ret[ret.size()-1].c_str());
  }

  return invisibleStr;
}

String LovyanGFX_DentaroUI::getFlickString(bool _visibleMode){
  if(_visibleMode == INVISIBLE)return getInvisibleFlickStrings();
  else return flickStr;
}

String LovyanGFX_DentaroUI::getFlickString(){
  return flickStr;
}

String LovyanGFX_DentaroUI::getFlickChar(){

  String RetChar = "";
  if(!selectModeF){
    RetChar = preFlickChar;
  }else if(selectModeF){
    RetChar = previewFlickChar;
  }
  return RetChar;
}

String LovyanGFX_DentaroUI::delEndChar(String _str, int _ByteNum){
  string stdFlickStr = _str.c_str();
  for( int i = 0; i < _ByteNum; i++ ){
    stdFlickStr.erase( --stdFlickStr.end() );
  }
  return stdFlickStr.c_str();
}

void LovyanGFX_DentaroUI::setLayoutPosToAllBtn( lgfx::v1::touch_point_t  _layoutPos ){
  for(int i=0; i < getAllBtnNum(); i++){
    touch_btn_list[ i ]->setlayoutSpritePos(_layoutPos);
  }
}

int LovyanGFX_DentaroUI::getXtile(){
  return xtile;
}

int LovyanGFX_DentaroUI::getYtile(){
  return ytile;
}

int LovyanGFX_DentaroUI::getZtile(){
  return ztile;
}

int LovyanGFX_DentaroUI::getXtileNo(){
  return xtileNo;
}

int LovyanGFX_DentaroUI::getYtileNo(){
  return ytileNo;
}

int LovyanGFX_DentaroUI::getXtileNo(int _objNo){
  return MapTiles[_objNo]->getXtileNo();
}

int LovyanGFX_DentaroUI::getYtileNo(int _objNo){
  return MapTiles[_objNo]->getYtileNo();
}

int LovyanGFX_DentaroUI::getPreXtileNo(int _objNo){
  return MapTiles[_objNo]->getPreXtileNo();
}

int LovyanGFX_DentaroUI::getPreYtileNo(int _objNo){
  return MapTiles[_objNo]->getPreYtileNo();
}

int LovyanGFX_DentaroUI::getPreXtileNo(){
  return preXtileNo;
}

int LovyanGFX_DentaroUI::getPreYtileNo(){
  return preYtileNo;
}

int LovyanGFX_DentaroUI::getPreXtile(){
  return preXtile;
}

int LovyanGFX_DentaroUI::getPreYtile(){
  return preYtile;
}


int LovyanGFX_DentaroUI::getPreDirID(){
  return preDirID;
}

int LovyanGFX_DentaroUI::getVx(){
  return vx;
}

int LovyanGFX_DentaroUI::getVy(){
  return vy;
}

int LovyanGFX_DentaroUI::getDirID(){
  

    vx = getXtile() - getPreXtile();
    vy = getYtile() - getPreYtile();

    float r = atan2( vy, vx );
    if (r < 0) { r = r + 2 * M_PI; }
    float vecAngle = r * 360 / (2 * M_PI);
    
    // preDirID = dirID;

         if(vecAngle < 15||vecAngle >= 345)   { dirID = 1;}
    else if(vecAngle >= 15  && vecAngle < 75 ){ dirID = 2; }
    else if(vecAngle >= 75  && vecAngle < 105){ dirID = 3; }
    else if(vecAngle >= 105 && vecAngle < 165){ dirID = 4; }
    else if(vecAngle >= 165 && vecAngle < 195){ dirID = 5; }
    else if(vecAngle >= 195 && vecAngle < 255){ dirID = 6; }
    else if(vecAngle >= 255 && vecAngle < 285){ dirID = 7; }
    else if(vecAngle >= 285 && vecAngle < 345){ dirID = 8; }

    return dirID;
}

void LovyanGFX_DentaroUI::setPreDirID(int _dirID){
  preDirID = _dirID;
}

// void LovyanGFX_DentaroUI::setMapNolist(int _spriteNo, int _xTileNo, int _yTileNo){
//   mapNolist[_spriteNo][0] = _xTileNo;
//   mapNolist[_spriteNo][1] = _yTileNo;
// }

// bool LovyanGFX_DentaroUI::existMapNolist(int _spriteNo, int _xTileNo, int _yTileNo){
//   if(mapNolist[_spriteNo][0] == _xTileNo&&mapNolist[_spriteNo][1] == _yTileNo){
//     return true;
//   }else{
//     return false;
//   }
// }

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define X_1 0
#define Y_1 1
#define X_2 2
#define Y_2 3

// (X_1,Y_1)
// o-------
// | \    |
// |  \   |
// |   \  |
// |    \ |
// -------o(X_2,Y_2)

bool LovyanGFX_DentaroUI::getDrawMapF(int _posId){
  // Serial.print(MapTiles[1]->getXtilePos());Serial.print(":");//[2]29100
  // Serial.print(MapTiles[3]->getXtilePos());Serial.print(":");//[2]29100
  // Serial.println(":");
  // Serial.print(MapTiles[5]->getYtilePos());Serial.print(":");//[2]29100

  //画面内に入っているタイルのみ描画をtrue
  //if(tilePositons[_spriteNo][X_1] <= SCREEN_WIDTH && tilePositons[_spriteNo][X_2] >= 0 && tilePositons[_spriteNo][Y_1] <= SCREEN_HEIGHT && tilePositons[_spriteNo][Y_2] >= 0){
  MapTiles[_posId]->setMapReadF(false);//一度リセット
  
  // if(MapTiles[_posId]->getXtilePos()<= SCREEN_WIDTH && MapTiles[_posId]->getXtilePos() > -256&&
  //   MapTiles[_posId]->getYtilePos() <= SCREEN_HEIGHT && MapTiles[_posId]->getYtilePos() > -256){
  //    MapTiles[_posId]->setMapReadF(true);
  // }
  if(MapTiles[_posId]->getXtilePos() < SCREEN_WIDTH  && MapTiles[_posId]->getXtilePos()+255 >= 0 &&
     MapTiles[_posId]->getYtilePos() < SCREEN_HEIGHT && MapTiles[_posId]->getYtilePos()+255 >= 0){
     MapTiles[_posId]->setMapReadF(true);
  }

  
  return MapTiles[_posId]->getMapReadF();
}

// void LovyanGFX_DentaroUI::setTilePosition(int _spriteNo, int _xtileNo, int _ytileNo, int _addXTileNo, int _addYTileNo)
// {

//   MapTiles[_spriteNo]->setXtileNo(_xtileNo);
//   MapTiles[_spriteNo]->setYtileNo(_ytileNo);

  // MapTiles[_spriteNo]->setAddX(_addXTileNo);
  // MapTiles[_spriteNo]->setAddY(_addYTileNo);

  // Serial.print(_xtileNo);Serial.print(":");//[2]29100
  // Serial.print(_ytileNo);Serial.print(":");//[2]12901

  // Serial.print(MapTiles[_spriteNo]->getXtile());Serial.print(":");//[2]7449636
  // Serial.print(MapTiles[_spriteNo]->getXtileNo());Serial.print(":");//[2]29100

  // Serial.print(MapTiles[2]->getYtile());Serial.print(":");//[2]3302891
  // Serial.print(MapTiles[2]->getYtileNo());Serial.print(":");//[2]12901

  // for(int i = 0; i < 9; i++){
  //   // Serial.print(MapTiles[i]->getXtileNo());
  //   // Serial.print(":");
  //   // Serial.print(MapTiles[i]->getYtileNo());
  //   // Serial.print(":");

  //   if(MapTiles[i]->getXtileNo() == _xtileNo && MapTiles[i]->getYtileNo() == _ytileNo){//タイルのXY番号(ID)が一致したら
  //     MapTiles[i]->setAddX(_addXTileNo);//9マスのどこに置くかを決める
  //     MapTiles[i]->setAddY(_addYTileNo);

  //     // MapTiles[i]->setXtile(getXtile());//9マスのどこに置くかを決める
  //     // MapTiles[i]->setYtile(getYtile());

  //     MapTiles[i]->setXtileNo(getXtileNo() + _addXTileNo);//9マスのどこに置くかを決める
  //     MapTiles[i]->setYtileNo(getYtileNo() + _addYTileNo);

  //     Serial.print(MapTiles[i]->getAddX());Serial.print(":");//[2]0
  //     Serial.print(MapTiles[i]->getAddY());Serial.print(":");//[2]0

  //     break;
  //   }
  // }
// }

// void LovyanGFX_DentaroUI::setTilePositions(int _spriteNo, int _X_1, int _Y_1){
//   int _X_2 = _X_1 + 255;
//   int _Y_2 = _Y_1 + 255;
//   tilePositons[_spriteNo][X_1] = _X_1;
//   tilePositons[_spriteNo][Y_1] = _Y_1;
//   tilePositons[_spriteNo][X_2] = _X_2;
//   tilePositons[_spriteNo][Y_2] = _Y_2;
// }

int LovyanGFX_DentaroUI::getPositionNo(int _addXTileNo, int _addYTileNo)//スプライトに高速描画
{
  int _posNo = 0;
       if(_addXTileNo ==  0 && _addYTileNo ==  0){_posNo = 0;}
  else if(_addXTileNo ==  1 && _addYTileNo ==  0){_posNo = 1;}
  else if(_addXTileNo ==  0 && _addYTileNo ==  1){_posNo = 2;}
  else if(_addXTileNo == -1 && _addYTileNo ==  0){_posNo = 3;}
  else if(_addXTileNo ==  0 && _addYTileNo == -1){_posNo = 4;}
  else if(_addXTileNo ==  1 && _addYTileNo == -1){_posNo = 5;}
  else if(_addXTileNo ==  1 && _addYTileNo ==  1){_posNo = 6;}
  else if(_addXTileNo == -1 && _addYTileNo ==  1){_posNo = 7;}
  else if(_addXTileNo == -1 && _addYTileNo == -1){_posNo = 8;}
  return _posNo;
}

int LovyanGFX_DentaroUI::getAddX(int _spriteNo)
{
  int addX = MapTiles[_spriteNo]->getAddX();
  return addX;
}

int LovyanGFX_DentaroUI::getAddY(int _spriteNo)
{
  int addY = MapTiles[_spriteNo]->getAddY();
  return addY;
}

void LovyanGFX_DentaroUI::setAddX(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setAddX(_xtileNo);
}

void LovyanGFX_DentaroUI::setAddY(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setAddY(_ytileNo);
}

//---------

int LovyanGFX_DentaroUI::getPreAddX(int _spriteNo)
{
  int addX = MapTiles[_spriteNo]->getPreAddX();
  return addX;
}

int LovyanGFX_DentaroUI::getPreAddY(int _spriteNo)
{
  int addY = MapTiles[_spriteNo]->getPreAddY();
  return addY;
}

void LovyanGFX_DentaroUI::setPreAddX(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setPreAddX(_xtileNo);
}

void LovyanGFX_DentaroUI::setPreAddY(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setPreAddY(_ytileNo);
}

//---------

void LovyanGFX_DentaroUI::setXtileNo(int _objId,  int _xtileNo)
{
  MapTiles[_objId]->setXtileNo(_xtileNo);
}

void LovyanGFX_DentaroUI::setPreXtileNo(int _objId,  int _preXtileNo)
{
  MapTiles[_objId]->setPreXtileNo(_preXtileNo);
}

void LovyanGFX_DentaroUI::setYtileNo(int _objId,  int _ytileNo)
{
  MapTiles[_objId]->setYtileNo(_ytileNo);
}

void LovyanGFX_DentaroUI::setPreYtileNo(int _objId,  int _preYtileNo)
{
  MapTiles[_objId]->setPreYtileNo(_preYtileNo);
}

void LovyanGFX_DentaroUI::setTileNo(int _objId, int _xtileNo, int _ytileNo)
{
  MapTiles[_objId]->setXtileNo(_xtileNo);
  MapTiles[_objId]->setYtileNo(_ytileNo);
}

// void LovyanGFX_DentaroUI::setMapName(int objId,  int _xtileNo,  int _ytileNo){
//   // MapTiles[_objId]->setXtileNo(_xtileNo);
//   // MapTiles[_objId]->setYtileNo(_ytileNo);
//   // MapTiles[_objId]->setMapName(_xtileNo, _ytileNo);
// }

void LovyanGFX_DentaroUI::setXtilePos(int _objId,  int _xtilePos)
{
  MapTiles[_objId]->setXtilePos(_xtilePos);
}

void LovyanGFX_DentaroUI::setYtilePos(int _objId,  int _ytilePos)
{
  MapTiles[_objId]->setYtilePos(_ytilePos);
}

int LovyanGFX_DentaroUI::getXtilePos(int _objId)
{
  return MapTiles[_objId]->getXtilePos();
}
int LovyanGFX_DentaroUI::getYtilePos(int _objId)
{
  return MapTiles[_objId]->getYtilePos();
}



void LovyanGFX_DentaroUI::setMapReadF(int _objId,  bool _mapReadF)
{
  MapTiles[_objId]->setMapReadF(_mapReadF);
}

void LovyanGFX_DentaroUI::setMapPosNo(int _objId, int _posNo)
{
  MapTiles[_objId]->setPosNo(_posNo);
}

int LovyanGFX_DentaroUI::getMapPosNo(int _objId)
{
  return MapTiles[_objId]->getPosNo();
}

void LovyanGFX_DentaroUI::setMapNoArray(int _objId, int _mapNo){
  mapNoArray[_objId] = _mapNo;
}

void LovyanGFX_DentaroUI::setMapNo(int _objId, int _mapNo)
{
  MapTiles[_objId]->setMapNo(_mapNo);
}

int LovyanGFX_DentaroUI::getMapNo(int _objId)
{
  return MapTiles[_objId]->getMapNo();
}

void LovyanGFX_DentaroUI::setPreMapNo(int _objId, int _preMapNo)
{
  MapTiles[_objId]->setPreMapNo(_preMapNo);
}

int LovyanGFX_DentaroUI::getPreMapNo(int _objId)
{
  return MapTiles[_objId]->getPreMapNo();
}

void LovyanGFX_DentaroUI::setExistF(int _objId, bool _existF){
  MapTiles[_objId]->setExistF(_existF);
}

bool LovyanGFX_DentaroUI::getExistF(int _objId){
  return MapTiles[_objId]->getExistF();
}

void LovyanGFX_DentaroUI::setDrawFinishF(int _objId, bool _drawFinishF){
  touch_btn_list[_objId]->setDrawFinishF(_drawFinishF);
}

bool LovyanGFX_DentaroUI::getDrawFinishF(int _objId){
  return touch_btn_list[_objId]->getDrawFinishF();
}


// #define BUF_PNG_NUM 9

// int addTPosList[9][2] = {
//   { 0, 0},{ 1, 0},{ 0, 1},
//   { -1,0},{ 0,-1},{ 1,-1},
//   {1, 1},{-1,1},{ -1,-1}
// };

// bool DownloadF = false;
// static LGFX_Sprite layoutSprite_list[BUF_PNG_NUM];

// void LovyanGFX_DentaroUI::drawMaps(){

//   for(int objId = 0; objId < BUF_PNG_NUM; objId++){
//     setMapNo(objId, objId);//位置番号を登録
    
//     setXtileNo(objId, getXtileNo() + addTPosList[objId][0]);
//     setYtileNo(objId, getYtileNo() + addTPosList[objId][1]);
    
//     setAddX(objId, addTPosList[objId][0]);
//     setAddY(objId, addTPosList[objId][1]);


//   }
// }

// LGFX_Sprite& LovyanGFX_DentaroUI::getLayoutSprite(int _no){
//   return &layoutSprite_list[_no];
// }


void LovyanGFX_DentaroUI::setDownloadF(bool _b)
{
  DownloadF = _b;
}

bool LovyanGFX_DentaroUI::getDownloadF()
{
  return DownloadF;
}

int LovyanGFX_DentaroUI::get_gPosId()
{
  return gPosId;
}

void LovyanGFX_DentaroUI::set_gPosId(int _gPosId)
{
  gPosId = _gPosId;
}

// int LovyanGFX_DentaroUI::getNowMapNo(int _addX, int _addY){
//   // for(int i = 0; i < BUF_PNG_NUM; i++){
//   //   if(addTPosList[i][0] == _addX && addTPosList[i][1] == _addY){
//   //     return i;
//   //   }
//   // }
// }

void LovyanGFX_DentaroUI::drawMaps(LGFX* _lcd, double _walkLatPos, double _walkLonPos, int _tileZoom){

  for(int objId = 0; objId < BUF_PNG_NUM; objId++)
    {
      setPreXtileNo(objId, getXtileNo(objId));//過去の名前（位置）を保存
      setPreYtileNo(objId, getYtileNo(objId));//過去の名前（位置）を保存
    }

  getTilePos(_walkLatPos, _walkLonPos, _tileZoom);//経緯度からタイル座標を計算

   //9枚のマップ座標取得
   for(int objId = 0; objId < BUF_PNG_NUM; objId++){
     setAddX(objId, addTPosList[objId][0]);//相対位置情報を登録
     setAddY(objId, addTPosList[objId][1]);//相対位置情報を登録
     setXtileNo(objId, getXtileNo() + getAddX(objId));//名前（位置）を登録
     setYtileNo(objId, getYtileNo() + getAddY(objId));//名前（位置）を登録

     matrix_list[objId][2] = 120 + (getAddX(objId)*255) - getXtile()%255;
     matrix_list[objId][5] = 120 + (getAddY(objId)*255) - getYtile()%255;
     setXtilePos(objId,  matrix_list[objId][2]);
     setYtilePos(objId,  matrix_list[objId][5]);
   }

   if(getXtileNo() != getPreXtileNo()||getYtileNo() != getPreYtileNo()){
     mataidaF = true;
     //Serial.println("mataida!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
   }else{
     mataidaF = false;
   }

   for(int objId = 0; objId < BUF_PNG_NUM; objId++)
   {
     //ディスプレイ内にはいっていて
     //まだ読み込んでいない場合
     setExistF(objId, false);
       for(int i=0; i < 9; i++){
         if(getXtileNo(objId) == preReadXtileNo[i] && getYtileNo(objId) == preReadYtileNo[i])
         {
           
           setExistF(objId, true);
           break;
         }else{
           
           setExistF(objId, false);
         }
       }
       
       if(getExistF(objId)==false)
       {
         
         preReadXtileNo[objId] = -1;
         preReadYtileNo[objId] = -1;

       }
     }

//--------------------------------------- 

   if(!mataidaF){
     //地図をまたいでいない時の処理
     //描画
     for(int objId = 0; objId < BUF_PNG_NUM; objId++)
     {
        for(int j = 0; j<6; j++ )matrix[j] = matrix_list[objId][j];//位置差分番号からスプライトを置く位置番号を特定し、実際の位置を登録
        layoutSprite_list[objId].pushAffine( _lcd, matrix );//0~9までのスプライトを差分位置に描画 
     }
   }
   
//--------------------------------------- 

   for(int objId = 0; objId < BUF_PNG_NUM; objId++)
   {
         if(mataidaF){
           //読み込む
           set_gPosId(objId);
 
           //上書き可能リストに、データ読み込み
           //マップID取得
           
           setDownloadF(true);//trueにするだけでタスク２が１回作動する

           while(getDownloadF() == true)
           {
           //読み込み完了を待つだけ
             delay(1);
           }

           drawTileAuto(
             objId + BUF_PNG_NUM,
             _lcd, 
             layoutSprite_list[objId], 
             TFT_ORANGE, 
             objId);

        //  Serial.print(objId);
        Serial.print("[");
        Serial.print(getXtileNo());
        Serial.print(":");
        Serial.print(getYtileNo());
        Serial.print("]");
        Serial.println("");

         preReadXtileNo[objId] = getXtileNo(objId);
         preReadYtileNo[objId] = getYtileNo(objId);
         setExistF(objId, true);
         }
  }
  
}


void LovyanGFX_DentaroUI::task2_setPngTile(int _posId)
{
    ROI_m_url = "/tokyo/" + String(getZtile()) + "/"+String(getXtileNo(_posId)) + "/"+String(getYtileNo(_posId)) + ".png";
    setPngTile( SD, ROI_m_url, _posId );//SDからの地図の読み込み

}