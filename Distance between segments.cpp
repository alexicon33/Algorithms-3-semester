#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <iostream>
                                
using namespace std;

const long double pi = acos(-1);
const long double precision = 1e-10;
// Количество итераций тернарного поиска.
const int operations_number = 1000;

struct Vector;

struct Point {
	long double x, y, z;
	
	Point(long double x_, long double y_, long double z_): x(x_), y(y_), z(z_) {}
	
	Point(): x(0.0), y(0.0), z(0.0) {}
	
	Point operator -(const Vector other);
	
	Point& operator =(const Point& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
};

istream& operator >>(istream& in, Point& A) {
	in >> A.x >> A.y >> A.z;
	return in;
}


struct Vector {
	long double x, y, z;
	
	Vector(long double x_, long double y_, long double z_): x(x_), y(y_), z(z_) {}
	
	Vector(const Point A, const Point B): x(B.x - A.x), y(B.y - A.y), z(B.z - A.z) {}
	
	long double length() {
		return pow(x * x + y * y + z * z, 0.5);
	}
	
	Vector operator +(const Vector other) {
		return Vector(x + other.x, y + other.y, z + other.z);
	}
	
	Vector operator -(const Vector other) {
		return Vector(x - other.x, y - other.y, z - other.z);
	}
	
	Vector operator *(const long double multiplier) {
		return Vector(x * multiplier, y * multiplier, z * multiplier);
	}
	
	Vector operator /(const long double multiplier) {
		assert(abs(multiplier) > precision);
		return Vector(x / multiplier, y / multiplier, z / multiplier);
	}
};

Point operator +(const Point point, const Vector other) {
	return Point(point.x + other.x, point.y + other.y, point.z + other.z);
}
	
Point Point::operator -(const Vector other) {
	return Point(x - other.x, y - other.y, z - other.z);
}


struct Segment {
	Point first, second;
	
	Segment(Point first_, Point second_): first(first_), second(second_) {}
	
	Segment(): first(), second() {}
};

istream& operator >>(istream& in, Segment& seg) {
	in >> seg.first >> seg.second;
	return in;
}                                      


long double scalar(const Vector a, const Vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

long double points_distance(const Point A, const Point B) {
	return pow((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y) + (A.z - B.z) * (A.z - B.z), 0.5);
}

long double point_to_segment_distance(const Point& point, const Segment& segment) {
	long double result = min(points_distance(point, segment.first), points_distance(point, segment.second));
	// направляющий вектор, вектор из одного из концов отрезка к рассматриваемой точке
	Vector direction(segment.first, segment.second), to_point(segment.first, point);
	if (direction.length() < precision)
		return result;
	long double coef = scalar(direction, to_point) / (direction.length() * direction.length());
	// если основание перпендикуляра лежит на отрезке
	if (coef > precision && coef < 1.0 - precision)
		result = min(result, points_distance(point, segment.first + direction * coef));
	return result;
}

// Используем то, что функция расстояния от точки до отрезка унимодальна на отрезке.
long double segments_distance(const Segment& first_segment, const Segment& second_segment) {
	Point left = second_segment.first, right = second_segment.second;
	for (int i = 0; i < operations_number; i++) {
		Vector current(left, right);
		Point first_middle = left + current / 3.0, second_middle = right - current / 3.0;
		if (point_to_segment_distance(first_middle, first_segment) < 
			point_to_segment_distance(second_middle, first_segment) - precision)
			right = second_middle;
		else
			left = first_middle;
	}
	return point_to_segment_distance(left, first_segment);
}

void solve() {
	Segment first_segment, second_segment;
	cin >> first_segment >> second_segment;
	cout << fixed << setprecision(10) << segments_distance(first_segment, second_segment);
}
                                                                                                      
int main() 
{
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                         
	cout.tie(0);
	solve();
	return 0;
}