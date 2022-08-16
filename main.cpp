#include <cstdlib>
#include "terminal.h"
#include "channel.h"
#include <chrono>

enum KeyboardEvent {
    Spacebar,
    Quit,

};

auto channel = buffered_channel<KeyboardEvent>();

void readKeyboard() {
    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 0) continue;
        if (c == ' ') {
            channel.send(KeyboardEvent::Spacebar);
        }
        else if (c == 'q') {
            channel.send(KeyboardEvent::Quit);
        }
    }
}



int main() {
    Terminal::enableRawMode();
    Terminal::clearScreen();

    std::thread keyboardThread (readKeyboard);
    keyboardThread.detach();

    int width = 100;
    int height = 50;
    int topOffset = 9;

    float birdY = height/2;
    int birdX = width/3;

    int birdHeight = 1;
    int birdWidth = 3;

    int counter = 0;

    int boxX = 100;
    int boxWidth = 10;
    auto boxIdk = Terminal::Position(boxWidth,2*(rand()%10));
    auto boxIdk2 = Terminal::Position(boxWidth,2*(rand()%10));

    KeyboardEvent event;

    while (true) {
        Terminal::drawPogRectangle(Terminal::Position(0,0), Terminal::Position(100,50));
        //Terminal::flush();

        Terminal::drawNumbers(counter, Terminal::Position(3,2));
        Terminal::gotoXY(80,2);
        printf("exit with Q");
        //Terminal::flush();
        Terminal::drawPogRectangle(Terminal::Position(0,0), Terminal::Position(100, topOffset-1));
        //Terminal::flush();

        Terminal::drawPogRectangle(Terminal::Position(birdX, (int)birdY), Terminal::Position(birdX+birdWidth, (int)birdY+birdHeight));
        //Terminal::flush();

        auto boxPos = Terminal::Position(boxX,topOffset-1);
        auto boxPos2 = Terminal::Position(boxX+10,height);

        Terminal::drawPogRectangle(boxPos, boxPos+boxIdk);
        Terminal::drawPogRectangle(boxPos2, boxPos2-boxIdk2);

        Terminal::flush();


        boxX-=2;
        birdY+=0.3;
        if (boxX <= -3) {
            boxX = width-boxWidth;
            boxIdk.y = std::max(abs(2*(rand()%10)),2);
            boxIdk2.y = std::max(abs(2*(rand()%10)),2);
        }
        if (birdY > height-(birdHeight+1)) {
            birdY = height-(birdHeight+1);
        }
        else if (birdY <= 2) {
            birdY = 2;
        }

        if (boxX == width/3+1) {
            counter++;
        }

        if (boxX <= birdX && boxX+boxWidth >= birdX) {
            if (birdY > (boxPos2-boxIdk2).y) {
                counter = 0;
                birdY = height/2;
                boxX = 100;
                boxIdk.y = std::max(abs(2*(rand()%10)),2);
                boxIdk2.y = std::max(abs(2*(rand()%10)),2);
            }
            else if (birdY < (boxPos+boxIdk).y) {
                counter = 0;
                birdY = height/2;
                boxX = 100;
                boxIdk.y = std::max(abs(2*(rand()%10)),2);
                boxIdk2.y = std::max(abs(2*(rand()%10)),2);
            }
        }


        if (channel.nonBrecv(event)) {
            switch (event) {
                case Spacebar:
                    birdY-=12;
                    break;
                case Quit:
                    Terminal::die("gud");
                    break;
            }
        }
        if (birdY < topOffset) {
            birdY = topOffset;
        }

        Terminal::clearScreen();
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
}