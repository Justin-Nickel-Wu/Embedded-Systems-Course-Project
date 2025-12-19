#include "chess.h"
#include "lcd.h"

const int ChessBoardPos[5] = {24, 72, 120, 168, 216};

int PieceX, PieceY; // 当前选中的棋子位置
int LastPieceX = -1, LastPieceY = -1; // 上次选中的棋子位置
int Table[5][5]; // 棋盘状态，0表示无子，1表示白子，2表示黑子

// 初始化棋盘，包括了棋子位置的初始化
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

    memset(Table, 0, sizeof(Table));
    Table[0][0] = Table[0][1] = Table[0][2] = Table[0][3] = Table[0][4] = 2; // 黑子
    Table[4][0] = Table[4][1] = Table[4][2] = Table[4][3] = Table[4][4] = 1; // 白子
}

// 重画棋盘线条
// 输入参数x,y表示棋盘交叉位置
void reDrawChessboardLine(int x, int y) {
    int xx, yy, L, R, T, B;
    xx = 24 + x * 48;
    yy = 24 + y * 48;
    POINT_COLOR = BLACK;
    L = (x == 0) ? xx : xx - (PIECE_RADIUS + 1);
    R = (x == 4) ? xx : xx + (PIECE_RADIUS + 1);
    T = (y == 0) ? yy : yy - (PIECE_RADIUS + 1);
    B = (y == 4) ? yy : yy + (PIECE_RADIUS + 1);
    if (Table[x][y] != 0) { // 如果有棋子，只需要画几个点
        if (x != 0) LCD_DrawPoint(L, yy);
        if (x != 4) LCD_DrawPoint(R, yy);
        if (y != 0) LCD_DrawPoint(xx, T);
        if (y != 4) LCD_DrawPoint(xx, B);
    } else { // 否则需要画线
        LCD_DrawLine(L, yy, R, yy); // 横线
        LCD_DrawLine(xx, T, xx, B); // 竖线
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
                reDrawChessboardLine(LastPieceX, LastPieceY);
            }
            // 记录本次选中
            POINT_COLOR = CYAN;
            LCD_Draw_Circle(ChessBoardPos[PieceX], ChessBoardPos[PieceY], PIECE_RADIUS + 1, 0);
            LastPieceX = PieceX;
            LastPieceY = PieceY;
        }
    }
}