#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_set>
#include <tuple>
#include <clocale>
#include <locale.h>
#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <vector>





/*
Программа создает окно с картой размером 10 на 10 клеток.
Левой кнопкой мыши можно перемещать роботов, устанавливать цели или ставить/удалять препятствия.
Клавиша "O" включает/выключает режим размещения препятствий.
Клавиша "1" выключает режим установки робота 1 и его цель.
Клавиша "2" выключает режим установки робота 2 и его цель.
Клавиша "3" выключает режим установки робота 3 и его цель.
Клавиша "P" запускает алгоритм поиска пути методом потенциальных полей.
Клавиша "D" запускает алгоритм поиска пути методом Дейкстры.
Препятствие обозначается черным цветом клетки,
Цели роботов 1, 2, 3 обозначаются синим, зеленым и красным цветом клетки соответственно,
Роботы 1, 2, 3 обозначаются окружностями тех же цветов, что и их цели.
*/



const int CELL_SIZE = 50; // Размер клетки на карте
const float CELL_SIZE_F = 50.0; // Размер клетки на карте
const int MAP_SIZE = 10; // Размер карты
const float OUTLINE_THICKNESS = 1.0; // толщина контура клетки на карте
const float RADIUS = 24.0; // радиус робота


void timer(int x)
{
    sf::Clock clock;
    float time = 0;
    while (time < x)
    {
        time = clock.getElapsedTime().asSeconds();
    }
}

std::set<int> ways(int c, std::set<int> cols) {
    std::set<int> variants;
    variants.insert(c);
    if ((c + 1) % 10 != 0 && cols.count(c + 1) == false) {
        variants.insert(c + 1);
    }
    if ((c - 1) % 10 != 9 && cols.count(c - 1) == false && c != 0) {
        variants.insert(c - 1);
    }
    if (c + 10 <= 99 && cols.count(c + 10) == false) {
        variants.insert(c + 10);
    }
    if (c - 10 >= 0 && cols.count(c - 10) == false) {
        variants.insert(c - 10);
    }
    return variants;
}

std::set<int> rem(std::set<int> pways, int c1, int c2, std::set<int> cols) {
    std::set<int> c1w = { c1 };
    std::set<int> c2w = { c2 };
    for (int i : c1w) {
        if (pways.count(i)) {
            pways.erase(i);
        }
    }
    for (int i : c2w) {
        if (pways.count(i)) {
            pways.erase(i);
        }
    }
    return pways;
}

std::set<int> near_with_no_collision(int c1, int c2, int c3, int i, int j, int k) {
    std::set<int> variants;
    if ((c1 + 10 == c2 and c2 + 10 == j) or (c1 - 10 == c2 and c2 - 10 == j) or (c1 + 1 == c2 and c2 + 1 == j) or (c1 - 1 == c2 and c2 - 1 == j)) {
        variants.insert(c2 + j * 100 + k * 10000);
    }
    if ((c2 + 10 == c1 and c1 + 10 == i) or (c2 - 10 == c1 and c1 - 10 == i) or (c2 + 1 == c1 and c1 + 1 == i) or (c2 - 1 == c1 and c1 - 1 == i)) {
        variants.insert(i + c1 * 100 + k * 10000);
    }
    if ((c2 + 10 == c3 and c3 + 10 == j) or (c2 - 10 == c3 and c3 - 10 == j) or (c2 + 1 == c3 and c3 + 1 == j) or (c2 - 1 == c3 and c3 - 1 == j)) {
        variants.insert(i + c3 * 100 + k * 10000);
    }
    if ((c3 + 10 == c2 and c2 + 10 == i) or (c3 - 10 == c2 and c2 - 10 == i) or (c3 + 1 == c2 and c2 + 1 == i) or (c3 - 1 == c2 and c2 - 1 == i)) {
        variants.insert(i + j * 100 + c2 * 10000);
    }
    if ((c1 + 10 == c3 and c3 + 10 == j) or (c1 - 10 == c3 and c3 - 10 == j) or (c1 + 1 == c3 and c3 + 1 == j) or (c1 - 1 == c3 and c3 - 1 == j)) {
        variants.insert(c3 + j * 100 + k * 10000);
    }
    if ((c3 + 10 == c1 and c1 + 10 == i) or (c3 - 10 == c1 and c1 - 10 == i) or (c3 + 1 == c1 and c1 + 1 == i) or (c3 - 1 == c1 and c1 - 1 == i)) {
        variants.insert(i + j * 100 + c3 * 10000);
    }
    return variants;
}

