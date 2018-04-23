#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct Point {
	double x;
	double y;

	bool operator==(const Point& p) { return x == p.x && y == p.y; }
	double distanceToLine(Point a, Point b);
};

// расстояние от точки до прямой ab
double Point::distanceToLine(Point a, Point b) {
	return fabs((b.x - a.x) * (a.y - y) - (a.x - x) * (b.y - a.y)) / sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

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

// построение минимальной выпуклой оболчки по алгоритму Джарвиса
vector<Point> jarvisHull(vector<Point> points) {
	size_t n = points.size();

	// определяем самую левую точку
	for (size_t i = 1; i < n; i++)
		if (points[i].x < points[0].x)
			swap(points[i], points[0]);

	vector<Point> H;
	H.push_back(points[0]);

	points.erase(points.begin());
	points.push_back(H[0]);

	while (1) {
		int right = 0;

		for (size_t i = 1; i < points.size(); i++) {
			if (rotate(H[H.size() - 1], points[right], points[i]) < 0)
				right = i;
		}

		if (points[right] == H[0])
			break;
		else {
			H.push_back(points[right]);
			points.erase(points.begin() + right);
		}
	}

	return H;
}

// построение минимальной выпуклой оболчки по алгоритму Эндрю
vector<Point> andrewHull(vector<Point> points) {
	size_t n = points.size();

	vector<Point> hull;

	// сортируем точки по удалённости
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n - 1; j++) {
			if ((points[j].x >= points[j + 1].x) && (points[j].x != points[j + 1].x || points[j].y >= points[j + 1].y)) {
				Point p = points[j];
				points[j] = points[j + 1];
				points[j + 1] = p;
			}
		}
	}

	// строим нижнюю часть оболочки
	for (size_t i = 0; i < n; i++) {
		while (hull.size() >= 2 && rotate(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0)
			hull.pop_back();

		hull.push_back(points[i]);
	}

	// строим верхнюю часть оболочки
	for (size_t i = n - 1, t = hull.size() + 1; i > 0; i--) {
		while (hull.size() >= t && rotate(hull[hull.size() - 2], hull[hull.size() - 1], points[i - 1]) <= 0)
			hull.pop_back();

		hull.push_back(points[i - 1]);
	}

	hull.pop_back();

	return hull;
}

// разделение точек на два подмножества для алгоритмы быстрой выпуклой оболочки
vector<Point> divide(vector<Point> points, Point p1, Point p2) {
	vector<Point> hull;

	if (points.size() == 0)
		return hull;

	if (points.size() == 1) {
		hull.push_back(points[0]);
		return hull;
	}

	Point maxDistancePoint = points[0];
	size_t index = 0;
	double distance = 0.0;
	vector<Point> l1, l2;

	for (size_t i = 0; i < points.size(); i++) {
		if (points[i].distanceToLine(p1, p2) > distance) {
			distance = points[i].distanceToLine(p1, p2);
			maxDistancePoint = points[i];
			index = i;
		}
	}

	points.erase(points.begin() + index);

	for (size_t i = 0; i < points.size(); i++) {
		if (rotate(points[i], p1, maxDistancePoint) <= 0) {
			l1.push_back(points[i]);
		}
		else if (rotate(points[i], maxDistancePoint, p2) <= 0) {
			l2.push_back(points[i]);
		}
	}

	vector<Point> bottom = divide(l1, p1, maxDistancePoint);
	vector<Point> top = divide(l2, maxDistancePoint, p2);

	hull.insert(hull.end(), bottom.begin(), bottom.end());
	hull.push_back(maxDistancePoint);
	hull.insert(hull.end(), top.begin(), top.end());

	return hull;
}

// построение минимальной выпуклой оболчки по быстрому алгоритму
vector<Point> quickHull(vector<Point> points) {
	vector<Point> convexHull;

	Point leftMostPoint = points[0];
	Point rightMostPoint = points[0];

	// находим самую левую и самую првую точки
	for (size_t i = 1; i < points.size(); i++) {
		if (points[i].x > rightMostPoint.x) {
			rightMostPoint = points[i];
		}
		else if (points[i].x < leftMostPoint.x) {
			leftMostPoint = points[i];
		}
	}

	vector<Point> leftOfLine;
	vector<Point> rightOfLine;

	// разделяем на два подмножества - выше прямой и ниже
	for (size_t i = 0; i < points.size(); i++) {
		if (points[i] == rightMostPoint || points[i] == leftMostPoint)
			continue;

		if (rotate(points[i], leftMostPoint, rightMostPoint) <= 0) {
			leftOfLine.push_back(points[i]);
		}
		else {
			rightOfLine.push_back(points[i]);
		}
	}

	convexHull.push_back(leftMostPoint);

	vector<Point> hull = divide(leftOfLine, leftMostPoint, rightMostPoint);
	convexHull.insert(convexHull.end(), hull.begin(), hull.end());

	convexHull.push_back(rightMostPoint);

	hull = divide(rightOfLine, rightMostPoint, leftMostPoint);
	convexHull.insert(convexHull.end(), hull.begin(), hull.end());

	return convexHull;
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

	vector<Point> jarvisPoints = jarvisHull(points);
	cout << "JarvisHull points: " << endl;
	printPoints(jarvisPoints);

	vector<Point> andrewPoints = andrewHull(points);
	cout << "AndrewHull points: " << endl;
	printPoints(andrewPoints);

	vector<Point> quickPoints = quickHull(points);
	cout << "QuickHull points: " << endl;
	printPoints(quickPoints);
}
