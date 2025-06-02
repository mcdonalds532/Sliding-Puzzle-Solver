#ifndef SLIDINGPUZZLEAPP_H
#define SLIDINGPUZZLEAPP_H
#include "Malena/Utilities/TextureSlicer.h"
#include "Malena/Utilities/ImageRects.h"
#include "SlidingPuzzleSolver.h"
#include <Malena/common.hpp>

class SlidingPuzzleApp final : public ml::Application
{
public:
    SlidingPuzzleApp();
    void initialization() override;
    void registerEvents() override;
private:
    std::vector<std::vector<ml::RectangleButton>> grid;
    std::vector<int> solutionSteps;
    std::string imagePath;
    sf::Texture texture;
    int currentStep = 0;
    float delay = 0.25f;
    float dimensions;
    sf::Clock clock;
    Model model;
    int n;
};

#endif //SLIDINGPUZZLEAPP_H