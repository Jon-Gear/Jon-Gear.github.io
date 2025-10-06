#pragma once

#if defined(BEE_PLATFORM_PC) && defined(BEE_GRAPHICS_OPENGL)
#include "platform/opengl/render_xsr_gl.hpp"
#elif defined(BEE_PLATFORM_PROSPERO)
#include "platform/prospero/rendering/render_xsr_prospero.hpp"
#endif
