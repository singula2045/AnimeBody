#if !defined (VECTOR3_H)
#define VECTOR3_H

/////////////////////////////////////////////////////////////////////////////
// CVector3 ３次ベクトルクラス

class CVector3
{
  // アトリビュート
public:

  double x, y, z;

  // オペレーション
public:

  // コンストラクタ

  // 配列使用時速度低下を防ぐための、空コンストラクタ
  CVector3() {
  };

  CVector3(double ax, double ay = 0.0, double az = 0.0) {
    x = ax;
    y = ay;
    z = az;
  };

  // 代入
  void set(double ax = 0.0, double ay = 0.0, double az = 0.0) {
    x = ax;
    y = ay;
    z = az;
  };

  // 和
  CVector3 operator+(CVector3& av) {
    return CVector3(x + av.x, y + av.y, z + av.z);
  };
  
  // 加算
  CVector3& operator+=(CVector3& av) {
    x += av.x;
    y += av.y;
    z += av.z; 
    return *this;
  };
  
  // 差
  CVector3 operator-(CVector3& av) {
    return CVector3(x - av.x, y - av.y, z - av.z);
  };

  // 減算
  CVector3& operator-=(CVector3& av) {
    x -= av.x;
    y -= av.y;
    z -= av.z; 
    return *this;
  };
  
  // 定数倍
  CVector3 operator*(double a) {
    return CVector3(x * a, y * a, z * a);
  };

  // 積算
  CVector3& operator*=(double a) {
    x *= a;
    y *= a;
    z *= a;
    return *this;
  };

  // 定数で割る
  CVector3 operator/(double a) {
    return CVector3(x / a, y / a, z / a);
  };

  // 定数で割る
  CVector3& operator/=(double a) {
    x /= a;
    y /= a;
    z /= a;
    return *this;
  };

  // 積和
  CVector3& move(CVector3& av, double at) {
    x += av.x * at; 
    y += av.y * at; 
    z += av.z * at; 
    return *this;
  };

  // ベクトル長
  double abs() {
    return sqrt(abs2());
  };

  // ２乗ベクトル長
  double abs2() {
    return x * x + y * y + z * z;
  };

  // 内積
  double inprod(CVector3& av) {
    return x * av.x + y * av.y + z * av.z;
  };

  // 外積
  CVector3 outprod(CVector3& av) {
    double ax, ay, az;
    ax = (y * av.z - z * av.y);
    ay = (z * av.x - x * av.z);
    az = (x * av.y - y * av.x);
    return CVector3(ax, ay, az);
  };

  // 単位ベクトルを求める
  CVector3 norm() {
    double v = abs();
    return CVector3(x / v, y / v, z / v);
  };

  // 単位ベクトル化
  CVector3& normalize() {
    double v = abs();
    x /= v;
    y /= v;
    z /= v;
    return *this;
  };
};

/////////////////////////////////////////////////////////////////////////////

#endif
