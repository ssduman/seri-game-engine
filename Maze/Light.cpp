#include "Light.h"

Light::Light() {
    lightShader = new Shader("shaders/light_vs.shader", "shaders/light_fs.shader");

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Light::light(glm::vec3 pos, glm::mat4 view, glm::mat4 projection) {
    lightShader->bind();
    lightShader->setVec3("pos", pos + glm::vec3(0, -10, 20));
    lightShader->setMat4("projection", projection);
    lightShader->setMat4("view", view);
    lightShader->setMat4("model", glm::mat4(1.0f));
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
