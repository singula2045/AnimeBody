// BodyModel.cpp : インプリメンテーション ファイル
//

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include "Vector3.h"
#include "Body.h"
#include "BodyModel.h"
#include "BodyView.h"

/////////////////////////////////////////////////////////////////////////////
// CBodyModel

CBodyModel::CBodyModel()
{
}

// sBuf内で、cで区切ったi番目の部分文字列を返す
void GetStr(char* sBuf, int pos, char c, char* sAns)
{
  int i;
  char buf[256];
  strcpy(buf, sBuf);
  char *p, *p2;

  p = buf;
  while (*p && *p == c)
    p++;

  if (!*p) {
    sAns[0] = 0;
    return;
  }

  for (i = 0; i <= pos; i++) {
    while (*p && *p == c)
      p++;
    p2 = p;
    while (*p && *p != c)
      p++;
    if (!*p) {
      if (i == pos) {
        strcpy(sAns, p2);
        return;
      }
      else {
        sAns[0] = 0;
        return;
      }
    }
  }
  if (*p) {
    *p = 0;
    strcpy(sAns, p2);
    return;
  }
  else {
    sAns[0] = 0;
    return;
  }
}

// sBuf内で、cで区切ったi番目の部分文字列を0から20に丸めて返す
// sBuf内で、スペースで区切った数
int GetPosNo(char* sBuf, int i)
{
  char buf[256];
  GetStr(sBuf, i, ' ', buf);
  int ans = atoi(buf);
  if (ans > 20)
    ans = 20;
  if (ans < 0)
    ans = 0;

  return ans;
}

void CBodyModel::Init()
{
  int i;

  // 各パラメータ初期化
  
  skin = false;

  head_age = 8.0;
  head_h = 1.2;
  head_figure = 1.5;
  jaw_figure = -0.2;
  eye_size = 0.3;
  eye_height = 6;
  eye_gap = 0.7;
  eye_angle = 0.0;
  eye_in_r = 3.0;
  eye_light_r = 1.0;
  double_eyelid = true;
  eye_move = true;
  wink = true;

  // 髪初期設定
  hair_init = false;
  skirt_init = false;

  hair_vx = 0.17;
  hair_vy = -0.03;
  hair_gx = -0.013;
  hair_gy = 0.028;
  hair_angle = 0.0;
  SetHairMax("20 20 20 20 20 15 8 8 8 8 15 20 20 20 20 20");

  // 前髪初期設定
  hair_vx2 = 0.12;
  hair_vy2 = 0.14;
  hair_gx2 = -0.005;
  hair_gy2 = 0.0;
  hair_angle2 = 0.0;
  SetHairMax2("20 20 15 20 12 20 15 12 11 15 20 10 15 20 15 20");

  vLight.set(-0.25, -0.5, -0.5);  // 光源方向

  eye_timer = 0;

  for (i = 0; i < cPosMax; i++)
    mPos[i].vPos.x = -1000;
  /*
    mPos[0].vPos.set(0, -0.5, 3);               // 胴
    mPos[1].vPos.set(0, 0, -1);                 // 首
    mPos[2].vPos.set(0, 0, -4);                 // 頭
    mPos[3].vPos.set(0, -1.5, -2.5);            // 顔

    mPos[10].vPos.set(-1.5, 0.5, -0.3);         // 右肩
    mPos[11].vPos.set(-5, 0, 0);                // 右ひじ
    mPos[12].vPos.set(-6, 0, 5);                // 右手首

    mPos[13].vPos.set(1.5, 0.5, -0.3);          // 左肩
    mPos[14].vPos.set(5, 0, 0);                 // 左ひじ
    mPos[15].vPos.set(6, 0, 5);                 // 左手首

    mPos[16].vPos.set(-1.2, -0.5, 6);           // 右足もと
    mPos[17].vPos.set(-4, -5, 5);               // 右ひざ
    mPos[18].vPos.set(-8, 0, 8);                // 右足首

    mPos[19].vPos.set(1.2, -0.5, 6);            // 左足もと
    mPos[20].vPos.set(4, -5, 5);                // 左ひざ
    mPos[21].vPos.set(8, 0, 8);                 // 左足首
  */

  mPos[0].vPos.set(-0.0988659, 0.320302, 0.0189341);  // 胴
  mPos[1].vPos.set(-0.728868, 1.20829, -3.86953);     // 首
  mPos[2].vPos.set(-0.743852, 1.46375, -6.86662);     // 頭
  mPos[3].vPos.set(0.0463491, 0.0658803, -5.47986);   // 顔

  mPos[10].vPos.set(-2.20316, 1.21004, -2.96266);     // 右肩
  mPos[11].vPos.set(-2.52937, -0.404083, 0.180357);   // 右ひじ
  mPos[12].vPos.set(-3.26741, -4.04536, 3.67272);     // 右手首

  mPos[13].vPos.set(0.672052, 2.01282, -3.25055);     // 左肩
  mPos[14].vPos.set(3.28218, 1.24891, -0.972119);     // 左ひじ
  mPos[15].vPos.set(4.41426, -1.82571, 2.93477);      // 左手首

  mPos[16].vPos.set(-0.859788, -0.284093, 3.10048);   // 右足もと
  mPos[17].vPos.set(0.181103, -3.38983, 7.38545);     // 右ひざ
  mPos[18].vPos.set(-2.13814, -1.53053, 13.8014);     // 右足首

  mPos[19].vPos.set(1.46213, 0.268095, 2.84257);      // 左足もと
  mPos[20].vPos.set(3.0335, -2.09734, 7.42801);       // 左ひざ
  mPos[21].vPos.set(2.70674, 0.139826, 14.1279);      // 左足首

  // 各制御点を使うかどうかチェック＆初期位置設定

  for (i = 0; i < cPosMax; i++) {
    if (mPos[i].vPos.x > -1000)
      mPos[i].bUse = true;
    else
      mPos[i].bUse = false;
    mPos[i].vInitPos = mPos[i].vPos;
  }

  // 各関連間の距離を求めておく

  distance[0] = CalcDis(0, 1);     // 胴−首
  distance[1] = CalcDis(10, 13);   // 肩間
  distance[2] = CalcDis(0, 10);    // 胴−右肩
  distance[3] = CalcDis(10, 11);   // 右肩−右ひじ
  distance[4] = CalcDis(11, 12);   // 右ひじ−右手首
  distance[5] = CalcDis(0, 13);    // 胴−左肩
  distance[6] = CalcDis(13, 14);   // 左肩−左ひじ
  distance[7] = CalcDis(14, 15);   // 左ひじ−左手首
  distance[8] = CalcDis(0, 16);    // 胴−右足もと
  distance[9] = CalcDis(16, 17);   // 右足もと−右ひざ
  distance[10] = CalcDis(17, 18);  // 右ひざ−右足首
  distance[11] = CalcDis(0, 19);   // 胴−左足もと
  distance[12] = CalcDis(19, 20);  // 左足もと−左ひざ
  distance[13] = CalcDis(20, 21);  // 左ひざ−左足首
  distance[14] = CalcDis(16, 19);  // 右足もと−左足もと
  distance[15] = CalcDis(10, 16);  // 右肩−右足もと
  distance[16] = CalcDis(13, 19);  // 左肩−左足もと
  distance[17] = CalcDis(1, 10);   // 首−右肩
  distance[18] = CalcDis(1, 13);   // 首−左肩
  distance[19] = CalcDis(1, 2);    // 首−頭
  distance[20] = CalcDis(0, 2);    // 胴−頭
  distance[21] = CalcDis(2, 3);    // 頭−顔
  distance[22] = CalcDis(1, 3);    // 首−顔
  distance[23] = CalcDis(13, 16);  // 左肩−右足もと
  distance[24] = CalcDis(10, 19);  // 右肩−左足もと

  // 頭部形状は動的に生成しないので、先に生成しておく

  CreateHead();

  for (i = 0; i < cPosMax; i++) {
    mPos[i].vInitPos = mPos[i].vPos;
  }
}

CBodyModel::~CBodyModel()
{
}


// 制御点位置を返す

CVector3 CBodyModel::Pos(int no)
{
  return mPos[no].vPos;
}


// 制御点位置を変更

void CBodyModel::SetPos(int no, CVector3& pos)
{
  mPos[no].vPos = pos;
}

// 制御点位置を移動
void CBodyModel::MovePos(int no, CVector3& pos)
{
  mPos[no].vPos += pos;
}

// 各パラメータへのアクセサ

