#pragma once

#include "Maze.h"
#include "Util.h"
#include "State.h"
#include "Logger.h"
#include "Shader.h"
#include "ICamera.h"

class CameraMaze : public ICamera {
public:
    CameraMaze(CameraProperties cameraProperties) : ICamera(cameraProperties) {
        init();
        initShader();
        setLight();
        updateVectors();
        view();
        projection();
        update();
    }

    void init() override {
        _cameraProperties.far = 1000.0f;
        _cameraProperties.speed = 0.3f;
        _cameraProperties.sensitivity = 0.04f;
        _cameraProperties.up = glm::vec3{ 0.0f, -1.0f, 0.0f };
    }

    void initShader(const std::string& vsCodePath = "shaders/basic_vs.shader", const std::string& fsCodePath = "shaders/basic_fs.shader") {
        _shader.init(vsCodePath, fsCodePath);
    }

    void setLight() {
        _shader.use();
        _shader.setVec3("lightPos", glm::vec3(0, 10, 0));
        _shader.setVec3("viewPos", glm::vec3(0, 10, 0));
    }

    void setCameraPosition(glm::vec3 position) {
        if (!_cheatActivated) {
            _cameraProperties = CameraProperties{};
            _cameraProperties.position = position;

            _roll = 0.0f, _pitch = 0.0f, _yaw = 90.0f;
            _xPosLast = -1.0f, _yPosLast = -1.0f;

            view();
            updateVectors();
            update();
        }
    }

    void setDimensions(float mazeWidth, float mazeHeight, float cubeThickness) {
        _mazeWidth = mazeWidth;
        _mazeHeight = mazeHeight;
        _cubeThickness = cubeThickness;
    }

    void setWallPos(std::vector<glm::vec3> verticalWallPosition, std::vector<glm::vec3> horizontalWallPosition) {
        _verticalWallPosition = verticalWallPosition;
        _horizontalWallPosition = horizontalWallPosition;
    }

    void handleInput(GLFWwindow* window, Maze** maze, bool& escaping, bool& restart) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        if (_isPlaying == false) {
            return;
        }

        if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (checkE)) {
            escaping = escaping == false ? true : false;
            checkE = false;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            checkE = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (checkR)) {
            setCameraPosition(CameraProperties{}.position);
            delete* maze;
            *maze = new Maze(_cubeThickness, _mazeWidth, _mazeHeight);
            setWallPos((*maze)->getVerticalWallPosition(), (*maze)->getHorizontalWallPosition());
            restart = true;
            checkR = false;
            escaping = false;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
            checkR = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && (checkC)) {
            _cheatActivated = !_cheatActivated;
            checkC = false;
            _cameraProperties.speed = 0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
            checkC = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (_cheatActivated)) {
            _cameraProperties.speed += 0.2f;
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (_cheatActivated)) {
            if (_cameraProperties.speed > 0.3f) {
                _cameraProperties.speed -= 0.2f;
            }
        }

        auto front = _cameraProperties.front;
        auto right = _cameraProperties.right;
        auto cameraPositionTemp = _cameraProperties.position;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // check one step ahead for invisible wall, block stucking or passing maze wall
            cameraPositionTemp += glm::normalize(projectionVector(front, right)) * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_cheatActivated)) {
                // cannot move, step back old position
                cameraPositionTemp = _cameraProperties.position;
            } else if (canIPass(_cameraProperties.position) && (!_cheatActivated)) {
                _cameraProperties.position += glm::normalize(projectionVector(front, right)) * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            } else if (_cheatActivated) {
                _cameraProperties.position += front * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPositionTemp -= glm::normalize(projectionVector(front, right)) * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_cheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if (canIPass(_cameraProperties.position) && (!_cheatActivated)) {
                _cameraProperties.position -= glm::normalize(projectionVector(front, right)) * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            } else if (_cheatActivated) {
                _cameraProperties.position -= front * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPositionTemp += right * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_cheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if ((canIPass(_cameraProperties.position)) && (!_cheatActivated)) {
                _cameraProperties.position += right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            } else if (_cheatActivated) {
                _cameraProperties.position += right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPositionTemp -= right * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_cheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if ((canIPass(_cameraProperties.position)) && (!_cheatActivated)) {
                _cameraProperties.position -= right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            } else if (_cheatActivated) {
                _cameraProperties.position -= right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            }
        }
    }

    void update() {
        view();
        _shader.use();

        if (!_isPlaying) {
            static float dx = 0.0f;
            dx += 0.01f;
            if (dx >= 360) dx = 0.0f;

            auto t = _cubeThickness;
            glm::vec3 center = glm::vec3(0.0f, -t / 2, 0.0f);
            glm::mat4 trans = glm::rotate(dx, center);
            glm::mat4 myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(_mazeWidth / 2) * t * 2, -t / 2, -(_mazeHeight / 2) * t * 2));

            _shader.setMat4("model", trans * myMatrix);
        } else {
            _shader.setMat4("model", glm::mat4(1.0f));
        }

        _shader.setMat4("view", getView());
        _shader.setMat4("projection", getProjection());

        _shader.setFloat("ambientS", _ambient);
    }

    bool checkWin() {
        glm::vec3 pos = _cameraProperties.position;
        float t1 = _cubeThickness, t2 = _cubeThickness * 2, h = _mazeHeight;
        bool x = pos.x >= -_cubeThickness && pos.x <= _cubeThickness;
        bool z = pos.z >= (h - 2) * t2 + t1 && pos.z <= (h - 1) * t2 + t1;
        if (x && z) return true;
        return false;
    }

    float& getAmbient() {
        return _ambient;
    }

    bool& getIsPlaying() {
        return _isPlaying;
    }

