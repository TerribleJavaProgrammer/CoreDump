#pragma once

namespace coredump
{
    enum class Color
    {
        WHITE,
        BLACK
    };

    constexpr inline Color invertColor(Color color)
    {
        return color == Color::WHITE ? Color::BLACK : Color::WHITE;
    }

    constexpr inline const char *colorToString(Color color)
    {
        return color == Color::WHITE ? "White" : "Black";
    }

    constexpr inline const char *colorToChar(Color color)
    {
        return color == Color::WHITE ? "w" : "b";
    }
}