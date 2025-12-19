#ifndef __CHESS_H
#define __CHESS_H

#define CHESSBOARD_COL 0x02F0 // 棋盘颜色
#define CYAN 0xFFE0
#define PIECE_RADIUS 13 // 棋子半径

extern const int ChessBoardPos[5];

void drawChessboard();

#endif