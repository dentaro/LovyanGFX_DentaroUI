#pragma once //インクルードガード

#include "LovyanGFX_DentaroUI.hpp"
#include "TouchBtn.hpp"
// #include <vector>
#include <deque>

#define MULTI_EVENT -1

#define SHOW_NAMED 0
#define SHOW_ALL 1

#define TOUCH_MODE 0
#define PHYSICAL_MODE 1
#define ANIME_MODE 2

#define TOGGLE_MODE true

#define PARENT_LCD 0
#define PARENT_SPRITE 1

#define ROW_MODE false
#define COL_MODE true

class RetClass {
public:
  int btnID = -1;
  lgfx::v1::touch_point_t sp;
  
  void reset(){
    btnID = -1;
    sp.x = 0;
    sp.y = 0;
  }

  int ret0(int _btnID){
    btnID = _btnID;
    return btnID;
  }
  
  void func01(int _btnID, int _x, int _y, int _eventState, int _runEventNo) {   // 関数プロトタイプ「void xxx(int,int,int,int);」
    btnID = _btnID;
    sp.x = _x;
    sp.y = _y;
//    if(runEventNo == eventState){//イベント状態が一致したら関数を実行
      if(_eventState!=-1&&_eventState!=0&&_eventState!=5&&_eventState!=6&&_eventState!=7&&_eventState!=8){//触れていないときNONEと、待機中WAIT、移動中MOVEのイベントは表示しない
        Serial.print("BUTTON_NO_"+String(_btnID));
        
        if(_eventState == 3)Serial.println(String("TAP"));
        if(_eventState == 4)Serial.println(String("W_TAP"));
        if(_eventState == 10)Serial.println(String("RIGHT"));
        if(_eventState == 12)Serial.println(String("UP"));
        if(_eventState == 14)Serial.println(String("LEFT"));
        if(_eventState == 16)Serial.println(String("DOWN"));

        if(_eventState == 11)Serial.println(String("RIGHT_UP"));
        if(_eventState == 13)Serial.println(String("LEFT_UP"));
        if(_eventState == 15)Serial.println(String("LEFT_DOWN"));
        if(_eventState == 17)Serial.println(String("RIGHT_DOWN"));
        
        if(_eventState == 2)Serial.println(String("DRAG"));
        
        //FrameEvent
        if(_eventState == 6)Serial.println(String("TOUCH"));
        if(_eventState == 7)Serial.println(String("WAIT"));
        if(_eventState == 8)Serial.println(String("MOVE"));
        if(_eventState == 9)Serial.println(String("RELEASE"));
        
      }
  }
};

class RetClass;

class UiContainer{
  public:
  int id = -1;
  int x = 0;
  int y = 0;
  int w = 0;
  int h = 0;
  int row = 0;
  int col = 0;
  int b_sNo = 0;
  int b_num = 0;
  int eventNo = -1; //MULTI_EVENT=-1
  int parentID = 0;
  String label = "";
  //std::vector<int, int> uiPos = {0, 0};
  //std::vector<std::pair<int, int>> uiPos ={ {x, y}, {w, h} };
};

struct Vec2{
    double x;
    double y;

    Vec2() = default; // デフォルトコンストラクタ

    Vec2(double _x, double _y) // コンストラクタ
    : x(_x)
    , y(_y) {}
};

class LovyanGFX_DentaroUI {
  
   private:
    lgfx::v1::touch_point_t tp;
    lgfx::v1::touch_point_t sp;
    lgfx::v1::touch_point_t sp2;
    lgfx::v1::touch_point_t layoutSpritePos; //レイアウト用
    lgfx::v1::touch_point_t lcdPos;//レイアウト用
    uint32_t btnState = B00000000;
    uint16_t touchState = B00000000;
    unsigned long sTime = 0;
    unsigned long tappedTime = 0;
    
    int eventState = -1;
    int tap_flick_thre = 82000;//タップとフリックの閾値
    int dist_thre = 40;
    int uiID = -1;
    int btnID = 0;
    int selectBtnID = -1;
    int selectBtnBoxID = -1;
    int runEventNo =  -1;
    int parentID = 0;//初期値0 =　PARENT_LCD
    bool availableF = false;
    int uiMode = TOUCH_MODE;
    std::deque<TouchBtn*> touch_btn_list; //ボタン配置用デック配列
    int timeCnt = 0;
    uint16_t clist[5] = {0,0,0,0,0};
    int preEventState = -1;
    int AngleCount = 0;
    int uiBoxes_num = 0;
    std::deque<UiContainer> uiBoxes;
    int layoutSprite_w = 0;
    int layoutSprite_h = 0;
    bool toggle_mode = false;
    LGFX_Sprite g_basic_sprite;
    String m_url = "";
    RetClass obj_ret;

