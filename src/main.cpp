//Copyright 2025-2026 namenotfoundindb. All rights reserved

//TO DO: OTHER stuff and
//		Add reciept printing
//		Add the ability to scan cupons for discounts
//		Add search to tabs. When pressing '/' search product. For BASKET tab if hit enter will add to basket list first product. If starting with numbers then, auto move to search, for barcode scanners.
#include <ncurses.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <pqxx/pqxx>

//For reciept printing
#include <unistd.h>
#include <fcntl.h>
#include <format>

using namespace std;
using namespace pqxx;

#include "h_files/database_functions.h"
#include "h_files/definitions_and_variables.h"
#include "h_files/shop_data.h"
#include "h_files/binary_search.h"
#include "h_files/menus.h"
#include "h_files/colors.h"
#include "h_files/text.h"
#include "h_files/input_string.h"

using namespace std;
using namespace pqxx;

#define KEY_DELETE		8
#define KEY_ESC			27

long int tab_selections[] = {0, 0, 0};
double total = 0;

string conn_param =
        "dbname = " + database.db_name +
        " user = " + database.user +
        " password = " + database.password +
        " hostaddr = " + database.hostaddr +
        " port = " + database.port;

connection C(conn_param);

void print_product(product product) {
	printw("NAME = %s\n", product.name.c_str());
	printw("CODE = %ld\n", product.code);
	printw("PRICE = %2f\n", product.price);
	printw("DISCOUNT = %i\n", product.discount);
	printw("TAX = %c\n", product.tax);
	printw("STOCK = %2f\n", product.stock);
	printw("PRICE TO STOCK = %2f\n", product.price_to_stock);
	printw("SOLD BY = %i\n", product.sold_by);
}

void clear_messages() {
	int i, j;

	move(LINES - message_dist_from_bottom, message_x);

	for (i = 0; i < 2; i++) {
		for (j = 0; j < ROWS; j++) {
			printw(" ");
		}
		printw("\n");
	}

	move(LINES - message_dist_from_bottom, message_x);
}

void print_table_head_section(string name, int content_size, int color) {
	int i;

	if ((name.length() % 2) == 1 && (content_size % 2) != 1) {	//It dosen't like odd numbers
		name.push_back(' ');
	}

	printw("|");
	for (i = 0; i < int((content_size - name.length()) / 2); i++) {
		printw(" ");
	}

	if (!monochrome_mode) {
		attron(COLOR_PAIR(color));
		printw("%s", name.c_str());
		attroff(COLOR_PAIR(color));
	}
	else {
		printw("%s", name.c_str());
	}

	for (i = 0; i < int((content_size - name.length()) / 2); i++) {
		printw(" ");
	}
}

int nr_length(long int number, bool is_float) {
	if (is_float) {
		return to_string(number).length() + nr_decimal_points + 1;
	}
	else {
		return to_string(number).length();
	}
}

optional<string> add_modify_criteria(string criteria, string placeholder, short int color, bool modify, int max_size) {
	string output;
	int i;
	bool contains_commas = false;

	clear_messages();
	printw("%s ", text::info.product.c_str());

	attron(COLOR_PAIR(color));
	printw("%s", criteria.c_str());
	attroff(COLOR_PAIR(color));
	printw(": ");

	if (!modify) placeholder = "";

	auto auto_output = get_string(true, placeholder, color, max_size);

	if (auto_output) {
		output = auto_output.value();

		if (output.length() > max_size) {
			output.resize(max_size);
			printw("%s", output.c_str());
			getch();
		}

		for (i = 0; i < output.length(); i++) {
			if (output[i] == ',') contains_commas = true;
		}

		if (contains_commas) {
			clear_messages();
			move(LINES - message_section_lines, message_x);
			printw("%s %s", criteria.c_str(), text::errors.no_commas.c_str());
			getch();
			auto_output = add_modify_criteria(criteria, placeholder, color, modify, max_size);
			output = auto_output.value();
		}

		if (output == "") {
			output = "1";
		}

		return output;
	}
	else return nullopt;
}


