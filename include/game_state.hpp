#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <memory>
#include <SFML/Graphics.hpp>
#include <player_metadata_t.hpp>
#include <monster_t.hpp>
#include <filesystem>
#include <true_rng_t.hpp>

enum game_mode_t : uint8_t {
    menu,
    overworld, // walking around not in combat 
    battling // new game state where you are in combat not in the overworld
};

enum menu_selection_t : uint8_t {
    new_game = 0,
    existing_game,
    exit_game
};

static const std::initializer_list<std::string> MENU_STRINGS = {"[*] New Game\n[ ] Load Exisiting Game\n[ ] Exit", 
                                                                "[ ] New Game\n[*] Load Exisiting Game\n[ ] Exit",
                                                                "[ ] New Game\n[ ] Load Exisiting Game\n[*] Exit" };

class game_state_t {
    public:
        game_state_t(std::weak_ptr<sf::RenderWindow> weak_render_window) : m_weak_render_window{weak_render_window}, 
                                                        m_player{}, m_true_rng_supported{true_rng_t::rdseed64_instruction_supported()}, m_mode{game_mode_t::menu} {
            //Load font
            m_font.loadFromFile(std::filesystem::current_path().string() + "/assets/GohuFontuni14NerdFont-Regular.ttf");
            m_true_random_supported_text.setFont(m_font);
            m_true_random_supported_text.setPosition(sf::Vector2<float>(0.0f, 0.0f));
            m_true_random_supported_text.setCharacterSize(9);
            //Initial title screen setup
            m_title_screen_text.setFont(m_font);
            m_title_screen_text.setPosition(sf::Vector2<float>(340.0f, 100.0f));
            m_title_screen_text.setCharacterSize(20);
            m_title_screen_text.setString("Loot Tracker");
            //Initial menu text setup
            m_menu_text.setFont(m_font);
            m_menu_text.setPosition(sf::Vector2<float>(320.0f, 140.0f));
            m_menu_text.setCharacterSize(15);
            m_menu_text.setString(*MENU_STRINGS.begin());   
        }

        void on_event() {
            switch (m_mode) {
                case game_mode_t::menu: {
                    on_event_menu();
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
                default: {
                    break;
                }
            }
        }

        void clear() {
            if (auto window = m_weak_render_window.lock()) {
                window->clear(sf::Color::Black);
                if (m_mode == game_mode_t::menu) {
                    window->draw(m_true_random_supported_text);
                    window->draw(m_title_screen_text);
                    window->draw(m_menu_text);
                }
            }   
        }
private:
        void on_event_menu() {
            if (auto window = m_weak_render_window.lock()) {
                sf::Event event;
                while (window->pollEvent(event))
                {
                    
                    if ((event.type == sf::Event::KeyPressed) 
                                && (event.key.code == sf::Keyboard::Down)) {
                        m_selection_index++;
                        m_selection_index %= (static_cast<size_t>(menu_selection_t::exit_game) + 1); // Wrap around highest enum value 0-n
                    }
                    if ((event.type == sf::Event::KeyPressed) 
                                && (event.key.code == sf::Keyboard::Up)) {
                        if (m_selection_index > 0) {
                            m_selection_index--;
                        }
                        else {
                            m_selection_index = static_cast<size_t>(menu_selection_t::exit_game); // Take the highest enum value
                        }
                        
                    }                  
                    if (event.type == sf::Event::Closed) {
                        window->close();
                    }
                }

                m_menu_text.setString(*(MENU_STRINGS.begin() + m_selection_index));
            }
        }

        void on_event_overworld() {

        }

        void on_event_battle() {

        }

        void on_render_overworld() {

        }

        void on_render_menu() {
             if (auto window = m_weak_render_window.lock()) {
                
                [[likely]]
                if (m_true_rng_supported) {
                    m_true_random_supported_text.setColor(sf::Color::Green);
                    m_true_random_supported_text.setString("Hardware true random will be used in this game and is supported by your x86_64 machine..." );
                }
                else {
                    m_true_random_supported_text.setColor(sf::Color::Red);
                    m_true_random_supported_text.setString("Hardware true random not supported by your x86_64 machine will be using the standard random...");
                }
            }
        }

        void on_render_battle() {

        }

        std::weak_ptr<sf::RenderWindow> m_weak_render_window;
        player_metadata_t m_player;
        sf::Font m_font;
        sf::Text m_true_random_supported_text;
        sf::Text m_title_screen_text;
        sf::Text m_menu_text;
        bool m_true_rng_supported;
        game_mode_t m_mode;
        size_t m_selection_index{0};
};

#endif