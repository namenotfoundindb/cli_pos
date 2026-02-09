#include <string>
#include <ncurses.h>
#include <optional>

#include "h_files/colors.h"

#define KEY_DELETE		8
#define KEY_ESC			27

using namespace std;

optional<string> get_string(bool show_text = true, string s = "", short int color_pair = white_pair, short int max_length = 0) {
	int key;
	int x,y, initial_x, initial_y, j;

	getyx(stdscr, initial_y, initial_x);

	attron(COLOR_PAIR(color_pair));
	attron(A_REVERSE);
	printw("%s", s.c_str());
	attroff(A_REVERSE);

	if (s != "") {
		key  = getch();

		if (key != '\n' && s != "") {
			move(initial_y, initial_x);
			for (j = 0; j < s.length(); j++) {
				printw(" ");
			}
			s = "";
			move(initial_y, initial_x);
		
			if (isprint(static_cast<char> (key))) {
				s.push_back(key);
				if (show_text) addch(key);
				refresh();
			}
		}

		else {
			attroff(COLOR_PAIR(color_pair));
			return s;
		}
	}
	while (true) {
		if (max_length) {
			attron(A_REVERSE);
			move(initial_y, initial_x + max_length);
			printw(" ");
			move(initial_y, initial_x + s.length());
			attroff(A_REVERSE);
		}
		else max_length = -1;

		key = getch();
		getyx(stdscr, y, x);

		if (key == KEY_BACKSPACE || key == KEY_DELETE) {
			if (s.length()) {
				s.pop_back();
				mvaddch(y, x - 1, ' ');
				move(y, x - 1);
				refresh();
			}
		}

		if (key == '\n') {
			attroff(COLOR_PAIR(color_pair));
			return s;
		}

		if (key == KEY_ESC) {
			attroff(COLOR_PAIR(color_pair));
			return nullopt;
		}

		if (isprint(static_cast<char> (key))) {
			if (s.length() < max_length) {
				s.push_back(key);
				if (show_text) {
					addch(key);
					move(y, x + 1);
				}	
			}
			
		}
	}
	return s;
}

string remove_non_nr(string input) {
	short int i;
	string output;

	for (i = 0; i < input.length(); i++) {

		if (48 <= input[i] && input[i] <= 57 || input[i] == 46) {
			output.append(input, i, 1);
		}
	}
	if (output == "") output = "0";
	return output;
}

