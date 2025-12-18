#include "chess.h"
#include "lcd.h"

void drawChessboard() {
    LCD_Clear(CHESSBOARD_COL);

    // 240*320 选取240*240区域座位棋盘。
    // 五条线各占48，上下左右各留24
    for (int i = 0; i < 5; ++i) {
        LCD_DrawLine(24, 24 + i * 48, 240 - 24, 24 + i * 48); // 横线
        LCD_DrawLine(24 + i * 48, 24, 24 + i * 48, 240 - 24); // 竖线
    }
}