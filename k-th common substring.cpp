#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

using namespace std;

// Символы-разделители, не встречающиеся в строках.
const char special_symbol_1 = '$';
const char special_symbol_2 = '#';
const long long alphabet_size = 97;

/* Модифицирует массив, получившийся после подсчёта, так, чтобы cnt[i] 
 * хранил позицию, в которую нужно начинать ставить элементы с ключом i.
 * Изначально cnt[i] - количество элементов с ключом i. */
void count_positions(vector <int>& cnt) {
	int last = cnt[0];
	cnt[0] = 0;
	for (int i = 1; i < static_cast <int>(cnt.size()); i++) {
		int temp = cnt[i];
		cnt[i] = last;
		last += temp;
	}
}

// Построение суффиксного массива suf (изначально заполнен нулями) по строке s.
void build_suffix_array(string& s, vector <int>& suf) {
	int size = static_cast <int>(s.length());
	// вспомогательный массив для сортировки подсчётом
	vector <int> cnt(alphabet_size, 0);
	// классы эквивалентности суффиксов по первым символам
	vector <int> classes(size, 0);
	for (int i = 0; i < size; i++)
		cnt[s[i] - special_symbol_2]++;
	count_positions(cnt);
	for (int i = 0; i < size; i++)
		suf[cnt[s[i] - special_symbol_2]++] = i;
	char current_char = special_symbol_2;
	int current_class = 0;
	// определяем классы эквивалентности для первого этапа сортировки
	for (int i = 0; i < size; i++) {
		if (s[suf[i]] != current_char) {
			current_char = s[suf[i]];
			current_class++;
		}
		classes[suf[i]] = current_class;
	}
	
	int current_length = 1;
	// сортируем по первым current_length символам
	while (current_length < size) {
		// сортировка по 2 половине
		vector <int> sufs_by_2nd_half(size, 0);
		for (int i = 0; i < size; i++) {
			sufs_by_2nd_half[i] = (suf[i] - current_length + size) % size;
		}
	
		// сортировка по 1 половине
		cnt.clear();
		cnt.resize(size, 0);
		for (int i = 0; i < size; i++)
			cnt[classes[sufs_by_2nd_half[i]]]++;
		count_positions(cnt);
		for (int i = 0; i < size; i++)
			suf[cnt[classes[sufs_by_2nd_half[i]]]++] = sufs_by_2nd_half[i];
			
		// определение новых классов эквивалентности
		vector <int> new_classes(size, 0);
		current_class = 0;
		for (int i = 0; i < size; i++) {
			new_classes[suf[i]] = current_class;
			if (i < size - 1 && (classes[suf[i]] != classes[suf[i + 1]] || 
				classes[(suf[i] + current_length) % size] != classes[(suf[i + 1] + current_length) % size]))
				current_class++;
		}
		classes = new_classes;
		current_length *= 2;
	}
}

// Построение массива lcp алгоритмом Касаи и др., за линейное время.
// В данной реализации lcp[i] - длина наибольшего общего префикса suf[i] и suf[i - 1]-го суффиксов.
void build_lcp (string& s, vector <int>& suf, vector <int>& lcp) {
	int size = static_cast <int>(s.length());
	vector <int> positions(size, 0);
	for (int i = 0; i < size; i++)
		positions[suf[i]] = i;
	int current_lcp = 0;
	for (int i = 0; i < size; i++) {
		if (current_lcp > 0)
			current_lcp--;
		if (positions[i] > 0) {
			int k = suf[positions[i] - 1];
			while (i + current_lcp < size && k + current_lcp < size && 
				   s[i + current_lcp] == s[k + current_lcp])
				current_lcp++;
			lcp[positions[i]] = current_lcp;
		}
		else {
			current_lcp = 0;
			lcp[0] = -1;
		}
	}
}

/* Определяет тип суффикса suf[suffix_position] в суффиксном массиве, 
 * т.е. то, к которой из строк данный суффикс относится. */
int type (int suffix_position, long long s_length) {
	return (suffix_position <= s_length ? 0 : 1);
}

// Функция для решения задачи.
void solve() {
	string s, t;
	long long k;
	cin >> s >> t >> k;
	int s_length = static_cast <int>(s.length());
	string concat = s + special_symbol_1 + t + special_symbol_2;
	int size = static_cast <int>(concat.length());
	vector <int> suf(concat.length(), 0), lcp(concat.length(), 0);
	build_suffix_array(concat, suf);
	build_lcp(concat, suf, lcp);
	
	/* Если суффиксы относятся к разным строкам, их общими префиксами будут общие подстроки s и t.
	 * Чтобы не учитывать их многократно, нужно всякий раз вычитать кол-во уже учтённых ранее подстрок, равное last_lcp. */
	long long current_sum = 0;
	int last_lcp = 0;
	for (int i = 0; i < size - 1; i++) {
		if (type(suf[i], s_length) != type(suf[i + 1], s_length)) {
			last_lcp = min(last_lcp, lcp[i + 1]);
			current_sum += lcp[i + 1] - last_lcp;
			if (current_sum >= k) {
				long long addition = k - (current_sum - (lcp[i + 1] - last_lcp));
				cout << concat.substr(suf[i], last_lcp + addition);
				return;
			}
			last_lcp = lcp[i + 1];
		}
		else {
			last_lcp = min(last_lcp, lcp[i + 1]);
		}
	}
	
	cout << -1;
}

                                                                                                                          
int main() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                         
	cout.tie(0);
	solve();
	return 0;
}