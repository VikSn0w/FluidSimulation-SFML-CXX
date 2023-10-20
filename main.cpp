#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iomanip>

#define SCREEN_SIZE_X 500
#define SCREEN_SIZE_Y 500
#define FPS_LIMIT 60
#define PARTICLE_RADIUS 10.f
#define GRAVITATIONAL_COSTANT 9.81f
#define PARTICLE_MASS 100
#define ELASTIC_COEFFICIENT 0.7

int Sign(double number){
    if (number > 0) {
        return 1.0f;
    } else if (number < 0) {
        return -1.0f;
    } else {
        return 0.0f;
    }
}

struct Position {
    double x = 0.f;
    double y = 0.f;
};

struct Velocity {
    double x = 0.f;
    double y = 0.f;
};

struct Particle {
    sf::CircleShape drawing;
    Position position;
    Velocity velocity;
    double elastic_coefficient = 0.f;
    double mass = 0.f;

    // Constructor to initialize the particle
    Particle(double r, double x, double y, double ec, double m) : drawing(r) {
        // Customize the particle's properties here if needed
        drawing.setFillColor(sf::Color(127, 127, 255));
        drawing.setPosition(x, y);
        elastic_coefficient = ec;
        position.x = drawing.getPosition().x;
        position.y = drawing.getPosition().y;
        mass = m;
    }

};

void updateParticle(Position &position, Velocity &velocity, double deltaTime, Particle &particle){
    velocity.y += (-GRAVITATIONAL_COSTANT*particle.mass) * -1 * deltaTime;
    position.y += velocity.y * deltaTime;
    particle.drawing.setPosition(particle.drawing.getPosition().x, position.y);
    printf("y: %f\n", position.y);
};

void checkForCollisions(Particle &particle){
    double d = particle.drawing.getRadius()*2;
    if(abs(particle.drawing.getPosition().x) > SCREEN_SIZE_X - d){
        float x = SCREEN_SIZE_X*d * Sign(particle.drawing.getPosition().x);
        particle.position.x = x;
        particle.drawing.setPosition(x, particle.drawing.getPosition().y);
        particle.velocity.x *= -1 * particle.elastic_coefficient;
    }
    if(abs(particle.drawing.getPosition().y) > SCREEN_SIZE_Y - d){
        float y = SCREEN_SIZE_Y - d * Sign(particle.drawing.getPosition().y);
        particle.position.y = y;
        particle.drawing.setPosition(particle.drawing.getPosition().x, y);
        particle.velocity.y *= -1 * particle.elastic_coefficient;
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y), "FluidSimulation-SFML-CXX");
    window.setFramerateLimit(FPS_LIMIT);
    sf::Clock frameClock;

    Particle particle(PARTICLE_RADIUS, (window.getSize().x/2)-particle.drawing.getRadius(),(window.getSize().y/2)-particle.drawing.getRadius(), ELASTIC_COEFFICIENT, PARTICLE_MASS);

    while (window.isOpen()) {
        sf::Time frameTime = frameClock.restart(); // Get the time elapsed since the last frame
        float deltaTime = frameTime.asSeconds();

        window.clear(sf::Color::Black);

        updateParticle(particle.position, particle.velocity, deltaTime, particle);
        checkForCollisions(particle);

        window.draw(particle.drawing);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
        }
    }

    return 0;
}

