from __future__ import annotations
from dataclasses import dataclass
from typing import List, Tuple, Optional, Iterable
import math
import serial
import time
import serial.tools.list_ports


Coord = Tuple[int, int]          # (x, y)
Move = Tuple[Coord, Coord]       # ((x1,y1),(x2,y2))

EMPTY = '.'
BLACK = 'B'
WHITE = 'W'

W, H = 5, 5

# 初始布局：每行 B...W，共5行
INIT_BOARD = [
    list("B...W"),
    list("B...W"),
    list("B...W"),
    list("B...W"),
    list("B...W"),
]

DIRS_4 = [(1,0), (-1,0), (0,1), (0,-1)]


def in_bounds(x: int, y: int) -> bool:
    return 0 <= x < W and 0 <= y < H


def other(player: str) -> str:
    return WHITE if player == BLACK else BLACK


@dataclass(frozen=True)
class GameState:
    board: Tuple[Tuple[str, ...], ...]   # 5x5 immutable
    turn: str                             # whose turn to move: 'B' or 'W'

    @staticmethod
    def new_initial() -> "GameState":
        return GameState(tuple(tuple(r) for r in INIT_BOARD), BLACK)  # 黑先

    def at(self, x: int, y: int) -> str:
        return self.board[y][x]

    def count(self, piece: str) -> int:
        return sum(1 for y in range(H) for x in range(W) if self.at(x, y) == piece)

    def iter_pieces(self, piece: str) -> Iterable[Coord]:
        for y in range(H):
            for x in range(W):
                if self.at(x, y) == piece:
                    yield (x, y)

    def legal_moves(self) -> List[Move]:
        """当前 turn 方的所有合法移动（一步、上下左右、落点空）。"""
        p = self.turn
        moves: List[Move] = []
        for (x, y) in self.iter_pieces(p):
            for dx, dy in DIRS_4:
                nx, ny = x + dx, y + dy
                if in_bounds(nx, ny) and self.at(nx, ny) == EMPTY:
                    moves.append(((x, y), (nx, ny)))
        return moves

    def is_terminal(self) -> bool:
        """输赢判断：1子或无路可走。"""
        if self.count(BLACK) <= 1 or self.count(WHITE) <= 1:
            return True
        if len(self.legal_moves()) == 0:
            return True
        return False

    def winner(self) -> Optional[str]:
        """返回赢家（B/W），未结束返回 None。"""
        if self.count(BLACK) <= 1:
            return WHITE
        if self.count(WHITE) <= 1:
            return BLACK
        if len(self.legal_moves()) == 0:
            return other(self.turn)  # 轮到谁走但无路，谁输
        return None

    def apply_move(self, mv: Move) -> "GameState":
        """执行一个（已保证合法的）移动，并按规则处理行+列吃子（仅由本步落点触发）。"""
        (x1, y1), (x2, y2) = mv
        p = self.turn
        q = other(p)

        # 转成可变
        b = [list(row) for row in self.board]

        # move
        b[y1][x1] = EMPTY
        b[y2][x2] = p

        # 只检查“本步落点”在 行 与 列 上触发的吃子
        captures = []
        captures += self._compute_line_captures(board=b, moved_to=(x2, y2), moved_piece=p, axis="row")
        captures += self._compute_line_captures(board=b, moved_to=(x2, y2), moved_piece=p, axis="col")

        # 去重并执行
        for (cx, cy) in dict.fromkeys(captures):
            b[cy][cx] = EMPTY

        return GameState(tuple(tuple(r) for r in b), q)


    @staticmethod
    def _compute_line_captures(board: List[List[str]],
                            moved_to: Coord,
                            moved_piece: str,
                            axis: str) -> List[Coord]:
        """
        只由“本步落点 moved_to”触发的吃子判定（行 / 列）：

        规则：
        1. 只检查落点所在的行或列
        2. 若该行/列非空棋子数 >= 4，则整条线不发生吃子
        3. 否则，仅检查“包含落点”的连续三格
        4. 三格都非空，且“两同一异”
        5. 仅当“两同 == moved_piece 且 孤子 == enemy”时，吃掉孤子
        """
        mx, my = moved_to
        enemy = other(moved_piece)
        captures: List[Coord] = []

        # --- 线访问工具 ---
        def get_cell(i: int) -> str:
            return board[my][i] if axis == "row" else board[i][mx]

        def coord(i: int) -> Coord:
            return (i, my) if axis == "row" else (mx, i)

        L = W if axis == "row" else H
        pos = mx if axis == "row" else my

        # --- 规则：整条线 >=4 子，禁止吃子 ---
        nonempty = sum(1 for i in range(L) if get_cell(i) != EMPTY)
        if nonempty >= 4:
            return []

        # --- 仅枚举“包含落点”的三连窗口 ---
        for s in (pos - 2, pos - 1, pos):
            if not (0 <= s <= L - 3):
                continue

            a = get_cell(s)
            b = get_cell(s + 1)
            c = get_cell(s + 2)

            # 三格必须全非空
            if EMPTY in (a, b, c):
                continue

            # 必须是“两同一异”
            if len({a, b, c}) != 2:
                continue

            # --- 找孤子 ---
            if a != b and b == c:
                lone_i, majority, lone_piece = s, b, a
            elif c != b and a == b:
                lone_i, majority, lone_piece = s + 2, b, c
            else:
                # ABA 型
                lone_i, majority, lone_piece = s + 1, a, b

            # --- 只吃对方，不吃自己 ---
            if majority == moved_piece and lone_piece == enemy:
                captures.append(coord(lone_i))

        return captures

    def printBoard(self):
        for j in range(0, 5):
            for i in range(0,5):
                str = self.at(i,j)
                print(str, end='')
                if i == 4:
                    print('\n')


