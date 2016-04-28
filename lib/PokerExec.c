//====================================================================
//  事前処理
//====================================================================

//--------------------------------------------------------------------
//  ヘッダファイル
//--------------------------------------------------------------------

#include <stdio.h>     // 標準優出力
#include <stdlib.h>    // 標準ライブラリ
#include <string.h>    // 文字列処理
#include <time.h>      // 時間関数
#include <math.h>      // 数学関数
#include <dirent.h>    // ディレクトリ操作

#include "Poker.h"     // 課題ライブラリのヘッダファイル(相対パス指定)

//====================================================================
//  大域宣言
//====================================================================

int Trial;                        // トライ回数
char Stck[FILE_NAME_LEN];         // 山札ファイル
int Disp_Mode;                    // 表示モード
int Hand_Value[10] = {P0, P1, P2, P3, P4, P5, P6, P7, P8, P9};
double Take_Weight[10] = {1.0, 1.0, 1.5, 1.5, 2.0, 2.0};

//====================================================================
//  本体関数
//====================================================================

int main(int argc, char *argv[]) 
{
  //----  局所宣言
  int point[GNUM][TAKE];    // 各回得点
  FILE *fp;                 // 山札へのファイルポインタ
  double total;             // 総合得点
  double ave;               // 平均得点
  int k;                    // 反復変数

  //----  コマンド引数の確認
  if ( argc < 4 ) {
    puts("ERROR : コマンド引数が不正です。");
    puts("実行パラメタ : トライ回数 山札ファイル 表示モード");
    exit(EXIT_FAILURE);
  }

  //----  コマンド引数の格納
  Trial = atoi(argv[1]);    // トライ回数
  strcpy(Stck, argv[2]);    // 山札ファイル
  Disp_Mode = atoi(argv[3]);     // 表示モード

  //----  トライ回数の確認
  if ( Trial > GNUM ) {
    puts("ERROR : トライ回数が多すぎます。");
    exit(EXIT_FAILURE);
  }

  //----  山札ファイルのオープンとエラー処理
  fp = fopen(Stck, "r");
  if ( fp == NULL ) {
    puts("ERROR : 山札ファイルの指定が不正です。");
    exit(EXIT_FAILURE);
  }

  //----  乱数の初期化
  srand(time(NULL));

  //----  トライ回数だけのゲームの反復
  total = 0.0;
  for ( k = 0; k < Trial; k++ ) {
    total += poker_exec(fp, point[k]);
  }

  //----  結果表の出力
  result_show(point);

  //----  平均点の出力
  ave = (double) total / Trial;
  printf("\n");
  printf("平均得点 :  %lf\n", ave);

  //----  終了
  return 0;
}


//====================================================================
//  ポーカーの進行
//====================================================================

//--------------------------------------------------------------------
//  各トライの進行
//--------------------------------------------------------------------

double poker_exec(FILE *fp, int point[]) 
{
  //----  局所変数
  int stock[CNUM];            // 山札
  int used[CNUM];             // 捨札配列
  int us;                     // 捨札数
  double try_p;               // トライ得点
  int tk;                     // テイク数
  
  //----  トライの準備
  card_stock(stock, fp);      // 山札の生成
  
  //----  トライの最中(テイクを繰り返す)
  us = 0;
  try_p = 0;
  for ( tk = 0; tk < TAKE; tk++ ) {
    point[tk] = poker_take(stock, tk, used, &us);
    try_p += point[tk] * Take_Weight[tk];
  }

  // トライ得点の表示
  if ( Disp_Mode ) {
    printf("トライ得点 : %4f\n", try_p);
  }

  return try_p;
}

//--------------------------------------------------------------------
//  テイクの実行
//--------------------------------------------------------------------