void CBodyModel::Name(char* buf) { strcpy(buf, name); }
void CBodyModel::SetName(char* a_name) { strcpy(name, a_name); } 
double CBodyModel::HeadAge() { return head_age; }
void CBodyModel::HeadAge(double a_head_age) { head_age = a_head_age; }
double CBodyModel::HeadH() { return head_h; }
void CBodyModel::HeadH(double a_head_h) { head_h = a_head_h; }
double CBodyModel::HeadFigure() { return head_figure; }
void CBodyModel::HeadFigure(double a_head_figure) { head_figure = a_head_figure; }
double CBodyModel::JawFigure() { return jaw_figure; }
void CBodyModel::JawFigure(double a_jaw_figure) { jaw_figure = a_jaw_figure; }
double CBodyModel::EyeSize() { return eye_size; }
void CBodyModel::EyeSize(double an_eye_size) { eye_size = an_eye_size; }
int CBodyModel::EyeHeight() { return eye_height; }
void CBodyModel::EyeHeight(int an_eye_height) { eye_height = an_eye_height; }
double CBodyModel::EyeGap() { return eye_gap; }
void CBodyModel::EyeGap(double an_eye_gap) { eye_gap = an_eye_gap; }
double CBodyModel::EyeAngle() { return eye_angle; }
void CBodyModel::EyeAngle(double an_eye_angle) { eye_angle = an_eye_angle; }
BOOL CBodyModel::DoubleEyelid() { return double_eyelid; }
void CBodyModel::DoubleEyelid(BOOL a_double_eyelid) { double_eyelid = a_double_eyelid; }
double CBodyModel::EyeInR() { return eye_in_r; }
void CBodyModel::EyeInR(double an_eye_in_r) { eye_in_r = an_eye_in_r; }
double CBodyModel::EyeLightR() { return eye_light_r; }
void CBodyModel::EyeLightR(double an_eye_light_r) { eye_light_r = an_eye_light_r; }
BOOL CBodyModel::EyeMove() { return eye_move; }
void CBodyModel::EyeMove(BOOL an_eye_move) { eye_move = an_eye_move; }
BOOL CBodyModel::Wink() { return wink; }
void CBodyModel::Wink(BOOL a_wink) { wink = a_wink; }
double CBodyModel::HairVx() { return hair_vx; }
void CBodyModel::HairVx(double a_vx) { hair_vx = a_vx; hair_init = false; }
double CBodyModel::HairVy() { return hair_vy; }
void CBodyModel::HairVy(double a_vy) { hair_vy = a_vy; hair_init = false; }
double CBodyModel::HairGx() { return hair_gx; }
void CBodyModel::HairGx(double a_gx) { hair_gx = a_gx; hair_init = false; }
double CBodyModel::HairGy() { return hair_gy; }
void CBodyModel::HairGy(double a_gy) { hair_gy = a_gy; hair_init = false; }
double CBodyModel::HairAngle() { return hair_angle; }
void CBodyModel::HairAngle(double an_angle) { hair_angle = an_angle; hair_init = false; }
double CBodyModel::HairVx2() { return hair_vx2; }
void CBodyModel::HairVx2(double a_vx) { hair_vx2 = a_vx; hair_init = false; }
double CBodyModel::HairVy2() { return hair_vy2; }
void CBodyModel::HairVy2(double a_vy) { hair_vy2 = a_vy; hair_init = false; }
double CBodyModel::HairGx2() { return hair_gx2; }
void CBodyModel::HairGx2(double a_gx) { hair_gx2 = a_gx; hair_init = false; }
double CBodyModel::HairGy2() { return hair_gy2; }
void CBodyModel::HairGy2(double a_gy) { hair_gy2 = a_gy; hair_init = false; }
double CBodyModel::HairAngle2() { return hair_angle2; }
void CBodyModel::HairAngle2(double an_angle) { hair_angle2 = an_angle; hair_init = false; }
BOOL CBodyModel::IsSkin() { return skin; }
void CBodyModel::SetSkin(BOOL a_mode) { skin = a_mode; }


// 全制御点平行移動

void CBodyModel::Move(CVector3 dp)
{
  for (int i = 0; i < cPosMax; i++) {
    if (mPos[i].bUse) {
      mPos[i].vPos += dp;
      mPos[i].vOldPos += dp;
    }
  }
}

// ビューの方にポリゴン情報フラッシュ

void CBodyModel::Draw(CBodyView* pView)
{
  DrawHead(pView);                // 頭
  DrawHair(pView);                // 髪
  DrawLeg(pView, 10, true);       // 右手
  DrawLeg(pView, 13, true);       // 左手
  DrawLeg(pView, 16, false);      // 右足
  DrawLeg(pView, 19, false);      // 左足
  DrawTopBody(pView, 0, 1, 2, 10, 13, 16, 19);    //　上ボディ
  DrawBottomBody(pView, 0, 1, 10, 13, 16, 19);    //　下ボディ
  if (!skin)
    DrawSkirt(pView);             // 服
}

// 各制御点を表示

void CBodyModel::DrawPoint(CBodyView* pView) 
{
  CVector3 l;
  POINT v;
  for (int i = 0; i < cPosMax; i++) {
    if (mPos[i].bUse) {
      // 各制御点をスクリーン座標に変換して表示
      pView->ChangeWtoV(mPos[i].vPos, l);
      pView->ChangeVtoS(l, v);
      pView->FillRectangle(v.x - 1, v.y - 1, 3, 3);
    }
  }
}


// 各腕（足も含む）を表示
// 制御点は、posが椀の元、pos+1がひじ（ひざ）、pos+2が先端、
// legf=trueなら腕、falseなら足

void CBodyModel::DrawLeg(CBodyView* pView, int pos, BOOL legf) 
{
  int i, j;

  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 p[17][8];   // 各頂点位置（ワールド座標系）
  static CVector3 n[17][8];   // 各頂点での法線ベクトル（ワールド座標系）
  static CVector3 n0[17][8];  // 各頂点での法線ベクトル（ビュー座標系）
  static CVector3 l[17][8];   // 各頂点位置（ビュー座標系）
  static POINT s[17][8];      // 各頂点位置（スクリーン座標系）

  CVector3 dx, dy;

  // 高速化のため、Sin, Cosテーブルを作っておく
  double sint[8], cost[8];
  for (i = 0; i < 8; i++) {
    double a = i / 8.0 * M_PI * 2;
    cost[i] = cos(a);
    sint[i] = sin(a);
  }

  CVector3 sp = mPos[pos].vPos;
  CVector3 mp = mPos[pos + 1].vPos;
  CVector3 ep = mPos[pos + 2].vPos;

  // 上腕の基準系（伸びる方向a0、Ｘ、Ｙ方向(a1,a2)を求める）

  CVector3 dp;
  dp = mp - sp;
  CVector3 dp2;
  dp2 = ep - mp;

  double r0 = dp.abs();

  CVector3 a0, a1, a2, a;
  a0 = dp / r0;
  a1 = dp.outprod(dp2);
  a1.normalize();

  a2 = a0.outprod(a1);
  a2.normalize();

  // 下腕の基準系（伸びる方向b0、Ｘ、Ｙ方向(b1,b2)を求める）

  CVector3 b0, b1, b2, b;

  b0 = dp2.norm();
  b1 = a1;

  b2 = b0.outprod(b1);
  b2.normalize();

  // 接続部の基準系（伸びる方向b0、Ｘ、Ｙ方向(b1,b2)を求める）
  // 上腕と下腕の平均

  CVector3 c0, c1, c2;
  c0 = (a0 + b0) / 2;
  c0.normalize();

  c1 = (a1 + b1) / 2;
  c1.normalize();

  c2 = c0.outprod(c1);
  c2.normalize();

  double rr = a0.inprod(b0);

  // 上腕形状
  double rb0[8] = {0.0, 0.08, 0.15, 0.3, 0.5, 0.7, 0.9, 0.95};  // 長さ方向位置
  double rb1[8] = {0.4, 0.9, 0.95, 1.0, 0.97, 0.9, 0.8, 0.8};   // X方向半径
  double rb2[8] = {0.4, 0.8, 0.95, 1.0, 0.97, 0.9, 0.8, 0.8};   // Y方向半径

  // 腕か足かで半径調節
  if (!legf) {
    for (i = 0; i < 8; i++) {
      rb1[i] *= 1.1;
      rb2[i] *= 1.1;
    }
  }
  else {
    for (i = 0; i < 8; i++) {
      rb1[i] *= 0.7;
      rb2[i] *= 0.7;
    }
  }


  // 上腕生成
  for (j = 0; j < 8; j++) {
    if (j >= 7) {
      // 接続部処理
      a1 = (a1 * 2 + c1) / 3;
      a2 = (a2 * 2 + c2) / 3;
    }
    for (i = 0; i < 8; i++) {
      p[j][i] = mPos[pos].vPos + dp * rb0[j] + a1 * (cost[i] * rb1[j]) + a2 * (sint[i] * rb2[j]);
      pView->ChangeWtoV(p[j][i], l[j][i]);
      pView->ChangeVtoS(l[j][i], s[j][i]);
    }
  }

  // 下腕生成

  double rt0[9] = {0.0, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9, 1.0, 1.0};
  double rt1[9] = {0.8, 0.9, 0.9, 0.9, 0.8, 0.7, 0.6, 0.5, 0};
  double rt2[9] = {0.8, 0.9, 0.9, 0.9, 0.8, 0.7, 0.6, 0.5, 0};
  if (!legf) {
    for (i = 0; i < 9; i++) {
      rt1[i] *= 1.0;
      rt2[i] *= 1.0;
    }
  }
  else {
    for (i = 0; i < 9; i++) {
      rt1[i] *= 0.7;
      rt2[i] *= 0.7;
    }
  }
  for (j = 8; j >= 0; j--) {
    if (j < 2) {
      b1 = (b1 * 2 + c1) / 3;
      b2 = (b2 * 2 + c2) / 3;
      if (rr < 0) {
        rt1[j] *= 1.1;
        rt2[j] *= 1.1;
      }
    }
    for (i = 0; i < 8; i++) {
      p[j + 8][i] = mPos[pos + 1].vPos + dp2 * rt0[j] + b1 * (cost[i] * rt1[j]) + b2 * (sint[i] * rt2[j]);
      pView->ChangeWtoV(p[j + 8][i], l[j + 8][i]);
      pView->ChangeVtoS(l[j + 8][i], s[j + 8][i]);
    }
  }

  // 各頂点の法線ベクトルを求める

  for (j = 0; j < 16; j++) {
    for (i = 0; i < 8; i++) {
      dx  = p[j][i] - p[j][(i + 1) % 8];
      if (j < 15) {
        dy  = p[j][i] - p[j + 1][i];
        n0[j][i] = dx.outprod(dy);
      }
      else {
        n0[j][i] = p[j][i] - p[j + 1][i];
      }
      n0[j][i].normalize();
      pView->ChangeWtoV(n0[j][i], n[j][i]);
    }
  }


  // エッジ判定してポリゴン表示

  POINT buf[4];
  double c[4];
  for (j = 0; j < 16; j++) {
    for (i = 0; i < 8; i++) {
      int ni = (i + 1) % 8;
      if (n[j][i].y < 0 || n[j][ni].y < 0 || n[j + 1][i].y < 0 || n[j + 1][ni].y < 0) { // 裏面チェック
        // 頂点位置セット
        buf[0] = s[j][i];
        buf[1] = s[j][ni];
        buf[2] = s[j + 1][ni];
        buf[3] = s[j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[j][i].y < 0 && n[j][ni].y > 0 || n[j][i].y > 0 && n[j][ni].y < 0)
          f |= 1;
        if (n[j][i].y < 0 && n[j + 1][i].y > 0 || n[j][i].y > 0 && n[j + 1][i].y < 0)
          f |= 2;
        if (j == 15)
          f = 0;

        // Ｚ値計算（ポリゴン中心を採用）
        double z = (l[j][i].y + l[j][ni].y + l[j + 1][ni].y + l[j + 1][i].y) / 4.0;

        // 各頂点光度を計算
        c[0] = n[j][i].inprod(vLight);
        c[1] = n[j][ni].inprod(vLight);
        c[2] = n[j + 1][ni].inprod(vLight);
        c[3] = n[j + 1][i].inprod(vLight);

        if (legf && !skin)
          pView->DrawPolygon(buf, 3, c, z, f);
        else
          pView->DrawPolygon(buf, 0, c, z, f);
      }
    }
  }
}


