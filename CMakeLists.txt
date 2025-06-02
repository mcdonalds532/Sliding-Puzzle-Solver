cmake_minimum_required(VERSION 3.14)
project(3x3_to_5x5_Sliding_Puzzle_Solver)

include(FetchContent)
FetchContent_Declare(
        malena
        GIT_REPOSITORY https://github.com/daversmith/Malena.git
        GIT_TAG v0.2.5
)
FetchContent_MakeAvailable(malena)

add_executable(3x3_to_5x5_Sliding_Puzzle_Solver
        main.cpp
        CompareNodes.h
        Node.h
        SlidingPuzzleApp.cpp
        SlidingPuzzleApp.h
        SlidingPuzzleSolver.cpp
        SlidingPuzzleSolver.h
        SubGoal.h
        Model.cpp
        Model.h
)

target_link_libraries(3x3_to_5x5_Sliding_Puzzle_Solver PRIVATE Malena::Malena)