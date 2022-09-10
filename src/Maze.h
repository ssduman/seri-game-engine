#pragma once

#include "Util.h"
#include "Entity.h"
#include "Logger.h"
#include "Texture.h"
#include "Renderer.h"
#include "CameraMaze.h"

#include <tuple>
#include <ctime>
#include <random>
#include <unordered_map>

typedef std::pair<int, int> m_pair;
typedef std::vector<m_pair> v_pair;

class Maze : public Entity {
public:
    Maze(ICamera* camera, float width, float height, float thickness) : Entity(camera), _width(width), _height(height), _thickness(thickness) {
        Maze::init();
        initTextures();
        initRenderer();

        constexpr float max_maze_area = 1000000.0f;

        const auto mazeArea{ _width * _height };
        const std::string mazeDimensions{ std::to_string(static_cast<int>(_width)) + "x" + std::to_string(static_cast<int>(_height)) };

        LOGGER(info, "generating '" << mazeDimensions << "' maze");
        generateMaze();
        LOGGER(info, "generated '" << mazeDimensions << "' maze");

        if (mazeArea < max_maze_area) {
            LOGGER(info, "solving the maze");
            solveMaze();
            LOGGER(info, "solved the maze");
        } else {
            LOGGER(info, "maze is too big to solve, max is " << max_maze_area);
        }

        _cameraMaze = dynamic_cast<CameraMaze*>(_camera);

        LOGGER(info, "maze init succeeded");
    }

    void resetMaze(float width, float height, float thickness) {
        verticalWallPosition.clear();
        horizontalWallPosition.clear();
        passPosition.clear();
        escapePosition.clear();
        nonEscapePosition.clear();
        mazeMap.clear();
        nodesMap.clear();
        mazeMapTree.clear();
        visitedTable.clear();

        delete wallTexture;
        delete wallVerticalTexture;
        delete passTexture;
        delete escapeTexture;
        delete nonEscapeTexture;
        delete verticalWallRender;
        delete horizontalWallRender;
        delete passRender;
        delete escapeRender;
        delete nonEscapeRender;

        _width = width;
        _height = height;
        _thickness = thickness;

        init();
        initTextures();
        initRenderer();

        generateMaze();
        solveMaze();
    }

    void update() override {}

    void render() override {}

    void display() override {
        wallVerticalTexture->bind();
        verticalWallRender->render();
        wallVerticalTexture->unbind();

        wallTexture->bind();
        horizontalWallRender->render();
        wallTexture->unbind();

        if (!_cameraMaze->showEscapePath()) {
            passTexture->bind();
            passRender->render();
            passTexture->unbind();
        } else {
            escapeTexture->bind();
            escapeRender->render();
            escapeTexture->unbind();

            nonEscapeTexture->bind();
            nonEscapeRender->render();
            nonEscapeTexture->unbind();
        }
    }

    const std::vector<glm::vec3>& getVerticalWallPosition() {
        return verticalWallPosition;
    }

    const std::vector<glm::vec3>& getHorizontalWallPosition() {
        return horizontalWallPosition;
    }

    float getMazeWidth() {
        return _width;
    }

    float getMazeHeight() {
        return _height;
    }

    float getMazeThickness() {
        return _thickness;
    }

private:
    void init() override {}

    void initTextures() {
        wallTexture = new Texture("textures/wall1.png");
        wallVerticalTexture = new Texture("textures/wall2.png");
        passTexture = new Texture("textures/passage.png");
        escapeTexture = new Texture("textures/escape.png");
        nonEscapeTexture = new Texture("textures/nonescape.png");
    }

    void initRenderer() {
        verticalWallRender = new Renderer(_thickness, 0.2f, _thickness);
        horizontalWallRender = new Renderer(_thickness, _thickness, 0.2f);
        passRender = new Renderer(_thickness / (_thickness * 2) - 1.0f, _thickness, _thickness);
        escapeRender = new Renderer(_thickness / (_thickness * 2) - 1.0f, _thickness, _thickness);
        nonEscapeRender = new Renderer(_thickness / (_thickness * 2) - 1.0f, _thickness, _thickness);
    }

    void generateMaze() {
        std::srand(time(nullptr));

        randomMaze();
        calculateWallPositions();
    }

    void randomMaze() {
        squaresMap();

        int randomX = std::rand() % static_cast<int>(_width);
        int randomY = std::rand() % static_cast<int>(_height);
        m_pair root = nodesMap[makeNodeKey(std::make_pair(randomX, randomY))];
        nodesMap.erase(makeNodeKey(std::make_pair(randomX, randomY)));

        v_pair frontier;
        frontier.push_back(std::make_pair(root.first, root.second));

        while (!nodesMap.empty()) {
            m_pair node = frontier.back();
            frontier.pop_back();

            v_pair temp = neighbors4(node);
            v_pair nbrs = intersectionMap(temp);

            if (!nbrs.empty()) {
                int random = std::rand() % nbrs.size();
                m_pair nbr = nbrs[random];

                v_pair edge = edgePair(node, nbr);
                mazeMap[makeKey(edge)] = edge;

                if (nodesMap.find(makeNodeKey(std::make_pair(nbr.first, nbr.second))) != nodesMap.end()) {
                    nodesMap.erase(makeNodeKey(std::make_pair(nbr.first, nbr.second)));
                }

                frontier.push_back(node);
                frontier.push_back(nbr);
            }
        }

        v_pair exitUp = edgePair(m_pair(_width - 1, _height - 1), m_pair(_width - 1, _height));
        mazeMap[makeKey(exitUp)] = exitUp;
    }

