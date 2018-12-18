#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
                                
using namespace std;

const long double pi = acos(-1);
// константа, отвечающая за точность вычислений
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
	
	Vector(const Point& A, const Point& B): x(B.x - A.x), y(B.y - A.y), z(B.z - A.z) {}
	
	long double length() const {
		return pow(x * x + y * y + z * z, 0.5);
	}
	
	// Нормировка.
	void make_unit() {
		long double len = length();
		x /= len;
		y /= len;
		z /= len;
	}
	
	Vector operator +(const Vector& other) {
		return Vector(x + other.x, y + other.y, z + other.z);
	}
	
	Vector operator -(const Vector& other) {
		return Vector(x - other.x, y - other.y, z - other.z);
	}
	
	Vector operator *(long double multiplier) {
		return Vector(x * multiplier, y * multiplier, z * multiplier);
	}
	
	Vector& operator *=(long double multiplier) {
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
		return *this;
	}
};                              

// Векторное произведение.
Vector cross_product(const Vector& a, const Vector& b) {
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// Скалярное произведение.
long double scalar_product(const Vector& a, const Vector& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// "Отнормированное" скалярное произведение - по сути просто косинус угла между векторами.
long double norm_scalar_product(const Vector& a, const Vector& b) {
	if (abs(a.length()) < precision || abs(b.length()) < precision)
		return 0.0;
	return scalar_product(a, b) / (a.length() * b.length());
}


// Грань выпуклой оболочки. Задаётся 3 точками, а ориентация - их порядком.
struct Face {
	int first, second, third;
	
	Face() = default;
	
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

ostream& operator <<(ostream& out, const Face& face) {
	out << "3 " << face.first << " " << face.second << " " << face.third;
	return out;
}

bool compare(const Face& a, const Face& b) {
	return (a.first < b.first || (a.first == b.first && a.second < b.second) ||
			(a.first == b.first && a.second == b.second && a.third < b.third));
}

class Convex_hull_builder {
private:
	vector <Face> faces;
	vector <Point> points;
	
	int find_first_point() {
		int first = 0;
		int n = points.size();
		// Ищем первую точку - с лексикографически минимальными координатами (z, y, x) (сравниваются именно в таком порядке).
		for (int i = 0; i < n; i++) {
			if ((points[i].z < points[first].z - precision) || 
				(abs(points[i].z - points[first].z) < precision && points[i].y < points[first].y - precision) ||
				(abs(points[i].z - points[first].z) < precision &&
				 abs(points[i].y - points[first].y) < precision &&
				 points[i].x < points[first].x - precision))
				first = i;
		}
		return first;
	}

	int find_second_point(int first) {
		/* Вторая точка такова, что отрезок, соединяющий её с первой, 
		 * образует наибольший угол с осью z среди всех подобных отрезков.
		 * Этот отрезок будет лежать в выпуклой оболочке.*/
		int second = -1;
		int n = points.size();
		long double min_norm_scalar = 1.1;
		Vector z(0.0, 0.0, 1.0);
		for (int i = 0; i < n; i++) {
			if (i == first)
				continue;
			Vector current(points[first], points[i]);
			if (norm_scalar_product(current, z) < min_norm_scalar - precision) {
				min_norm_scalar = norm_scalar_product(current, z);
				second = i;
			}
		}
		return second;
	}

	int find_third_point(int first, int second) {
		/* Третья точка образует вместе с первыми двумя "крайнюю" грань - 
		 * т.е. такую, что все остальные точки лежат по одну сторону от неё. */
		int third = -1;
		int n = points.size();
		long double max_norm_scalar = -1.1;
		Vector first_edge(points[first], points[second]);
		
		// Хочется, чтобы нормальный вектор к первому ребру смотрел вверх по оси z.
		Vector at_line = Vector(first_edge.y, -first_edge.x, 0.0);
		Vector normal = cross_product(at_line, first_edge);
		normal.make_unit();
		if (normal.z < -precision)
			normal *= (-1);
		for (int i = 0; i < n; i++) {
			if (i == first || i == second)
				continue;
			Vector normal_to_plane = cross_product(Vector(points[first], points[i]), first_edge);
			normal_to_plane.make_unit();
			if (norm_scalar_product(normal, normal_to_plane) > max_norm_scalar + precision) {
				third = i;
				max_norm_scalar = norm_scalar_product(normal, normal_to_plane);
			}
		}
		return third;
	}

	Face find_first_face() {
		int first = find_first_point();
		int second = find_second_point(first);
		int third = find_third_point(first, second);
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
	Face find_next_face(Face face, const int position) {
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
		Vector normal = cross_product(first_edge, second_edge), first = cross_product(normal, first_edge);
		first.make_unit();
		/* Минимальному углу между плоскостями (т.е. их нормальными векторами) соответствует 
		 * максимальное значение нормированного скалярного произведения. */
		int third = -1;
		long double max_norm_scalar = -1.1;
		for (int i = 0; i < n; i++) {
			if (i == face.first || i == face.second || i == face.third)
				continue;
			Vector current(points[face.first], points[i]);
			Vector norm_to_plane = cross_product(first_edge * (-1), current);
			
			if (norm_scalar_product(norm_to_plane, normal) > max_norm_scalar + precision) {
				max_norm_scalar = norm_scalar_product(norm_to_plane, normal);
				third = i;
			}
		}
		return Face(face.second, face.first, third);
	}
	
	// Оборачивает грань face_to_wrap по тем рёбрам, которые есть в контуре.
	void try_to_wrap(Face face_to_wrap, set <pair <int, int> >& border, queue <Face>& faces_queue) {
		if (border.find({face_to_wrap.first, face_to_wrap.second}) != border.end()) {
				Face next = find_next_face(face_to_wrap, 0); // приходит уже с правильной ориентацией
				add_edges(next, border);
				faces_queue.push(next);
			}
			if (border.find({face_to_wrap.second, face_to_wrap.third}) != border.end()) {
				Face next = find_next_face(face_to_wrap, 1);
				add_edges(next, border);
				faces_queue.push(next);
			}
			if (border.find({face_to_wrap.third, face_to_wrap.first}) != border.end()) {
				Face next = find_next_face(face_to_wrap, 2);
				add_edges(next, border);
				faces_queue.push(next);
			}
	}
	
public:
	Convex_hull_builder(const vector <Point>& points_): points(points_) {
		Face first_face = find_first_face();
		queue <Face> faces_queue;
		faces_queue.push(first_face);
		set <pair <int, int> > border;
		border.insert({first_face.first, first_face.second});
		border.insert({first_face.second, first_face.third});
		border.insert({first_face.third, first_face.first});
		while (!faces_queue.empty()) {
			Face current = faces_queue.front();
			faces_queue.pop();
			try_to_wrap(current, border, faces_queue);
			faces.push_back(current);
		}
		
		for (auto& x: faces)
			x.sort();
		sort(faces.begin(), faces.end(), compare);
	}
	
	friend ostream& operator <<(ostream& out, const Convex_hull_builder& builder) {
		out << builder.faces.size() << '\n';
		for (auto x: builder.faces)
			out << x << '\n';
		return out;
	}
};

void solve() {
	int tests_number = 0;
	cin >> tests_number;
	for (int i = 0; i < tests_number; i++) {
		int points_number = 0;
		cin >> points_number;
		vector <Point> points;
		for (int j = 0; j < points_number; j++) {
			Point current;
			cin >> current;
			points.push_back(current);
		}
		cout << Convex_hull_builder(points);
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