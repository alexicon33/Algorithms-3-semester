#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

using namespace std;

// Специальный символ, не встречающийся в данной строке и добавляемый в её конец.
const char special_symbol = 31;
const int alphabet_size = 97;

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
		cnt[s[i] - special_symbol]++;
	count_positions(cnt);
	for (int i = 0; i < size; i++)
		suf[cnt[s[i] - special_symbol]++] = i;
	char current_char = special_symbol;
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

// Функция для решения задачи.
void solve() {
	string s;
	getline(cin, s);
	s += special_symbol;
	long long size = static_cast <long long>(s.length());
	vector <int> suf(s.length(), 0), lcp(s.length(), 0);
	build_suffix_array(s, suf);
	build_lcp(s, suf, lcp);
	
	long long summary_lcp = 0;
	for (int i = 1; i < size; i++) {
		summary_lcp += lcp[i];
	}
	
	cout << (size * (size - 1)) / 2 - summary_lcp;
}

                                                                                                                          
int main() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                         
	cout.tie(0);
	solve();
	return 0;
}