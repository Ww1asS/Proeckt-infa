#include "raylib.h"
#include <vector>
#include <cmath>

// Гравитационная постоянная (упрощённая для масштаба)
const double G = 6.67e-11;
// Масштаб расстояний (1 AU = 100 пикселей)
const double AU = 100.0;
// Масштаб времени
const double TIME_SCALE = 100000;

// Структура для небесного тела
struct Body {
    Vector2 position;   // Позиция (x, y)
    Vector2 velocity;   // Скорость
    double mass;       // Масса
    float radius;      // Радиус для отрисовки
    Color color;       // Цвет
    std::vector<Vector2> orbit; // След орбиты
};

// Класс для солнечной системы
class SolarSystem {
public:
    std::vector<Body> bodies;

    void addBody(Vector2 pos, Vector2 vel, double mass, float radius, Color color) {
        bodies.push_back({pos, vel, mass, radius, color});
    }

    void update(float dt) {
        // Рассчитываем силы гравитации (только от Солнца для простоты)
        std::vector<Vector2> forces(bodies.size(), {0, 0});
        Vector2 sunPos = bodies[0].position; // Солнце — первое тело
        double sunMass = bodies[0].mass;

        for (size_t i = 1; i < bodies.size(); ++i) { // Пропускаем Солнце
            Vector2 delta = {sunPos.x - bodies[i].position.x, sunPos.y - bodies[i].position.y};
            float dist = sqrtf(delta.x * delta.x + delta.y * delta.y);
            if (dist < 1.0f) dist = 1.0f; // Избегаем деления на 0
            float force = (float)(G * bodies[i].mass * sunMass / (dist * dist));
            Vector2 forceDir = {delta.x / dist, delta.y / dist};
            forces[i].x = force * forceDir.x / bodies[i].mass;
            forces[i].y = force * forceDir.y / bodies[i].mass;
        }

        // Обновляем позиции и скорости (метод Эйлера)
        for (size_t i = 1; i < bodies.size(); ++i) {
            bodies[i].velocity.x += forces[i].x * dt * TIME_SCALE;
            bodies[i].velocity.y += forces[i].y * dt * TIME_SCALE;
            bodies[i].position.x += bodies[i].velocity.x * dt * TIME_SCALE;
            bodies[i].position.y += bodies[i].velocity.y * dt * TIME_SCALE;
            // Сохраняем орбиту
            bodies[i].orbit.push_back(bodies[i].position);
            if (bodies[i].orbit.size() > 200) bodies[i].orbit.erase(bodies[i].orbit.begin());
        }
    }

    void draw() {
        // Рисуем орбиты
        for (size_t i = 1; i < bodies.size(); ++i) {
            if (bodies[i].orbit.size() > 1) {
                for (size_t j = 1; j < bodies[i].orbit.size(); ++j) {
                    DrawLineV(bodies[i].orbit[j - 1], bodies[i].orbit[j], Fade(bodies[i].color, 0.5f));
                }
            }
        }
        // Рисуем тела
        for (const auto& body : bodies) {
            DrawCircleV(body.position, body.radius, body.color);
        }
    }
};

int main() {
    // Устанавливаем уровень логирования
    SetTraceLogLevel(LOG_INFO);
    TraceLog(LOG_INFO, "Starting program...");

    // Настройки окна для macOS
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    TraceLog(LOG_INFO, "Initializing window...");
    const int screenWidth = 1080;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Solar System");
    TraceLog(LOG_INFO, "Window initialized successfully");
    SetTargetFPS(60);

    // Инициализация системы
    SolarSystem system;

    // Солнце (в центре, неподвижное)
    system.addBody({screenWidth / 2.0f, screenHeight / 2.0f}, {0, 0}, 1.0e6, 20.0f, YELLOW);

    // Планеты (расстояния в AU, массы относительны Земли, скорости для круговых орбит)
    struct Planet {
        float distance; // AU
        double mass;   // Относительно Земли
        float radius;  // Для отрисовки
        Color color;
    };
    std::vector<Planet> planets = {
        {0.39f, 0.055, 3.0f, GRAY},      // Меркурий
        {0.72f, 0.815, 4.0f, ORANGE},    // Венера
        {1.00f, 1.0, 5.0f, BLUE},        // Земля
        {1.52f, 0.107, 4.0f, RED},       // Марс
        {3.20f, 317.8, 10.0f, BROWN},    // Юпитер
        {4.58f, 95.2, 8.0f, GOLD},       // Сатурн
        {5.18f, 14.5, 7.0f, SKYBLUE},   // Уран
        {7.07f, 17.1, 7.0f, DARKBLUE}   // Нептун
    };

    for (const auto& planet : planets) {
        float dist = planet.distance * AU;
        float speed = sqrtf(G * 1.0e6/ dist); // Скорость для круговой орбиты
        system.addBody({screenWidth / 2.0f + dist, screenHeight / 2.0f}, {0, speed}, planet.mass, planet.radius, planet.color);
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        system.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        system.draw();
        DrawText("Solar System Simulation", 10, 10, 20, WHITE);
        DrawText("Planets: Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune", 10, 30, 10, WHITE);
        EndDrawing();
    }

    TraceLog(LOG_INFO, "Closing window...");
    CloseWindow();
    TraceLog(LOG_INFO, "Program terminated");
    return 0;
}
