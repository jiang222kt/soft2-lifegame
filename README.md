## ライフゲームの課題について
以下、ライフゲームの課題でやった内容を簡潔にまとめる。

## 追加した関数の内容

- 生きているセルの総数をカウントする
 
int count_alive_cells(const int height, const int width,  int cell[height][width]);


- Life1.06形式のファイルに出力する

int output_to_life06(int filenamenum, const int height, const int width, int cell[height][width]);


- RLE形式のファイルを読み込む

void rle_my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);


- 'o'の連続セルをcellの2次配列を反映

void partial_cell(int h,  int position,int num, const int height, const int width, int cell[height][width]);


## 課題1について
- 重複しないように乱数を生成するための種にtime関数の結果を利用する。
- 乱数を0~0.1の範囲に変換して、0.1より小さいものを生きているセルとした。
- これにより、ランダム初期化の際に生きたセルの割合がおよそ10%になった。

## 課題2について
- 100世代ごとの盤面の情報をLife1.06形式でファイルに出力するよう実装した。
- 10000世代以上は出力しないようにした。

## 課題3について
- RLEフォーマットのファイルを行に分けて、1行ずつ解析しそれを行全体にわたってループした。
- 1行ずつ解析する際に、表示されない'b'の係数も考慮し、また、'$'が出てくると次の行に移動し、列も初期化(1列目)した。
- 係数がないものは係数が1として判別するようにした。

**実行結果を確かめるためのサンプルデータを用意したので、それを実行するコマンドを以下に記す**

コンパイル:
```
gcc -o mylife3game -Wall mylife3.c
```

実行:
```
./mylife3game data.rle
```
