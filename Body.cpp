// Body.cpp : メインインプリメンテーション ファイル
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include "Vector3.h"
#include "Body.h"
#include "BodyModel.h"
#include "BodyView.h"

// μ秒単位でウェイト
void pwait(long t)
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = t;
  select(0, 0, 0, 0, &tv);
}

void main(int argc, char* argv[])
{
  signal(SIGFPE, SIG_IGN);  // 浮動小数点演算例外を無視

  CBodyView view;
  view.Init(argc, argv);
  
  for (;;) {
    view.Idle();
    view.Draw();
    pwait(1);  // 明示的にウェイト入れたほうが早い場合が多い
  }
}
