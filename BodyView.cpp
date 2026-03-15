// BodyView.cpp : CBodyView クラスの動作の定義を行います。
//

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <math.h>
#include "Vector3.h"
#include "Body.h"
#include "BodyModel.h"
#include "BodyView.h"

/////////////////////////////////////////////////////////////////////////////
// CBodyView

/////////////////////////////////////////////////////////////////////////////
// CBodyView クラスの構築/消滅

CBodyView::CBodyView()
{
  camera_v.set(0.0, 0.0, 0.0);  // カメラ移動速度

  double ax = -0.2;
  double ay = 0.0;
  double az = -7.2;
  angle.set(ax, ay, az);        // ワールド初期回転角度

  double px = 0.0;
  double py = -64.0;
  double pz = 0.0;
  camera_pos.set(px, py, pz);   // カメラ初期位置

  body[0].SetName("Body1");
  body[1].SetName("Body2");
  body[0].Init();
  body[1].Init();

  iBodyNo = 0;
  vp.set(0, 0, 0);
  bLeftButton = false;
  bWireView = true;
  bPointView = false;
  bEdgeView= false;
  bUseEdge = true;
  bFlatShade = false;
  bUseBody[0] = true;
  bUseBody[1] = false;
  bPointSelectFlag = false;
  iTime = 0;

  for (int i = 0; i < cPosMax; i++)
    bPointSelect[i] = false;

  body[1].Move(CVector3(0, 10, 0));     // 人体２を少し動かしておく
}

CBodyView::~CBodyView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CBodyView クラスの描画

// 制御点選択クリア

void CBodyView::ClearPointSelect()
{
  for (int i = 0; i < cPosMax; i++)
    bPointSelect[i] = false;
}

// 変換行列を計算

void CBodyView::mcalc()
{
  double sasc, sacc;
  double sina, cosa, sinb, cosb, sinc, cosc;

  sina = sin(angle.x); cosa = cos(angle.x);
  if (fabs(cosa) < 1e-9)
    cosa = sgn(cosa) * 1e-9;
  sinb = sin(angle.y); cosb = cos(angle.y);
  sinc = sin(angle.z); cosc = cos(angle.z);
  sasc = sina * sinc;
  sacc = sina * cosc;

  vNorm[0].set(cosb * cosc - sasc * sinb, -cosb * sinc - sacc * sinb, -sinb * cosa);
  vNorm[1].set(cosa * sinc, cosa * cosc, -sina);
  vNorm[2].set(sinb * cosc + sasc * cosb, -sinb * sinc + sacc * cosb, cosb * cosa);
}

// ワールド座標→ビュー座標変換

void CBodyView::ChangeWtoV(CVector3& vWorldPos, CVector3& vViewPos)
{
  vViewPos.x = vWorldPos.inprod(vNorm[0]);
  vViewPos.y = vWorldPos.inprod(vNorm[1]);
  vViewPos.z = vWorldPos.inprod(vNorm[2]);
}

// ビュー座標→ワールド座標変換

void CBodyView::ChangeVtoW(CVector3& vViewPos, CVector3& vWorldPos)
{
  vWorldPos.x = vViewPos.x * vNorm[0].x + vViewPos.y * vNorm[1].x + vViewPos.z * vNorm[2].x;
  vWorldPos.y = vViewPos.x * vNorm[0].y + vViewPos.y * vNorm[1].y + vViewPos.z * vNorm[2].y;
  vWorldPos.z = vViewPos.x * vNorm[0].z + vViewPos.y * vNorm[1].z + vViewPos.z * vNorm[2].z;
}

#define Q 100   // 視野角決定用パラメータ

// ビュー座標→スクリーン座標変換

void CBodyView::ChangeVtoS(CVector3& vViewPos, POINT& sScreenPos)
{
  double z = (vViewPos.y - camera_pos.y) / 10;
  sScreenPos.x = (int)((vViewPos.x - camera_pos.x) * Q / z + 200);
  sScreenPos.y = (int)((vViewPos.z - camera_pos.z) * Q / z + 150);
}

// 画面表示

