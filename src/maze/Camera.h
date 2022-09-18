#pragma once

#include <GLFW/glfw3.h>

#include "../engine/Util.h"
#include "../engine/Logger.h"
#include "../engine/Shader.h"
#include "../engine/ICamera.h"

class Camera : public ICamera {
public:
    Camera(CameraProperties cameraProperties) : ICamera(cameraProperties) {
        LOGGER(info, "camera init succeeded");
    }

    ~Camera() override {
        LOGGER(info, "camera delete succeeded");
    }

    void init() override {
        _cameraProperties.far = 1000.0f;
        _cameraProperties.speed = 0.3f;
        _cameraProperties.sensitivity = 0.04f;

        updateEulerAngles();
        updateView();
        updateProjection();
        update();
    }

    void setLight() {
        _shader.use();
        _shader.setFloat("u_ambient", _ambient);
        _shader.setVec3("u_viewPos", glm::vec3{ 0, 10, 0 });
        _shader.setVec3("u_lightPos", glm::vec3{ 0, 10, 0 });
    }

    void setCameraPosition(const glm::vec3& position) {
        _cameraProperties = CameraProperties{};
        _cameraProperties.position = position;

        _roll = 0.0f;
        _pitch = 0.0f;
        _yaw = 90.0f;
        _xPosLast = -1.0f;
        _yPosLast = -1.0f;

        _isPlaying = false;
        _isCheatActivated = false;
        _checkC = true;
        _checkE = true;
        _checkR = true;

        updateView();
        updateEulerAngles();
        update();
    }

    void setMazeDimensions(float mazeWidth, float mazeHeight, float cubeThickness) {
        _mazeWidth = mazeWidth;
        _mazeHeight = mazeHeight;
        _cubeThickness = cubeThickness;
    }

    void setMazeWallPositions(const std::vector<glm::vec3>& verticalWallPosition, const std::vector<glm::vec3>& horizontalWallPosition) {
        _verticalWallPosition = verticalWallPosition;
        _horizontalWallPosition = horizontalWallPosition;
    }

