//
// Created by vasabi on 8/15/22.
//

#ifndef UNTITLED6_TERMINAL_H
#define UNTITLED6_TERMINAL_H
#include <termios.h>

namespace Terminal {
    struct TerminalDimensions {
        int width;
        int height;
    };

    enum Direction {
        down,
        up,
        right,
        left
    };

    struct Position {
        Position(int x, int y)
                : x {x}, y{y} {}
        int x;
        int y;

        Position operator + (Position const &obj) const {
            return {this->x + obj.x, this->y + obj.y};
        }
        Position operator + (int const &obj) const {
            return {this->x + obj, this->y};
        }
        Position operator - (Position const &obj) const {
            return {this->x - obj.x, this->y - obj.y};
        }
        Position operator * (Position const &obj) const {
            return {this->x * obj.x, this->y * obj.y};
        }
        Position operator / (Position const &obj) const {
            return {this->x / obj.x, this->y / obj.y};
        }
    };

    void enableRawMode();

    void disableRawMode();

    void gotoXY(int x, int y);

    void goDown(int n);

    void goUp(int n);

    void goRight(int n);

    void goLeft(int n);

    void flush();

    void clearScreen();

    void die(const char *s);

    void drawBox();

    void handleExit();

    void drawLine(Position a, Position b, char c);

    void drawRectangle(Position a, Position b, char c);

    void drawPogRectangle(Position a, Position b);

    int drawNumber(int n, Position p);

    int drawNumbers(int n, Position p);

    Direction getArrowKey();

    Direction getWASDKey();

    TerminalDimensions getTerminalDimensions();

    inline TerminalDimensions initialSize = getTerminalDimensions();

    inline Direction lastDirection;

    inline termios orig_termios;
}

#endif //UNTITLED6_TERMINAL_H
