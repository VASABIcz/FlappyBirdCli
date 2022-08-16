//
// Created by vasabi on 8/15/22.
//

#include "terminal.h"
#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>
#include <string>
#include <ios>

struct Number {
    int width;
    std::string lines[6];
};

namespace Terminal {
    void die(const char *s) {
        clearScreen();
        disableRawMode();
        perror(s);
        exit(1);
    }

    void disableRawMode() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
            die("tcsetattr");
    }
    void enableRawMode() {
        std::ios_base::sync_with_stdio(false);
        std::setvbuf(stdout, nullptr, _IOFBF, BUFSIZ);
        if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
        atexit(disableRawMode);
        struct termios raw = orig_termios;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
    }

    TerminalDimensions getTerminalDimensions() {
        struct winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        return {w.ws_col, w.ws_row};
    }

    void clearScreen() {
        printf("\e\143");
    }

    void goUp(int n) {
        printf("\e[%d#A", n);
    }

    void goDown(int n) {
        printf("\e[%d#B", n);
    }

    void goRight(int n) {
        printf("\e[%d#C", n);
    }

    void goLeft(int n) {
        printf("\e[%d#D", n);
    }

    void gotoXY(int x, int y) {
        printf("\e[%d;%dH", y, x);
    }

    void flush() {
        Terminal::gotoXY(6969,6969);
        fflush(stdout);
    }

    Direction getArrowKey() {
        char c[3];

        read(STDIN_FILENO, &c, 3);

        if (c[0] == '\e') {
            if (c[2] == 'A') {
                lastDirection = Direction::up;
            }
            else if (c[2] == 'B') {
                lastDirection = Direction::down;
            }
            else if (c[2] == 'C') {
                lastDirection = Direction::right;
            }
            else if (c[2] == 'D') {
                lastDirection = Direction::left;
            }
        }
        return lastDirection;
    }

    Direction getWASDKey() {
        char c = '\0';

        read(STDIN_FILENO, &c, 1);

        if (c == 's') {
            return Direction::down;
        }
        else if (c == 'w') {
            return Direction::up;
        }
        else if (c == 'd') {
            return Direction::right;
        }
        else if (c == 'a') {
            return Direction::left;
        }
        return static_cast<Direction>(-1);
    }

    void drawBox()  {
        gotoXY(0,0);
        for (int i = 0; i < initialSize.width; i++) {
            printf("#");
        }
        printf("\r");

        for (int i = 0; i < initialSize.height-2; ++i) {
            printf("#");
            for (int i = 0; i < initialSize.width-2; i++) {
                printf(" ");
            }
            printf("#");
            printf("\r");
        }

        for (int i = 0; i < initialSize.width; i++) {
            printf("#");
        }
    }

    void drawLine(Position a, Position b, char c) {
        auto res = a-b;
        if (a.x > b.x) {
            auto r = a;
            a = b;
            b = r;
        }
        if (a.y > b.y) {
            auto r = a.y;
            a.y = b.y;
            b.y = r;
        }
        auto lastY = a.y;
        for (int x = a.x; x <= b.x ; x++) {
            int newY;
            if (res.x == 0) {
                newY = b.y;
            }
            else {
                newY = round((float)(x - a.x) / res.x * res.y + a.y);
            }
            for (int y = lastY; y <= newY; y++) {
                gotoXY(x, y);
                printf("%c", c);
            }
            lastY = newY;
        }
    }

    void drawRectangle(Position a, Position b, char c) {
        drawLine(a, Position(b.x, a.y), c);
        drawLine(a, Position(a.x, b.y), c);
        drawLine(Position(b.x, a.y), b, c);
        drawLine(Position(a.x, b.y), b, c);
    }

    void drawPogRectangle(Position a, Position b) {
        drawLine(a, Position(b.x, a.y), '-');
        drawLine(a, Position(a.x, b.y), '|');
        drawLine(Position(b.x, a.y), b, '|');
        drawLine(Position(a.x, b.y), b, '-');
        gotoXY(a.x, b.y);
        printf("%c", '+');
        gotoXY(b.x, a.y);
        printf("%c", '+');
        gotoXY(b.x, b.y);
        printf("%c", '+');
        gotoXY(a.x, a.y);
        printf("%c", '+');
    }

    void handleExit() {
        char c;
        read(STDIN_FILENO, &c, 1);
        if (c == 'q') {
            die("e");
        }
    }

    Number nunbers[] = {
            Number {
                7,
                {"  ___  ",
                 " / _ \\",
                 "| | | |",
                 "| | | |",
                 "| |_| |",
                 " \\___/"}
            },
            Number {
                4,
                {" __ ",
                 "/_ |",
                 " | |",
                 " | |",
                 " | |",
                 " |_|"}
            },
            Number {
                6,
                {" ___  ",
                 "|__ \\",
                 "   ) |",
                 "  / / ",
                 " / /_ ",
                 "|____|"}
            },
            Number {
                7,
                {" ____  ",
                 "|___ \\",
                 "  __) |",
                 " |__ < ",
                 " ___) |",
                 "|____/ "}
            },
            Number {
                    8,
                    {" _  _   ",
                     "| || |  ",
                     "| || |_ ",
                     "|__   _|",
                     "   | |  ",
                     "   |_|  "}
            },
            Number {
                    7,
                    {" _____ ",
                     "| ____|",
                     "| |__  ",
                     "|___ \\",
                     " ___) |",
                     "|____/ "}
            },
            Number {
                    7,
                    {"   __  ",
                     "  / /  ",
                     " / /_  ",
                     "| '_ \\",
                     "| (_) |",
                     " \\___/"}
            },
            Number {
                    8,
                    {" ______ ",
                     "|____  |",
                     "    / / ",
                     "   / /  ",
                     "  / /   ",
                     " /_/    "}
            },
            Number {
                    7,
                    {"  ___  ",
                     " / _ \\",
                     "| (_) |",
                     " > _ < ",
                     "| (_) |",
                     " \\___/"}
            },
            Number {
                    8,
                    {"  ___   ",
                     " / _ \\ ",
                     "| (_) | ",
                     " \\__, |",
                     "   / /  ",
                     "  /_/   "}
            },
    };

    int drawNumber(int n, Position pos) {
        Number numero = nunbers[n];
        int y = 0;
        for (auto &x : numero.lines) {
            gotoXY(pos.x, pos.y+y);
            printf("%s", x.c_str());
            y+=1;
        }
        return numero.width;
    }

    int drawNumbers(int n, Position p) {
        int offset = 0;
        for (int i = 3; i >= 0 ; i--) {
            int num = (n / (int)pow(10, i))%10;
            offset += drawNumber(num, p+offset);
        }
        return offset;
    }
}
