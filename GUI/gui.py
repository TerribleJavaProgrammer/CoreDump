import sys
import os
import tkinter as tk
import platform
from tkinter import messagebox, ttk

sys.path.append(os.path.join(os.path.dirname(__file__), 'API'))

from API import EngineInteraction

class ChessBoard:
    PIECE_SYMBOLS = {
        'P': '♙', 'N': '♘', 'B': '♗', 'R': '♖', 'Q': '♕', 'K': '♔',
        'p': '♟', 'n': '♞', 'b': '♝', 'r': '♜', 'q': '♛', 'k': '♚'
    }

    def __init__(self, master, engine_path):
        self.master = master
        self.master.title("Chess Engine")
        self.master.geometry("600x700")

        self.engine_path = engine_path
        self.engine = None
        self.player_color = True
        self.game_active = False

        self.selected_square = None

        self.create_widgets()
        self.setup_board()

    def create_widgets(self):
        color_frame = ttk.Frame(self.master)
        color_frame.pack(pady = 10)

        ttk.Label(color_frame, text = "Choose Your Color:").pack(side = tk.LEFT, padx = 5)
        self.color_var = tk.StringVar(value = "white")
        white_radio = ttk.Radiobutton(color_frame, text = "White", variable = self.color_var, value = "white")
        black_radio = ttk.Radiobutton(color_frame, text = "Black", variable = self.color_var, value = "black")
        white_radio.pack(side = tk.LEFT, padx = 5)
        black_radio.pack(side = tk.LEFT)

        self.start_button = ttk.Button(self.master, text = "Start Game", command = self.start_game)
        self.start_button.pack(pady = 10)

        self.board_frame = ttk.Frame(self.master)
        self.board_frame.pack(padx = 20, pady = 20)

        self.status_label = ttk.Label(self.master, text = "", font = ('Arial', 12))
        self.status_label.pack(pady = 10)

    def setup_board(self):
        self.squares = []
        for row in range(8):
            row_squares = []
            for col in range(8):
                bg_color = "white" if (row + col) % 2 == 0 else "gray"
                square = tk.Button(
                    self.board_frame, 
                    width = 5, 
                    height = 2, 
                    bg = bg_color, 
                    command = lambda r = row, c = col: self.on_square_click(r, c)
                )
                square.grid(row = row, column = col)
                row_squares.append(square)
            self.squares.append(row_squares)

    def start_game(self):
        if self.engine:
            self.engine.close()

        self.player_color = self.color_var.get() == "white"

        self.engine = EngineInteraction(self.engine_path)
        if not self.engine.init_game(self.player_color):
            messagebox.showerror("Error", "Failed to start the game")
            return

        self.game_active = True
        self.update_board()
        self.start_button.config(state = tk.DISABLED)

    def update_board(self):
        if not self.game_active:
            return

        board_state = self.engine.fetch_state()
        if not board_state:
            return

        rows = board_state.split('/')
        for row in range(8):
            for col in range(8):
                piece = rows[row][col]
                square = self.squares[row][col]
                
                square.config(text = self.PIECE_SYMBOLS.get(piece, ''))

    def on_square_click(self, row, col):
        if not self.game_active:
            return

        row = 7 - row
        col = col

        square_index = row * 8 + col

        if self.selected_square is None:
            self.selected_square = (row, col)
            
            moves = self.engine.get_moves(square_index)
            
            if not moves:
                return

            for square in self.squares:
                for button in square:
                    button.config(relief = tk.RAISED)

            for move in moves:
                dest_row = 7 - move // 8
                dest_col = move % 8
                self.squares[dest_row][dest_col].config(relief = tk.SUNKEN)

        else:
            source_row, source_col = self.selected_square
            source_index = source_row * 8 + source_col

            if self.engine.make_move(source_index, square_index):
                self.update_board()
                self.check_game_status()

                self.selected_square = None
                for row in self.squares:
                    for square in row:
                        square.config(relief = tk.RAISED)

                if self.game_active and not self.player_color:
                    self.make_bot_move()
            
            else:
                self.selected_square = None
                for row in self.squares:
                    for square in row:
                        square.config(relief = tk.RAISED)

    def make_bot_move(self):
        if not self.game_active:
            return

        if self.engine.bot_move():
            self.update_board()
            self.check_game_status()

    def check_game_status(self):
        status = self.engine.is_mate()
        if status:
            self.game_active = False
            if status == "whiteWins":
                messagebox.showinfo("Game Over", "White Wins!")
            elif status == "blackWins":
                messagebox.showinfo("Game Over", "Black Wins!")
            else:
                messagebox.showinfo("Game Over", "Stalemate!")
            
            self.start_button.config(state = tk.NORMAL)

def main():
    root = tk.Tk()
    os_name = platform.system()
    exe_path = "out/chess"
    if (os_name == "Windows"):
        exe_path += ".exe"
    print("Using api at " + exe_path)
    game = ChessBoard(root, exe_path)
    root.mainloop()

if __name__ == "__main__":
    main()
