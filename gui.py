import tkinter as tk
from tkinter import messagebox
import build.core_dump_py as cd
from build.core_dump_py import Color, Move, Position

cd.engine_init()

PIECE_SYMBOLS = {
    'P': '♙', 'N': '♘', 'B': '♗', 'R': '♖', 'Q': '♕', 'K': '♔',
    'p': '♟', 'n': '♞', 'b': '♝', 'r': '♜', 'q': '♛', 'k': '♚'
}

MAX_DEPTH = 4
MAX_TIME = 5
HUMAN_COLOR = Color.WHITE

def square_to_index(rc: tuple[int, int]):
    return (7 - rc[0]) * 8 + rc[1]

def index_to_square(index: int):
    return (7 - (index // 8), index % 8)

def get_square_color(row: int, col: int):
    return "white" if (row + col) % 2 == 0 else "gray"

class ChessBoard:
    def __init__(self, root):
        self.root = root
        self.root.title("Chess Board")
        self.board_frame = tk.Frame(self.root)
        self.board_frame.pack()
        self.squares = {}
        self.selected_square = None
        self.game_over = None
        self.lock_for_engine = False
        self.position = Position()
        self.current_player = Color.WHITE
        self.human_color = HUMAN_COLOR
        self.full_move_counter = 1
        self.halfmove_clock = 0
        self.pgn = "1. "
        self.create_board()
        self.update_board()
        if (self.human_color == Color.BLACK):
            self.yield_to_engine()

    def create_board(self):
        for row in range(8):
            for col in range(8):
                color = get_square_color(row, col)
                square = tk.Label(
                    self.board_frame,
                    bg=color,
                    width=3,
                    height=1,
                    relief="solid",
                    font=("TkDefaultFont", 48),
                )
                square.grid(row=row, column=col)
                square.bind("<Button-1>", lambda e, r=row, c=col: self.on_square_click((r, c)))
                self.squares[(row, col)] = square

    def on_square_click(self, clicked_square: tuple[int, int]):
        if self.game_over:
            return
        if self.lock_for_engine:
            return

        if self.selected_square == clicked_square:
            # clicking twice on a square deselects it
            self.selected_square = None
            self.update_board()
            return

        if self.selected_square:
            # there is already a selected square, so we try to move from there to clicked square
            from_square = self.selected_square
            self.selected_square = None
            success = self.try_move_piece(square_to_index(from_square), square_to_index(clicked_square))
            if self.game_over:
                return
            if success:
                # if player successfully moved, then the bot moves
                self.yield_to_engine()                
            else:
                # player attempted illegal move
                if self.get_moves_from(clicked_square):
                    # if the newly clicked square has moves from it, select it
                    self.selected_square = clicked_square
                else:
                    self.selected_square = None
                self.update_board()

        else:
            # there is no square already selected
            moves_from_clicked = self.get_moves_from(clicked_square)
            if moves_from_clicked:
                # there are valid moves from the clicked square so we select it
                self.selected_square = clicked_square
                self.update_board()

    def try_move_piece(self, from_idx: int, to_idx: int):
        # Create a move object
        move = Move(from_idx, to_idx, self.current_player)

        # Check if the move is legal, and copy the new legalized move if it is
        (legal, move) = self.is_legal_move(move)
        if not legal:
            return False

        self.make_move(move)
        return True

    def make_move(self, move: Move):
        # Make the move
        self.position.make_move(move)
        self.pgn += move.get_pgn() + ' '
        print(f"{self.current_player.to_string()} plays: {self.get_algebraic_move(move)}")

        # Update game state
        self.current_player = Color.BLACK if self.current_player == Color.WHITE else Color.WHITE
        if self.current_player == Color.WHITE:
            self.full_move_counter += 1
            self.pgn += str(self.full_move_counter) + '. '
        
        # print for debugging
        print(self.pgn)
        print(self.get_fen())

        self.update_board()
        self.check_for_endgame()

    def check_for_endgame(self):
        status = cd.check_endgame_conditions(self.position, self.current_player)
        if status == 1:
            print(f"{self.current_player.to_string()} in CHECK!")
        elif status == 2:
            winner = cd.invert_color(self.current_player).to_string()
            self.game_over = f"CHECKMATE! {winner} wins."
        elif status == 3:
            self.game_over = "STALEMATE! Nobody wins"

        if self.game_over:
            print(self.game_over)
            messagebox.showinfo("Game Over", self.game_over)

    def yield_to_engine(self):
        self.lock_for_engine = True
        print("Bot is thinking...")
        move, debugOut = cd.find_best_move(self.position, self.current_player, MAX_DEPTH, MAX_TIME, True)
        print(debugOut)
        self.make_move(move)
        self.lock_for_engine = False

    def get_algebraic_move(self, move: Move):
        return f"{Move.to_algebraic(move.from_square)} {Move.to_algebraic(move.to_square)}"

    def is_legal_move(self, move: Move):
        legal_moves = cd.generate_moves(self.position, self.current_player)
        legal_move = next((m for m in legal_moves if m.from_square == move.from_square and m.to_square == move.to_square), None)
        if legal_move is None:
            return False, None
        return True, legal_move
    
    def get_moves_from(self, square: tuple[int, int]):
        legal_moves = cd.generate_moves(self.position, self.current_player)
        return [m for m in legal_moves if m.from_square == square_to_index(square)]

    def get_fen(self):
        # TODO add castling rights and en-passant target to the fen
        return self.position.get_fen(self.current_player, self.halfmove_clock, self.full_move_counter, "-", "-")

    def update_board(self):
        # Reset square colors
        for (row, col), square in self.squares.items():
            color = get_square_color(row, col)
            square.config(bg=color)

        # set the selected piece's color
        if self.selected_square:
            row, col = self.selected_square
            self.squares[(row, col)].config(bg="light green")

        # Populate squares with pieces
        for idx in range(64):
            row, col = index_to_square(idx)
            piece_char = self.position.get_square_char(idx)
            if piece_char != '.':
                self.squares[(row, col)].config(text=PIECE_SYMBOLS[piece_char])
            else:
                self.squares[(row, col)].config(text='')

def main():
    root = tk.Tk()
    chess_board = ChessBoard(root)
    root.mainloop()

if __name__ == "__main__":
    main()
