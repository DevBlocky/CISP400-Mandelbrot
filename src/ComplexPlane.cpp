#include "ComplexPlane.h"
#include <complex>
#include <sstream>

#define CAST(val, prim) static_cast<prim>(val)

ComplexPlane::ComplexPlane(float aspectRatio) : m_aspectRatio(aspectRatio) {
    m_view.setSize({BASE_WIDTH, BASE_HEIGHT * m_aspectRatio});
    m_view.setCenter({0.f, 0.f});
}

void ComplexPlane::zoomIn() {
    m_zoomCount++;
    double xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    double ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_view.setSize({CAST(xSize, float), CAST(ySize, float)});
}

void ComplexPlane::zoomOut() {
    m_zoomCount--;
    double xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    double ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_view.setSize({CAST(xSize, float), CAST(ySize, float)});
}

void ComplexPlane::setCenter(const sf::Vector2f &vec) {
    m_view.setCenter(vec);
}

void ComplexPlane::setMouseLocation(const sf::Vector2f &vec) {
    m_mouseLocation = vec;
}

void ComplexPlane::loadText(sf::Text &text) {
    auto center = m_view.getCenter();
    auto cursor = m_mouseLocation;

    std::ostringstream oss;
    oss << "Mandelbrot Set" << std::endl;
    oss << "Center: (" << center.x << ", " << center.y << ")" << std::endl;
    oss << "Cursor: (" << cursor.x << ", " << cursor.y << ")" << std::endl;
    oss << "Left-click to Zoom in" << std::endl;
    oss << "Right-click to Zoom out" << std::endl;

    text.setString(oss.str());
}

size_t ComplexPlane::countIterations(sf::Vector2f coord) {
    std::complex c(coord.x, coord.y);
    std::complex z = c;

    size_t i;
    for (i = 1; i <= MAX_ITER; i++) {
        // effectively the same as abs(z) > 2.0, but less computation
        if (std::norm(z) > 4.0)
            break;
        z = z * z + c;
    }
    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, sf::Uint8 &r, sf::Uint8 &g, sf::Uint8 &b) {
    r = 0;
    g = 0;
    b = 0;
    if (count >= MAX_ITER) return; // don't use more CPU time

    // yes I am aware that this will overflow r/g, and yes it's intended
    float rgMultiplier = pow(CAST(count, float) / CAST(MAX_ITER, float), 1.f / 5.f);
    r = CAST(255 * rgMultiplier, sf::Uint8);
    g = CAST(255 * rgMultiplier, sf::Uint8);
    b = 255;
}
