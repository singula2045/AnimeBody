// BodyView.h : CBodyView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(BODYVIEW_H)
#define BODYVIEW_H

//
// CBodyView
//
// アニメ調ポリゴン人体を表示するView
//

class CBodyView
{
protected:

  // X-Windowリソース
  Display *d;
  Window w;
  GC gc;
  Font font;
  unsigned long black, white;

  BOOL KeyState[0x100];  // キー状態

  CPolyBuffer polybuf[cMaxPolyBuffer];    // ポリゴンバッファ
  CPolyIndex polyindex[cMaxPolyBuffer];   // ポリゴンインデックス
  int iPolyCount;                         // ポリゴンバッファ内のポリゴン数
  BOOL bWireView;                         // trueでワイヤー表示
  BOOL bPointView;                        // trueで制御点表示
  BOOL bEdgeView;                         // trueで全てのエッジを表示
  BOOL bFlatShade;                        // trueでフラットシェーディング
  BOOL bUseEdge;                          // エッジ強調を用いる
  BOOL bUseBody[2];                       // 各人体を用いるならtrue
  BOOL bPointSelect[30];                  // 選択されている各制御点でtrue
  BOOL bPointSelectFlag;                  // 制御点が選択されていればtrue

  clock_t ctTime[20];                     // FPS測定用
  int iTime;                              // タイマカウンタ

  CVector3 vNorm[3];                      // World→Viewへの変換用単位ベクトル
  CVector3 angle;                         // Worldの回転角度（オイラー角）
  CVector3 camera_pos;                    // カメラ位置
  CVector3 camera_v;                      // カメラ移動速度
  int iBodyNo;                            // 選択されている人体No.

  Pixmap mbm;                             // バックバッファ用のピックスマップ
  CVector3 vp;                            // 制御点移動時の速度
  int sx, sy;                             // マウス位置（スクリーン座標）
  int dsx, dsy;                           // ドラッグ開始位置（スクリーン座標）
  BOOL bLeftButton;                       // 左ボタンダウン中ならtrue

  GC brush[5][4];                         // ポリゴン塗りつぶし用GC
  GC spen;                                // 細線用GC
  GC bpen;                                // 太線用GC
  GC rpen;                                // 制御点表示用GC

  CBodyModel body[2];                     // 人体インスタンス

  // オペレーション
public:
  CBodyView();
  ~CBodyView();

  // 制御点選択クリア
  void ClearPointSelect();

  // 変換行列を計算
  void mcalc();

  // ワールド座標→ビュー座標変換
  void ChangeWtoV(CVector3& vWorldPos, CVector3& vViewPos);

  // ビュー座標→ワールド座標変換
  void ChangeVtoW(CVector3& vViewPos, CVector3& vWorldPos);

  // ビュー座標→スクリーン座標変換
  void ChangeVtoS(CVector3& vViewPos, POINT& sScreenPos);

  // 画面表示
  void Draw();

  // ビュー初期化
  void Init(int argc, char* argv[]);

  // 各オブジェクト移動処理
  void Idle();

  // ポリゴンバッファの表示
  void FlushPolyBuffer();

  // ポリゴンのエッジを表示
  void DrawPolygonEdge(POINT* p, int iEdgeFlag);

  // アニメ調ポリゴン塗りつぶし
  void DrawAnimePolygon(POINT* p, int kind, double* c);

  // アニメ調でポリゴン表示
  void DrawPolygon(POINT* pos, int kind, double* color, double zval, int iEdgeFlag);

  // 矩形塗りつぶし
  void FillRectangle(int iXpos, int iYpos, int iWidth, int iHeight);

protected:

  // マウス左ボタンダウン処理
  void OnLButtonDown(int x, int y);

  // マウス左ボタンアップ処理
  void OnLButtonUp(int x, int y);

  // マウス左ボタンドラッグ処理
  void OnMouseMove(int x, int y);

  // X-Windowイベント処理
  void CheckEvent();
};

#endif

