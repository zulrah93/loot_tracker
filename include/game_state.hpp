#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <memory>
#include <monster_t.hpp>
#include <player_metadata_t.hpp>
#include <true_rng_t.hpp>

enum game_mode_t : uint8_t {
  menu = 0,
  account_creation,
  account_loading,
  overworld, // walking around not in combat
  battling   // new game state where you are in combat not in the overworld
};

enum menu_selection_t : uint8_t { new_game = 0, existing_game, exit_game };

constexpr auto CREDITS_STRING{
    "Developed by Daniel Lopez -- special thanks to "
    "all the gamer friends I have made in my life..."};

static const std::initializer_list<std::string> MENU_STRINGS = {
    "[*] New Game\n[ ] Load Exisiting Game\n[ ] Exit",
    "[ ] New Game\n[*] Load Exisiting Game\n[ ] Exit",
    "[ ] New Game\n[ ] Load Exisiting Game\n[*] Exit"};

class game_state_t {
public:
  game_state_t(std::weak_ptr<sf::RenderWindow> weak_render_window)
      : m_weak_render_window{weak_render_window}, m_player{},
        m_true_rng_supported{true_rng_t::rdseed64_instruction_supported()},
        m_mode{game_mode_t::menu} {
    // Load font
    m_font.loadFromFile(std::filesystem::current_path().string() +
                        "/assets/GohuFontuni14NerdFont-Regular.ttf");
    m_true_random_supported_text.setFont(m_font);
    m_true_random_supported_text.setPosition(sf::Vector2<float>(150.0f, 0.0f));
    m_true_random_supported_text.setCharacterSize(11);

    // Initial title screen setup
    m_title_screen_text.setFont(m_font);
    m_title_screen_text.setPosition(sf::Vector2<float>(295.0f, 130.0f));
    m_title_screen_text.setCharacterSize(25);
    m_title_screen_text.setString("Loot Tracker");
    m_title_screen_text.setColor(sf::Color::Cyan);

    // Initial menu text setup
    m_menu_text.setFont(m_font);
    m_menu_text.setPosition(sf::Vector2<float>(275.0f, 200.0f));
    m_menu_text.setCharacterSize(20);
    m_menu_text.setString(*MENU_STRINGS.begin());
    m_menu_text.setColor(sf::Color::Cyan);

    // Initial credits text
    m_credits_text.setFont(m_font);
    m_credits_text.setPosition(sf::Vector2<float>(172.0f, 574.0f));
    m_credits_text.setCharacterSize(9);
    m_credits_text.setString(CREDITS_STRING);
    m_credits_text.setColor(sf::Color::White);

    // Initial text for instructions for loading account or creating account
    m_new_character_name.setFont(m_font);
    m_new_character_name.setPosition(sf::Vector2<float>(55.0f, 245.0f));
    m_new_character_name.setCharacterSize(11);
    m_new_character_name.setColor(sf::Color::Yellow);

    // Load menu background
    m_texture.loadFromFile(std::filesystem::current_path().string() +
                           "/assets/loot_tracker.png");
    m_menu_background.setOrigin(sf::Vector2<float>(0.0f, 35.0f));
    m_menu_background.setTexture(m_texture);
  }

  void on_event() {
    switch (m_mode) {
    case game_mode_t::menu: {
      on_event_menu();
      break;
    }
    case game_mode_t::account_creation: {
      on_event_account_creation();
      break;
    }
    case game_mode_t::account_loading: {
      on_event_account_loading();
      break;
    }
    default: {
      break;
    }
    }
  }

  void on_render() {
    switch (m_mode) {
    case game_mode_t::menu: {
      on_render_menu();
      break;
    }
    case game_mode_t::account_creation: {
      on_render_account_creation();
      break;
    }
    case game_mode_t::account_loading: {
      on_render_account_loading();
      break;
    }
    default: {
      break;
    }
    }
  }

  void clear() {
    if (auto window = m_weak_render_window.lock()) {
      window->clear(sf::Color::Black);
    }
  }

private:
  void on_event_menu() {
    if (auto window = m_weak_render_window.lock()) {
      sf::Event event;
      while (window->pollEvent(event)) {

        if ((event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::Down)) {
          m_selection_index++;
          m_selection_index %=
              (static_cast<size_t>(menu_selection_t::exit_game) +
               1); // Wrap around highest enum value 0-n
        }
        if ((event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::Up)) {
          if (m_selection_index > 0) {
            m_selection_index--;
          } else {
            m_selection_index = static_cast<size_t>(
                menu_selection_t::exit_game); // Take the highest enum value
          }
        }

        if ((event.type == sf::Event::TextEntered) && event.text.unicode < 128) {
            m_new_character_name_input.setString(m_new_character_name_input.getString() + event.text.unicode);
        }

        if ((event.type == sf::Event::KeyPressed) &&
            (event.key.code == sf::Keyboard::Enter)) {
          switch (static_cast<menu_selection_t>(m_selection_index)) {
          case menu_selection_t::new_game: {
            m_mode = game_mode_t::account_creation;
            m_new_character_name.setString("Character Name: ");
            m_new_character_name.setPosition(sf::Vector2f(50.0f, 150.0f));
            break;
          }
          case menu_selection_t::existing_game: {
            m_mode = game_mode_t::account_loading;
            break;
          }
          case menu_selection_t::exit_game: {
            exit(0);
            break;
          }
          }
        }
        if (event.type == sf::Event::Closed) {
          window->close();
        }
      }

      [[likely]]
      if (m_true_rng_supported) {
        m_true_random_supported_text.setColor(sf::Color::Green);
        m_true_random_supported_text.setString(
            "Hardware true random will be used in this game and is supported "
            "by your x86_64 machine...");
      } else {
        m_true_random_supported_text.setColor(sf::Color::Red);
        m_true_random_supported_text.setString(
            "Hardware true random not supported by your x86_64 machine will be "
            "using the standard random...");
      }

      m_menu_text.setString(*(MENU_STRINGS.begin() + m_selection_index));
    }
  }

  void on_event_overworld() {}

  void on_event_battle() {}

  void on_event_account_creation() {}

  void on_event_account_loading() {}

  void on_render_account_creation() {
    if (auto window = m_weak_render_window.lock()) {
      window->draw(m_menu_background);
      window->draw(m_new_character_name);
    }
  }

  void on_render_account_loading() {
    if (auto window = m_weak_render_window.lock()) {
      window->draw(m_menu_background);
      window->draw(m_new_character_name);
    }
  }

  void on_render_overworld() {}

  void on_render_menu() {
    if (auto window = m_weak_render_window.lock()) {
      window->draw(m_menu_background);
      window->draw(m_true_random_supported_text);
      window->draw(m_title_screen_text);
      window->draw(m_menu_text);
      window->draw(m_credits_text);
    }
  }

  void on_render_battle() {}

  std::weak_ptr<sf::RenderWindow> m_weak_render_window;
  player_metadata_t m_player;
  sf::Font m_font;
  sf::Text m_true_random_supported_text;
  sf::Text m_title_screen_text;
  sf::Text m_menu_text;
  sf::Text m_credits_text;
  sf::Text m_new_character_name;
  sf::Text m_new_character_name_input;
  bool m_true_rng_supported;
  game_mode_t m_mode;
  size_t m_selection_index{0};
  sf::Texture m_texture;
  sf::Sprite m_menu_background;
};

#endif