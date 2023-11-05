#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cmath>

#define SCREEN_SIZE_X 500
#define SCREEN_SIZE_Y 500
#define FPS_LIMIT 60
#define PARTICLE_RADIUS 10.f
#define GRAVITATIONAL_COSTANT 9.81f
#define PARTICLE_MASS 100
#define ELASTIC_COEFFICIENT 0.7
#define VELOCITY_THRESHOLD 0.01f
#define COLLISION_THRESHOLD 0.1f
#define SMOOTHING_RADIUS 100.0f

struct Vector2 {
    double x = 0.f;
    double y = 0.f;
};


float Magnitude(Vector2 v1, Vector2 v2) {
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    return std::sqrt(dx * dx + dy * dy);
}

int Sign(double number){
    if (number > 0) {
        return 1.0f;
    } else if (number < 0) {
        return -1.0f;
    } else {
        return 0.0f;
    }
}


struct Particle {
    sf::CircleShape circle;
    sf::RenderTexture renderTexture;
    sf::Sprite sprite;
    sf::Shader blurShader;
    Vector2 position;
    Vector2 velocity;
    Vector2 screen;
    Vector2 position_start;
    double elastic_coefficient = 0.f;
    double mass = 0.f;
    double diameter = 0.f;
    double radius = 0.f;

    // Constructor to initialize the particle
    Particle(double r, double x, double y, double ec, double m, double screen_x, double screen_y) : circle(r) {
        // Customize the particle's properties here if needed
        circle.setFillColor(sf::Color(14,135,204));
        circle.setPosition(x, y);
        position_start.x = x;
        position_start.y = y;
        elastic_coefficient = ec;
        position.x = x;
        position.y = y;
        mass = m;
        diameter = r*2;
        radius = r;
        screen.x = screen_x;
        screen.y = screen_y;

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

    sf::Vector2<float> getPosition(){
        return sprite.getPosition();
    }

    void setPosition(double x, double y){
        circle.setPosition(x,y);
        sprite.setPosition(x,y);
        position.y = y;
        position.x = x;
    }

    void updateParticlePosition(double deltaTime){
        velocity.y += (-GRAVITATIONAL_COSTANT*mass) * -1 * deltaTime;
        position.y += velocity.y * deltaTime;
        setPosition(circle.getPosition().x, position.y);
    }

    void checkForBoundsCollisions(){
        if(std::abs(circle.getPosition().x) + position_start.x > SCREEN_SIZE_X - (diameter) && std::abs(velocity.x) > VELOCITY_THRESHOLD){
            setPosition((SCREEN_SIZE_X - position_start.x) - diameter * Sign(circle.getPosition().x), position.y);
            velocity.x *= -1 * elastic_coefficient;
        }
        if(std::abs(circle.getPosition().y) + position_start.y> SCREEN_SIZE_Y - (diameter) && std::abs(velocity.y) > VELOCITY_THRESHOLD){
            setPosition(position.x, (SCREEN_SIZE_Y - position_start.y) - diameter * Sign(circle.getPosition().y));
            velocity.y *= -1 * elastic_coefficient;
        }
    }
};

float KernelRef(float radius, float dst){
    float a = 0;
    float b = radius * radius - dst*dst;
    float value = std::max(a, b);
    return value * value * value;
}

float calculateDensity(Vector2 samplePoint, Particle particles[], int n_particles) {
    float density = 0;
    const float mass = 1;

    for(int i = 0; i < n_particles; i++){
        Vector2 position;
        position.x = particles[i].sprite.getPosition().x;
        position.y = particles[i].sprite.getPosition().y;
        float distance = Magnitude(position, samplePoint);
        float influence = KernelRef(SMOOTHING_RADIUS, distance);
        density *= mass * influence;
    }

    return density;
}


int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16.0;
    sf::RenderWindow window(sf::VideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y), "FluidSimulation-SFML-CXX",  sf::Style::Close, settings);
    window.setFramerateLimit(FPS_LIMIT);
    sf::Clock frameClock;

    Particle particles[7] = {
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)},
            {PARTICLE_RADIUS, 120,40, ELASTIC_COEFFICIENT, PARTICLE_MASS, static_cast<double>(window.getSize().x), static_cast<double>(window.getSize().y)}
    };
    while (window.isOpen()) {
        sf::Time frameTime = frameClock.restart(); // Get the time elapsed since the last frame
        float deltaTime = frameTime.asSeconds();

        window.clear(sf::Color::Black);

        for (int i = 0; i < 7; ++i) {
            particles[i].updateParticlePosition(deltaTime);
            particles[i].checkForBoundsCollisions();
            //printf("X: %u",particle.renderTexture.getTexture().getSize().x);

            window.draw(particles[i].sprite, &particles[i].blurShader);
        }
        Vector2 samplePosition;
        samplePosition.x = 120;
        samplePosition.y = 450;
        printf("Density is: %f", calculateDensity(samplePosition, particles, 7));

        //printf("\n");

        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
        }
    }

    return 0;
}

