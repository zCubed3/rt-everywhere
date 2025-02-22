//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_FRAMEBUFFER_HPP
#define RTEVERYWHERE_RTE_FRAMEBUFFER_HPP

#include <glm/vec4.hpp>

/// @brief Represents a write-only framebuffer interface, allowing for generic data transportation
class rteFramebuffer {

public:
    /// @brief Reports whether this framebuffer can be shared among other threads, please try to allow concurrency
    virtual bool IsThreadSafe() const = 0;

    /// @brief Reports how wide this framebuffer is
    virtual int GetWidth() const = 0;

    /// @brief Reports how tall this framebuffer is
    virtual int GetHeight() const = 0;


    /// @brief Attempts to write to the pixel located at X,Y with high precision float value
    virtual void WritePixel(int x, int y, glm::vec4 color) = 0;


    /// @brief Used to safely lock and synchronize the framebuffer
    virtual void Lock() = 0;

    /// @brief Used to safely unlock the synchronize the framebuffer
    virtual void Unlock() = 0;

};

#endif //RTEVERYWHERE_RTE_FRAMEBUFFER_HPP
