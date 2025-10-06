#pragma once
#include <platform/opengl/open_gl.hpp>

namespace bee
{

class FrameBuffer
{
public:
    FrameBuffer(int, int);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    unsigned int GetColorBuffer() { return CB; }

private:
    unsigned int FBO = 0;
    unsigned int RBO = 0;
    unsigned int CB = 0;

    int m_width = -1;
    int m_height = -1;
};
}