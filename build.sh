cp assets/* bin/
clang++-19 src/main.cpp -o bin/loot_tracker_game -cxx-isysteminclude -Wall --std=c++23 -lsfml-graphics -lsfml-window -lsfml-system -DDEBUG