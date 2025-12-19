#include "chess.h"
#include "lcd.h"

const int ChessBoardPos[5] = {24, 72, 120, 168, 216};

struct cordinate {
    int x;
    int y;
};

int PieceX = -1, PieceY = -1, PieceValid = 0; // 当前选中的棋子位置
int LastPieceX = -1, LastPieceY = -1, LastPieceValid = 0; // 上次选中的棋子位置
int Table[5][5]; // 棋盘状态，0表示无子，1表示白子，2表示黑子
int whichTurn = 2; // 1表示白方，2表示黑方。黑色先行。
int SelectPieceFlag = 0; // 选择棋子标志
int MovePieceFlag = 0; // 移动棋子标志

// 初始化棋盘，包括了棋子位置的初始化
void drawChessboard() {
    char showstr[32]; // 显示用字符串
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

    sprintf(showstr, "Time for:");
    POINT_COLOR = BLACK;
    BACK_COLOR = CHESSBOARD_COL;
    LCD_ShowString(65, 260, 240, 20, 16, showstr);
    LCD_Draw_Circle(160, 265, PIECE_RADIUS, 1); // 黑子先行
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
        SelectPieceFlag = 1;
        // 擦除上次选中
        // 如果本次触摸到棋盘外，相当于取消上次的选择
        LastPieceX = PieceX;
        LastPieceY = PieceY;
        LastPieceValid = PieceValid;
        if (LastPieceValid) {
            POINT_COLOR = CHESSBOARD_COL;
            LCD_Draw_Circle(ChessBoardPos[LastPieceX], ChessBoardPos[LastPieceY], PIECE_RADIUS + 1, 0);
            reDrawChessboardLine(LastPieceX, LastPieceY);
        }

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
            PieceValid = 1;
            // 记录本次选中
            POINT_COLOR = CYAN;
            LCD_Draw_Circle(ChessBoardPos[PieceX], ChessBoardPos[PieceY], PIECE_RADIUS + 1, 0);
        } else {
            PieceValid = 0;
        }
    } else
        SelectPieceFlag = 0;
}

void movePiece() {
    if (SelectPieceFlag) {
        int LastTouchValid, TouchValid, Distance;
        LastTouchValid = LastPieceValid && Table[LastPieceX][LastPieceY] == whichTurn;
        TouchValid = PieceValid && Table[PieceX][PieceY] == 0;
        Distance = abs(LastPieceX - PieceX) + abs(LastPieceY - PieceY);

        // 判断是否可以移动
        if (LastTouchValid && TouchValid && (Distance == 1)) {
            // 移动棋子
            Table[PieceX][PieceY] = Table[LastPieceX][LastPieceY];
            Table[LastPieceX][LastPieceY] = 0;

            // 绘制移动后的棋子
            POINT_COLOR = (Table[PieceX][PieceY] == 1) ? WHITE : BLACK;
            LCD_Draw_Circle(ChessBoardPos[PieceX], ChessBoardPos[PieceY], PIECE_RADIUS, 1);

            // 重新绘制原有位置
            POINT_COLOR = CHESSBOARD_COL;
            LCD_Draw_Circle(ChessBoardPos[LastPieceX], ChessBoardPos[LastPieceY], PIECE_RADIUS, 1);
            reDrawChessboardLine(LastPieceX, LastPieceY);

            // 置位移动标志
            MovePieceFlag = 1;
            return;
        }
    }
    MovePieceFlag = 0;
}

void changeTurn() {
    if (MovePieceFlag) {
        whichTurn = (whichTurn == 1) ? 2 : 1;
        POINT_COLOR = whichTurn == 1 ? WHITE : BLACK;
        LCD_Draw_Circle(160, 265, PIECE_RADIUS, 1);
    }
}