// 腕を除く上半身を表示

void CBodyModel::DrawTopBody(CBodyView* pView, int center, int top, int head, int p0, int p1, int p2, int p3)
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 p[12][16];    // 各頂点位置（ワールド座標系）
  static CVector3 n[12][16];    // 各頂点での法線ベクトル（ワールド座標系）
  static CVector3 n0[12][16];   // 各頂点での法線ベクトル（ビュー座標系）
  static CVector3 l[12][16];    // 各頂点位置（ビュー座標系）
  static POINT s[12][16];       // 各頂点位置（スクリーン座標系）

  CVector3 bcp, cp, d, d2, lb, rb, dl, dr, dx, dy, dcp;
  CVector3 vXc, vYc, vZc;
  CVector3 vXt, vYt, vZt;
  CVector3 vXn, vYn, vZn;
  CVector3 vX, vY, vZ;
  int i, j;
  double rr, as;

  // 高速化のため、Sin, Cosテーブルを作っておく
  double sint[16], cost[16];
  for (i = 0; i < 16; i++) {
    double a = i / 16.0 * M_PI * 2;
    cost[i] = cos(a);
    sint[i] = sin(a);
  }

  // 基準位置決定
  // topが首,p0からp3が各肩と足元
  // dcpは、肩を変形させる際に使用

  cp = mPos[center].vPos;
  lb = mPos[p0].vPos;
  rb = mPos[p1].vPos;
  bcp = mPos[top].vPos;
  dcp = (cp + bcp * 3) / 4.0;

  // 基準系決定
  // v?tが肩位置での基準系
  // v?cが胴位置での基準系
  // v?nが首位置での基準系

  d = bcp - cp;
  d2 = mPos[head].vPos - mPos[top].vPos;

  vXt = (lb - rb).norm();
  vZt = d.norm();
  vYt = vXt.outprod(vZt);

  vXc = (((lb - rb) + (mPos[p2].vPos - mPos[p3].vPos)) / 2).norm();
  vZc = (bcp - (mPos[p2].vPos + mPos[p3].vPos) / 2).norm();
  vYc = vXc.outprod(vZc);

  vXn = vXc;
  vZn = d2.norm();
  vYn = vXn.outprod(vZn);

  // 胴から見た肩の位置（肩を変形させる際に使用する）

  dl = dcp - lb;
  dl.normalize();

  dr = dcp - rb;
  dr.normalize();

  // 上半身の形状

  double rt[12] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.85, 0.9, 0.1, 0.3}; // 高さ位置
  double rx[12] = {1.4, 1.4, 1.5, 1.6, 1.7, 1.8, 1.7, 1.5, 1.0, 0.4, 0.4, 0.4};  // Ｘ半径
  double ry[12] = {1.0, 1.0, 1.1, 1.2, 1.2, 1.2, 1.1, 0.9, 0.7, 0.4, 0.4, 0.4};  // Ｙ半径

  for (j = 0; j < 12; j++) { // 胴→肩方向へのループ

    if (j < 10) {
      // 基準系を肩と胴の基準系間の補間から求める
      as = j / 9.0;
      as = pow(as, 20);   // 胴側に曲がるように調整
      vX = vXc * (1 - as) + vXt * as;
      vY = vYc * (1 - as) + vYt * as;
      vZ = vZc * (1 - as) + vZt * as;
    }
    else {
      // 基準系を肩と首の基準系間の補間から求める
      as = (j - 9) / 2.0;
      vX = vXt * (1 - as) + vXn * as;
      vY = vYt * (1 - as) + vYn * as;
      vZ = vZt * (1 - as) + vZn * as;
    }

    for (i = 0; i < 16; i++) { // 円周方向へのループ

      if (j < 10) {
        p[j][i] = cp + d * rt[j];
      }
      else {
        p[j][i] = mPos[top].vPos + d2 * rt[j];
      }
      p[j][i] += vX * (sint[i] * rx[j]) + vY * (cost[i] * ry[j]);

      if (j) {
        // 肩制御点の距離が近い場合、そちらに変形（肩を綺麗に接続させるため）
        CVector3 d2;
        d2 = p[j][i] - lb;
        rr = d2.abs();
        if (rr < 0.7)
          rr = 0.7;
        p[j][i] -= dl / (rr * 1.4);

        d2 = p[j][i] - rb;
        rr = d2.abs();
        if (rr < 0.7)
          rr = 0.7;
        p[j][i] -= dr / (rr * 1.4);
      }

      // 各頂点をスクリーン座標に変換
      pView->ChangeWtoV(p[j][i], l[j][i]);
      pView->ChangeVtoS(l[j][i], s[j][i]);
    }
  }

  // 各頂点の法線ベクトルを求める
  for (j = 0; j < 11; j++) {
    for (i = 0; i < 16; i++) {
      dx = p[j][i] - p[j][(i + 1) % 16];
      dy = p[j][i] - p[j + 1][i];
      n0[j][i] = dx.outprod(dy);
      n0[j][i].normalize();
      pView->ChangeWtoV(n0[j][i], n[j][i]);
    }
  }
  for (i = 0; i < 16; i++) {
    n0[11][i] = n0[10][i];
    n[11][i] = n[10][i];
  }


  // エッジ判定してポリゴン表示

  POINT buf[4];
  double c[4];
  for (j = 0; j < 11; j++) {
    for (i = 0; i < 16; i++) {
      int ni = (i + 1) % 16;
      if (n[j][i].y < 0 || n[j][ni].y < 0 || n[j + 1][i].y < 0 || n[j + 1][ni].y < 0) { // 裏面チェック
        // 頂点位置セット
        buf[0] = s[j][i];
        buf[1] = s[j][ni];
        buf[2] = s[j + 1][ni];
        buf[3] = s[j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[j][i].y < 0 && n[j][ni].y > 0 || n[j][i].y > 0 && n[j][ni].y < 0)
          f |= 1;
        if (n[j][i].y < 0 && n[j + 1][i].y > 0 || n[j][i].y > 0 && n[j + 1][i].y < 0)
          f |= 2;

        // Ｚ値計算（ポリゴン中心を採用）
        double z = (l[j][i].y + l[j][ni].y + l[j + 1][ni].y + l[j + 1][i].y) / 4.0;

        // 各頂点光度を計算
        c[0] = n[j][i].inprod(vLight);
        c[1] = n[j][ni].inprod(vLight);
        c[2] = n[j + 1][ni].inprod(vLight);
        c[3] = n[j + 1][i].inprod(vLight);

        if (j < 8 && !skin)
          pView->DrawPolygon(buf, 3, c, z, f);
        else
          pView->DrawPolygon(buf, 0, c, z, f);
      }
    }
  }
}


// 腕を除く下半身を表示

