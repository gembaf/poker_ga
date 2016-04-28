//====================================================================
//  事前処理
//====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "Poker.h"


//====================================================================
//  本体関数
//====================================================================

int main(int argc, char *argv[]) {
  //----  宣言
  int stock[CNUM];       // 各回得点
  int tril;              // トライ回数
  int i, j;

  //----  コマンドライン引数の確認
  if ( argc < 2 ) {
    puts("トライ回数を指定してください。");
    exit(EXIT_FAILURE);
  }
  tril = atoi(argv[1]);

  //----  初期化
  srand(time(NULL));

  //----  山札出力
  for ( i = 0; i < tril; i++ ) {
    //----  山札ランダム生成
    qrand(stock, CNUM);
    for ( j = 0; j < CNUM-1; j++ ) { printf("%d ", stock[j]); }
    printf("%d\n", stock[j]);
  }

  return 0;
}

