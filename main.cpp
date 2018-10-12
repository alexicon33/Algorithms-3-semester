/* ������� ��� ��������� ������� � ������. ����� ������� - p, ����� ������ - n. 
 * ����� O(n + p), ���. ������ - O(p).
 * p <= 30000, n <= 300000.
 * ������ �����:
   ������, ������ �������� ������, ������.
 * ������ ������:
   ������� ��������� ������� � ������.*/
 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;


// ������� �������-������� prefix ������ sample.
void prefix_function(const string& sample, vector <int>& prefix) {
        int sample_length = static_cast<int>(sample.length());
        prefix.resize(sample_length, 0);
        for (int i = 1; i < sample_length; i++) {
                int current_position = i - 1;
                // ������� � ������� �������� ����� ���� ���������, ������� ������������ ����� ����������.
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


// ��������� �������� �����-�������-������. ��������� �� ���� ������ ����� � ������, 
// ������ � ����� ����������� � �������������� � �������� ������.
void find_pattern_in_stream(ifstream& fin, ofstream& fout) {
		string pattern;
		fin >> pattern;
        vector <int> prefix; // ������ �������-������� �������
        prefix_function(pattern, prefix);
        char last = '#';  // ��������� �������� ������
        // �������-������� �������������� � ���������� ��������� � �� �������.
        int previous_prefix = 0, last_prefix = 0, counter = 0;
        while (fin >> last) {
        	// ������������ �������-������� ��� �������� ���������������� ��������.
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


// ������� ��� �������.
void solve() {
		ifstream fin("input.txt");
		ofstream fout("output.txt");
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
