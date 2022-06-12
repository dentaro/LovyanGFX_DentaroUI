#pragma once //インクルードガード

#include "LovyanGFX_DentaroUI.hpp"
#include "TouchBtn.hpp"
#include <deque>

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#include <limits.h>
#include <stddef.h>
#include <vector>
using namespace std;

//開発者表記
#define CHAR_1_BYTE_5 5//=EN 5文字
#define CHAR_1_BYTE_9 9//=EN 9文字
#define CHAR_3_BYTE_5 15//=JP 5文字
#define CHAR_3_BYTE_9 27//=JP 9文字


//開発者表記
#define CHAR_3_BYTE 0//=JP
#define CHAR_1_BYTE 2//=EN
#define NUMERIC 4
//ユーザー表記(Arduino側でしか使わない)
#define JP 0
#define EN 2



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

#define SHIFT_NUM 5//シフトパネルの数
#define HENKAN_NUM 57

#define VISIBLE true
#define INVISIBLE false

class RetClass {
public:
  int btnID = -1;
  int btnNo = -1;
  lgfx::v1::touch_point_t sp;
  
  void reset(){
    btnID = -1;
    btnNo = -1;
    sp.x = 0;
    sp.y = 0;
  }

  int ret0(int _btnID){
    btnID = _btnID;
    return btnID;
  }

