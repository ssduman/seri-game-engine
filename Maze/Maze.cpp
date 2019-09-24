#include "Maze.h"

newMaze::newMaze(int thickness, int width, int height) {
	wallTexture = new TextureManager("textures/wall.png");
	wallRerticalTexture = new TextureManager("textures/wallParallel.png");
	passTexture = new TextureManager("textures/pass.png");
	escapeTexture = new TextureManager("textures/escape.png");
	nonEscapeTexture = new TextureManager("textures/pass.png");

	verticalWallRender = new Renderer(thickness, 0.2f, thickness);
	horizontalWallRender = new Renderer(thickness, thickness, 0.2f);
	passRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
	escapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);
	nonEscapeRender = new Renderer(thickness / (thickness * 2) - 1.0f, thickness, thickness);

	this->width = width;
	this->height = height;
	this->thickness = thickness;

	nodes = squares(width, height);
	maze = randomTree(nodes, width, height);
	
	mazeStr = positions();
	//printMazeToConsole();
	
	std::vector<std::vector<bool>> table(height, std::vector<bool>(width, false));
	visitedTable = table;

	struct Node *root = newNode(0, 0);
	root = mazeTree(root, 0, 0);

	std::vector<std::vector<bool>> tableNew(height, std::vector<bool>(width, false));
	visitedTable = tableNew;

	DeepFirstSearch(root);
	removeDeadEnd();

	std::pair<int, int> exitDown(width - 1, height - 1);
	path.push_back(exitDown);

	std::vector<std::vector<bool>> tableEscape(height, std::vector<bool>(width, false));
	visitedTable = tableEscape;

	escapeBlocks();
}

void newMaze::escapeBlocks() {
	int escapeCount = path.size();
	float t = thickness * 2;
	std::vector<std::pair<int, int>>::iterator it = path.begin();
	for (int x = 0; it != path.end(); it++) {
		visitedTable[it->first][it->second] = true;
		escapePosition.push_back(glm::vec3(it->first * t, t / 2.0f, t * (height - 1) - it->second * t));
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (visitedTable[x][y] == false) {
				nonEscapePosition.push_back(glm::vec3(x * t, t / 2.0f, t * (height - 1) - y * t));
			}
		}
	}

	escapeRender->init(escapeCount, escapePosition);
	nonEscapeRender->init(width * height - escapeCount, nonEscapePosition);
}

struct Node *newMaze::newNode(int x, int y) {
	struct Node *node = new Node();
	node->x = x;
	node->y = y;
	node->right = NULL;
	node->down = NULL;
	node->left = NULL;
	node->up = NULL;

	return node;
}

struct Node *newMaze::mazeTree(struct Node *root, int posX, int posY) {
	bool right = std::get<2>(mazeStr[posY][posX]);
	bool left = std::get<3>(mazeStr[posY][posX]);
	bool down = std::get<4>(mazeStr[posY][posX]);
	bool up = std::get<5>(mazeStr[posY][posX]);

	if ((posX == width - 1) && (posY == height - 1)) {
		return root;
	}
	else if ((posX < width) && (posY < height)) {
		if ((right) && (!visitedTable[posY][posX + 1])) {
			visitedTable[posY][posX + 1] = true;
			root->right = newNode(posX + 1, posY);
			mazeTree(root->right, posX + 1, posY);
		}

		if ((down) && (!visitedTable[posY + 1][posX])) {
			visitedTable[posY + 1][posX] = true;
			root->down = newNode(posX, posY + 1);
			mazeTree(root->down, posX, posY + 1);
		}

		if ((left) && (!visitedTable[posY][posX - 1])) {
			visitedTable[posY][posX - 1] = true;
			root->left = newNode(posX - 1, posY);
			mazeTree(root->left, posX - 1, posY);
		}

		if ((up) && (!visitedTable[posY - 1][posX])) {
			visitedTable[posY - 1][posX] = true;
			root->up = newNode(posX, posY - 1);
			mazeTree(root->up, posX, posY - 1);
		}
	}

	return root;
}

