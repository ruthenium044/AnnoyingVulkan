#pragma once
#include <memory>

#include "GameObj.h"
#include "Model.h"
#include "Window.h"
#include "Renderer.h"

namespace svk {
    class TriangleApp {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        TriangleApp();
        ~TriangleApp();

        TriangleApp(const TriangleApp&) = delete;
        TriangleApp& operator=(const TriangleApp&) = delete;

        void run();
    private:
        void loadGameObjs();
        void LoadModels(const char* filepath);

        bool isRunning = true;

        Window window{WIDTH, HEIGHT};
        Device device{window};
        Renderer renderer{window, device};
        
        std::vector<GameObj> gameObjs;

        void mainLoop();
    };
}
