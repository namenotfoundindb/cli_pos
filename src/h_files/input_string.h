#pragma once
#include <ncurses.h>
#include <string>
#include <optional>

#include "colors.h"

using namespace std;

optional<string> get_string(bool show_text = true, string s = "", short int color_pair = white_pair, short int max_length = 0);
string remove_non_nr(string input);
