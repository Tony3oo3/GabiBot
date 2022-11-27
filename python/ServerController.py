import json
import struct

from utils import recvall
from utils import convert_board
from gabibbot import State
from gabibbot import Player
from gabibbot import create_state_from_str


# TODO refactor with C++ module

# This class is the interface between the server and the python bot
class ServerController():

    def __init__(self, sock):
        self.sock = sock

    # Syncs the board
    def sync_board(self) -> State:
        try:
            len_bytes = struct.unpack('>i', recvall(self.sock, 4))[0]
        except TypeError:
            return None
        
        current_state_server_bytes = self.sock.recv(len_bytes)
        json_board = json.loads(current_state_server_bytes)
        str_board = convert_board(json_board["board"])
        turn =  Player.White if json_board["turn"] == "WHITE" else Player.Black
        print(turn)
        return create_state_from_str(str_board, turn)

    # Send a move to the server
    def send_move(self, m: tuple[str, str], player: str):
        move_for_server = '{"from":"%s","to":"%s","turn":"%s"}' % (m[0], m[1], player)
        self.sock.send(struct.pack('>i', len(move_for_server)))
        self.sock.send(move_for_server.encode())