void newMaze::DeepFirstSearch(struct Node *root) {
	if (root == NULL) {
		return;
	}

	if ((root->x == width - 1) && (root->y == height - 1)) {
		found = true;
		return;
	}
	else {
		bool right = std::get<2>(mazeStr[root->y][root->x]);
		bool left = std::get<3>(mazeStr[root->y][root->x]);
		bool down = std::get<4>(mazeStr[root->y][root->x]);
		bool up = std::get<5>(mazeStr[root->y][root->x]);

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				std::pair<int, int> add(root->x, root->y);
				path.push_back(add);
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->right);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				std::pair<int, int> add(root->x, root->y);
				path.push_back(add);
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->down);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				std::pair<int, int> add(root->x, root->y);
				path.push_back(add);
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->left);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				std::pair<int, int> add(root->x, root->y);
				path.push_back(add);
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->up);
		}
	}
}

void newMaze::removeDeadEnd() {
	std::vector<std::pair<int, int>>::iterator it = path.begin();
	for (int x = 0; it != path.end() - 1; it++) {
		bool right = std::get<2>(mazeStr[it->second][it->first]);
		bool left = std::get<3>(mazeStr[it->second][it->first]);
		bool down = std::get<4>(mazeStr[it->second][it->first]);
		bool up = std::get<5>(mazeStr[it->second][it->first]);
		bool del = true;

		if (right) {
			if ((it->first + 1 == (it + 1)->first) && (it->second == (it + 1)->second)) {
				del = false;
			}
		}
		if (left) {
			if ((it->first - 1 == (it + 1)->first) && (it->second == (it + 1)->second)) {
				del = false;
			}
		}
		if (down) {
			if ((it->first == (it + 1)->first) && (it->second + 1 == (it + 1)->second)) {
				del = false;
			}
		}
		if (up) {
			if ((it->first == (it + 1)->first) && (it->second - 1 == (it + 1)->second)) {
				del = false;
			}
		}
		if (del == true) {
			path.erase(it);
			it = path.begin();
		}

	}
}

std::vector<std::pair<int, int>> newMaze::squares(int width, int height) {
	std::vector<std::pair<int, int>> nodes;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			std::pair<int, char> temp(x, y);
			nodes.push_back(temp);
		}
	}

	return nodes;
}

std::vector<std::pair<int, int>> newMaze::neighbors4(std::pair<int, int> square) {
	std::vector<std::pair<int, int>> neighbors;
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

std::vector<std::pair<int, int>> newMaze::intersection(std::vector<std::pair<int, int>> v1, std::vector<std::pair<int, int>> v2) {
	std::vector<std::pair<int, int>> intersection;

	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());

	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(intersection));

	return intersection;
}

std::vector<std::pair<int, int>> newMaze::Edge(std::pair<int, int> node1, std::pair<int, int> node2) {
	std::vector<std::pair<int, int>> edge;
	edge.push_back(node1);
	edge.push_back(node2);

	return edge;
}

std::vector<std::vector<std::pair<int, int>>> newMaze::randomTree(std::vector<std::pair<int, int>> nodes, int width, int height) {
	std::srand(time(NULL));

	std::vector<std::pair<int, int>>::iterator itNodes;
	itNodes = nodes.begin();
	int random = std::rand() % (width * height);
	for (int x = 0; x < random; x++) {
		itNodes++;
	}
	std::pair<int, int> root = *itNodes;
	nodes.erase(itNodes);

	//std::pair<int, int> root = nodes.back();
	//nodes.pop_back();

	std::vector<std::pair<int, int>> frontier;
	std::pair<int, int> temp(root.first, root.second);
	frontier.push_back(temp);

	while (!nodes.empty()) {
		std::pair<int, int> node = frontier.back();

		frontier.pop_back();

		std::vector<std::pair<int, int>> temp = neighbors4(node);
		std::vector<std::pair<int, int>> nbrs = intersection(temp, nodes);

		if (!nbrs.empty()) {
			int random = std::rand() % nbrs.size();
			std::pair<int, int> nbr = nbrs[random];
			tree.push_back(Edge(node, nbr));

			std::vector<std::pair<int, int>>::iterator position = std::find(nodes.begin(), nodes.end(), nbr);
			if (position != nodes.end()) {
				nodes.erase(position);
			}

			frontier.push_back(node);
			frontier.push_back(nbr);
		}
	}

	std::vector<std::pair<int, int>> exitUp = Edge(std::pair<int, int>(width - 1, height - 1), std::pair<int, int>(width - 1, height));
	tree.push_back(exitUp);
	return tree;
}

