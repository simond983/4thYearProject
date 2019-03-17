#include "Grid.h"


//Default constructor
Grid::Grid()
{

}

//\param randomWalkGenerator : pointer to RandomWalkGenerator
//\param cellularAutomataGenerator : pointer to CellularAutomataGenerator
Grid::Grid(RandomWalkGenerator* randomWalkGenerator, CellularAutomataGenerator* cellularAutomataGenerator, int index_X, int index_Y) : m_index_X(index_X), m_index_Y(index_Y)
{
	//Store pointers to generators
	m_pCellularAutomataGenerator = cellularAutomataGenerator;
	m_pRandomWalkGenerator = randomWalkGenerator;

	//Calculate overall width and height
	m_width = randomWalkGenerator->getWidth() + cellularAutomataGenerator->getWidth();
	m_height = randomWalkGenerator->getHeight() + cellularAutomataGenerator->getHeight();

	//Reserve memory for containers
	m_dataVector.resize(m_height);
	for (int i = 0; i < m_dataVector.size(); i++)
	{
		m_dataVector[i].resize(m_width);
	}

	m_tileVector.resize(m_height);
	m_tileVector.reserve(m_height);
	for (int i = 0; i < m_tileVector.size(); i++)
	{
		m_tileVector[i].resize(m_height);
		m_tileVector[i].reserve(m_height);
	}
	
	
	m_pCellularAutomataGenerator->getData();
}

void Grid::generate()
{
	//Get information from both generators and put into m_dataVector, which has been allocated space for both.
	for (int i = 0; i < m_pRandomWalkGenerator->getHeight(); i++)
	{
		for (int j = 0; j < m_pRandomWalkGenerator->getWidth(); j++)
		{
			m_dataVector[i].at(j) = m_pRandomWalkGenerator->getData()[i].at(j);
		}
	}

	for (int i = m_index_Y; i < m_index_Y + m_pCellularAutomataGenerator->getHeight(); i++)
	{
		for (int j = m_index_X; j < m_index_X + m_pCellularAutomataGenerator->getWidth(); j++)
		{
			m_dataVector[i].at(j) = m_pCellularAutomataGenerator->getData()[i].at(j - m_pCellularAutomataGenerator->getWidth());
		}
	}
}

void Grid::createTiles()
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			if (m_dataVector[i].at(j) == 1)
			{
				m_tileVector[i].at(j) = new WallTile(i, j);
			}

			if (m_dataVector[i].at(j) == 0)
			{
				m_tileVector[i].at(j) = new FloorTile(i, j);
			}
		}
	}

}

void Grid::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_tileVector[i].at(j)->draw(window);
		}
	}
}

int Grid::getWidth()
{
	return m_width;
}

int Grid::getHeight()
{
	return m_height;
}

std::vector<std::vector<int>> Grid::getData()
{
	return m_dataVector;
}