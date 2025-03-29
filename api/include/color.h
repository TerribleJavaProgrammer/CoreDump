#ifndef COLOR_H
#define COLOR_H
enum class Color
{
    WHITE,
    BLACK
};

inline Color invertColor(Color color) {
    return color == Color::WHITE ? Color::BLACK : Color::WHITE;
}

#endif // COLOR_H