  int ret1(int _btnNo){
    btnNo = _btnNo;
    return btnNo;
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

class FlickPanel
{
  // private:
  public:
  int b_No = -1;
  std::deque<String> text_list; //パネルテキスト配置用デック配列
  FlickPanel(){};

  void new_text_list(int _btnID, String _btnsString )
  {
    if(_btnID==0)text_list[_btnID] = _btnsString;
    else text_list.push_back(_btnsString);
    //text_list[_btnID] = _btnsString;
  }

};

class LovyanGFX_DentaroUI {
  
   private:
   
    String kanalist[HENKAN_NUM][3] = {
  {"あ","ぁ","＿"},
  {"い","ぃ","ゐ"},
  {"う","ぅ","＿"},
  {"え","ぇ","ゑ"},
  {"お","ぉ","＿"},
  {"か","が","＿"},
  {"き","ぎ","＿"},
  {"く","ぐ","＿"},
  {"け","げ","＿"},
  {"こ","ご","＿"},
  
  {"さ","ざ","＿"},
  {"し","じ","＿"},
  {"す","ず","＿"},
  {"せ","ぜ","＿"},
  {"そ","ぞ","＿"},
  {"た","だ","＿"},
  {"ち","ぢ","＿"},
  {"つ","っ","づ"},
  {"て","で","＿"},
  {"と","ど","＿"},
  
  {"は","ば","ぱ"},
  {"ひ","び","ぴ"},
  {"ふ","ぶ","ぷ"},
  {"へ","べ","ぺ"},
  {"ほ","ぼ","ぽ"},
  {"や","ゃ","＿"},
  {"ゆ","ゅ","＿"},
  {"よ","ょ","＿"},
  {"ー","～","＿"},

  {"ア","ァ","＿"},
  {"イ","ィ","ヰ"},
  {"ウ","ゥ","＿"},
  {"エ","ェ","ヱ"},
  {"オ","ォ","＿"},
  {"カ","ガ","＿"},
  {"キ","ギ","＿"},
  {"ク","グ","＿"},
  {"ケ","ゲ","＿"},
  {"コ","ゴ","＿"},
  
  {"サ","ザ","＿"},
  {"シ","ジ","＿"},
  {"ス","ズ","＿"},
  {"セ","ゼ","＿"},
  {"ソ","ゾ","＿"},
  {"タ","ダ","＿"},
  {"チ","ヂ","＿"},
  {"ツ","ッ","ヅ"},
  {"テ","デ","＿"},
  {"ト","ド","＿"},
  
  {"ハ","バ","パ"},
  {"ヒ","ビ","ピ"},
  {"フ","ブ","プ"},
  {"ヘ","ベ","ペ"},
  {"ホ","ボ","ポ"},
  {"ヤ","ャ","＿"},
  {"ユ","ュ","＿"},
  {"ヨ","ョ","＿"},
};

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
    int flickState = -1;
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
    std::deque<TouchBtn*> flick_touch_btn_list; //フリックボタン配置用デック配列
    std::deque<FlickPanel*> flickPanels;
    int showFlickPanelNo = 0;

    int timeCnt = 0;
    uint16_t clist[5] = {0,0,0,0,0};
    int preEventState = -1;
    int AngleCount = 0;
    int uiBoxes_num = 0;
    // std::deque<UiContainer> uiBoxes;
    std::deque<UiContainer> uiBoxes;
    // UiContainer uiBoxes[18];
    UiContainer flickPanel;
    int layoutSprite_w = 0;
    int layoutSprite_h = 0;
    bool toggle_mode = false;
    // LGFX_Sprite g_basic_sprite;
    // LGFX_Sprite g_basic_sprite_list[9];
    String m_url = "";
    RetClass obj_ret;
    int shiftNum = 3;
    int charMode = CHAR_3_BYTE; //日本語
    //int charMode = CHAR_1_BYTE; //英語

    int charNumMode = CHAR_3_BYTE_5;//日本語5文字
    int sec, psec;
    int fps = 0;
    int frame_count = 0;
    bool use_flickUiSpriteF = false;
    int charNo=0;
    String flickString = "";
    String flickStr = "";
    String flickStrDel = "";
    String preFlickChar = "";
    String previewFlickChar = "";
    String finalChar ="";
    int fpNo = 0;
    int kanaShiftNo = 0;
    bool selectModeF = false;
    int curbtnID;//現在の行番号
    int curKanaRowNo = 0;
    int curKanaColNo = 0;
    bool touchCalibrationF = false;

    int TopBtnUiID = 0;
    int LeftBtnUiID = 0;
    int RightBtnUiID = 0;
    int FlickUiID = 0;

    bool uiOpenF = true;

public:
    // LovyanGFX_DentaroUI( LGFX& _lcd ): lcd(_lcd) {};
    // LovyanGFX_DentaroUI( LGFX* _lcd );
    // LovyanGFX_DentaroUI();
    LovyanGFX_DentaroUI();
    // LovyanGFX_DentaroUI( LGFX* _lcd ): lcd(_lcd) {};
    //LGFX* lcd;//タッチポイントとれる
    //LovyanGFX* lgfx;//グラフィックのみ
    // LGFX_Sprite flickUiSprite;//フリック展開パネル用
    void update( LGFX& _lcd );
    void begin( LGFX& _lcd, int _shiftNum, int _colBit, int _rotateNo, bool _calibF );
    void begin(LGFX& _lcd);
    void drawLayOut(LGFX& _lcd, LGFX_Sprite& _layoutSprite);
    
    void touchCalibration (bool _calibUseF);
    bool isAvailable(int _btnID);
    void addHandler(int _btnID, int _btnNo, DelegateBase2* _func, uint16_t _runEventNo, int parentID = 0);
    void circle(LovyanGFX* _lgfx, uint16_t c, int fillF);
    void rect(LovyanGFX* _lgfx, uint16_t c, int fillF);
    float getAngle(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b );
    float getDist(lgfx::v1::touch_point_t a, lgfx::v1::touch_point_t b );
    int getPreEvent();
    // int getTouchEvent();
    void updateSelectBtnID(int _selectBtnID);
    lgfx::v1::touch_point_t getStartPos();//タッチされたスタート地点を取得
    lgfx::v1::touch_point_t getPos();//タッチしている座標を取得
    void setPos(int _x, int _y);
    void setStartPos(int _x, int _y);
    DelegateBase2 *ret0_DG = Delegate2<RetClass>::createDelegator2( &obj_ret, &RetClass::ret0 );//型式が違うプロトタイプ関数

    DelegateBase2 *ret1_DG = Delegate2<RetClass>::createDelegator2( &obj_ret, &RetClass::ret1 );//型式が違うプロトタイプ関数

    void setLayoutPos( int _x, int _y );

    void createLayout( int _layoutSprite_x, int _layoutSprite_y, int _layoutSprite_w, int _layoutSprite_h, LGFX_Sprite& _layoutSprite, int _eventNo);
    void setLayoutSpritePos( int _LayoutSpritePosx, int _LayoutSpritePosy );
    
    void setLayoutPosToAllBtn( lgfx::v1::touch_point_t  _layoutPos );

    void setBtnName(int _btnNo, String _btnName);
    void setBtnName(int _btnNo, String _btnName, String _btnNameFalse);//toggle用
    void setQWERTY(int _uiID, String _btnsString, LGFX_Sprite& _sprite);
    void createBtns( int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo);//縦方向に並ぶ
    void createBtns( int _uiSprite_x, int _uiSprite_y, int _w,int _h,int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo, bool _colF);//横方向に並ぶ
    void createToggles( int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo);//縦方向に並ぶ
    void createSliders( int _x, int _y, int _w, int _h,  int _row, int _col, LGFX_Sprite& _uiSprite, int _visible_mode, int _eventNo);
    // void createTile( LGFX_Sprite& _layoutSprite, int _layoutUiID, int _eventNo, int _spriteNo);
    void createFlicks( int _uiSprite_x, int _uiSprite_y, int _w,int _h, int _row, int _col, LGFX_Sprite& _uiSprite, int _eventNo);//フリック生成
    void createFlick9Btns(LGFX_Sprite& _flickUiSprite);
    void drawFlickBtns( LovyanGFX& _lgfx,  LGFX_Sprite& _flickUiSprite, int _btnID, int _btnNo, bool _visible, bool _available );
    // void setFlickPanel(int _flickPanelID, int _btnID, String _btnsString, int _btn_mode);
    void setFlickPanel( int _flickPanelID, int _btnID, String _btnsString );
    void drawFlicks(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite);
    void drawFlicks( int uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawBtns(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite);
    void drawBtns(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawToggles(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawToggles(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite);
    void drawSliders(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite, int _uiSprite_x, int _uiSprite_y);
    void drawSliders(int _uiID, LovyanGFX& _lgfx, LGFX_Sprite& _uiSprite);
    // void drawTile(int _uiID, LovyanGFX* _lgfx, LGFX_Sprite& _layoutSprite, uint8_t _bgColIndex, int _spriteNo);
    
    void drawSelectBtn(int _id);
    // void setPngTile(fs::FS &fs, String _m_url, int _spriteNo);
    // void setTilePos(int _id, int _x, int _y);
    // LGFX_Sprite getTileSprite(int _btnID);
    int getTouchBtnNo();//タッチされたボタンオブジェクトの番号を取得
    int getTouchBtnID();//タッチされたボタンオブジェクトのIDを取得
    int getEvent();
    int getFlickEvent();
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

    void setCharMode(int _charMode);
    int getCharMode();

    void changeBtnMode(int _uiID, int _btnID, int _btn_mode);

    String getFlickStr();

    int getflickPanelBtnNo(int uiID);
    
    int getUiFirstNo(int _uiID);
    int getUiBtnNum(int _uiID);
    int getUiBoxNum();
    int getAllBtnNum();
    int getParentID();
    int getUiID( const char* _uiLabel );

    // void setShowFlickPanelNo(int _showFlickPanelNo);
    void setUiLabels(int uiID, int _showFlickPanelNo);
    lgfx::v1::touch_point_t getBtnPos(int _btnID);

    //void showInfo(LGFX& _lgfx );
    void showInfo(LGFX& _lgfx , int _infox, int _infoy);
    // void showInfo( LovyanGFX* _lgfx );
    void showInfo( LovyanGFX* _lgfx, int _infox, int _infoy);

    String getHenkanChar(int _henkanListNo, int _kanaShiftNo);
    void setFlickPanels();//キーボード用プリセット
    void setFlick( int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID);//キーボード用プリセッ；
    void setFlick( int _charMode, int _TopBtnUiID, int _FlickUiID, int _LeftBtnUiID, int _RightBtnUiID );
    void flickSetup( LGFX_Sprite& _layoutSprite, 
                      LGFX_Sprite& _ui_sprite0, LGFX_Sprite& _ui_sprite1, LGFX_Sprite& _ui_sprite2, LGFX_Sprite& _ui_sprite3, LGFX_Sprite& _flickUiSprite);
    void flickUpdate( LGFX& _lcd, LGFX_Sprite& _layoutSprite, 
                      LGFX_Sprite& _ui_sprite0, LGFX_Sprite& _ui_sprite1, LGFX_Sprite& _ui_sprite2, LGFX_Sprite& _ui_sprite3, LGFX_Sprite& _flickUiSprite);
    String getInvisibleFlickStrings();
    String getFlickString();
    String getFlickString(bool _visibleMode);
    String getFlickChar();
    String getKana(int _panelID, int _rowID, int _colID, int _transID);

    const char* next_c_mb(const char* c);
    void ngetc(char* const dst,const char* src);
    bool nchr_cmp(const char* c1, const char* c2);
    std::vector<std::string> split_mb(const char* src, const char* del);

    // std::vector<std::string> delete_mb(const char* src, const char* del);
    String delEndChar(String _str, int _ByteNum);

    void delChar();

//  getTouchingDist();//タッチしている距離を取得
//  getTouchingTime();//タッチしている時間を取得
};
