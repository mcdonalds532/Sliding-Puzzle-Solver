#include "SlidingPuzzleApp.h"

// Make sure manually change the dimensions of sf::VideoMode based on the chosen n.
SlidingPuzzleApp::SlidingPuzzleApp() : Application(sf::VideoMode({240, 240}, 32), "Sliding Puzzle App", *this)
{
    n = model.getN();
    if(n == 3)
    {
        dimensions = 796.f;
        imagePath = "3x3_Puzzle.jpg";
    }
    else if(n == 4)
    {
        dimensions = 240.f;
        imagePath = "4x4_Puzzle.jpg";
    }
    else if(n == 5)
    {
        dimensions = 321.f;
        imagePath = "5x5_Puzzle.jpg";
    }
}

void SlidingPuzzleApp::initialization()
{
    texture.loadFromFile(imagePath);
    ml::ImageRects rects = ml::TextureSlicer::getImageRects(texture, n, n);
    std::string startingBoard = model.getCurrentBoard();

    for(int i = 0; i < n; i++)
    {
        std::vector<ml::RectangleButton> row;
        for(int j = 0; j < n; j++)
        {
            const int tileNumber = startingBoard[i * n + j] - 'a';
            const int textureIndex_x = tileNumber / n;
            const int textureIndex_y = tileNumber % n;
            sf::IntRect intRect = rects.getIntRect(textureIndex_x, textureIndex_y);
            ml::RectangleButton gridBox;
            float boxWidth = dimensions / static_cast<float>(n);
            float boxHeight = dimensions / static_cast<float>(n);
            gridBox.setSize({boxWidth, boxHeight});
            gridBox.setTextureRect(intRect);
            gridBox.setTexture(&texture);
            gridBox.setPosition({static_cast<float>(j) * boxWidth, static_cast<float>(i) * boxHeight});
            row.push_back(gridBox);
        }
        grid.push_back(row);
    }

    for(auto& row : grid)
        for(auto& gridBox : row)
            addComponent(gridBox);

    SlidingPuzzleSolver solver(model);
    solver.solve();
    solutionSteps = model.getSolutionSteps();

    onUpdate([this]()
    {
        if(currentStep >= solutionSteps.size() - 1)
           return;
        if(clock.getElapsedTime().asSeconds() >= delay)
        {
            const int startingTileIndex = solutionSteps[currentStep];
            const int startingTileRow = startingTileIndex / n;
            const int startingTileColumn = startingTileIndex % n;
            const int nextTileIndex = solutionSteps[currentStep + 1];
            const int nextTileRow = nextTileIndex / n;
            const int nextTileColumn = nextTileIndex % n;
            const sf::Vector2f startingPosition = grid[startingTileRow][startingTileColumn].getPosition();
            const sf::Vector2f nextPosition = grid[nextTileRow][nextTileColumn].getPosition();
            grid[startingTileRow][startingTileColumn].setPosition(nextPosition);
            grid[nextTileRow][nextTileColumn].setPosition(startingPosition);
            std::swap(grid[startingTileRow][startingTileColumn], grid[nextTileRow][nextTileColumn]);
            currentStep++;
            clock.restart();
        }
    });

    std::cout << "Steps to solution: " << solutionSteps.size() << '\n';
    std::cout << "States explored: " << solver.getStatesExplored();
}

void SlidingPuzzleApp::registerEvents() {}