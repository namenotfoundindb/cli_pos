#pragma once

#include <string>
#include <pqxx/pqxx>
#include <vector>
#include <map>

#include "definitions_and_variables.h"

using namespace std;
using namespace pqxx;

int stock_size(work& tx);
product get_product_data(long int code, work& tx);
bool product_exists(long int code, work& tx);
void update_stock(product product, work& tx);
vector<long int> code_list(work& tx);
vector<string> name_list(work& tx);
void erase(long int code, work& tx);
map<char, short int> get_tax_rates(work& tx);
