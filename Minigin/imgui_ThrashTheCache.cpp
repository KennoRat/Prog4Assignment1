#include "imgui_ThrashTheCache.h"

ImGui::imgui_ThrashTheCache::~imgui_ThrashTheCache()
{
    delete[] m_samplesInt;
    delete[] m_samplesGameObject3D;
}

void ImGui::imgui_ThrashTheCache::MeasureAndPlotInts()
{
    ImGui::Begin("Exercise 1 (Ints)", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Samples input
    ImGui::InputInt("Samples##samples_input_ints", m_samplesInt);
    // Button to start measurement
    if (ImGui::Button("Trash The Cache"))
    {
        MeasureInts(*m_samplesInt);
        m_measurementDoneInts = true;
    }

    if (m_measurementDoneInts)
    {
        DrawPlot(m_averageTimeDataInts, m_stepSizes, 0);
    }

    ImGui::End();
}

void ImGui::imgui_ThrashTheCache::MeasureAndPlotGameObjects()
{
    ImGui::Begin("Exercise 2 (GameObjects)", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Samples input
    ImGui::InputInt("Samples##samples_input_gameobjects", m_samplesGameObject3D);

    // Button for normal gameObject3D
    if (ImGui::Button("Trash The Cache with GameObject3D##1"))
    {
        MeasureGameObjects(*m_samplesGameObject3D);
        m_measurementDoneGameObject3D = true;
    }

    if (m_measurementDoneGameObject3D)
    {
        DrawPlot(m_averageTimeDataGameObjects, m_stepSizes, 1, ImColor(0, 255, 0));
    }

    // Button for alternate gameObject3D
    if (ImGui::Button("Trash The Cache with GameObject3DAlt##2"))
    {
        MeasureGameObjectsAlt(*m_samplesGameObject3D);
        m_measurementDoneGameObject3DAlt = true;
    }

    if (m_measurementDoneGameObject3DAlt)
    {
        DrawPlot(m_averageTimeDataGameObjectsAlt, m_stepSizes, 2, ImColor(150, 150, 250));
    }

    if(m_measurementDoneGameObject3D && m_measurementDoneGameObject3DAlt)
    {
        ImGui::Separator();
        ImGui::Text("Combined: ");
        DrawCombinedPlot(m_averageTimeDataGameObjects, m_averageTimeDataGameObjectsAlt, m_stepSizes);
    }

    ImGui::End();
}


void ImGui::imgui_ThrashTheCache::MeasureInts(int samplesSize)
{
    std::vector<int> vectorInts(DATA_SIZE, 1);
    int index{};

    for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<long long> timings;

        for (int sample = 0; sample < samplesSize; ++sample)
        {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < DATA_SIZE; i += stepsize)
            {
                vectorInts[i] *= 2;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            timings.push_back(elapsedTime);
        }

        m_averageTimeDataInts[index] = GetAverageTime(timings);
        m_stepSizes[index] = static_cast<float>(stepsize);
        ++index;
    }
}

void ImGui::imgui_ThrashTheCache::MeasureGameObjects(int samplesSize)
{
    std::vector<GameObject3D> vectorGameObject(DATA_SIZE);
    int index{};

    for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<long long> timings;

        for (int sample = 0; sample < samplesSize; ++sample)
        {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < DATA_SIZE; i += stepsize)
            {
                vectorGameObject[i].id *= 2;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            timings.push_back(elapsedTime);
        }

        m_averageTimeDataGameObjects[index] = GetAverageTime(timings);
        m_stepSizes[index] = static_cast<float>(stepsize);
        ++index;
    }
}

void ImGui::imgui_ThrashTheCache::MeasureGameObjectsAlt(int samplesSize)
{
    std::vector<GameObject3DAlt> vectorGameObjectAlt(DATA_SIZE);
    int index{};

    for (int stepsize = 1; stepsize <= 1024; stepsize *= 2)
    {
        std::vector<long long> timings;

        for (int sample = 0; sample < samplesSize; ++sample)
        {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < DATA_SIZE; i += stepsize)
            {
                vectorGameObjectAlt[i].id *= 2;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            timings.push_back(elapsedTime);
        }

        m_averageTimeDataGameObjectsAlt[index] = GetAverageTime(timings);
        m_stepSizes[index] = static_cast<float>(stepsize);
        ++index;
    }
}

float ImGui::imgui_ThrashTheCache::GetAverageTime(std::vector<long long> timings)
{
    // Remove highest and lowest value
    std::sort(timings.begin(), timings.end());
    timings.erase(timings.begin());       // Remove min
    timings.erase(timings.end() - 1);     // Remove max

    // Compute average time
    long long averageTime = 0;
    for (long long t : timings)
        averageTime += t;
    averageTime /= timings.size();

    return static_cast<float>(averageTime);
}

void ImGui::imgui_ThrashTheCache::DrawPlot(float averageTime[11], float stepSizes[11], int idNumber, ImColor color)
{
    const int stepAmount{ 11 };

    // Scale dynamically based on the data

    float minValue = *std::min_element(averageTime, averageTime + stepAmount);
    float maxValue = *std::max_element(averageTime, averageTime + stepAmount);

    if (minValue == maxValue) {
        maxValue += 1.0f;  // Prevent scale issue
    }

    // Configure plot
    ImGui::PlotConfig conf;
    conf.values.xs = stepSizes;
    conf.values.ys = averageTime;
    conf.values.count = stepAmount;
    conf.values.color = color;
    conf.scale.min = minValue * 0.9f;    // Scale smaller than min
    conf.scale.max = maxValue * 1.1f;    // Scale larger than max
    conf.tooltip.show = true;
    conf.frame_size = ImVec2(200, 150);

    // Draw the plot
    ImGui::Plot("Cache Thrashing Performance##" + idNumber, conf);
}

void ImGui::imgui_ThrashTheCache::DrawCombinedPlot(float averageTime[11], float averageTime2[11], float stepSizes[11])
{
    const float* y_data[] = { averageTime , averageTime2 };
    ImU32 colors[2] = { ImColor(0, 255, 0), ImColor(150, 150, 250) };
    const int stepAmount{ 11 };

    // Scale dynamically based on the data

    float minValue = *std::min_element(averageTime, averageTime + stepAmount);
    float maxValue = *std::max_element(averageTime, averageTime + stepAmount);

    float minElement = *std::min_element(averageTime2, averageTime2 + stepAmount);
    if (minValue > minElement)
    {
        minValue = minElement;
    }

    float maxElement = *std::max_element(averageTime2, averageTime2 + stepAmount);
    if (maxValue < maxElement)
    {
        maxValue = maxElement;
    }

    if (minValue == maxValue) {
        maxValue += 1.0f;  // Prevent scale issue
    }

    // Configure plot
    ImGui::PlotConfig conf;
    conf.values.xs = stepSizes;
    conf.values.ys_list = y_data;
    conf.values.ys_count = 2;
    conf.values.count = stepAmount;
    conf.values.colors = colors;
    conf.scale.min = minValue * 0.9f;    // Scale smaller than min
    conf.scale.max = maxValue * 1.1f;    // Scale larger than max
    conf.tooltip.show = true;
    conf.frame_size = ImVec2(200, 150);

    // Draw the plot
    ImGui::Plot("Combined Cache Thrashing Performance", conf);
}

