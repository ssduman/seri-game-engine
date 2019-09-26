#include "Maze.h"

Maze::Maze(int thickness, int width, int height) {
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

	std::srand(time(NULL));

	randomMaze();
	positions();
	//printMazeToConsole();

	std::vector<std::vector<bool>> table(height, std::vector<bool>(width, false));
	visitedTable = table;

	struct Node *root = newNode(0, 0);
	root = mazeTree(root, 0, 0);

	visitedTable = table;
	DeepFirstSearch(root);

	visitedTable = table;
	removeDeadEnd();
	escapeBlocks();
}

void Maze::escapeBlocks() {
	int escapeCount = path.size();
	float t = thickness * 2;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (visitedTable[y][x] == false) {
				nonEscapePosition.push_back(glm::vec3(x * t, t / 2.0f, t * (height - 1) - y * t));
			}
			else {
				escapePosition.push_back(glm::vec3(x * t, t / 2.0f, t * (height - 1) - y * t));
			}
		}
	}
	
	escapeRender->init(escapeCount, escapePosition);
	nonEscapeRender->init(width * height - escapeCount, nonEscapePosition);
}

struct Node *Maze::newNode(int x, int y) {
	struct Node *node = new Node();
	node->x = x;
	node->y = y;
	node->right = NULL;
	node->down = NULL;
	node->left = NULL;
	node->up = NULL;

	return node;
}

struct Node *Maze::mazeTree(struct Node *root, int posX, int posY) {
	bool right = std::get<2>(mazeMapTree[makeNodeKey(std::make_pair(posX, posY))]);
	bool left = std::get<3>(mazeMapTree[makeNodeKey(std::make_pair(posX, posY))]);
	bool down = std::get<4>(mazeMapTree[makeNodeKey(std::make_pair(posX, posY))]);
	bool up = std::get<5>(mazeMapTree[makeNodeKey(std::make_pair(posX, posY))]);

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

void Maze::DeepFirstSearch(struct Node *root) {
	/*
	(-t,-t)----------------------> +X
	  |
	  |		o  o--o--o--o--o
	  |		|            a |
	  |		o  o--o--o--o  o
	  |		|  |   b    |  |
	  |		o  o--o--o  o  o
	  |		|        |     |
	  |		o  o--o  o--o--o
	  |		|  |  |        |
	  |		o  o  o--o--o  o
	  |		|     |        |
	  |		o--o--o--o--o  o
	  |				  (camera)
	  +Y
	  a at (4,0), b at (1,2)
	  starting from (0,0), to reach exit (4,4) we should either go as far as right and as far as down
	  and left and up or as far as down and as far as right and left and up. 
	*/
	if (root == NULL) {
		return;
	}
	if ((root->x == width - 1) && (root->y == height - 1)) {
		found = true;
		path.push_back(std::make_pair(width - 1, height - 1));
		return;
	}
	else {
		bool right = std::get<2>(mazeMapTree[makeNodeKey(std::make_pair(root->y, root->x))]);
		bool left = std::get<3>(mazeMapTree[makeNodeKey(std::make_pair(root->y, root->x))]);
		bool down = std::get<4>(mazeMapTree[makeNodeKey(std::make_pair(root->y, root->x))]);
		bool up = std::get<5>(mazeMapTree[makeNodeKey(std::make_pair(root->y, root->x))]);

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				path.push_back(std::make_pair(root->x, root->y));
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->right);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				path.push_back(std::make_pair(root->x, root->y));
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->down);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				path.push_back(std::make_pair(root->x, root->y));
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->left);
		}

		if (found == false) {
			if (visitedTable[root->y][root->x] == false) {
				path.push_back(std::make_pair(root->x, root->y));
				visitedTable[root->y][root->x] = true;
			}
			DeepFirstSearch(root->up);
		}
	}
}

void Maze::removeDeadEnd() {
	/*
	if current location has no access to next location, delete current location and back to begining.
	if location has no access to next location, algorithm encountures a dead-end and jump back other
	possible routes. this function removes dead-ends
	*/
	std::vector<std::pair<int, int>>::iterator it = path.begin();
	for (int x = 0; it != path.end() - 1; it++) {
		visitedTable[it->second][it->first] = true;
		bool right = std::get<2>(mazeMapTree[makeNodeKey(std::make_pair(it->first, it->second))]);
		bool left = std::get<3>(mazeMapTree[makeNodeKey(std::make_pair(it->first, it->second))]);
		bool down = std::get<4>(mazeMapTree[makeNodeKey(std::make_pair(it->first, it->second))]);
		bool up = std::get<5>(mazeMapTree[makeNodeKey(std::make_pair(it->first, it->second))]);
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
			visitedTable[it->second][it->first] = false;
			path.erase(it);
			it = path.begin();
		}

	}
	visitedTable[it->second][it->first] = true;
}