void CBodyView::Draw() 
{
  XFillRectangle(d, mbm, gc, 0, 0, 400, 400);
  mcalc();

  iPolyCount = 0;       // ポリゴンバッファ初期化
  iTime++;

  // 各ボディ表示内容をポリゴンバッファにストック
  int i;
  if (iTime > 2) {
    for (i = 0; i < 2; i++)
      if (bUseBody[i])
        body[i].Draw(this);
  }

  // ポリゴンバッファをバックバッファにフラッシュ
  FlushPolyBuffer();

  // 各制御点をバックバッファに表示
  if (bPointView) {
    for (i = 0; i < 2; i++)
      if (bUseBody[i])
        body[i].DrawPoint(this);
  }

  // ドラッグ中ならバックバッファに枠表示
  if (!bPointSelectFlag && bLeftButton) {
    int x1, x2, y1, y2;
    x1 = min(sx, dsx);
    x2 = max(sx, dsx);
    y1 = min(sy, dsy);
    y2 = max(sy, dsy);
    XDrawRectangle(d, mbm, spen, x1, y1, x2 - x1, y2 - y1);
  }

  // 表示ポリゴン数を表示
  char buf[32];
  sprintf(buf, "%4d poly", iPolyCount);
  XDrawString(d, mbm, spen, 10, 20, buf, strlen(buf));

  ctTime[iTime % 20] = clock();
  if (iTime > 20) {
    sprintf(buf, "%5.1f FPS", 10.0 / ((ctTime[iTime % 20] - ctTime[(iTime + 1) % 20]) / 20.0));
    XDrawString(d, mbm, spen, 10, 40, buf, strlen(buf));
  }

  // バックバッファをフロントバッファにコピー
  XCopyArea(d, mbm, w, gc, 0, 0, 400, 400, 0, 0);
}

// Ｚソート用Ｚ値比較関数（qsortから使用）

int CmpZ(const void* p1, const void* p2) {
  return ((((CPolyIndex*)p1)->z < ((CPolyIndex*)p2)->z) ? 1 : -1);
}


// ポリゴンバッファの表示

void CBodyView::FlushPolyBuffer()
{
  // ポリゴンをＺソート（クイックソート）
  qsort(polyindex, iPolyCount, sizeof(CPolyIndex), CmpZ);

  for (int i = 0; i < iPolyCount; i++) {
    int j = polyindex[i].no;

    // エッジ表示
    if (bUseEdge && polybuf[j].edge) {
      DrawPolygonEdge(polybuf[j].pos, polybuf[j].edge);
    }

    // アニメポリゴン表示
    if (bWireView) {
      DrawAnimePolygon(polybuf[j].pos, polybuf[j].kind, polybuf[j].color);
    }
  }
}


// ポリゴンのエッジを表示

void CBodyView::DrawPolygonEdge(POINT* p, int iEdgeFlag)
{
  int mask = 1;
  // iEdgeFlagの下４ビットをチェック
  for (int k = 0; k < 4; k++) {
    if (iEdgeFlag & mask) {
      int m = (k + 1) % 4;
      int n = (k + 2) % 4;
      XDrawLine(d, mbm, bpen, p[m].x, p[m].y, p[n].x, p[n].y);
    }
    mask <<= 1;
  }
}

// 頂点明度からブラシインデックス値を決定
// DrawAnimePolygon()で使用

inline int cVal(double c)
{
  if (c < T1)
    return 0;
  else if (c > T3)
    return 3;
  else if (c > T2)
    return 2;
  else
    return 1;
}

// 頂点位置を線形補完
// DrawAnimePolygon()で使用

inline void inP(POINT& p0, POINT& p1, POINT& p2, double c0, double c1, double c2)
{
  double t = (c2 - c0) / (c1 - c0);
  p2.x = p0.x + (int)((p1.x - p0.x) * t);
  p2.y = p0.y + (int)((p1.y - p0.y) * t);
}


// アニメ調ポリゴン塗りつぶし

