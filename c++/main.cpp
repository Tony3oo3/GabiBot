#include "gabibbot.cpp"

void pvp(State& current_state){
    using namespace std;

    while(not current_state.is_winning(Black) and not current_state.is_winning(White)){
        cout << "Current turn: " << current_state.current_player << endl;
        string fr_s, to_s;
        cin >> fr_s;
        cin >> to_s;
        coordinate from(fr_s[0] - 48, fr_s[1] - 48);
        coordinate to(to_s[0] - 48, to_s[1] - 48);
        Move m(from, to);
        if(current_state.check_move(m)) {
            current_state = current_state.execute_move(m);
            current_state.print_board();
        }
    }
}

void pvb(State& current_state, int max_depth, int timeout_s, Player human_start){
    using namespace std;
    GabibBot bot;
    if(human_start == Black) {
        Move first_move = bot.min_max_iterative_d(current_state, timeout_s);
        cout << "Bot move: " << first_move.get_string() << endl;
        current_state = current_state.execute_move(first_move);
        current_state.current_player = next_player(current_state.current_player);
        current_state.print_board();
    }

    while(not current_state.winning_b and not current_state.winning_w){
        cout << "Current turn: " << current_state.current_player << endl;
        string fr_s, to_s;
        cin >> fr_s;
        cin >> to_s;
        coordinate from(fr_s[0] - 48, fr_s[1] - 48);
        coordinate to(to_s[0] - 48, to_s[1] - 48);
        Move m(from, to);
        if(current_state.check_move(m)) {
            current_state = current_state.execute_move(m);
            current_state.current_player = next_player(current_state.current_player);
            current_state.print_board();

            Move bot_move = bot.min_max_iterative_d(current_state, timeout_s);
            cout << "Bot move: " << bot_move.get_string() << endl;
            current_state = current_state.execute_move(bot_move);
            current_state.current_player = next_player(current_state.current_player);
            current_state.print_board();
        }
    }
    current_state.print_board();
}

int main(){
    using namespace std;

    vector<string> input(9);
    for(int i = 0; i < 9; i++)
        cin >> input[i];

    State current_state = create_state_from_str(input, White);

    current_state.print_board();
    GabibBot bot;
    Move best_move = bot.min_max_iterative_d(current_state, 50);
    cout << best_move.get_string() << endl;
    // cout << tot_s << endl;

    // State next = current_state.execute_move(Move(coordinate(6, 5), coordinate(7, 5)));
    // next.print_board();

    // pvp(current_state);
    // pvb(current_state, 2, 50, Black);

    return 0;
}