// 检查是否吃子
void checkEatPiece() {
    int MyPieceCnt, EnPieceCnt, i, j, MyPiece;
    int targetX = -1, targetY = -1;
    MyPiece = Table[PieceX][PieceY];
    // 只有刚刚移动的子会触发吃子检查
    // 检查纵向
    MyPieceCnt = EnPieceCnt = 0;
    for (j = 0; j < 5; ++j)
        if (Table[PieceX][j] != 0)
            Table[PieceX][j] == MyPiece ? ++MyPieceCnt : ++EnPieceCnt;

    // 三子可吃，四子及以上触发特殊规则不能吃。
    // 共三种情况： OOX OXO XOO
    // 下同
    if (MyPieceCnt == 2 && EnPieceCnt == 1) {
        if (PieceY >= 2 && Table[PieceX][PieceY - 1] && Table[PieceX][PieceY - 2]) {
            if (Table[PieceX][PieceY - 1] != MyPiece) {
                targetX = PieceX;
                targetY = PieceY - 1;
            } else {
                targetX = PieceX;
                targetY = PieceY - 2;
            }
        }

        if (PieceY <= 2 && Table[PieceX][PieceY + 1] && Table[PieceX][PieceY + 2]) {
            if (Table[PieceX][PieceY + 1] != MyPiece) {
                targetX = PieceX;
                targetY = PieceY + 1;
            } else {
                targetX = PieceX;
                targetY = PieceY + 2;
            }
        }

        if (PieceY > 0 && PieceY < 4 && Table[PieceX][PieceY - 1] && Table[PieceX][PieceY + 1]) {
            if (Table[PieceX][PieceY - 1] != MyPiece) {
                targetX = PieceX;
                targetY = PieceY - 1;
            } else {
                targetX = PieceX;
                targetY = PieceY + 1;
            }
        }
    }

    // 发生吃子
    if (targetX != -1) {
        Table[targetX][targetY] = 0;
        POINT_COLOR = CHESSBOARD_COL;
        LCD_Draw_Circle(ChessBoardPos[targetX], ChessBoardPos[targetY], PIECE_RADIUS, 1);
        reDrawChessboardLine(targetX, targetY);
    }

    // 检查横向
    targetX = -1;
    MyPieceCnt = EnPieceCnt = 0;
    for (i = 0; i < 5; ++i)
        if (Table[i][PieceY] != 0)
            Table[i][PieceY] == MyPiece ? ++MyPieceCnt : ++EnPieceCnt;
    if (MyPieceCnt == 2 && EnPieceCnt == 1) {
        if (PieceX >= 2 && Table[PieceX - 1][PieceY] && Table[PieceX - 2][PieceY]) {
            if (Table[PieceX - 1][PieceY] != MyPiece) {
                targetX = PieceX - 1;
                targetY = PieceY;
            } else {
                targetX = PieceX - 2;
                targetY = PieceY;
            }
        }
        if (PieceX <= 2 && Table[PieceX + 1][PieceY] && Table[PieceX + 2][PieceY]) {
            if (Table[PieceX + 1][PieceY] != MyPiece) {
                targetX = PieceX + 1;
                targetY = PieceY;
            } else {
                targetX = PieceX + 2;
                targetY = PieceY;
            }
        }
        if (PieceX > 0 && PieceX < 4 && Table[PieceX - 1][PieceY] && Table[PieceX + 1][PieceY]) {
            if (Table[PieceX - 1][PieceY] != MyPiece) {
                targetX = PieceX - 1;
                targetY = PieceY;
            } else {
                targetX = PieceX + 1;
                targetY = PieceY;
            }
        }
    }
    // 发生吃子
    if (targetX != -1) {
        Table[targetX][targetY] = 0;
        POINT_COLOR = CHESSBOARD_COL;
        LCD_Draw_Circle(ChessBoardPos[targetX], ChessBoardPos[targetY], PIECE_RADIUS, 1);
        reDrawChessboardLine(targetX, targetY);
    }
}

void checkTable() {
    if (MovePieceFlag) {
        // TODO: 检查对手是否无子可动，或其他胜利条件
        checkEatPiece();
    }
}