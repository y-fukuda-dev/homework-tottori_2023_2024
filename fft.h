/*********************************************************************
 *                                                                   *
 *                 DFT-IDFT filter utility function                  *
 *         1) fft                                                    *
 *         2) filter_fft                                             *
 *         3) power_sum                                              *
 *         4) markov_power4                                          *
 *********************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//**********************************************************
// 関数名: double power_sum( l_band, h_band, fft_ss )
// 機　能: 周波数帯域（l_band〜h_band）における
// 　　　: スペクトル成分量の総和
// 入　力: 周波数帯域（低，高），求めたいスペクトル系列
// 出　力: スペクトル成分量の総和
//**********************************************************
double power_sum( short n, double xr[], double xi[], double f1, double f2, double st )
{
  short i, l1, l2;
  double fn, cf, pp;
  
  fn = 1.0F/( 2.0F*st ); // ナイキスト周波数
  l1 = (int)( f1*(double)(n/2)/fn );
  l2 = (int)( f2*(double)(n/2)/fn );
  cf = st/(double)n;
  pp = 0.0F;
  for( i = l1; i <= l2; i++ ){
    pp += ( xr[i]*xr[i] + xi[i]*xi[i] )*cf;
  }
  pp /= (st*(double)n);
  return( pp );
}

//**********************************************************
// 関数名: double markov_power4( l_band, h_band, fft_ss )
// 機　能: 周波数帯域（l_band〜h_band）における
// 　　　: スペクトル成分量の総和に基づくマルコフ正弦波脳波
// 　　　: モデルを介した振幅値の導出
// 入　力: 周波数帯域（低，高），求めたいスペクトル系列
// 出　力: マルコフモデルに基づいた振幅値
//**********************************************************
double markov_power4( short n, double xr[], double xi[], double f1, double f2, double st )
{
  short i, l1, l2;
  double fn, cf, pp;
  
  fn = 1.0F/( 2.0F*st ); // ナイキスト周波数
  l1 = (int)( f1*(double)(n/2)/fn );
  l2 = (int)( f2*(double)(n/2)/fn );
  cf = st/(double)n;
  pp = 0.0F;
  for( i = l1; i <= l2; i++ ){
    pp += ( xr[i]*xr[i] + xi[i]*xi[i] )*cf;
  }
  pp /= (st*(double)n);
  if( pp == 0.0F ){
    return( 0.0F );
  }else{
    return( 4.0F*(double)sqrt( pp ) );
  }
}

//**********************************************************
// 関数名: void filter_fft( int n, double xr[], double xi[],
// 　　　: double f1, double f2, double st )
// 機　能: 指定した周波数f1からf2[Hz]間のデータを削除
// 入　力: データ個数，FFT実部配列，FFT虚部配列，周波数(低域)，
// 　　　: 周波数(高域)，サンプリング時間間隔[s]
// 出　力: なし
//**********************************************************
void filter_fft( short n, double xr[], double xi[], double f1, double f2, double st )
{
  short i, l1, l2;
  double fn;
  
  fn = 1.0F/( 2.0F*st ); // ナイキスト周波数
  l1 = (int)( f1*(double)(n/2)/fn );
  l2 = (int)( f2*(double)(n/2)/fn );
  for( i = l1; i <= l2; i++ ){
    xr[i] = 0.0F; xi[i] = 0.0F; xr[n-i] = 0.0F; xi[n-i] = 0.0F;
  }
  return;
}

//**********************************************************
// 関数名: static void make_sintbl( int n, double sintbl[] )
//**********************************************************
static void make_sintbl( short n, double  sintbl[] )
{
  short i, n2, n4, n8;
  double  c, s, dc, ds, t;
  
  n2 = n / 2;
  n4 = n / 4;
  n8 = n / 8;
  t = (double)sin( 3.14159F / n );
  dc = 2.0F * t * t;
  ds = (double)sqrt( dc * (2.0F-dc) );
  t = 2.0F * dc;
  c = sintbl[n4] = 1.0F;
  s = sintbl[0] = .0F;
  for( i = 1; i < n8; i++ ){
    c -= dc;
    dc += t * c;
    s += ds;
    ds -= t * s;
    sintbl[i] = s;
    sintbl[n4-i] = c;
  }
  if( n8 != 0 )
    sintbl[n8] = (double)sqrt(0.5);
  for( i = 0; i < n4; i++ )
    sintbl[n2-i] = sintbl[i];
  for( i = 0; i < n2 + n4; i++ )
    sintbl[i+n2] = -sintbl[i];
}

//**********************************************************
// 関数名: static void make_bitrev( int n, int sintbl[] )
//**********************************************************
static void make_bitrev( short n, short bitrev[] )
{
  int i, j, k, n2;
  
  n2 = n / 2;
  i = j = 0;
  for( ; ; ){
    bitrev[i] = j;
    if( ++i >= n )
      break;
    k = n2;
    while( k <= j ){
      j -= k; k /= 2;
    }
    j += k;
  }
}

//**********************************************************
// 関数名: int fft( int n, double  xr[], double  xi[] )
// 機　能: FFTの計算および逆FFTの計算
// 入　力: n    Number of Data( positive/DFT,  negative/IDFT )
// 　　　: xr   Data array( real part )
// 　　　: xi   Data array( immaginary part )
// 出　力: 0 (正常終了)
//**********************************************************
int fft( short n, double  xr[], double  xi[] )
{
  static short  last_n = 0;     // 前回呼出時のn
  static short  *bitrev = NULL; // ビット反転表
  static double  *sintbl = NULL; // 三角関数表
  short i, j, k, ik, h, d, k2, n4, inverse;
  double  t, s, c, dx, dy;
  
  // 準備
  if( n < 0 ){
    n = -n; inverse = 1; // 逆変換
  }else{
    inverse = 0;
  }
  n4 = n / 4;
  if( n != last_n || n == 0 ){
    last_n = n;
    if( sintbl != NULL )
      free(sintbl);
    if( bitrev != NULL )
      free( bitrev );
    if( n == 0 )
      return 0; // 記憶領域を解放
    sintbl = (double *)malloc( (n+n4) * sizeof(double ) );
    bitrev = (short *)malloc( n * sizeof(short) );
    if( sintbl == NULL || bitrev == NULL ){
      fprintf( stderr, "記憶領域不足\n" );  return 1;
    }
    make_sintbl( n, sintbl );
    make_bitrev( n, bitrev );
  }
  for( i = 0; i < n; i++ ){ // ビット反転
    j = bitrev[i];
    if( i < j ){
      t = xr[i]; xr[i] = xr[j]; xr[j] = t;
      t = xi[i]; xi[i] = xi[j]; xi[j] = t;
    }
  }
  
  for( k = 1; k < n; k = k2 ){ // 変換
    h = 0;  k2 = k + k;  d = n / k2;
    for( j = 0; j < k; j++ ){
      c = sintbl[h+n4];
      if( inverse ){
	s = -sintbl[h];
      }else{
	s =  sintbl[h];
      }
      for( i = j; i < n; i += k2 ){
	ik = i + k;
	dx = s * xi[ik] + c * xr[ik];
	dy = c * xi[ik] - s * xr[ik];
	xr[ik] = xr[i] - dx;  xr[i] += dx;
	xi[ik] = xi[i] - dy;  xi[i] += dy;
      }
      h += d;
    }
  }
  if( inverse ){
    for( i = 0; i < n; i++ ){
      xr[i] /= (double)n; xi[i] /= (double)n;
    }
  }
  return 0; // 正常終了
}

//**********************************************************
// 関数名: void FFT_Window( int N, double  x[] )
// 機　能: FFT前の窓処理
// 入　力: n    Number of Data( positive/DFT,  negative/IDFT )
// 　　　: x    Data array( time series )
// 出　力: 0
//**********************************************************
void FFT_Window(int N, double x[])
{
  int n;
  for(n=0; n<N; n++) {
    // ハミング窓
    x[n] = x[n]*(0.54 - 0.46*cos(2*M_PI*n/N));

    // ハニング窓
    //x[n] = x[n]*(0.50 - 0.50*cos(2*M_PI*n/N));

    // ブラックマン窓
    //x[n] = x[n]*(0.42 - 0.5*cos(2*M_PI*n/N) + 0.08*cos(4*M_PI*n/N));
  }
}
