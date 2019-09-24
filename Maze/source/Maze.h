#pragma once

#include <vector> 
#include <iterator> 
#include <algorithm>
#include <random>
#include <time.h>
#include <tuple>
#include <iostream>

#include "glm/glm.hpp"

#include "textures/TextureManager.h"
#include "Renderer.h"

struct Node {
	int x;
	int y;
	struct Node *right;
	struct Node *down;
	struct Node *left;
	struct Node *up;
};

class newMaze {
private:
	TextureManager *passTexture, *wallTexture, *wallRerticalTexture, *escapeTexture, *nonEscapeTexture;
	Renderer *verticalWallRender, *horizontalWallRender, *passRender, *escapeRender, *nonEscapeRender;
	
	std::vector<glm::vec3> verticalWallPosition;
	std::vector<glm::vec3> horizontalWallPosition;
	std::vector<glm::vec3> passPosition;
	std::vector<glm::vec3> escapePosition;
	std::vector<glm::vec3> nonEscapePosition;

	int passCount, width, height, thickness, verticalCount, horizontalCount;

	std::vector<std::vector<std::pair<int, int>>> maze;
	std::vector<std::pair<int, int>> nodes;
	std::vector<std::vector<std::pair<int, int>>> tree;

	std::vector<std::pair<int, int>> visited;
	std::vector<std::vector<bool>> visitedTable;
	Node *newNode(int x, int y);
	Node *mazeTree(Node *node, int x, int y);
	std::vector<std::pair<int, int>> path;
	bool found = false;
	void DeepFirstSearch(struct Node *root);
	void removeDeadEnd();
	void escapeBlocks();

	std::vector<std::vector<std::tuple<int, int, bool, bool, bool, bool>>> mazeStr;
	std::vector<std::tuple<int, int, bool, bool, bool, bool>> tempStr;
	std::tuple<int, int, bool, bool, bool, bool> str;

	std::vector<std::pair<int, int>> squares(int width, int height);
	std::vector<std::pair<int, int>> neighbors4(std::pair<int, int> square);
	std::vector<std::pair<int, int>> intersection(std::vector<std::pair<int, int>> v1, std::vector<std::pair<int, int>> v2);
	std::vector<std::pair<int, int>> Edge(std::pair<int, int> node1, std::pair<int, int> node2);
	std::vector<std::vector<std::pair<int, int>>> randomTree(std::vector<std::pair<int, int>> nodes, int width, int height);
	std::vector<std::vector<std::tuple<int, int, bool, bool, bool, bool>>> positions();

public:
	newMaze(int thickness, int width, int height);
	bool findVerticalWall(int x, int y);
	bool findHorizontalWall(int x, int y);
	void printMazeToConsole();
	void display(bool escaping);
	int getVerticalCount() { return verticalCount; }
	int getHorizontalCount() { return horizontalCount; }
	std::vector<glm::vec3> getVerticalWallPosition() { return verticalWallPosition; }
	std::vector<glm::vec3> getHorizontalWallPosition() { return horizontalWallPosition; }
	std::vector<std::pair<int, int>> getExitPath() { return path; }
};

