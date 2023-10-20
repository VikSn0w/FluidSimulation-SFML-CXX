#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Shader.hpp>
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
    sf::CircleShape circle;
    sf::RenderTexture renderTexture;
    sf::Sprite sprite;
    sf::Shader blurShader;
    Position position;
    Velocity velocity;
    double elastic_coefficient = 0.f;
    double mass = 0.f;

    // Constructor to initialize the particle
    Particle(double r, double x, double y, double ec, double m, double screen_x, double screen_y) : circle(r) {
        // Customize the particle's properties here if needed
        circle.setFillColor(sf::Color(14,135,204));
        circle.setPosition(x, y);
        elastic_coefficient = ec;
        position.x = x;
        position.y = y;
        mass = m;

        renderTexture.create(screen_x,screen_y);
        renderTexture.clear(sf::Color::Transparent);

        // Draw the circle shape to the render texture
        renderTexture.draw(circle);
        renderTexture.display();

        // Load the Gaussian blur shader
        if (!blurShader.loadFromFile("gaussian.frag", sf::Shader::Fragment))
        {
        }

        // Set the shader parameters
        blurShader.setUniform("source", renderTexture.getTexture());
        blurShader.setUniform("offsetFactor", sf::Vector2f(2.0f / 500.0f, 2.0f / 500.0f)); // Adjust this value as needed

        // Create a sprite to display the blurred texture
        sprite = *new sf::Sprite(renderTexture.getTexture());
    }

    void setPosition(double x, double y){
        circle.setPosition(x,y);
        sprite.setPosition(x,y);
        position.y = y;
        position.x = x;
    }

};

void updateParticle(Position &position, Velocity &velocity, double deltaTime, Particle &particle){
    velocity.y += (-GRAVITATIONAL_COSTANT*particle.mass) * -1 * deltaTime;
    position.y += velocity.y * deltaTime;
    particle.setPosition(particle.circle.getPosition().x, position.y);
    printf("y: %f\n", position.y);
};

void foo(Particle &particle,double x, double y){
    particle.setPosition(x,y);
    particle.position.y = y;
    particle.position.x = x;
}

void checkForCollisions(Particle &particle){
    double d = particle.circle.getRadius()*2;
    if(abs(particle.circle.getPosition().x) > SCREEN_SIZE_X - d){
        float x = SCREEN_SIZE_X*d * Sign(particle.circle.getPosition().x);
        particle.position.x = x;
        particle.circle.setPosition(x, particle.circle.getPosition().y);
        particle.velocity.x *= -1 * particle.elastic_coefficient;
    }
    if(abs(particle.circle.getPosition().y) > SCREEN_SIZE_Y - d){
        float y = SCREEN_SIZE_Y - d * Sign(particle.circle.getPosition().y);
        particle.position.y = y;
        particle.circle.setPosition(particle.circle.getPosition().x, y);
        particle.velocity.y *= -1 * particle.elastic_coefficient;
    }
}


int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16.0;
    sf::RenderWindow window(sf::VideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y), "FluidSimulation-SFML-CXX",  sf::Style::Close, settings);
    window.setFramerateLimit(FPS_LIMIT);
    sf::Clock frameClock;

    Particle particle(PARTICLE_RADIUS, 0,10, ELASTIC_COEFFICIENT, PARTICLE_MASS, window.getSize().x, window.getSize().y);

    Velocity velocity;
    Position position;

    while (window.isOpen()) {
        sf::Time frameTime = frameClock.restart(); // Get the time elapsed since the last frame
        float deltaTime = frameTime.asSeconds();

        updateParticle(particle.position, particle.velocity, deltaTime, particle);

        window.clear(sf::Color::Black);

        velocity.y += (-GRAVITATIONAL_COSTANT*particle.mass) * -1 * deltaTime;
        position.y += velocity.y * deltaTime;
        foo(particle, 0, position.y);
        printf("x: %f",particle.position.x);
        window.draw(particle.sprite, &particle.blurShader);
        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
        }
    }

    return 0;
}

