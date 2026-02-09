#pragma once

#include <string>
#include <pqxx/pqxx>

#include <map>

using namespace std;
using namespace pqxx;

#define PRODUCTS		0
#define BASKET			1
#define STOCK			2

extern bool monochrome_mode;

struct basket_product {
	long int code;
	double quantity;
};

extern int message_x;
extern int message_dist_from_bottom;

extern int key;

extern int LINES, ROWS;

extern int max_name_length;
extern int max_code_length; 
extern int max_product_quanity_length; 
extern int max_price_length; 
extern short int nr_decimal_points; 
extern short int nr_of_selling_options; 
extern short int max_sold_by_name_length; 
extern vector<basket_product>	basket;

extern int nr_of_newlines;

extern int current_tab; 

extern int message_section_lines; 
extern int barcode_prefix; 

extern map<char, short int> tax_rates;

extern string printer_device_file; 

extern string conn_param;

enum mesurments {
	PIECE,
	LENGTH,
	WEIGHT
};

struct product {
	string name;
	long int code;
	double price;
	int discount;
	double price_after_discount;
	short int tax;
	double stock;
	double profit;
	double price_to_stock;

	enum mesurments sold_by;
};

struct {
        string db_name = 		"shop";
        string user = 			"test";
        string password = 		"test123";
        string hostaddr =	 	"192.168.0.163";
        string port = 			"5432";
        string products_table = 	"products";
	string tax_rates_table = 	"tax_rates";
} database;

