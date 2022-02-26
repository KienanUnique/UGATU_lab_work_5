#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>

using namespace sf;

const String windowTitle = "Lab work №5";
const unsigned int width = 600;
const unsigned int height = 600;
const unsigned int maxFPS = 60;

// todo: public const class for int border intersect

class RotatingRectangle {
protected:
    const Color contactColor = Color::Red;

    RectangleShape rectangleShape;
    Color defaultColor;

public:
    virtual void update() = 0;

    RectangleShape getShape() {
        return rectangleShape;
    }

    bool isCollidingShape(const RectangleShape &otherRectangleShape) {
        return rectangleShape.getGlobalBounds().intersects(otherRectangleShape.getGlobalBounds());
    }

    void applyDefaultColor() {
        rectangleShape.setFillColor(defaultColor);
    }

    void applyContactColor() {
        rectangleShape.setFillColor(contactColor);
    }

    Vector2f getPosition() {
        return rectangleShape.getPosition();
    }

    Color getDefaultColor(){
        return defaultColor;
    }
};

class RotatingRectangleOriginal : public RotatingRectangle {
private:
    const float maxRectangleSide = 100;
    const float minRectangleSide = 20;
    const float maxRectangleRotateRadius = 70;
    const float minRectangleRotateRadius = 50;
    const float maxAngularVelocity = 0.05;
    const float minAngularVelocity = 0.001f;
    const double startAngle = 270;

    double angularVelocity{};
    double currentAngle{};
    float rotateRadius{};
    Vector2f rotateCenter;
public:
    explicit RotatingRectangleOriginal() {

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

    void update() override {
        float x = rotateRadius * (float) cos(currentAngle) + rotateCenter.x;
        float y = rotateRadius * (float) sin(currentAngle) + rotateCenter.y;
        rectangleShape.setPosition(x, y);
        currentAngle += angularVelocity;
        if (currentAngle >= 360) {
            currentAngle -= 360;
        }
    }

private:
    static float getRandomFloat(float minRandom, float maxRandom, bool canBeNegative = false) {
        float generatedNum = std::rand() / (float) RAND_MAX * (maxRandom - minRandom) + minRandom;
        if (canBeNegative && std::rand() / (float) RAND_MAX > 0.5f) {
            generatedNum *= -1;
        }
        return generatedNum;
    }

    static Color getRandomColor() {
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;
        return Color(r, g, b);
    }
};


class RotatingRectangleDuplicate : public RotatingRectangle {
private:
    RotatingRectangleOriginal &original;
public:
    // todo spawn object in depending int whichBorderIntersect value
    explicit RotatingRectangleDuplicate(RotatingRectangleOriginal &_original) : original(_original) {
        rectangleShape = RectangleShape(_original.getShape());
        defaultColor = _original.getDefaultColor();
    }

    void update() override {
        Vector2f originalPosition = original.getPosition();
        rectangleShape.setPosition(originalPosition.x + 20, originalPosition.y + 30);
    }
};


class RotatingRectanglesGroup {
private:
    std::vector<RotatingRectangle *> rotatingRectangles;
    RotatingRectangleOriginal *rectangleOriginal;
    int id;
    bool isContactColored = false;

public:
    explicit RotatingRectanglesGroup(int _id, RotatingRectangleOriginal *_rectangleMain) {
        id = _id;
        rectangleOriginal = _rectangleMain;
        rotatingRectangles.push_back(_rectangleMain);
    }

    void updateGroup() {
        for (auto &i: rotatingRectangles) {
            i->update();
        }
    }

    void drawGroup(RenderWindow &window) {
        for (auto &i: rotatingRectangles) {
            window.draw(i->getShape());
        }
    }

    bool isCollidingGroup(RotatingRectanglesGroup objectsToCheck) {
        for (auto &i: objectsToCheck.getGroupVector()) {
            if (isCollidingSingle(i->getShape())) {
                return true;
            }
        }
        return false;
    }

    void applyDefaultColor() {
        isContactColored = false;
        for (auto &i: rotatingRectangles) {
            i->applyDefaultColor();
        }
    }

    void applyContactColor() {
        isContactColored = true;
        for (auto &i: rotatingRectangles) {
            i->applyContactColor();
        }
    }

    bool isGroupDefaultColored() const {
        return !isContactColored;
    }

    bool isGroupContactColored() const {
        return isContactColored;
    }

    void addDuplicate() {
        auto *rectangleDuplicate = new RotatingRectangleDuplicate(*rectangleOriginal);
        rotatingRectangles.push_back(rectangleDuplicate);
    }

    int getId() const {
        return id;
    }

    bool isCollidingSingle(const RectangleShape &shapeToCheck) {
        for (auto &i: rotatingRectangles) {
            if (i->isCollidingShape(shapeToCheck)) {
                return true;
            }
        }
        return false;
    }

protected:
    std::vector<RotatingRectangle *> getGroupVector() {
        return rotatingRectangles;
    }

};


class RotatingRectangleHandler {
private:
    std::vector<RotatingRectanglesGroup*> rotatingRectanglesGroups;
public:
    void spawnMany(int countOfObjects) {
        for (int i = 0; i < countOfObjects; i++) {
            spawnNew();
        }
    }

    void updateAll() {
        for (auto &i: rotatingRectanglesGroups) {
            i->updateGroup();
            bool isInContact = isCollidingOthers(*i);
            if (isInContact && i->isGroupDefaultColored()) {
                i->applyContactColor();
            } else if (!isInContact && i->isGroupContactColored()) {
                i->applyDefaultColor();
            }
        }
    }

    void drawAll(RenderWindow &window) {
        for (auto &i: rotatingRectanglesGroups) {
            i->drawGroup(window);
        }
    }

private:
    void spawnNew() {
        // todo: check borders, if intercept - create duplicate
        auto *rectangleOriginal = new RotatingRectangleOriginal();
        while (isCollidingOthers(*rectangleOriginal)) {
            rectangleOriginal->generateCoordinates();
        }
        auto *rectangleGroup = new RotatingRectanglesGroup((int) rotatingRectanglesGroups.size(), rectangleOriginal);
        rectangleGroup->addDuplicate();
        rotatingRectanglesGroups.push_back(rectangleGroup);
    }

    bool isCollidingOthers(RotatingRectanglesGroup &objectToCheck) {
        for (auto &i: rotatingRectanglesGroups) {
            if (i->getId() != objectToCheck.getId() && objectToCheck.isCollidingGroup(*i)) {
                return true;
            }
        }
        return false;
    }

    bool isCollidingOthers(RotatingRectangleOriginal &objectToCheck) {
        for (auto &i: rotatingRectanglesGroups) {
            if (i->isCollidingSingle(objectToCheck.getShape())) {
                return true;
            }
        }
        return false;
    }

    // todo: willCollideBorderLeft()
    // todo: willCollideBorderRight()
    // todo: willCollideBorderUp()
    // todo: willCollideBorderDown()
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
