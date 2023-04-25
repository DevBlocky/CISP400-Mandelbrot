#include <iostream>
#include <mutex>
#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"

using namespace sf;

enum class DrawState {
    CALCULATING,
    DISPLAYING
};

int main() {
    // the game window
    auto vm = VideoMode(1920, 1080);
    auto aspectRatio = static_cast<float>(vm.height) / static_cast<float>(vm.width);
    RenderWindow window(vm, "Mandelbrot", Style::Default);

    // an abstract of the complex plane
    ComplexPlane cxPlane(aspectRatio);
    DrawState status = DrawState::CALCULATING;

    // the text shown on screen
    Font fontMain;
    fontMain.loadFromFile("fonts/RobotoMono.ttf");
    Text text;
    text.setFont(fontMain);
    text.setCharacterSize(20);

    // a VertexArray that holds all pixel color values
    VertexArray vArray(PrimitiveType::Points);
    vArray.resize(vm.width * vm.height);
    std::mutex vArray_lock; // for thread safety when multi-threading

    /*
     * MAIN GAME LOOP
     */
    while (window.isOpen()) {
        /*
         * POLL EVENTS
         */
        Event pollEv{};
        while (window.pollEvent(pollEv)) {
            // close window on esc
            if (pollEv.type == Event::KeyPressed && pollEv.key.code == Keyboard::Escape)
                window.close();
            // close window on close btn press
            if (pollEv.type == Event::Closed)
                window.close();

            // mouse click
            if (pollEv.type == Event::MouseButtonPressed) {
                auto coord = window.mapPixelToCoords({pollEv.mouseButton.x, pollEv.mouseButton.y}, cxPlane.getView());
                // set the center of the complex plane to the mouse coord
                cxPlane.setCenter(coord);
                // left click: zoom out, right click: zoom in
                if (pollEv.mouseButton.button == Mouse::Left)
                    cxPlane.zoomIn();
                else if (pollEv.mouseButton.button == Mouse::Right)
                    cxPlane.zoomOut();
                // recalculate complex plane
                status = DrawState::CALCULATING;
            }

            // mouse move
            if (pollEv.type == Event::MouseMoved) {
                // update cxPlane's mouse position
                auto coord = window.mapPixelToCoords({pollEv.mouseMove.x, pollEv.mouseMove.y}, cxPlane.getView());
                cxPlane.setMouseLocation(coord);
            }
        }

        /*
         * UPDATE SCENE (SINGLE THREADED)
         */
//        if (status == DrawState::CALCULATING) {
//            for (auto y = 0; y < vm.height; y++) {
//                for (auto x = 0; x < vm.width; x++) {
//                    // set position of the vertex in array
//                    vArray[x + y * vm.width].position = {static_cast<float>(x), static_cast<float>(y)};
//
//                    // find the nIters for a pixel
//                    auto pixelCoord = window.mapPixelToCoords({x, y}, cxPlane.getView());
//                    auto nIters = ComplexPlane::countIterations(pixelCoord);
//
//                    // get the associated rgb value and set pixel color
//                    Uint8 r, g, b;
//                    ComplexPlane::iterationsToRGB(nIters, r, g, b);
//                    vArray[x + y * vm.width].color = {r, g, b};
//                }
//            }
//            status = DrawState::DISPLAYING;
//        }
        /*
         * UPDATE SCENE (MULTI THREADED)
         *
         * For a better explanation:
         * A thread is created for each logical processor of the machine
         * Each thread will calculate a row of pixels, where the row (y-value) is decided by the nextY counter
         * Each thread will then put the calculated colors into vArray
         * Once nextY reaches the screen height, all threads will exit
         * The main thread waits by joining all created threads
         */
        if (status == DrawState::CALCULATING) {
            std::vector<std::thread> computeThreads;
            std::atomic<int> nextY(0); // next y-value that should be computed (atomic for thread safety)

            // create threads for all processors
            for (int n = 0; n < std::thread::hardware_concurrency(); n++) {
                std::thread t([&]() {
                    std::vector<Color> pixelColors;
                    pixelColors.resize(vm.width);

                    while (true) {
                        // find the next y-value in the queue
                        auto y = nextY.fetch_add(1);
                        if (y >= vm.height) break; // exit if we've gone through all y-values

                        // loop through x-values and compute color
                        for (int x = 0; x < vm.width; x++) {
                            auto pixelCoord = window.mapPixelToCoords({x, y}, cxPlane.getView());
                            auto nIters = ComplexPlane::countIterations(pixelCoord);

                            Uint8 r, g, b;
                            ComplexPlane::iterationsToRGB(nIters, r, g, b);
                            pixelColors[x] = {r, g, b};
                        }

                        // lock the vertex array and apply positions/colors
                        vArray_lock.lock();
                        for (int x = 0; x < vm.width; x++) {
                            auto &pixel = vArray[x + y * vm.width];
                            pixel.position = {static_cast<float>(x), static_cast<float>(y)};
                            pixel.color = pixelColors[x];
                        }
                        vArray_lock.unlock();
                    }
                });
                computeThreads.push_back(std::move(t));
            }

            // wait for all threads to finish
            for (auto &thread: computeThreads) thread.join();
            status = DrawState::DISPLAYING;
        }
        // update the text
        cxPlane.loadText(text);

        /*
         * DRAW COMMANDS
         */
        window.clear();
        window.draw(vArray);
        window.draw(text);
        window.display();
    }

    std::cout << "Goodbye!" << std::endl;

    return 0;
}
