#include "Game.h"

Game::Game(float w, float h) {
    width = w;
    height = h;

    vertices[0] = vertices[12] = vertices[16] = w - w / 6;
    vertices[9] = vertices[17] = vertices[21] = h - h / (6 * (float)(w) / h);
    vertices[1] = vertices[5] = vertices[13] = h / (6 * (float)(w) / h);
    vertices[4] = vertices[8] = vertices[20] = w / 6;

    shader = new Shader("shaders/game_vs.shader", "shaders/game_fs.shader");
    typer = new Typer(width, height);

    shader->bind();
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
    shader->setMat4("projection", projection);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int w_t, h_t, nrChannels;
    unsigned char* data = stbi_load("textures/gameWindow.png", &w_t, &h_t, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_t, h_t, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::display(std::string userInput, bool play, bool& restart, bool exit) {
    if ((inExit == true) && (restart == true)) {
        inExit = false;
    }
    else if (inExit == true) {
        inExit = true;
    }
    else {
        inExit = exit;
    }
    if (play == true) {
        stopwatch(restart);
        return;
    }
    shader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float x = width / 3 + 45.0f;
    float y = height / 2 - 45.0f;
    renderText(userInput, x + 8 * 46.5, y, 0.5f, glm::vec3(0.3f, 0.8f, 0.9f));
    renderText("Enter maze size: __________", x, y, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
}

void Game::stopwatch(bool& restart) {
    if (start == NULL) start = std::clock();
    if (restart == true) {
        start = 0, hour = 0, min = 0, sec = 0;
        restart = false;
        inExit = false;
    }
    if (inExit == true) {
        timer += 0.01;
        if (timer >= 0.16) {
            renderText(h_m_s, width - width / 5, height - height / 12, 1.0f, glm::vec3(0.95f, 0.6f, 0.0f));
            renderText("YOU WON!", width / 2.8, height / 2, 1.0f, glm::vec3(0.95f, 0.5f, 1.0f));
            if (timer >= 0.32) timer = 0.0f;
        }
        return;
    }
    sec = floor((std::clock() - (double)start) / (double)CLOCKS_PER_SEC);
    if (sec == 60) {
        sec = 0;
        start = 0;
        min++;
        if (min == 60) {
            min = 0;
            hour++;
        }
    }
    std::string sec_s = sec < 10 ? "0" + std::to_string(sec) : std::to_string(sec);
    std::string min_s = min < 10 ? "0" + std::to_string(min) : std::to_string(min);
    std::string hour_s = hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
    h_m_s = hour_s + ":" + min_s + ":" + sec_s;
    renderText(h_m_s, width - width / 5, height - height / 12, 1.0f, glm::vec3(0.95f, 0.6f, 0.0f));
}
