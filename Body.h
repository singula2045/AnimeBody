// Body.h : BODY アプリケーションのメイン ヘッダー ファイル
//

#if !defined(BODY_H)
#define BODY_H

#define sgn(x) ((x) > 0 ? 1 : -1)       // x<0なら-1,x=0なら0,x>0なら1
#define min(x,y) ((x) < (y) ? x : y)    // 小さいほうを返す
#define max(x,y) ((x) < (y) ? y : x)    // 大きいほうを返す

const double T1 = 0.0;                  // 塗りつぶし時の明るさ境界値
const double T2 = 0.3;
const double T3 = 0.6;

const int cPosMax = 30;                 // 制御点最大数
const int cLinkMax = 35;                // 制御点間リンク最大数
const int cMaxPolyBuffer = 4000;        // Ｚソート用ポリゴンバッファの大きさ

// 型定義

typedef bool BOOL;
typedef struct XPoint POINT;

// Ｚソート用ポリゴンバッファ
struct CPolyBuffer {
  POINT pos[4];           // ポリゴン頂点
  double color[4];        // 各頂点の明るさ
  int edge;               // 各エッジを強調するかどうか（下４ビットそれぞれ）
  int kind;               // ポリゴンの種類
};

// Ｚソート時に用いるポリゴンインデックス（qsort用）
// CPolyBufferと分離しているのは、構造体サイズを減らしてqsortを高速にするため
struct CPolyIndex {
  int no;                         // CPolyBuffer配列のインデックス値
  double z;                       // ポリゴンのＺ値
};

#endif
