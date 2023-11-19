
#include <string>
#include <stdexcept>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "core.h"
#include "window.h"
#include "logger.h"

namespace ruthen
{

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

class Window::Impl
{
// Declarations, constants, aliases, friends, enumerations

// Constructors, operators and destructor
public:
    Impl();


// Methods
public:
    void Open(int width, int height, const char* title);
    void Close();
    void MakeCurrent();
    void SwapBuffers();
    void SetSize(int width, int height);
    void SetPosition(int x, int y);
    void SetTitle(const char* title);
    void Clear();
    void Update();

public:
    bool IsOpen() const;
    bool IsClosed() const;
    bool ShouldClose() const;
    std::pair<int, int> GetDimensions() const;
    bool IsValid() const;
    std::string GetTitle() const;

public:
    static void InitializeGraphicsFunctional();

// Static data
private:
    static bool kOpenGLInitFlag;

// Private data
private:
    GLFWwindow* window_handle_;
    std::string window_title_;
    bool window_open_flag_;
    bool window_resize_flag_;


};

//------------------------------------------------------------

bool Window::Impl::kOpenGLInitFlag = false;

//------------------------------------------------------------

Window::Impl::Impl() :
    window_handle_{nullptr},
    window_title_{""},
    window_open_flag_{false},
    window_resize_flag_{true}
{}

//------------------------------------------------------------

void Window::Impl::Open(int width, int height, const char* title)
{
    if(width <= 0) 
    {
        SYSLOGF_ERROR("Invalid \'width\' argument for window construction (%1)", std::to_string(width));
        THROW(std::invalid_argument{"Invalid \'width\' argument for window construction"});
        return;
    }
    if(height <= 0)
    {
        SYSLOGF_ERROR("Invalid \'height\' argument for window construction (%1)", std::to_string(height)); 
        THROW(std::invalid_argument{"Invalid \'height\' argument for window construction"});
        return;
    }
    if(title == nullptr || title[0] == 0)
    {
        SYSLOGF_ERROR("Invalid \'title\' argument for window construction (%1)", title);
        THROW(std::invalid_argument{"Invalid \'title\' argument for window construction"});
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(window == nullptr)
    {
        const char* error_string;
        glfwGetError(&error_string);
        SYSLOGF_ERROR("Failed to create an instance of a window. Internal API message: %1", error_string);
        THROW(std::runtime_error{"Failed to create an instance of a window"});
        return;
    }
    if(IsOpen()) Close();
    window_handle_ = window;
    window_open_flag_ = true;
    MakeCurrent();
    glfwSwapInterval(0);
    Impl::InitializeGraphicsFunctional();
}

//------------------------------------------------------------

void Window::Impl::Close()
{
    if(IsClosed()) 
    {
        SYSLOG_ERROR("Failed to close a non-existent window");
        THROW(std::invalid_argument{"Failed to close a non-existent window"});
        return;
    }
    glfwDestroyWindow(window_handle_);
    window_handle_ = nullptr;
    window_open_flag_ = false;
    window_title_ = "";
}

//------------------------------------------------------------

void Window::Impl::MakeCurrent()
{
    if(!IsValid()) 
    {
        SYSLOG_ERROR("Failed to make a context current of a non-existent window current");
        THROW(std::invalid_argument{"Failed to make a context current of a non-existent window current"});
        return;
    }
    glfwMakeContextCurrent(window_handle_);
}

//------------------------------------------------------------

void Window::Impl::SwapBuffers()
{
    if(!IsValid()) 
    {
        SYSLOG_ERROR("Failed to swap buffers of a non-existent window");
        THROW(std::invalid_argument{"Failed to swap buffers of a non-existent window"});
        return;
    }
    glfwSwapBuffers(window_handle_);
}

//------------------------------------------------------------

void Window::Impl::SetSize(int width, int height)
{
    if(!IsValid()) 
    {
        SYSLOG_ERROR("Failed to set the size of a non-existent window");
        THROW(std::invalid_argument{"Failed to set the size of a non-existent window"});
        return;
    }
    if(width <= 0)
    {
        SYSLOG_ERROR("Invalid \'width\' argument for window resize");
        THROW(std::invalid_argument{"Invalid \'width\' argument for window resize"});
        return;
    }
    if(height <= 0)
    {
        SYSLOG_ERROR("Invalid \'height\' argument for window resize");
        THROW(std::invalid_argument{"Invalid \'height\' argument for window resize"});
        return;
    }
    glfwSetWindowSize(window_handle_, width, height);
}

//------------------------------------------------------------

void Window::Impl::SetPosition(int x, int y)
{
    if(!IsValid())
    {
        SYSLOG_ERROR("Failed to set the position of a non-existent window");
        THROW(std::invalid_argument{"Failed to set the position of a non-existent window"});
        return;
    }
    glfwSetWindowPos(window_handle_, x, y);
}

//------------------------------------------------------------

void Window::Impl::SetTitle(const char* title)
{
    if(!IsValid())
    {
        SYSLOG_ERROR("Failed to set the title of a non-existent window");
        THROW(std::invalid_argument{"Failed to set the title of a non-existent window"});
        return;
    }
    window_title_ = title;
    glfwSetWindowTitle(window_handle_, title);
}

//------------------------------------------------------------

void Window::Impl::Clear()
{
    if(!IsValid())
    {
        SYSLOG_ERROR("Failed to clear a non-existent window");
        THROW(std::invalid_argument{"Failed to clear a non-existent window"});
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

//------------------------------------------------------------

void Window::Impl::Update()
{
    glfwPollEvents();
}

//------------------------------------------------------------

bool Window::Impl::IsOpen() const
{
    return window_open_flag_ == true;
}

//------------------------------------------------------------

bool Window::Impl::IsClosed() const
{
    return window_open_flag_ == false;
}

//------------------------------------------------------------

bool Window::Impl::ShouldClose() const
{
    if(!IsValid()) 
    {
        SYSLOG_ERROR("Failed to check the state of a non-existent window");
        THROW(std::invalid_argument{"Failed to check the state of a non-existent window"});
        return true;
    }
    bool result = glfwWindowShouldClose(window_handle_);
    return result;
}

//------------------------------------------------------------

std::pair<int, int> Window::Impl::GetDimensions() const
{
    if(!IsValid()) 
    {
        SYSLOG_ERROR("Failed to obtain the dimensions of a non-existent window");
        throw std::invalid_argument{"Failed to obtain the dimensions of a non-existent window"};
    }
    int width, height;
    glfwGetWindowSize(window_handle_, &width, &height);
    return {width, height};
}

//------------------------------------------------------------

bool Window::Impl::IsValid() const
{
    bool result = window_handle_ != nullptr;
    return result;
}

//------------------------------------------------------------

std::string Window::Impl::GetTitle() const
{
    if(!IsValid())
    {
        SYSLOG_ERROR("Failed to obtain the title of a non-existent window");
        THROW(std::invalid_argument{"Failed to obtain the title of a non-existent window"});
        return "";
    }
    return window_title_;
}

//------------------------------------------------------------

void Window::Impl::InitializeGraphicsFunctional()
{
    if(glfwGetCurrentContext() == nullptr) 
    {
        SYSLOG_ERROR("No valid OpenGL context detected to load graphics functional");
        THROW(std::runtime_error{"No valid OpenGL context detected to load graphics functional"});
        return;
    }
    glewExperimental = true;
    GLenum code = glewInit();
    if(code == GLEW_OK)
    {
        kOpenGLInitFlag = true;
        return;
    }
    kOpenGLInitFlag = false;
    const GLubyte* description;
    description = glewGetErrorString(code);
    SYSLOGF_ERROR("Failed to initialize graphics functional. Internal API message: %1", reinterpret_cast<const char*>(description));
    THROW(std::runtime_error{"Failed to initialize graphics functional"});
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

Window::Window() :
    impl_{std::make_unique<Window::Impl>()}
{}

//------------------------------------------------------------

Window::Window(int width, int height, const char* title) :
    impl_{std::make_unique<Window::Impl>()}
{
    impl_->Open(width, height, title);
}

//------------------------------------------------------------

Window::Window(Window&& src) noexcept :
    impl_{std::move(src.impl_)}
{}


//------------------------------------------------------------

Window& Window::operator=(Window&& rhs) noexcept
{
    impl_ = std::move(rhs.impl_);
    return *this;
}

//------------------------------------------------------------

Window::~Window()
{
    impl_->Close();
}

//------------------------------------------------------------

void Window::Open(int width, int height, const char* title)
{
    impl_->Open(width, height, title);
}

//------------------------------------------------------------

void Window::Close()
{
    impl_->Close();
}

//------------------------------------------------------------

void Window::MakeCurrent()
{
    impl_->MakeCurrent();
}

//------------------------------------------------------------

void Window::SwapBuffers()
{
    impl_->SwapBuffers();
}

//------------------------------------------------------------

void Window::SetSize(int width, int height)
{
    impl_->SetSize(width, height);
}

//------------------------------------------------------------

void Window::SetPosition(int x, int y)
{
    impl_->SetPosition(x, y);
}

//------------------------------------------------------------

void Window::SetTitle(const char* title)
{
    impl_->SetTitle(title);
}

//------------------------------------------------------------

void Window::Clear()
{
    impl_->Clear();
}

//------------------------------------------------------------

void Window::Update()
{
    impl_->Update();
}

//------------------------------------------------------------

bool Window::IsOpen() const
{
    bool result = impl_->IsOpen();
    return result;
}

//------------------------------------------------------------

bool Window::IsClosed() const
{
    bool result = impl_->IsClosed();
    return result;
}

//------------------------------------------------------------

bool Window::ShouldClose() const
{
    bool result = impl_->ShouldClose();
    return result;
}

//------------------------------------------------------------

std::pair<int, int> Window::GetDimensions() const
{
    auto pair = impl_->GetDimensions();
    return pair;
}

//------------------------------------------------------------

bool Window::IsValid() const
{
    bool result = impl_->IsValid();
    return result;
}

//------------------------------------------------------------

std::string Window::GetTitle() const
{
    std::string title = impl_->GetTitle();
    return title;
}

//------------------------------------------------------------


} // namespace ruthen