void newMaze::display(bool escaping) {
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
	}
	else {
		escapeTexture->bind();
		escapeRender->render();
		escapeTexture->unbind();

		nonEscapeTexture->bind();
		nonEscapeRender->render();
		nonEscapeTexture->unbind();
	}
}

bool newMaze::findVerticalWall(int x, int y) {
	std::vector<std::vector<std::pair<int, int>>>::iterator it1;
	std::vector<std::vector<std::pair<int, int>>>::iterator it2;

	std::vector<std::pair<int, int>> search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x + 1, y));
	std::vector<std::pair<int, int>> search2 = Edge(std::pair<int, int>(x + 1, y), std::pair<int, int>(x, y));
	it1 = std::find(maze.begin(), maze.end(), search1);
	it2 = std::find(maze.begin(), maze.end(), search2);
	if (it1 != maze.end()) {	// true means no wall
		return true;
	}
	if (it2 != maze.end()) {
		return true;
	}
	return false;
}

bool newMaze::findHorizontalWall(int x, int y) {
	std::vector<std::vector<std::pair<int, int>>>::iterator it1;
	std::vector<std::vector<std::pair<int, int>>>::iterator it2;

	std::vector<std::pair<int, int>> search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x, y + 1));
	std::vector<std::pair<int, int>> search2 = Edge(std::pair<int, int>(x, y + 1), std::pair<int, int>(x, y));
	it1 = std::find(maze.begin(), maze.end(), search1);
	it2 = std::find(maze.begin(), maze.end(), search2);

	if (it1 != maze.end()) {
		return true;
	}
	if (it2 != maze.end()) {
		return true;
	}
	return false;
}

void newMaze::printMazeToConsole() {
	std::string lin = "--", bar = "|", sp = "  ", dot = "o";
	std::string temp = dot + sp;
	for (int x = 0; x < width - 1; x++) {
		temp += dot + lin;
	}
	std::cout << temp + dot << std::endl;

	for (int y = 0; y < height; y++) {
		temp = "";
		for (int x = 0; x < width; x++) {
			if (findVerticalWall(x, y) == true) {
				temp += sp + " ";
			}
			else {
				temp += sp + "|";
			}
		}
		std::cout << bar << temp << std::endl;

		temp = "";
		for (int x = 0; x < width; x++) {
			if (findHorizontalWall(x, y) == true) {
				temp += "  " + dot;
			}
			else {
				temp += "--" + dot;
			}
		}
		std::cout << dot << temp << std::endl;
	}
}

std::vector<std::vector<std::tuple<int, int, bool, bool, bool, bool>>> newMaze::positions() {

	std::vector<std::vector<std::tuple<int, int, bool, bool, bool, bool>>> mazeStr;
	std::vector<std::tuple<int, int, bool, bool, bool, bool>> tempStr;
	std::tuple<int, int, bool, bool, bool, bool> str;

	float t = thickness * 2;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x == 0) {
				verticalWallPosition.push_back(glm::vec3(x * t - t / 2, 0.0f, t * (height - 1) - y * t));
				verticalCount++;
			}
			if ((y == 0) && (x != 0)) {
				horizontalWallPosition.push_back(glm::vec3(x * t, 0.0f, t * height - (y * t + t / 2)));
				horizontalCount++;
			}
			if (findVerticalWall(x, y) != true) {
				verticalWallPosition.push_back(glm::vec3(x * t + t / 2, 0.0f, t * (height - 1) - y * t));
				verticalCount++; 
			}
			if (findHorizontalWall(x, y) != true) {
				horizontalWallPosition.push_back(glm::vec3(x * t, 0.0f, t * (height - 1) - (y * t + t / 2)));
				horizontalCount++;
			}
			passPosition.push_back(glm::vec3(x * t, t / 2.0f, y * t));

			// r, l, d, u
			str = std::make_tuple(x, y, findVerticalWall(x, y), findVerticalWall(x - 1, y), 
									findHorizontalWall(x, y), findHorizontalWall(x, y - 1));
			tempStr.push_back(str);
		}
		mazeStr.push_back(tempStr);
		tempStr.clear();
	}

	std::cout << "ver: " << verticalCount << "hor: " << horizontalCount << std::endl;

	verticalWallRender->init(verticalCount, verticalWallPosition);
	horizontalWallRender->init(horizontalCount, horizontalWallPosition);
	passRender->init(width * height, passPosition);

	return mazeStr;
}
