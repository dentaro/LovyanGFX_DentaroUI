#include "LovyanGFX_DentaroUI.hpp"

//LovyanGFX_DentaroUI::LovyanGFX_DentaroUI(LGFX& _lcd)
//{
// lcd = _lcd;
//}

void LovyanGFX_DentaroUI::begin( LGFX* _lcd )
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
  if (_lcd->touch())
  {
    //renameFile(SPIFFS, "/config.txt", fileName);
    if (_lcd->width() < _lcd->height()) _lcd->setRotation(_lcd->getRotation() ^ 1);

    // 画面に案内文章を描画します。
    _lcd->setTextDatum(textdatum_t::middle_center);
    _lcd->drawString("touch the arrow marker.", _lcd->width()>>1, _lcd->height() >> 1);
    _lcd->setTextDatum(textdatum_t::top_left);

    // タッチを使用する場合、キャリブレーションを行います。画面の四隅に表示される矢印の先端を順にタッチしてください。
    std::uint16_t fg = TFT_WHITE;
    std::uint16_t bg = TFT_BLACK;
    
    if (_lcd->isEPD()) std::swap(fg, bg);
    _lcd->calibrateTouch(nullptr, fg, bg, std::max(_lcd->width(), _lcd->height()) >> 3);
  
    _lcd->fillScreen(TFT_BLACK);
    _lcd->setColorDepth(COL_DEPTH);
  
//     Serial.printf("heap_caps_get_free_size(MALLOC_CAP_SPIRAM)            : %6d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) );
//     Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
//     Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
//     Serial.printf("Width:%d, Height:%d\n", 256, 256);
// //    void *p = sprite1.createSprite(256, 256);
// //    if ( p == NULL ) {
// //      Serial.println("メモリが足りなくて確保できない");
// //    }
//     Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA):%d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
//     Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA):%d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
  }

  //m_url = "/tokyo/15/29094/"+String(12902)+".png";

  // this-> g_basic_sprite.setPsram(USE_PSRAM);
  // this-> g_basic_sprite.createSprite(256, 256);//子スプライトメモリ確保
  // this-> g_basic_sprite.setColorDepth(TILE_CHILD_COL_DEPTH);//子スプライトの色深度
  // this-> g_basic_sprite.setPaletteGrayscale();
  // this-> g_basic_sprite.drawPngFile(SD, m_url,
  //                               0, 0,
  //                               256, 256,
  //                               0, 0, 1.0, 1.0,
  //                               datum_t::top_left);
  Serial.println("");
  Serial.println("[UI_ID information]");
}

//void LovyanGFX_DentaroUI::update(LGFX& _lcd, int _mode){
//  uiMode = _mode;
//  update(_lcd);
//}

void LovyanGFX_DentaroUI::update( LGFX* _lcd )
{
  _lcd->getTouch(&tp);
  for(int i = 0; i < 4; i++)
  {
    clist[i+1] = clist[i];
  }
  if(eventState != NO_EVENT) preEventState = eventState;//前のイベント状態を保持

  //以下イベント取得処理
  eventState = NO_EVENT;//イベント初期化
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
      if(dist <= 40){
        eventState = WAIT;//5
      }
    }
    else if(touchedTime > 160000)
    {
      eventState = DRAG;//2
    }
    else if(dist <= 40){
      eventState = TAPPED;//4
      float dist2 = sqrt( pow((tp.x - sp2.x),2) + pow((tp.y - sp2.y),2) );
      if(sTime - tappedTime < 200000 && dist2 < 40 ){//ダブルタップの間の時間調整, 位置判定
        eventState = WTAPPED;//5
      }
      tappedTime = micros();
      sp2 = tp;
    }
    else if(dist > 40)
    {
       if(touchedTime <= 160000){
        float angle = getAngle(sp, tp);
  //      Serial.println(angle);
          if(angle <= 22.5 || angle > 337.5){
            eventState = RIGHT_FLICK;//0
          }else if(angle <= 67.5 && angle > 22.5){
            eventState = R_D_FLICK;//7
          }else if(angle <= 112.5 && angle > 67.5){
            eventState = DOWN_FLICK;//6
          }else if(angle <= 157.5 && angle > 112.5){
            eventState = D_L_FLICK;//5
          }else if(angle <= 202.5 && angle > 157.5){
            eventState = LEFT_FLICK;//4
          }else if(angle <= 247.5 && angle > 202.5){
            eventState = L_U_FLICK;//3
          }else if(angle <= 292.5 && angle > 247.5){
            eventState = UP_FLICK;//2
          }else if(angle <= 337.5 && angle > 292.5){
            eventState = U_R_FLICK;//1
          }
       }
    }
  }else if (touchState == B_UNTOUCH) {
    clist[0] = TFT_DARKGREEN;
  }

  for(int i = 0; i < uiBoxes_num; i++){
    for(int j = 0; j<uiBoxes[i].b_num; j++){
      addHandler(uiBoxes[i].b_sNo + j, ret0_DG, uiBoxes[i].eventNo, uiBoxes[i].parentID);
    }
  }
}

