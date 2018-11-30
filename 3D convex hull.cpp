#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
                                
using namespace std;

const long double pi = acos(-1);
const long double precision = 1e-10;

struct Vector;
 
struct Point {
	long double x, y, z;
	
	Point(): x(0.0), y(0.0), z(0.0) {}
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
	
	// Нормировка.
	void make_unit() {
		long double len = length();
		x /= len;
		y /= len;
		z /= len;
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
	
	Vector operator *=(const long double multiplier) {
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
		return *this;
	}
};                              

// Векторное произведение.
Vector cross (const Vector a, const Vector b) {
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// Скалярное произведение.
long double scalar(const Vector a, const Vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// "Отнормированное" скалярное произведение - по сути просто косинус угла между векторами.
long double norm_scalar(Vector a, Vector b) {
	if (fabs(a.length()) < precision || fabs(b.length()) < precision)
		return 0.0;
	return scalar(a, b) / (a.length() * b.length());
}


// Грань выпуклой оболочки. Задаётся 3 точками, а ориентация - их порядком.
struct Face {
	int first, second, third;
	
	Face(): first(-1), second(-1), third(-1) {}
	
	Face(int first_, int second_, int third_): first(first_), second(second_), third(third_) {}
	
	/* Переход к записи, в которой на первом месте стоит точка с наименьшим номером.
	 * Не меняет ориентацию, нужен только для корректного вывода в соответствии с условием. */
	void sort() {
		if (second < first && second < third) {
			swap(first, second);
			swap(second, third);
		}
		else if (third < first && third < second) {
			swap(first, third);
			swap(second, third);
		}
	}
};

ostream& operator <<(ostream& out, Face& face) {
	out << "Face: " << '(' << face.first << " " << face.second << " " << face.third << ')';
	return out;
}

long double sign(const long double x) {
	return (x < 0.0 ? -1.0 : 1.0);
}


Face find_first_face(const vector <Point>& points) {
	int first = 0, n = points.size();
	// Ищем первую точку - с лексикографически минимальными координатами (z, y, x) (сравниваются именно в таком порядке).
	for (int i = 0; i < n; i++) {
		if ((points[i].z < points[first].z - precision) || 
			(fabs(points[i].z - points[first].z) < precision && points[i].y < points[first].y - precision) ||
			(fabs(points[i].z - points[first].z) < precision &&
			 fabs(points[i].y - points[first].y) < precision &&
			 points[i].x < points[first].x - precision))
			first = i;
	}
	/* Вторая точка такова, что отрезок, соединяющий её с первой, 
	 * образует наибольший угол с осью z среди всех подобных отрезков.
	 * Этот отрезок будет лежать в выпуклой оболочке.*/
	int second = -1;
	long double min_norm_scalar = 1.1;
	Vector z(0.0, 0.0, 1.0);
	for (int i = 0; i < n; i++) {
		if (i == first)
			continue;
		Vector current(points[first], points[i]);
		if (norm_scalar(current, z) < min_norm_scalar - precision) {
			min_norm_scalar = norm_scalar(current, z);
			second = i;
		}
	}
	/* Третья точка образует вместе с первыми двумя "крайнюю" грань - 
	 * т.е. такую, что все остальные точки лежат по одну сторону от неё. */
	int third = -1;
	long double max_norm_scalar = -1.1;
	Vector first_edge(points[first], points[second]);
	
	// Хочется, чтобы нормальный вектор к первому ребру смотрел вверх по оси z.
	Vector at_line = Vector(first_edge.y, -first_edge.x, 0.0);
	Vector normal = cross(at_line, first_edge);
	normal.make_unit();
	if (normal.z < -precision)
		normal *= (-1);
	for (int i = 0; i < n; i++) {
		if (i == first || i == second)
			continue;
		Vector normal_to_plane = cross(Vector(points[first], points[i]), first_edge);
		normal_to_plane.make_unit();
		if (norm_scalar(normal, normal_to_plane) > max_norm_scalar + precision) {
			third = i;
			max_norm_scalar = norm_scalar(normal, normal_to_plane);
		}
	}
	return Face(first, second, third);
}


// Преобразование контура после добавления грани.
void add_edges(const Face& face, set <pair <int, int> >& border) {
	border.insert({face.first, face.second});
	border.insert({face.second, face.third});
	border.insert({face.third, face.first});
	if (border.find({face.second, face.first}) != border.end()) {
		border.erase({face.second, face.first});
		border.erase({face.first, face.second});
	}
	if (border.find({face.third, face.second}) != border.end()) {
		border.erase({face.second, face.third});
		border.erase({face.third, face.second});
	}
	if (border.find({face.first, face.third}) != border.end()) {
		border.erase({face.first, face.third});
		border.erase({face.third, face.first});
	}
}


// Здесь face - та грань, от ребра которой начинается поиск следующей грани. Position показывает, что это за ребро.
Face find_next_face(Face face, const int position, const vector <Point>& points) {
	int n = points.size();
	if (position == 1) {
		swap(face.first, face.second);
		swap(face.second, face.third);
	}
	if (position == 2) {
		swap(face.first, face.third);
		swap(face.second, face.third);
	}
	Vector first_edge(points[face.first], points[face.second]), second_edge(points[face.second], points[face.third]);
	first_edge.make_unit();
	second_edge.make_unit();
	Vector normal = cross(first_edge, second_edge), first = cross(normal, first_edge);
	first.make_unit();
	/* Минимальному углу между плоскостями (т.е. их нормальными векторами) соответствует 
	 * максимальное значение нормированного скалярного произведения. */
	int third = -1;
	long double max_norm_scalar = -1.1;
	for (int i = 0; i < n; i++) {
		if (i == face.first || i == face.second || i == face.third)
			continue;
		Vector current(points[face.first], points[i]);
		Vector norm_to_plane = cross(first_edge * (-1), current);
		
		if (norm_scalar(norm_to_plane, normal) > max_norm_scalar + precision) {
			max_norm_scalar = norm_scalar(norm_to_plane, normal);
			third = i;
		}
	}
	return Face(face.second, face.first, third);
}


// После построения оболочки в faces будет лежать список построенных граней.
void build_convex_hull(const vector <Point>& points, vector <Face>& faces) {
	Face first_face = find_first_face(points);
	queue <Face> faces_queue;
	faces_queue.push(first_face);
	set <pair <int, int> > border;
	border.insert({first_face.first, first_face.second});
	border.insert({first_face.second, first_face.third});
	border.insert({first_face.third, first_face.first});
	while (!faces_queue.empty()) {
		Face current = faces_queue.front();
		faces_queue.pop();
		if (border.find({current.first, current.second}) != border.end()) {
			Face next = find_next_face(current, 0, points); // приходит уже с правильной ориентацией
			add_edges(next, border);
			faces_queue.push(next);
		}
		if (border.find({current.second, current.third}) != border.end()) {
			Face next = find_next_face(current, 1, points);
			add_edges(next, border);
			faces_queue.push(next);
		}
		if (border.find({current.third, current.first}) != border.end()) {
			Face next = find_next_face(current, 2, points);
			add_edges(next, border);
			faces_queue.push(next);
		}
		faces.push_back(current);
	}
}

bool compare(Face a, Face b) {
	return (a.first < b.first || (a.first == b.first && a.second < b.second) ||
			(a.first == b.first && a.second == b.second && a.third < b.third));
}

void solve() {
	int tests_number = 0;
	cin >> tests_number;
	for (int i = 0; i < tests_number; i++) {
		int points_number = 0;
		cin >> points_number;
		vector <Point> points;
		vector <Face> faces;
		for (int j = 0; j < points_number; j++) {
			Point current;
			cin >> current;
			points.push_back(current);
		}
		build_convex_hull(points, faces);
		int faces_number = faces.size();
		cout << faces_number << '\n';
		for (int j = 0; j < faces_number; j++)
			faces[j].sort();
		sort(faces.begin(), faces.end(), compare);
		for (int j = 0; j < faces_number; j++)
			cout << "3 " << faces[j].first << " " << faces[j].second << " " << faces[j].third << '\n';
	}
}

int main()
{
	ios_base::sync_with_stdio(0);
	cin.tie(0);                                                                                
	cout.tie(0);
	solve();
	return 0;
}