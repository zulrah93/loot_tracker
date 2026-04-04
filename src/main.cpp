#include <game_state.hpp>

int main()
{
    std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 800), "Loot Tracker");
    game_state_t game_state(window);
    
    while (window->isOpen())
    {
        
        game_state.on_event();

        game_state.clear();

        game_state.on_render();
        
        window->display();
    }

    return 0;
}