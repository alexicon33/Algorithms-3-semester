#include <algorithm>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

using namespace std;

const int alphabet_size = 26;
const char start_symbol = 'a';


// Бор. Реализованы операции добавления строки, вычисления суффиксных ссылок (в т.ч. сжатых),
// переходы в новые состояния.
class Trie {
	// вершина бора
	struct Node {
		// хранит индексы строк, заканчивающихся в этой вершине 
		// (индексы из вектора patterns, см. далее для понимания)
		vector <int> ending_strings;
		// рёбра дерева
		vector <unique_ptr <Node> > transitions;
		// переходы в новые состояния
		vector <Node*> links;
		// суффиксная ссылка, сжатая суффиксная ссылка, родитель
		Node *suffix_link, *compressed_link, *parent;
		// по какому символу идёт ребро из родителя к вершине
		char char_to_parent;
		
		Node(): char_to_parent('0') {
			for (int i = 0; i < alphabet_size; i++) {
				links.push_back(nullptr);
				transitions.push_back(nullptr);
			}
		}
		
		// На вход передаются родитель и символ, по которому пришли в новую вершину.
		Node(Node* parent_, char char_to_parent_): parent(parent_), char_to_parent(char_to_parent_) {
			for (int i = 0; i < alphabet_size; i++) {
				links.push_back(nullptr);
				transitions.push_back(nullptr);
			}
		}
	};
	
	// корень
	unique_ptr <Node> root;
	// текущее состояние
	Node* current;
	
	// Вычисление суффиксной ссылки для вершины node.
	void make_suffix_link(Node* node) {
		if (node == root.get() || node->parent == root.get()) {
			node->suffix_link = root.get();
		}
		else {
			node->suffix_link = node->parent->suffix_link->links[node->char_to_parent - start_symbol];
		}
	}
	
	// Вычисление переходов для вершины node.
	void make_links(Node* node) {
		for (int c = 0; c < alphabet_size; c++) {
			if (node->transitions[c] == nullptr) {
				if (node == root.get()) {
					node->links[c] = root.get();
				}
				else {
					node->links[c] = node->suffix_link->links[c];
				}
			}
			else {
				node->links[c] = node->transitions[c].get();
			}
		}
	}
	
	// Вычисление суффиксной ссылки для вершины node.
	void make_compressed_link(Node* node) {
		if (!node->suffix_link->ending_strings.empty()) {
			node->compressed_link = node->suffix_link;
		}
		else if (node->suffix_link == root.get()) {
			node->compressed_link = nullptr;
		}
		else {
			node->compressed_link = node->suffix_link->compressed_link;
		}
	}
	
public:
	Trie(): root(make_unique <Node>()), current(root.get()) {
		root->parent = root.get();
	}
	
	// Построение автомата.
	void build(const vector <pair <string, int> >& patterns) {
		int size = static_cast <int>(patterns.size());
		for (int i = 0; i < size; i++) {
			add_string(patterns[i].first, i);
		}	
		make_links();
		make_compressed_links();
	}
	
	/* Поиск всех вхождений шаблона в строку text. pattern_length - общая длина шаблона.
	   Описание вектора patterns приведено в функции solve. 
	   Возвращает вектор, в котором записаны индексы позиций, где шаблон начинается. */
	vector <int> search(const vector <pair <string, int> >& patterns, const string& text, int pattern_length) {
		vector <int> start_points;
		int text_size = static_cast <int>(text.length());
		// occurrences[i] - кол-во отметок о вхождениях на i-й позиции.
		vector <int> occurrences(text_size, 0);
		for (int i = 0; i < text_size; i++) {
			change_state(text[i]);
			check_occurrences(patterns, occurrences, i);
		}
		for (int i = 0; i < text_size; i++) {
			if (occurrences[i] == static_cast <int>(patterns.size()) && i + pattern_length - 1 < text_size) {
				start_points.push_back(i);
			}
		}
		// Проверка особого случая, когда в шаблоне только вопросики.
		if (patterns.empty()) {
			for (int i = 0; i + pattern_length - 1 < text_size; i++) {
				start_points.push_back(i);
			}
		}
		return start_points;
	}
	
