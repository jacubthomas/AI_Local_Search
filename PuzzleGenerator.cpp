#include "PuzzleGenerator.h"
#include <limits>
#include <cmath>

using namespace std;

PuzzleGenerator::PuzzleGenerator(int _nRows, int _nColumns, int _minVal, int _maxVal)
	:nRows(_nRows), nColumns(_nColumns), minVal(_minVal), maxVal(_maxVal)
{}

Puzzle PuzzleGenerator::GeneratePuzzle()
{
	timer.StartTimer();
	maxtime = 30;	// To make sure we don't exceed a minute
	
	double random_walk_time = 10;	// 5 seconds.
	
//	return RandomWalk(random_walk_time);	// Do a random walk for some time and return the solution.
    return hillClimb(random_walk_time);

}

Puzzle PuzzleGenerator::RandomWalk(double timelimit)
{
	// A very simple function that start at a random configuration and keeps randomly modifying it
	// until t hits the time limit. Returns the best solution found so far.

	Puzzle p(nRows, nColumns, minVal, maxVal);	// Generate a random puzzle with the specified values.
	
	// Keep track of the best puzzle found so far (and its value).
	Puzzle bestPuzzle = p;			
	int bestValue = p.GetValue();
	
	// Keep track of the time so we don't exceed it.
	Timer t;
	t.StartTimer();
	
	// Loop until we hit the time limit.
	while (t.GetElapsedTime() < timelimit-0.1)	// To make sure that we don't exceed the time limit, we stop just before we hit the time limit.
	{
		// Generate a successor of p by randomly changing the value of a random cell 
		// (since we are doing a random walk, we just replace p with its successor).
		p = p.GetRandomSuccessor();	
		int value = p.GetValue();
		
		// Update the current best solution.
		if (value > bestValue)	// Calling GetValue() for the first time is costly
										// since the puzzle has to be evaluated first.
		{
//            std::cout << "value: " << value << ", bestvalue: " << bestValue << std::endl;
			bestValue = value;	// Calling it a second time simply returns the value that was computed before.
			bestPuzzle = p;
		}
	}
	
	return bestPuzzle;
	
	// The following code is not executed in this function. It exists just as an example for getting all the successors of a puzzle.
	vector<Puzzle> successors;
	bestPuzzle.GetAllSuccessors(successors);
}

Puzzle PuzzleGenerator::hillClimb(double timelimit)
{
    Puzzle p(nRows, nColumns, minVal, maxVal);    // Generate a random puzzle with the specified values.
   
    // Keep track of the best puzzle found so far (and its value).
    Puzzle bestPuzzle = p;
    int bestValue = p.GetValue();
    
    // look at all successors from current puzzle
    vector<Puzzle> successors;
    GetSomeSuccessors(successors, p, nRows*nColumns);
    
    // Keep track of the time so we don't exceed it.
    Timer t;
    t.StartTimer();
    int count = 0;
    int plateau = 0;
    while (t.GetElapsedTime() < timelimit-0.1)    // To make sure that we don't exceed the time limit, we stop just before we hit the time limit.
    {
        if(plateau > 1000){
            p = Puzzle(nRows, nColumns, minVal, maxVal);
            plateau = 0;
        }
        // Generate a successor of p by randomly changing the value of a random cell
        // (since we are doing a random walk, we just replace p with its successor)
        if(count < 10){
            p = GetClimbingSuccessor(successors, p, bestValue);
            GetSomeSuccessors(successors, p, nRows*nColumns);
        }
        else
        {
            p = p.GetRandomSuccessor();
            while(!ProbabilityGenerator(bestValue,p.GetValue())){
                p = p.GetRandomSuccessor();
            }
            count = 0;
            GetSomeSuccessors(successors, p, nRows*nColumns);
        }
        int value = p.GetValue();
        
        // Update the current best solution.
        if (value > bestValue)    // Calling GetValue() for the first time is costly
                                        // since the puzzle has to be evaluated first.
        {
            bestValue = value;    // Calling it a second time simply returns the value that was computed before.
            bestPuzzle = p;
            count = 0;
        } else if(value == bestValue)
            count++;
        plateau++;
    }
    
    return bestPuzzle;
}

bool PuzzleGenerator::ProbabilityGenerator(int bestvalue, int potential)
{
    int numerator = (nRows * nColumns * 4);// + bestvalue;
    int delta = potential - bestvalue;
    if(delta >= 0)
        return pCoinFlip(1.0);
    int denom = (bestvalue % numerator) + (bestvalue / numerator);
    double Tau = exp(delta/denom);
        return pCoinFlip(Tau);
}
bool PuzzleGenerator::pCoinFlip(double probability)
{
    int coin = rand()%100+1;
    double prob = probability * 100;
    if(coin <= prob)
        return true;
    return false;
}

Puzzle PuzzleGenerator::GetClimbingSuccessor(std::vector<Puzzle> successors, Puzzle p, int bestvalue)
{
    int i = 0;
    Puzzle nextpuzzle = successors.at(i);
    while(nextpuzzle.GetValue() < bestvalue){
        nextpuzzle = successors.at(i);
        if (i > 20)
            return p.GetRandomSuccessor();
        i++;
    }
    return nextpuzzle;
}

void PuzzleGenerator::GetSomeSuccessors(std::vector<Puzzle> & successors, Puzzle p, int size)
{
    successors.clear();
    
    // Want to produce only 20*5 (random) successor puzzles to speed things up.
    int randos;
    if(size < 50) randos = 20;
    else randos = 10;
        
    for (int i = 0; i < randos; i++)    // Do not modify the goal!
    {
        for (int v = minVal; v <= maxVal; v++)
        {
            Puzzle p2 = p.GetRandomSuccessor();
            successors.push_back(p2);
        }
    }
}
