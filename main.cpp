/* Найдите все вхождения шаблона в строку. Длина шаблона - p, длина строки - n. 
 * Время O(n + p), доп. память - O(p).
 * p <= 30000, n <= 300000.
 * Формат ввода:
   Шаблон, символ перевода строки, строка.
 * Формат вывода:
   Позиции вхождения шаблона в строке.*/
 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


// Считает префикс-функцию prefix строки sample.
void prefix_function(const string& sample, vector <int>& prefix) {
        int sample_length = static_cast<int>(sample.length());
        prefix.resize(sample_length, 0);
        for (int i = 1; i < sample_length; i++) {
				int current_position = i - 1;
				// Перебор в порядке убывания длины всех суффиксов, которые потенциально можно продолжить.
				while (current_position >= 0 && sample[i] != sample[prefix[current_position]]) {
					current_position = prefix[current_position] - 1;
				}
				if (current_position >= 0 && sample[i] == sample[prefix[current_position]]) {
					prefix[i] = prefix[current_position] + 1;
				}
				else {
					prefix[i] = 0;
				}
		}
}


// Реализует алгоритм Кнута-Морриса-Пратта. Принимает на вход потоки ввода и вывода, 
// шаблон и текст считываются и обрабатываются в процессе работы.
void find_pattern_in_stream(ifstream& fin, ofstream& fout) {
		string pattern;
		fin >> pattern;
        vector <int> prefix; // хранит префикс-функцию шаблона
        prefix_function(pattern, prefix);
        char last = '#';  // последний введённый символ
        // Префикс-функция предпоследнего и последнего элементов и их счётчик.
        int previous_prefix = 0, last_prefix = 0, counter = 0;
        while (fin >> last) {
        	// Подсчитываем префикс-функцию для текущего рассматриваемого элемента.
                if (last == pattern[previous_prefix])
                        last_prefix = previous_prefix + 1;
                else {
                        int current = previous_prefix - 1;
                        while (current >= 0 && last != pattern[prefix[current]])
                                current = prefix[current] - 1;
                        if (current >= 0 && last == pattern[prefix[current]]) {
							last_prefix = prefix[current] + 1; 
						}
						else {
							last_prefix = 0;
						}
                }
                if (last_prefix == static_cast<int>(pattern.length()))
                                fout << counter - static_cast<int>(pattern.length()) + 1 << " ";
                counter++;
                previous_prefix = last_prefix;
        }
}


// Функция для решения.
void solve() {
		ifstream fin("C:/Users/810046/Documents/codelite_workspace/what/input.txt");
		ofstream fout("C:/Users/810046/Documents/codelite_workspace/what/output.txt");
		find_pattern_in_stream(fin, fout);
		fin.close();
		fout.close();
}

                                                                                                                          
int main() {
		ios_base::sync_with_stdio(0);
		cin.tie(0);                                                                                         
		cout.tie(0); 
		solve();
		return 0;
}
