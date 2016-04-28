//====================================================================
//  事前処理
//====================================================================

//--------------------------------------------------------------------
//  定数定義マクロ
//--------------------------------------------------------------------

#define  GNUM  10000    // ゲーム最大回数
#define  CNUM     52    // カードの枚数
#define  HNUM      5    // 手札の枚数
#define  FNUM     10    // 場札の最大枚数(5×2)

#define GAME_NAME_LEN 256    // ゲームのインスタンス名の最大長
#define FILE_NAME_LEN 256    // ファイル名の最大長
#define DIR_NAME_LEN  256    // ディレクトリ名の最大長
#define COMM_LEN      256    // コマンド行の最大長
#define POINT_MAX       9    // 役番号の最大点
#define POINT_MIN       0    // 役番号の最小点
#define POINT_NUM      10    // 手役の数

//----  役の配点
#define P0    0    // ノーペア
#define P1    1    // ワンペア
#define P2    2    // ツーペア
#define P3    8    // スリーカインズ
#define P4   32    // ストレート
#define P5   24    // フラッシュ
#define P6   16    // フルハウス
#define P7   64    // フォーカード
#define P8  128    // ストレートフラッシュ
#define P9  256    // ロイヤルストレートフラッシュ

//====================================================================
//  大域宣言
//====================================================================

//--------------------------------------------------------------------
//  大域変数
//--------------------------------------------------------------------

static char SUITE[] = "SHDC";
static char NUMBER[] = "123456789TJQK";

//====================================================================
//  関数宣言
//====================================================================

//--------------------------------------------------------------------
//  乱順列
//--------------------------------------------------------------------

void qrand(int seq[], int n);
int irand(int n);
double frand(void);

//--------------------------------------------------------------------
//  配列の基本処理
//--------------------------------------------------------------------

void arr_copy(int arr0[], const int arr1[], int n);
void arr_swap(int arr[], int p1, int p2);
void arr_output(int arr[], int n);

//--------------------------------------------------------------------
//  トランプカードの表現
//--------------------------------------------------------------------

void card_show(int cd[], int n);
char *card_str(int id);
int card_id(char *str);

//--------------------------------------------------------------------
//  トランプの基本処理
//--------------------------------------------------------------------

void card_stock(int stock[], FILE *fp);

//--------------------------------------------------------------------
//  ポーカーの進行
//--------------------------------------------------------------------

double poker_exec(FILE *fp, int point[]);
int poker_take(const int stock[], int tk, int used[], int *us);
void take_show(int st[][HNUM], int ope[], int fd[], int cg, int tp);
#ifdef TAKE
void result_show(int point[][TAKE]);
#endif

//--------------------------------------------------------------------
//  ポーカーの役と得点
//--------------------------------------------------------------------

int poker_point(int hand[]);
int poker_point_pair(int num[]);
int poker_point_flash(int sut[]);
int poker_point_straight(int num[], int p);

//--------------------------------------------------------------------
//  ゲーム戦略
//--------------------------------------------------------------------

int strategy(const int hd[], const int fd[], int cg, int tk, const int ud[], int us);