void CBodyModel::DrawBottomBody(CBodyView* pView, int center, int top, int p0, int p1, int p2, int p3)
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 p[10][16];    // 各頂点位置（ワールド座標系）
  static CVector3 n[10][16];    // 各頂点での法線ベクトル（ワールド座標系）
  static CVector3 n0[10][16];   // 各頂点での法線ベクトル（ビュー座標系）
  static CVector3 l[10][16];    // 各頂点位置（ビュー座標系）
  static POINT s[10][16];       // 各頂点位置（スクリーン座標系）

  CVector3 bcp, cp, d, lb, rb, dl, dr, dx, dy, dcp;
  CVector3 vXc, vYc, vZc;
  CVector3 vXb, vYb, vZb;
  CVector3 vX, vY, vZ;
  int i, j;

  // 高速化のため、Sin, Cosテーブルを作っておく
  double sint[16], cost[16];
  for (i = 0; i < 16; i++) {
    double a = i / 16.0 * M_PI * 2;
    cost[i] = cos(a);
    sint[i] = sin(a);
  }

  // 基準位置決定
  // bcpが腰,p0からp3が各肩と足元
  // dcpは、足元を変形させる際に使用

  lb = mPos[p2].vPos;
  rb = mPos[p3].vPos;

  cp = mPos[center].vPos;

  bcp = (mPos[p2].vPos + mPos[p3].vPos) / 2;
  dcp = (cp + bcp) / 2.0;

  d = bcp - cp;

  // 胴から見た足元の位置（足元を変形させる際に使用する）

  dl = dcp - lb;
  dl.normalize();

  dr = dcp - rb;
  dr.normalize();

  // 基準系決定
  // v?bが足元位置での基準系
  // v?cが胴位置での基準系

  vXb = (lb - rb).norm();
  vZb = d.norm() * -1;
  vYb = vXb.outprod(vZb);

  vXc = (((lb - rb) + (mPos[p0].vPos - mPos[p1].vPos)) / 2).norm();
  vZc = (mPos[top].vPos - bcp).norm();
  vYc = vXc.outprod(vZc);

  // 下半身の形状

  double rt[10] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.65, 0.75, 0.80, 0.75}; // 高さ位置
  double rx[10] = {1.4, 1.5, 1.7, 1.8, 1.9, 2.0, 1.8, 1.5, 0.9, 0.0};     // Ｘ半径
  double ry[10] = {1.0, 1.1, 1.2, 1.2, 1.2, 1.2, 1.2, 1.0, 0.5, 0.0};     // Ｙ半径
  for (j = 0; j < 10; j++) { // 胴→腰方向へのループ
    double as = j / 9.0;
    as = as * as * as;
    as = pow(as, 20);     // 胴側にしなるように調整

    // 基準系を腰と胴の基準系間の補間から求める
    vX = vXc * (1 - as) + vXb * as;
    vY = vYc * (1 - as) + vYb * as;
    vZ = vZc * (1 - as) + vZb * as;

    CVector3 vTmp;
    vTmp.set(rx[j] * 1.2, ry[j] * 1.2, rt[j] * 5);

    for (i = 0; i < 16; i++) { // 円周方向へのループ

      // お尻をへこませる
      double rr = 1.2;
      if (i == 8 || i == 0)
        rr = 1.0;

      p[j][i] = cp + d * (rt[j] * rr) + vX * (sint[i] * rx[j]) + vY * (cost[i] * ry[j]);

      if (j) {
        // 足元制御点の距離が近い場合、そちらに変形（足を綺麗に接続させるため）
        CVector3 d2;
        d2 = p[j][i] - lb;
        rr = d2.abs();
        if (rr < 0.7)
          rr = 0.7;
        p[j][i] -= dl / (rr * 1.5);

        d2 = p[j][i] - rb;
        rr = d2.abs();
        if (rr < 0.7)
          rr = 0.7;
        p[j][i] -= dr / (rr * 1.5);
      }

      // 各頂点をスクリーン座標に変換
      pView->ChangeWtoV(p[j][i], l[j][i]);
      pView->ChangeVtoS(l[j][i], s[j][i]);
    }
  }

  // 各頂点の法線ベクトルを求める
  for (j = 0; j < 10; j++) {
    for (i = 0; i < 16; i++) {
      dx  = p[j][i] - p[j][(i + 15) % 16];
      if (j < 9) {
        dy  = p[j][i] - p[j + 1][i];
        n0[j][i] = dx.outprod(dy);
      }
      else {
        n0[j][i].set(0, 0, 1); 
      }
      n0[j][i].normalize();
      pView->ChangeWtoV(n0[j][i], n[j][i]);
    }
  }

  // エッジ判定してポリゴン表示

  POINT buf[4];
  double c[4];
  for (j = 0; j < 9; j++) {
    for (i = 0; i < 16; i++) {
      int ni = (i + 1) % 16;
      if (n[j][i].y < 0 || n[j][ni].y < 0 || n[j + 1][i].y < 0 || n[j + 1][ni].y < 0) { // 裏面チェック
        // 頂点位置セット
        buf[0] = s[j][i];
        buf[1] = s[j][ni];
        buf[2] = s[j + 1][ni];
        buf[3] = s[j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[j][i].y < 0 && n[j][ni].y > 0 || n[j][i].y > 0 && n[j][ni].y < 0)
          f |= 4;
        if (j != 9 && (n[j][i].y < 0 && n[j + 1][i].y > 0 || n[j][i].y > 0 && n[j + 1][i].y < 0))
          f |= 8;

        // Ｚ値計算（ポリゴン中心を採用）
        double z = (l[j][i].y + l[j][ni].y + l[j + 1][ni].y + l[j + 1][i].y) / 4.0;

        // 各頂点光度を計算
        c[0] = n[j][i].inprod(vLight);
        c[1] = n[j][ni].inprod(vLight);
        c[2] = n[j + 1][ni].inprod(vLight);
        c[3] = n[j + 1][i].inprod(vLight);

        if (j > 0 && j < 3 || skin)
          pView->DrawPolygon(buf, 0, c, z, f);
        else 
          pView->DrawPolygon(buf, 3, c, z, f);
      }
    }
  }
}


//
// 頭を作成
//
// 各パラメータは基本的にFaceCheckerと同じなのでそちらを参照
// http://village.infoweb.ne.jp/~fwhz9346/fccheck.htm
//

void CBodyModel::CreateHead()
{
  int nBands = 16;       // 角度分割数
  int i;

  CVector3 bp;

  double r = 1.2 + (20 / head_age) * 0.15;      // 基準半径
  head_r = r;
  double h = head_h;                            // 縦横比率
  double mo = head_figure;                      // 卵型比率
  double a = 0.2 - (20 / head_age) * 0.15;      // 目の位置
  double e = jaw_figure;                        // えらの張り方
  double nh = 0.5 - (20 / head_age) * 0.15;     // 鼻の位置

  double h2 = h * 2 / (1 + mo);                 // moで補正後のh
  double ha = r * (1 - a) * h;                  // 頭頂の位置
  double hb = r * (1 + a) * h;                  // 顔下の位置
  double dy = -r * h2 * (mo - 1) / 2;           // 上下方向の座標移動量（首位置が原点）
  double h0 = r * h2 - dy - ha;                 // 上下方向の目の位置

  double rr;    // あご表示時の補正半径

  // 顔下位置

  bp.x = 0;
  bp.y = 0;
  bp.z = -r * h2 * mo - dy;

  // 首相対系での目の高さ
  eye_line = h0 - bp.z;

  // 頭頂位置

  head_tp.x = 0;
  head_tp.y = 0;
  head_tp.z = -(r * h2 - dy);
  head_tp += bp;

  head_cp = bp;

  double da = M_PI / nBands;    // 水平方向回転のDelta Angle(Radian)
  double aa = da;
  double y = r * cos(aa) * h2 - dy;
  double rband = r * sin(aa);
  double ab = 0;

  // 頭頂から始まるポリゴンの座標

  for (i = 0; i < nBands; i++) {
    head_pv[0][i].x = rband * sin(ab);
    head_pv[0][i].y = -rband * cos(ab);
    head_pv[0][i].z = -y;
    head_pv[0][i] += bp;
    ab += da * 2;
  }

  // その他のポリゴン（なお、最下部のポリゴンは作ってない）

  for (int iBand = 1; iBand < nBands-1; iBand++) {
    aa += da;

    // あごの部分をパラメータeで補正（えらの張り出し）
    rr = r;
    if (aa > M_PI / 2 && aa <= M_PI / 2 * 1.5) {
      rr += (aa - M_PI / 2) * e * 0.5;
    }
    if (aa > M_PI / 2 * 1.5) {
      rr += (M_PI - aa) * e * 0.5;
    }
    rband = sin(aa) * rr;
    y = cos(aa) * rr * h2;
    if (aa > M_PI / 2 && aa < M_PI / 2 * 3) {
      y *= mo;
    }
    y -= dy;

    double rr2 = r;
    double aa2 = aa + da;
    if (aa2 > M_PI / 2 && aa2 <= M_PI / 2 * 1.5) {
      rr2 += (aa2 - M_PI / 2) * e * 0.5;
    }
    if (aa2 > M_PI / 2 * 1.5) {
      rr2 += (M_PI - aa2) * e * 0.5;
    }

    // パラメータmoに応じて卵型に変形

    double rband2 = sin(aa2) * rr2;
    double y2 = cos(aa2) * rr2 * h2;
    if (aa2 > M_PI / 2 && aa2 < M_PI / 2 * 3) {
      y2 *= mo;
    }
    y2 -= dy;
    ab = 0;

    // 座標生成
    for (i = 0; i < nBands; i++) {
      head_pv[iBand][i].x = rband * sin(ab);
      head_pv[iBand][i].y = rband * cos(ab);
      head_pv[iBand][i].z = -y;

      // 鼻生成
      if (fabs(head_pv[iBand][i].x) < r * 0.2 && head_pv[iBand][i].y <= 0 && y < h0 && y > h0 - nh * h2 * mo) {
        head_pv[iBand][i].y *= 1.0 + ((h0 - y) * 0.3);
      }
      // 目生成
      else if (fabs(h0 - y) < 0.15 && head_pv[iBand][i].y <= 0) {
        head_pv[iBand][i].y *= 0.90f;
        head_pv[iBand][i].x *= 0.90f;
      }

      // あご生成
      if (y < h0)
        head_pv[iBand][i].y -= (h0 - y) * 0.2f;

      head_pv[iBand][i].y *= -1;
      head_pv[iBand][i] += bp;
      ab += da * 2;

    }
  }


  // 各頂点の法線ベクトルを求める
  for (int j = 0; j < nBands; j++) {
    for (i = 0; i < nBands; i++) {
      static CVector3 dx;
      static CVector3 dy;
      dx  = head_pv[j][i] - head_pv[j][(i + 1) % 16];
      dy  = head_pv[j][i] - head_pv[j + 1][i];
      head_n0[j][i] = dx.outprod(dy);
      head_n0[j][i].normalize();
    }
  }

}

// 頭を表示

