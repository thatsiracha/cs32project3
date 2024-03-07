#include "Player.h"
#include "Board.h"
#include "Side.h"
#include "Game.h"
#include <iostream>
#include <cassert>
using namespace std;

void doPlayerTests()
{
	HumanPlayer hp("Marge");
	assert(hp.name() == "Marge" && hp.isInteractive());
	BadPlayer bp("Homer");
	assert(bp.name() == "Homer" && !bp.isInteractive());
	SmartPlayer sp("Lisa");
	assert(sp.name() == "Lisa" && !sp.isInteractive());
	Board b(3, 2);
	b.setBeans(SOUTH, 2, 0);
	cout << "=========" << endl;
	int n = hp.chooseMove(b, SOUTH);
	cout << "=========" << endl;
	assert(n == 1 || n == 3);
	n = bp.chooseMove(b, SOUTH);
	assert(n == 1 || n == 3);
	n = sp.chooseMove(b, SOUTH);
	assert(n == 1 || n == 3);
}

int main()
{
	// doPlayerTests();
	// cout << "Passed all tests" << endl; 

	Board b(3, 3);
	HumanPlayer hp("Suraj");
	SmartPlayer sp("Computer");

	Game mancala(b, &hp, &sp);

	mancala.play();

	return 0;
}