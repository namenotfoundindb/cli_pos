#include <ncurses.h>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <variant>

using namespace std;

#include "h_files/colors.h"
#include "h_files/text.h"
#include "h_files/shop_data.h"
#include "h_files/input_string.h"
#include "h_files/definitions_and_variables.h"


#define KEY_ESC 27

#define PRODUCTS	0
#define SETTINGS	1
#define INFO		2
#define QUIT		3

#define SHOP		0
#define DATABASE	1
#define COLORSCHEME	2


namespace test {
	string s =	"Hello world";
	int i =		10;
	float f =	5.98;
	bool b = 	true;

	void test_function() {
		clear();
		printw("Test function\n");
		getch();
	}
}

struct menu;

namespace menu_type {
	struct submenu {
		menu* submenu;
	};

	struct string_item {
		string* value;
	};
	
	struct funct {
		function<void()> action;
	};

	struct int_item {
		int* value;
	};

	struct float_item {
		float* value;
	};

	struct bool_item {
		bool* value;
	};

	struct exit_item {
		int* return_value = 0;
	};
};

using menu_item_data = variant<
	menu_type::submenu,
	menu_type::string_item,
	menu_type::funct,
	menu_type::int_item,
	menu_type::float_item,
	menu_type::bool_item,
	menu_type::exit_item
>;

struct menu_item {
	string label;
	menu_item_data data;
	short int color = white_pair;
	string description = "";
};

struct menu {
	vector<menu_item> options;
	string name = "";
	bool centered = false;
};

void get_barcode_prefix() {
	clear();
	printw("%s\n", text::menus.main.settings.peripherals.barcode_scanner.scan_a_code.c_str());
	printw("%s%c", text::menus.main.settings.peripherals.barcode_scanner.current_prefix.c_str(), barcode_prefix);
	auto s = get_string(false);
	if (!s) return;

	barcode_prefix = s.value()[0];
}

menu submenu {
	{
		{"exit", menu_type::exit_item{&test::i}}
	}
};

menu basic {
	{
		{"function", 	menu_type::funct{test::test_function}},
		{"submenu",	menu_type::submenu{&submenu}}	
	},
	"Basic menu functions"
};

menu shop_settings {
	{
		{text::menus.main.settings.shop.name,		menu_type::string_item{&shop_data.name}},
		{text::menus.main.settings.shop.code, 		menu_type::string_item{&shop_data.code}},
		{text::menus.main.settings.shop.addres,		menu_type::string_item{&shop_data.addres}}
	}
};

menu printer_settings {
	{
		{text::menus.main.settings.peripherals.receipt_printer.printer_device_file,	menu_type::string_item{&printer_device_file}},
		{text::menus.main.settings.peripherals.receipt_printer.cpl,			menu_type::int_item{&max_name_length}},
		{text::menus.main.settings.peripherals.receipt_printer.newlines, 		menu_type::int_item{&nr_of_newlines}, white_pair, text::menus.main.settings.peripherals.receipt_printer.newlines_description}
	}
};

menu barcode_scanner {
	{
		{text::menus.main.settings.peripherals.barcode_scanner.prefix, 			menu_type::funct{get_barcode_prefix}}
	}
};

menu peripherals {
	{
		{text::menus.main.settings.peripherals.receipt_printer_label,	menu_type::submenu{&printer_settings}},
		{text::menus.main.settings.peripherals.barcode_scanner_label, 	menu_type::submenu{&barcode_scanner}}
	}
};

menu settings {
	{
		{text::menus.main.settings.shop_label,		menu_type::submenu{&shop_settings}},
		{text::menus.main.settings.peripherals_label, 	menu_type::submenu{&peripherals}},
		{text::menus.main.settings.colorscheme_label,	menu_type::submenu{&submenu}},
		{text::menus.main.settings.users_label,		menu_type::submenu{&submenu}}
	}
};

menu main_menu {
	{
		{text::menus.main.products_label,	menu_type::exit_item{}},
		{text::menus.main.settings_label,	menu_type::submenu{&settings}}
	},
	text::program.name,
	true
};

menu variables {
	{
		{"string",	menu_type::string_item{ &test::s }, discount_pair},
		{"bool",	menu_type::bool_item{&test::b}, money_pair},
		{"int", 	menu_type::int_item{&test::i},  UPC_pair},
		{"float", 	menu_type::float_item{&test::f}, QTY_pair}
	},
	"Variables"
};

menu menu_test {
	{
		{"Basic", menu_type::submenu{&basic}},
		{"Variables", menu_type::submenu{&variables}}
	},	
	"Testing menu",
	true
};

int max(const vector<string>& s) {
	int max_length = 0;
	int i;
	for (i = 0; i < s.size(); i++) {
		if (s[i].length() > max_length) max_length = s[i].length();
	}
	return max_length;
}

int max_menu_options_length(const menu& menu) {
	int max_length = 0;
	for (int i = 0; i < menu.options.size(); i++) {
		if (menu.options[i].label.length() > max_length) max_length = menu.options[i].label.length();
	}
	return max_length;
}

