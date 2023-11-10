
#include <stdexcept>

#include "GLFW/glfw3.h"

#include "core.h"

namespace ruthen
{

void InitializeAPIs()
{
    int code = glfwInit();
    if(code != GLFW_TRUE)
    {
        glfwTerminate();
        throw std::runtime_error{"Failed to initialize internal APIs."};
    }
}

void TerminateAPIs()
{
    glfwTerminate();
}

}