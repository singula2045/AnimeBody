#if !defined(BODYMODEL_H)
#define BODYMODEL_H

class CBodyView;

/////////////////////////////////////////////////////////////////////////////
// CBodyModel

// 制御点構造体

struct CModelPoint
{
  CVector3 vPos;        // 各制御点位置
  CVector3 vOldPos;     // 一つ前の制御点位置
  CVector3 vInitPos;    // 各制御点の初期位置
  BOOL bUse;
};

class CBodyModel
{
// アトリビュート
private:
  char name[16];                    // 名前
  CModelPoint mPos[cPosMax];        // 各制御点
  CVector3 vLight; // 光源方向
  double distance[cLinkMax];        // 各関連間の距離

  double head_age;       // 頭年齢
  double head_h;         // 頭部縦横比
  double head_figure;    // 頭部形状（卵型係数）
  double jaw_figure;     // あご形状
  double eye_size;       // 目の大きさ
  int eye_height;        // 目の開き具合
  double eye_gap;        // 両目の間隔
  double eye_angle;      // 目の角度（吊目・垂目）
  BOOL double_eyelid;    // 二重まぶたか
  BOOL eye_move;         // 眼球移動するか
  BOOL wink;             // まばたきするか

  double eye_in_r;       // 黒目の大きさ
  double eye_light_r;    // 目のハイライトの大きさ
  double eye_line;       // 目の高さ
  double head_r;         // 頭の基準長
  int eye_timer;         // 目ぱち用タイマー		  

  CVector3 head_pv[16][16];      // 頭部ポリゴン座標
  CVector3 head_n0[16][16];      // 頭部座標での法線
  CVector3 head_tp;              // 頭頂座標
  CVector3 head_cp;              // 頭中央座標

  BOOL hair_init;                // 髪の毛を初期化したか？
  CVector3 hair_p[2][20][16];    // 髪の毛頂点座標

  BOOL skirt_init;               // スカートを初期化したか？
  CVector3 skirt_p[20][16];      // スカートの頂点座標

  BOOL skin;                     // 裸か？

  double hair_vx;                // 開始速度
  double hair_vy;
  double hair_gx;                // 加速度
  double hair_gy;
  double hair_angle;             // カット角度
  int hair_max[16];              // カット位置

  double hair_vx2;               // 開始速度(前髪)
  double hair_vy2;
  double hair_gx2;               // 加速度(前髪)
  double hair_gy2;
  double hair_angle2;            // カット角度(前髪)
  int hair_max2[16];             // カット位置(前髪)

// オペレーション
public:
  CBodyModel();
  ~CBodyModel();

  // オブジェクト初期化
  void Init();

  // 制御点位置を返す
  CVector3 Pos(int no);

  // 制御点位置を変更
  void SetPos(int no, CVector3& pos);

  // 制御点位置を移動
  void MovePos(int no, CVector3& pos);

  //各パラメータへのアクセサ
  void Name(char* buf);
  void SetName(char* a_name);
  double HeadAge();
  void HeadAge(double a_head_age);
  double HeadH();
  void HeadH(double a_head_h);
  double HeadFigure();
  void HeadFigure(double a_head_figure);
  double JawFigure();
  void JawFigure(double a_jaw_figure);
  double EyeSize();
  void EyeSize(double an_eye_size);
  int EyeHeight();
  void EyeHeight(int an_eye_height);
  double EyeGap();
  void EyeGap(double an_eye_gap);
  double EyeAngle();
  void EyeAngle(double an_eye_angle);
  BOOL DoubleEyelid();
  void DoubleEyelid(BOOL a_double_eyelid);
  double EyeInR();
  void EyeInR(double an_eye_in_r);
  double EyeLightR();
  void EyeLightR(double an_eye_in_r);
  BOOL EyeMove();
  void EyeMove(BOOL an_eye_move);
  BOOL Wink();
  void Wink(BOOL a_wink);
  double HairVx();
  void HairVx(double a_vx);
  double HairVy();
  void HairVy(double a_vy);
  double HairGx();
  void HairGx(double a_gx);
  double HairGy();
  void HairGy(double a_gy);
  double HairAngle();
  void HairAngle(double an_angle);
  double HairVx2();
  void HairVx2(double a_vx);
  double HairVy2();
  void HairVy2(double a_vy);
  double HairGx2();
  void HairGx2(double a_gx);
  double HairGy2();
  void HairGy2(double a_gy);
  double HairAngle2();
  void HairAngle2(double an_angle);
  BOOL IsSkin();
  void SetSkin(BOOL a_mode);

  // ビューの方にポリゴン情報フラッシュ
  void Draw(CBodyView* pView);

  // 各制御点を表示
  void DrawPoint(CBodyView* pView);

  // 各制御点位置を更新
  void PosCalc();

  // 全制御点平行移動
  void Move(CVector3 dp);

  // 指定された制御点と特定のスクリーン座標との距離を求める
  double CalcScreenDis(CBodyView* pView, int p0, int sx, int sy);

  // 指定された制御点が特定のスクリーンエリアに含まれているか調べる
  BOOL InScreenArea(CBodyView* pView, int no, int sx, int sy, int ex, int ey);

  // 頭部形状を生成
  void CreateHead();

  // 髪形状を文字列で返す
  void HairMax(char* sBuf);
  void HairMax2(char* sBuf);

  // 文字列で髪形状をセット
  void SetHairMax(char* sBuf);
  void SetHairMax2(char* sBuf);

private:

  // 腕を除く上半身を表示
  void DrawTopBody(CBodyView* pView, int center, int top, int head, int p0, int p1, int p2, int p3);

  // 腕を除く下半身を表示
  void DrawBottomBody(CBodyView* pView, int center, int top, int p0, int p1, int p2, int p3);

  // 各腕（足も含む）を表示
  void DrawLeg(CBodyView* pView, int pos, BOOL legf);

  // 頭部を表示
  void DrawHead(CBodyView* pView);

  // 目を表示
  void DrawEye(CBodyView* pView, CVector3& pos, CVector3& vX, CVector3& vY, CVector3& vZ, double r);

  // 口を表示
  void DrawMouth(CBodyView* pView, CVector3& pos, CVector3& vX, CVector3& vY, CVector3& vZ, double r);

  // 髪の毛を表示
  void DrawHair(CBodyView* pView);

  // 髪の毛を移動
  void MoveHair();

  // スカートを表示
  void DrawSkirt(CBodyView* pView);

  // スカートを移動
  void MoveSkirt(int center, int top, int p0, int p1);

  // 制御点間の距離を求める
  double CalcDis(int p0, int p1);

  // 制御点間と初期位置間の制約処理
  void CalcSubConst(int p, double t);

  // 制御点間制約のサブステップ処理
  void CalcSub(int p0, int p1, double r, double t);

  // 各関節の制約チェック
  void ConditionCheck();
};

/////////////////////////////////////////////////////////////////////////////

#endif
