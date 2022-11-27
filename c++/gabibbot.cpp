#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <thread>

using string = std::string;

enum Player : char {Black, White};
enum Piece : char {White_P = 'W', Black_P = 'B', King = 'K', Empty_P = '_'};
enum Tile: char {Castle = '+', Camp = '^', Escape = '*', Free = '_'};

using coordinate = std::pair<short, short>;
coordinate null_coord(-1, -1);

bool coord_in_range(coordinate c, int start = 0, int end = 9){
    return start <= c.first and c.first < end and start <= c.second and c.second < end;
}

coordinate coord_sum(coordinate c1, coordinate c2){
    return std::move(coordinate(c1.first + c2.first, c1.second + c2.second));
}

using tile_matrix = std::vector<std::vector<Tile>>;
const tile_matrix tile_board = {
        {Free, Escape, Escape, Camp, Camp, Camp, Escape, Escape, Free},
        {Escape, Free, Free, Free, Camp, Free, Free, Free, Escape},
        {Escape, Free, Free, Free, Free, Free, Free, Free, Escape},
        {Camp, Free, Free, Free, Free, Free, Free, Free, Camp},
        {Camp, Camp, Free, Free, Castle, Free, Free, Camp, Camp},
        {Camp, Free, Free, Free, Free, Free, Free, Free, Camp},
        {Escape, Free, Free, Free, Free, Free, Free, Free, Escape},
        {Escape, Free, Free, Free, Camp, Free, Free, Free, Escape},
        {Free, Escape, Escape, Camp, Camp, Camp, Escape, Escape, Free}
};

Tile get_tile(coordinate coord){
    return tile_board[coord.first][coord.second];
}

Player next_player(Player current_player){
    return (current_player == White) ? Black : White;
}

class Move{
    public:
        coordinate from;
        coordinate to;

        Move(coordinate from_, coordinate to_):
            from(std::move(from_)),
            to(std::move(to_)) {}

        string get_string() const{
            return std::to_string(from.first) + std::to_string(from.second) + " " + std::to_string(to.first) + std::to_string(to.second);
        }

        string get_to() const{
            return std::to_string(to.first) + std::to_string(to.second);
        }
        string get_from() const{
            return std::to_string(from.first) + std::to_string(from.second);
        }
};

int tot_s = 0;

class State{
    private:

        Piece get_piece(coordinate coord){
            return board[coord.first][coord.second];
        }

        // used only in check move
        // check if the whole line is empty, x2 is not included in the interval
        // usually called x1 -> to, x2 -> from
        bool is_line_empty(coordinate x1, coordinate x2){
            // here we're not checking that the final tile is legal or not, just the tiles in between
            bool cross_camps = false;
            // only if the moved piece is black, and it's position is in a camp we can cross them
            if(get_piece(x2) == Black_P and get_tile(x2) == Camp)
                cross_camps = true;

            if(x1.first == x2.first) {
                // Same col
                int start = x2.second + 1;
                int end = x1.second;
                if (x1.second < x2.second){
                   start = x1.second;
                   end = x2.second;
                }
                for (int i = start; i < end; i++){
                    bool camp_cond = (not cross_camps) and tile_board[x1.first][i] == Camp;
                    if (board[x1.first][i] != Empty_P or tile_board[x1.first][i] == Castle or camp_cond)
                        return false;
                }
                return true;
            }else if(x1.second == x2.second){
                // Same row
                int start = x2.first + 1;
                int end = x1.first;
                if (x1.first < x2.first){
                    start = x1.first;
                    end = x2.first;
                }
                for(int i = start; i < end; i++) {
                    bool camp_cond = (not cross_camps) and tile_board[i][x1.second] == Camp;
                    if (board[i][x1.second] != Empty_P or tile_board[i][x1.second] == Castle or camp_cond)
                        return false;
                }
                return true;
            }
            // Returns false in case none of the coordinate is equal -> diag move
            return false;
        }

