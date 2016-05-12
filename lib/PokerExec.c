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

#include "Poker.h"

//====================================================================
//  大域宣言
//====================================================================

extern int Trial;                        // トライ回数
extern char Deck[FILE_NAME_LEN];         // 山札ファイル
extern int Disp_Mode;                    // 表示モード
extern int Hand_Value[10];
extern double Take_Weight[10];

double poker_exec(FILE *fp, int point[]);

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
  strcpy(Deck, argv[2]);    // 山札ファイル
  Disp_Mode = atoi(argv[3]);     // 表示モード

  //----  トライ回数の確認
  if ( Trial > GNUM ) {
    puts("ERROR : トライ回数が多すぎます。");
    exit(EXIT_FAILURE);
  }

  //----  山札ファイルのオープンとエラー処理
  fp = fopen(Deck, "r");
  if ( fp == NULL ) {
    puts("ERROR : 山札ファイルの指定が不正です。");
    exit(EXIT_FAILURE);
  }

  //----  乱数の初期化
  srand(time(NULL));

  //----  トライ回数だけのゲームの反復
  total = 0.0;
  for ( k = 0; k < Trial-1; k++ ) {
    total += poker_exec(fp, point[k]);
    if ( Disp_Mode ) { puts("===TAKE==="); }
  }
  total += poker_exec(fp, point[k]);

  //----  結果表の出力
  puts("");
  result_show(point);

  //----  終了
  return 0;
}

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

  return try_p;
}