    void handleInput(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        if (_isPlaying == false) {
            return;
        }

        if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && _checkE) {
            _showEscapePath = !_showEscapePath;
            _checkE = false;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            _checkE = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) && _checkC) {
            _isCheatActivated = !_isCheatActivated;
            _cameraProperties.speed = 0.3f;
            _checkC = false;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
            _checkC = true;
        }

        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && _isCheatActivated) {
            _cameraProperties.speed += 0.2f;
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && _isCheatActivated) {
            if (_cameraProperties.speed > 0.3f) {
                _cameraProperties.speed -= 0.2f;
            }
        }

        auto front = _cameraProperties.front;
        auto right = _cameraProperties.right;
        auto cameraPositionTemp = _cameraProperties.position;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPositionTemp += glm::normalize(calculateProjectionVector(front, right)) * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_isCheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if (canIPass(_cameraProperties.position) && (!_isCheatActivated)) {
                _cameraProperties.position += glm::normalize(calculateProjectionVector(front, right)) * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            } else if (_isCheatActivated) {
                _cameraProperties.position += front * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPositionTemp -= glm::normalize(calculateProjectionVector(front, right)) * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_isCheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if (canIPass(_cameraProperties.position) && (!_isCheatActivated)) {
                _cameraProperties.position -= glm::normalize(calculateProjectionVector(front, right)) * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            } else if (_isCheatActivated) {
                _cameraProperties.position -= front * _cameraProperties.speed;
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPositionTemp += right * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_isCheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if ((canIPass(_cameraProperties.position)) && (!_isCheatActivated)) {
                _cameraProperties.position += right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            } else if (_isCheatActivated) {
                _cameraProperties.position += right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPositionTemp -= right * _cameraProperties.speed;
            if ((canIPass(cameraPositionTemp) == false) && (!_isCheatActivated)) {
                cameraPositionTemp = _cameraProperties.position;
            } else if ((canIPass(_cameraProperties.position)) && (!_isCheatActivated)) {
                _cameraProperties.position -= right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            } else if (_isCheatActivated) {
                _cameraProperties.position -= right * (_cameraProperties.speed);
                cameraPositionTemp = _cameraProperties.position;
            }
        }
    }

    void update() override {
        updateView();

        _shader.use();
        glm::mat4 model = getModel();
        if (!_isPlaying) {
            model = getRotateModel();
        }
        _shader.setMat4("u_model", model);
        _shader.setMat4("u_view", getView());
        _shader.setMat4("u_projection", getProjection());
    }

    bool checkWin() {
        const glm::vec3 position = _cameraProperties.position;
        const float h = _mazeHeight;
        const float t = _cubeThickness;
        const bool conditionX = position.x >= -_cubeThickness && position.x <= _cubeThickness;
        const bool conditionZ = position.z >= (h - 2) * t * 2 + t && position.z <= (h - 1) * t * 2 + t;
        if (conditionX && conditionZ) {
            return true;
        }
        return false;
    }

    float getAmbient() {
        return _ambient;
    }

    void incrementAmbient(float val) {
        _ambient += val;
        _shader.use();
        _shader.setFloat("u_ambient", _ambient);
    }

    bool& getIsPlaying() {
        return _isPlaying;
    }

    bool& showEscapePath() {
        return _showEscapePath;
    }

    bool& isRestartTriggered() {
        return _isRestartTriggered;
    }

private:
    glm::vec3 calculateProjectionVector(const glm::vec3& front, const glm::vec3& right) {
        glm::vec3 u = glm::cross(_cameraProperties.up, right);
        glm::vec3 v = front;

        return ((u * v) / static_cast<float>(pow(glm::length(u), 2)) * u);
    }

    glm::mat4 getRotateModel() {
        static float dx = 0.0f;

        dx += 0.01f;
        if (dx >= 360.0f) {
            dx = 0.0f;
        }

        const auto t = _cubeThickness;
        glm::vec3 center = glm::vec3(0.0f, -t / 2, 0.0f);
        glm::mat4 rotate = glm::rotate(dx, center);
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-(_mazeWidth / 2) * t * 2, -t / 2, -(_mazeHeight / 2) * t * 2));

        return rotate * translate;
    }

    bool canIPass(const glm::vec3& position) {
        float currentX = position.x;
        float currentZ = position.z;

        // coordinate based location to index based location
        float currentXPosAtMaze = (currentX - (-_cubeThickness)) / (_cubeThickness * 2);
        float currentYPosAtMaze = (currentZ - (-_cubeThickness)) / (_cubeThickness * 2);

        bool cond1 = currentZ > -_cubeThickness;                                    // lower bound
        bool cond2 = currentZ < _cubeThickness * 2 * _mazeHeight - _cubeThickness;  // upper bound
        bool cond3 = currentX < _cubeThickness * 2 * _mazeWidth - _cubeThickness;   // right bound
        bool cond4 = currentXPosAtMaze >= 0 && currentXPosAtMaze < _mazeWidth;      // legal index
        bool cond5 = currentYPosAtMaze >= 0 && currentYPosAtMaze < _mazeHeight;     // legal index

        if (cond1 && cond2 && cond3 && cond4 && cond5) {
            currentX = std::round(position.x);
            currentZ = std::round(position.z);

            /*     A 5x5 maze
             +Z
              ^
              |               (exit)
              |    o--o--o--o--o  o
              |    |              |
              |    o  o--o--o--o  o
              |    |  |        |  |
              |    o  o--o--o  o  o
              |    |        |     |
              |    o  o--o  o--o--o
              |    |  |  |        |
              |    o  o  o--o--o  o
              |    |     |        |
              |    o  o--o--o--o--o
              |  (camera)
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

            glm::vec3 vertical(currentX, 0.0f, (std::round(currentZ / 10)) * 10);
            glm::vec3 horizontal((std::round(currentX / 10)) * 10, 0.0f, currentZ);

            auto it1 = std::find(_verticalWallPosition.begin(), _verticalWallPosition.end(), vertical);
            auto it2 = std::find(_horizontalWallPosition.begin(), _horizontalWallPosition.end(), horizontal);
            if ((it1 == _verticalWallPosition.end()) && (it2 == _horizontalWallPosition.end())) {
                return true;
            }
        } else {
            return true;
        }

        return false;
    }

    float _ambient{ 0.7f };
    glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };

    bool _isPlaying{ false };
    bool _showEscapePath{ false };
    bool _isCheatActivated{ false };
    bool _isRestartTriggered{ false };

    bool _checkC{ true };
    bool _checkE{ true };
    bool _checkR{ true };

    float _mazeWidth{ 0.0f };
    float _mazeHeight{ 0.0f };
    float _cubeThickness{ 0.0f };
    std::vector<glm::vec3> _verticalWallPosition;
    std::vector<glm::vec3> _horizontalWallPosition;

};
