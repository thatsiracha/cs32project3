#include "Board.h"

using namespace std;

Board::Board(int nHoles, int nInitialBeansPerHole) {
	if (nHoles <= 0) { // check nHoles
		nHoles = 1;
	}
	if (nInitialBeansPerHole < 0) { // check nInitialBeansPerHole
		nInitialBeansPerHole = 0;
	}

	this->m_Holes = nHoles;
	this->m_InitialBeansPerHole = nInitialBeansPerHole;
	this->kalah = vector<vector<int>>(2, vector<int>(nHoles, nInitialBeansPerHole));
	this->kalah[0].insert(kalah[0].begin(), POT);
	this->kalah[1].insert(kalah[1].begin(), POT);
	//this->kalah[0].push_back(0); // North's pot
	//this->kalah[1].push_back(0); // South's pot
}

Board::Board(const Board& other) : kalah(other.kalah) // copy constructor
{
	m_Holes = other.m_Holes;
	m_InitialBeansPerHole = other.m_InitialBeansPerHole;
}

int Board::holes() const {
	return m_Holes;
}

int Board::beans(Side s, int hole) const {
	if (hole == 0) { //for the pot
		return kalah[s][hole];
	}
	else if (hole < 0 || hole > holes()) // technically correct way for the holes, doesnt acc for pot tho
	{
		return -1;
	}
	else {
		return kalah[s][hole];
	}
}

int Board::beansInPlay(Side s) const {
	int total = 0;

	for (int i = 1; i <= holes(); i++)
	{
		total += beans(s, i);
	}

	return total;
}

int Board::totalBeans() const {
	int totalB = 0;

	for (int i = 0; i <= 1; i++)
	{
		for (int j = 0; j <= holes(); j++)
		{
			totalB += kalah[i][j];
		}
	}

	return totalB;
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole) {
	//If the hole indicated by (s,hole) is empty or invalid or a pot, this function returns false without changing anything.
	if ((s != NORTH && s != SOUTH) || (hole > holes() || hole <= 0))
	{
		return false;
	}

	//Otherwise, it will return true after sowing the beans: the beans are removed from hole (s,hole) and sown counterclockwise,
	//including s's pot if encountered, but skipping s's opponent's pot.

	int onHole = hole;
	int sowing = kalah[s][onHole]; //move the beans from hole to temp storage
	kalah[s][onHole] = 0; //no beans left in the hole

	int correctPot = s;
	while (sowing != 0)
	{
		if (s == NORTH) 
		{
			onHole--;
			if (onHole < 0) //if on the left side (past board)
			{
				onHole = 1;
				s = SOUTH; //south side, leftmost hole
				kalah[s][onHole] += 1;
				sowing--;
			}
			else if (onHole == 0 && correctPot != s)
			{
				continue; //don't put anything in the pot if it is the wrong side
			}
			else
			{
				kalah[s][onHole] += 1;
				sowing--;
			}
		}
		else if (s == SOUTH) //south side
		{
			if (onHole != 0)
			{
				onHole++;
				if (onHole > holes()) //if on the right side
				{
					onHole = 0;
					if (correctPot == s)
					{
						kalah[s][onHole] += 1; //into South pot
						sowing--;
					}

				}
				else
				{
					kalah[s][onHole] += 1;
					sowing--;
				}
			}
			else //when onHole == 0
			{
				s = NORTH;
				onHole = holes();
				kalah[s][onHole] += 1;
				sowing--;
			}
		}

	}

	//The function sets the parameters endSide and endHole to the side and hole where the last bean was placed.
	endSide = s;
	endHole = onHole;

	return true;
}

bool Board::moveToPot(Side s, int hole, Side potOwner) {
	if ((s != 0 && s != 1) || (hole > holes() || hole <= 0))
	{
		return false;
	}

	kalah[potOwner][POT] += kalah[s][hole];
	kalah[s][hole] = 0;

	return true;
}

bool Board::setBeans(Side s, int hole, int beans) {
	if ((s != 0 && s != 1) || (hole > holes() || hole < 0) || (beans < 0))
	{
		return false;
	}

	kalah[s][hole] = beans;
	return true;
}