int poker_take(const int stock[], int tk, int used[], int *us) 
{
  //----  局所変数
  int field[FNUM];            // 場札
  int hand[HNUM];             // 手札
  int state[CHNG+1][HNUM];    // 局面(手札の列)
  int ope[CHNG];              // 打手札
  int the;                    // 打手
  int cg;                     // チェンジ回数
  int take_p;                 // テイク得点
  int k;                      // 反復変数

  //----  テイクの準備
  if ( *us +5 < CNUM ) {
    arr_copy(hand, &stock[*us], HNUM); // 手札の配布(5枚)
    arr_copy(state[0], hand, HNUM);    // 局面の記録
  } else {
    return 0;
  }

  //----  テイクの最中(チェンジの繰返し)
  for ( cg = 0; cg < CHNG; cg++ ) {
    //----  
    if ( *us < CNUM-5 ) {
      the = strategy(hand, field, cg, tk, used, *us);  // 戦略により捨札を決定
    } else {
      the = -1; break;
    }
    //----  
    ope[cg] = hand[the];                  // 打手札の記録
    if ( the < 0 ) { break; }             // 手札を交換せずにテイク終了
    field[cg] = ope[cg];                  // 捨札を場札に追加
    used[(*us)++] = ope[cg];              // 捨札を捨札配列に追加
    hand[the] = stock[HNUM-1+(*us)];      // 手札を交換
    arr_copy(state[cg+1], hand, HNUM);    // 局面の記録
  }

  //----  テイクの後処理
  take_p = poker_point(hand);                 // テイク得点

  for ( k = 0; k < HNUM; k++ ) {
    used[*us] = hand[k];                      // 手札を捨札配列に追加
    (*us)++;
  }

  //----  テイクの経過表示
  if ( Disp_Mode ) {
    take_show(state, ope, field, cg, take_p);
    printf("テイク素点 : %4d\n", take_p);
    printf("テイク得点 : %4f\n", take_p*Take_Weight[tk]);
  }

  return take_p;
}


//====================================================================
//  ゲームの表示
//====================================================================

//--------------------------------------------------------------------
//  テイクの表示
//--------------------------------------------------------------------

void take_show(int st[][HNUM], int ope[], int fd[], int cg, int tp) 
{
  int k;
  int p;
  for ( k = 0; k <= cg; k++ ) {
    //----  手札の表示
    printf("[%d] ", k); 
    card_show(st[k], HNUM); 
    //----  捨札の表示
    if ( k < cg ) { printf(" >%s", card_str(ope[k])); }
    printf("\n");
  }
}

//--------------------------------------------------------------------
//  結果表の表示
//--------------------------------------------------------------------

void result_show(int point[][TAKE]) 
{
  int i, j, k;
  int deg[POINT_NUM][TAKE];    // 
  int sum[POINT_NUM];          // 
  double scr[TAKE] = {0.0};    // 
  double total = 0.0;          // 

  //----  配列の初期化
  for( k = POINT_MIN; k <= POINT_MAX; k++ ) {
    sum[k] = 0;
    for( j = 0; j < TAKE; j++ ) {
      deg[k][j] = 0;
    }
  }
  
  //----  役ごとの集計
  for ( i = 0; i < Trial; i++ ) {
    for ( j = 0; j < TAKE; j++ ) {
      switch( point[i][j] ) {
      case P9: deg[9][j]++; break;
      case P8: deg[8][j]++; break;
      case P7: deg[7][j]++; break;
      case P6: deg[6][j]++; break;
      case P5: deg[5][j]++; break;
      case P4: deg[4][j]++; break;
      case P3: deg[3][j]++; break;
      case P2: deg[2][j]++; break;
      case P1: deg[1][j]++; break;
      case P0: deg[0][j]++; break;
      default : break;
      }
    }
  }

  //----  全テイクでの役の総数
  for( k = POINT_MIN; k <= POINT_MAX; k++ ) {
    for( j = 0; j < TAKE; j++ ) {
      sum[k] += deg[k][j];
      scr[j] += deg[k][j] * Hand_Value[k];
    }
  }
  puts("");

  //----  結果表のテイク番号の表示
  printf("        役名         ");
  for( i = 0; i < TAKE; i++ ) {
    printf("| take%d", i+1);
  }
  printf("|  合計  \n");
  //-----  結果表の区切線の表示
  printf("---------------------");
  for( i = 0; i <= TAKE; i++ ) {
    printf("+------");
  }
  printf("\n");

  for ( i = POINT_MIN; i <= POINT_MAX; i++ ) {
    //----  結果表の役名の表示
    switch( i ) {
      case 9: printf("ロイヤルストレート   "); break;
      case 8: printf("ストレートフラッシュ "); break;
      case 7: printf("フォーカード         "); break;
      case 6: printf("フルハウス           "); break;
      case 5: printf("フラッシュ           "); break;
      case 4: printf("ストレート           "); break;
      case 3: printf("スリーカード         "); break;
      case 2: printf("ツーペア             "); break;
      case 1: printf("ワンペア             "); break;
      case 0: printf("ノーペア             "); break;
      default : break;
    }
    for ( j = 0; j < TAKE; j++ ) {
      //----  結果表の役頻度の表示
      printf("|%6d", deg[i][j]);
      if( j == TAKE-1 ) {
        printf("|%6d", sum[i]);
      }
    }
    printf("\n");
  }

  //-----  結果表の区切線の表示
  printf("---------------------");
  for( i = 0; i <= TAKE; i++ ) {
    printf("+------");
  }
  printf("\n");

  //----  
  printf("単純素点             ");
  for ( j = 0; j < TAKE; j++ ) {
    printf("|%6.2f", scr[j] / Trial);
  }
  printf("|\n");

  //----  
  printf("傾斜得点             ");
  for ( j = 0; j < TAKE; j++ ) {
    printf("|%6.2f", scr[j] * Take_Weight[j]/Trial);
    total += scr[j] * Take_Weight[j];
  }
  printf("|%6.2f\n", total / Trial);
}


