#include "renderutils.h"

ScreenSpacedBuffer::ScreenSpacedBuffer() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Pos (x,y)
    const GLfloat vertices[] = {
        -1.f, -1.f,
        1.f, -1.f,
        1.f, 1.f,

        -1.f, -1.f,
        1.f, 1.f,
        -1.f, 1.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void ScreenSpacedBuffer::bind() {
    glBindVertexArray(mVAO);
}

void ScreenSpacedBuffer::unbind() {
    glBindVertexArray(0);
}

void ScreenSpacedBuffer::draw() {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

ScreenSpacedBuffer::~ScreenSpacedBuffer() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}