# ---------------- AI 部分：Minimax + AlphaBeta ----------------

def evaluate(state: GameState, me: str = WHITE) -> float:
    """
    简单但够用的评估：
    - 子力差（最重要）
    - 机动性（可走步数）
    - “下一步能吃子”的机会（粗略）
    """
    if state.is_terminal():
        w = state.winner()
        if w is None:
            return 0.0
        return 1e9 if w == me else -1e9

    my = me
    op = other(me)

    my_count = state.count(my)
    op_count = state.count(op)

    # 机动性：谁能走得动谁更强
    # 注意：state.legal_moves() 是当前 turn 的，这里要分别估计两边
    my_moves = count_moves_for(state, my)
    op_moves = count_moves_for(state, op)

    # 简单“吃子威胁”：看我方任意一步是否会让对方少子
    my_threat = count_immediate_captures(state, my)
    op_threat = count_immediate_captures(state, op)

    return (
        1000.0 * (my_count - op_count)
        + 3.0 * (my_moves - op_moves)
        + 30.0 * (my_threat - op_threat)
    )


def count_moves_for(state: GameState, player: str) -> int:
    # 临时改 turn 计算
    tmp = GameState(state.board, player)
    return len(tmp.legal_moves())


def count_immediate_captures(state: GameState, player: str) -> int:
    tmp = GameState(state.board, player)
    before_op = tmp.count(other(player))
    cnt = 0
    for mv in tmp.legal_moves():
        ns = tmp.apply_move(mv)
        after_op = ns.count(other(player))
        if after_op < before_op:
            cnt += 1
    return cnt


def alphabeta(state: GameState, depth: int, alpha: float, beta: float, me: str) -> Tuple[float, Optional[Move]]:
    if depth == 0 or state.is_terminal():
        return evaluate(state, me), None

    maximizing = (state.turn == me)
    best_move: Optional[Move] = None

    moves = state.legal_moves()
    # 小优化：优先搜索“能吃子”的走法
    moves = sorted(moves, key=lambda mv: move_captures_count(state, mv), reverse=maximizing)

    if maximizing:
        value = -math.inf
        for mv in moves:
            ns = state.apply_move(mv)
            score, _ = alphabeta(ns, depth - 1, alpha, beta, me)
            if score > value:
                value, best_move = score, mv
            alpha = max(alpha, value)
            if alpha >= beta:
                break
        return value, best_move
    else:
        value = math.inf
        for mv in moves:
            ns = state.apply_move(mv)
            score, _ = alphabeta(ns, depth - 1, alpha, beta, me)
            if score < value:
                value, best_move = score, mv
            beta = min(beta, value)
            if alpha >= beta:
                break
        return value, best_move


