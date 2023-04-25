#pragma once

#include <SFML/Graphics.hpp>

const unsigned int MAX_ITER = 128;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

class ComplexPlane {
public:
    explicit ComplexPlane(float aspectRatio);

    void zoomIn();

    void zoomOut();

    void setCenter(const sf::Vector2f &coord);

    void setMouseLocation(const sf::Vector2f &coord);

    void loadText(sf::Text &text);

    static size_t countIterations(sf::Vector2f coord);

    static void iterationsToRGB(size_t count, sf::Uint8 &r, sf::Uint8 &g, sf::Uint8 &b);

    const sf::View &getView() const { return m_view; };

private:
    sf::Vector2f m_mouseLocation;
    sf::View m_view;
    int m_zoomCount = 0;
    float m_aspectRatio;
};
