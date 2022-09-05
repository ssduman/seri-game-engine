#pragma once

#include "Renderer.h"
#include "Texture.h"

#include <glm/glm.hpp>

#include <tuple>
#include <vector> 
#include <random>
#include <time.h>
#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <unordered_map>

typedef std::pair<int, int> m_pair;
typedef std::vector<m_pair> v_pair;

class Maze {
public:
    Maze(float thickness, float width, float height) {
        wallTexture = new Texture("textures/wall1.png");
        wallRerticalTexture = new Texture("textures/wall2.png");
        passTexture = new Texture("textures/passage.png");
        escapeTexture = new Texture("textures/escape.png");
        nonEscapeTexture = new Texture("textures/nonescape.png");

        verticalWallRender = new Renderer(thickness, 0.2f, thickness);
        horizontalWallRender = new Renderer(thickness, thickness, 0.2f);
        passRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
        escapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
        nonEscapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);

        // cubes will be 10.0f x 10.0f x 10.0f
        _width = width;
        _height = height;
        _thickness = thickness;

        std::srand(time(NULL));

        randomMaze();
        positions();
        //printMazeToConsole();

        std::vector<std::vector<bool>> table(height, std::vector<bool>(width, false));
        visitedTable = table;

        solveMaze();
        escapeBlocks();
    }

    bool findVerticalWall(int x, int y) {
        v_pair search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x + 1, y));
        v_pair search2 = Edge(std::pair<int, int>(x + 1, y), std::pair<int, int>(x, y));

        std::string key;
        key = makeKey(search1);
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
        v_pair search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x, y + 1));
        v_pair search2 = Edge(std::pair<int, int>(x, y + 1), std::pair<int, int>(x, y));

        std::string key;
        key = makeKey(search1);
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
        std::cout << temp + dot << std::endl;

        for (int y = 0; y < _height; y++) {
            std::string temp1 = "";
            std::string temp2 = "";
            for (int x = 0; x < _width; x++) {
                temp1 += sp + ((findVerticalWall(x, y) == true) ? " " : "|");
                temp2 += ((findHorizontalWall(x, y) == true) ? "  " : "--") + dot;
            }
            std::cout << bar << temp1 << std::endl;
            std::cout << dot << temp2 << std::endl;
        }
    }

    void display(bool escaping) {
        wallRerticalTexture->bind();
        verticalWallRender->render();
        wallRerticalTexture->unbind();

        wallTexture->bind();
        horizontalWallRender->render();
        wallTexture->unbind();

        if (escaping == false) {
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

    void resetMaze(float thickness, float width, float height) {
        verticalWallPosition.clear();
        horizontalWallPosition.clear();
        passPosition.clear();
        escapePosition.clear();
        nonEscapePosition.clear();
        mazeMap.clear();
        nodesMap.clear();
        mazeMapTree.clear();
        visitedTable.clear();

        _width = width;
        _height = height;
        _thickness = thickness;

        delete wallTexture;
        delete wallRerticalTexture;
        delete passTexture;
        delete escapeTexture;
        delete nonEscapeTexture;

        wallTexture = new Texture("textures/wall1.png");
        wallRerticalTexture = new Texture("textures/wall2.png");
        passTexture = new Texture("textures/passage.png");
        escapeTexture = new Texture("textures/escape.png");
        nonEscapeTexture = new Texture("textures/nonescape.png");

        delete verticalWallRender;
        delete horizontalWallRender;
        delete passRender;
        delete escapeRender;
        delete nonEscapeRender;

        verticalWallRender = new Renderer(thickness, 0.2f, thickness);
        horizontalWallRender = new Renderer(thickness, thickness, 0.2f);
        passRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
        escapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
        nonEscapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);

        std::srand(time(NULL));

        randomMaze();
        positions();

        std::vector<std::vector<bool>> table(height, std::vector<bool>(width, false));
        visitedTable = table;

        solveMaze();
        escapeBlocks();
    }

    std::vector<glm::vec3> getVerticalWallPosition() {
        return verticalWallPosition;
    }

    std::vector<glm::vec3> getHorizontalWallPosition() {
        return horizontalWallPosition;
    }

private:
    void solveMaze() {
        int posX = 0, posY = 0;
        while (!((posX == _width - 1) && (posY == _height - 1))) {
            std::string nodeKey = makeNodeKey(std::make_pair(posX, posY));
            bool right = std::get<2>(mazeMapTree[nodeKey]); // neighbors of the tile
            bool down = std::get<3>(mazeMapTree[nodeKey]);  // which way is open?
            bool left = std::get<4>(mazeMapTree[nodeKey]);
            bool up = std::get<5>(mazeMapTree[nodeKey]);

            bool cond1 = (right + left + down + up == 1); // dead-end
            bool cond2 = (right + left + down + up == 0); // dead-end, second encounter
            if ((cond1 || cond2) && ((posX != 0) || (posY != 0))) { // dead-end, removing by steping back
                passCount--;
                visitedTable[posY][posX] = false;
                if (right) {
                    posX++; // step back
                    std::string key = makeNodeKey(std::make_pair(posX, posY)); // find neighbors of new tile
                    std::get<4>(mazeMapTree[key]) = false; // assing as no way to get old positions
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
        visitedTable[0][0] = true; // enterence
        visitedTable[_height - 1][_width - 1] = true; // exit
        passCount++;

        mazeMapTree.clear();
        nodesMap.clear();
        mazeMap.clear();
    }

    void escapeBlocks() {
        float t = _thickness * 2;
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                if (visitedTable[y][x] == false) {
                    nonEscapePosition.push_back(glm::vec3(x * t, t / 2.0f, t * (_height - 1) - y * t));
                } else { // this is the way of exit
                    escapePosition.push_back(glm::vec3(x * t, t / 2.0f, t * (_height - 1) - y * t));
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
                std::pair<int, int> temp(x, y);
                nodesMap[makeNodeKey(temp)] = temp;
            }
        }
    }

    v_pair neighbors4(std::pair<int, int> square) {
        v_pair neighbors;
        std::pair<int, int> temp1(square.first + 1, square.second);
        std::pair<int, int> temp2(square.first, square.second + 1);
        std::pair<int, int> temp3(square.first, square.second - 1);
        std::pair<int, int> temp4(square.first - 1, square.second);
        neighbors.push_back(temp1);
        neighbors.push_back(temp2);
        neighbors.push_back(temp3);
        neighbors.push_back(temp4);

        return neighbors;
    }

    v_pair intersectionMap(v_pair v) {
        v_pair inter;
        v_pair::iterator it;
        for (it = v.begin(); it != v.end(); it++) {
            if (nodesMap.find(makeNodeKey(std::make_pair(it->first, it->second))) != nodesMap.end()) {
                inter.push_back(std::make_pair(it->first, it->second));
            }
        }

        return inter;
    }

    v_pair Edge(std::pair<int, int> node1, std::pair<int, int> node2) {
        v_pair edge;
        edge.push_back(node1);
        edge.push_back(node2);

        return edge;
    }

    void randomMaze() {
        squaresMap();

        int randomX = std::rand() % (int)_width;
        int randomY = std::rand() % (int)_height;
        std::pair<int, int> root = nodesMap[makeNodeKey(std::make_pair(randomX, randomY))];
        nodesMap.erase(makeNodeKey(std::make_pair(randomX, randomY)));

        v_pair frontier;
        frontier.push_back(std::make_pair(root.first, root.second));

        while (!nodesMap.empty()) {
            std::pair<int, int> node = frontier.back();
            frontier.pop_back();

            v_pair temp = neighbors4(node);
            v_pair nbrs = intersectionMap(temp);

            if (!nbrs.empty()) {
                int random = std::rand() % nbrs.size();
                std::pair<int, int> nbr = nbrs[random];

                v_pair edge = Edge(node, nbr);
                mazeMap[makeKey(edge)] = edge;

                if (nodesMap.find(makeNodeKey(std::make_pair(nbr.first, nbr.second))) != nodesMap.end()) {
                    nodesMap.erase(makeNodeKey(std::make_pair(nbr.first, nbr.second)));
                }

                frontier.push_back(node);
                frontier.push_back(nbr);
            }
        }

        v_pair exitUp = Edge(std::pair<int, int>(_width - 1, _height - 1), std::pair<int, int>(_width - 1, _height));
        mazeMap[makeKey(exitUp)] = exitUp;
    }

    void positions() {
        std::tuple<int, int, bool, bool, bool, bool> tempTuple;

        float t = _thickness * 2;
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                if (x == 0) {
                    verticalWallPosition.push_back(glm::vec3(x * t - t / 2, 0.0f, t * (_height - 1) - y * t));
                    verticalCount++;
                }
                if ((y == 0) && (x != 0)) {
                    horizontalWallPosition.push_back(glm::vec3(x * t, 0.0f, t * _height - (y * t + t / 2)));
                    horizontalCount++;
                }
                if (findVerticalWall(x, y) != true) {
                    verticalWallPosition.push_back(glm::vec3(x * t + t / 2, 0.0f, t * (_height - 1) - y * t));
                    verticalCount++;
                }
                if (findHorizontalWall(x, y) != true) {
                    horizontalWallPosition.push_back(glm::vec3(x * t, 0.0f, t * (_height - 1) - (y * t + t / 2)));
                    horizontalCount++;
                }
                passPosition.push_back(glm::vec3(x * t, t / 2.0f, y * t));

                // r, d, l, u
                tempTuple = std::make_tuple(x, y, findVerticalWall(x, y), findHorizontalWall(x, y),
                    findVerticalWall(x - 1, y), findHorizontalWall(x, y - 1));
                mazeMapTree[makeNodeKey(std::make_pair(x, y))] = tempTuple;
            }
        }

        std::cout << "total blocks: " << verticalCount + horizontalCount + _width * _height << std::endl;

        verticalWallRender->setCoordinates(verticalCount, verticalWallPosition);
        horizontalWallRender->setCoordinates(horizontalCount, horizontalWallPosition);
        passRender->setCoordinates(_width * _height, passPosition);

        passPosition.clear();
    }

    std::string makeNodeKey(std::pair<int, int> pair) {
        std::string key = std::to_string(pair.first) + ", " + std::to_string(pair.second);

        return key;
    }

    std::string makeKey(v_pair edge) {
        std::string key = std::to_string(edge[0].first) + " " + std::to_string(edge[0].second) + " " +
            std::to_string(edge[1].first) + " " + std::to_string(edge[1].second);
        return key;
    }

    Texture* passTexture, * wallTexture, * wallRerticalTexture, * escapeTexture, * nonEscapeTexture;
    Renderer* verticalWallRender, * horizontalWallRender, * passRender, * escapeRender, * nonEscapeRender;

    std::vector<glm::vec3> verticalWallPosition;
    std::vector<glm::vec3> horizontalWallPosition;
    std::vector<glm::vec3> passPosition;
    std::vector<glm::vec3> escapePosition;
    std::vector<glm::vec3> nonEscapePosition;

    float _width = 0.0f, _height = 0.0f, _thickness = 0.0f;
    int passCount = 0, verticalCount = 0, horizontalCount = 0;

    std::unordered_map<std::string, v_pair> mazeMap;
    std::unordered_map<std::string, std::pair<int, int>> nodesMap;
    std::unordered_map<std::string, std::tuple<int, int, bool, bool, bool, bool>> mazeMapTree;

    std::vector<std::vector<bool>> visitedTable;
};

