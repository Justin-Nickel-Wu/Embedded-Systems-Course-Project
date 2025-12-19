#include "chess.h"
#include "lcd.h"

const int ChessBoardPos[5] = {24, 72, 120, 168, 216};

int PieceX, PieceY; // 当前选中的棋子位置
int LastPieceX = -1, LastPieceY = -1; // 上次选中的棋子位置

void drawChessboard() {
    LCD_Clear(CHESSBOARD_COL);

    // 240*320 选取240*240区域座位棋盘。
    // 五条线各占48，上下左右各留24
    for (int i = 0; i < 5; ++i) {
        LCD_DrawLine(24, 24 + i * 48, 240 - 24, 24 + i * 48); // 横线
        LCD_DrawLine(24 + i * 48, 24, 24 + i * 48, 240 - 24); // 竖线
    }

    POINT_COLOR = BLACK;
    for (int i = 0; i < 5; ++i) {
        LCD_Draw_Circle(24, 24 + i * 48, PIECE_RADIUS, 1);
    }

    POINT_COLOR = WHITE;
    for (int i = 0; i < 5; ++i) {
        LCD_Draw_Circle(240 - 24, 24 + i * 48, PIECE_RADIUS, 1);
    }
}

void selectPiece() {
    if (PressFlag > 0) {
        PressFlag = 0;
        PieceX = PieceY = -1;
        for (int i = 0; i < 5; ++i)
            if (abs(xScreen - ChessBoardPos[i]) < PIECE_RADIUS) {
                PieceX = i;
            }
        for (int i = 0; i < 5; ++i)
            if (abs(yScreen - ChessBoardPos[i]) < PIECE_RADIUS) {
                PieceY = i;
            }
        if (PieceX != -1 && PieceY != -1) {
            // 擦除上次选中
            if (LastPieceX != -1 && LastPieceY != -1) {
                POINT_COLOR = CHESSBOARD_COL;
                LCD_Draw_Circle(ChessBoardPos[LastPieceX], ChessBoardPos[LastPieceY], PIECE_RADIUS + 1, 0);
            }
            // 记录本次选中
            POINT_COLOR = CYAN;
            LCD_Draw_Circle(ChessBoardPos[PieceX], ChessBoardPos[PieceY], PIECE_RADIUS + 1, 0);
            LastPieceX = PieceX;
            LastPieceY = PieceY;
        }
    }
}