        void generate_moves_from_coord(std::vector<Move>& out_moves, coordinate start){
            bool b1 = false;
            bool b2 = false;
            bool b3 = false;
            bool b4 = false;

            for(int i = 1; i < 9; i++){
                if(start.first + i < 9 and not b1) {
                    coordinate c(start.first + i, start.second);
                    //if (check_move_l(c))
                    if(get_piece(c) == Empty_P and (get_tile(c) != Camp or get_tile(start) == Camp) and get_tile(c) != Castle)
                        out_moves.emplace_back(start, c);
                    else b1 = true;
                }
                if(start.first - i >= 0 and not b2) {
                    coordinate c(start.first - i, start.second);
                    //if (check_move_l(c))
                    if(get_piece(c) == Empty_P and (get_tile(c) != Camp or get_tile(start) == Camp) and get_tile(c) != Castle)
                        out_moves.emplace_back(start, c);
                    else b2 = true;
                }
                if(start.second + i < 9 and not b3) {
                    coordinate c(start.first, start.second + i);
                    //if (check_move_l(c))
                    if(get_piece(c) == Empty_P and (get_tile(c) != Camp or get_tile(start) == Camp) and get_tile(c) != Castle)
                        out_moves.emplace_back(start, c);
                    else b3 = true;
                }
                if(start.second - i >= 0 and not b4) {
                    coordinate c(start.first, start.second - i);
                    //if (check_move_l(c))
                    if(get_piece(c) == Empty_P and (get_tile(c) != Camp or get_tile(start) == Camp) and get_tile(c) != Castle)
                        out_moves.emplace_back(start, c);
                    else b4 = true;
                }
            }
        }

    public:
        using pieces_matrix = Piece[9][9];

        pieces_matrix board{};
        Player current_player;
        coordinate king_position;
        short white_p_n = 0;
        short black_p_n = 0;
        bool winning_w;
        bool winning_b;

        State(pieces_matrix  board_, Player current_turn_) :
                current_player(current_turn_)
            {
                tot_s++;
                king_position = coordinate(-1, -1);
                for(int i = 0;i < 9; i++)
                    for(int j = 0;j < 9; j++) {
                        board[i][j] = board_[i][j];
                        if(board[i][j] == King) king_position = coordinate(i, j);
                        if(board[i][j] == White_P) white_p_n++;
                        if(board[i][j] == Black_P) black_p_n++;
                    }

                winning_w = is_winning(White);
                winning_b = is_winning(Black);
            }

        void copy_matrix(pieces_matrix& dest){
            for(int i = 0; i<9; i++){
                for(int j = 0; j<9; j++){
                    dest[i][j] = board[i][j];
                }
            }
        }

        pieces_matrix* get_board(){
           return &board;
        }

        void print_board(const string& padding = "  "){
            std::cout << get_board_str(padding, "\n") << std::endl;
        }

        string get_board_str(const string& padding = "", const string& end_line = "") {
            string b;
            for (auto &row: board) {
                for (const Piece &p: row) {
                    char p_c = p;
                    b += string(1, p_c);
                    b += padding;
                }
                b += end_line;
            }
            return b;
        }

        // Working
        bool is_winning(Player player) const{
            if(player == Black){
                // if the king is not on the board black wins
                if(king_position.first == -1) return true;
                else return false;
            }
            if(player == White){
                // if the king is in one of the escape tiles white wins
                if(king_position.first != -1 && tile_board[king_position.first][king_position.second] == Escape)
                    return true;
            }
            return false;
        }

        static Piece get_piece_b(coordinate c, pieces_matrix mat){
            return mat[c.first][c.second];
        }

