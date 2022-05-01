#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Max attributes: " << nrAttributes << std::endl;
}