    void calculateWallPositions() {
        float t = _thickness * 2;
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                if (x == 0) {
                    verticalWallPosition.emplace_back(x * t - t / 2, 0.0f, t * (_height - 1) - y * t);
                    verticalCount++;
                }
                if ((y == 0) && (x != 0)) {
                    horizontalWallPosition.emplace_back(x * t, 0.0f, t * _height - (y * t + t / 2));
                    horizontalCount++;
                }
                if (findVerticalWall(x, y) != true) {
                    verticalWallPosition.emplace_back(x * t + t / 2, 0.0f, t * (_height - 1) - y * t);
                    verticalCount++;
                }
                if (findHorizontalWall(x, y) != true) {
                    horizontalWallPosition.emplace_back(x * t, 0.0f, t * (_height - 1) - (y * t + t / 2));
                    horizontalCount++;
                }
                passPosition.emplace_back(x * t, -t / 2.0f, y * t);

                // r, d, l, u
                mazeMapTree[makeNodeKey(std::make_pair(x, y))] = std::make_tuple(x, y, findVerticalWall(x, y), findHorizontalWall(x, y), findVerticalWall(x - 1, y), findHorizontalWall(x, y - 1));
            }
        }

        LOGGER(info, "total blocks: " << verticalCount + horizontalCount + _width * _height);

        verticalWallRender->setCoordinates(verticalCount, verticalWallPosition);
        horizontalWallRender->setCoordinates(horizontalCount, horizontalWallPosition);
        passRender->setCoordinates(_width * _height, passPosition);

        passPosition.clear();
    }

    void solveMaze() {
        visitedTable = std::vector<std::vector<bool>>(_height, std::vector<bool>(_width, false));

        int posX = 0, posY = 0;
        while (!((posX == _width - 1) && (posY == _height - 1))) {
            std::string nodeKey = makeNodeKey(std::make_pair(posX, posY));
            bool right = std::get<2>(mazeMapTree[nodeKey]);  // neighbors of the cell
            bool down = std::get<3>(mazeMapTree[nodeKey]);   // which way is open?
            bool left = std::get<4>(mazeMapTree[nodeKey]);
            bool up = std::get<5>(mazeMapTree[nodeKey]);

            bool cond1 = (right + left + down + up == 1);            // dead-end
            bool cond2 = (right + left + down + up == 0);            // dead-end, second encounter
            if ((cond1 || cond2) && ((posX != 0) || (posY != 0))) {  // dead-end, removing by stepping back
                passCount--;
                visitedTable[posY][posX] = false;
                if (right) {
                    posX++;                                                     // step back
                    std::string key = makeNodeKey(std::make_pair(posX, posY));  // find neighbors of new cell
                    std::get<4>(mazeMapTree[key]) = false;                      // assign as no way to get old positions
                } else if (down) {
                    posY++;
                    std::string key = makeNodeKey(std::make_pair(posX, posY));
                    std::get<5>(mazeMapTree[key]) = false;
                } else if (left) {
                    posX--;
                    std::string key = makeNodeKey(std::make_pair(posX, posY));
                    std::get<2>(mazeMapTree[key]) = false;
                } else if (up) {
                    posY--;
                    std::string key = makeNodeKey(std::make_pair(posX, posY));
                    std::get<3>(mazeMapTree[key]) = false;
                }
            } else {
                if ((right) && (!visitedTable[posY][posX + 1])) {
                    visitedTable[posY][posX + 1] = true;
                    posX++;
                    passCount++;
                } else if ((down) && (!visitedTable[posY + 1][posX])) {
                    visitedTable[posY + 1][posX] = true;
                    posY++;
                    passCount++;
                } else if ((left) && (!visitedTable[posY][posX - 1])) {
                    visitedTable[posY][posX - 1] = true;
                    posX--;
                    passCount++;
                } else if ((up) && (!visitedTable[posY - 1][posX])) {
                    visitedTable[posY - 1][posX] = true;
                    posY--;
                    passCount++;
                }
            }
        }
        visitedTable[0][0] = true;                     // entrance
        visitedTable[_height - 1][_width - 1] = true;  // exit
        passCount++;

        mazeMapTree.clear();
        nodesMap.clear();
        mazeMap.clear();

        findEscapeBlocks();
    }

    void findEscapeBlocks() {
        float t = _thickness * 2;
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                if (visitedTable[y][x] == false) {
                    nonEscapePosition.emplace_back(x * t, -t / 2.0f, t * (_height - 1) - y * t);
                } else {
                    escapePosition.emplace_back(x * t, -t / 2.0f, t * (_height - 1) - y * t);
                }
            }
        }

        escapeRender->setCoordinates(passCount, escapePosition);
        nonEscapeRender->setCoordinates(_width * _height - passCount, nonEscapePosition);

        visitedTable.clear();
        escapePosition.clear();
        nonEscapePosition.clear();
    }

    void squaresMap() {
        for (int x = 0; x < _width; x++) {
            for (int y = 0; y < _height; y++) {
                m_pair temp(x, y);
                nodesMap[makeNodeKey(temp)] = temp;
            }
        }
    }

    bool findVerticalWall(int x, int y) {
        v_pair search1 = edgePair(m_pair(x, y), m_pair(x + 1, y));
        v_pair search2 = edgePair(m_pair(x + 1, y), m_pair(x, y));

        std::string key = makeKey(search1);
        if (mazeMap.find(key) != mazeMap.end()) {
            return true;
        }
        key = makeKey(search2);
        if (mazeMap.find(key) != mazeMap.end()) {
            return true;
        }
        return false;
    }

    bool findHorizontalWall(int x, int y) {
        v_pair search1 = edgePair(m_pair(x, y), m_pair(x, y + 1));
        v_pair search2 = edgePair(m_pair(x, y + 1), m_pair(x, y));

        std::string key = makeKey(search1);
        if (mazeMap.find(key) != mazeMap.end()) {
            return true;
        }
        key = makeKey(search2);
        if (mazeMap.find(key) != mazeMap.end()) {
            return true;
        }
        return false;
    }

    void printMazeToConsole() {
        std::string lin = "--", bar = "|", sp = "  ", dot = "o";
        std::string temp = dot + sp;
        for (int x = 0; x < _width - 1; x++) {
            temp += dot + lin;
        }
        LOGGER(info, temp + dot);

        for (int y = 0; y < _height; y++) {
            std::string temp1{};
            std::string temp2{};
            for (int x = 0; x < _width; x++) {
                temp1 += sp + ((findVerticalWall(x, y) == true) ? " " : "|");
                temp2 += ((findHorizontalWall(x, y) == true) ? "  " : "--") + dot;
            }
            LOGGER(info, bar << temp1);
            LOGGER(info, dot << temp2);
        }
    }

    std::string makeNodeKey(m_pair pair) {
        std::string key = std::to_string(pair.first) + ", " + std::to_string(pair.second);

        return key;
    }

    std::string makeKey(v_pair edge) {
        std::string key = std::to_string(edge[0].first) + " " + std::to_string(edge[0].second) + " " +
            std::to_string(edge[1].first) + " " + std::to_string(edge[1].second);
        return key;
    }

    v_pair neighbors4(m_pair square) {
        v_pair neighbors;
        m_pair temp1(square.first + 1, square.second);
        m_pair temp2(square.first, square.second + 1);
        m_pair temp3(square.first, square.second - 1);
        m_pair temp4(square.first - 1, square.second);
        neighbors.push_back(temp1);
        neighbors.push_back(temp2);
        neighbors.push_back(temp3);
        neighbors.push_back(temp4);

        return neighbors;
    }

    v_pair intersectionMap(v_pair& v) {
        v_pair inter;
        for (const auto& it : v) {
            if (nodesMap.find(makeNodeKey(std::make_pair(it.first, it.second))) != nodesMap.end()) {
                inter.push_back(std::make_pair(it.first, it.second));
            }
        }

        return inter;
    }

    v_pair edgePair(m_pair node1, m_pair node2) {
        v_pair edge;
        edge.push_back(node1);
        edge.push_back(node2);

        return edge;
    }

    Texture* passTexture = nullptr;
    Texture* wallTexture = nullptr;
    Texture* escapeTexture = nullptr;
    Texture* nonEscapeTexture = nullptr;
    Texture* wallVerticalTexture = nullptr;

    Renderer* passRender = nullptr;
    Renderer* escapeRender = nullptr;
    Renderer* nonEscapeRender = nullptr;
    Renderer* verticalWallRender = nullptr;
    Renderer* horizontalWallRender = nullptr;

    float _width = 0.0f, _height = 0.0f, _thickness = 0.0f;
    int passCount = 0, verticalCount = 0, horizontalCount = 0;

    CameraMaze* _cameraMaze = nullptr;

    std::vector<glm::vec3> passPosition;
    std::vector<glm::vec3> escapePosition;
    std::vector<glm::vec3> nonEscapePosition;
    std::vector<glm::vec3> verticalWallPosition;
    std::vector<glm::vec3> horizontalWallPosition;

    std::vector<std::vector<bool>> visitedTable;
    std::unordered_map<std::string, v_pair> mazeMap;
    std::unordered_map<std::string, m_pair> nodesMap;
    std::unordered_map<std::string, std::tuple<int, int, bool, bool, bool, bool>> mazeMapTree;

};
