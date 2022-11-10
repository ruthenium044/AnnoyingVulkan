#pragma once
#include <memory>

#include "Descriptors.h"
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

        bool isRunning = true;

        Window window{WIDTH, HEIGHT};
        Device device{window};
        Renderer renderer{window, device};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObj::Map gameObjs;
    };
}
