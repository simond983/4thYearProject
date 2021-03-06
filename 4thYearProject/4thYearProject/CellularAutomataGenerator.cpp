#include "CellularAutomataGenerator.h"

CellularAutomataGenerator::CellularAutomataGenerator()
{
}

CellularAutomataGenerator::CellularAutomataGenerator(int width, int height, int numberOfSimulationSteps) : m_width(width), m_height(height), m_numSimulationSteps(numberOfSimulationSteps)
{
	//Initialize vector 

	m_chanceStartAlive = 0.40f;
	m_birthLimit = 4;
	m_deathLimit = 3;
	grid.reserve(m_width);
	grid.resize(m_width);
	m_tileVector.reserve(m_width);
	m_tileVector.resize(m_width);
	for (int i = 0; i < m_width; i++)
	{
		grid.at(i).reserve(m_height);
		grid.at(i).resize(m_height);
		m_tileVector.at(i).reserve(m_height);
		m_tileVector.at(i).resize(m_height);
	}

}

auto CellularAutomataGenerator::doSimulationStep()
{
	//Create a new temp grid for storing new information during the simulation step.
	std::vector<std::vector<std::shared_ptr<GridSpace>>> newGrid;
	newGrid = grid;
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			int neighbours = countAliveNeighbours(grid, i, j);

			if (*grid[i].at(j) == GridSpace::alive)
			{
				if (neighbours < m_deathLimit)
				{
					newGrid[i].at(j) = std::make_shared<GridSpace>(GridSpace::dead);
				}

				else
				{
					newGrid[i].at(j) = std::make_shared<GridSpace>(GridSpace::alive);
				}
			}

			else //The cell is currently dead
			{
				if (neighbours > m_birthLimit)
				{
					newGrid[i].at(j) = std::make_shared<GridSpace>(GridSpace::alive);
				}

				else
				{
					newGrid[i].at(j) = std::make_shared<GridSpace>(GridSpace::dead);
				}
			}
		}
	}

	return newGrid;

}

void CellularAutomataGenerator::generate(const int offsetX, const int offsetY, int numberOfSimulationSteps, int birthLimit, int deathLimit, float chanceStartAlive)
{
	//Assign variables to user-defined data
	m_offsetX = offsetX;
	m_offsetY = offsetY;
	m_numSimulationSteps = numberOfSimulationSteps;
	m_birthLimit = birthLimit;
	m_deathLimit = deathLimit;
	m_chanceStartAlive = chanceStartAlive;

	//Initialize the grid with each cell being randomly selected to be dead or alive
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			if (thor::random(0.0f, 1.0f) <= m_chanceStartAlive)
			{
				grid[i].at(j) = std::make_shared<GridSpace>(GridSpace::alive);
			}

			else
			{
				grid[i].at(j) = std::make_shared<GridSpace>(GridSpace::dead);
			}

		}
	}

	for (int i = 0; i < m_numSimulationSteps - 1; i++)
	{
		grid = doSimulationStep();
	}
}

//Counts the neighbours around the cell at index x, y in the 2d vector
//\param grid : 2D vector of gridspaces
//\param x : current index X
//\param y : current index Y
int CellularAutomataGenerator::countAliveNeighbours(std::vector<std::vector<std::shared_ptr<GridSpace>>> grid, int x, int y)
{
	int count = 0;	
	
	if (x - 1 < 0 || y - 1 < 0 || x >= m_width || y >= m_height)
	{
		count++;
	}

	if (*grid[std::clamp((x - 1), 0, m_width - 1)].at(std::clamp((y - 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[x].at(std::clamp((y - 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[std::clamp(x + 1, 0, m_width - 1)].at(std::clamp((y - 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[std::clamp(x + 1, 0, m_width - 1)].at(y) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[std::clamp(x + 1, 0, m_width - 1)].at(std::clamp((y + 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[x].at(std::clamp((y + 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[std::clamp(x - 1, 0, m_width - 1)].at(std::clamp((y + 1), 0, m_height - 1)) == GridSpace::alive)
	{
		count++;
	}

	if (*grid[std::clamp(x - 1, 0, m_width - 1)].at(y) == GridSpace::alive)
	{
		count++;
	}
	return count;
}

void CellularAutomataGenerator::createTileArray(sf::Texture* floorTexture, sf::Texture* wallTexture)
{

	//Clear old data from vector and free memory
	m_tileVector.clear();

	m_tileVector.reserve(m_width);
	m_tileVector.resize(m_width);
	for (int i = 0; i < m_width; i++)
	{
		m_tileVector.at(i).reserve(m_height);
		m_tileVector.at(i).resize(m_height);
	}

	//Create tiles based on generated data
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			if (*grid[i].at(j) == GridSpace::alive)
			{
				m_tileVector[i].at(j) = std::make_shared<WallTile>(wallTexture, m_offsetX, m_offsetY, i, j);
			}

			else if (*grid[i].at(j) == GridSpace::dead)
			{
				m_tileVector[i].at(j) = std::make_shared<FloorTile>(floorTexture, m_offsetX, m_offsetY, i, j);
			}
		}
	}
}

void CellularAutomataGenerator::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			m_tileVector[i].at(j)->draw(window);
		}
	}
}

CellularAutomataGenerator::~CellularAutomataGenerator()
{
}

int CellularAutomataGenerator::getWidth()
{
	return m_width;
}

int CellularAutomataGenerator::getHeight()
{
	return m_height;
}

//Writes data to binary format and returns in a 2D vector
std::vector<std::vector<int>> CellularAutomataGenerator::getData()
{
	std::vector<std::vector<int>> data;
	data.reserve(m_height);
	data.resize(m_height);
	for (int i = 0; i < m_height; i++)
	{
		data[i].reserve(m_width);
		for (int j = 0; j < m_width; j++)
		{
			data[i].push_back(0);
		}

	}

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			if (*grid[i].at(j) == GridSpace::dead)
			{
				data[i].at(j) = 0;
			}

			else if (*grid[i].at(j) == GridSpace::alive)
			{
				data[i].at(j) = 1;
			}
		}
	}

	return data;
}
