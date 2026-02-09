#pragma once

#include <string>
#include "colors.h"

using namespace std;

namespace text {
	struct {
		string terminal_no_color = 		"Terminal does not support colors. Program will run in monochrome mode!";
		string failed_to_open_stock = 		"Failed to load stock. Make new stock file? (Y, n)";
		string no_product_to_modify = 		"No products to modify!";
		string basket_not_empty = 		"Products still in basket! Exit anyway? (y, N)";
		string no_commas =			"can't contain commas! Try again!";
		string product_not_found =		"No product with this code found.";
		string receipt_printing = 		"Could not print reciept";
		string no_items_in_basket =		"No items in basket!";
} errors;

	struct {
		string product = 					"Product";
		string saved_stock =				"Saved stock!";
		string loaded_stock =				"Loaded stock!";
		string tabs[3] =					{"PRODUCTS", "BASKET", "STOCK"};

		struct {
			string product_or_upc =			"Name or code? (N, c)";
			string search =					"Search: ";
		} search;
		string total =						"TOTAL";
		string sold_by[3] =				{"BUC", "KG", "M"};

		string shop_code = 				"CUI: ";

		string reprint_receipt = 			"Reprint receipt?(y, N)";
	} info;

	struct {
		string name =				"NAME";
		string code =				"UPC";
		string price =				"PRICE";
		string discount_short = 	"DIS";
		string discount_long =		"DISCOUNT";
		string price_after_dis =	"PAD";
		string tax =				"TAX";
		string quantity_short =		"QTY";
		string quantity_long = 		"QUANTITY";
		string stock =				"STOCK";
		string profit =				"PROFIT";
		string price_to_stock =		"PTS";
		string sold_by = 		"SOLD BY";

	} product_criteria;
	struct {
		string name = 			"cli_pos";
		string version_string = 	"Version 0.4";

		string copyright_statement =	"Copyright 2025-2026 namenotfoundindb. All rights reserved";
		string designed_for = 		"Simple and lightweight point of sale program for the terminal";
	} program;

	struct {
		string please_select = 			"Please select one of the following options\n\n";

		string false_true[2] = 			{"FALSE", "TRUE"};
		struct {
			string products_label = 	"Products";
			string settings_label = 	"Settings";
			string info_label =		"Info";
			string quit_label =		"Quit";

			struct {
				string shop_label =		"Shop";
				struct {
					string name = 	"Name";
					string code = 	"Code";
					string addres = "Addres";
				} shop;

				struct {
					string barcode_scanner_label = 		"Barcode scanner";
					string receipt_printer_label =	 	"Receipt printer";

					struct {
						string printer_device_file = 	"Printer file location";
						string cpl =			"Characters per line";
						string newlines =			"Nr of newlines";
						string newlines_description = "How many lines is the printer going to put at the end of the receipt";
					} receipt_printer;

					struct {
						string prefix = 		"Barcode prefix";
						string scan_a_code = 		"Please scan a code to get the prefix!";
						string current_prefix = 	"You're current prefix is: ";
					} barcode_scanner;
				} peripherals;

				string peripherals_label =	"Peripherals";
				string colorscheme_label = 	"Colorschem";
				string users_label =		"Users";
			} settings;
		} main;
	} menus;
}