private:
    void updateVectors() override {
        glm::vec3 eulerAngle{};
        eulerAngle.x = -cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
        eulerAngle.y = -sin(glm::radians(_pitch));
        eulerAngle.z = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));

        _cameraProperties.front = glm::normalize(eulerAngle);
        _cameraProperties.right = glm::normalize(glm::cross(_cameraProperties.front, _cameraProperties.up));
    }

    bool canIPass(glm::vec3 position) {
        float currentX = position.x;
        float currentZ = position.z;

        // coordinat based location to index based location
        float currentXPosAtMaze = (currentX - (-_cubeThickness)) / (_cubeThickness * 2);
        float currentYPosAtMaze = (currentZ - (-_cubeThickness)) / (_cubeThickness * 2);

        bool cond1 = currentZ > -_cubeThickness; // lower bound
        bool cond2 = currentZ < _cubeThickness * 2 * _mazeHeight - _cubeThickness; // upper bound
        bool cond3 = currentX < _cubeThickness * 2 * _mazeWidth - _cubeThickness;  // right bound
        bool cond4 = currentXPosAtMaze >= 0 && currentXPosAtMaze < _mazeWidth;   // legal index
        bool cond5 = currentYPosAtMaze >= 0 && currentYPosAtMaze < _mazeHeight;  // legal index

        if (cond1 && cond2 && cond3 && cond4 && cond5) {
            std::vector<glm::vec3>::iterator it1;
            std::vector<glm::vec3>::iterator it2;

            position.x = std::round(position.x);
            position.y = 0.0f;
            position.z = std::round(position.z);

            /*     A 5x5 maze
             +Z
              ^
              |
              |    o  o--o--o--o--o
              |    |              |
              |    o  o--o--o--o  o
              |    |  |        |  |
              |    o  o--o--o  o  o
              |    |        |     |
              |    o  o--o  o--o--o
              |    |  |  |        |
              |    o  o  o--o--o  o
              |    |     |        |
              |    o--o--o--o--o  o
              |              (camera)
            (-t,-t)----------------------> +X

            t is the thickness of the cubes. cubes becomes (t*2)x(t*2)x(t*2) cubes.
            so origin of the schema is middle point of the first rendered cube.
            most lower  left   vertical wall ("|",    red walls) is located at (-t, 0, 0)
            most lower right   vertical wall ("|",    red walls) is located at (-t * (width - 1), 0, 0)
            most lower  left horizontal wall ("--", black walls) is located at (0, 0, -t)
            most lower right horizontal wall ("--", black walls) is located at (0, 0, -t * (width - 2))
            x and z of next set of walls increases by t*2

            at first  row we search (t * c, 0, 0)  for vertical wall,    c -> [-1, width - 1]
            at first  row we search (0, 0, t * c)  for horizontal wall,  c -> [-1, width - 1]
            at second row we search (t * c, 0, 10) for vertical wall,    c -> [-1, width - 1]
            at second row we search (10, 0, t * c) for horizontal wall,  c -> [-1, width - 1]

            so at this point z for vertical walls and x for horizontal walls will become indicator of
            which row we are in.
            */

            glm::vec3 vertical(position.x, 0.0f, (std::round(position.z / 10)) * 10);
            glm::vec3 horizontal((std::round(position.x / 10)) * 10, 0.0f, position.z);

            it1 = std::find(_verticalWallPosition.begin(), _verticalWallPosition.end(), vertical);
            it2 = std::find(_horizontalWallPosition.begin(), _horizontalWallPosition.end(), horizontal);
            if ((it1 == _verticalWallPosition.end()) && (it2 == _horizontalWallPosition.end())) {
                return true;
            }
        } else {
            return true;
        }

        return false;
    }

    Shader _shader;
    float _ambient = 0.7f;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    bool _isPlaying = false, _cheatActivated = false;
    bool checkC = true, checkE = true, checkR = true;

    float _mazeWidth{}, _mazeHeight{}, _cubeThickness{};
    std::vector<glm::vec3> _verticalWallPosition{};
    std::vector<glm::vec3> _horizontalWallPosition{};

};
