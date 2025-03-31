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

    inline const char* colorToString(Color color)
    {
        return color == Color::WHITE ? "White" : "Black";
    }

    inline const char* colorToChar(Color color)
    {
        return color == Color::WHITE ? "w" : "b";
    }
}