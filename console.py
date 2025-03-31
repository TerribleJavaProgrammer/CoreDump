import build.core_dump_py as cd
from build.core_dump_py import Color, PieceType, Move, Position

# Initialize important setup, like magic bitboards
cd.engine_init()

MAX_DEPTH = 3
MAX_TIME = 5
DEBUG = False
USE_HUMAN = False
CHOOSE_RANDOMLY = False

def make_human_turn(current_position, current_player):
    print("\nCommands:")
    print("- Make move: e2 e4")
    # print("- Show moves for square: moves e2")
    print("- Quit: quit> ")
    input_str = input().strip()

    if input_str == "quit":
        print("Quitting the game")
        return -1, None

    # Handle move command
    if len(input_str) != 5 or input_str[2] != ' ':
        print("Invalid move format. Use 'e2 e4'")
        return 1, None

    # Create move to check legality
    move = Move(input_str, current_player)

    legal_moves = cd.generate_moves(current_position, current_player)
    legal_move = next((m for m in legal_moves if m.from_square == move.from_square and m.to_square == move.to_square), None)

    if legal_move is None:
        print("Illegal move.")
        return 1, None

    move = legal_move  # Copy extra properties from legal move

    # Handle pawn promotion
    if move.is_promotion:
        print("Choose promotion piece Q(ueen), R(ook), B(ishop), k(N)ight: ", end="")
        choice = input().strip().upper()
        pieceChoice = PieceType.NONE
        try:
            pieceChoice = cd.get_promotion_piece(choice)
            if pieceChoice == PieceType.NONE:
                raise ValueError
        except ValueError:
            print("Invalid promotion piece. Defaulting to Queen.")
            pieceChoice = PieceType.QUEEN
        finally:
            print(f"Promoting to {pieceChoice}")
            move.set_promotion_piece(pieceChoice)

    return 0, move  # Valid move

# result (0 for continue, -1 for quit), current_position, current_player, full_move_counter, halfmove_clock, pgn
def game_loop(current_position, current_player, human_color, full_move_counter, halfmove_clock, pgn):
    print("\n====================")
    print(f"Move {full_move_counter}, {current_player.to_string()} to move")

    status = cd.check_endgame_conditions(current_position, current_player)
    if status == 1:
        print("CHECK!")
    elif status == 2:
        print(f"CHECKMATE! {cd.invert_color(current_player).to_string()} wins.")
        return -1, current_position, current_player, full_move_counter, halfmove_clock, pgn
    elif status == 3:
        print("STALEMATE! Nobody wins")
        return -1, current_position, current_player, full_move_counter, halfmove_clock, pgn

    # TODO add more endgame conditions
    # elif cd.is_insufficient_material(current_position):
    #     print("INSUFFICIENT MATERIAL!")
    #     break
    # elif cd.is_threefold_repetition(current_position):
    #     print("THREEFOLD REPETITION!")
    #     break
    # elif cd.is_fifty_move_rule(current_position):
    #     print("FIFTY MOVE RULE!")
    #     break

    print("\033[7m")
    print(current_position.display_position(), end="")
    print("\033[0m") # Reset color
    print(current_position.get_fen(current_player, halfmove_clock, full_move_counter, "", ""))
    print(pgn)

    # Determine if it's human's turn
    is_human_turn = (current_player == human_color) and USE_HUMAN
    if is_human_turn:
        quit_game = False
        while True:
            status, move = make_human_turn(current_position, current_player)
            if status == 0:
                break  # Move was made successfully
            elif status == -1:
                quit_game = True
                break
        if quit_game:
            return -1, current_position, current_player, full_move_counter, halfmove_clock, pgn
    else:
        # AI's turn
        print("AI is thinking...")
        if CHOOSE_RANDOMLY:
            move = cd.find_random_move(current_position, current_player)
        else:
            move, debugOut = cd.find_best_move(current_position, current_player, MAX_DEPTH, MAX_TIME, DEBUG)
            if (DEBUG):
                print(debugOut)
        # Convert move to algebraic notation for display
        move_str = f"{Move.to_algebraic(move.from_square)} {Move.to_algebraic(move.to_square)}"
        print(f"\033[41mComputer plays: {move_str} \033[0m")

    # Make the selected move
    current_position.make_move(move)

    # Update PGN
    if current_player == Color.WHITE:
        pgn += f"{full_move_counter}."
    else:
        full_move_counter += 1
    pgn += f"{move.get_pgn()} "

    # Switch to next player
    current_player = cd.invert_color(current_player)
    return 0, current_position, current_player, full_move_counter, halfmove_clock, pgn

current_position = Position()
current_player = Color.WHITE
human_color = Color.WHITE
full_move_counter = 1
halfmove_clock = 0
pgn = ""

# Let player choose color
choice = input("Play as (w)hite or (b)lack? ").strip().lower()
if choice == 'q':
    print("Quitting the game")
    exit(0)
if choice == 'b':
    human_color = Color.BLACK

# Print out who plays who
print()
print(f"You are playing as {(human_color.to_string())}")
print(f"AI is playing as {(cd.invert_color(human_color).to_string())}")
if human_color == Color.WHITE:
    print("You play first")
else:
    print("AI plays first")

# Main game loop
while True:
    result, current_position, current_player, full_move_counter, halfmove_clock, pgn = game_loop(current_position, current_player, human_color, full_move_counter, halfmove_clock, pgn)
    if result == -1:
        break  # Game ended

print("\nGame ended.")
print("PGN:")
print(pgn)
