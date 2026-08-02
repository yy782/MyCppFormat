#include <iostream>
#include <memory>
#include <utility>

class Shape {
public:
    virtual ~Shape() = default;

    virtual double area() const = 0;

    virtual void draw() const = 0;

    virtual std::unique_ptr<Shape> clone() const = 0;
};

class Circle final : public Shape {
private:
    double radius_;

public:
    explicit Circle(double radius) : radius_(radius) {}

    double area() const override {
        return 3.14159 * radius_ * radius_;
    }

    void draw() const override {
        std::cout << "Drawing a circle with radius " << radius_ << std::endl;
    }

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);
    }

    Circle& operator=(const Circle &other) {
        if (this != &other) {
            radius_ = other.radius_;
        }
        return *this;
    }
};

template <typename T>
class Rectangle final : public Shape {
private:
    T width_;
    T height_;

public:
    Rectangle(T width,T height) : width_(std::move(width)),height_(std::move(height)) {}

    double area() const override {
        return static_cast<double>(width_) * static_cast<double>(height_);
    }

    void draw() const override {
        std::cout << "Drawing a rectangle " << width_ << "x" << height_ << std::endl;
    }

    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Rectangle<T>>(*this);
    }

    Rectangle<T>& operator=(const Rectangle<T> &other) {
        if (this != &other) {
            width_ = other.width_;
            height_ = other.height_;
        }
        return *this;
    }
};

int main() {
    std::unique_ptr<Shape> circle = std::make_unique<Circle>(5.0);
    std::unique_ptr<Shape> rectangle = std::make_unique<Rectangle<int>>(4,6);

    circle->draw();
    std::cout << "Area: " << circle->area() << std::endl;

    rectangle->draw();
    std::cout << "Area: " << rectangle->area() << std::endl;

    auto cloned = rectangle->clone();
    cloned->draw();

    return 0;
}