namespace draw_menus {
	optional<int> selection_list(const menu& menu, int selection = 0, bool can_return_nullopt = true, short int space_in_betwen = 0) {
		int i, j, padding;
		int initial_y, initial_x, current_y;
		int options_size = menu.options.size();
		int terminal_middle = int(COLS / 2);
		int key;

		getyx(stdscr, initial_y, initial_x);

		int max_length = max_menu_options_length(menu) + 2;

		while (true) {
			for (i = 0; i < menu.options.size(); i++) {
				if (menu.centered) {
					padding = int((COLS - menu.options[i].label.length()) / 2);
				}
				else padding = 0;

				current_y = initial_y + i + (space_in_betwen * i);

				move(current_y, initial_x + padding);

				if (!menu.centered) {
					visit([&](auto&& data) {
						using T = decay_t<decltype(data)>;

						if constexpr (is_same_v<T, menu_type::submenu>) {
						printw(">");
						}

						else if constexpr (is_same_v<T, menu_type::funct>) {
						addch(ACS_LLCORNER);
						}

						else addch(ACS_BULLET);

						}, menu.options[i].data);
				}

				attron(COLOR_PAIR(menu.options[i].color));
				if (i == selection) attron(A_REVERSE);

				printw(" %s", menu.options[i].label.c_str());
				if (i == selection) attroff(A_REVERSE);
				attroff(COLOR_PAIR(menu.options[i].color));
				
				visit([&](auto&& data) {
					using T = decay_t<decltype(data)>;

					if constexpr (is_same_v<T, menu_type::string_item>) {
						move(current_y, terminal_middle);  
						printw("%s", data.value -> c_str());
					}

					if constexpr (is_same_v<T, menu_type::bool_item>) {
						move(current_y, terminal_middle);  
						printw("%s", text::menus.false_true[*data.value].c_str());
					}

					if constexpr (is_same_v<T, menu_type::int_item>) {
						move(current_y, terminal_middle);  
						printw("%i", *data.value);
					}

					if constexpr (is_same_v<T, menu_type::float_item>) {
						move(current_y, terminal_middle);  
						printw("%.3f", *data.value);
					}
				}, menu.options[i].data);

				if (!menu.options[i].description.length()) {
					move(current_y + 1, terminal_middle);
					printw("%s", menu.options[i].description.c_str());
				}
			}

			key = getch();
			switch (key) {
				case KEY_UP:
				case 'k':
					if (selection) selection--;
					break;

				case KEY_DOWN:
				case 'j':
					if (selection != options_size - 1) selection++;
					break;

				case '\n':
					return selection;
					break;

				case KEY_ESC:
					if (can_return_nullopt) return nullopt;
					break;
			}
		}
	}

	optional<int> dropdown(const vector<string>& options, const vector<short int>& colors = {}, bool can_return_nullopt = false, int selection = 0) {
		int key, i;
		int initial_y, initial_x;
		int max_length = max(options);
		bool use_color = true;

		if (!colors.size()) use_color = false;

		getyx(stdscr, initial_y, initial_x);

		while (true) {;
			move(initial_y, initial_x);

			if (use_color) attron(COLOR_PAIR(colors[selection]));
			printw("%s", options[selection].c_str());

			for (i = 0; i < max_length - options[selection].length(); i++) {
				printw(" ");
			}

			if (use_color) attroff(COLOR_PAIR(colors[selection]));

			key = getch();
			switch (key) {
				case KEY_UP:
				case 'k':
					if (selection) selection--;
					else selection = options.size() - 1;
					break;

				case KEY_DOWN:
				case 'j':
					if (selection != options.size() - 1) selection++;
					else selection = 0;
					break;

				case '\n':
					return selection;
					break;

				case KEY_ESC:
					if (can_return_nullopt) return nullopt;
					break;
			} 
		}
	}
}

void print_centered(const string& s) {
	int initial_y, initial_x;
	int max_x = getmaxx(stdscr);
	getyx(stdscr, initial_y, initial_x);

	move(initial_y, int((max_x - s.length()) / 2));
	printw("%s", s.c_str());
}

bool run_menu(const menu& menu, vector<string> menu_labels = {""}) {
	int i, selection = 0;
	bool exit = false;

	while (true) {
		if (exit) return true;	

		clear();
		if (menu.centered) print_centered(menu.name);
		else {
			for (i = 0; i < menu_labels.size(); i++) {
				if (i) printw("> ");
				printw("%s ",menu_labels[i].c_str());
			}
			move(2, 0);
		}

		int terminal_middle = int(COLS / 2);

		auto output = draw_menus::selection_list(menu, selection);
		if (!output) return exit;
		selection = output.value();

		auto& selected = menu.options[*output];

		visit([&](auto&& data) {
			using T = decay_t<decltype(data)>;

			if constexpr (is_same_v<T, menu_type::submenu>) {
				menu_labels.push_back(menu.options[*output].label);
				exit = run_menu(*data.submenu, menu_labels);
				//Delete the last menu label when coming back from its submenu
				menu_labels.erase(menu_labels.end());
			}

			if constexpr (is_same_v<T, menu_type::funct>) {
				data.action();
			}

			if constexpr (is_same_v<T, menu_type::bool_item>) {
				*data.value = !*data.value;
			}

			if constexpr (is_same_v<T, menu_type::string_item>) {
				move(2 + output.value(), terminal_middle);
				auto str = get_string(true, *data.value);
				if (str) *data.value = str.value();
			}

			if constexpr (is_same_v<T, menu_type::int_item>) {
				move(2 + output.value(), terminal_middle);
				auto str = get_string(true, to_string(*data.value));
				if (str && stoi(remove_non_nr(str.value()))) *data.value = stoi(remove_non_nr(str.value()));
			}

			if constexpr (is_same_v<T, menu_type::float_item>) {
				move(2 + output.value(), terminal_middle);
				auto str = get_string(true, to_string(*data.value));
				if (str && stof(remove_non_nr(str.value()))) *data.value = stof(remove_non_nr(str.value()));
			}

			if constexpr (is_same_v<T, menu_type::exit_item>) {
				exit = true;
			}
		}, selected.data);
	}

	return false;
}

void test_menu() {
	clear();
	//run_menu(menu_test);
	run_menu(main_menu);
}

void start_main_menu() {
	run_menu(main_menu);
}
