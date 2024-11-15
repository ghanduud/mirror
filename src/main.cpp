#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

sf::Color operator*(float value, sf::Color color)
{
    return sf::Color(color.r * value, color.g * value, color.b * value, color.a * value);
}

template<class T>
T lerp(const T& start, const T& end, float alpha)
{
    if (alpha < 0) return T(start);
    if (alpha > 1) return T(end);

    return (1 - alpha) * start + alpha * end;
}


class DraggabelPoint {

    sf::CircleShape shape;
    bool isDragging;
public:
    DraggabelPoint(const sf::Vector2f& position)
    {
        shape = sf::CircleShape(10);
        shape.setOrigin(10, 10);
        shape.setPosition(position);
        isDragging = false;
    }

    void beginDrag(const sf::Vector2i& mousePos)
    {
        isDragging = shape.getGlobalBounds().contains((sf::Vector2f)mousePos);
    }

    void drag(const sf::Vector2i& mousePos)
    {
        if (!isDragging) return;
        shape.setPosition((sf::Vector2f)mousePos);
    }

    void endDrag()
    {
        isDragging = false;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    sf::Vector2f getPosition()
    {
        return shape.getPosition();
    }
};


sf::Vector2f reflectPoint(const sf::Vector2f& point, const sf::Vector2f& lineStart, const sf::Vector2f& lineEnd) {
    sf::Vector2f lineDir = lineEnd - lineStart;
    float lineLength = std::sqrt(lineDir.x * lineDir.x + lineDir.y * lineDir.y);
    lineDir /= lineLength;

    sf::Vector2f pointToStart = point - lineStart;
    float projectionLength = pointToStart.x * lineDir.x + pointToStart.y * lineDir.y;
    sf::Vector2f projection = lineStart + projectionLength * lineDir;

    return 2.0f * projection - point;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Reflection", sf::Style::Default);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    sf::Clock clock;
    sf::Time timePerFrame = sf::seconds(1.f / 80.f);

    DraggabelPoint pMirror0(sf::Vector2f(400, 100));
    DraggabelPoint pMirror1(sf::Vector2f(400, 500));
    sf::VertexArray mirror(sf::LineStrip, 2);

    DraggabelPoint pTriangle0(sf::Vector2f(200, 200));
    DraggabelPoint pTriangle1(sf::Vector2f(100, 400));
    DraggabelPoint pTriangle2(sf::Vector2f(300, 400));
    sf::VertexArray triangle(sf::LineStrip, 4);

    sf::VertexArray reflectedTriangle(sf::TrianglesFan, 3);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                pMirror0.beginDrag(sf::Mouse::getPosition(window));
                pMirror1.beginDrag(sf::Mouse::getPosition(window));

                pTriangle0.beginDrag(sf::Mouse::getPosition(window));
                pTriangle1.beginDrag(sf::Mouse::getPosition(window));
                pTriangle2.beginDrag(sf::Mouse::getPosition(window));
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                pMirror0.endDrag();
                pMirror1.endDrag();

                pTriangle0.endDrag();
                pTriangle1.endDrag();
                pTriangle2.endDrag();
            }
        }

        sf::Time elapsedTime = clock.restart();

        while (elapsedTime > timePerFrame) {
            pMirror0.drag(sf::Mouse::getPosition(window));
            pMirror1.drag(sf::Mouse::getPosition(window));

            mirror[0].position = pMirror0.getPosition();
            mirror[1].position = pMirror1.getPosition();

            pTriangle0.drag(sf::Mouse::getPosition(window));
            pTriangle1.drag(sf::Mouse::getPosition(window));
            pTriangle2.drag(sf::Mouse::getPosition(window));

            triangle[0].position = pTriangle0.getPosition();
            triangle[1].position = pTriangle1.getPosition();
            triangle[2].position = pTriangle2.getPosition();
            triangle[3].position = pTriangle0.getPosition();

            reflectedTriangle[0].position = reflectPoint(triangle[0].position, mirror[0].position, mirror[1].position);
            reflectedTriangle[1].position = reflectPoint(triangle[1].position, mirror[0].position, mirror[1].position);
            reflectedTriangle[2].position = reflectPoint(triangle[2].position, mirror[0].position, mirror[1].position);

            reflectedTriangle[0].color = sf::Color(255, 0, 0);
            reflectedTriangle[1].color = sf::Color(0, 255, 0);
            reflectedTriangle[2].color = sf::Color(0, 0, 255);

            elapsedTime -= timePerFrame;
        }

        window.clear();

        pMirror0.draw(window);
        pMirror1.draw(window);
        window.draw(mirror);

        pTriangle0.draw(window);
        pTriangle1.draw(window);
        pTriangle2.draw(window);
        window.draw(triangle);

        window.draw(reflectedTriangle);

        window.display();
    }

    return 0;
}