void add_modify_product(long int product_code, bool modify, work& tx1) {
	string string_tax;
	bool canceled = false;

	product cur_product = get_product_data(product_code, tx1);

	string_tax = cur_product.tax;

	auto name = add_modify_criteria(text::product_criteria.name, cur_product.name, 0, modify, max_name_length);
	if (!name) return;
	
	auto code = add_modify_criteria(text::product_criteria.code, to_string(cur_product.code), UPC_pair, modify, max_code_length + 1);//+1 because of the barcode prefix
	if (!code) return;

	auto price = add_modify_criteria(text::product_criteria.price, to_string(cur_product.price), money_pair, modify, max_price_length);
	if (!price) return;

	auto discount = add_modify_criteria(text::product_criteria.discount_long, to_string(cur_product.discount), discount_pair, modify, 2);
	if (!discount) return;

	auto tax = add_modify_criteria(text::product_criteria.tax, string_tax, tax_pair, modify, 3);
	if (!tax) return;

	auto stock = add_modify_criteria(text::product_criteria.stock, to_string(cur_product.stock), stock_pair, modify, max_product_quanity_length);
	if (!stock) return;

	auto price_to_stock = add_modify_criteria(text::product_criteria.price_to_stock, to_string(cur_product.price_to_stock), pts_pair, modify, max_price_length);
	if (!price_to_stock) return;

	//Get sold_by
	short int selection = static_cast<mesurments>(cur_product.sold_by);
	int key;
	bool determined_selection = false;
	while (!determined_selection) {
		clear_messages();
		printw("%s: ", text::product_criteria.sold_by.c_str());
		printw("%s", text::info.sold_by[selection].c_str());

		key = getch();
		
		switch (key) {
			case KEY_UP:
			case 'k':
				if (selection) {selection--;}
				else {selection = nr_of_selling_options - 1;}
				break;

			case KEY_DOWN:
			case 'j':
				if (selection < nr_of_selling_options - 1) {selection++;}
				else {selection = 0;}
				break;

			case '\n':
				determined_selection = true;
				break;

			case 27:	//ESC
				return;
				break;
		}
	}

	code = remove_non_nr(code.value());
	price = remove_non_nr(price.value());
	discount = remove_non_nr(discount.value());
	stock = remove_non_nr(stock.value());
	price_to_stock = remove_non_nr(price_to_stock.value());	

	cur_product.name		= name.value();
	cur_product.code		= stol(code.value());
	cur_product.price		= stod(price.value());
	cur_product.discount		= stod(discount.value());
	cur_product.tax			= tax.value()[0];
	cur_product.stock		= stod(stock.value());
	cur_product.price_to_stock	= stod(price_to_stock.value());

	cur_product.profit		= cur_product.price - cur_product.price_to_stock;
	cur_product.sold_by		= mesurments(selection);

	update_stock(cur_product, tx1);
	tx1.commit();
}

