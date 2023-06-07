#include <iostream>
#include <utility>
#include <string>
#include <fstream>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <time.h>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

bool mapRenderDone = false;

class Move {
protected:
	vector<string> map;
	size_t loadMap();
	COORD cursorPosition;
private:
	char icon;
	string mapName;
	int directX;
	int directY;
	pair<short, short> findIconPosition();
	void resetPosition(COORD delete_pos);
	void renderMap();
	void updateMap(short x, short y);
	virtual bool checkCollision(short x, short y) = 0;
public:
	Move(char c, string mapName_) : icon(c), mapName(mapName_)
	{
		loadMap();
		auto[Y, X] = findIconPosition();
		cursorPosition.Y = X;
		cursorPosition.X = Y;
		directX = 1;

		if (mapRenderDone == false) {
			renderMap();
			mapRenderDone = true;
		}
	}

	void setDX(int x) {
		directX = x;
	}
	void setDY(int y) {
		directY = y;
	}

	int getDX() { return directX; }
	int getDY() { return directY; }

	void move();

	pair<short, short> getCursorPositionInfo() {
		return make_pair(cursorPosition.X, cursorPosition.Y);
	}

	void getCursorPositionInfoDisplay(short height) {
		SetConsoleCursorPosition(hConsole, COORD({ 0, height }));
		cout << "X: " << cursorPosition.X << ", Y: "
			<< cursorPosition.Y << endl;
	}
};

size_t Move::loadMap() {
	ifstream file(mapName);
	string line;
	size_t all_count_pointer = 0;
	while (getline(file, line)) {
		string strhandler = "";
		for (size_t i = 0; i < line.size(); i++) {
			switch (line[i])
			{
			case '#':
				strhandler += char(219);
				break;
			case ' ':
				strhandler += '.';
				++all_count_pointer;
				break;
			default:
				strhandler += line[i];
			}
		}
		map.push_back(strhandler);
	}

	return all_count_pointer;
}

bool Move::checkCollision(short x, short y) {
	return map[y][x] != ' ';
}

void Move::renderMap() {
	for (size_t i = 0; i < map.size(); i++) {
		cout << map[i] << endl;
	}
}

pair<short, short> Move::findIconPosition() {
	ifstream file(mapName);
	string line;
	size_t row = 0;
	while (getline(file, line)) {
		for (size_t i = 0; i < line.size(); i++) {
			if (line[i] == icon) {
				return make_pair(i, row);
			}
		}
		++row;
	}
	return make_pair(-1, -1);
}

void Move::resetPosition(COORD delete_pos) {
	SetConsoleCursorPosition(hConsole, delete_pos);
	putchar(' ');
}

void Move::updateMap(short x, short y) {
	SetConsoleCursorPosition(hConsole, COORD({ y, x }));
	cursorPosition = COORD({ y, x });
	putchar(icon);
}

void Move::move() {
	resetPosition(cursorPosition);
	short nextX = cursorPosition.Y + directY;
	short nextY = cursorPosition.X + directX;

	if (icon != '@') {
		SetConsoleCursorPosition(hConsole, cursorPosition);
		putchar('.');
	}
	if (!checkCollision(nextY, nextX))
	{
		cursorPosition.Y = nextX;
		cursorPosition.X = nextY;
	}
	else {
		directX = 0;
		directY = 0;
	}

	updateMap(cursorPosition.Y, cursorPosition.X);
}

class Pacman : public Move {
private:
	size_t count_pointer = 0;
	size_t all_count_pointer = 0;
public:
	Pacman() : Move('@', "map.txt") { 
		all_count_pointer = loadMap();
	}

	bool checkWin();
	void checkPoint();
private:
	
	bool checkCollision(short x, short y) override {
		return map[y][x] == char(219);
	}
};

bool Pacman::checkWin() {
	return all_count_pointer == count_pointer;
}

void Pacman::checkPoint() {
	if (map[cursorPosition.Y][cursorPosition.X] == '.') {
		++count_pointer;
		map[cursorPosition.Y][cursorPosition.X] = ' ';
	}

	SetConsoleCursorPosition(hConsole, COORD({ 0, 12 }));
	cout << "Pointer count: " << count_pointer << "/" << all_count_pointer;
}

class Enemy : public Move {
private:
	size_t hack_python_count = 0;
public:
	Enemy() : Move('$', "map.txt") {
		setDX(rand() % 3 - 1);
		setDY(rand() % 3 - 1);
	}

private:
	bool checkCollision(short x, short y) override {
		if (map[y][x] != char(219)) {
			setDX(rand() % 3 - 1);
		}

		return map[y][x] == char(219);
	}
};

int main(int argc, char* argv[]) {
	Pacman pacman;
	Enemy enemy;

	while (true) 
	{
		if (_kbhit())
		{
			switch (char c; tolower(c = _getch()))
			{
			case VK_UP:
			case 0x77:
				pacman.setDY(-1);
				pacman.setDX(0);
				break;
			case VK_LEFT:
			case 0x61:
				pacman.setDY(0);
				pacman.setDX(-1);
				break;
			case VK_DOWN:
			case 0x73:
				pacman.setDY(1);
				pacman.setDX(0);
				break;
			case VK_RIGHT:
			case 0x64:
				pacman.setDX(1);
				pacman.setDY(0);
				break;
			}
		}

		enemy.move();
		Sleep(30);
		pacman.move();
		Sleep(70);
		pacman.checkPoint();

		if (pacman.getCursorPositionInfo() == enemy.getCursorPositionInfo()) {
			SetConsoleCursorPosition(hConsole, COORD({ 0, 15 }));
			cout << "You dead" << endl;
			break;
		}
	}
	cin.get();
}