void Maze::squaresMap() {
	int count = 0;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			std::pair<int, char> temp(x, y);
			nodesMap[makeNodeKey(temp)] = temp;
		}
	}
}

std::vector<std::pair<int, int>> Maze::neighbors4(std::pair<int, int> square) {
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

std::vector<std::pair<int, int>> Maze::intersectionMap(std::vector<std::pair<int, int>> v) {
	std::vector<std::pair<int, int>> inter;
	std::vector<std::pair<int, int>>::iterator it = v.begin();
	for (int x = 0; it != v.end(); it++) {
		if (nodesMap.find(makeNodeKey(std::make_pair(it->first, it->second))) != nodesMap.end()) {
			inter.push_back(std::make_pair(it->first, it->second));
		}
	}

	return inter;
}

std::vector<std::pair<int, int>> Maze::Edge(std::pair<int, int> node1, std::pair<int, int> node2) {
	std::vector<std::pair<int, int>> edge;
	edge.push_back(node1);
	edge.push_back(node2);

	return edge;
}

void Maze::randomMaze() {
	squaresMap();
	
	int randomX = std::rand() % width;
	int randomY = std::rand() % height;
	std::pair<int, int> root = nodesMap[makeNodeKey(std::make_pair(randomX, randomY))];
	nodesMap.erase(makeNodeKey(std::make_pair(randomX, randomY)));

	std::vector<std::pair<int, int>> frontier;
	frontier.push_back(std::make_pair(root.first, root.second));

	while (!nodesMap.empty()) {
		std::pair<int, int> node = frontier.back();
		frontier.pop_back();

		std::vector<std::pair<int, int>> temp = neighbors4(node);
		std::vector<std::pair<int, int>> nbrs = intersectionMap(temp);

		if (!nbrs.empty()) {
			int random = std::rand() % nbrs.size();
			std::pair<int, int> nbr = nbrs[random];

			std::vector<std::pair<int, int>> edge = Edge(node, nbr);
			mazeMap[makeKey(edge)] = edge;

			if (nodesMap.find(makeNodeKey(std::make_pair(nbr.first, nbr.second))) != nodesMap.end()) {
				nodesMap.erase(makeNodeKey(std::make_pair(nbr.first, nbr.second)));
			}

			frontier.push_back(node);
			frontier.push_back(nbr);
		}
	}

	std::vector<std::pair<int, int>> exitUp = Edge(std::pair<int, int>(width - 1, height - 1), std::pair<int, int>(width - 1, height));
	mazeMap[makeKey(exitUp)] = exitUp;
}

void Maze::display(bool escaping) {
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

std::string Maze::makeNodeKey(std::pair<int, char> pair) {
	std::string key = std::to_string(pair.first) + ", " + std::to_string(pair.second);

	return key;
}

std::string Maze::makeKey(std::vector<std::pair<int, int>> edge) {
	std::string key = std::to_string(edge[0].first) + " " + std::to_string(edge[0].second) + " " +
		std::to_string(edge[1].first) + " " + std::to_string(edge[1].second);
	return key;
}

bool Maze::findVerticalWall(int x, int y) {
	std::vector<std::pair<int, int>> search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x + 1, y));
	std::vector<std::pair<int, int>> search2 = Edge(std::pair<int, int>(x + 1, y), std::pair<int, int>(x, y));

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

bool Maze::findHorizontalWall(int x, int y) {
	std::vector<std::pair<int, int>> search1 = Edge(std::pair<int, int>(x, y), std::pair<int, int>(x, y + 1));
	std::vector<std::pair<int, int>> search2 = Edge(std::pair<int, int>(x, y + 1), std::pair<int, int>(x, y));

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

void Maze::printMazeToConsole() {
	std::string lin = "--", bar = "|", sp = "  ", dot = "o";
	std::string temp = dot + sp;
	for (int x = 0; x < width - 1; x++) {
		temp += dot + lin;
	}
	std::cout << temp + dot << std::endl;

	for (int y = 0; y < height; y++) {
		std::string temp1 = "";
		std::string temp2 = "";
		for (int x = 0; x < width; x++) {
			temp1 += sp + ((findVerticalWall(x, y) == true) ? " " : "|");
			temp2 += ((findHorizontalWall(x, y) == true) ? "  " : "--") + dot;
		}
		std::cout << bar << temp1 << std::endl;
		std::cout << dot << temp2 << std::endl;
	}
}

void Maze::positions() {
	std::tuple<int, int, bool, bool, bool, bool> tempTuple;

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
			tempTuple = std::make_tuple(x, y, findVerticalWall(x, y), findVerticalWall(x - 1, y),
				findHorizontalWall(x, y), findHorizontalWall(x, y - 1));
			mazeMapTree[makeNodeKey(std::make_pair(x, y))] = tempTuple;
		}
	}

	verticalWallRender->init(verticalCount, verticalWallPosition);
	horizontalWallRender->init(horizontalCount, horizontalWallPosition);
	passRender->init(width * height, passPosition);
}
