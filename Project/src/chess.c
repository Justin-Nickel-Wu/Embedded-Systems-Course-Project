#include "chess.h"
#include "lcd.h"

const int ChessBoardPos[5] = {24, 72, 120, 168, 216};

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