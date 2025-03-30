#pragma once

namespace coredump
{
    enum class Color
    {
        WHITE,
        BLACK
    };

    inline Color invertColor(Color color)
    {
        return color == Color::WHITE ? Color::BLACK : Color::WHITE;
    }
}