	// Добавление в бор строки pattern, которая суть patterns[number].first.
	void add_string(const string& pattern, int number) {
		Node* state = root.get();
		int size = static_cast <int> (pattern.length());
		for (int i = 0; i < size; i++) {
			if (state->transitions[pattern[i] - start_symbol] == nullptr) {
				state->transitions[pattern[i] - start_symbol] = make_unique <Node>(state, pattern[i]);
			}
			state = state->transitions[pattern[i] - start_symbol].get();
			if (i == size - 1) {
				state->ending_strings.push_back(number); 
			}
		}
	}
	
	// Вычисление переходов для бора в целом (обходом в ширину).
	void make_links() {
		queue <Node*> q;
		q.push(root.get());
		while (!q.empty()) {
			auto vertex = q.front();
			for (int i = 0; i < alphabet_size; i++) {
				if (vertex->transitions[i] != nullptr) {
					q.push(vertex->transitions[i].get());
				}
			}
			make_suffix_link(vertex);
			make_links(vertex);
			q.pop();
		}
	}
	
	// Вычисление сжатых суффиксных ссылок для бора в целом (обходом в ширину).
	void make_compressed_links() {
		queue <Node*> q;
		q.push(root.get());
		while (!q.empty()) {
			auto vertex = q.front();
			for (int i = 0; i < alphabet_size; i++) {
				if (vertex->transitions[i] != nullptr) {
					q.push(vertex->transitions[i].get());
				}
			}
			make_compressed_link(vertex);
			q.pop();
		}
	}
	
	// Изменение состояния бора, если нужно обработать символ c.
	void change_state(char c) {
		current = current->links[c - start_symbol];
	}
	
	/* Проверяет и отмечает все вхождения подстрок шаблона, 
	   хранящихся в векторе patterns, заканчивающихся в позиции position. */
	void check_occurrences(const vector <pair <string, int> >& patterns, vector <int>& occurrences, int position) {
		Node* link = current;
		while (link != nullptr && link != root.get()) {
			for (auto x: link->ending_strings) {
				if (position - patterns[x].second >= 0) {
					occurrences[position - patterns[x].second]++;
				}
			}
			link = link->compressed_link;
		}
	}
	
	Trie(const Trie&) = delete;
	Trie& operator=(const Trie&) = delete;
};


// Поиск всех вхождений подстрок шаблона из patterns в строке text.
// Реализует алгоритм Ахо-Корасик.
vector <int> patterns_search(const vector <pair <string, int> >& patterns, const string& text, int pattern_length) {
	// строим бор на имеющихся подстроках
	Trie trie;
	vector <int> start_points;
	trie.build(patterns);
	return trie.search(patterns, text, pattern_length);
}


// Выделение из шаблона pattern подстрок patterns, вхождения которых потом будут проверяться.
// Описание вектора patterns приведено в функции solve.
void pattern_split(const string& pattern, vector <pair <string, int> >& patterns) {
	int size = static_cast <int>(pattern.length());
	int marks = 0, lengths = 0;
	for (int i = 0; i < size; i++) {
		if (pattern[i] != '?') {
			string current_substring = "";
			for (; i < size && pattern[i] != '?'; i++) {
				current_substring.push_back(pattern[i]);
			}
			lengths += current_substring.length();
			patterns.push_back({current_substring, marks + lengths - 1});
			if (i < size) {
				marks++;
			}
		}
		else {
			marks++;
		}
	}
}


// Функция для решения.
void solve() {
	string pattern, text;
	/* Первый элемент - подстрока шаблона (между вопросиками), второй - на сколько символов назад нужно отметить вхождение.
	   Т.е., если строка patterns[i].first вошла в текст и закончилась на j-м его символе, 
	   вхождение будет отмечено для j - patterns[i].second символа. Тогда вхождению всего шаблона
	   будут соответствовать patterns.size() таких отметок в позиции, где этот шаблон начинается. */
	vector <pair <string, int> > patterns;
	cin >> pattern >> text;
	// размер шаблона, сколько вопросиков было до текущего момента, сколько иных символов было до текущего момента.
	pattern_split(pattern, patterns);
	vector <int> pattern_entries = patterns_search(patterns, text, static_cast <int>(pattern.length()));
	for (auto i: pattern_entries)
		cout << i << " ";
}

                                                                                                                          
int main() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                         
	cout.tie(0);
	solve();
	return 0;
}
