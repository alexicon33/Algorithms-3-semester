#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int base = 10;

class BigInteger {
private:
	string number;
	bool sign;

public:
	// конструкторы
	BigInteger() = default;
	explicit BigInteger (const string& value);
	BigInteger (const int value);

	// строковое представление числа
	string toString() const;

	// логика и порядок
	bool operator ==(const BigInteger& other) const;
	bool operator !=(const BigInteger& other) const;
	bool operator <(const BigInteger& other) const;
	bool operator <=(const BigInteger& other) const;
	bool operator >(const BigInteger& other) const;
	bool operator >=(const BigInteger& other) const;

	//	унарный оператор
	BigInteger operator -() const;
	
	//	prefix increment/decrement
	BigInteger& operator ++();
	BigInteger& operator --();

	// postfix increment/decrement
	BigInteger operator ++(int);
	BigInteger operator --(int);

	//	бинарные операторы
	friend BigInteger operator +(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator *(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator -(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator /(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator %(const BigInteger& left, const BigInteger& right);

	//	составные операторы
	BigInteger& operator +=(const BigInteger& other);
	BigInteger& operator -=(const BigInteger& other);
	BigInteger& operator *=(const BigInteger& other);
	BigInteger& operator /=(const BigInteger& other);
	BigInteger& operator %=(const BigInteger& other);

	//	преобразование типов
	explicit operator bool() const;

	//	ввод/вывод
	friend ostream& operator <<(ostream &stream, const BigInteger& value);
	friend istream& operator >>(istream &stream, BigInteger& value);
};


void delete_leading_zeros(string& s) {
	while (s.length() > 1 && s.back() == '0')
		s.pop_back();
}

void reverse(string& s) {
	int size = s.length();
	for (int i = 0; i < (size >> 1); i++)
		swap(s[i], s[size - i - 1]);
}

// Строки сравниваются как числа, записанные в развёрнутом виде.
bool operator <(const string& first, const string& second) {
	if (first.length() < second.length())
		return true;
	if (first.length() > second.length())
		return false;
	int size = first.length();
	for (int i = 0; i < size; i++) {
		if (first[size - i - 1] < second[size - i - 1])
			return true;
		if (first[size - i - 1] > second[size - i - 1])
			return false;
	}
	return false;
}

// Получение цифры, стоящей в строке s в позиции position (именно как числа).
int get(const string& s, int position) {
	return (position < static_cast <int>(s.length()) ? s[position] - '0' : 0);
}

// Сложение в столбик двух чисел, записанных в строках up и down.
string add(const string& up, const string& down) {
	string result;
	int max_size = max(up.length(), down.length());
	int addition = 0;
	for (int i = 0; i < max_size; i++) {
		result.push_back((get(up, i) + get(down, i) + addition) % base + '0');
		addition = (get(up, i) + get(down, i) + addition) / base;
	}
	if (addition)
		result.push_back('0' + addition);
	return result;
}

// Вычитание в столбик двух чисел, записанных в строках up и down.
string substraction(const string& up, const string& down) {
	string result;
	int size = up.length();
	int addition = 0;
	for (int i = 0; i < size; i++) {
		result.push_back((addition + get(up, i) - get(down, i) + base) % base + '0');
		addition = (addition + get(up, i) - get(down, i) < 0 ? -1 : 0);
	}
	delete_leading_zeros(result);
	return result;
}

// Умножение длинного числа, записанного в строке up, на короткое (меньше 10) multiplier.
string multiplication(const string& up, int multiplier, int position) {
	string result;
	result.assign(position, '0');
	int size = up.length();
	int addition = 0;
	for (int i = 0; i < size || addition > 0; i++) {
		result.push_back((get(up, i) * multiplier + addition) % base + '0');
		addition = (get(up, i) * multiplier + addition) / base;
	}
	return result;
}

string BigInteger::toString() const {
	string result;
	if (!sign)
		result.push_back('-');
	int size = number.length();
	for (int i = 0; i < size; i++)
		result.push_back(number[size - i - 1]);
	return result;
}

BigInteger::BigInteger(const string& value) {
	if (value == "-0") {
		sign = true;
		number = "0";
	}
	else {
		int start_position = 0;
		int size = value.size();
		if (value[0] == '-') {
			sign = false;
			start_position++;
		}
		else
			sign = true;
		for (int i = size - 1; i >= start_position; i--)
			number.push_back(value[i]);
	}
}

BigInteger::BigInteger(const int value) {
	sign = value >= 0;
	number = to_string(abs(value));
	reverse(number);
}

ostream& operator <<(ostream& stream, const BigInteger& value) {
	stream << (value.sign ? "" : "-");
	int size = value.number.length();
	for (int i = 0; i < size; i++)
		stream << value.number[size - i - 1];
	return stream;
}

istream& operator >>(istream& stream, BigInteger& value) {
	string string_value;
	stream >> string_value;
	value = BigInteger(string_value);
	return stream;
}

BigInteger operator +(const BigInteger& left, const BigInteger& right) {
	BigInteger result;
	if (left.sign == right.sign) {
		result.sign = left.sign;
		result.number = add(left.number, right.number);
	}
	else {
		if (left.number < right.number) {
			result.sign = right.sign;
			result.number = substraction(right.number, left.number);
		}
		else {
			result.number = substraction(left.number, right.number);
			result.sign = (result.number == "0" ? true : left.sign);
		}
	}
	return result;
}

BigInteger& BigInteger::operator +=(const BigInteger& other) {
	return *this = *this + other;
}

BigInteger operator *(const BigInteger& left, const BigInteger& right) {
	if (left.number == "0" || right.number == "0")
		return BigInteger(0);
	BigInteger result;
	result.sign = left.sign == right.sign;
	int size = right.number.length();
	for (int i = 0; i < size; i++)
		result.number = add(result.number, multiplication(left.number, right.number[i] - '0', i));
	return result;
}

BigInteger& BigInteger::operator *=(const BigInteger& other) {
	return *this = *this * other;
}

BigInteger operator -(const BigInteger& left, const BigInteger& right) {
	return left + right * BigInteger(-1);
}

BigInteger& BigInteger::operator -=(const BigInteger& other) {
	return *this = *this - other;
}

int get_next_number(const string& up, const string& down) {
	for (int i = 1; i <= base; i++) {
		if (up < multiplication(down, i, 0))
			return i - 1;
	}
	return -1;
}

BigInteger operator /(const BigInteger& left, const BigInteger& right) {
	if (left.number < right.number)
		return BigInteger(0);
	BigInteger result;
	result.sign = left.sign == right.sign;
	string up = left.number, down = right.number;
	int up_size = up.length();
	int position = 0;
	string current = "";
	while (position < up_size) {
		if (current == "0")
			current = up[up_size - position - 1];
		else
			current = up[up_size - position - 1] + current;
		position++;
		int next_number = get_next_number(current, down);
		result.number.push_back(next_number + '0');
		current = substraction(current, multiplication(down, next_number, 0));
	}
	reverse(result.number);
	delete_leading_zeros(result.number);
	return result;
}

BigInteger& BigInteger::operator /=(const BigInteger& other) {
	return *this = *this / other;
}

BigInteger operator %(const BigInteger& left, const BigInteger& right) {
	return left - right * (left / right);
}

BigInteger& BigInteger::operator %=(const BigInteger& other) {
	return *this = *this % other;
}

bool BigInteger::operator ==(const BigInteger& other) const {
	return other.sign == sign && other.number == number;
}

bool BigInteger::operator <(const BigInteger& other) const {
	if (!sign && other.sign)
		return true;
	if (sign && other.sign)
		return number < other.number;
	if (!sign && !other.sign)
		return other.number < number;
	return false;
}

bool BigInteger::operator >(const BigInteger& other) const {
	return !(*this == other) && !(*this < other);
}

bool BigInteger::operator !=(const BigInteger& other) const {
	return !(*this == other);
}

bool BigInteger::operator <=(const BigInteger& other) const {
	return *this == other || *this < other;
}

bool BigInteger::operator >=(const BigInteger& other) const {
	return *this > other || *this == other;
}

BigInteger BigInteger::operator -() const {
	return *this * BigInteger(-1);
}

BigInteger& BigInteger::operator ++() {
	return *this += 1;
}

BigInteger& BigInteger::operator --() {
	return *this -= 1;
}

BigInteger BigInteger::operator ++(int) {
	BigInteger temp(*this);
	++(*this);
	return temp;
}

BigInteger BigInteger::operator --(int) {
	BigInteger temp(*this);
	--(*this);
	return temp;
}

BigInteger::operator bool() const {
	return !(number == "0");
}