        // TODO test all the options
        bool is_king_captured(pieces_matrix& new_board){
            if(current_player == Black){
                coordinate up(3,4);
                coordinate down(5,4);
                coordinate left(4,3);
                coordinate right(4,5);

                if(get_tile(king_position) == Castle)
                    return get_piece_b(up, new_board) == Black_P and get_piece_b(down, new_board) == Black_P and
                            get_piece_b(left, new_board) == Black_P and get_piece_b(right, new_board) == Black_P;
                else{
                    coordinate up_k(king_position.first - 1, king_position.second);
                    coordinate down_k(king_position.first + 1, king_position.second);
                    coordinate left_k(king_position.first, king_position.second - 1);
                    coordinate right_k(king_position.first, king_position.second + 1);


                    if(king_position == up or king_position == down or king_position == left or king_position == right){
                        //adj to the castle capture
                        return (get_piece_b(up_k, new_board) == Black_P or get_tile(up_k) == Castle) and
                               (get_piece_b(down_k, new_board) == Black_P or get_tile(down_k) == Castle) and
                               (get_piece_b(left_k, new_board) == Black_P or get_tile(left_k) == Castle) and
                               (get_piece_b(right_k, new_board) == Black_P or get_tile(right_k) == Castle);
                    }else{
                        // normal capture
                        // TODO test the fix
                        bool row_cap = (not coord_in_range(up_k) or get_piece_b(up_k, new_board) == Black_P or get_tile(up_k) == Camp) and (not coord_in_range(down_k) or get_piece_b(down_k, new_board) == Black_P or get_tile(down_k) == Camp);
                        bool col_cap = (not coord_in_range(left_k) or get_piece_b(left_k, new_board) == Black_P or get_tile(left_k) == Camp) and (not coord_in_range(right_k) or get_piece_b(right_k, new_board) == Black_P or get_tile(right_k) == Camp);
                        return row_cap or col_cap;
                    }
                }
            }
            return false;
        }

        // We assume that the move is valid
        // This will not change the player of the state, has to be done manually
        State execute_move(const Move& m){

            coordinate from = m.from;
            coordinate to = m.to;

            pieces_matrix new_board;
            copy_matrix(new_board);

            // Execute the move
            new_board[to.first][to.second] = new_board[from.first][from.second];
            new_board[from.first][from.second] = Empty_P;

            // Check for captures
            // We will check all 4 nearest tiles of the moved piece (only 3 are needed but it complicates the code)

            // check for black capture
            coordinate directions[4]{
                        coordinate(1, 0),  // up
                        coordinate(-1, 0), // down
                        coordinate(0, 1),  // right
                        coordinate(0, -1)  // left
                    };

            for(auto& direction: directions){
                coordinate near_coord = coord_sum(to, direction);
                coordinate next_coord = coord_sum(near_coord, direction);

                if(coord_in_range(near_coord) and coord_in_range(next_coord)){
                    Piece near_piece = get_piece(near_coord);
                    Tile near_tile = get_tile(near_coord);
                    Piece next_piece = get_piece(next_coord);
                    Tile next_tile = get_tile(next_coord);

                    bool white_capture =
                            current_player == White and
                            near_piece == Black_P and
                            (next_piece == White_P or (next_tile == Camp and near_tile != Camp) or next_tile == Castle);

                    bool black_capture =
                            current_player == Black and
                            near_piece == White_P and
                            (next_piece == Black_P or next_tile == Camp or next_tile == Castle);

                    bool king_capture = get_piece(near_coord) == King and is_king_captured(new_board);

                    if(white_capture or black_capture or king_capture)
                        new_board[near_coord.first][near_coord.second] = Empty_P;
                }
            }

            State new_state(new_board, current_player);
            return new_state;
        }