    int sec, psec;
    int fps = 0;
    int frame_count = 0;

public:
    // LovyanGFX_DentaroUI( LGFX& _lcd ): lcd(_lcd) {};
    LovyanGFX_DentaroUI( LGFX* _lcd ): lcd(_lcd) {};
    LGFX* lcd;
    LovyanGFX* lgfx;
    void update( LGFX* _lcd );
    void begin(LGFX* _lcd);
    // void begin(LovyanGFX* _lgfx, int _mode);
    bool isAvailable(int _btnID);
    void addHandler(int _btnID, DelegateBase2* _func, uint16_t _runEventNo, int parentID = 0);
    void circle(LovyanGFX* _lgfx, uint16_t c, int fillF);
    void rect(LovyanGFX* _lgfx, uint16_t c, int fillF);
    float getAngle(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b );
    int getPreEvent();
    int getTouchEvent();
    void updateSelectBtnID(int _selectBtnID);
    lgfx::v1::touch_point_t getStartPos();//タッチされたスタート地点を取得
    lgfx::v1::touch_point_t getPos();//タッチしている座標を取得
    void setPos(int _x, int _y);
    void setStartPos(int _x, int _y);
    DelegateBase2 *ret0_DG = Delegate2<RetClass>::createDelegator2( &obj_ret, &RetClass::ret0 );//型式が違うプロトタイプ関数
    void createLayout( int _layoutSprite_x, int _layoutSprite_y, int _layoutSprite_w, int _layoutSprite_h, LGFX_Sprite& _layoutSprite, int _eventNo);
    void setBtnName(int _btnNo, String _btnName);
    void setBtnName(int _btnNo, String _btnName, String _btnNameFalse);//toggle用
    void setQWERTY(int _uiID, String _btnsString, LGFX_Sprite& _sprite);
    void createBtns( int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo);//縦方向に並ぶ
    void createBtns( int _uiSprite_x, int _uiSprite_y, int _w,int _h,int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo, bool _colF);//横方向に並ぶ
    void createToggles( int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo);//縦方向に並ぶ
    void createSliders( int _x, int _y, int _w, int _h,  int _row, int _col, LGFX_Sprite& _uiSprite, int _visible_mode, int _eventNo);
    void createTile( LGFX_Sprite& _layoutSprite, int _layoutUiID, int _eventNo);
    // void drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    // void drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite);
    void drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite);
    void drawBtns(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawToggles(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite);
    void drawSliders(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawSliders(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _uiSprite);
    void drawTile(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex);
    void drawLayOut(LGFX_Sprite& _layoutSprite);
    void drawSelectBtn(int _id);
    void setPngTile(fs::FS &fs, String _m_url);
    void setTilePos(int _id, int _x, int _y);
    LGFX_Sprite getTileSprite(int _btnID);
    int getTouchBtnID();//タッチされたボタンオブジェクトの番号を取得
    int getEvent();
    void showTouchEventInfo(LovyanGFX* _lgfx, int _x, int _y);
    lgfx::v1::touch_point_t getTouchPoint(int _x, int _y);
    float getSliderVal( int uiID, int _btnNo );
    float getSliderVal(int uiID,int _btnNo, int _xy);
    Vec2 getSliderVec2( int uiID, int _btnNo );

    void setSliderVal(int uiID, int _btnNo, float _x, float _y);
    bool getToggleVal(int _uiID, int _btnID);

    void setAllBtnAvailableF(int uiID, bool _available);
    void setAvailableF(int uiID, int _btnID, bool _available);

    void setAllBtnVisibleF(int uiID, bool _visible);
    void setVisibleF(int uiID, int _btnID, bool _visible);
    
    int getUiFirstNo(int _uiID);
    int getUiBtnNum(int _uiID);
    int getUiBoxNum();
    int getAllBtnNum();
    int getParentID();
    int getUiID( const char* _uiLabel );

    void showInfo(LGFX& _lgfx );
//  getTouchingDist();//タッチしている距離を取得
//  getTouchingTime();//タッチしている時間を取得
};