//====================================================================
//  ポーカーの役と得点
//====================================================================

//--------------------------------------------------------------------
//  ポーカーの得点
//--------------------------------------------------------------------

int poker_point(int hand[]) 
{
  //----  局所宣言
  int num[13] = {0};     // 数位ごとの個数を格納する配列(A23456789TJQK)
  int sut[4] = {0};      // 種類ごとの個数を格納する配列(SHDC)
  int p0, p1, p2, p3;    // 最終および系別の役の得点
  int k;    // 反復変数
  int t;    // 一時変数

  //----  数位および種類ごとの計数
  for ( k = 0; k < HNUM; k++ ) { t = hand[k] % 13; num[t]++; }    // 数位
  for ( k = 0; k < HNUM; k++ ) { t = hand[k] / 13; sut[t]++; }    // 種類

  //----  系別の役の判定
  p1 = poker_point_pair(num);            // ペア系
  p2 = poker_point_flash(sut);           // フラッシュ系
  p3 = poker_point_straight(num, p2);    // ストレート系(ロイヤル判定のための引数付)

  //----  最高点の役を採用
  p0 = p1;                       // ペア系の役の得点
  if ( p0 < p2 ) { p0 = p2; }    // フラッシュ系の役への変更
  if ( p0 < p3 ) { p0 = p3; }    // ストレート系の役への変更

  //----  確定した役の返却
  return p0;
}

//--------------------------------------------------------------------
//  ペア判定
//--------------------------------------------------------------------

int poker_point_pair(int num[]) 
{
  //----  局所宣言
  int c2 = 0, c3 = 0;
  int k;

  //----  フォーカード
  for ( k = 0; k < 13; k++ ) { if ( num[k] == 4 ) { return P7; } }

  //----  その他のペア要素の検出
  for ( k = 0; k < 13; k++ ) { 
    if ( num[k] == 3 ) { c3++; } 
    if ( num[k] == 2 ) { c2++; } 
  }

  //----  フルハウス
  if ( c3 == 1 && c2 == 1 ) { return P6; }
  //----  スリーカインズ
  if ( c3 == 1 ) { return P3; }
  //----  ツーペア
  if ( c2 == 2 ) { return P2; }
  //----  ワンペア
  if ( c2 == 1 ) { return P1; }
  //----  ノーペア
  return 0;
}

//--------------------------------------------------------------------
//  フラッシュ判定
//--------------------------------------------------------------------

int poker_point_flash(int sut[]) 
{
  int k;
  for ( k = 0; k < 4; k++ ) { if ( sut[k] == 5 ) { return P5; } }
  return P0;
}

//--------------------------------------------------------------------
//  ストレート判定
//--------------------------------------------------------------------

int poker_point_straight(int num[], int p) 
{
  int len = 0;
  int k;
  for ( k = 0; k < 13; k++ ) {
    if ( num[k] > 0 ) { 
      len++; 
      if ( len == 5 ) { break; }
    } else { len = 0; }
  }

  //----  ロイヤルストレート(TJQKA)
  if ( len == 4 && num[0] == 1 ) { len = 6; }
  //----  ロイヤルストレートフラッシュ
  if ( len == 6 ) { if ( p > 0 ) { return P9; } else { return P4; } }
  //----  ノーマルストレート
  if ( len == 5 ) { if ( p > 0 ) { return P8; } else { return P4; } }

  //----  
  return 0;
}

