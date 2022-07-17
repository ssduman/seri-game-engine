#pragma once

#include <vector> 
#include <iterator> 
#include <algorithm>
#include <random>
#include <time.h>
#include <unordered_map>
#include <tuple>
#include <string>
#include <iostream>

#include "glm/glm.hpp"

#include "textures/TextureManager.h"
#include "Renderer.h"

class Maze {
private:
    TextureManager* passTexture, * wallTexture, * wallRerticalTexture, * escapeTexture, * nonEscapeTexture;
    Renderer* verticalWallRender, * horizontalWallRender, * passRender, * escapeRender, * nonEscapeRender;

    std::vector<glm::vec3> verticalWallPosition;
    std::vector<glm::vec3> horizontalWallPosition;
    std::vector<glm::vec3> passPosition;
    std::vector<glm::vec3> escapePosition;
    std::vector<glm::vec3> nonEscapePosition;

    int passCount = 0, width, height, thickness, verticalCount, horizontalCount;

    std::unordered_map<std::string, std::vector<std::pair<int, int>>> mazeMap;
    std::unordered_map<std::string, std::pair<int, int>> nodesMap;
    std::unordered_map<std::string, std::tuple<int, int, bool, bool, bool, bool>> mazeMapTree;

    std::vector<std::vector<bool>> visitedTable;
    void solveMaze();
    void escapeBlocks();

    void squaresMap();
    std::vector<std::pair<int, int>> neighbors4(std::pair<int, int> square);
    std::vector<std::pair<int, int>> intersectionMap(std::vector<std::pair<int, int>> v);
    std::vector<std::pair<int, int>> Edge(std::pair<int, int> node1, std::pair<int, int> node2);
    void randomMaze();
    void positions();
    std::string makeNodeKey(std::pair<int, int> pair);
    std::string makeKey(std::vector<std::pair<int, int>> edge);

public:
    Maze(int thickness, int width, int height);
    bool findVerticalWall(int x, int y);
    bool findHorizontalWall(int x, int y);
    void printMazeToConsole();
    void display(bool escaping);
    std::vector<glm::vec3> getVerticalWallPosition() { return verticalWallPosition; }
    std::vector<glm::vec3> getHorizontalWallPosition() { return horizontalWallPosition; }
};

