#include <algorithm>
#include <iostream>
                                
using namespace std;

const long double pi = acos(-1);
// константа, отвечающая за точность вычислений
const long double precision = 1e-10;

struct Vector;

struct Point {
	long double x, y;
	
	Point(const long double x_, const long double y_): x(x_), y(y_) {}
	
	Point(): x(0.0), y(0.0) {}
	
	Point operator +(const Point other) const {
		return Point(x + other.x, y + other.y);
	}
	
	Point& operator +=(const Vector other);
	
	Point& operator =(const Point& other) {
		x = other.x;
		y = other.y;
		return *this;
	}
	
	Point& operator *=(const long double multiplier) {
		x *= multiplier;
		y *= multiplier;
		return *this;
	}
};

istream& operator >>(istream& in, Point& A) {
	in >> A.x >> A.y;
	return in;
}


struct Vector {
	long double x, y;
	
	Vector(const long double x_, const long double y_): x(x_), y(y_) {}
	
	Vector(const Point A, const Point B): x(B.x - A.x), y(B.y - A.y) {}
	
	long double length() const {
		return pow(x * x + y * y, 0.5);
	}
	
	/* Угол, который вектор составляет с вектором (-1, 0) 
	 * (для самого такого вектора и сонапрвленных ему считаем angle = 2 * pi). */
	long double angle() const {
		long double probable_result = pi - atan2(y, x);
		return (fabs(probable_result) < precision ? 2 * pi : probable_result);
	}
	
	Vector operator +(const Vector other) const {
		return Vector(x + other.x, y + other.y);
	}
	
	Vector& operator *=(const long double multiplier) {
		x *= multiplier;
		y *= multiplier;
		return *this;
	}
};

Point& Point::operator +=(const Vector other) {
	x += other.x;
	y += other.y;
	return *this;
}

long double cross(const Vector a, const Vector b) {
	return a.x * b.y - a.y * b.x;
}

// Выпуклый многоугольник на плоскости.
struct Polygon {
	int size; // количество вершин
	Point start_point; // некоторая вершина, которая считается начальной
	/* Векторы такие, что edges[i] идёт от i-ой точки к {i+1}-й при i = 0...size-2
	 * edges[size - 1] идёт от {size-1}-й точки к начальной. */
	vector <Vector> edges;
	
	Polygon():size(0) {};
	
	/* Построение по множеству точек, указанных в порядке по часовой стрелке.
	 * Считается, что points[0] = points[size] - начальная точка. */
	void build(const vector <Point>& points) {
		size = static_cast <int>(points.size()) - 1;
		start_point = points[0];
		for (int i = 1; i <= size; i++)
			edges.push_back(Vector(points[i - 1], points[i]));
	}
	
	// Центральная симметрия относительно начала координат (точки (0, 0)).
	void make_reflected() {
		start_point *= -1;
		for (int i = 0; i < size; i++) 
			edges[i] *= -1;
	}
	
	// Проверка на то, содержит ли многоугольник точку test_point.
	bool contain (const Point& test_point) const {
		int negative = 0, positive = 0;
		Point current = start_point;
		for (int i = 0; i < size; i++) {
			long double current_cross = cross(edges[i], Vector(current, test_point));
			if (current_cross > precision)
				positive++;
			if (current_cross < -precision)
				negative++;
			current += edges[i];
		}
		return (negative == 0 || positive == 0);
	}
	
	/* Поиск точки с минимальной ординатой, а среди таковых - с минимальной абсциссой, 
	 * а также соответствующее перестроение многоугольника так, чтобы эта точка стала начальной. */
	void find_min_point() {
		Point result = start_point, current = start_point;
		int result_number = 0;
		for (int i = 0; i < size; i++) {
			current += edges[i];
			if (current.y < result.y - precision || 
				(fabs(current.y - result.y) < precision && current.x < result.x - precision)) {
				result = current;
				result_number = i + 1;
			}
		}
		start_point = result;
		vector <Vector> buffer;
		buffer.insert(buffer.begin(), edges.begin() + result_number, edges.end());
		buffer.insert(buffer.end(), edges.begin(), edges.begin() + result_number);
		edges = buffer;
	}
};

istream& operator >>(istream& in, Polygon& polygon) {
	in >> polygon.size >> polygon.start_point;
	Point current = polygon.start_point;
	for (int i = 1; i < polygon.size; i++) {
		Point next;
		in >> next;
		polygon.edges.push_back(Vector(current, next));
		if (i == polygon.size - 1)
			polygon.edges.push_back(Vector(next, polygon.start_point));
		current = next;
	}
	return in;
}

// Определяет сумму Минковского многоугольников first и last и записывает результат в sum.
void minkowski_sum(Polygon& first, Polygon& second, Polygon& sum) {
	first.find_min_point();
	second.find_min_point();
	vector <Point> points;
	Point current = first.start_point + second.start_point;
	points.push_back(current);
	int i = 0, j = 0;
	while (i < first.size && j < second.size) {
		if (first.edges[i].angle() < second.edges[j].angle() - precision) {
			current += first.edges[i];
			i++;
		}
		else if (second.edges[j].angle() < first.edges[i].angle() - precision) {
			current += second.edges[j];
			j++;
		}
		else if (fabs(second.edges[j].angle() - first.edges[i].angle()) < precision) {
			current += first.edges[i] + second.edges[j];
			i++;
			j++;
		}
		points.push_back(current);
	}
	for (; j < second.size; j++) {
		current += second.edges[j];
		points.push_back(current);
	}
	for (; i < first.size; i++) {
		current += first.edges[i];
		points.push_back(current);
	}
	sum.build(points);
}

void solve() {
	Polygon first, second, sum;
	cin >> first >> second;
	second.make_reflected();
	minkowski_sum(first, second, sum);
	cout << (sum.contain(Point(0.0, 0.0)) ? "YES" : "NO");
}

int main() 
{
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
	solve();
	return 0;
}