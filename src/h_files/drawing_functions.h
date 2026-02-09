#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

#include "esentials.h"

using namespace std;

void print_product(product product);
void clear_messages();
void draw_product_list(const vector<long int>& list, int selection, bool print_name, bool print_code, bool print_price, bool print_discount, bool print_price_after_discount, bool print_tax, bool print_qty,bool print_stock, bool print_profit, bool print_price_to_stock, bool print_sold_by, bool print_total, double total);
