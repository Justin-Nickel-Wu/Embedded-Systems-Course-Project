#ifndef __CHESS_H
#define __CHESS_H

#define CHESSBOARD_COL 0x02F0 // 棋盘颜色
#define CYAN 0xFFE0
#define PIECE_RADIUS 13 // 棋子半径

extern const int ChessBoardPos[5];
extern int Table[5][5]; // 棋盘状态，0表示无子，1表示白子，2表示黑子
extern int PieceX, PieceY, PieceValid; // 当前选中的棋子位置
extern int LastPieceX, LastPieceY, LastPieceValid; // 上次选中的棋子位置

void drawChessboard(); // 画棋盘
void reDrawChessboardLine(int x, int y); // 重画棋盘线条
void selectPiece(); // 选择棋子
void movePiece(); // 移动棋子
void changeTurn(); // 切换玩家
void checkTable(); // 检查棋盘状态
#endif