def move_captures_count(state: GameState, mv: Move) -> int:
    before_b = state.count(BLACK)
    before_w = state.count(WHITE)
    ns = state.apply_move(mv)
    after_b = ns.count(BLACK)
    after_w = ns.count(WHITE)
    return (before_b - after_b) + (before_w - after_w)


class WhiteAI:
    def __init__(self, depth: int = 4):
        self.state = GameState.new_initial()
        self.depth = depth

    def apply_opponent_move(self, x1: int, y1: int, x2: int, y2: int) -> None:
        """黑子走一步（你保证合法）。"""
        assert self.state.turn == BLACK, "现在不是黑方回合？"
        self.state = self.state.apply_move(((x1, y1), (x2, y2)))

    def choose_move(self) -> Tuple[int, int, int, int]:
        """白子选一步并落子，返回 x1 y1 x2 y2。"""
        assert self.state.turn == WHITE, "现在不是白方回合？"

        _, mv = alphabeta(self.state, self.depth, -math.inf, math.inf, WHITE)
        if mv is None:
            # 无棋可走：直接认输式输出（你那边也会判负）
            return 0, 0, 0, 0

        (x1, y1), (x2, y2) = mv
        self.state = self.state.apply_move(mv)
        return x1, y1, x2, y2
    
ser = serial.Serial(
    port='COM4',        # 改成你的
    baudrate=115200,    # 必须和 STM32 一致
    bytesize=8,
    parity='N',
    stopbits=1,
    timeout=0.005
)

# 计算 CRC-16 
def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 0x0001:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF

def recvUartFrame(ser):
    frame = bytearray()
    while True:
        chunk = ser.read(256)   # 读当前缓冲
        if chunk:
            frame += chunk
        else:
            # timeout 触发：说明总线空闲了一段时间
            if frame:
                return frame

def sendData(ser, msg: bytes):
    crc = crc16_modbus(msg)
    msg_with_crc = msg + crc.to_bytes(2, byteorder='little')
    ser.write(msg_with_crc)
    print("Sent frame:", msg_with_crc)

# 你在 PC 上调试时可以用这个 main 做命令行对弈
if __name__ == "__main__":
    ai = WhiteAI(depth=4)
    ai.state.printBoard()
    while True:
        frame = recvUartFrame(ser)
        print("Frame received:", frame)
        if frame:
            print("Received frame:", frame)
            if len(frame) >= 3:  # 至少要有数据和 CRC
                # 检查校验码
                data = frame[:-2]
                received_crc = int.from_bytes(frame[-2:], byteorder='little')
                calculated_crc = crc16_modbus(data)
                if received_crc != calculated_crc:
                    print("CRC check failed. Received:", received_crc, "Calculated:", calculated_crc)
                    continue
                print("CRC check passed. Data:", data)

                # 解析收到的数据帧
                s = data.decode('utf-8').strip()
                if s == "RESET":
                    ai = WhiteAI(depth=4)
                    print("Game reset.")
                    ai.state.printBoard()
                    continue
                
                x1, y1, x2, y2 = data
                ai.apply_opponent_move(x1, y1, x2, y2)
                print(f"BLACK> {x1} {y1} {x2} {y2}")

                #发送应对数据帧
                x1, y1, x2, y2 = ai.choose_move()
                msg = bytes([x1, y1, x2, y2])
                sendData(ser, msg)
                print(f"WHITE> {x1} {y1} {x2} {y2}")

                ai.state.printBoard()

        # if ai.state.is_terminal():
        #     print("Game Over. Winner:", ai.state.winner())
        #     break

        # if ai.state.turn == BLACK:
        #     s = input("BLACK> ").strip()
        #     x1, y1, x2, y2 = map(int, s.split())
        #     ai.apply_opponent_move(x1, y1, x2, y2)
        # else:
        #     x1, y1, x2, y2 = ai.choose_move()
        #     print(f"WHITE> {x1} {y1} {x2} {y2}")
        # ai.state.printBoard()
