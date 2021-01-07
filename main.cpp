#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <ctime>

// 注意：我們每回合都會給200000，所以如果你花100000拜票然後看到錢增加100000的話，這不是bug

enum State { // 這告訴我們現在在哪個狀況下 然後main 就會跑相對應的程式
    MENU, 
    NAME,
    DICE, // when the first player rolls the dice
    CITY, // should be <location type 1> state_1
    NEWS,
    WAIT,
    END,
  }state;

// Window consts
const int kWindowWidth = 1000;
const int kWindowHeight = 1000;

// Game Setting Consts
const int kPlayerNum = 2;
const int kTotalRounds = 10;
const int kLocationIndexNum = 28;
const int kStartMoney = 20000000;
const int kBribeNum = 10; // The starting value of bribe_day_
const int kMiaoliNum = 3; // The starting value of bribe_day_
const int kJailNum = 3;
const int kHospitalNum = 3;

// Const Earn Money Consts
const int kMoneyEachRound = 200000;
const int kMoneyEarnedAtStart = 500000;

// Const Spend Money Consts
const int kBaiPiaoSpendMoney = 100000;

// Random Consts
const int kDiceMin = 1;
const int kDiceMax = 6;
const int kDiceRange = kDiceMax - kDiceMin + 1;
const int kBaiPiaoVoteMin = 5;
const int kBaiPiaoVoteMax = 10; 
const int kBaiPiaoVoteRange = kBaiPiaoVoteMax - kBaiPiaoVoteMin + 1;
const int kSpeechGetMoneyMin = 100000;
const int kSpeechGetMoneyMax = 300000;
const int kSpeechGetMoneyRange = kSpeechGetMoneyMax - kSpeechGetMoneyMin + 1;

// Random Functions:
int RandomDice() {
  return rand() % kDiceRange + kDiceMin;
}

int RandomBaiPiaoVote() {
  return rand() % kBaiPiaoVoteRange + kBaiPiaoVoteMin;
}

int RandomSpeechGetMoney() {
  return rand() % kSpeechGetMoneyRange + kSpeechGetMoneyMin;
}


class Player {
  private:
  std::string name_;
  int player_index_;
  int location_index_;
  int money_;
  int votes_;
  int bribe_day_; // if the player meets the police when bribe_day_ > 0, he gets caught
  int miaoli_day_; // the days left until the player could go out of miaoli
  int jail_day_; // the days left until the player could go out of jail
  int hospital_day_; // the days left until the player could go out of hospital

  public:
  // Constructors and Destructors
  // - no Destructors, because no pointers exists.
  Player(std::string name, int player_index) {
    name_ = name;
    player_index_ = player_index;
    location_index_ = 0;
    money_ = kStartMoney;
    votes_ = 0;
    bribe_day_ = 0;
    miaoli_day_ = 0;
    jail_day_ = 0;
    hospital_day_ = 0;
  }


  // Accessors, Mutators, and Functions
  // Player name
  std::string get_player_name() const {
    return name_;
  }

  // Player Index
  int get_player_index() const {
    return player_index_;
  }

  // Location Index
  int get_location_index() const {
    return location_index_;
  }
  void UpdateLocationIndex(int steps) {
    location_index_ += steps;
    location_index_ %= kLocationIndexNum;
  }
  void TeleportToJail() {
    location_index_ = 21;
  }

  // Money
  // returns the money of the player
  int get_money() const {
    return money_;
  }
  
   // enter the amount you want to add or spend. Amount is positive if add, and negative if spend.
  void UpdateMoney(int amount) {
    money_ += amount;
  }

  // Votes
  // plz dont use this rn
  int get_votes() const {
    return votes_;
  }

  // Bribe
  // pass in true when today is bribe day. Otherwise, just put this code when a day passess.
  void UpdateBribeDay(bool is_today_bribe_day = false) { 
    if (is_today_bribe_day)
      bribe_day_ = kBribeNum;
    else
      bribe_day_ = std::max(bribe_day_ - 1, 0);
  }
  // When the police wants to check if you have bribed
  bool get_is_bribed() const { 
    return (bribe_day_ > 0) ? true : false;
  }
  