void LovyanGFX_DentaroUI::updateSelectBtnID(int _selectBtnID){
 selectBtnID = _selectBtnID;
}

void LovyanGFX_DentaroUI::showTouchEventInfo(LovyanGFX* _lgfx, int _x, int _y){
  for(int i = 0; i < 5; i++){
    _lgfx->fillRect(_x + 1 + 10*i, _y, 9,9,clist[0]);
  }
}

void LovyanGFX_DentaroUI::createToggles(int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo){
  toggle_mode = true;
  createBtns( _uiSprite_x, _uiSprite_y, _w, _h, _row, _col, _uiSprite, _eventNo);
  toggle_mode = false;
}

void LovyanGFX_DentaroUI::createBtns(int _uiSprite_x, int _uiSprite_y, int _w,int _h,int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo){//縦方向に並ぶ
  uiBoxes_num++;
  uiID++;
  uiBoxes[uiID].label = "BTN_" + String(uiID);
  Serial.println("BTN_" + String(uiID)  + "=[" + String(uiID) + "]");
  
  int _startId = btnID;//スタート時のボタンIDをセット
  uiBoxes[uiID].b_sNo = btnID;
  uiBoxes[uiID].id  = uiID;
  uiBoxes[uiID].x   = _uiSprite_x;
  uiBoxes[uiID].y   = _uiSprite_y;
  uiBoxes[uiID].w   = _w;
  uiBoxes[uiID].h   = _h;
  uiBoxes[uiID].row = _row;
  uiBoxes[uiID].col = _col;
  uiBoxes[uiID].eventNo = _eventNo;
  // uiBoxes[uiID].toggle_mode = toggle_mode;
  // uiBoxes[uiID].parentID = _parentID;
  
  _uiSprite.setPsram(USE_PSRAM);
  //_uiSprite.setPsram(false);
  _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );
  _uiSprite.setColorDepth( COL_DEPTH );

  int b_w = int(uiBoxes[uiID].w/uiBoxes[uiID].row);
  int b_h = int(uiBoxes[uiID].h/uiBoxes[uiID].col);

  for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    for(int j= 0; j < uiBoxes[uiID].col; j++)
    {
    touch_btn_list[_startId + j * uiBoxes[uiID].row + i] = NULL;
    touch_btn_list[_startId + j * uiBoxes[uiID].row + i] = new TouchBtn( lcd ); 
    touch_btn_list[_startId + j * uiBoxes[uiID].row + i]->initBtn(_startId + j*uiBoxes[uiID].row + i,
    i*b_w,
    j*b_h,
    b_w,
    b_h, 
    String(_startId + j*uiBoxes[uiID].row + i), 
    layoutSpritePos, 
    getTouchPoint(_uiSprite_x, _uiSprite_y), 
    _uiSprite,
    toggle_mode);
    // touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setAvailableF(_btnAvailableF);
    touch_btn_list[_startId + j*uiBoxes[uiID].row + i]->setAvailableF(true);//ベータ版は基本true
    btnID++;//ボタンを更新
    }
  }
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
}

void LovyanGFX_DentaroUI::createBtns( int _x, int _y, int _w,int _h,int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo, bool _colMode){//縦方向に並ぶ
  if(!_colMode){
    createBtns( _x, _y, _w, _h, _row, _col, _uiSprite, _eventNo);
  }else if(!_colMode){
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
  //_uiSprite.setPsram( false );
  _uiSprite.createSprite( uiBoxes[uiID].w, uiBoxes[uiID].h );
  _uiSprite.setColorDepth( COL_DEPTH );
  
  //btnNum = _row * _col ;
  int b_w = int( uiBoxes[uiID].w / uiBoxes[uiID].row );
  int b_h = int( uiBoxes[uiID].h / uiBoxes[uiID].col );
  
  for(int j= 0; j < uiBoxes[uiID].col; j++)
  {
    for(int i= 0; i < uiBoxes[uiID].row; i++)
    {
    btnID = _startId + i*uiBoxes[uiID].col + j;
    touch_btn_list[_startId + i*uiBoxes[uiID].col + j] = NULL;
    touch_btn_list[_startId + i*uiBoxes[uiID].col + j] = new TouchBtn( lcd ); 
    
    touch_btn_list[_startId + i*uiBoxes[uiID].col + j]->initBtn( _startId + i * uiBoxes[uiID].col + j,
    i*b_w, 
    j*b_h, 
    b_w, 
    b_h, 
    String(_startId + i*_col + j), 
    layoutSpritePos, 
    getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y),
    _uiSprite, 
    toggle_mode);
    
    touch_btn_list[_startId + i * uiBoxes[uiID].col + j]->setAvailableF(false);
    // touch_btn_list[_startId + j * uiBoxes[uiID].row + i]->setAvailableF( _btnAvailableF );
    touch_btn_list[_startId + j * uiBoxes[uiID].row + i]->setAvailableF( true );//ベータ版は基本true
    btnID++;
    }
  }
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;//UIのボタン数をセット
  }
}