void CBodyModel::DrawHead(CBodyView* pView)
{
  static CVector3 vpv[16][16];  // ビュー座標保持用
  static CVector3 n[16][16];    // ビュー座標での法線
  static POINT spv[16][16];     // スクリーン座標保持用

  CVector3 l_eye, r_eye;
  CVector3 tn0, tn;
  CVector3 p, p0;
  CVector3 vtp, cp;
  POINT stp;
  POINT buf[4];
  double c[4];
  double z;

  CVector3 vX, vY, vZ;  // 頭の基準系(X-左右方向、Y-前後方向,Z-上下方向)

  // 頭の基準系を求める

  vZ = (mPos[1].vPos - mPos[2].vPos).norm();   // 上下方向は頭・首間の差方向

  cp = (mPos[1].vPos + mPos[2].vPos) / 2.0;    // 頭・首間の中点（頭中心）
  vY = (mPos[3].vPos - cp).norm();             // 前後方向は頭中心と鼻の差方向
  vZ -= vY * (vY.inprod(vZ));                  // vY,vZが必ず直交するように補正
  vZ.normalize();
  vX = (vY).outprod(vZ);                       // vY,vZそれぞれに直交する方向にvX

  CVector3 vXe, vYe, vZe;       // 目の基準系(X-左右方向、Y-前後方向,Z-上下方向)

  vXe = (vX - vZ * eye_angle).norm();
  vYe = vY;
  vZe = vZ;
  l_eye = mPos[1].vPos + vX * eye_gap + vY * 0.7 - vZ * eye_line;
  DrawEye(pView, l_eye, vXe, vYe, vZe, eye_size);

  vXe = (vX + vZ * eye_angle).norm();
  r_eye = mPos[1].vPos + vX * -eye_gap + vY * 0.7 - vZ * eye_line;
  DrawEye(pView, r_eye, vXe, vYe, vZe, eye_size);

  DrawMouth(pView, mPos[1].vPos, vX, vY, vZ, 0.05);

  c[0] = c[1] = c[2] = c[3] = 0;

  int nBands = 16;  // 分割数
  int i;
  int iBand;

  // 頭頂座標変換

  p = vX * head_tp.x + vY * head_tp.y + vZ * head_tp.z;
  p += mPos[1].vPos;
  pView->ChangeWtoV(p, vtp);
  pView->ChangeVtoS(vtp, stp);

  // 座標変換

  for (iBand = 0; iBand < nBands-1; iBand++) {
    for (i = 0; i < nBands; i++) {

      // 頭部各頂点位置をローカル→ワールド→ビュー→スクリーン座標に変換

      p0 = head_pv[iBand][i];                    // ローカル（顔座標系）
      p = vX * p0.x + vY * p0.y + vZ * p0.z;     // ワールド（首相対系）へ変換   
      p += mPos[1].vPos;                         // ワールドへ変換
      pView->ChangeWtoV(p, vpv[iBand][i]);       // ビュー座標へ変換
      pView->ChangeVtoS(vpv[iBand][i], spv[iBand][i]);  // スクリーン座標へ変換

      // 頭部各法線ベクトルをビュー座標系へ変換

      p0 = head_n0[iBand][i];
      p = vX * p0.x + vY * p0.y + vZ * p0.z;
      pView->ChangeWtoV(p, n[iBand][i]);
    }
  }

  // エッジ判定してポリゴン表示

  buf[0] = stp;
  buf[1] = stp;
  tn0.set(0, 0, -1);
  pView->ChangeWtoV(tn0, tn);

  for (i = 0; i < nBands; i++) {
    int ni = (i + 1) % nBands;
    if (tn.y < 0 || n[0][i].y < 0 || n[0][ni].y < 0) { // 裏面チェック
      // 頂点位置セット
      buf[2] = spv[0][(i + 1) % nBands];
      buf[3] = spv[0][i];

      // エッジチェック
      int f = 0;

      // Ｚ値計算（ポリゴン中心を採用）
      z =  (vtp.y + vpv[0][i].y + vpv[0][(i + 1) % nBands].y) / 3;

      // 各頂点光度を計算
      c[0] = c[1] = tn.inprod(vLight);
      c[2] = n[0][ni].inprod(vLight);
      c[3] = n[0][i].inprod(vLight);

      pView->DrawPolygon(buf, 0, c, z, 0);
    }
  }

  for (int j = 0; j < nBands - 2; j++) {
    for (i = 0; i < nBands; i++) {
      int ni = (i + 1) % nBands;
      if (n[j][i].y < 0 || n[j][ni].y < 0 || n[j + 1][i].y < 0 || n[j + 1][ni].y < 0) { // 裏面チェック
        // 頂点位置セット
        buf[0] = spv[j][i];
        buf[1] = spv[j][(i + 1) % nBands];
        buf[2] = spv[j + 1][(i + 1) % nBands];
        buf[3] = spv[j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[j][i].y < 0 && n[j][ni].y > 0 || n[j][i].y > 0 && n[j][ni].y < 0)
          f |= 1;
        if (n[j][i].y < 0 && n[j + 1][i].y > 0 || n[j][i].y > 0 && n[j + 1][i].y < 0)
          f |= 2;

        // Ｚ値計算（ポリゴン中心を採用）
        z =  (vpv[j][i].y + vpv[j][(i + 1) % nBands].y + vpv[j + 1][(i + 1) % nBands].y + vpv[j + 1][i].y) / 4;

        // 各頂点光度を計算
        c[0] = n[j][i].inprod(vLight);
        c[1] = n[j][ni].inprod(vLight);
        c[2] = n[j + 1][ni].inprod(vLight);
        c[3] = n[j + 1][i].inprod(vLight);

        pView->DrawPolygon(buf, 0, c, z, f);
      }
    }
  }
}

// 目を表示
void CBodyModel::DrawEye(CBodyView* pView, CVector3& pos, CVector3& vX, CVector3& vY, CVector3& vZ, double r)
{
  static CVector3 vpv[16][16];  // ビュー座標保持用
  static CVector3 n[16][16];    // ビュー座標での法線
  static POINT spv[16][16];     // スクリーン座標保持用
  static CVector3 n0[16][16];   // ワールド座標での法線

  CVector3 tn0, tn;
  CVector3 p, p0;
  CVector3 vtp, cp;
  POINT buf[4];
  double c[4];
  double z;

  r *= head_r * 0.8;

  int nBands = 12;  // 回転方向分割数
  int pBands = 12;  // 垂直方向分割数
  int i;
  int iBand;
  double ri, rn, a;

  // 座標変換

  for (iBand = 0; iBand < pBands; iBand++) {
    ri = cos((iBand + 2) * M_PI / 15) * r;
    rn = sin((iBand + 2) * M_PI / 15) * r;
    for (i = 0; i < nBands; i++) {
      a = (i + 2) * 2 * M_PI / nBands / 3;

      // 目頂点位置をローカル→ワールド→ビュー→スクリーン座標に変換

      p0 = vX * ri + vZ * (cos(a) * rn) + vY * (sin(a) * rn);     // ワールド座標（相対系）
      p = pos + p0;                                               // ワールド座標（絶対系）
      pView->ChangeWtoV(p, vpv[iBand][i]);                        // ビュー座標へ変換
      pView->ChangeVtoS(vpv[iBand][i], spv[iBand][i]);  // スクリーン座標へ変換

      // 法線ベクトルをビュー座標系へ変換

      n0[iBand][i] = p0;
      pView->ChangeWtoV(p0, n[iBand][i]);
    }
  }

  tn0.set(0, 0, -1);
  pView->ChangeWtoV(tn0, tn);

  pView->ChangeWtoV(vY, p);
  double ecx, ecy;

  if (eye_move) {
    ecx = 5.5 + p.z * 4;  // 眼球移動あり
    ecy = 5.5 - p.x * 4;
  }
  else {
    ecx = 5.5;  // 眼球移動なし
    ecy = 5.5;
  }

  double hcx, hcy;
  hcx = 6.5 + p.z * 4;
  hcy = 4.5 - p.x * 4;

  int eh = eye_height;    // 下まぶた位置
  int eh2 = 5 + eh;       // 上まぶた位置
  switch (eye_timer) {
  case 1:
  case 3:
    eh2 /= 2;
    break;
  case 2:
    eh2 = 0;
  }
  int eh3 = eh2 - 1;      // 上まぶた位置（二重に関する補正値）
  if (double_eyelid)
    eh3--;

  // エッジ判定してポリゴン表示

  c[0] = c[1] = c[2] = c[3] = -1;
  for (i = 0; i < nBands - 1; i++) {
    int ni = i + 1;
    for (int j = 0; j < pBands - 1; j++) {
      if (n[j][i].y < 0 || n[j][ni].y < 0 || n[j + 1][i].y < 0 || n[j + 1][ni].y < 0) { // 裏面チェック
        // 頂点位置セット
        buf[0] = spv[j][i];
        buf[1] = spv[j][i + 1];
        buf[2] = spv[j + 1][i + 1];
        buf[3] = spv[j + 1][i];

        // Ｚ値計算（ポリゴン中心を採用）
        // 目の回りのポリゴンはz値を少なめにして優先的に表示する

        z =  (vpv[j][i].y + vpv[j][i + 1].y + vpv[j + 1][i + 1].y + vpv[j + 1][i].y) / 4 - 0.3;

        if (i > 5 - eh && i < eh2) {      // 眼球内
          double r;

          // 光彩チェック
          r= sqrt((i - ecx) * (i - ecx) * 0.5 + (j - ecy) * (j - ecy) * 1.5);
          c[0] = -1 + r / eye_in_r;
          r = sqrt(((i + 1) - ecx) * ((i + 1) - ecx) * 0.5 + (j - ecy) * (j - ecy) * 1.5);
          c[1] = -1 + r / eye_in_r;
          r = sqrt(((i + 1) - ecx) * ((i + 1) - ecx) * 0.5 + ((j + 1) - ecy) * ((j + 1) - ecy) * 1.5);
          c[2] = -1 + r / eye_in_r;
          r = sqrt((i - ecx) * (i - ecx) * 0.5 + ((j + 1) - ecy) * ((j + 1) - ecy) * 1.5);
          c[3] = -1 + r / eye_in_r;

          // ハイライトチェック
          r= sqrt((i - hcx) * (i - hcx) + (j - hcy) * (j - hcy));
          c[0] += eye_light_r / r;
          r = sqrt(((i + 1) - hcx) * ((i + 1) - hcx) + (j - hcy) * (j - hcy));
          c[1] += eye_light_r / r;
          r = sqrt(((i + 1) - hcx) * ((i + 1) - hcx) + ((j + 1) - hcy) * ((j + 1) - hcy));
          c[2] += eye_light_r / r;
          r = sqrt((i - hcx) * (i - hcx) + ((j + 1) - hcy) * ((j + 1) - hcy));
          c[3] += eye_light_r / r;

          // 上アイラインチェック
          if (i >= eh3 && !eye_timer || i == eh2 - 1) {
            c[0] = -1;
            c[1] = 1;
            c[2]  = 1;
            c[3]  = -1;
            pView->DrawPolygon(buf, 4, c, z, 0);
          }
          else
            pView->DrawPolygon(buf, 2, c, z, 0);
        }
        else {  // 眼球外（まぶた）
          // 各頂点光度を計算
          c[0] = n[j][i].inprod(vLight);
          c[1] = n[j][ni].inprod(vLight);
          c[2] = n[j + 1][ni].inprod(vLight);
          c[3] = n[j + 1][i].inprod(vLight);

          pView->DrawPolygon(buf, 0, c, z, 0);
        }
      }
    }
  }
}