std::set<int> ways_united(int c, std::set<int> cols) {
    std::set<int> variants;
    int c1 = c % 100;
    int c2 = c % 10000 / 100;
    int c3 = c % 1000000 / 10000;
    std::set<int> c1vars = rem(ways(c1, cols), c2, c3, cols);
    std::set<int> c2vars = rem(ways(c2, cols), c1, c3, cols);
    std::set<int> c3vars = rem(ways(c3, cols), c1, c2, cols);
    for (int i : c1vars) {
        for (int j : c2vars) {
            for (int k : c3vars) {
                if (i != j and j != k and i != k) {
                    variants.insert(i + j * 100 + k * 10000);
                    std::set<int> add = near_with_no_collision(c1, c2, c3, i, j, k);
                    if (!add.empty()) {
                        variants.insert(add.begin(), add.end());
                    }
                }
            }
        }
    }
    return variants;
}

std::map <int, int> shortest_way(int start, int target, std::set<int> cols) {
    if (start == target) {
        std::cout << "Агенты уже в точке назначения!" << std::endl;
        return std::map <int, int>{};
    }
    else {
        std::set<int> passed = cols;
        std::map<int, std::set<int>> points_at_dist;
        int c = 1;
        points_at_dist[0] = { start };
        bool found = false;
        while (true) {
            std::cout << "Итерация " << c << std::endl;
            int ps = passed.size();
            std::set<int> nc = points_at_dist[c - 1];
            passed.insert(nc.begin(), nc.end());
            for (int n : points_at_dist[c - 1]) {
                std::set<int> buf = ways_united(n, cols);
                points_at_dist[c].insert(buf.begin(), buf.end());
            }
            std::cout << "Сохранено " << points_at_dist[c].size() << " возможных шагов." << std::endl;
            if (points_at_dist[c].count(target)) {
                found = true;
                break;
            }
            if (passed.size() == ps) {
                break;
            }
            c++;
        }
        if (found) {
            std::cout << "Решение найдено!" << std::endl;
            std::map <int, int> path;
            int point = target;
            path[c] = target;
            for (int i = c; i >= 0; i--) {
                std::set<int> ws = ways_united(point, cols);
                int mindif = 4;
                for (int n : ws) {
                    if (points_at_dist[i - 1].count(n)) {
                        int p1 = (path[i] / 10000) - (n / 10000);
                        if (p1 < 0)
                            p1 = -p1;
                        p1 = p1 % 10 + p1 / 10;
                        int p2 = (path[i] % 10000 / 100) - (n % 10000 / 100);
                        if (p2 < 0)
                            p2 = -p2;
                        p2 = p2 % 10 + p2 / 10;
                        int p3 = (path[i] % 100) - (n % 100);
                        if (p3 < 0)
                            p3 = -p3;
                        p3 = p3 % 10 + p3 / 10;
                        int dif = p1 + p2 + p3;
                        if (dif < mindif) {
                            point = n;
                            mindif = dif;
                        }
                    }
                }
                path[i - 1] = point;
            }
            path[0] = start;
            return path;
        }
        else {
            std::cout << "Решение не найдено!" << std::endl;
            return std::map <int, int>{};
        }
    }
}

enum class CellType {
    Empty,
    Obstacle,
    Goal1,
    Goal2,
    Goal3,
};

enum class ModeSetCell {
    ModeRobot1,
    ModeRobot2,
    ModeRobot3,
    ModeObstacle
};

