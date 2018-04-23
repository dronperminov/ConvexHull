#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Point {
	double x;
	double y;

	bool operator==(const Point& p) { return x == p.x && y == p.y; }
};

// получение вектора из n точек
vector<Point> getPoints(ifstream& f) {
	vector<Point> points;
	Point p;

	while (f >> p.x >> p.y)
		points.push_back(p);

	return points;
}

// вывод на экран вектора точек
void printPoints(vector<Point> &points) {
	for (size_t i = 0; i < points.size(); i++)
		cout << "(" << points[i].x << ", " << points[i].y << ")" << endl;

	cout << endl;
}

// с какой стороны от вектора ab находится точка c (>0 — левая сторона, < 0 — правая).
int rotate(Point a, Point b, Point c) {
	return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}

// построение минимальной выпуклой оболчки по алгоритму Грэхэма
vector<Point> grahamHull(vector<Point> points) {
	size_t n = points.size();

	// определяем самую левую точку
	for (size_t i = 1; i < n; i++)
		if (points[i].x < points[0].x)
			swap(points[i], points[0]);

	// сортировка всех точек по степени "левизны" относительно стартовой точки
	for (size_t i = 2; i < n; i++) {
		int j = i;

		while (j > 1 && rotate(points[0], points[j - 1], points[j]) < 0) {
			swap(points[j], points[j - 1]);
			j--;
		}
	}

	vector<Point> S;
	S.push_back(points[0]);
	S.push_back(points[1]);

	// срезание углов (удаление вершин с правым поворотом)
	for (size_t i = 2; i < n; i++) {
		while (rotate(S[S.size() - 2], S[S.size() - 1], points[i]) <= 0)
			S.pop_back();

		S.push_back(points[i]);
	}

	return S;
}

int main() {
	string path;

	cout << "Enter path: ";
	getline(cin, path);

	ifstream f;
	f.open(path, ios::in);

	if (!f) {
		cout << "Error during opening file '" << path << "'" << endl;
		return -1;
	}

	vector<Point> points = getPoints(f);
	f.close();

	cout << "Readed points: " << endl;
	printPoints(points);

	vector<Point> grahamPoints = grahamHull(points);
	cout << "GrahamHull points: " << endl;
	printPoints(grahamPoints);
}
