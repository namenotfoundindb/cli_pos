#include <ncurses.h>
#include <string>
#include <vector>

#include "esentials.h"
#include "definitions_and_variables.h"
#include "database_functions.h"

using namespace std;

int nr_length(long int number, bool is_float) {
	if (is_float) {
		return to_string(number).length() + nr_decimal_points + 1;
	}
	else {
		return to_string(number).length();
	}
}

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

        if ((name.length() % 2) == 1 && (content_size % 2) != 1) {      //It dosen't like odd numbers
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

void draw_product_list(const vector<long int>& list, int selection, bool print_name, bool print_code, bool print_price, bool print_discount, bool print_price_after_discount, bool print_tax, bool print_qty,bool print_stock, bool print_profit, bool print_price_to_stock, bool print_sold_by, bool print_total, double total) {
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

	work tx1(C);

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

	tx1.commit();

}