class Map {
public:
    Map() {
        for (int i = 0; i < MAP_SIZE; i++) {
            std::vector<CellType> row(MAP_SIZE, CellType::Empty);
            cells.push_back(row);
        }
    }

    void setCell(int x, int y, CellType type) {
        cells[x][y] = type;
    }

    CellType getCell(int x, int y) {
        return cells[x][y];
    }

private:
    std::vector<std::vector<CellType>> cells;
};

//Создание уникального идентификатора положения роботов
int createIdentificator(int x1, int y1, int x2, int y2, int x3, int y3) {
    return x3 + 10 * y3 + x2 * 100 + y2 * 1000 + x1 * 10000 + y1 * 100000;
}

void drawMap(sf::RenderWindow& window, Map map, int robots_identificator) {
    bool placingRobot = false; //состояние, что в клетке находится любой робот
    int robotX3 = robots_identificator % 10;
    int robotY3 = (robots_identificator / 10) % 10;
    int robotX2 = (robots_identificator / 100) % 10;
    int robotY2 = (robots_identificator / 1000) % 10;
    int robotX1 = (robots_identificator / 10000) % 10;
    int robotY1 = robots_identificator / 100000;
    // Отрисовка карты
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int y = 0; y < MAP_SIZE; y++) {
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE_F, CELL_SIZE_F));
            cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            cell.setOutlineColor(sf::Color::Black);
            cell.setOutlineThickness(OUTLINE_THICKNESS);

            sf::CircleShape robot_image(RADIUS);
            robot_image.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            robot_image.setOutlineColor(sf::Color::Black);
            robot_image.setOutlineThickness(2 * OUTLINE_THICKNESS);
            placingRobot = ((x == robotX1) and (y == robotY1)) or ((x == robotX2) and (y == robotY2)) or ((x == robotX3) and (y == robotY3));
            if (map.getCell(x, y) == CellType::Goal1) {
                // Цель 1
                cell.setFillColor(sf::Color::Blue);
            }
            else if (map.getCell(x, y) == CellType::Goal2) {
                // Цель 2
                cell.setFillColor(sf::Color::Green);
            }
            else if (map.getCell(x, y) == CellType::Goal3) {
                // Цель 3
                cell.setFillColor(sf::Color::Red);
            }
            else if (map.getCell(x, y) == CellType::Obstacle) {
                // Препятствие
                cell.setFillColor(sf::Color::Black);
            }
            else {
                // Пустая клетка
                cell.setFillColor(sf::Color::White);
            }

            if (placingRobot) {
                // Робот
                //cell.setFillColor(sf::Color::White);
                if ((x == robotX1) and (y == robotY1)) {
                    robot_image.setFillColor(sf::Color::Blue);
                }
                else if ((x == robotX2) and (y == robotY2)) {
                    robot_image.setFillColor(sf::Color::Green);
                }
                else if ((x == robotX3) and (y == robotY3)) {
                    robot_image.setFillColor(sf::Color::Red);
                }
            }


            window.draw(cell);
            if (placingRobot) window.draw(robot_image);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_SIZE, CELL_SIZE * MAP_SIZE), "Pathfinding");
    Map map;
    int robotX1 = 0, robotY1 = 0;
    int robotX2 = 1, robotY2 = 1;
    int robotX3 = 2, robotY3 = 2;
    int goalX1 = 0, goalY1 = 0;
    int goalX2 = 1, goalY2 = 1;
    int goalX3 = 2, goalY3 = 2;
    map.setCell(goalX1, goalY1, CellType::Goal1);
    map.setCell(goalX2, goalY2, CellType::Goal2);
    map.setCell(goalX3, goalY3, CellType::Goal3);
    bool Robot1InCell = true; //состояние, что в клетке находится робот
    bool Robot2InCell = true;
    bool Robot3InCell = true;
    bool Goal1InCell = true; //состояние, что в клетке находится робот
    bool Goal2InCell = true;
    bool Goal3InCell = true;
    bool clickable = true;
    ModeSetCell mode = ModeSetCell::ModeRobot1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed and clickable) {
                if (event.mouseButton.button == sf::Mouse::Left and clickable) {
                    int cellX = event.mouseButton.x / CELL_SIZE;
                    int cellY = event.mouseButton.y / CELL_SIZE;
                    Robot1InCell = (cellX == robotX1) and (cellY == robotY1);
                    Robot2InCell = (cellX == robotX2) and (cellY == robotY2);
                    Robot3InCell = (cellX == robotX3) and (cellY == robotY3);
                    // Устанавливаем цели или ставим/удаляем препятствия
                    if (map.getCell(cellX, cellY) == CellType::Empty) {
                        switch (mode)
                        {
                        case (ModeSetCell::ModeObstacle):
                            if (!Robot1InCell and !Robot2InCell and !Robot3InCell) {
                                map.setCell(cellX, cellY, CellType::Obstacle);
                            }
                            break;
                        case (ModeSetCell::ModeRobot1):
                            map.setCell(goalX1, goalY1, CellType::Empty);
                            map.setCell(cellX, cellY, CellType::Goal1);
                            goalX1 = cellX;
                            goalY1 = cellY;
                            break;
                        case (ModeSetCell::ModeRobot2):
                            map.setCell(goalX2, goalY2, CellType::Empty);
                            map.setCell(cellX, cellY, CellType::Goal2);
                            goalX2 = cellX;
                            goalY2 = cellY;
                            break;
                        case (ModeSetCell::ModeRobot3):
                            map.setCell(goalX3, goalY3, CellType::Empty);
                            map.setCell(cellX, cellY, CellType::Goal3);
                            goalX3 = cellX;
                            goalY3 = cellY;
                            break;
                        default:
                            map.setCell(cellX, cellY, CellType::Empty);
                            break;
                        }
                    }
                    else if (map.getCell(cellX, cellY) == CellType::Obstacle) {
                        map.setCell(cellX, cellY, CellType::Empty);
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right and clickable) {
                    int cellX = event.mouseButton.x / CELL_SIZE;
                    int cellY = event.mouseButton.y / CELL_SIZE;
                    Robot1InCell = (cellX == robotX1) and (cellY == robotY1);
                    Robot2InCell = (cellX == robotX2) and (cellY == robotY2);
                    Robot3InCell = (cellX == robotX3) and (cellY == robotY3);

                    // Устанавливаем/удаляем препятствия или помещаем роботов
                    if (map.getCell(cellX, cellY) == CellType::Empty) {
                        switch (mode)
                        {
                        case (ModeSetCell::ModeObstacle):
                            if (!Robot1InCell and !Robot2InCell and !Robot3InCell) {
                                map.setCell(cellX, cellY, CellType::Obstacle);
                            }
                            break;
                        case (ModeSetCell::ModeRobot1):
                            //map.setCell(robotX1, robotY1, CellType::Empty);
                            //map.setCell(cellX, cellY, CellType::Robot1);
                            if (!Robot2InCell and !Robot3InCell) {
                                robotX1 = cellX;
                                robotY1 = cellY;
                            }
                            break;
                        case (ModeSetCell::ModeRobot2):
                            //map.setCell(robotX2, robotY2, CellType::Empty);
                            //map.setCell(cellX, cellY, CellType::Robot2);
                            if (!Robot1InCell and !Robot3InCell) {
                                robotX2 = cellX;
                                robotY2 = cellY;
                            }
                            break;
                        case (ModeSetCell::ModeRobot3):
                            //map.setCell(robotX3, robotY3, CellType::Empty);
                            //map.setCell(cellX, cellY, CellType::Robot3);
                            if (!Robot1InCell and !Robot2InCell) {
                                robotX3 = cellX;
                                robotY3 = cellY;
                            }
                            break;
                        default:
                            map.setCell(cellX, cellY, CellType::Empty);
                            break;
                        }
                    }
                    else if (map.getCell(cellX, cellY) == CellType::Goal1) {
                        if (mode == ModeSetCell::ModeRobot1)
                        {
                            if (!Robot2InCell and !Robot3InCell) {
                                robotX1 = cellX;
                                robotY1 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot2)
                        {
                            if (!Robot1InCell and !Robot3InCell) {
                                robotX2 = cellX;
                                robotY2 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot3)
                        {
                            if (!Robot1InCell and !Robot2InCell) {
                                robotX3 = cellX;
                                robotY3 = cellY;
                            }
                        }
                    }
                    else if (map.getCell(cellX, cellY) == CellType::Goal2) {
                        if (mode == ModeSetCell::ModeRobot1)
                        {
                            if (!Robot2InCell and !Robot3InCell) {
                                robotX1 = cellX;
                                robotY1 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot2)
                        {
                            if (!Robot1InCell and !Robot3InCell) {
                                robotX2 = cellX;
                                robotY2 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot3)
                        {
                            if (!Robot1InCell and !Robot2InCell) {
                                robotX3 = cellX;
                                robotY3 = cellY;
                            }
                        }
                    }
                    else if (map.getCell(cellX, cellY) == CellType::Goal3) {
                        if (mode == ModeSetCell::ModeRobot1)
                        {
                            if (!Robot2InCell and !Robot3InCell) {
                                robotX1 = cellX;
                                robotY1 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot2)
                        {
                            if (!Robot1InCell and !Robot3InCell) {
                                robotX2 = cellX;
                                robotY2 = cellY;
                            }
                        }
                        else if (mode == ModeSetCell::ModeRobot3)
                        {
                            if (!Robot1InCell and !Robot2InCell) {
                                robotX3 = cellX;
                                robotY3 = cellY;
                            }
                        }
                    }
                    else {
                        map.setCell(cellX, cellY, CellType::Empty);
                    }
                }
            }
            // Выбираем режим выбора установки ячейки или запускаем алгоритмы поиска пути
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::O) {
                    mode = ModeSetCell::ModeObstacle;
                }
                else if (event.key.code == sf::Keyboard::Num1) {
                    mode = ModeSetCell::ModeRobot1;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    mode = ModeSetCell::ModeRobot2;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    mode = ModeSetCell::ModeRobot3;
                }
                else if (event.key.code == sf::Keyboard::P) {
                    //Поиск пути по методу потенциальных полей
                }
                else if (event.key.code == sf::Keyboard::D) {
                    //Поиск пути по Дейкстре
                    clickable = false;
                    std::set<int> cols;
                    for (int i = 0; i < 10; i++) {
                        for (int j = 0; j < 10; j++) {
                            if (map.getCell(j, i) == CellType::Obstacle) {
                                cols.insert(j + i * 10);
                            }
                        }
                    }
                    std::map <int, int> path = shortest_way((robotY1 * 10 + robotX1) * 10000 + (robotY2 * 10 + robotX2) * 100 + (robotY3 * 10 + robotX3), (goalY1 * 10 + goalX1) * 10000 + (goalY2 * 10 + goalX2) * 100 + (goalY3 * 10 + goalX3), cols);
                    if (!path.empty()) {
                        std::cout << "Путь по графу:\n№\tA\tB\tC" << std::endl;
                        for (int i = 0; i < path.size() - 1; i++)
                            std::cout << i << '\t' << path[i] / 10000 << '\t' << path[i] % 10000 / 100 << '\t' << path[i] % 100 << '\t' << std::endl;
                        for (int i = 0; i < path.size() - 1; i++) {
                            robotX3 = path[i] % 10;
                            robotY3 = (path[i] / 10) % 10;
                            robotX2 = (path[i] / 100) % 10;
                            robotY2 = (path[i] / 1000) % 10;
                            robotX1 = (path[i] / 10000) % 10;
                            robotY1 = path[i] / 100000;
                            drawMap(window, map, path[i]);
                            window.display();
                            timer(2);
                        }
                    }
                    clickable = true;
                }
            }
        }

        window.clear(sf::Color::White);


        drawMap(window, map, createIdentificator(robotX1, robotY1, robotX2, robotY2, robotX3, robotY3));

        window.display();
    }

    return 0;
}