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

#define DECK 10000
#define STEP (CNUM + TAKE)  // TAKE数分を余分に取っておく

#define POPSIZE 200
#define GEN 10000
#define PROB_C 85
#define PROB_M 5

//====================================================================
//  大域宣言
//====================================================================

typedef struct {
  int gene[STEP];
  double fit;
} Genome;

extern int Trial;                        // トライ回数
extern char Deck[FILE_NAME_LEN];         // 山札ファイル
extern int Disp_Mode;                    // 表示モード
extern int Hand_Value[10];
extern double Take_Weight[10];

extern int StepCounter;
int Step[STEP] = {0};
int UsedFlag[STEP] = {0};

void ga_exec(int deck[][CNUM]);
Genome ga_stock_exec(int stock[]);
double poker_exec(int stock[]);

void initialize(Genome parent[], int stock[]);
void set_fitness(Genome g[], int n, int stock[]);

void crossover(Genome parent[], Genome *c1, Genome *c2);
void crossover_uniform(Genome *g1, Genome *g2);
void crossover_double(Genome *g1, Genome *g2);

void mutation(Genome g[], int n);

Genome selection(Genome g[], int n);
Genome selection_roulette(Genome g[], int n);

void generation_change(Genome parent[], Genome child[]);
Genome get_elite(Genome g[], int n);

int uniq_rand(int base);
int myrand(void);

//====================================================================
//  本体関数
//====================================================================

int main(int argc, char *argv[])
{
  //----  局所宣言
  int point[GNUM][TAKE];    // 各回得点
  FILE *fp;                 // 山札へのファイルポインタ
  int k;
  int deck[DECK][CNUM];

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

  //----  山札作成
  for ( k = 0; k < Trial; k++ ) {
    int stock[CNUM];
    card_stock(stock, fp);      // 山札の生成
    arr_copy(deck[k], stock, CNUM);
  }

  //----  ファイルのクローズ
  fclose(fp);

  //----  GAの実行
  ga_exec(deck);

  //----  終了
  return 0;
}

//----------------------------------------------------------
//  GAの実行
//----------------------------------------------------------

void ga_exec(int deck[][CNUM])
{
  int k1, k2;
  double sum = 0;

  // fprintf(stderr, "{\n");
  for ( k1 = 0; k1 < DECK; k1 += 100 ) {
    Genome elite;
    elite = ga_stock_exec(deck[k1]);

    sum *= (double)k1 / (k1+1);
    sum += elite.fit / (k1+1);
    // printf("%5d : %8.3f %8.3f\n", k1, elite.fit, sum);
    // for ( k2 = 0; UsedFlag[k2] ; k2++ ) { fprintf(stderr, "%d,", Step[k2]); }
    // fprintf(stderr, "\n");
  }
  // fprintf(stderr, "}\n");
  // printf("%5d : %8.3f %8.3f\n", k1, elite.fit, sum);
}

//----------------------------------------------------------
//  各トライの進行
//----------------------------------------------------------

Genome ga_stock_exec(int stock[])
{
  Genome elite;
  Genome *parent;
  int gen = 0;
  int k;

  parent = (Genome*)malloc(sizeof(Genome)*POPSIZE);
  initialize(parent, stock);

  for ( gen = 0; gen < GEN; gen++ ) {
    Genome *child = (Genome*)malloc(sizeof(Genome)*POPSIZE);

    //----  交叉
    for ( k = 0; k < POPSIZE; k += 2 ) {
      crossover(parent, &child[k], &child[k+1]);
    }

    //----  突然変異
    mutation(child, POPSIZE);

    //----  適合度の計算
    set_fitness(child, POPSIZE, stock);

    //----  世代交代
    generation_change(parent, child);
    set_fitness(parent, POPSIZE, stock);
    free(child);

    //----  過程の出力
    if ( gen%100 == 0 ) {
      elite = get_elite(parent, POPSIZE);
      printf("%d,%.3f\n", gen, elite.fit);
    }
  }
  puts("");

  free(parent);
  return elite;
}

//----------------------------------------------------------
//  各トライの進行
//----------------------------------------------------------

