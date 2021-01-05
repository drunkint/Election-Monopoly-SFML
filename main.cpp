#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <string>
#include <ctime>

enum State { // 這告訴我們現在在哪個狀況下 然後main 就會跑相對應的程式
    MENU, 
    NAME,
    DICE, // when the first player rolls the dice
    CITY, // should be <location type 1> state_1
    MIAO_LI,
    NEWS,
    WAIT,
    END,
  }state;

// Window consts
const int kWindowWidth = 1000;
const int kWindowHeight = 1000;

// Game Setting Consts
const int kPlayerNum = 2;
const int kLocationIndexNum = 28;
const int kMoneyEachRound = 200000;
const int kStartMoney = 20000000;
const int kBribeNum = 5;
const int kStayNum = 3;

// Spend Money consts
const int kBaiPiaoSpendMoney = 100000;

// Random consts
const int kDiceMin = 2;
const int kDiceMax = 12;
const int kDiceRange = kDiceMax - kDiceMin;
const int kBaiPiaoVoteMin = 5;
const int kBaiPiaoVoteMax = 10; 
const int kBaiPiaoVoteRange = kBaiPiaoVoteMax - kBaiPiaoVoteMin;
const int kSpeechGetMoneyMin = 100000;
const int kSpeechGetMoneyMax = 300000;
const int kSpeechGetMoneyRange = kSpeechGetMoneyMax - kSpeechGetMoneyMin;

// Random Functions:
int RandomDice() {
  return rand() % kDiceRange;
}

int RandomBaiPiaoVote() {
  return rand() % kBaiPiaoVoteRange;
}

int RandomSpeechGetMoney() {
  return rand() % kSpeechGetMoneyRange;
}


class Player {
  private:
  std::string name_;
  int player_index_;
  int location_index_;
  int money_;
  int votes_;
  int bribe_day_; // if the player meets the police when bribe_day_ > 0, he gets caught
  int stay_day_; // the days left until the player could go out of jail/hospital

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
    stay_day_ = 0;
  }

  // Accessors and Mutators
  // Player Index
  int get_player_index() {
    return player_index_;
  }

  // Location Index
  int get_location_index() {
    return location_index_;
  }
  void update_location_index(int steps) {
    location_index_ += steps;
    location_index_ %= kLocationIndexNum;
  }

  // Money
  int get_money() {
    return money_;
  }
  void update_money(int amount) { // enter the amount you want to add or spend. Amount is positive if add, and negative if spend.
    money_ += amount;
  }

  // Votes
  int get_votes() {
    return votes_;
  }

  // Bribe
  void update_bribe_day(bool is_today_bribe_day = false) { // pass in true when today is bribe day. Otherwise, just put this code when a day passess.
    if (is_today_bribe_day)
      bribe_day_ = kBribeNum;
    else
      bribe_day_ = std::max(bribe_day_ - 1, 0);
  }
  bool get_is_bribed() { // When the police wants to check if you have bribed
    return (bribe_day_ > 0) ? true : false;
  }
  
  // Stay
  void update_stay_day() { // updates the days left until freedom
    stay_day_ = std::max(bribe_day_ - 1, 0);
  }
  bool get_is_staying() {
    return (stay_day_ > 0) ? true : false;
  }

};


class Location {
  private:
  const std::string name_;
  int vote_[kPlayerNum]; // 百分比
  int current_winner_;

  public:
  // Constructor
  Location(std::string name) :name_(name) { // assuming 2 players
    current_winner_ = kPlayerNum;
    for (int i = 0; i < kPlayerNum; i++) {
      vote_[i] = 50;
    }
  }

  // Accessors
  int get_current_winner()  { // assuming 2 players
    current_winner_ = (vote_[0] > vote_[1]) ? 0 : 1;
    return current_winner_;
  }

  // Member Functions
  void BaiPiao(Player &player) { // assuming only 2 players
    int delta_vote = RandomBaiPiaoVote();
    vote_[player.get_player_index()] += delta_vote;
    vote_[std::abs(1 - player.get_player_index())] -= delta_vote;
    player.update_money(-kBaiPiaoSpendMoney);
  }

  void Speech(Player &player) {
    int delta_money = RandomSpeechGetMoney();
    player.update_money(delta_money);
  }

};

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



int main (int argc, char** argv) {
  sf::RenderWindow render_window(sf::VideoMode(kWindowWidth, kWindowHeight), "cute cattt");
  sf::Event ev;

// Menu setup
  // Menu Font
  sf::Font big_font;
  big_font.loadFromFile("fonts/HanaleiFill-Regular.ttf");

  // Menu Text
  sf::Text menu_text;
  BuildText(menu_text, big_font, "MENU", 50, sf::Color::White, sf::Text::Bold, 500, 500);

  sf::Text menu_sentence;
  BuildText(menu_sentence, big_font, "press space to continue", 25, sf::Color::Magenta, sf::Text::Italic, 500, 700);

// Game setup
  // Dice Randomizer
  srand(time(0));
  int dice_value = 0;

  // Dice Prompt Text
  sf::Text dice_prompt_text;
  BuildText(dice_prompt_text, big_font, "Press Space to roll the dice!", 50, sf::Color::Blue, sf::Text::Bold, 500, 250);

  // Dice Text
  sf::Text dice_text;
  BuildText(dice_text, big_font, "", 25, sf::Color::White, sf::Text::Regular, 500, 500);

  // Tell Location Text
  sf::Text tell_location_text;
  BuildText(tell_location_text, big_font, "", 25, sf::Color::White, sf::Text::Regular, 500, 500);
// BackGround Setup

  //暫時地圖
  sf::Texture board_texture;
  board_texture.loadFromFile("images/map.png");
  sf::Sprite board_sprite;
  board_sprite.setTexture(board_texture);


  // player1跟2 座標

// Location Setup
  std::string list_of_location_names[kLocationIndexNum] = {
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
  Location* locations[kLocationIndexNum];
  for (int i = 0; i < kLocationIndexNum; i++) {
    locations[i] = new Location(list_of_location_names[i]);
  }

// Player Setup
  std::string list_of_player_names[kPlayerNum] = {"one", "two"};
  Player* players[kPlayerNum];
  for (int i = 0; i < kPlayerNum; i++) {
    players[i] = new Player(list_of_player_names[i], i); // 在沒有做輸入player1 2的名字前 先暫時這樣
  }


  int current_id = 0;
  state = MENU;
  int current_round = 0;
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
                // add 20,000
                players[current_id]->update_money(kMoneyEachRound);

                // throw the dice, update location
                int old_location_index = players[current_id]->get_location_index();
                int dice_value = RandomDice();
                players[current_id]->update_location_index(dice_value);
                int new_location_index = players[current_id]->get_location_index();

                // check if went past start(總統府). If so, add money
                if (new_location_index - old_location_index < 0) {

                }

                if (players[current_id]->get_location_index() == 7) {

                }
              }
              break;
          }
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(dice_prompt_text);
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
              case sf::Keyboard::Num2:

                
              
            }
          }        
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        // draw the questions
        render_window.display();
        break;


      case MIAO_LI:
        break;

      case NEWS:
        break;

      case WAIT:
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
        render_window.display();
        break;
    }

  }

  delete [] locations;
  for (int i = 0; i < kLocationIndexNum; i++)
    locations[i] = nullptr;
  delete [] players;
  for (int i = 0; i < kPlayerNum; i++)
    players[i] = nullptr;


  return 0;
}