// 口を表示
void CBodyModel::DrawMouth(CBodyView* pView, CVector3& pos, CVector3& vX, CVector3& vY, CVector3& vZ, double r)
{
  static CVector3 vpv[16][16];  // ビュー座標保持用
  static CVector3 n[16][16];    // ビュー座標での法線
  static CVector3 n0[16][16];   // ワールド座標での法線

  CVector3 tn0, tn;
  CVector3 p, p0;
  CVector3 vtp, cp;
  POINT buf[4];
  double c[4];
  double z;

  r *= head_r;

  p = pos + vY * 1.1 - vZ * eye_line * 0.4 + vX * r;
  pView->ChangeWtoV(p, vpv[0][0]);
  pView->ChangeVtoS(vpv[0][0], buf[0]);
  p = pos + vY * 1.1 - vZ * eye_line * 0.4 + vX * -r;
  pView->ChangeWtoV(p, vpv[0][1]);
  pView->ChangeVtoS(vpv[0][1], buf[1]);
  p = pos + vY * 1.0 - vZ * eye_line * (0.4 - r * 0.2) + vX * (-r * 0.8);
  pView->ChangeWtoV(p, vpv[0][2]);
  pView->ChangeVtoS(vpv[0][2], buf[2]);
  p = pos + vY * 1.0 - vZ * eye_line * (0.4 - r * 0.2) + vX * (r * 0.8);
  pView->ChangeWtoV(p, vpv[0][3]);
  pView->ChangeVtoS(vpv[0][3], buf[3]);

  pView->ChangeWtoV(vY, n[0][0]);

  // Ｚ値計算（ポリゴン中心を採用）
  z =  (vpv[0][0].y + vpv[0][1].y + vpv[0][2].y + vpv[0][3].y) / 4.0 - 1;

  // 各頂点光度を計算
  c[0] = c[1] = c[2] = c[3] = -1;

  pView->DrawPolygon(buf, 0, c, z, 15);
}

// 髪の毛を表示
void CBodyModel::DrawHair(CBodyView* pView)
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 n[2][20][16];         // 各頂点での法線ベクトル（ワールド座標系）
  static CVector3 n0[2][20][16];        // 各頂点での法線ベクトル（ビュー座標系）
  static CVector3 l[2][20][16];         // 各頂点位置（ビュー座標系）
  static POINT s[2][20][16];            // 各頂点位置（スクリーン座標系）

  CVector3 vX, vY, vZ;
  CVector3 cp, dx, dy;
  int i, j, k;

  for (k = 0; k < 2; k++) { // k=0で外周、k=1で前髪
    for (i = 0; i < 16; i++) { // 円周方向へのループ
      for (j = 0; j < 20; j++) { // つむじ→髪先端方向へのループ
        // 各頂点をスクリーン座標に変換
        pView->ChangeWtoV(hair_p[k][j][i], l[k][j][i]);
        pView->ChangeVtoS(l[k][j][i], s[k][j][i]);
      }
    }
  }

  // 各頂点の法線ベクトルを求める
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 16; i++) {
      dx  = hair_p[0][j][(i + 1) % 16] - hair_p[0][j][i];
      dy  = hair_p[0][j][i] - hair_p[0][j + 1][i];
      n0[0][j][i] = dx.outprod(dy);
      n0[0][j][i].normalize();
      pView->ChangeWtoV(n0[0][j][i], n[0][j][i]);
    }
  }
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 16; i++) {
      dx  = hair_p[1][j][i + 1] - hair_p[1][j][i];
      dy  = hair_p[1][j][i] - hair_p[1][j + 1][i];
      n0[1][j][i] = dx.outprod(dy);
      n0[1][j][i].normalize();
      pView->ChangeWtoV(n0[1][j][i], n[1][j][i]);
    }
  }
  for (k = 0; k < 2; k++) { // k=0で外周、k=1で前髪
    for (i = 0; i < 16; i++) {
      n0[k][19][i] = n0[k][18][i];
      n[k][19][i] = n[k][18][i];
    }
  }
  for (j = 0; j < 20; j++) {
    n0[1][j][15] = n0[1][j][14];
    n[1][j][15] = n[1][j][14];
  }


  // エッジ判定してポリゴン表示

  POINT buf[4];
  double c[4];
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 16; i++) {
      if (hair_max[i] > j) {
        int ni = (i + 1) % 16;
        // 頂点位置セット
        buf[0] = s[0][j][i];
        buf[1] = s[0][j][ni];
        buf[2] = s[0][j + 1][ni];
        buf[3] = s[0][j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[0][j][i].y < 0 && n[0][j][ni].y > 0 || n[0][j][i].y > 0 && n[0][j][ni].y < 0)
          f |= 1;
        if (n[0][j][i].y < 0 && n[0][j + 1][i].y > 0 || n[0][j][i].y > 0 && n[0][j + 1][i].y < 0)
          f |= 2;

        // Ｚ値計算（ポリゴン中心を採用）
        double z = (l[0][j][i].y + l[0][j][ni].y + l[0][j + 1][ni].y + l[0][j + 1][i].y) / 4.0;

        // 各頂点光度を計算
        c[0] = n[0][j][i].inprod(vLight);
        c[1] = n[0][j][ni].inprod(vLight);
        c[2] = n[0][j + 1][ni].inprod(vLight);
        c[3] = n[0][j + 1][i].inprod(vLight);

        pView->DrawPolygon(buf, 1, c, z, f);
      }
    }
  }
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 15; i++) {
      if (hair_max2[i] > j) {
        int ni = i + 1;
        // 頂点位置セット
        buf[0] = s[1][j][i];
        buf[1] = s[1][j][ni];
        buf[2] = s[1][j + 1][ni];
        buf[3] = s[1][j + 1][i];

        // エッジチェック
        int f = 0;
        if (n[1][j][i].y < 0 && n[1][j][ni].y > 0 || n[1][j][i].y > 0 && n[1][j][ni].y < 0)
          f |= 1;
        if (n[1][j][i].y < 0 && n[1][j + 1][i].y > 0 || n[1][j][i].y > 0 && n[1][j + 1][i].y < 0)
          f |= 2;

        // Ｚ値計算（ポリゴン中心を採用）
        double z = (l[1][j][i].y + l[1][j][ni].y + l[1][j + 1][ni].y + l[1][j + 1][i].y) / 4.0;

        // 各頂点光度を計算
        c[0] = n[1][j][i].inprod(vLight);
        c[1] = n[1][j][ni].inprod(vLight);
        c[2] = n[1][j + 1][ni].inprod(vLight);
        c[3] = n[1][j + 1][i].inprod(vLight);

        pView->DrawPolygon(buf, 1, c, z, 0);
      }
    }
  }
}

// 髪の毛を移動
void CBodyModel::MoveHair()
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 p[2][20][16];         // 各頂点位置（ワールド座標系）

  CVector3 vX, vY, vZ;
  CVector3 cp, dx, dy;
  int i, j, k;

  // 高速化のため、Sin, Cosテーブルを作っておく
  // sint[0]が髪全体、sint[1]が前髪用
  double sint[2][16], cost[2][16];
  for (i = 0; i < 16; i++) {
    double a = i / 16.0 * M_PI * 2;
    cost[0][i] = cos(a);
    sint[0][i] = sin(a);
    a = i / 32.0 * M_PI + M_PI * 0.75;
    cost[1][i] = cos(a);
    sint[1][i] = sin(a);
  }

  // 頭の基準系を求める

  vZ = (mPos[1].vPos - mPos[2].vPos).norm();      // 上下方向は頭・首間の差方向

  cp = (mPos[1].vPos + mPos[2].vPos) / 2.0;       // 頭・首間の中点（頭中心）
  vY = (cp - mPos[3].vPos).norm();                // 前後方向は頭中心と鼻の差方向
  vZ -= vY * (vY.inprod(vZ));                     // vY,vZが必ず直交するように補正
  vZ.normalize();
  vX = (vY).outprod(vZ);                          // vY,vZそれぞれに直交する方向にvX

  double x, y, vx, vy;

  // 髪の毛の定常位置（t→∞）を求める

  cp = vX * head_tp.x + vY * head_tp.y + vZ * head_tp.z;
  cp += mPos[1].vPos;
  for (k = 0; k < 2; k++) { // k=0で外周、k=1で前髪
    for (i = 0; i < 16; i++) { // 円周方向へのループ
      x = y = 0;
      if (k) {
        vx = hair_vx2;
        vy = hair_vy2;
      }
      else {
        vx = hair_vx;
        vy = hair_vy;
      }

      for (j = 0; j < 20; j++) { // つむじ→髪先端方向へのループ
        p[k][j][i] = cp + vZ * (y * head_r);
        p[k][j][i] += vX * (sint[k][i] * x * head_r) + vY * (cost[k][i] * x * head_r);

        x += vx;
        y += vy;
        if (k) {
          vy += hair_gy2 + cost[k][i]  * hair_angle2;
          vx += hair_gx2;
        }
        else {
          vy += hair_gy + cost[k][i]  * hair_angle;
          vx += hair_gx;
        }
      }
    }
  }

  // 定常位置と現在位置の合成で新位置を定める

  if (!hair_init) {
    // 位置初期化
    for (k = 0; k < 2; k++) { // k=0で外周、k=1で前髪
      for (i = 0; i < 16; i++) { // 円周方向へのループ
        for (j = 0; j < 20; j++) { // つむじ→髪先端方向へのループ
          hair_p[k][j][i] = p[k][j][i];
        }
      }
    }
    hair_init = true;
  }
  else {
    cp = vX * head_cp.x + vY * head_cp.y + vZ * head_cp.z;
    cp += mPos[1].vPos;
    for (k = 0; k < 2; k++) { // k=0で外周、k=1で前髪
      for (i = 0; i < 16; i++) { // 円周方向へのループ
        for (j = 0; j < 20; j++) { // つむじ→髪先端方向へのループ
          double a;
          // 髪の毛の末端へ行くほど遅れて動く
          if (!j) {
            hair_p[k][j][i] = p[k][j][i];
          }
          else {
            if (j < 12)
              hair_p[k][j][i] = p[k][j][i];
            else {
              // 擬似重力
              hair_p[k][j][i].z += 1.0;

              a = (j - 12) / (8.0 + k * 3);
              hair_p[k][j][i] = hair_p[k][j][i] * a + p[k][j][i] * (1 - a);
            }

            // 髪の毛の伸縮を防止(髪の毛方向の各点距離を定常時の値に補正)
            CVector3 vN;
            vN = (hair_p[k][j][i] - hair_p[k][j - 1][i]).norm();
            double dis = (p[k][j][i] - p[k][j - 1][i]).abs();
            hair_p[k][j][i] = hair_p[k][j - 1][i] + (vN * dis);
          }

          // 頭との干渉チェック
          CVector3 dp = hair_p[k][j][i] - cp;
          double dr = dp.abs();
          if (dr < head_r + 0.5) {
            hair_p[k][j][i] = cp + dp * ((head_r + 0.5) / dr);
          }
        }
      }
    }
  }
}

