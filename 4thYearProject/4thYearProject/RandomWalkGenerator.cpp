#include "RandomWalkGenerator.h"


RandomWalkGenerator::RandomWalkGenerator()
{

}
//Overloaded constructor
//Width: width of the level in number of tiles.
//Height: height of the level in number of tiles.

RandomWalkGenerator::RandomWalkGenerator(int width, int height) : m_width(width), m_height(height)
{
	//Add the original walker to the vector
	m_walkers.push_back(new RandomWalker(sf::Vector2i(thor::random(0, m_width), thor::random(0, m_height))));
	
	//Dynamically allocate 2d array of gridspaces.
	m_gridSpace = new GridSpace*[m_width];
	for (int i = 0; i < m_width; i++)
	{
		m_gridSpace[i] = new GridSpace[m_height];
	}


	//Initialize gridspace as entirely empty cells.
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			m_gridSpace[i][j] = GridSpace::empty;
		}
	}
}

void RandomWalkGenerator::generate(const int offsetX, const int offsetY, const int maxWalkers, const float fillPercentage, const float chanceToChangeDirection, const float chanceToDestroyWalker, const float chanceToSpawnWalker)
{
	//Number of times to automatically break of out loop
	int iterations = 0;

	//Assign variables to user-defined input
	m_offsetX = offsetX;
	m_offsetY = offsetY;
	MAX_WALKERS = maxWalkers;
	FILL_PERCENTAGE = fillPercentage;
	CHANCE_TO_CHANGE_DIRECTION = chanceToChangeDirection;
	CHANCE_TO_DESTROY = chanceToDestroyWalker;
	CHANCE_TO_SPAWN_NEW_WALKER = chanceToSpawnWalker;
	

	//Main loop
	//Each walker places a floor at its current position
	//Has a chance to be destroyed
	//Has a chance to spawn a new walker
	//Has a chance to change its direction
	//Moves
	//Repeat until max iterations reached or maximum defined fill percentage has been reached
	while(iterations < 5000)
	{
		//Create floor at the position of each walker.
		for (auto walker : m_walkers)
		{
			if (m_gridSpace[std::clamp(walker->getPosition().x, 1, m_width - 1)][std::clamp(walker->getPosition().y, 1, m_height - 1)] == GridSpace::empty)
			{
				m_gridSpace[std::clamp(walker->getPosition().x, 1, m_width - 1)][std::clamp(walker->getPosition().y, 1, m_height - 1)] = GridSpace::floor;
			}
		}

		destroyWalker();
		spawnNewWalker();
		calculateNewDirection();

		//Move each walker
		for (auto walker : m_walkers)
		{
			//move each walker
			walker->move();
		}

		//Check if the number of floors exceeds the fill percentage
		if ((float)(numberOfFloorsInGrid() / (float)(m_width * m_height)) > FILL_PERCENTAGE)
		{
			iterations = 5000;
		}

		iterations++;
	}
}

//Destroy a walker by removing it from the list
void RandomWalkGenerator::destroyWalker()
{
	std::list<RandomWalker*>::iterator iter;
	for (iter = m_walkers.begin(); iter != m_walkers.end(); ++iter)
	{
		//if number generated is less than our constant chance to destroy and there is more than 1 walker
		if (thor::random(0.0f, 1.0f) < CHANCE_TO_DESTROY && m_walkers.size() > 1)
		{
			m_walkers.remove(*iter);
			break;
		}
	}
}

void RandomWalkGenerator::spawnNewWalker()
{
	//If the random value is less than the chance to spawn a new one and there isnt already the maximum number of walkers
	std::list<RandomWalker*>::iterator iter;
	for (iter = m_walkers.begin(); iter != m_walkers.end(); ++iter)
	{
		if (thor::random(0.0f, 1.0f) < CHANCE_TO_SPAWN_NEW_WALKER && m_walkers.size() < MAX_WALKERS)
		{
			//add a new walker to the list, starting in the middle of the grid.
			//m_walkers.push_back(new RandomWalker(sf::Vector2i(m_width / 2, m_height / 2)));
			m_walkers.push_back(new RandomWalker((*iter)->getPosition()));
			
		}
	}
}
//Change directions of walkers
void RandomWalkGenerator::calculateNewDirection()
{
	std::list<RandomWalker*>::iterator iter;
	for (iter = m_walkers.begin(); iter != m_walkers.end(); ++iter)
	{
		if (thor::random(0.0f, 1.0f) < CHANCE_TO_CHANGE_DIRECTION)
		{
			(*iter)->setDirection((*iter)->getNewDirection());

		}
		
	}
}

//Count the number of floors in the 2d vector and return it
int RandomWalkGenerator::numberOfFloorsInGrid()
{
	int count = 0;
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			if (m_gridSpace[i][j] == GridSpace::floor)
			{
				count++;
			}
		}
	}
	return count;
}

//Create tiles
void RandomWalkGenerator::createTileArray(sf::Texture* floorTexture, sf::Texture* wallTexture)
{
	//Free up memory from old tiles
	m_tileVector.clear();

	m_tileVector.reserve(m_width);
	m_tileVector.resize(m_width);
	for (int i = 0; i < m_width; i++)
	{
		m_tileVector.at(i).reserve(m_height);
		m_tileVector.at(i).resize(m_height);
	}

	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			if (m_gridSpace[i][j] == GridSpace::empty)
			{
				m_tileVector[i].at(j) = std::make_shared<WallTile>(wallTexture, m_offsetX, m_offsetY, i, j);
			}

			if (m_gridSpace[i][j] == GridSpace::wall)
			{
				m_tileVector[i].at(j) = std::make_shared<WallTile>(wallTexture, m_offsetX, m_offsetY, i, j);
			}

			if (m_gridSpace[i][j] == GridSpace::floor)
			{
				m_tileVector[i].at(j) = std::make_shared<WallTile>(floorTexture, m_offsetX, m_offsetY, i, j);
			}
		}
	}
}

void RandomWalkGenerator::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			m_tileVector[i].at(j)->draw(window);
		}
	}
}

int RandomWalkGenerator::getHeight()
{
	return m_height;
}

int RandomWalkGenerator::getWidth()
{
	return m_width;
}

//Function to get the data as a binary format in a 2D vector
std::vector<std::vector<int>> RandomWalkGenerator::getData()
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
			if (m_gridSpace[i][j] == GridSpace::floor)
			{
				data[i].at(j) = 0;
			}

			else if (m_gridSpace[i][j] == GridSpace::wall || m_gridSpace[i][j] == GridSpace::empty)
			{
				data[i].at(j) = 1;
			}
		}
	}

	return data;
}