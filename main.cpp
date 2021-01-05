#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <string>
#include <ctime>

// ���ݨ��~

enum State { // �o�i�D�ڭ̲{�b�b���Ӫ��p�U �M��main �N�|�]�۹������{��
    MENU, 
    NAME,
    DICE, // when the first player rolls the dice
    CITY, // should be <location type 1> state_1
    NEWS,
    WAIT,
    END,
  }state;

// Window consts
const int kWindowWidth = 1400;
const int kWindowHeight = 1400;

// Game Setting Consts
const int kPlayerNum = 2;
const int kTotalRounds = 5;
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
  // Player Index
  int get_player_index() {
    return player_index_;
  }

  // Location Index
  int get_location_index() {
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
  int get_money() {
    return money_;
  }
  void UpdateMoney(int amount) { // enter the amount you want to add or spend. Amount is positive if add, and negative if spend.
    money_ += amount;
  }

  // Votes
  int get_votes() {
    return votes_;
  }

  // Bribe
  void UpdateBribeDay(bool is_today_bribe_day = false) { // pass in true when today is bribe day. Otherwise, just put this code when a day passess.
    if (is_today_bribe_day)
      bribe_day_ = kBribeNum;
    else
      bribe_day_ = std::max(bribe_day_ - 1, 0);
  }
  bool get_is_still_bribed() { // When the police wants to check if you have bribed
    return (bribe_day_ > 0) ? true : false;
  }
  
  // miaoli
  void UpdateMiaoliDay(bool is_today_miaoli_day = false) { // updates the days left until freedom
    if (is_today_miaoli_day)
      miaoli_day_ = kMiaoliNum;
    else
      miaoli_day_ = std::max(miaoli_day_ - 1, 0);
  }
  bool get_is_still_in_miaoli() {
    return (miaoli_day_ > 0) ? true : false;
  }

  // Jail
  void UpdateJailDay(bool is_today_jail_day = false) { // updates the days left until freedom
    if (is_today_jail_day)
      jail_day_ = kJailNum;
    else
      jail_day_ = std::max(jail_day_ - 1, 0);
  }
  bool get_is_still_in_jail() {
    return (jail_day_ > 0) ? true : false;
  }

  // Hospital
  void UpdateHospitalDay(bool is_today_hospital_day = false) { // updates the days left until freedom
    if (is_today_hospital_day)
      hospital_day_ = kHospitalNum;
    else
      hospital_day_ = std::max(hospital_day_ - 1, 0);
  }
  bool get_is_still_in_hospital() {
    return (miaoli_day_ > 0) ? true : false;
  }

};


class Location {
  private:
  const std::string name_;
  int vote_[kPlayerNum]; // �ʤ���
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
    player.UpdateMoney(-kBaiPiaoSpendMoney);
  }

  void Speech(Player &player) {
    int delta_money = RandomSpeechGetMoney();
    player.UpdateMoney(delta_money);
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
  BuildText(dice_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 250);

  // Tell Location Text
  sf::Text tell_location_text;
  BuildText(tell_location_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 500);

  // Miao Li Text (optional)
  sf::Text miaoli_text;
  BuildText(miaoli_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 500, 500);

  // Jail Text (optional)
  sf::Text jail_text;
  BuildText(jail_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 800, 500);

  // Hospital Text (optional)
  sf::Text hospital_text;
  BuildText(hospital_text, big_font, "", 25, sf::Color::Blue, sf::Text::Regular, 800, 500);


// BackGround Setup

  //�Ȯɦa��
  sf::Texture board_texture;
  board_texture.create(kWindowWidth, kWindowHeight);
  board_texture.loadFromFile("images/map.png");
  sf::Sprite board_sprite;
  board_sprite.setTexture(board_texture);


  // player1��2 �y��

// Location Setup
  std::string list_of_location_names[kLocationIndexNum] = {
    "�`�Ω�",
    "��",
    "�s�_",
    "�O�_",
    "���",
    "�s�D�x",
    "�s��",
    "�]�߰�",
    "�O��",
    "�n��",
    "����",
    "�s�D�x",
    "���L",
    "�Ÿq",
    "�O�j��|",
    "�O�n",
    "����",
    "�s�D�x",
    "���",
    "�̪F",
    "�O�F",
    "��q�ʺ�",
    "�y��",
    "ĵ�",
    "�Ὤ",
    "����",
    "����",
    "�s�D�x"
  };
  Location* locations[kLocationIndexNum];
  for (int i = 0; i < kLocationIndexNum; i++) {
    locations[i] = new Location(list_of_location_names[i]);
  }

// Player Setup
  std::string list_of_player_names[kPlayerNum] = {"one", "two"};
  Player* players[kPlayerNum];
  for (int i = 0; i < kPlayerNum; i++) {
    players[i] = new Player(list_of_player_names[i], i); // �b�S������Jplayer1 2���W�r�e ���Ȯɳo��
  }


// Game Loop
  int current_id = 0; // The current id of the player (0 or 1)
  state = MENU; 
  int current_round = 1; // Stop when player[1] finishes round kRound 
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

                // Check if matches any of the special cases. 
                // If so, go to state = WAIT and use get_is_still_in_XXX to get the text
                if (players[current_id]->get_is_still_in_miaoli() || players[current_id]->get_is_still_in_hospital() 
                    || players[current_id]->get_is_still_in_jail() || players[current_id]->get_is_still_bribed()) {
                  state = WAIT;
                } 

                // add 20,000
                players[current_id]->UpdateMoney(kMoneyEachRound);

                // throw the dice, update location and text
                int dice_value = RandomDice();
                std::cout << dice_value << std::endl;
                players[current_id]->UpdateLocationIndex(dice_value);
                int new_location_index = players[current_id]->get_location_index();
                ChangeDiceText(dice_text, dice_value);

                // check if is on start(�`�Ω�). If so, add money
                if (new_location_index == 0) {
                  players[current_id]->UpdateMoney(kMoneyEarnedAtStart);
                  state = WAIT;
                }

                // check if is on �]��. If so, change state and stay for 3 days
                else if (new_location_index == 7) {
                  players[current_id]->UpdateMiaoliDay(true);
                  state = WAIT;
                }

                // check if is on �O�j��|. If so, change state and stay for 3 days.
                else if (new_location_index == 14) {
                  players[current_id]->UpdateHospitalDay(true);
                  state = WAIT;
                }

                // check if is on Police and has bribed in kBribeNum days
                // if so, teleport to jail and stay for 3 days
                else if (new_location_index == 23) {
                  if (players[current_id]->get_is_still_bribed()) {
                    players[current_id]->TeleportToJail();
                    players[current_id]->UpdateJailDay(true);
                  }
                  state = WAIT;
                }

                // else if at news
                // else at city
                else {
                  state = WAIT;
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
              //case sf::Keyboard::Num2:

                
              
            }
          }        
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        // draw the questions
        render_window.display();
        break;


      case NEWS:
        break;

      case WAIT:
        while (render_window.pollEvent(ev)) {
          switch (ev.type) {
            case sf::Event::EventType::Closed:
              render_window.close();
              break;
          }
        }
        render_window.clear(sf::Color::Black);
        render_window.draw(board_sprite);
        render_window.draw(dice_text);
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
        render_window.display();
        break;
    }

  }


  delete locations;
  for (int i = 0; i < kLocationIndexNum; i++)
    locations[i] = nullptr;
  delete players;
  for (int i = 0; i < kPlayerNum; i++)
    players[i] = nullptr;


  return 0;
}