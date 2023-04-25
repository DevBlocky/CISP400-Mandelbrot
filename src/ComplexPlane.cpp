#include "ComplexPlane.h"
#include <complex>
#include <sstream>

ComplexPlane::ComplexPlane(float aspectRatio) : m_aspectRatio(aspectRatio) {
    m_view.setSize({BASE_WIDTH, BASE_HEIGHT * m_aspectRatio});
    m_view.setCenter({0.f, 0.f});
}

void ComplexPlane::zoomIn() {
    m_zoomCount++;
    float xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    float ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_view.setSize({xSize, ySize});
}

void ComplexPlane::zoomOut() {
    m_zoomCount--;
    float xSize = BASE_WIDTH * pow(BASE_ZOOM, m_zoomCount);
    float ySize = BASE_HEIGHT * m_aspectRatio * pow(BASE_ZOOM, m_zoomCount);
    m_view.setSize({xSize, ySize});
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

    // color based on count (lower index == lower count)
    static std::vector<sf::Color> colorMaps = {
            sf::Color::Red,
            sf::Color::Yellow,
            sf::Color::Green,
            {48, 213, 200}, // turquoise
            {255, 0, 255}, // purple
    };

    // find the color to use
    for (size_t i = 0; i < colorMaps.size(); i++) {
        auto threshold = static_cast<float>(i + 1) / static_cast<float>(colorMaps.size());
        if (static_cast<float>(count) < threshold * MAX_ITER) {
            auto const &color = colorMaps[i];
            r = color.r;
            g = color.g;
            b = color.b;
            return;
        }
    }
}
