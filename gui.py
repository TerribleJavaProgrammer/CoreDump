import tkinter as tk
import build.core_dump_py as cd
from build.core_dump_py import Color, PieceType, Move, Position

cd.engine_init()

PIECE_SYMBOLS = {
    'P': '♙', 'N': '♘', 'B': '♗', 'R': '♖', 'Q': '♕', 'K': '♔',
    'p': '♟', 'n': '♞', 'b': '♝', 'r': '♜', 'q': '♛', 'k': '♚'
}

def square_to_index(rc: tuple[int, int]):
    """
    Args:
        row (int): Row number (0-7, from top to bottom)
        col (int): Column number (0-7, from left to right)
    
    Returns:
        int: Index from 0 to 63
    """
    return (7 - rc[0]) * 8 + rc[1]

def index_to_square(index: int):
    """
    Args:
        index (int): Board index (0-63)
    
    Returns:
        tuple: (row, col) coordinates (0-7)
    """
    return (7 - (index // 8), index % 8)

class ChessBoard:
    def __init__(self, root):
        self.root = root
        self.root.title("Chess Board")
        self.board_frame = tk.Frame(self.root)
        self.board_frame.pack()
        self.board_size = 8
        self.squares = {}
        self.selected_square = None
        self.position = Position()
        self.current_player = Color.WHITE
        self.human_color = Color.WHITE
        self.full_move_counter = 1
        self.halfmove_clock = 0
        self.pgn = ""
        self.create_board()
        self.update_board()

    def create_board(self):
        for row in range(self.board_size):
            for col in range(self.board_size):
                color = "white" if (row + col) % 2 == 0 else "gray"
                square = tk.Label(
                    self.board_frame,
                    bg=color,
                    width=4,
                    height=2,
                    relief="solid",
                )
                square.grid(row=row, column=col)
                square.bind("<Button-1>", lambda e, r=row, c=col: self.on_square_click(r, c))
                self.squares[(row, col)] = square

    def on_square_click(self, row: int, col: int):
        if self.selected_square:
            self.try_move_piece(self.selected_square, (row, col))
            self.selected_square = None
        else:
            # Select the clicked square
            self.selected_square = (row, col)
        self.update_board()

    def try_move_piece(self, from_square: tuple[int, int], to_square: tuple[int, int]):
        # Create a move object
        move = Move(square_to_index(from_square), square_to_index(to_square), self.current_player)

        # Check if the move is legal
        (legal, move) = self.is_legal_move(move)
        if not legal:
            print("Illegal move!")
            return False

        # Make the move
        self.position.make_move(move)

        # Update game state
        self.current_player = Color.BLACK if self.current_player == Color.WHITE else Color.WHITE
        if self.current_player == Color.WHITE:
            self.full_move_counter += 1

        return True
    
    def is_legal_move(self, move):
        legal_moves = cd.generate_moves(self.position, self.current_player)
        legal_move = next((m for m in legal_moves if m.from_square == move.from_square and m.to_square == move.to_square), None)
        if (legal_move is None):
            return False, None
        return True, legal_move

    def get_fen(self):
        # TODO add castling rights and en-passant target to the fen
        return self.position.get_fen(self.current_player, self.halfmove_clock, self.full_move_counter, "-", "-")

    def update_board(self):
        """
        Updates the board by fetching the current state and highlighting the selected square.
        """
        # Fetch the current FEN position
        fen = self.get_fen()
        print(fen)

        # Reset square colors
        for (row, col), square in self.squares.items():
            color = "white" if (row + col) % 2 == 0 else "gray"
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