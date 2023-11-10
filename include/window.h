#ifndef RUTHEN_WINDOW_H
#define RUTHEN_WINDOW_H

#include <utility>
#include <memory>

namespace ruthen
{

class Window
{
// Declarations, constants, aliases, friends, enumerations
public:
    class Impl;

// Constructors, operators and destructor
public:
    Window();
    Window(int width, int height, const char* title);
    Window(const Window& src) = delete;
    Window& operator=(const Window& rhs) = delete;
    Window(Window&& src) noexcept;
    Window& operator=(Window&& rhs) noexcept;
    ~Window();

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


public:
    bool IsOpen() const;
    bool IsClosed() const;
    bool ShouldClose() const;
    std::pair<int, int> GetDimensions() const;
    bool IsValid() const;
    std::string GetTitle() const;

// Static methods
public:
    static void InitializeGraphicsFunctional();
    static void SetBufferSwapInterval(int interval = 0);
    static void PollEvents();

// Private data
private:
    std::unique_ptr<Impl> impl_;
};

}

#endif