#pragma once

#include <vector>
#include <string>
#include <optional>

using namespace std;

namespace draw_menus {
	optional<int> selection_list(const vector<string>& options, const vector<short int>& colors = {}, bool centered = true, bool can_return_nullopt = false, int selection = 0, short int space_in_betwen = 0);
	optional<int> dropdown(const vector<string>& options, const vector<short int>& colors = {}, bool can_return_nullopt = false, int selection = 0);
	void main_menu();
}

void test_menu();
void start_main_menu();