void LovyanGFX_DentaroUI::createLayout( int _x, int _y, int _w, int _h, LGFX_Sprite& _layoutSprite, int _eventNo){ 
    uiBoxes_num++;
    uiID++;
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
    
    _layoutSprite.setPsram(false);
    _layoutSprite.createSprite(layoutSprite_w, layoutSprite_h);
    _layoutSprite.setColorDepth(TILE_COL_DEPTH);//親スプライトの色深度１６
    _layoutSprite.setPaletteGrayscale();
}

void LovyanGFX_DentaroUI::createSliders(int _x, int _y, int _w, int _h,  int _row, int _col,
LGFX_Sprite& _uiSprite, 
int _xy_mode, 
int _eventNo)
{ 
  
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
  _uiSprite.createSprite( _w, _h );
  _uiSprite.setColorDepth( COL_DEPTH );

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
    touch_btn_list[_id + i]->setAvailableF(true);
    touch_btn_list[_id + i]->setBtnName(String(btnChars[i]));
    //touch_btn_list[_id + i]->setColor(_bgColIndex);
  }
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName, String _btnNameFalse){
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setBtnName(_btnName);
  touch_btn_list[_btnId]->setBtnNameFalse(_btnNameFalse);
}

void LovyanGFX_DentaroUI::setBtnName( int _btnNo, String _btnName)
{
  int _btnId = _btnNo;
  touch_btn_list[_btnId]->setAvailableF(true);
  touch_btn_list[_btnId]->setBtnName(_btnName);
  //touch_btn_list[_btnId]->setColor(_bgColIndex);
}

void LovyanGFX_DentaroUI::drawBtns(int _uiID, LovyanGFX *_lgfx, LGFX_Sprite& _uiSprite)
{
  drawBtns(_uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawBtns( int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y)
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
  _uiSprite.pushSprite(lgfx, uiBoxes[uiID].x, uiBoxes[uiID].y);
  }
  }
}

void LovyanGFX_DentaroUI::drawSliders(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite)
{
  drawSliders(_uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}
void LovyanGFX_DentaroUI::drawSliders(int uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y)
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
  _uiSprite.pushSprite(_lgfx, _uiSprite_x, _uiSprite_y);
  }
  }
  //_layoutSprite.pushSprite(layoutSpritePos.x, layoutSpritePos.y);//最終的な出力
}

void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite){
  drawToggles(_uiID, _lgfx, _uiSprite, uiBoxes[_uiID].x, uiBoxes[_uiID].y);
}

void LovyanGFX_DentaroUI::drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y){
  toggle_mode = true;
  drawBtns(_uiID, _lgfx, _uiSprite, _uiSprite_x, _uiSprite_y);
  toggle_mode = false;
}

void LovyanGFX_DentaroUI::createTile( LGFX_Sprite& _layoutSprite, int _layoutUiID, int _eventNo){//横方向に並ぶ
  uiBoxes_num++;
  uiID++;
  Serial.println("TILE_ID_"+String(uiID) + " " + String(uiID));
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

  touch_btn_list[_startId] = NULL;
  touch_btn_list[_startId] = new TouchBtn(lcd);

  touch_btn_list[_startId] -> initTile(_startId, String(_startId), getTouchPoint(uiBoxes[uiID].x, uiBoxes[uiID].y), uiBoxes[uiID].w, uiBoxes[uiID].h, _layoutSprite, g_basic_sprite);

  
  uiBoxes[uiID].b_num =  btnID - uiBoxes[uiID].b_sNo;
}

void LovyanGFX_DentaroUI::setPngTile(fs::FS &fs, String _m_url){
  this->g_basic_sprite.drawPngFile(fs, _m_url,
                                0, 0,
                                256, 256,
                                0, 0, 1.0, 1.0,
                                datum_t::top_left);
}


void LovyanGFX_DentaroUI::drawTile(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex)//スプライトに描画
{
  if( getEvent() != NO_EVENT ){
  if( getEvent() == uiBoxes[uiID].eventNo || uiBoxes[uiID].eventNo == MULTI_EVENT)
  {
    int _id = uiBoxes[uiID].b_sNo;
    touch_btn_list[_id]->setAvailableF(true);
    touch_btn_list[_id]->tileDraw( _lgfx, _layoutSprite, layoutSpritePos, sp, _bgColIndex, g_basic_sprite);
  }
  }
}

