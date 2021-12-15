#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
// #include "gol.h"
#include <time.h>
#include <string.h>
#include <ctype.h>


int count_alive_cells(const int height, const int width,  int cell[height][width]);

int output_to_life06(int filenamenum, const int height, const int width, int cell[height][width]){
  FILE *outputfile;
  char filename[256];
  int points = 0;
  sprintf(filename, "gen%04d.lif",filenamenum);
    if ( (outputfile = fopen(filename,"w")) != NULL ) {
      fputs("#Life 1.06\n",outputfile);
      for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
          if (cell[y][x]) {
            char line[256];
            sprintf(line, "%d %d\n", y, x);
            fputs(line, outputfile);
            points++;
          }
        }
      }
    }
    else{
      fprintf(stderr,"cannot open file %s\n",filename);
      return EXIT_FAILURE;
    }
    fclose(outputfile);
    return points;
}

int count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]);

/* 'o'の連続セルをcellの2次配列を反映 */
void partial_cell(int h,  int position,int num, const int height, const int width, int cell[height][width]){
  for ( int i = 0; i < num; i++){
    cell[h][position + i] = 1;
    if ( position + i > width){
      printf("Over Error\n");
      exit(0);
    }
  }
  

}
/* RLEファイルを処理する */
void rle_init_cells(const int height, const int width, int cell[height][width], FILE* fp){

    int h = 0;
    int position, i, num;
    char buf[256];
    char tmp_num[64];
    int count = 0;
    while(fgets(buf, 256, fp) != NULL){
        i = 0;
        position = 0;
        count = 0;
        num = 0;
        while(buf[i] != '\n' && buf[i] != '!' && buf[i] != '\r'){
          if (buf[i] == '$'){
            h++;
            position = 0;
            i++;
            continue;
          }
          if (isdigit(buf[i])){
            count = 0;
            while(isdigit(buf[i])){
              tmp_num[count] = buf[i];
              count++;
              tmp_num[count] = '\0';
              i++;          
            }
            num = atoi(tmp_num);
            count = 0;
            if(buf[i] != 'b' && buf[i] != 'o'){
              printf("RLE file format erro\n");
              exit(0);
            }
            if (buf[i] == 'o'){
              partial_cell(h, position, num, height, width, cell); 
            }
            position = position + num;
          }
          else {
            if (buf[i] == 'o'){
            partial_cell(h, position, 1, height, width, cell);
            }
            position++;
          }
          i++;
        }
      }
}

/*
 ファイルによるセルの初期化: 生きているセルの座標が記述されたファイルをもとに2次元配列の状態を初期化する
 fp = NULL のときは、関数内で適宜定められた初期状態に初期化する。関数内初期値はdefault.lif と同じもの
 */
void init_cells(const int height, const int width, int cell[height][width], FILE* fp){
    
    if(fp != NULL ){
        int x, y;

        while(fscanf(fp, "%d %d\n", &x, &y) == 2){
            cell[y][x] = 1;
        }
    }else{
        // time関数を使って乱数の種を設定
        srand((unsigned int)time(NULL));

        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                // 生きるセルを10%に設定
                // rand() の範囲 0～RAND_MAX
                if( (float)rand() / RAND_MAX < 0.1){
                    cell[y][x] = 1;
                } 
            }
        }
    }
}

/*
 グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
 */
void print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]){
      //上の壁

  fprintf(fp,"[cells alive(%%)]: %.2f%%\n", (float)count_alive_cells(height, width,cell)/(height * width)*100); // この場合 (fp = stdout), printfと同じ
  fprintf(fp,"+");
  for (int x = 0 ; x < width ; x++)
    fprintf(fp, "-");
  fprintf(fp, "+\n");
  
  /* 外壁と 内側のゲーム部分 */
  for (int y = 0; y < height; y++) {
    fprintf(fp,"|");
    for (int x = 0; x < width; x++) {
      // ANSIエスケープコードを用いて、赤い"#" を表示
      // \e[31m で 赤色に変更
      // \e[0m でリセット（リセットしないと以降も赤くなる）
      if(cell[y][x]){
	fprintf(fp, "\e[31m#\e[0m");
      }
      else{
	fprintf(fp, " ");
      }
    }
    fprintf(fp,"|\n");
  }

  // 下の壁
  fprintf(fp, "+");
  for (int x = 0 ; x < width ; x++)
    fprintf(fp, "-");
  fprintf(fp, "+\n");
  
  fflush(fp); // バッファされている文字列を出力する
  
}

/*
 着目するセルの周辺の生きたセルをカウントする関数
 */
int count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]){
    int num = 0;
    for ( int i = -1; i <= 1; i = i + 2){
        // 左右のセル
        if ( w + i >= 0 && w + i < width ){
            num = num + cell[h][w+i];
        }
        // 上下のセル
        if ( h + i >=0 && h + i < height ){
            num = num + cell[h+i][w];
        }
        // 右上方向のセル
        if ( h + i >=0 && h + i < height && w + i >= 0 && w + i < width){
            num = num + cell[h+i][w+i];
        }
        // 右下方向のセル
        if ( h - i >=0 && h - i < height && w + i >= 0 && w + i < width ){
            num = num + cell[h-i][w+i];
        }
    }
    return num;
}

/*
 ライフゲームのルールに基づいて2次元配列の状態を更新する
 */
void update_cells(const int height, const int width, int cell[height][width]){
    // 次世代のセルを一時保管
    int tmp_cell[40][70];

    for(int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            tmp_cell[y][x] = cell[y][x];
            int z;
            z = count_adjacent_cells(y, x, height, width, cell);
            if (cell[y][x]){
                if(z != 2 && z != 3){
                    tmp_cell[y][x] = 0;
                }
            }else{
                if(z == 3){
                    tmp_cell[y][x] = 1;
                }
            }
        }
    }

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            cell[y][x] = tmp_cell[y][x];
        }
    }

}

int count_alive_cells(const int height, const int width,  int cell[height][width]){
    int count = 0;
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            count = count + cell[y][x];
        }
    }
    return count;
}

int main(int argc, char **argv)
{
  FILE *fp = stdout;
  const int height = 40;
  const int width = 70;
  char buf[256];
  char filetype[64];

  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0;
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 2 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    FILE *lgfile;
    if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
      
      fgets(buf, 256, lgfile);
      sscanf(buf, "%s", filetype);
      printf("%s\n", filetype);
      if(strcmp("#Life", filetype) ==0 ){
        init_cells(height,width,cell,lgfile); // Lifeファイルによる初期化
      }else{
        if(buf[0] == '#'){
          // 2行目のヘッダーを読み捨て
          fgets(buf, 256, lgfile);
          rle_init_cells(height,width,cell,lgfile);
        }
      }  
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
    init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
  }

  printf("begin\n");

  print_cells(fp, 0, height, width, cell); // 表示する
  sleep(1); // 1秒休止
  fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

  /* 世代を進める*/
  for (int gen = 1 ;; gen++) {
    update_cells(height, width, cell); // セルを更新
    print_cells(fp, gen, height, width, cell);  // 表示する
    sleep(1); //1秒休止する
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
    if ( gen % 100 == 0 && gen < 10000){
      output_to_life06(gen, height, width, cell);
    }
  }

  return EXIT_SUCCESS;
}