// スカートを表示
void CBodyModel::DrawSkirt(CBodyView* pView)
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 n[20][16];            // 各頂点での法線ベクトル（ワールド座標系）
  static CVector3 n0[20][16];           // 各頂点での法線ベクトル（ビュー座標系）
  static CVector3 l[20][16];            // 各頂点位置（ビュー座標系）
  static POINT s[20][16];               // 各頂点位置（スクリーン座標系）

  CVector3 vX, vY, vZ;
  CVector3 cp, dx, dy;
  int i, j;

  for (i = 0; i < 16; i++) { // 円周方向へのループ
    for (j = 0; j < 20; j++) { // 元→先へのループ
      // 各頂点をスクリーン座標に変換
      pView->ChangeWtoV(skirt_p[j][i], l[j][i]);
      pView->ChangeVtoS(l[j][i], s[j][i]);
    }
  }

  // 各頂点の法線ベクトルを求める
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 16; i++) {
      dx  = skirt_p[j][(i + 1) % 16] - skirt_p[j][i];
      dy  = skirt_p[j][i] - skirt_p[j + 1][i];
      n0[j][i] = dx.outprod(dy);
      n0[j][i].normalize();
      pView->ChangeWtoV(n0[j][i], n[j][i]);
    }
  }
  for (i = 0; i < 16; i++) {
    n0[19][i] = n0[18][i];
    n[19][i] = n[18][i];
  }

  // エッジ判定してポリゴン表示

  POINT buf[4];
  double c[4];
  for (j = 0; j < 19; j++) {
    for (i = 0; i < 16; i++) {
      int ni = (i + 1) % 16;
      // 頂点位置セット
      buf[0] = s[j][i];
      buf[1] = s[j][ni];
      buf[2] = s[j + 1][ni];
      buf[3] = s[j + 1][i];

      // エッジチェック
      int f = 0;
      if (n[j][i].y < 0 && n[j][ni].y > 0 || n[j][i].y > 0 && n[j][ni].y < 0)
        f |= 1;
      if (j == 18 || n[j][i].y < 0 && n[j + 1][i].y > 0 || n[j][i].y > 0 && n[j + 1][i].y < 0)
        f |= 2;

      // Ｚ値計算（ポリゴン中心を採用）
      double z = (l[j][i].y + l[j][ni].y + l[j + 1][ni].y + l[j + 1][i].y) / 4.0;

      // 各頂点光度を計算
      c[0] = n[j][i].inprod(vLight);
      c[1] = n[j][ni].inprod(vLight);
      c[2] = n[j + 1][ni].inprod(vLight);
      c[3] = n[j + 1][i].inprod(vLight);

      pView->DrawPolygon(buf, 3, c, z, f);
    }
  }
}

// スカートを移動
void CBodyModel::MoveSkirt(int center, int top, int p0, int p1)
{
  // ２次元配列のインデックスは[元から先][円周方向]

  static CVector3 p[20][16];        // 各頂点位置（ワールド座標系）

  CVector3 vXb, vYb, vZb;
  CVector3 vXc, vYc, vZc;
  CVector3 vXm, vYm, vZm;
  CVector3 bcp, bcp2, cp, d, d2, lb, rb, lb2, rb2, dx, dy;
  double dis2;
  int i, j;

  // 高速化のため、Sin, Cosテーブルを作っておく
  double sint[16], cost[16];
  for (i = 0; i < 16; i++) {
    double a = i / 16.0 * M_PI * 2;
    cost[i] = cos(a);
    sint[i] = sin(a);
  }

  // 基準位置決定
  // lb,rbが足元,lb2,rb2がひざ

  lb = mPos[p0].vPos;
  rb = mPos[p1].vPos;
  lb2 = mPos[p0 + 1].vPos;
  rb2 = mPos[p1 + 1].vPos;

  cp = mPos[center].vPos;
  bcp = (lb + rb) / 2;
  bcp2 = (lb2 + rb2) / 2;

  d = bcp - cp;
  d2 = bcp2 - bcp;
  dis2 = (lb2 - rb2).abs2();
  if (dis2 < 40)
    dis2 = 40;

  // 基準系決定
  // v?bが足元位置での基準系
  // v?cがひざ位置での基準系

  vXb = (lb - rb).norm();
  vZb = d.norm() * -1;
  vYb = vXb.outprod(vZb);

  vXc = (lb2 - rb2).norm();
  vZc = d2.norm() * -1;
  vYc = vXc.outprod(vZc);

  double x, y, vx, vy, oy;

  // スカートの定常位置（t→∞）を求める

  for (i = 0; i < 16; i++) { // 円周方向へのループ
    x = 1;
    y = 0;
    vx = 0.2;
    vy = -0.4;

    for (j = 0; j < 20; j++) { // 元→先端方向へのループ
      if (j < 3) {
        // 腰近辺
        p[j][i] = cp + vZb * y;
        p[j][i] += vXb * (sint[i] * x * 1.4) + vYb * (cost[i] * x);
      }
      else if (j < 9) {
        // 尻近辺
        if (j == 3)
          oy = y;
        double a = (j - 3) / 5.0;
        vXm = (vXb * (1 - a) + vXc * a).norm();
        vYm = (vYb * (1 - a) + vYc * a).norm();
        vZm = (vZb * (1 - a) + vZc * a).norm();
        p[j][i] = (cp + vZb * y) * (1 - a) + (bcp + vZc * (y - oy)) * a;
        p[j][i] += vXm * (sint[i] * x * 1.4) + vYm * (cost[i] * x);
      }
      else {
        // 太もも近辺

        // 足の形状から線形補間
        double a = (19 - j) / 10.0;
        double b = 1 - a;
        CVector3 lb3, rb3;
        lb3 = lb * a + lb2 * b;
        rb3 = rb * a + rb2 * b;
        double rr = (lb3 - rb3).abs() * 0.5 + 2;

        // 放物面と足の形状から線形補間した形状を合成
        double rx = (sint[i] * (x * 1.4 * a + rr * b));
        double ry = (cost[i] * (x * a + 3 * b));
        p[j][i] = bcp + vZc * (y - oy);
        p[j][i] += vXc * rx + vYc * ry;
      }

      vx -= 0.01;
      x += dis2 / 800.0;
      y += dis2 / 800.0;
      x += vx;
      y += vy;
    }
  }

  // 定常位置と現在位置の合成で新位置を定める

  if (!skirt_init) {
    // 位置初期化
    for (i = 0; i < 16; i++) { // 円周方向へのループ
      for (j = 0; j < 20; j++) { // スカート元→先方向へのループ
        skirt_p[j][i] = p[j][i];
      }
    }
    skirt_init = true;
  }
  else {
    for (i = 0; i < 16; i++) { // 円周方向へのループ
      double am = sin(i / 8.0 * M_PI);
      am = 1 - am * am;
      for (j = 0; j < 20; j++) { // スカート元→先方向へのループ
        double a;
        // スカートの末端と足の間が遅れて動く
        if (j < 18)
          a = 5.0 / (20 - j) * am;
        else
          a = 0.8;
        if (a > 0.8)
          a = 0.8;
        if (a > j / 10.0)
          a = j / 10.0;

        // 擬似重力
        skirt_p[j][i].z += 1.0;

        skirt_p[j][i] = skirt_p[j][i] * a + p[j][i] * (1 - a);

        if (j) {
          // スカートの伸縮を防止(各点距離を定常時の値に補正)
          CVector3 vN;
          vN = (skirt_p[j][i] - skirt_p[j - 1][i]).norm();
          double dis = (p[j][i] - p[j - 1][i]).abs();
          skirt_p[j][i] = skirt_p[j - 1][i] + (vN * dis);
        }
      }
    }
  }
}

// 制御点間の距離を求める

double CBodyModel::CalcDis(int p0, int p1)
{
  CVector3 d;
  d = mPos[p0].vInitPos - mPos[p1].vInitPos;
  return d.abs();
}


// 制御点間制約のサブステップ処理
// p0, p1が繋がっている制御点、rが基準距離、tはDT

void CBodyModel::CalcSub(int p0, int p1, double r, double t)
{
  CVector3 f, d;
  double dd;

  f.set(0, 0, 0);

  // 制御点間の差ベクトル
  d = mPos[p0].vPos - mPos[p1].vPos;

  // 制御点間距離
  dd = d.abs();

  // 位置の差を単位ベクトル化
  d /= dd;

  // 基準距離との差の分、d方向に移動
  f = d * ((r - dd) * t);
  mPos[p0].vOldPos += f;
  mPos[p1].vOldPos -= f;
}


// 制御点間と初期位置間の制約処理
// 制御点が初期位置から余り離れて欲しくないときに用いる

void CBodyModel::CalcSubConst(int p, double t)
{
  CVector3 f, d;
  double dd;
  f.set(0, 0, 0);

  // 初期位置間の差ベクトル
  d = mPos[p].vPos - mPos[p].vInitPos;

  // ずれ距離
  dd = d.abs();

  // ずれの分、d方向に移動
  f = d * t;
  mPos[p].vOldPos -= f;
}

// 各制御点位置を更新

