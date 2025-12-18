#ifndef __CHESS_H
#define __CHESS_H

#define CHESSBOARD_COL 0x02F0 // 棋盘颜色
#define BLACK 0x0000
#define WHITE 0xFFFF
#define PIECE_RADIUS 13 // 棋子半径

extern const int ChessBoardPos[5];

void drawChessboard();

#endif