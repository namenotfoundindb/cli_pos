#include <string>
#include <vector>

using namespace std;

bool compare_string(string s1, string s2) {
	int max_size = 0;
	if (s1.length() < s2.length()) max_size = s1.length();
	else max_size = s2.length();

	for (int i = 0; i < max_size; i++) {
		if (s1[i] > s2[i]) return true;
		else if (s1[i] < s2[i]) return false;
	} 
	return false;
}

vector<string> bubble_sort(vector<string> v) {
	bool sorted = false;

	int i;
	string temp;

	while (!sorted) {
		for (i = 0; i < v.size() -1; i++) {
			if (compare_string(v[i], v[i + 1])) {
				//If the first string is supperior, swap the around
				temp = v[i];
				v[i] = v[i + 1];
				v[i + 1] = temp;

				sorted = false;
			}
		}
	}

	return v;

}

int binary_search(vector<string> v, string search_word) {
	int start, mid, end;

	start  = 0;
	end = v.size() - 1;

	while (start <= end) {
		mid = start + (end - start) / 2;

		if (v[mid] == search_word) return mid;

		if (compare_string(search_word, v[mid])) start = mid + 1;
		else end = mid - 1;
	}

	return mid;
}