void draw_product_list(work& tx1, const vector<long int>& list, int selection, bool print_name, bool print_code, bool print_price, bool print_discount, bool print_price_after_discount, bool print_tax, bool print_qty,bool print_stock, bool print_profit, bool print_price_to_stock, bool print_sold_by, bool print_total, double total) {
	int i, j, z, table_width = 0;

	//Calculate table witdth
	if (print_name) table_width += max_name_length + 1;
	if (print_code) table_width += max_code_length + 1;
	if (print_price) table_width += max_price_length + 1;
	if (print_discount) table_width += 3 + 1;
	if (print_price_after_discount) table_width += max_price_length + 1;
	if (print_tax) table_width += 3 + 1;
	if (print_qty) table_width += max_product_quanity_length + 1;
	if (print_stock) table_width += max_product_quanity_length +1;
	if (print_profit) table_width += max_price_length + 1;
	if (print_price_to_stock) table_width += max_price_length + 1;
	if (print_sold_by) table_width += max_sold_by_name_length + 1;
	table_width++;	//for the end pipe (|)

	int table_head_padding = 0;

	table_head_padding = int((table_width / 2.0) - (text::info.tabs[current_tab].length() / 2.0));

	for (i = 0; i < table_head_padding; i++) {
		printw("-");
	}

	printw("%s", text::info.tabs[current_tab].c_str());

	for (i = 0; i < table_head_padding; i++) {
		printw("-");
	}

	if (table_width % 2) {
		printw("-");
	}

	printw("\n");

	//Print table head
	if (print_name) {
		print_table_head_section(text::product_criteria.name, max_name_length, white_pair);
	}

	if (print_code) {
		print_table_head_section(text::product_criteria.code, max_code_length, UPC_pair);
	}

	if (print_price) {
		print_table_head_section(text::product_criteria.price, max_price_length, money_pair);
	}

	if (print_discount) {
		print_table_head_section(text::product_criteria.discount_short, 3, discount_pair);	//3 in max length because max discount is 99%
	}

	if (print_price_after_discount) {
		print_table_head_section(text::product_criteria.price_after_dis, max_price_length, discount_pair);
	}

	if (print_tax) {
		print_table_head_section(text::product_criteria.tax, 3, tax_pair);
	}

	if (print_qty) {
		print_table_head_section(text::product_criteria.quantity_short, max_product_quanity_length, QTY_pair);
	}

	if (print_stock) {
		print_table_head_section(text::product_criteria.stock, max_product_quanity_length, stock_pair);
	}

	if (print_profit) {
		print_table_head_section(text::product_criteria.profit, max_price_length, profit_pair);
	}

	if (print_price_to_stock) {
		print_table_head_section(text::product_criteria.price_to_stock, max_price_length, pts_pair);
	}

	if (print_sold_by) {
		print_table_head_section(text::product_criteria.sold_by, max_sold_by_name_length, white_pair);
	}

	printw("|\n");

	for (i = 0; i < table_width; i++) {
		printw("-");
	}
	printw("\n");

	for (i = 0; i < list.size(); i++) {

		product cur_product = get_product_data(list[i], tx1);

		if (i == selection) {
			attron(COLOR_PAIR(selection_pair));
		}

		printw("|");

		if (print_name) {
			printw("%s", cur_product.name.c_str());
			for (j = 0; j < (max_name_length - cur_product.name.length()); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_code) {
			printw("%ld", cur_product.code);

			for (j = 0; j < (max_code_length - nr_length(cur_product.code, false)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_price) {
			printw("%.2f", cur_product.price);
			for (j = 0; j < (max_price_length - nr_length(cur_product.price, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_discount) {
			printw("%i", cur_product.discount);
			for (j = 0; j < (3 - nr_length(cur_product.discount, false)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_price_after_discount) {
			printw("%.2f", cur_product.price_after_discount);
			for (j = 0; j < (max_price_length - nr_length(cur_product.price_after_discount, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_tax) {
			printw(" %c |", cur_product.tax);
		}

		if (print_qty) {
			for (z = 0; z < basket.size(); z++) {
				if (basket[z].code == cur_product.code) {
					printw("%.2f", basket[z].quantity);
					break;
				}
			}

			for (j = 0; j < (max_product_quanity_length - nr_length(basket[z].quantity, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_stock) {
			printw("%.2f", cur_product.stock);
			for (j = 0; j < (max_product_quanity_length - nr_length(cur_product.stock, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_profit) {
			printw("%.2f", cur_product.profit);
			for (j = 0; j < (max_price_length - nr_length(cur_product.profit, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_price_to_stock) {
			printw("%.2f", cur_product.price_to_stock);
			for (j = 0; j < (max_price_length - nr_length(cur_product.price_to_stock, true)); j++) {
				printw(" ");
			}
			printw("|");
		}

		if (print_sold_by) {
			printw("%s", text::info.sold_by[static_cast<short int>(cur_product.sold_by)].c_str());
			for (j = 0; j < (max_sold_by_name_length - text::info.sold_by[static_cast<short int>(cur_product.sold_by)].length()); j++) {
				printw(" ");
			}
			printw("|");
		}

		printw("\n");

		if (i == selection) {
			attroff(COLOR_PAIR(selection_pair));
		}

		for (z = 0; z < table_width; z++) {
			printw("-");
		}

		printw("\n");

	}

	if (print_total) {
		printw("|%s", text::info.total.c_str());
		for (z = 0; z < table_width - 3 - (text::info.total.length() + nr_length(total, true)); z++) {
			printw(" ");
		}

		printw("%.2f", total);
		printw(" |\n");

		for (z = 0; z < table_width; z++) {
			printw("-");
		}
	}
}

//A function like init_color but with normal rgb(0 - 255) instead of ncurses rgb (0 - 1000)
void make_color(short int color, short int red, short int green, short int blue) {
	init_color(color, int((red / 255.0) * 1000.0), float((green / 255.0) * 1000.0), int((blue / 255.0) * 1000.0));
}

void init_colors() {
	init_pair(white_pair, COLOR_WHITE, COLOR_BLACK);
	init_pair(money_pair, COLOR_GREEN, COLOR_BLACK);
	init_pair(UPC_pair, COLOR_YELLOW, COLOR_BLACK);
	init_pair(QTY_pair, COLOR_CYAN, COLOR_BLACK);
	init_pair(discount_pair, COLOR_RED, COLOR_BLACK);

	make_color(COLOR_ORANGE, 240, 107, 12);
	init_pair(tax_pair, COLOR_ORANGE, COLOR_BLACK);

	make_color(COLOR_PRICE, 109, 245, 91);
	init_pair(profit_pair, COLOR_PRICE, COLOR_BLACK);

	init_pair(pts_pair, COLOR_MAGENTA, COLOR_BLACK);

	make_color(COLOR_STOCK, 133, 26, 7);
	init_pair(stock_pair, COLOR_STOCK, COLOR_BLACK);

	make_color(COLOR_REAL_BLACK, 0, 0, 0);
	init_pair(black_pair, COLOR_REAL_BLACK, COLOR_BLACK);

	init_pair(selection_pair, COLOR_BLACK, COLOR_WHITE);
}

long int current_list_size(work& tx) {

	long int size = 0;


	switch (current_tab) {
		case PRODUCTS:
			size = stock_size(tx);
			break;

		case BASKET:
			size = basket.size();
			break;

		default:
			size = stock_size(tx);
			break;
	}

	return size;
}

struct {
	long int start	= 0;
	long int end	= 0;
} visible_product_list;

vector<long int> calculate_visible_product_list(work& tx) {

	long int max_products_nr = int((LINES - 3 - message_section_lines) / 2);

	if (current_tab == BASKET) max_products_nr--;	//Because in the BASKET tab it prints the total.

	if (max_products_nr >= current_list_size(tx)) {
		visible_product_list.end = current_list_size(tx);
	}

	else {
		visible_product_list.end = max_products_nr + visible_product_list.start;
	}

	int i;
	vector<long int> list, codes;

	codes = code_list(tx);

	if (current_tab == PRODUCTS || current_tab == STOCK) {
		for (i = visible_product_list.start; i < visible_product_list.end; i++) {
			list.push_back(codes[i]);
		}
	}

	else if (current_tab == BASKET) {
		for (i = visible_product_list.start; i < visible_product_list.end; i++) {
			list.push_back(basket[i].code);
		}
	}
	return list;
}

void add_product_to_basket(product product, double quantity) {
	bool product_already_in_basket;
	int j, i;

	for (j = 0; j < basket.size() ; j++) {
		if (basket[j].code == product.code) {
			product_already_in_basket = true;
			basket[j].quantity += quantity;
			break;
		}

		else product_already_in_basket = false;
	}

	if (!product_already_in_basket) {
		basket.push_back({product.code, quantity});
	}
}

string append_nr(int i, string str = " ") {
	string output;
	for (int j = 0; j < i; j++) {
		output.append(str);
	}
	return output;
}

bool print_receipt(work& tx) {
	if (!basket.size()) {
		clear_messages();
		printw("%s", text::errors.no_items_in_basket.c_str());
		getch();
		return false;
	}

	int fd = open("/dev/usb/lp3", O_WRONLY);
	if (fd < 0) {
		clear_messages();
		printw("%s", text::errors.receipt_printing.c_str());
		getch();
		return false;
	}

	string data = "\x1b\x40";		//Initialize printer

	data.append("\x1b\x61\01" + shop_data.name + "\n" + 	//Centered shop name
			shop_data.addres + "\n" + 
			text::info.shop_code + shop_data.code + "\n\n\n" + 
			"\x1b\x40\x1b\x61\x00");

	data.append(append_nr(max_name_length + 1, "-") + "\n");
	for (int i = 0; i < basket.size(); i++) {
		product cur_product = get_product_data(basket[i].code, tx);
		data.append(cur_product.name + 
				append_nr(cur_product.name.length() - max_name_length) + "\n" +
				"  " + std::format("{0:.2f} x {1:.3f} {2:s} = {3:.2f}", cur_product.price, basket[i].quantity, text::info.sold_by[int(cur_product.sold_by)], cur_product.price * basket[i].quantity) + 
				append_nr(max_name_length - (4 + to_string(std::format("{0:.2f} x {1:.3f} {2:s} = {3:.2f}", cur_product.price, basket[i].quantity, text::info.sold_by[int(cur_product.sold_by)], cur_product.price * basket[i].quantity)).length())) + 
				std::format("{0:c}", cur_product.tax) + 
				"\n");


		if (cur_product.discount) {
			data.append(text::product_criteria.discount_long + std::format(" {0:}%: -{1:.2f}", cur_product.discount, ((cur_product.price / 100) * cur_product.discount) * basket[i].quantity) + "\n");
		}
		data.append(append_nr(max_name_length + 1, "-") + "\n");
	}
	data.append("\x1b\x21\x20" +		//wide text
			std::format("{0:s}: {1:.2f}", text::info.total, total) 	//print total
			+ "\n" + "\x1b\x40");
	data.append(append_nr(nr_of_newlines, "\n"));

	write(fd, data.c_str(), data.length());
	close(fd);

	clear_messages();
	printw("%s", text::info.reprint_receipt.c_str());
	int key = getch();
	if (key == 'y') {
		return print_receipt(tx);
	}

	return true;
}

int main() {
	initscr();

	if (has_colors()) {
		start_color();
	}
	else {
		printw("%s", text::errors.terminal_no_color.c_str());
		monochrome_mode = true;
		getch();
		clear();
	}

	cbreak();
	keypad(stdscr, TRUE);
	noecho();

	getmaxyx(stdscr, LINES, ROWS);

	init_colors();

	int i, j = 0;
	long int selection = 0;
	long int indexed_selection = 0;
	long int max_products_nr;
	string search_word;
	bool search_for_name = true;

	product product;

	vector<long int> list;
	vector<long int> search_list;
	vector<long int> product_codes;

	current_tab = PRODUCTS;

	while (true) {
		clear();

		list.clear();
		work tx(C);
		product_codes = code_list(tx);

		list = calculate_visible_product_list(tx);

		max_products_nr = int((LINES - 3 - message_section_lines) / 2);

		indexed_selection = visible_product_list.start + selection;

		for (i = 0; i < stock_size(tx); i++) {
			product = get_product_data(product_codes[i], tx);
		}

		total = 0;
		for (i = 0; i < basket.size(); i++) {
			total += get_product_data(basket[i].code, tx).price_after_discount * basket[i].quantity;
		}

		switch (current_tab) {
			case PRODUCTS:
				draw_product_list(tx, list, selection, true, true, true, true, false, true, false, false, false, false, false, false, 0);
				break;

			case BASKET:
				draw_product_list(tx, list, selection, true, true, true, true, true, true, true, false, false, false, true, true, total);
				break;

			case STOCK:
				draw_product_list(tx, list, selection, true, true, true, true, false, true, false, true, true, true, true, false, 0);
		}

		tx.commit();

		key = getch();

		if (key == 'q') {
			if (basket.size()) {
				move(LINES - message_dist_from_bottom, message_x);
				printw("%s", text::errors.basket_not_empty.c_str());

				key = getch();

				if (key == 'y' || key == 'Y') {
					endwin();
					return 0;
				}
			}

			else {
				endwin();
				return 0; 
			}
		}

		if (key == KEY_ESC) start_main_menu();

		if (key == 'R') {
			work tx_remove(C);
			if (current_list_size(tx_remove)) {
				tx_remove.commit();
				if (selection == visible_product_list.end) {
					selection--;

					work tx2(C);
					if (int((LINES - 3 - message_section_lines) / 2.0) < current_list_size(tx2) <= current_list_size(tx2)) {
						visible_product_list.start--;
						visible_product_list.end--;
					}
				}

				if (current_tab == PRODUCTS || current_tab == STOCK) {
					work tx3(C);
					erase(list[selection], tx3);
					tx3.commit();
				}

				else if (current_tab == BASKET) {
					for (i = 0; i < basket.size(); i++) {
						if (basket[i].code == list[selection]) {
							basket[i].quantity--;
							break;
						}
					}
					if (basket[i].quantity == 0) {
						basket.erase(basket.begin() + i);
					}

					if (!basket.size()) selection == 0;
				}
			}
		}

		if (key == 'a') {
			if (current_tab == PRODUCTS || current_tab == STOCK) {

				work tx4(C);

				if (stock_size(tx4) > 0) {
					add_modify_product(list[selection], false, tx4);
				}
				else {
					add_modify_product(list[selection], false, tx4);
				}

				tx4.commit();
			}
		}

		if (key == '\t') {
			if (current_tab == BASKET) {
				current_tab = PRODUCTS;
			}

			else if (current_tab == PRODUCTS) {
				current_tab++;
			}

			else if (current_tab == STOCK) {
				current_tab = PRODUCTS;
			}

			work txn(C);

			if (selection >= current_list_size(txn)) {
				visible_product_list.start = 0;
				indexed_selection = 0;
				selection = 0;
			}

			txn.commit();
		}

		if (key == 'r') current_tab = STOCK;

		if (key == 'm') {

			work tx6(C);

			if (current_list_size(tx6)) {
				add_modify_product(list[selection], true, tx6);	//TRUE for modify
			}

			else {
				clear_messages();
				move(LINES - message_dist_from_bottom, message_x);
				printw("%s", text::errors.no_product_to_modify.c_str());
				getch();
			}

			tx6.commit();
		}

		if (key == 'w')  {
			if (visible_product_list.start != 0) {							//Check if visible product window will not go off screen
				visible_product_list.start--;
				visible_product_list.end--;
			}
		}

		if (key == 's') {
			work tx8(C);

			if (visible_product_list.end != current_list_size(tx8)) {		//Same down here
				visible_product_list.start++;
				visible_product_list.end++;
			}

			tx8.commit();
		}

		if (key == '\n') {
			if (current_tab == PRODUCTS) {
				work tx5(C);
				add_product_to_basket(get_product_data(list[selection], tx5), 1);

				tx5.commit();
				current_tab = BASKET;

				if (indexed_selection >= basket.size()) {
					visible_product_list.start = 0;
					indexed_selection = 0;
					selection = 0;
				}
			}
		}

		if (key == '/') {
			work tx_search(C);
			long int index = 0;

			clear_messages();
			printw("%s", text::info.search.search.c_str());

			auto search_word = get_string();

			if (search_word) {

				index = binary_search(name_list(tx_search), search_word.value());

				clear();
				draw_product_list(tx_search, {product_codes[index]}, 0, true, true, true, true, false, true, false, false, false, false, false, false, 0);

				key = getch();

				switch (key) {
					case 'm':
						add_modify_product(product_codes[index], true, tx_search);
						break;

					case '\n':
						add_product_to_basket(get_product_data(product_codes[index], tx_search), 1);
						current_tab = BASKET;

						if (indexed_selection >= basket.size()) {
							visible_product_list.start = 0;
							indexed_selection = 0;
							selection = 0;
						}
						break;
				}
			}
		}

		if (key == 't') {
			/*printw("Testing menus!");
			getch();

			test_menu();*/

			/*work tx_tax_rates(C);
			endwin();
			tax_rates = get_tax_rates(tx_tax_rates);

			for (auto tax : tax_rates) {
				cout << "tax " << tax.first << " is " << tax.second << endl;
			}
			return 1;*/

			work tx_test(C);
			clear_messages();
			printw("Printing receipt");
			getch();
			print_receipt(tx_test);
		}

		if (key == 'T') {
			work tx_checkout(C);
			clear_messages();
			printw("Printing receipt");
			getch();
			//Clear basket and total for next customer
			//Also gonna implement in the future so that it records transactions
			if (print_receipt(tx_checkout)) {
				basket = {};
				total = 0;
			}
		}

		if (key == KEY_UP || key == 'k') {
			if (selection != 0) {
				selection--;
			}

			if (visible_product_list.start != 0 && selection == 0) {
				visible_product_list.start--;
				visible_product_list.end--;
				selection++;
			}
		}

		if (key == KEY_DOWN || key == 'j') {
			work tx7(C);
			if (selection < list.size() - 1 && indexed_selection < current_list_size(tx7)) {
				selection++;
			}

			if (visible_product_list.end != current_list_size(tx7) && selection == list.size() - 1) {
				visible_product_list.start++;
				visible_product_list.end++;
				selection--;
			}

			tx7.commit();
		}

		if (key == barcode_prefix)  {
			long int code;
			code = stol(remove_non_nr(get_string(false).value()));	
			work tx_scanner(C);

			if (current_tab == BASKET || current_tab == PRODUCTS) {
				if (product_exists(code, tx_scanner)) {
					add_product_to_basket(get_product_data(code, tx_scanner), 1);
					current_tab = BASKET;
				}

				else {
					clear_messages();
					printw("%s", text::errors.product_not_found.c_str());
					getch();
				}
			}

			else if (current_tab == STOCK) {
				//Search for product	
			}

			if (selection >= current_list_size(tx_scanner)) {
				visible_product_list.start = 0;
				indexed_selection = 0;
				selection = 0;
			}
			tx_scanner.commit();
		}

	}

	getch();

	endwin();

	return 0;
}
