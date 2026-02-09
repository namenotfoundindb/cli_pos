#include <string>
#include <pqxx/pqxx>

#include "h_files/definitions_and_variables.h"
#include <map>

using namespace std;
using namespace pqxx;

#define PRODUCTS		0
#define BASKET			1
#define STOCK			2

bool monochrome_mode = false;

int message_x = 0;
int message_dist_from_bottom = 2;
int nr_of_newlines = 15;

int key;

int LINES, ROWS;

int max_name_length = 				31;	//It has a problem with odd numbers;
int max_code_length = 				13;	//For Universal Product Code A
int max_product_quanity_length = 	8;
int max_price_length = 				6;
short int nr_decimal_points =		2;
short int nr_of_selling_options = 	3;
short int max_sold_by_name_length =	7;
vector<basket_product>	basket;

int current_tab = PRODUCTS;

int message_section_lines = 2;
int barcode_prefix = '#';

map<char, short int> tax_rates;

string printer_device_file = "/dev/usb/lp3";
