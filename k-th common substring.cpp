#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

/* Символы-разделители, не встречающиеся в строках.
 * Первый отделяет строку s от строки t, а второй добавляется в конец 
 * и нужен для построения суффиксного массива. Первый же нужен вот зачем:
 * s_i...s_{n-1}$t_0...t_{m-1}#
 * t_j...t_{m-1}#
 * Если его не добавлять, то при возникновении описанной выше ситуации lcp может "залезть"
 * на строку t, и будут посчитаны какие-то лишние подстроки, на самом деле не явлляющиеся общими.
 * Если же он есть, длина lcp гарантированно меньше длины суффикса s + 1, и всё будет учитываться верно. */
const char special_symbol_1 = '$';
const char special_symbol_2 = '#';
const int alphabet_size = 97;

/* Модифицирует массив, получившийся после подсчёта, так, чтобы count[i] 
 * хранил позицию, в которую нужно начинать ставить элементы с ключом i.
 * Изначально count[i] - количество элементов с ключом i. */
void count_positions(vector <int>& count) {
	if (count.empty())
		return;
	int last = count[0];
	count[0] = 0;
	for (int i = 1; i < static_cast <int>(count.size()); i++) {
		int temp = count[i];
		count[i] = last;
		last += temp;
	}
}

// Построение суффиксного массива suf (изначально заполнен нулями) по строке s.
void build_suffix_array(const string& s, vector <int>& suf) {
	const int size = static_cast <int>(s.length());
	// вспомогательный массив для сортировки подсчётом
	vector <int> count(alphabet_size, 0);
	// классы эквивалентности суффиксов по первым символам
	vector <int> classes(size, 0);
	for (int i = 0; i < size; i++)
		count[s[i] - special_symbol_2]++;
	count_positions(count);
	for (int i = 0; i < size; i++)
		suf[count[s[i] - special_symbol_2]++] = i;
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
		count.assign(size, 0);
		for (int i = 0; i < size; i++)
			count[classes[sufs_by_2nd_half[i]]]++;
		count_positions(count);
		for (int i = 0; i < size; i++)
			suf[count[classes[sufs_by_2nd_half[i]]]++] = sufs_by_2nd_half[i];
			
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
void build_lcp (const string& s, const vector <int>& suf, vector <int>& lcp) {
	const int size = static_cast <int>(s.length());
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
	return (suffix_position < s_length ? 0 : 1);
}

// Функция для решения задачи.
string common_substring_search(const string& s, const string& t, long long k) {
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
				return concat.substr(suf[i], last_lcp + addition);
			}
			last_lcp = lcp[i + 1];
		}
		else {
			last_lcp = min(last_lcp, lcp[i + 1]);
		}
	}
	return "-1";
}


void solve() {
	string s, t;
	long long k;
	cin >> s >> t >> k;
	cout << common_substring_search(s, t, k);
}

                                                                                                                          
int main() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                         
	cout.tie(0);
	solve();
	return 0;
}