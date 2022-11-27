from gabibbot import Piece
from gabibbot import Move

# Helper function to recv n bytes or return None if EOF is hit
def recvall(sock, n):
    data = b''
    while len(data) < n:
        packet = sock.recv(n - len(data))
        if not packet:
            return None
        data += packet
    return data

# Convert from json board to txt board
def convert_board(json_board) -> list[str]:
    cell_mapping = {"EMPTY": "_", "WHITE": "W", "BLACK": "B", "KING": "K", "THRONE": "_"}
    tmp_mat = []
    for row in json_board:
        tmp_mat.append(list(map(lambda x: cell_mapping[x], row)))
    return [''.join(row) for row in tmp_mat]

def convert_move(move: Move) -> str:
    fr = move.get_from()
    fr = str(chr(ord(fr[1]) + 49)) + str(chr(ord(fr[0]) + 1))
    to = move.get_to()
    to = str(chr(ord(to[1]) + 49)) + str(chr(ord(to[0]) + 1))
    return (fr, to) 