        // Not used in the move generation
        // Rough testing done
        // This function will only check if the move can be done or not
        bool check_move(const Move& m){
            Piece moved_p = get_piece(m.from);
            Piece target_p = get_piece(m.to);
            Tile start_t = get_tile(m.from);
            Tile target_t = get_tile(m.to);

            // check for moving piece legality
            bool is_white_piece_legal = (moved_p == White_P or moved_p == King) and current_player == White;
            bool is_black_piece_legal = moved_p == Black_P and current_player == Black;

            // if white then target camps are forbidden in any case
            // if black target camps:
            //  - if the starting pos is not a camp -> forbidden
            //  - if the starting pos is a camp of the same group -> ok
            //  - if the starting pos is a camp of another group -> forbidden
            bool is_target_legal = target_p == Empty_P and target_t != Castle;
            bool is_white_target_legal = is_white_piece_legal and target_t != Camp;
            bool is_black_target_legal = is_black_piece_legal and (start_t == Camp or target_t != Camp);
            bool is_line_empty = this->is_line_empty(m.to, m.from);

            return is_target_legal and (is_white_target_legal or is_black_target_legal) and is_line_empty;
        }

        std::vector<Move> generate_all_moves(){
            std::vector<Move> possible_moves;
            // Iterate on all the board
            for(int i = 0; i < 9; i++){
                for(int j = 0; j < 9; j++){
                    coordinate curr_coord(i, j);
                    Piece p = get_piece(curr_coord);
                    bool white_moving = (p == White_P or p == King) and current_player == White;
                    bool black_moving = p == Black_P and current_player == Black;
                    if(white_moving or black_moving)
                        generate_moves_from_coord(possible_moves, curr_coord);
                }
            }

            return std::move(possible_moves);
        }

        Player get_player() const{
            return current_player;
        }
};

bool timeout = false;
void timeout_func(int max_time_s){
    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(max_time_s));
    std::cout << "TIMEOUT" << std::endl;
    timeout = true;
}

class GabibBot{
    public:
        Player max_pl;
        int current_depth;
        // Compute all possible moves starting from the initial state
        // We put them in a stack
        // We pop the first one and evaluate it + computing all the possible moves
        // We need to save the level, when we've reached the desired level we stop pushing
        // When the stack is empty we return the best move

        float max(State& prev, Move& m, int levels_left, int move_diff, float a, float b){
            State new_state = prev.execute_move(m);

            if(levels_left == 0 or new_state.winning_w or new_state.winning_b) return heuristics(new_state, levels_left, move_diff);
            new_state.current_player = next_player(new_state.current_player);

            std::vector<Move> all_moves = new_state.generate_all_moves();
            float best_score = -1000000.;
            for(Move& move : all_moves){
                if(timeout) return best_score;
                float tmp_score = min(new_state, move, levels_left - 1, all_moves.size() + move_diff, a, b);
                best_score = (tmp_score > best_score) ? tmp_score : best_score;

                // Alpha-Beta cut
                if(best_score >= b) return best_score;
                a = (a > best_score) ? a : best_score;
            }
            return best_score;
        }

        float min(State& prev, Move& m, int levels_left, int move_diff, float a, float b){
            State new_state = prev.execute_move(m);

            if(levels_left == 0 or new_state.winning_w or new_state.winning_b) return heuristics(new_state, levels_left, move_diff);
            new_state.current_player = next_player(new_state.current_player);

            std::vector<Move> all_moves = new_state.generate_all_moves();
            float best_score = 1000000.;
            for(Move& move : all_moves){
                if(timeout) return best_score;
                float tmp_score = max(new_state, move, levels_left - 1, move_diff - all_moves.size(), a, b);
                best_score = (tmp_score < best_score) ? tmp_score : best_score;

                // Alpha-Beta cut
                if(best_score <= a) return best_score;
                b = (b <= best_score) ? b : best_score;
            }
            return best_score;
        }

        float min_max(State& s, int max_depth, Move& best_move){
            max_pl = s.current_player;

            float a = -1000000.;
            float b = 1000000.;

            float best_score = -1000000.;
            std::vector<Move> all_moves = s.generate_all_moves();

            for(Move& move : all_moves){
                if(timeout) {
                    best_move = Move(null_coord, null_coord);
                    return best_score;
                }
                float tmp_score = min(s, move, max_depth-1, all_moves.size(), a, b);
                if(tmp_score > best_score){
                    best_score = tmp_score;
                    best_move = move;
                }

                if(best_score >= b){
                    return best_score;
                }

                a = (a > best_score) ? a : best_score;
            }
            return best_score;
        }



