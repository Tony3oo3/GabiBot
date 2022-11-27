import socket
import struct
import sys
import time

from ServerController import ServerController
from gabibbot import GabibBot
from gabibbot import Player
from utils import convert_move

player_name = "GABIBBOT"

# from GabibBot import GabibBot


def start_game(player: Player, timeout: int, ip_addr: str):

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Initialize the connection with the server
        port = 5800 if player == Player.White else 5801
        print(port)
        sock.connect((ip_addr, port))
        sock.send(struct.pack('>i', len(player_name)))
        sock.send(player_name.encode())
        print("OK")
        # Start the game loop
        main_game_loop(sock, player, timeout)


def main_game_loop(sock, player: Player, timeout: int):
    controller = ServerController(sock)
    bot = GabibBot()
    time.sleep(1)
    while True:
        state = controller.sync_board()
        if state is None: return
        if(state.get_player() == player):
            
            state.print_board()

            move = bot.min_max_iterative_d(state, int(timeout - 2))

            print(move.get_from() + " " + move.get_to())
            print(convert_move(move))

            controller.send_move(convert_move(move), player)


if __name__ == "__main__":
    # Parse args
    color_arg = sys.argv[1].lower()
    timeout_arg = float(sys.argv[2])
    ip_addr_arg = sys.argv[3]

    # Check args
    if color_arg != "white" and color_arg != "black":
        raise Exception("Player color can only be 'white' or 'black'")

    player =  Player.White if color_arg == "white" else Player.Black
    # Initialize the game
    start_game(player, timeout_arg, ip_addr_arg)