  // miaoli
  // updates the days left until freedom
  void UpdateMiaoliDay(bool is_today_miaoli_day = false) { 
    if (is_today_miaoli_day)
      miaoli_day_ = kMiaoliNum;
    else
      miaoli_day_ = std::max(miaoli_day_ - 1, 0);
  }
  bool get_is_still_in_miaoli() const {
    return (miaoli_day_ > 0) ? true : false;
  }
  int get_miaoli_day() const {
    return miaoli_day_;
  }

  // Jail
  // updates the days left until freedom
  void UpdateJailDay(bool is_today_jail_day = false) { 
    if (is_today_jail_day)
      jail_day_ = kJailNum;
    else
      jail_day_ = std::max(jail_day_ - 1, 0);
  }
  bool get_is_still_in_jail() const {
    return (jail_day_ > 0) ? true : false;
  }
  int get_jail_day() const {
    return jail_day_;
  }

  // Hospital
  // updates the days left until freedom
  void UpdateHospitalDay(bool is_today_hospital_day = false) { 
    if (is_today_hospital_day)
      hospital_day_ = kHospitalNum;
    else
      hospital_day_ = std::max(hospital_day_ - 1, 0);
  }
  bool get_is_still_in_hospital() const {
    return (miaoli_day_ > 0) ? true : false;
  }
  int get_hospital_day() const {
    return hospital_day_;
  }

};


class Location {
  private:
  const std::string name_;
  const int votes_in_this_region_;
  int vote_[kPlayerNum]; // 百分比
  int current_winner_;

  public:
  // Constructor
  Location(std::string name, int votes_in_this_region) :name_(name), votes_in_this_region_(votes_in_this_region){ // assuming 2 players
    current_winner_ = kPlayerNum;
    for (int i = 0; i < kPlayerNum; i++) {
      vote_[i] = 50;
    }
  }

  // Accessors
  // used in the end
  int get_votes_in_this_region() const {
    return votes_in_this_region_;
  }
  int get_current_winner() { // assuming 2 players
    current_winner_ = (vote_[0] > vote_[1]) ? 0 : 1;
    return current_winner_;
  }

  // Member Functions
  // updates the money of the player and the votes of the player in this location if baipiao
  void BaiPiao(Player *player) { // assuming only 2 players
    int delta_vote = RandomBaiPiaoVote();
    vote_[player->get_player_index()] += delta_vote;
    vote_[std::abs(1 - player->get_player_index())] -= delta_vote;
    player->UpdateMoney(-kBaiPiaoSpendMoney);
  }

  // updates the money of the plaer and the votes of the player in this location if speech
  void Speech(Player *player) {
    int delta_money = RandomSpeechGetMoney();
    player->UpdateMoney(delta_money);
  }

};

// Functions on building and modifying text
// Builds the text
void BuildText(sf::Text &text, const sf::Font &font, const sf::String &content, unsigned int size,
               const sf::Color &color, sf::Uint32 style, float x, float y ) {
  text.setFont(font);
  text.setString(content);
  text.setCharacterSize(size);
  text.setFillColor(color);
  text.setStyle(style);
  text.setPosition(x, y);
} 

void ChangeDiceText(sf::Text &text, const int dice_number) {
  std::string dice_string = "You rolled " + std::to_string(dice_number) + " steps";
  text.setString(dice_string);
}

void ChangeTellLocationText(sf::Text &text, const int location_index, const std::string *list_of_locations) {
  std::string location_string = "You are now at " + list_of_locations[location_index];
  text.setString(location_string);
}

void ChangeMiaoliText(sf::Text &text, const int miaoli_day) {
  std::string miaoli_string = "You have " + std::to_string(miaoli_day) + " days until you leave miaoli.";
  text.setString(miaoli_string);
}

void ChangeJailText(sf::Text &text, const int jail_day) {
  std::string jail_string = "You have " + std::to_string(jail_day) + " days until you leave Jail.";
  text.setString(jail_string);
}

