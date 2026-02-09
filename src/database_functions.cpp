#include <vector>
#include <pqxx/pqxx>
#include <string>

#include "h_files/definitions_and_variables.h"
#include <iostream>

using namespace std;
using namespace pqxx;

mesurments to_mesurments(short int n) {
	switch (n) {
		case PIECE: return PIECE;
		case LENGTH: return LENGTH;
		case WEIGHT: return WEIGHT;
		default: return PIECE;
	}
}

int stock_size(work& tx) {

	result res(tx.exec("SELECT COUNT(\'code\') FROM " + database.products_table + ";"));
	return res[0][0].as<long int>();
}

product get_product_data(long int code, work& tx) {
	result res(tx.exec("SELECT * FROM " + database.products_table + " WHERE code = " + to_string(code) + ";"));

	product return_product;

	return_product.name = 					res[0][0].as<string>();
	return_product.code =					res[0][1].as<long int>();
	return_product.price =					res[0][2].as<double>();
	return_product.discount =				res[0][3].as<short int>();
	return_product.price_after_discount = 	res[0][2].as<double>() - (res[0][2].as<double>() / 100.0) * res[0][3].as<short int>();
	return_product.tax =					res[0][4].as<string>()[0];
	return_product.stock =					res[0][5].as<double>();
	return_product.profit =					res[0][2].as<double>() - res[0][6].as<double>();
	return_product.price_to_stock =			res[0][6].as<double>();
	return_product.sold_by =				to_mesurments(res[0][7].as<short int>());

	return return_product;
}

bool product_exists(long int code, work& tx) {
	result res = tx.exec("SELECT COUNT(code) FROM " + database.products_table + " WHERE code = " + to_string(code));

	return res[0][0].as<bool>();
}

void update_stock(product product, work& tx) {
	string string_tax, exec;
	string_tax = product.tax;

	if (product_exists(product.code, tx)) {
		exec = "UPDATE " + database.products_table + " SET " +
			"name = \'" + product.name + "\', " +
			"code = " + to_string(product.code) + ", " +
			"price = " + to_string(product.price) + ", " +
			"discount = " + to_string(product.discount) + ", " +
			"tax = \'" + string_tax + "\', " +
			"stock = " + to_string(product.stock) + ", " +
			"price_to_stock = " + to_string(product.price_to_stock) + ", " +
			"sold_by = " + to_string(int(product.sold_by)) +
			" WHERE code = " + to_string(product.code) + ";";

		tx.exec(exec);

	}
	else {				//If database didn't find another product with same code, then add product to database

		exec = "INSERT INTO " + database.products_table +
			"(name, code, price, discount, tax, stock, price_to_stock, sold_by) VALUES (\'"
			+ product.name + "\', "
			+ to_string(product.code) + ", "
			+ to_string(product.price) + ", "
			+ to_string(product.discount) + ", \'" + string_tax + "\', "
			+ to_string(product.stock) + ", "
			+ to_string(product.price_to_stock) + ", "
			+ to_string(int(product.sold_by)) + ");";

		tx.exec(exec);


	}
}

vector<long int> code_list(work& tx) {
	result res(tx.exec("SELECT code FROM " + database.products_table + " ORDER BY name ASC"));

	int i;
	vector<long int> return_list;
	for (i = 0; i < stock_size(tx); i++) {
		return_list.push_back(res[i][0].as<long int>());
	}

	return return_list;
}

vector<string> name_list(work& tx) {
	result res(tx.exec("SELECT name FROM " + database.products_table + " ORDER BY name ASC"));

	int i;
	vector<string> return_list;
	for (i = 0; i < stock_size(tx); i++) {
		return_list.push_back(res[i][0].as<string>());
	}

	return return_list;
}

void erase(long int code, work& tx) {
	tx.exec("DELETE FROM " + database.products_table + " WHERE code = " + to_string(code) + ";");
}

map<char, short int> get_tax_rates(work& tx) {
	map<char, short int> map;
	result res(tx.exec("SELECT * FROM " + database.tax_rates_table + " ORDER BY tax_char ASC"));

	for (int i = 0; i < (sizeof(res) / 8) - 1; i++) {
		map.insert({res[i][0].as<string>()[0], res[i][1].as<short int>()});
	}
	return map;
}
