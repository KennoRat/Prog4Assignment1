#pragma once
#include <chrono>
#include <vector>
#include <algorithm>

#include "imgui_plot.h"


namespace ImGui
{
    struct transform
    {
        float matrix[16] =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    };

    class GameObject3D
    {
    public:
        transform local{};
        int id{};
    };

    class GameObject3DAlt
    {
    public:
        transform* local{};
        int id{};
    };

	class imgui_ThrashTheCache
	{
	public:

        //Window
        void MeasureAndPlotInts();
        void MeasureAndPlotGameObjects();

    private:

        //Get data
        void MeasureInts(int samplesSize);
        void MeasureGameObjects(int samplesSize);
        void MeasureGameObjectsAlt(int samplesSize);
        float GetAverageTime(std::vector<long long> timings);

        //Draw
        void DrawPlot(float averageTime[11], float stepSizes[11], int idNumber, ImColor color = ImColor(255, 0, 0));
        void DrawCombinedPlot(float averageTime[11], float averageTime2[11], float stepSizes[11]);

        bool m_measurementDoneInts{ false };
        bool m_measurementDoneGameObject3D{ false };
        bool m_measurementDoneGameObject3DAlt{ false };
        const int DATA_SIZE{ 5'000'000 };
        int* m_samplesInt{new int (10)};
        int* m_samplesGameObject3D{ new int(100) };
        float m_averageTimeDataInts[11]{};
        float m_averageTimeDataGameObjects[11]{};
        float m_averageTimeDataGameObjectsAlt[11]{};
        float m_stepSizes[11]{};
	};
}


