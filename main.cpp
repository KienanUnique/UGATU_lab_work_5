#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <cmath>

using namespace sf;

const String windowTitle = "Lab work №5";
const unsigned int width = 600;
const unsigned int height = 600;
const unsigned int maxFPS = 60;

float getRandomFloat(float minRandom, float maxRandom, bool canBeNegative = false) {
    float generatedNum = std::rand() / (float) RAND_MAX * (maxRandom - minRandom) + minRandom;
    if(canBeNegative && std::rand() / (float) RAND_MAX > 0.5f){
        generatedNum *= -1;
    }
    return generatedNum;
}

Color getRandomColor() {
    int r = rand() % 255;
    int g = rand() % 255;
    int b = rand() % 255;
    return Color(r, g, b);
}

class RotatingRectangle {
private:
    const float maxRectangleSide = 100;
    const float minRectangleSide = 20;
    const float maxRectangleRotateRadius = 70;
    const float minRectangleRotateRadius = 50;
    const float maxAngularVelocity = 0.05;
    const float minAngularVelocity = 0.001f;
    const Color contactColor = Color::Red;
    const double startAngle = 270;

    RectangleShape rectangleShape;
    Vector2f rotateCenter;
    Color defaultColor;
    double angularVelocity;
    double currentAngle;
    float rotateRadius;
    int id;

public:
    RotatingRectangle(int _id) {
        id = _id;

        defaultColor = getRandomColor();
        currentAngle = startAngle;

        rectangleShape.setSize({getRandomFloat(minRectangleSide, maxRectangleSide),
                                getRandomFloat(minRectangleSide, maxRectangleSide)});
        generateCoordinates();
        rectangleShape.setFillColor(defaultColor);
        rectangleShape.setOrigin(rectangleShape.getSize().x / 2, rectangleShape.getSize().y / 2);

        angularVelocity = getRandomFloat(minAngularVelocity, maxAngularVelocity, true);
    }

    void generateCoordinates() {
        float delta = maxRectangleRotateRadius + std::max(rectangleShape.getSize().x, rectangleShape.getSize().y);
        rotateCenter = {getRandomFloat(delta, width - delta), getRandomFloat(delta, height - delta)};
        rotateRadius = getRandomFloat(minRectangleRotateRadius, maxRectangleRotateRadius);
        rectangleShape.setPosition(rotateCenter.x, rotateCenter.y + rotateRadius);
    }

    void update() {
        float x = rotateRadius * (float)cos(currentAngle) + rotateCenter.x;
        float y = rotateRadius * (float)sin(currentAngle) + rotateCenter.y;
        rectangleShape.setPosition(x, y);
        currentAngle += angularVelocity;
        if(currentAngle >= 360){
            currentAngle -= 360;
        }
    }

    RectangleShape getShape() {
        return rectangleShape;
    }

    bool isCollidingShape(const RectangleShape &otherRectangleShape) {
        return rectangleShape.getGlobalBounds().intersects(otherRectangleShape.getGlobalBounds());
    }

    int getId() const {
        return id;
    }

    void applyDefaultColor(){
        rectangleShape.setFillColor(defaultColor);
    }

    void applyContactColor(){
        rectangleShape.setFillColor(contactColor);
    }

    bool isContactColored(){
        return rectangleShape.getFillColor() == contactColor;
    }

    bool isDefaultColored(){
        return rectangleShape.getFillColor() == defaultColor;
    }
private:

};

class RotatingRectangleHandler {
private:
    std::vector<RotatingRectangle> rotatingRectangles;
public:
    void spawnMany(int countOfObjects) {
        for (int i = 0; i < countOfObjects; i++) {
            spawnNew();
        }
    }

    void updateAll(){
        for (auto &i: rotatingRectangles) {
            i.update();
            bool isInContact = isCollidingOthers(i);
            if(isInContact && i.isDefaultColored()){
                i.applyContactColor();
            }
            else if(!isInContact && i.isContactColored()){
                i.applyDefaultColor();
            }
        }
    }

    void drawAll(RenderWindow &window){
        for (auto &i: rotatingRectangles) {
            window.draw(i.getShape());
        }
    }

private:
    void spawnNew() {
        RotatingRectangle rotatingRectangle = RotatingRectangle((int) rotatingRectangles.size());
        while (isCollidingOthers(rotatingRectangle)) {
            rotatingRectangle.generateCoordinates();
        }
        rotatingRectangles.push_back(rotatingRectangle);
    }

    bool isCollidingOthers(RotatingRectangle objectToCheck) {
        for (auto &i: rotatingRectangles) {
            if (i.getId() != objectToCheck.getId() && objectToCheck.isCollidingShape(i.getShape())) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    std::srand(std::time(NULL));

    RenderWindow window(VideoMode(width, height), windowTitle);
    window.setFramerateLimit(maxFPS);

    RotatingRectangleHandler rotatingRectangleHandler;
    rotatingRectangleHandler.spawnMany(10);

    while (window.isOpen()) {
        window.clear();
        Event event{};
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        rotatingRectangleHandler.updateAll();
        rotatingRectangleHandler.drawAll(window);

        window.display();
    }

    return 0;
}
