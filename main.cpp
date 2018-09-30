/* Найти лексикографически минимальную строку, 
 * построенную по префикс-функции, в алфавите a-z.*/
 
#include <iostream>
#include <vector>
#include <cstring>
#include <queue>
using namespace std;
const int alphabet_size = 26;


// Построение лексикографически минимальной строки по префикс-функции.
string prefix_to_string(const vector <int>& prefix) {
	string result = "a";
	for (int i = 1; i < static_cast<int>(prefix.size()); i++) {
		if (prefix[i])
			result += result[prefix[i] - 1];
		else {
			// хочется, чтобы ни один из потенциально возможных суффиксов нельзя было продолжить.
			// переберём их и пометим символы после них как "forbidden" - запрещённые. 
			// В конце выберем первый не запрещённый символ.
			int forbidden[alphabet_size];
			memset(forbidden, 0, sizeof(forbidden));
			int current = prefix[i - 1];
			while (current > 0) {
				forbidden[result[current] - 'a'] = 1;
				current = prefix[current - 1];
			}
			forbidden[result[current] - 'a'] = 1;
			for (int i = 0; i < alphabet_size; i++) {
				if (!forbidden[i]) {
					result += 'a' + i;
					break;
				}
			}
		}
	}
	return result;
}


// Преобразование z-функции некоторой строки в её префикс-функцию.
void z_to_prefix(vector <int>& prefix, const vector <int>& z) {
	int size = static_cast<int>(z.size());
	prefix.clear();
	prefix.resize(size, 0);
	prefix[0] = 0;
	/* z-функция задаёт некоторые "отрезки" - подстроки, совпадающие с префиксами.
	   Какие-то из них содержат i-й символ. 
	   Префикс-функция в i-й позиции равна наибольшей из длин префиксов таких подстрок, 
	   оканчивающихся на этом символе. Если идти по строке с начала до конца и 
	   записывать отрезки, то отрезок с наиболее длинным подходящим префиксом окажется записан
	   раньше всех, то есть будет первым в очереди. Уже оставшиеся позади отрезки нужно удалять.*/
	queue <pair <int, int> > segments;
	for (int i = 1; i < size; i++) {
		while (!segments.empty() && segments.front().first + segments.front().second <= i)
			segments.pop();
		if (z[i])
			segments.push({i, z[i]});
		if (!segments.empty())
			prefix[i] = i - segments.front().first + 1;
		else
			prefix[i] = 0;
	}
}


// Вычисляет z-функцию z строки text.
void z_function(const string& text, vector <int>& z) {
	int size = static_cast<int>(text.length());
	z.resize(size, 0);
	z[0] = size;
	// Границы самого правой найденной подстроки, совпадающей с некоторым префиксом.
	int left_border = 0, right_border = 0;
	for (int i = 1; i < size; i++) {
		// Используем уже посчитанные ранее значения, если это возможно.
		if (right_border >= i)
			z[i] = min(z[i - left_border], right_border - i + 1);
		while (i + z[i] < size && text[i + z[i]] == text[z[i]])
			z[i]++;
		// обновление границ
		if (i + z[i] - 1 > right_border) {
			left_border = i;
			right_border = i + z[i] - 1;
		}
	}
}


// Построение лексикографически минимальной строки по z-функции z.
string z_to_string(const vector <int>& z) {
	vector <int> prefix;
	z_to_prefix(prefix, z);
	return prefix_to_string(prefix);
}


// Преобразование префикс-функции некоторой строки в её z-функцию.
void prefix_to_z(const vector <int>& prefix, vector <int>& z) {
	string temporary = prefix_to_string(prefix);
	z_function(temporary, z);
}


// Вычисляет префикс-функцию prefix строки text.
void prefix_function(const string& text, vector <int>& prefix) {
	vector <int> z;
	z_function(text, z);
	z_to_prefix(prefix, z);
}


// Функция для решения.
void solve() {
	vector <int> prefix;
	int number;
	while (cin >> number) 
		prefix.push_back(number);
	cout << prefix_to_string(prefix);
}

                                                                                                                          
int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);        
	solve();
	return 0;
}