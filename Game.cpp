#include "Game.h"
#include <iostream>

using namespace std;

Game::Game(const Board& b, Player* south, Player* north) :board(b) {
	//this->board = b;
	this->southPlayer = south;
	this->northPlayer = north;
	this->currentPlayer = south;
	this->m_turn = SOUTH;
}

void Game::display() const {
	int numHoles = board.holes(); //# of holes
	
	cout << "Player 1 (south): " << southPlayer->name() << endl;
	cout << "Player 2 (north): " << northPlayer->name() << endl;

	cout << "            "; 
	for (int i = 1; i <= numHoles; i++) 
	{
		cout << "    " << board.beans(NORTH, i) << "   ";
	}
	cout << endl; 
	cout << "North's pot  " << board.beans(NORTH, POT) << "                       " << board.beans(SOUTH, POT) << "  South's pot" << endl;
	cout << "            ";

	for (int i = 1; i <= numHoles; i++)
	{
		cout << "    " << board.beans(SOUTH, i) << "   ";
	}
	cout << endl;

}

void Game::status(bool& over, bool& hasWinner, Side& winner) const {
	//cerr << "Game Status" << endl;

	over = false;

	if ((board.beansInPlay(m_turn) == 0)) //if no beans, then game is over
	{ 
		over = true;
		if (beans(NORTH, POT) > beans(SOUTH, POT))
		{
			hasWinner = true;
			winner = NORTH;
		}
		else if (beans(NORTH, POT) < beans(SOUTH, POT))
		{
			hasWinner = true;
			winner = SOUTH;
		}
		else
		{
			hasWinner = false;
		}
	}


}

bool Game::move(Side s) {
	
	Side opp = opponent(s);

	Side endSide;
	int endHole;

	do 
	{
		if (board.beansInPlay(s) == 0) //if the current side has no beans, move all of the opp's into opp pot
		{
			for (int i = 1; i <= board.holes(); i++)
			{
				board.moveToPot(opp, i, opp);
			}
			display();
			cerr << "-------------------------------no beans" << endl << endl;
			return false;
		}
		
		int hole = currentPlayer->chooseMove(board, s);
		board.sow(s, hole, endSide, endHole);
		display();
		//cerr << "-------------------end side: " << endSide << "-------------------s: " << s << endl;
	} while (endSide == s && endHole == POT); //ends up in the pot

	//cerr << "-----------------beans(s, endHole): " << beans(s, endHole) << "-----------------beans(opp, endHole): " << beans(opp, endHole) << endl;

	if ((endSide == s) && (beans(s, endHole) == 1) && (beans(opp, endHole) != 0)) //if same side as player and opponent has beans in their hole, move the beans
	{
		board.moveToPot(opp, endHole, s);
		board.moveToPot(s, endHole, s);
		display();
		cerr << "Captured!" << endl << endl;
	}

	//display();

	if (currentPlayer == northPlayer)
	{
		currentPlayer = southPlayer; //if it was north, now it is south
		m_turn = SOUTH;
	}
	else
	{
		currentPlayer = northPlayer;
		m_turn = NORTH;
	}

	return true;
}

void Game::play() {
	bool over = false;
	bool hasWinner = false;
	Side winner;

	//status(over, hasWinner, winner);

	bool pressEnter = false;
	if (!northPlayer->isInteractive() && !southPlayer->isInteractive())// if both are interactive then you have to press enter
	{
		pressEnter = true; 
	}

	cout << "STARTING BOARD" << endl;
	display();
	cout << endl;
	while (!over)
	{
		cerr << "--------------------------------------" << endl;
		cerr << endl;
		if (m_turn == SOUTH)
		{
			cout << "It's SOUTH's turn!" << endl;
		}
		else if (m_turn == NORTH)
		{
			cout << "It's NORTH's turn!" << endl;
		}
		else
		{
			cout << "ya done goofed" << endl;
		}
		
		move(m_turn);
		
		status(over, hasWinner, winner);
		if (pressEnter)
		{
			cout << "Press ENTER to continue";
			cin.ignore();
		}

		cout << endl;
	}

	status(over, hasWinner, winner);

	if (hasWinner)
	{
		string name;
		if (winner == NORTH)
		{
			name = northPlayer->name();
		}
		else
		{
			name = southPlayer->name();
		}
		cout << "The winner is " << name << "!" << endl;
	}
	else if (!hasWinner)
	{
		cout << "The game resulted in a tie." << endl;
	}

}

int Game :: beans(Side s, int hole) const {
	return board.beans(s, hole);
}