void CBodyModel::PosCalc()
{
  int i, j;
  double dt = 0.1;
  for (i = 0; i < 10; i++) {

    // 各制御点を保存しておく
    for (j = 0; j < cPosMax; j++) {
      mPos[j].vOldPos = mPos[j].vPos;
    }

    // 各接続処理
    CalcSub(0, 1, distance[0], dt);
    CalcSub(10, 13, distance[1], dt);
    CalcSub(0, 10, distance[2], dt);
    CalcSub(10, 11, distance[3], dt);
    CalcSub(11, 12, distance[4], dt);
    CalcSub(0, 13, distance[5], dt);
    CalcSub(13, 14, distance[6], dt);
    CalcSub(14, 15, distance[7], dt);
    CalcSub(0, 16, distance[8], dt);
    CalcSub(16, 17, distance[9], dt);
    CalcSub(17, 18, distance[10], dt);
    CalcSub(0, 19, distance[11], dt);
    CalcSub(19, 20, distance[12], dt);
    CalcSub(20, 21, distance[13], dt);
    CalcSub(16, 19, distance[14], dt);
    CalcSub(10, 16, distance[15], dt);
    CalcSub(13, 19, distance[16], dt);
    CalcSub(1, 10, distance[17], dt);
    CalcSub(1, 13, distance[18], dt);
    CalcSub(1, 2, distance[19], dt);
    CalcSub(0, 2, distance[20], dt);
    CalcSub(2, 3, distance[21], dt);
    CalcSub(1, 3, distance[22], dt);
    CalcSub(13, 16, distance[23], dt);
    CalcSub(10, 19, distance[24], dt);

    /*
      // 定常点処理
      CalcSubConst(1, dt);        // 首
      CalcSubConst(10, dt);       // 右肩
      CalcSubConst(13, dt);       // 左肩
      CalcSubConst(16, dt);       // 右足もと
      CalcSubConst(19, dt);       // 左足もと
    */

    // 制御点位置更新
    for (j = 0; j < cPosMax; j++) {
      mPos[j].vPos = mPos[j].vOldPos;
    }
          
    // 制約条件チェック
    ConditionCheck();
  }

  MoveHair();
  MoveSkirt(0, 1, 16, 19);

  // 目のまばたき処理
  if (wink) {
    if (eye_timer) {
      eye_timer--;
    }
    else if (rand() % 20 < 1) {
      eye_timer = 3;
    }
  }
}

// 各関節の制約チェック
void CBodyModel::ConditionCheck()
{
  CVector3 dp, dp2, cp, a0, a1, a2;
  CVector3 vX, vY, vZ;
  double x, y, a;

  // 上半身の基準系を求める

  vX = (mPos[13].vPos - mPos[10].vPos).norm();
  vZ = ((mPos[13].vPos + mPos[10].vPos) / 2 - mPos[0].vPos).norm();
  vY = vX.outprod(vZ);

  // 顔が頭より前かチェック
  cp = (mPos[1].vPos + mPos[2].vPos) * 0.5; //頭の中心位置
  dp = mPos[3].vPos - cp; //顔の相対位置
  y = vY.inprod(dp);
  if (y > -0.5) {
    mPos[3].vPos -= vY * 0.1;
    mPos[1].vPos += vY * 0.1;
    mPos[2].vPos += vY * 0.1;
  }

  // 右ひじチェック
  dp = mPos[11].vPos - mPos[10].vPos; //右ひじの相対位置
  x = vX.inprod(dp);
  y = vY.inprod(dp);
  if (y > 0) { // 肩よりひじが前
    mPos[10].vPos += vY * (y * 0.5);
    mPos[11].vPos -= vY * (y * 0.5);
  }
  if (x > 0) { // ひじは肩より外側
    mPos[10].vPos += vX * (x * 0.5);
    mPos[11].vPos -= vX * (x * 0.5);
  }

  // 右上腕基準系チェック
  dp2 = mPos[12].vPos - mPos[11].vPos;

  a0 = dp.norm();
  a1 = dp.outprod(dp2);
  a1.normalize();
  a2 = a0.outprod(a1);
  a2.normalize();

  if (a1.inprod(vZ) > 0) { // 上腕回転角度異常
    a = a2.inprod(vZ);
    if (a < -0.5) { // 上にひねりすぎ
      mPos[11].vPos -= a1 * 0.1;
      mPos[12].vPos += a1 * 0.1;
    }
    else if (a > 0.5) { // 下にひねりすぎ
      mPos[11].vPos += a1 * 0.1;
      mPos[12].vPos -= a1 * 0.1;
    }
    else {  // ひじが開きすぎ
      mPos[11].vPos -= a2 * 0.1;
      mPos[12].vPos += a2 * 0.1;
    }
  }

  // 左ひじチェック
  dp = mPos[14].vPos - mPos[13].vPos; //左ひじの相対位置
  x = vX.inprod(dp);
  y = vY.inprod(dp);
  if (y > 0) {
    mPos[13].vPos += vY * (y * 0.5);
    mPos[14].vPos -= vY * (y * 0.5);
  }
  if (x < 0) {
    mPos[13].vPos += vX * (x * 0.5);
    mPos[14].vPos -= vX * (x * 0.5);
  }

  // 左上腕基準系チェック
  dp2 = mPos[15].vPos - mPos[14].vPos;

  a0 = dp.norm();
  a1 = dp.outprod(dp2);
  a1.normalize();
  a2 = a0.outprod(a1);
  a2.normalize();

  if (a1.inprod(vZ) < 0) { // 上腕回転角度異常
    a = a2.inprod(vZ);
    if (a < -0.5) { // 上にひねりすぎ
      mPos[14].vPos += a1 * 0.1;
      mPos[15].vPos -= a1 * 0.1;
    }
    else if (a > 0.5) { // 下にひねりすぎ
      mPos[14].vPos -= a1 * 0.1;
      mPos[15].vPos += a1 * 0.1;
    }
    else {  // ひじが開きすぎ
      mPos[14].vPos -= a2 * 0.1;
      mPos[15].vPos += a2 * 0.1;
    }
  }

  // 下半身の基準系を求める

  vX = (mPos[19].vPos - mPos[16].vPos).norm();
  vZ = (mPos[0].vPos - (mPos[19].vPos + mPos[16].vPos) / 2).norm();
  vY = vX.outprod(vZ);

  // 右ひざチェック
  dp = mPos[17].vPos - mPos[16].vPos; //右ひざの相対位置
  x = vX.inprod(dp);
  y = vY.inprod(dp);
  if (y > 0) {
    mPos[16].vPos += vY * (y * 0.5);
    mPos[17].vPos -= vY * (y * 0.5);
  }
  if (x > 0) { // ひじは肩より外側
    mPos[16].vPos += vX * (x * 0.5);
    mPos[17].vPos -= vX * (x * 0.5);
  }

  // 右上肢基準系チェック
  dp2 = mPos[18].vPos - mPos[17].vPos;

  a0 = dp.norm();
  a1 = dp.outprod(dp2);
  a1.normalize();
  a2 = a0.outprod(a1);
  a2.normalize();

  if (a1.inprod(vX) > 0) { // 上肢回転角度異常
    a = a2.inprod(vX);
    if (a < -0.5) { // 内にひねりすぎ
      mPos[17].vPos -= a1 * 0.1;
      mPos[18].vPos += a1 * 0.1;
    }
    else if (a > 0.5) { // 外にひねりすぎ
      mPos[17].vPos += a1 * 0.1;
      mPos[18].vPos -= a1 * 0.1;
    }
    else {  // ひざが開きすぎ
      mPos[17].vPos -= a2 * 0.1;
      mPos[18].vPos += a2 * 0.1;
    }
  }

  // 左ひざチェック
  dp = mPos[20].vPos - mPos[19].vPos; //左ひざの相対位置
  x = vX.inprod(dp);
  y = vY.inprod(dp);
  if (y > 0) {
    mPos[19].vPos += vY * (y * 0.5);
    mPos[20].vPos -= vY * (y * 0.5);
  }
  if (x < 0) { // ひじは肩より外側
    mPos[19].vPos += vX * (x * 0.5);
    mPos[20].vPos -= vX * (x * 0.5);
  }

  // 左上肢基準系チェック
  dp2 = mPos[21].vPos - mPos[20].vPos;

  a0 = dp.norm();
  a1 = dp.outprod(dp2);
  a1.normalize();
  a2 = a0.outprod(a1);
  a2.normalize();

  if (a1.inprod(vX) > 0) { // 上肢回転角度異常
    a = a2.inprod(vX);
    if (a < -0.5) { // 内にひねりすぎ
      mPos[20].vPos -= a1 * 0.1;
      mPos[21].vPos += a1 * 0.1;
    }
    else if (a > 0.5) { // 外にひねりすぎ
      mPos[20].vPos += a1 * 0.1;
      mPos[21].vPos -= a1 * 0.1;
    }
    else {  // ひざが開きすぎ
      mPos[20].vPos -= a2 * 0.1;
      mPos[21].vPos += a2 * 0.1;
    }
  }

}

// 指定された制御点と特定のスクリーン座標との距離を求める

double CBodyModel::CalcScreenDis(CBodyView* pView, int p0, int sx, int sy)
{
  CVector3 p1;
  POINT p2;

  pView->ChangeWtoV(mPos[p0].vPos, p1);
  pView->ChangeVtoS(p1, p2);
  p2.x -= sx;
  p2.y -= sy;
  return sqrt(p2.x * p2.x + p2.y * p2.y);
}


// 指定された制御点が特定のスクリーンエリアに含まれているか調べる

BOOL CBodyModel::InScreenArea(CBodyView* pView, int no, int sx, int sy, int ex, int ey)
{
  CVector3 p, p1;
  POINT p2;

  pView->ChangeWtoV(mPos[no].vPos, p1);
  pView->ChangeVtoS(p1, p2);
  return p2.x > sx && p2.y > sy && p2.x < ex && p2.y < ey;
}


// 髪形状を文字列で返す
void CBodyModel::HairMax(char* sBuf)
{
  char buf[256];
  char buf2[256];
  int i;

  sprintf(buf, "%d", hair_max[0]);
  for (i = 1; i < 16; i++) {
    sprintf(buf2, " %d", hair_max[i]);
    strcat(buf, buf2);
  }

  strcpy(sBuf, buf);
}

void CBodyModel::HairMax2(char* sBuf)
{
  char buf[256];
  char buf2[256];
  int i;

  sprintf(buf, "%d", hair_max2[0]);
  for (i = 1; i < 16; i++) {
    sprintf(buf2, " %d", hair_max2[i]);
    strcat(buf, buf2);
  }

  strcpy(sBuf, buf);
}

// 文字列で髪形状をセット
void CBodyModel::SetHairMax(char* sBuf)
{
  for (int i = 0; i < 16; i++)
    hair_max[i] = GetPosNo(sBuf, i);
}

void CBodyModel::SetHairMax2(char* sBuf)
{
  for (int i = 0; i < 16; i++)
    hair_max2[i] = GetPosNo(sBuf, i);
}