        Move min_max_iterative_d(State &s, int timeout_s){
            auto start = std::chrono::steady_clock::now();

            std::thread timer(timeout_func, timeout_s);

            Move null_move(null_coord, null_coord);
            Move best_move = null_move;

            current_depth = 1;

            while(not timeout){
                std::cout << "Current depth -> " << current_depth << std::endl;

                Move tmp_move = null_move;
                float best_score = min_max(s, current_depth, tmp_move);

                std::cout << "Number of expanded states -> " << tot_s << std::endl;
                tot_s = 0;

                if(not timeout){
                    best_move = tmp_move;
                    std::cout << "Best score found with " << best_move.get_string() << " -> " << best_score << std::endl;
                }else{
                    std::cout << "Depth -> " << current_depth << " timed out!" << std::endl;
                }

                current_depth++;
            }

            timer.join();
            std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
            std::cout << "Time taken: " <<  elapsed.count() << std::endl;
            timeout = false;

            return best_move;
        }


        float heuristics(State& s, int depth_left, int moves_diff){
            // +inf if max wins
            // -inf if min wins
            // greater the pawn diff is max is winning
            // opposite min is winning
            float my_win = 0;
            float my_loose = 0;
            float pawn_diff = 0;
            float king_outside_penalty = 0;
            float depth_boost = 0;
            float white_p_weighted = s.white_p_n * 1.5f;
            float black_p_weighted = s.black_p_n * 1.f;
            auto moves_diff_f = static_cast<float>(moves_diff);
            auto current_d_f = static_cast<float>(current_depth);

            Player min_pl = next_player(max_pl);

            if(max_pl == s.current_player){
                my_win = (max_pl == White and s.winning_w or max_pl == Black and s.winning_b) ? 100 : 0;
                my_loose = (max_pl == White and s.winning_b or s.current_player == Black and s.winning_w) ? -100 : 0;
                pawn_diff = (max_pl == White) ? white_p_weighted - black_p_weighted : black_p_weighted - white_p_weighted;
                if(s.king_position != coordinate(4,4))
                    king_outside_penalty = max_pl == White ? -1 : 1;
                depth_boost = (float) depth_left;
            }else{
                my_win = (min_pl == White and s.winning_w or min_pl == Black and s.winning_b) ? -100 : 0;
                my_loose = (min_pl == White and s.winning_b or min_pl == Black and s.winning_w) ? 100 : 0;
                pawn_diff = (min_pl == White) ? black_p_weighted - white_p_weighted : white_p_weighted - black_p_weighted;
                if(s.king_position != coordinate(4,4))
                    king_outside_penalty = min_pl == White ? 1 : -1;
                depth_boost = (float) -depth_left;
            }

            // Weights the parameters
            pawn_diff = pawn_diff * 1.f;
            king_outside_penalty = king_outside_penalty * 0.1f;
            depth_boost = depth_boost * 5.f; // increase this?
            // moves_diff_f = (moves_diff_f / current_d_f) * 0.05f; // change the weight
            moves_diff_f = moves_diff_f * 0.05f;

            return my_win + my_loose + pawn_diff + depth_boost + king_outside_penalty + moves_diff_f;
        }

};

// this assumes that the input is respecting the enum
State create_state_from_str(const std::vector<string>& mat, Player current_turn){
    State::pieces_matrix res;
    int i = 0;
    int j;
    for(auto& row : mat){
        j = 0;
        for(char el : row){
            res[i][j] = ((Piece) el);
            j++;
        }
        i++;
    }
    return {res, current_turn};
}
