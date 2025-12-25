from __future__ import annotations
from dataclasses import dataclass
from typing import List, Tuple, Optional, Iterable
import math

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
        """执行一个（已保证合法的）移动，并按规则处理横向吃子。"""
        (x1, y1), (x2, y2) = mv
        p = self.turn
        q = other(p)

        # 转成可变
        b = [list(row) for row in self.board]

        # 移动前，用于规则⑤判断
        row_before = b[y2].copy()  # 只需要看落子所在那一行的“移动前”

        # move
        b[y1][x1] = EMPTY
        b[y2][x2] = p

        # 吃子处理（只看横线）
        captures = self._compute_row_captures(
            board=b,
            moved_to=(x2, y2),
            moved_piece=p,
            row_before=row_before
        )
        for (cx, cy) in captures:
            b[cy][cx] = EMPTY

        return GameState(tuple(tuple(r) for r in b), q)

    @staticmethod
    def _compute_row_captures(board: List[List[str]],
                              moved_to: Coord,
                              moved_piece: str,
                              row_before: List[str]) -> List[Coord]:
        """
        按规则④⑤⑥：
        - 在任意一行 y，任意连续三格 (x..x+2) 若都非空且是两同一异，则默认吃掉孤子
        - 若该三格属于“连续四格都非空”的一段（规则⑥），则不吃
        - 若将被吃掉的孤子 == 本步刚移动的那颗子，且移动前该行仅存在对方两个相邻子、无其他子（规则⑤），则免死
        """
        mx, my = moved_to
        enemy = other(moved_piece)

        def row_has_only_two_adjacent_enemy(row: List[str], enemy_piece: str) -> bool:
            # 行里只有两个 enemy_piece，且它们相邻；其他全空
            idx = [i for i, c in enumerate(row) if c == enemy_piece]
            if len(idx) != 2:
                return False
            if idx[1] != idx[0] + 1:
                return False
            # 不能有己方子或更多敌子
            for c in row:
                if c != EMPTY and c != enemy_piece:
                    return False
            return True

        # 规则⑤免死判定：只对“本步移动的那颗子”生效
        suicide_immunity = False
        if row_has_only_two_adjacent_enemy(row_before, enemy_piece=enemy):
            # 你是“主动走进”对方双子所在行
            if my >= 0:  # 占位，逻辑上就是落点那一行
                suicide_immunity = True

        captures: List[Coord] = []

        y = my
        row = board[y]

        # helper：判断某个 x 是否处于“连续四格都非空”的窗口里
        def in_nonempty_run_of_4(x: int, y: int) -> bool:
            r = board[y]
            # 可能的4窗口起点：x-3 .. x
            for s in range(x - 3, x + 1):
                if 0 <= s <= W - 4:
                    if all(r[s + k] != EMPTY for k in range(4)):
                        return True
            return False

        # 为了不漏：严格来说任何行都可能因本步变化触发吃子，
        # 但变化只发生在 y1/y2 两行。这里为了简单且稳妥：全盘扫描每一行。
        # 5x5 很小，成本忽略不计。
        for yy in range(H):
            r = board[yy]
            for x in range(W - 2):
                a, b, c = r[x], r[x + 1], r[x + 2]
                if a == EMPTY or b == EMPTY or c == EMPTY:
                    continue
                # 两同一异
                if len({a, b, c}) != 2:
                    continue

                # 规则⑥：若这三格中的“孤子”处在连续四子非空窗口里，则不吃
                # （更保守：若该三格任意位置属于4连非空窗口，就直接不触发）
                if any(in_nonempty_run_of_4(x + k, yy) for k in range(3)):
                    continue

                # 找孤子位置
                if a != b and b == c:
                    lone_pos = (x, yy)          # a 是孤子
                elif c != b and a == b:
                    lone_pos = (x + 2, yy)      # c 是孤子
                else:
                    lone_pos = (x + 1, yy)      # b 是孤子（形如 ABA）
                    # 题意主要强调 BBW/WBB，这里也允许 ABA 触发（仍满足“两同一异”）
                    # 如果你确定不该吃 ABA，把这段改成 continue 即可。

                # 规则⑤：若孤子就是本步移动的子，且满足“主动走进对方双子行”，免死
                if lone_pos == (mx, my) and suicide_immunity:
                    continue

                captures.append(lone_pos)

        # 去重（同一子可能被多个三连同时判定）
        captures = list(dict.fromkeys(captures))
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
    
# 你在 PC 上调试时可以用这个 main 做命令行对弈
if __name__ == "__main__":
    ai = WhiteAI(depth=4)


    ai.state.printBoard()
    print("输入黑方走法：x1 y1 x2 y2（例如：0 0 1 0）")

    while True:
        if ai.state.is_terminal():
            print("Game Over. Winner:", ai.state.winner())
            break

        if ai.state.turn == BLACK:
            s = input("BLACK> ").strip()
            x1, y1, x2, y2 = map(int, s.split())
            ai.apply_opponent_move(x1, y1, x2, y2)
        else:
            x1, y1, x2, y2 = ai.choose_move()
            print(f"WHITE> {x1} {y1} {x2} {y2}")
        ai.state.printBoard()
