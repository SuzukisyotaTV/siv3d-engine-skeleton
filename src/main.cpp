#include <Siv3D.hpp> // Siv3D main header
#include "ecs.hpp"

struct Position { double x, y; };
struct Velocity { double vx, vy; };
struct RenderCircle { double radius; Color color; };

int main()
{
    Window::Resize(800, 600);
    Scene::SetBackground(ColorF(0.12, 0.12, 0.15));

    World world;

    const int N = 400;
    for (int i = 0; i < N; ++i) {
        Entity e = world.createEntity();
        double x = Random(50.0, 750.0);
        double y = Random(50.0, 550.0);
        world.addComponent<Position>(e, { x, y });
        world.addComponent<Velocity>(e, { Random(-80.0, 80.0), Random(-80.0, 80.0) });
        world.addComponent<RenderCircle>(e, { 6.0, Palette::White });
    }

    bool paused = false;
    while (System::Update())
    {
        if (KeySpace.down()) paused = !paused;
        if (KeyR.down()) {
            auto posArr = world.getComponentArray<Position>();
            if (posArr) {
                for (auto& p : posArr->data) {
                    p.x = Random(50.0, 750.0);
                    p.y = Random(50.0, 550.0);
                }
            }
        }

        if (!paused) {
            auto posArr = world.getComponentArray<Position>();
            auto velArr = world.getComponentArray<Velocity>();
            if (posArr && velArr) {
                size_t m = std::min(posArr->data.size(), velArr->data.size());
                for (size_t i = 0; i < m; ++i) {
                    posArr->data[i].x += velArr->data[i].vx * Scene::DeltaTime();
                    posArr->data[i].y += velArr->data[i].vy * Scene::DeltaTime();
                    if (posArr->data[i].x < 10 || posArr->data[i].x > 790) velArr->data[i].vx *= -1;
                    if (posArr->data[i].y < 10 || posArr->data[i].y > 590) velArr->data[i].vy *= -1;
                }
            }
        }

        auto posArr = world.getComponentArray<Position>();
        auto rendArr = world.getComponentArray<RenderCircle>();
        if (posArr && rendArr) {
            size_t m = std::min(posArr->data.size(), rendArr->data.size());
            for (size_t i = 0; i < m; ++i) {
                Circle(posArr->data[i].x, posArr->data[i].y, rendArr->data[i].radius).draw(rendArr->data[i].color);
            }
        }

        FontAsset(U"ui", 18).draw(U"Space: Pause / R: Re-randomize", 10, 10, Palette::White);
        FontAsset(U"ui", 18).draw(Format(U"Entities: {}", world.entityCount()), 10, 30, Palette::White);
    }

    return 0;
}