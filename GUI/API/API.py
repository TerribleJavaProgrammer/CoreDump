import subprocess
import threading
import queue

class EngineInteraction:
    def __init__(self, engine_path):
        self.engine_path = engine_path
        self.process = None
        self.response_queue = queue.Queue()
        self.player_is_white = True

        try:
            self.process = subprocess.Popen(
                self.engine_path,
                stdin = subprocess.PIPE,
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE,
                text = True,
                bufsize = 1,
                encoding = 'utf-8'
            )
            print("subprocess started successfully")
        except FileNotFoundError:
            raise FileNotFoundError(f"Engine executable not found at {engine_path}")
        except Exception as e:
            raise RuntimeError(f"Error starting the engine: {e}")

        self.response_thread = threading.Thread(target = self._read_responses)
        self.response_thread.daemon = True
        print("response thread starting")
        self.response_thread.start()

    def _read_responses(self):
        while True:
            response = self.process.stdout.readline().strip()
            print(f"engine response: {response}")
            if response:
                self.response_queue.put(response)

    def send_command(self, command):
        try:
            print("about to send " + command)
            self.process.stdin.write(command + "\n")
            self.process.stdin.flush()
            print(command + " sent!")
        except BrokenPipeError:
            print(f"Error writing to the engine's stdin: {command}")
            self.close()

    def get_response(self):
        try:
            response = self.response_queue.get(timeout = 5)
            return response
        except queue.Empty:
            return None

    def init_game(self, is_white):
        self.player_is_white = is_white
        self.send_command(f"init {'white' if is_white else 'black'}")
        response = self.get_response()
        print(f"Engine response to initialization: {response}")
        return response == "initSuccess" if response else False

    def make_move(self, from_square, to_square):
        self.send_command(f"move {from_square} {to_square}")
        response = self.get_response()
        print(response)
        return response == "moveMade" if response else False

    def fetch_state(self):
        self.send_command("fetchState")
        response = self.get_response()
        print(response)
        return response if response else None

    def bot_move(self):
        self.send_command("botMove")
        response = self.get_response()
        print(response)
        return response == "botMoved" if response else False

    def is_mate(self):
        self.send_command("isMate")
        response = self.get_response()
        print(response)
        if response == "whiteWins":
            return "whiteWins"
        elif response == "blackWins":
            return "blackWins"
        elif response == "staleMate":
            return "staleMate"
        return None

    def get_moves(self, square):
        self.send_command(f"getMoves {square}")
        response = self.get_response()
        print(response)
        if response and response.startswith("moves"):
            return [int(move) for move in response.split()[1:]]
        return []

    def close(self):
        if self.process.poll() is None:
            self.send_command("quit")
            self.process.terminate()
            self.process.wait()