double poker_exec(int stock[])
{
  //----  局所変数
  int used[CNUM];             // 捨札配列
  int us;                     // 捨札数
  double try_p;               // トライ得点
  int tk;                     // テイク数

  //----  トライの最中(テイクを繰り返す)
  us = 0;
  try_p = 0;
  for ( tk = 0; tk < TAKE; tk++ ) {
    try_p += poker_take(stock, tk, used, &us) * Take_Weight[tk];
  }

  return try_p;
}

//----------------------------------------------------------
//  遺伝子の初期化
//----------------------------------------------------------

void initialize(Genome parent[], int stock[])
{
  int k1, k2;

  for ( k1 = 0; k1 < POPSIZE; k1++ ) {
    for ( k2 = 0; k2 < STEP; k2++ ) { parent[k1].gene[k2] = myrand(); }
  }
  set_fitness(parent, POPSIZE, stock);
}

//----------------------------------------------------------
//  適合度の計算
//----------------------------------------------------------

void set_fitness(Genome g[], int n, int stock[])
{
  int k;

  for ( k = 0; k < n; k++ ) {
    arr_copy(Step, g[k].gene, STEP);
    StepCounter = 0;
    g[k].fit = poker_exec(stock);
  }
}

//----------------------------------------------------------
//  選択
//----------------------------------------------------------

Genome selection(Genome g[], int n)
{
  return selection_roulette(g, n);
}

//----------------------------------------------------------
//  ルーレット選択
//----------------------------------------------------------

Genome selection_roulette(Genome g[], int n)
{
  double total = 0;
  double val = 0;
  double r = frand();
  int k;

  for ( k = 0; k < n; k++ ) { total += g[k].fit; }
  for ( k = 0; k < n; k++ ) {
    val += g[k].fit / total;
    if ( r < val ) { return g[k]; }
  }
  return g[ irand(n) ];
}

//----------------------------------------------------------
//  交叉
//----------------------------------------------------------

void crossover(Genome parent[], Genome *c1, Genome *c2)
{
  Genome g1 = parent[irand(POPSIZE)];
  Genome g2 = parent[irand(POPSIZE)];

  if ( irand(100) < PROB_C ) { crossover_double(&g1, &g2); }
  *c1 = g1; *c2 = g2;
}

//----------------------------------------------------------
//  二点交叉
//----------------------------------------------------------

void crossover_double(Genome *g1, Genome *g2)
{
  int k;
  int s1 = irand(STEP);
  int s2 = irand(STEP);

  if ( s1 > s2 ) { int t = s1; s1 = s2; s2 = s1; }

  for ( k = 0; k < STEP; k++ ) {
    if ( s1 <= k && k <= s2 ) {
      int t = g1->gene[k];
      g1->gene[k] = g2->gene[k];
      g2->gene[k] = t;
    }
  }
}

//----------------------------------------------------------
//  突然変異
//----------------------------------------------------------

void mutation(Genome g[], int n)
{
  int k;

  for ( k = 0; k < n; k++ ) {
    if ( irand(100) < PROB_M ) {
      int r = irand(STEP);
      g[k].gene[r] = uniq_rand(g[k].gene[r]);
    }
  }
}

//----------------------------------------------------------
//  エリートの選別
//----------------------------------------------------------

Genome get_elite(Genome g[], int n)
{
  int k;
  int s = 0;
  double max = 0;
  for ( k = 0; k < n; k++ ) {
    if ( max < g[k].fit ) { max = g[k].fit; s = k; }
  }
  return g[s];
}

//----------------------------------------------------------
//  世代交代
//----------------------------------------------------------

void generation_change(Genome parent[], Genome child[])
{
  int k;
  int size = POPSIZE / 2;

  //----  エリートは必ず次世代へ
  parent[0] = get_elite(parent, POPSIZE);

  //----  半分は親世代から引き継ぎ
  for ( k = 1; k < size; k++ ) {
    parent[k] = selection(child, POPSIZE);
  }

  //----  半分は新しい個体が参入
  for ( ; k < POPSIZE; k++ ) {
    int i;
    for ( i = 0; i < STEP; i++ ) {
      parent[k].gene[i] = myrand();
    }
  }
}

//----------------------------------------------------------
//  かぶらない乱数
//----------------------------------------------------------

int uniq_rand(int base)
{
  int r = myrand();
  while ( base == r ) { r = myrand(); }
  return r;
}

//----------------------------------------------------------
//  -1～4の乱数
//----------------------------------------------------------

int myrand(void)
{
  return irand(6)-1;
}