void CBodyView::DrawAnimePolygon(POINT* p, int kind, double* c)
{
  static POINT pos[4][8];  // 色境界位置バッファ
  static int no[4][8];     // 各境界での色No.
  int i, j, k;

  // 各頂点の色インデックスを求める
  for (i = 0; i < 4; i++) {
    no[i][0] = cVal(c[i]);
  }

  if (bFlatShade || no[0][0] == no[1][0] && no[1][0] == no[2][0] && no[2][0] == no[3][0]) {
    // 全頂点同じ色なので、単色で塗りつぶす
    XFillPolygon(d, mbm, brush[kind][no[0][0]], p, 4, Nonconvex, CoordModeOrigin);
  }
  else {

    // 色バッファの４隅を各頂点の色で初期化
    for (i = 0; i < 4; i++) {
      pos[i][0] = p[i];
    }

    // 各エッジ毎に、色境界を含むかチェック
    for (i = 0; i < 4; i++) {
      int count = 1;
      int s = no[i][0];
      int e = no[(i + 1) % 4][0];
      if (s > e) {  // 明るくなる境界線を含む
        if (s > 2) {
          inP(p[(i + 1) % 4], p[i], pos[i][count], c[(i + 1) % 4], c[i], T3);
          no[i][count] = 2;
          no[i][count + 1] = 3;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
        if (s >= 2 && e <= 1) {
          inP(p[(i + 1) % 4], p[i], pos[i][count], c[(i + 1) % 4], c[i], T2);
          no[i][count] = 1;
          no[i][count + 1] = 2;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
        if (e == 0) {
          inP(p[(i + 1) % 4], p[i], pos[i][count], c[(i + 1) % 4], c[i], T1);
          no[i][count] = 0;
          no[i][count + 1] = 1;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
      }
      else if (s < e) {  // 暗くなる境界線を含む
        if (s == 0) {
          inP(p[i], p[(i + 1) % 4], pos[i][count], c[i], c[(i + 1) % 4], T1);
          no[i][count] = 0;
          no[i][count + 1] = 1;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
        if (e >= 2 && s <= 1) {
          inP(p[i], p[(i + 1) % 4], pos[i][count], c[i], c[(i + 1) % 4], T2);
          no[i][count] = 1;
          no[i][count + 1] = 2;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
        if (e > 2) {
          inP(p[i], p[(i + 1) % 4], pos[i][count], c[i], c[(i + 1) % 4], T3);
          no[i][count] = 2;
          no[i][count + 1] = 3;
          pos[i][count + 1] = pos[i][count];
          count += 2;
        }
      }

      // 用いなかった色バッファをクリア
      for (j = count; j < 8; j++) {
        no[i][j] = -1;
      }
    }

    // 各色毎にバッファを調べて、ポリゴン表示
    static POINT polybuf[32];
    for (i = 0; i < 4; i++) {
      int count = 0;
      for (j = 0; j < 4; j++) {
        for (k = 0; k < 8; k++) {
          if (no[j][k] == i) {
            polybuf[count] = pos[j][k];
            count++;
          }
        }
      }
      if (count > 2) {
        XFillPolygon(d, mbm, brush[kind][i], polybuf, count, Nonconvex, CoordModeOrigin);
      }
    }
  }
}


// アニメ調でポリゴン表示

void CBodyView::DrawPolygon(POINT* pos, int kind, double* color, double zval, int iEdgeFlag)
{
  // ポリゴンバッファにストック
  polybuf[iPolyCount].pos[0] = pos[0];
  polybuf[iPolyCount].pos[1] = pos[1];
  polybuf[iPolyCount].pos[2] = pos[2];
  polybuf[iPolyCount].pos[3] = pos[3];
  polybuf[iPolyCount].color[0] = color[0];
  polybuf[iPolyCount].color[1] = color[1];
  polybuf[iPolyCount].color[2] = color[2];
  polybuf[iPolyCount].color[3] = color[3];
  polybuf[iPolyCount].kind = kind;
  if (bEdgeView)
    polybuf[iPolyCount].edge = 15;
  else
    polybuf[iPolyCount].edge = iEdgeFlag;
  polyindex[iPolyCount].z = zval;
  polyindex[iPolyCount].no = iPolyCount;
  iPolyCount++;
  if (iPolyCount > cMaxPolyBuffer) {
    fprintf(stderr, "polybuf over flow\n");
    exit(1);
  }
}

// 塗りつぶされた矩形を表示

void CBodyView::FillRectangle(int iXpos, int iYpos, int iWidth, int iHeight)
{
  XFillRectangle(d, mbm, rpen, iXpos, iYpos, iWidth, iHeight);
}

// X-Windowイベントチェック

void CBodyView::CheckEvent()
{
  XEvent e;
  KeySym key;
  char s[10];
  int checkf;

  do {
    checkf = 0;

    // キーダウン
    *s = '\0';
    if (XCheckMaskEvent(d, KeyReleaseMask, &e)) {
      if (XLookupString((XKeyEvent*)&e, s, 1, &key, 0) == 1) {
        KeyState[*s] = false;
        checkf = 1;
      }
    }

    // キーアップ
    *s = '\0';
    if (XCheckMaskEvent(d, KeyPressMask, &e)) {
      if (XLookupString((XKeyEvent*)&e, s, 1, &key, 0) == 1) {
        KeyState[*s] = true;
        checkf = 1;
      }
    }

    // マウスボタンダウン
    if (XCheckMaskEvent(d, ButtonPressMask, &e)) {
      OnLButtonDown(e.xmotion.x, e.xbutton.y);
      checkf = 1;
    }

    // マウスボタンアップ
    if (XCheckMaskEvent(d, ButtonReleaseMask, &e)) {
      OnLButtonUp(e.xbutton.x, e.xbutton.y);
      checkf = 1;
    }

    // マウスドラッグ
    if (XCheckMaskEvent(d, Button1MotionMask, &e)) {
      OnMouseMove(e.xbutton.x, e.xbutton.y);
      checkf = 1;
    }
  } while (checkf);
}

// 各オブジェクト移動処理

void CBodyView::Idle() 
{
  CheckEvent();

  if (KeyState['!'])
    exit(0);

  if (KeyState['j'])
    angle.z -= 0.1;
  if (KeyState['l'])
    angle.z += 0.1;
  if (KeyState['i'])
    angle.x -= 0.1;
  if (KeyState['k'])
    angle.x += 0.1;
  if (KeyState['a'])
    vp.x -= 0.1;
  if (KeyState['d'])
    vp.x += 0.1;
  if (KeyState['q'])
    vp.y -= 0.1;
  if (KeyState['e'])
    vp.y += 0.1;
  if (KeyState['w'])
    vp.z -= 0.1;
  if (KeyState['s'])
    vp.z += 0.1;
  if (KeyState['u'])
    camera_v.y -= 1;
  if (KeyState['o'])
    camera_v.y += 1;

  CVector3 dp(0, 0, 0.5);
  if (KeyState[' ']) {
    body[iBodyNo].Move(vp * 2); // 全制御点移動
  }
  else {
    // 選択されている制御点移動
    for (int i = 0; i < cPosMax; i++)
      if (bPointSelect[i]) {
        CVector3 pos = body[iBodyNo].Pos(i);
        pos.move(vp, 10);
        body[iBodyNo].SetPos(i, pos);
      }
  }

  // 制御点移動速度を減衰
  vp.move(vp, -0.5);

  // カメラ移動
  camera_pos.move(camera_v, 1);
  camera_v.move(camera_v, -0.5);

  if (camera_pos.y > -10) {
    camera_pos.y = -10;
    camera_v.y = 0;
  }

  // 各ボディの制御点間制約を処理
  for (int i = 0; i < 2; i++) {
    if (bUseBody[i]) {
      body[i].PosCalc();
    }
  }
}

// ビュー初期化

void CBodyView::Init(int argc, char* argv[])
{
  int i, j;
  Colormap cmap;
  XColor c0, c1;

  d = XOpenDisplay (NULL);
  cmap = DefaultColormap(d, 0);
  black = BlackPixel(d, DefaultScreen(d));
  white = WhitePixel(d, DefaultScreen(d));
  w = XCreateSimpleWindow (d, RootWindow (d, 0), 200, 200, 400, 400, 2, white, black);
  XSetStandardProperties(d, w, "AnimeBody", "AnimeBody", NULL, argv, argc, NULL);
  XMapWindow (d, w);
  mbm = XCreatePixmap(d, w, 400, 400, DefaultDepth(d, 0));

  XSelectInput (d, w, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | Button1MotionMask);

  gc = XCreateGC (d, w, 0, 0);
  XSetForeground(d, gc, white);
  XSetBackground(d, gc, black);
  XSetLineAttributes(d, gc, 0, LineSolid, CapRound, JoinRound);
  XSetGraphicsExposures(d, gc, False);
  XSetFillStyle(d, gc, FillOpaqueStippled);
  XSetFillRule(d, gc, WindingRule);

  // 各ブラシとペンを初期化

  for (j = 0; j < 5; j++) {
    for (i = 0; i < 4; i++) {
      brush[j][i] = XCreateGC (d, w, 0, 0);
      XSetBackground(d, brush[j][i], white);
      XSetLineAttributes(d, brush[j][i], 0, LineSolid, CapRound, JoinRound);
      XSetFillStyle(d, brush[j][i], FillOpaqueStippled);
      XSetFillRule(d, brush[j][i], WindingRule);
    }
  }

  // 肌
  XAllocNamedColor(d, cmap, "#e1a0a0", &c1, &c0);
  XSetForeground(d, brush[0][0], c1.pixel);
  XAllocNamedColor(d, cmap, "#ebb4b4", &c1, &c0);
  XSetForeground(d, brush[0][1], c1.pixel);
  XAllocNamedColor(d, cmap, "#f5c8c8", &c1, &c0);
  XSetForeground(d, brush[0][2], c1.pixel);
  XAllocNamedColor(d, cmap, "#ffe6e6", &c1, &c0);
  XSetForeground(d, brush[0][3], c1.pixel);

  // 髪
  XAllocNamedColor(d, cmap, "#3c3c3c", &c1, &c0);
  XSetForeground(d, brush[1][0], c1.pixel);
  XAllocNamedColor(d, cmap, "#505050", &c1, &c0);
  XSetForeground(d, brush[1][1], c1.pixel);
  XAllocNamedColor(d, cmap, "#646464", &c1, &c0);
  XSetForeground(d, brush[1][2], c1.pixel);
  XAllocNamedColor(d, cmap, "#828282", &c1, &c0);
  XSetForeground(d, brush[1][3], c1.pixel);

  // 目
  XAllocNamedColor(d, cmap, "#505050", &c1, &c0);
  XSetForeground(d, brush[2][0], c1.pixel);
  XAllocNamedColor(d, cmap, "#646464", &c1, &c0);
  XSetForeground(d, brush[2][1], c1.pixel);
  XAllocNamedColor(d, cmap, "#000000", &c1, &c0);
  XSetForeground(d, brush[2][2], c1.pixel);
  XAllocNamedColor(d, cmap, "#e2e2e2", &c1, &c0);
  XSetForeground(d, brush[2][3], c1.pixel);

  // 服
  XAllocNamedColor(d, cmap, "#c8c8c8", &c1, &c0);
  XSetForeground(d, brush[3][0], c1.pixel);
  XAllocNamedColor(d, cmap, "#dcdcdc", &c1, &c0);
  XSetForeground(d, brush[3][1], c1.pixel);
  XAllocNamedColor(d, cmap, "#f0f0f0", &c1, &c0);
  XSetForeground(d, brush[3][2], c1.pixel);
  XAllocNamedColor(d, cmap, "#ffffff", &c1, &c0);
  XSetForeground(d, brush[3][3], c1.pixel);

  // まぶた
  XAllocNamedColor(d, cmap, "#000000", &c1, &c0);
  XSetForeground(d, brush[4][0], c1.pixel);
  XAllocNamedColor(d, cmap, "#0a0a0a", &c1, &c0);
  XSetForeground(d, brush[4][1], c1.pixel);
  XAllocNamedColor(d, cmap, "#141414", &c1, &c0);
  XSetForeground(d, brush[4][2], c1.pixel);
  XAllocNamedColor(d, cmap, "#e1a0a0", &c1, &c0);
  XSetForeground(d, brush[4][3], c1.pixel);

  // エッジ

  bpen = XCreateGC (d, w, 0, 0);
  XSetForeground(d, bpen, black);
  XSetBackground(d, bpen, white);
  XSetLineAttributes(d, bpen, 2, LineSolid, CapRound, JoinRound);

  spen = XCreateGC (d, w, 0, 0);
  XSetForeground(d, spen, black);
  XSetBackground(d, spen, white);
  XSetLineAttributes(d, spen, 0, LineSolid, CapRound, JoinRound);

  font = XLoadFont(d, "variable");
  XSetFont(d, spen, font);

  rpen = XCreateGC (d, w, 0, 0);
  XAllocNamedColor(d, cmap, "#ff0000", &c1, &c0);
  XSetForeground(d, rpen, c1.pixel);
  XSetBackground(d, rpen, white);
}

// マウス左ボタン処理

void CBodyView::OnLButtonDown(int x, int y) 
{
  int i, j;

  bLeftButton = true;
  sx = x;
  sy = y;
  dsx = x;
  dsy = y;

  int iPointNo;

  // 単独各制御点選択
  bPointSelectFlag = false;
  for (j = 0; j < 2; j++) {
    for (i = 0; i < cPosMax; i++) {
      if (bUseBody[j] && body[j].CalcScreenDis(this, i, sx, sy) < 20) {
        iPointNo = i;
        iBodyNo = j;
        bPointSelectFlag = true;
      }
    }
  }

  // 選択されている制御点数カウント
  int iCount = 0;
  for (i = 0; i < cPosMax; i++)
    if (bPointSelect[i])
      iCount++;

  // 制御点が選択されていなければ、全選択解除
  if (!bPointSelectFlag || !bPointSelect[iPointNo])
    for (i = 0; i < cPosMax; i++)
      bPointSelect[i] = false;

  // 単独制御点が選択されていれば、選択セット
  if (bPointSelectFlag) {
    bPointSelect[iPointNo] = true;
  }
}

// マウス右ボタン処理

void CBodyView::OnLButtonUp(int x, int y)
{
  bLeftButton = false;
  int dm;

  if (bPointSelectFlag == false) {      // ドラッグ後に離されたか？

    int x1, x2, y1, y2;
    x1 = min(sx, dsx);
    x2 = max(sx, dsx);
    y1 = min(sy, dsy);
    y2 = max(sy, dsy);

    // 範囲内の制御点選択
    for (int i = 0; i < cPosMax; i++) {
      if (body[iBodyNo].InScreenArea(this, i, x1, y1, x2, y2)) {
        bPointSelect[i] = true;
      }
      else
        bPointSelect[i] = false;
    }
  }

  bPointSelectFlag = false;
}

// マウスドラッグ処理

void CBodyView::OnMouseMove(int x, int y)
{
  if (bLeftButton) {
    double dx = (x - sx) / 20.0;
    double dy = (y - sy) / 20.0;

    double r = sqrt(dx * dx + dy * dy);
    if (r > 0.3) {
    }

    // スペースキーが押されていれば、全制御点移動
    if (KeyState[' ']) {
      CVector3 p;
      p.x = dx * vNorm[0].x + dy * vNorm[2].x;
      p.y = dx * vNorm[0].y + dy * vNorm[2].y;
      p.z = dx * vNorm[0].z + dy * vNorm[2].z;
      body[iBodyNo].Move(p);
    }
    else {
      // 選択制御点移動
      for (int i = 0; i < cPosMax; i++) {
        if (bPointSelect[i]) {
          CVector3 p;
          p = body[iBodyNo].Pos(i);

          p.x += dx * vNorm[0].x + dy * vNorm[2].x;
          p.y += dx * vNorm[0].y + dy * vNorm[2].y;
          p.z += dx * vNorm[0].z + dy * vNorm[2].z;

          body[iBodyNo].SetPos(i, p);
        }
      }
    }
    sx = x;
    sy = y;
  }
}