void ChangeHospitalText(sf::Text &text, const int hospital_day) {
  std::string hospital_string = "You have " +std::to_string(hospital_day) + " days until you leave the hospital.";
  text.setString(hospital_string);
}

void ChangeTellRoundText(sf::Text &text, const int round) {
  std::string round_string = "Round: " + std::to_string(round);
  text.setString(round_string);
}

void ChangeTellPlayerAndPropertiesText(sf::Text &text, const Player *player) {
  std::string p_and_p_string = "Player: " + player->get_player_name() + "; " + std::to_string(player->get_money()) + " ntd";
  text.setString(p_and_p_string);
}

// function that finds the coordinates of players
sf::Vector2<int> getLocation(int player_num, int location_index, int coordinates[][4]){
    sf :: Vector2<int> result = {coordinates[location_index][player_num * 2] * 10 / 14, coordinates[location_index][player_num * 2 + 1] * 10 / 14};
    return result;
}

int main (int argc, char** argv) {
  sf::RenderWindow render_window(sf::VideoMode(kWindowWidth, kWindowHeight), "cute cattt", sf::Style::Titlebar | sf::Style::Close);
  sf::Event ev;

  sf::Font big_font;
  big_font.loadFromFile("fonts/HanaleiFill-Regular.ttf");

// Menu setup

  // Menu Text
  sf::Text menu_text;
  BuildText(menu_text, big_font, "MENU", 50, sf::Color::White, sf::Text::Bold, 500, 500);

  sf::Text menu_sentence;
  BuildText(menu_sentence, big_font, "press space to continue", 25, sf::Color::Magenta, sf::Text::Italic, 500, 700);

// Dice setup
  // Dice Randomizer
  srand(time(0));
  int dice_value = 0;

  // Dice Prompt Text
  sf::Text dice_prompt_text;
  BuildText(dice_prompt_text, big_font, "Press Space to roll the dice!", 50, sf::Color::Blue, sf::Text::Bold, 500, 250);

  // Dice Text
  sf::Text dice_text;
  BuildText(dice_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 250);

  // Miao Li Text (optional)
  sf::Text miaoli_text;
  BuildText(miaoli_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 500);

  // Jail Text (optional)
  sf::Text jail_text;
  BuildText(jail_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 800, 500);

  // Hospital Text (optional)
  sf::Text hospital_text;
  BuildText(hospital_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 800, 500);

// CITY and NEWS setup
  // Tell Location Text
  sf::Text tell_location_text;
  BuildText(tell_location_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 500);

// CITY setup
  // Option Bai Piao
  sf::Text option_bai_piao_text;
  BuildText(option_bai_piao_text, big_font, "1. pay 100 wan to get 5~10 percent of votes", 50, sf::Color::Blue, sf::Text::Regular, 250, 300);

  // Option Speech
  sf::Text option_speech_text;
  BuildText(option_speech_text, big_font, "2. get 10~30 wan ntd", 50, sf::Color::Blue, sf::Text::Regular, 250, 400);

// WAIT setup
  sf::Text next_player_prompt;
  BuildText(next_player_prompt, big_font, "press space to pass the dice to the next player!", 50, sf::Color::Blue, sf::Text::Regular, 250, 400);

// BackGround Setup

  //地圖
  sf::Texture board_texture;
  board_texture.create(kWindowWidth, kWindowHeight);
  board_texture.loadFromFile("images/map.png");
  sf::Sprite board_sprite;
  board_sprite.setTexture(board_texture);

// Location Setup
  const std::string list_of_location_names_mandarin[kLocationIndexNum] = {
    "總統府",
    "基隆",
    "新北",
    "臺北",
    "桃園",
    "新聞台",
    "新竹",
    "苗栗國",
    "臺中",
    "南投",
    "彰化",
    "新聞台",
    "雲林",
    "嘉義",
    "臺大醫院",
    "臺南",
    "高雄",
    "新聞台",
    "澎湖",
    "屏東",
    "臺東",
    "綠島監獄",
    "宜蘭",
    "警察局",
    "花蓮",
    "金門",
    "馬祖",
    "新聞台"
  };
  
  const std::string list_of_location_names[kLocationIndexNum] = {
    "President's place",
    "ji long",
    "xin bei",
    "tai bei",
    "tao yuan",
    "xin wen tai",
    "xin zhu",
    "Miaoli Kingdom",
    "tai zhong",
    "nan tou",
    "zhang hua",
    "xin wen tai",
    "yun lin",
    "jia yi",
    "ntu hospital",
    "tai nan",
    "gao xiong",
    "xin wen tai",
    "peng hu",
    "ping dong",
    "tai dong",
    "lv dao jian yu",
    "yi lan",
    "POLICEEEE",
    "hua lian",
    "jin men",
    "ma zu",
    "xin wen tai"
  };
  
  
  const int list_of_location_votes[kLocationIndexNum] = {
    0, 37, 403, 262, 226, 0, 102, 0, 282, 49, 126, 0, 68, 77, 0, 187, 277, 0, 11, 81, 22, 0, 45, 0, 33, 14, 2, 0
  };
  Location* locations[kLocationIndexNum];
  for (int i = 0; i < kLocationIndexNum; i++) {
    locations[i] = new Location(list_of_location_names[i], list_of_location_votes[i]);
  }

  int coordinates[28][4]={
          {1177, 1257, 1309, 1257},
          {1042, 1185, 1042, 1253},
          {893, 1185, 893, 1253},
          {744, 1185 , 744, 1253},
          {601, 1185, 601, 1253},
          {450, 1211, 450, 1291},
          {304, 1185, 304, 1253},
          {57, 1217, 141, 1217},
          {100, 1057, 175, 1057},
          {100, 908, 175, 908},
          {100, 762, 175, 762},
          {61, 603, 127, 603},
          {100, 465, 175, 465},
          {100, 319, 175, 319},
          {49, 77, 149, 77},
          {304, 89, 304, 155},
          {450, 89, 450, 155},
          {601, 65, 601, 149},
          {744, 89, 744, 155},
          {813, 89, 813, 155},
          {1042, 89, 1042, 155},
          {1178, 77, 1318, 77},
          {1162, 319, 1239, 319},
          {1200, 465, 1284, 465},
          {1162, 603, 1239, 603},
          {1162, 762, 1239, 762},
          {1162, 908, 1239, 908},
          {1200, 1057, 1284, 1057}
  };

// Player Setup
  std::string list_of_player_names[kPlayerNum] = {"one", "two"};
  Player* players[kPlayerNum];
  for (int i = 0; i < kPlayerNum; i++) {
    players[i] = new Player(list_of_player_names[i], i); // 在沒有做輸入player1 2的名字前 先暫時這樣
  }

// General text setup
  // tell round text
  sf::Text tell_round_text;
  BuildText(tell_round_text, big_font, "Round: 1", 25, sf::Color::Blue, sf::Text::Regular, 500, 150);

  // tell player and properties text
  sf::Text tell_player_and_properties_text;
  std::string p_and_p_init = "Player: " + players[0]->get_player_name() + "; " + std::to_string(players[0]->get_money()) + " ntd";
  BuildText(tell_player_and_properties_text, big_font, p_and_p_init, 25, sf::Color::Blue, sf::Text::Regular, 500, 200);

// Game Loop  
  state = MENU; 
  int current_id = 0; // The current id of the player (0 or 1)
  int current_round = 0; // Stop when player[1] finishes round kTotalRounds
  int zero = 0; // total votes in the end of player[0]
  int one = 0; // total votes in the end of player[1]
  int winner = kPlayerNum;
  while (render_window.isOpen()) {

    switch (state) {

      case MENU:
        while (render_window.pollEvent(ev)) {

          switch (ev.type) {
            case sf::Event::EventType::Closed:
              render_window.close();
              break;
            case sf::Event::EventType::KeyPressed:
              if (ev.key.code == sf::Keyboard::Space) {
              
                state = DICE;
              }
              break;
          }
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(menu_text);
        render_window.draw(menu_sentence);
        render_window.display();
        break;

      case DICE:
        while (render_window.pollEvent(ev)) {
          switch (ev.type) {
            case sf::Event::EventType::Closed:
              render_window.close();
              break;
            case sf::Event::EventType::KeyPressed:
              if (ev.key.code == sf::Keyboard::Space) {
                // Update all the special cases
                players[current_id]->UpdateMiaoliDay();
                players[current_id]->UpdateJailDay();
                players[current_id]->UpdateHospitalDay();
                players[current_id]->UpdateBribeDay();

                ChangeTellRoundText(tell_round_text, current_round + 1);
                ChangeTellPlayerAndPropertiesText(tell_player_and_properties_text, players[current_id]);

                // Check if matches any of the special cases. 
                // If so, go to state = WAIT and use get_is_still_in_XXX to get the text
                if (players[current_id]->get_is_still_in_miaoli() || players[current_id]->get_is_still_in_hospital() 
                    || players[current_id]->get_is_still_in_jail()) {
                  ChangeMiaoliText(miaoli_text, players[current_id]->get_miaoli_day());
                  ChangeJailText(jail_text, players[current_id]->get_jail_day());
                  ChangeHospitalText(hospital_text, players[current_id]->get_hospital_day());
                  state = WAIT;
                } 

                // add 20,000
                players[current_id]->UpdateMoney(kMoneyEachRound);

                // throw the dice, update location and text
                int dice_value = RandomDice();
                std::cout << dice_value << std::endl;
                players[current_id]->UpdateLocationIndex(dice_value);
                int new_location_index = players[current_id]->get_location_index(); // indicates the location of the player in this round
                ChangeDiceText(dice_text, dice_value);
                ChangeTellLocationText(tell_location_text, new_location_index, list_of_location_names);

                // check if is on start(總統府). If so, add money
                if (new_location_index == 0 && current_round != 0) {
                  players[current_id]->UpdateMoney(kMoneyEarnedAtStart);
                  state = WAIT;
                }

                // check if is on 苗栗. If so, change state and stay for 3 days
                else if (new_location_index == 7) {
                  players[current_id]->UpdateMiaoliDay(true);
                  ChangeMiaoliText(miaoli_text, players[current_id]->get_miaoli_day());
                  state = WAIT;
                }

                // check if is on 臺大醫院. If so, change state and stay for 3 days.
                else if (new_location_index == 14) {
                  players[current_id]->UpdateHospitalDay(true);
                  ChangeHospitalText(hospital_text, players[current_id]->get_hospital_day());
                  state = WAIT;
                }

                // check if is on Police and has bribed in kBribeNum days
                // if so, teleport to jail and stay for 3 days
                else if (new_location_index == 23) {
                  if (players[current_id]->get_is_bribed()) {
                    players[current_id]->TeleportToJail();
                    players[current_id]->UpdateJailDay(true);
                    ChangeJailText(jail_text, players[current_id]->get_jail_day());
                  }
                  state = WAIT;
                }

                // else if at news
                else if (new_location_index == 5 || new_location_index == 11 || new_location_index == 17 || new_location_index == 27) {
                  state = NEWS;
                }
                // else at city
                else {
                  state = CITY;
                }
              }
              break;
          }
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        render_window.draw(dice_prompt_text);
        render_window.draw(tell_round_text);
        render_window.draw(tell_player_and_properties_text);
        render_window.display();
        break;
            
      case CITY:
        while (render_window.pollEvent(ev)) {
          if (ev.type == sf::Event::EventType::Closed) {
            render_window.close();
          } else if (ev.type == sf::Event::EventType::KeyPressed) {
            switch (ev.key.code) {
              case sf::Keyboard::Escape:
                state = END;
                break;
              case sf::Keyboard::Num1:
                locations[players[current_id]->get_location_index()]->BaiPiao(players[current_id]);
                ChangeTellRoundText(tell_round_text, current_round + 1);
                ChangeTellPlayerAndPropertiesText(tell_player_and_properties_text, players[current_id]);
                state = WAIT;
                break;
              case sf::Keyboard::Num2:
                locations[players[current_id]->get_location_index()]->Speech(players[current_id]); 
                ChangeTellRoundText(tell_round_text, current_round + 1);
                ChangeTellPlayerAndPropertiesText(tell_player_and_properties_text, players[current_id]);
                state = WAIT; 
                break;
            }
          }        
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        render_window.draw(dice_text);
        render_window.draw(tell_round_text);
        render_window.draw(tell_player_and_properties_text);
        render_window.draw(tell_location_text); // eg. "你現在在苗栗!"
        render_window.draw(option_bai_piao_text); // eg. "1. 支付100萬，增取5~10%的選票"
        render_window.draw(option_speech_text); // eg. "2. 獲得10~30萬台幣"
        render_window.display();
        break;


      case NEWS:
        while (render_window.pollEvent(ev)) {
          if (ev.type == sf::Event::EventType::Closed) {
            render_window.close();
          } else if (ev.type == sf::Event::EventType::KeyPressed) {
            switch (ev.key.code) {
              case sf::Keyboard::Escape:
                state = END;
                break;
              case sf::Keyboard::Space:
                state = WAIT;
                break;
            }
          }        
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        render_window.draw(tell_round_text);
        render_window.draw(tell_player_and_properties_text);
        render_window.draw(tell_location_text); // eg. "你現在在新聞台!"
        render_window.display();
        break;

      case WAIT:
        while (render_window.pollEvent(ev)) {
          if (ev.type == sf::Event::EventType::Closed) {
            render_window.close();
          } else if (ev.type == sf::Event::EventType::KeyPressed) {
            switch (ev.key.code) {
              case sf::Keyboard::Space:
                std::cout << "round: " << current_round + 1 << std::endl;
                std::cout << "rn is "<< current_id <<"'s turn" << std::endl;
                if (current_id == kPlayerNum - 1) {
                  current_round++;
                } 
                if (current_id == kPlayerNum - 1 && current_round == kTotalRounds) {
                  for (int i = 0; i < kLocationIndexNum; i++) {
                    if (locations[i]->get_current_winner() == 0) {
                      zero += locations[i]->get_votes_in_this_region();
                    } else if (locations[i]->get_current_winner() == 1) {
                      one += locations[i]->get_votes_in_this_region();
                    }
                  }
                  winner = (zero > one) ? 0 : 1;
                  if (zero == one) {
                    winner = (players[0]->get_money() > players[1]->get_money()) ? 0 : 1;
                  }
                  std::cout << "end game, winner: " << winner << std::endl;
                  state = END;
                } else {
                  current_id = std::abs(1 - current_id);
                  ChangeTellRoundText(tell_round_text, current_round + 1);
                  ChangeTellPlayerAndPropertiesText(tell_player_and_properties_text, players[current_id]);
                  state = DICE;
                  break;
                }
            }
          }
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        if (players[current_id]->get_is_still_in_miaoli()) {
          render_window.draw(miaoli_text);
        } else if (players[current_id]->get_is_still_in_jail()) {
          render_window.draw(jail_text);
        } else if (players[current_id]->get_is_still_in_hospital()) {
          render_window.draw(hospital_text);
        }
        render_window.draw(next_player_prompt); // "press space to give the dice to the other player"
        render_window.draw(tell_round_text);
        render_window.draw(tell_player_and_properties_text);
        render_window.display();
        break;
      
      case END:
        while (render_window.pollEvent(ev)) {
          switch (ev.type) {
            case sf::Event::EventType::Closed:
              render_window.close();
              break;
          }
        }
        render_window.clear(sf::Color::Black);
        // render_window.draw(winner_text) // eg. "winner is XXX. \n <player0> has xx votes.\n <player1> has yy votes."
        render_window.display();
        break;
    }

  }


  for (int i = 0; i < kLocationIndexNum; i++) {
    delete locations[i];
    locations[i] = nullptr;
  }
  for (int i = 0; i < kPlayerNum; i++) {
    delete players[i];
    players[i] = nullptr;
  }


  return 0;
}