void LovyanGFX_DentaroUI::drawLayOut(LGFX_Sprite& _layoutSprite){
  if(getEvent() == uiBoxes[uiID].eventNo)
  {
    _layoutSprite.pushSprite(layoutSpritePos.x, layoutSpritePos.y);//最終的な出力
  }
}

bool LovyanGFX_DentaroUI::isAvailable( int _btnID ){
  return touch_btn_list[_btnID]->getAvailableF();
}

void LovyanGFX_DentaroUI::addHandler( int _btnID, DelegateBase2* _func, uint16_t _runEventNo, int _parentID){

  runEventNo = _runEventNo;
  //btnID = _btnID;
  _parentID = 0;//ベータ版ではとりあえず強制的にLCDのみのイベントをとる
  
  touch_btn_list[_btnID]->addHandler(_func);
  if( parentID == PARENT_LCD ) touch_btn_list[_btnID]->run2( _btnID, sp, tp, eventState, runEventNo, lcdPos );//ボタン内のイベントかチェック
  else if( parentID == PARENT_SPRITE ) touch_btn_list[_btnID]->run2( _btnID, sp, tp, eventState, runEventNo, layoutSpritePos );//ボタン内のイベントかチェック
  
//  if(_btnID >= 0 && _btnID < 40 ){//キーボード
//    touch_btn_list[_btnID].run2( _btnID, sp, tp, eventState, runEventNo, lcdPos );//ボタン内のイベントかチェック
//  }
//  else if(_btnID == 40 || _btnID == 41 ){//＋‐ボタン
//    touch_btn_list[_btnID].run2( _btnID, sp, tp, eventState, runEventNo, layoutSpritePos );//ボタン内のイベントかチェック
//  }
//  else if(_btnID == 42 ){//Tile
//    touch_btn_list[_btnID].run2( _btnID, sp, tp, eventState, runEventNo, layoutSpritePos );//ボタン内のイベントかチェック
//  }
//  else if(_btnID == 43 || _btnID == 44 ){//SLIDER
//    touch_btn_list[_btnID].run2( _btnID, sp, tp, eventState, runEventNo, lcdPos );//ボタン内のイベントかチェック
//  }
//  else if(_btnID == 45 || _btnID == 46 ){//TOGGLE
//    touch_btn_list[_btnID].run2( _btnID, sp, tp, eventState, runEventNo, lcdPos );//ボタン内のイベントかチェック
//  }
  touch_btn_list[_btnID]->delHandlers2();
}

float LovyanGFX_DentaroUI::getAngle(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b ){

  float r = atan2( b.y - a.y, b.x - a.x );
  if (r < 0) {
        r = r + 2 * M_PI;
    }
    return r * 360 / (2 * M_PI);
}

int LovyanGFX_DentaroUI::getPreEvent()
{
  return preEventState;
}

int LovyanGFX_DentaroUI::getTouchEvent()
{
  return eventState;
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

int LovyanGFX_DentaroUI::getEvent(){
  return eventState;
}

LGFX_Sprite LovyanGFX_DentaroUI::getTileSprite(int _btnID)
{
  return g_basic_sprite;
}

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

//float LovyanGFX_DentaroUI::getSliderVal(int _btnID, int _xy){
//  return touch_btn_list[_btnID].sliderValy;
//}

float LovyanGFX_DentaroUI::getSliderVal(int uiID,int _btnNo){
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

void LovyanGFX_DentaroUI::setSliderVal(int uiID, int _btnNo, float _x, float _y){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  touch_btn_list[_btnID]->setSliderVal(_x, _y);
}


bool LovyanGFX_DentaroUI::getToggleVal(int _uiID, int _btnNo){
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  return touch_btn_list[_btnID]->getToggleVal();
}

lgfx::v1::touch_point_t LovyanGFX_DentaroUI::getSliderPos(int uiID,int _btnNo){
  lgfx::v1::touch_point_t  retPos;
  int _btnID = uiBoxes[uiID].b_sNo + _btnNo;
  if(touch_btn_list[_btnID]->get_xy_mode() == XY_VAL){getTouchPoint(touch_btn_list[_btnID]->getSliderValx(), touch_btn_list[_btnID]->getSliderValy());}
  return retPos;
}

int LovyanGFX_DentaroUI::getUiFirstNo(int uiID){
  return uiBoxes[uiID].b_sNo;
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


//void LovyanGFX_DentaroUI::setSliderVal(int _btnID){
//  touch_btn_list[_btnID].sliderValx = tp.x - uiSpritePos.x;
//  touch_btn_list[_btnID].sliderValy = tp.y - uiSpritePos.y;
//}
