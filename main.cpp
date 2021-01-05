#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <string>

enum State { // 這告訴我們現在在哪個狀況下 然後main 就會跑相對應的程式
    MENU, 
    NAME_1,
    NAME_2,
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
const int kStartMoney = 2000;
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



class Player {
  private:
  std::string name_;
  int location_index_;
  int money_;
  int votes_;
  int bribe_day_; // if the player meets the police when bribe_day_ > 0, he gets caught
  int stay_day_; // the days left until the player could go out of jail/hospital

  public:
  // Constructors and Destructors
  // - no Destructors, because no pointers exists.
  Player(std::string name) {
    name_ = name;
    location_index_ = 0;
    money_ = kStartMoney;
    votes_ = 0;
    bribe_day_ = 0;
    stay_day_ = 0;
  }

  // Accessors and Mutators
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
  int vote_[kPlayerNum];
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

  // Functions
  void BaiPiao(int player_num, int random_number) { // assuming only 2 players
    std::uniform_int_distribution<int> bai_piao_range(kBaiPiaoVoteMin, kBaiPiaoVoteMax);
    //player(player_num).LostMoney
    int delta_vote = bai_piao_range(random_number);
    vote_[player_num] += delta_vote;
    vote_[std::abs(1 - player_num)] -= delta_vote;
  }

  void Speech(int player_num, int random_number) {
    std::uniform_int_distribution<int> speech_money_range(kSpeechGetMoneyMin, kSpeechGetMoneyMax);
    int delta_money = speech_money_range(random_number);
    // add money for player
  }

};


int main (int argc, char** argv) {
  sf::RenderWindow render_window(sf::VideoMode(kWindowWidth, kWindowHeight), "cute cattt");
  sf::Event ev;

// Menu setup
  // Menu Font
  sf::Font big_font;
  big_font.loadFromFile("fonts/HanaleiFill-Regular.ttf");

  // Menu words
  sf::Text menu_text;
  menu_text.setFont(big_font);
  menu_text.setString("menu");
  menu_text.setCharacterSize(50);
  menu_text.setFillColor(sf::Color::Blue);
  menu_text.setStyle(sf::Text::Bold);
  //menu setposition

  sf::Text menu_sentence;
  menu_sentence.setFont(big_font);
  menu_sentence.setString("press space to continue");
  menu_sentence.setCharacterSize(25);
  menu_sentence.setFillColor(sf::Color::Magenta);
  menu_sentence.setStyle(sf::Text::Italic);
  menu_sentence.setPosition(0, 100); // 暫時

// Game setup
  // Dice setup // 這個失敗了 我到時候改 rand() 版本
  std::uniform_int_distribution<int> random_dice_range(kDiceMin, kDiceMax);
  std::random_device rd;
  std::mt19937 random_number(rd());
  
  int dice_value = 2;
  sf::Text dice_sentence;
  dice_sentence.setFont(big_font);
  std::string dice_string = "You rolled " + std::to_string(dice_value) + " steps";
  dice_sentence.setString(dice_string);

// Sprite Setup
  //測試貓 沒啥特別意義 就測試
  sf::Texture cat_texture;
  cat_texture.loadFromFile("images/cat.png");
  sf::Sprite cat_sprite;
  cat_sprite.setTexture(cat_texture);

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
  Player* players[kPlayerNum];


  
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