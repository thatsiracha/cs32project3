#include "Player.h"
#include <iostream>

using namespace std;

Player::Player(std::string name) {  
	this->m_name = name;
}

std::string Player::name() const {
	return m_name;
}

bool Player::isInteractive() const {
	return false;
}

Player :: ~Player() {

}

HumanPlayer::HumanPlayer(std::string name) : Player(name) {} //constructor

bool HumanPlayer::isInteractive() const { // return true if people (this player) make their own moves
	return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const { // select which hole to pck of beans and play the game
	int hole = -1;
	do {
		cout << "Please choose a hole from 1 to " << b.holes() << endl;
		cin >> hole;

	} while ((hole <= 0 || hole > b.holes()) || (b.beans(s, hole)) <= 0); // make sure they selected a valid hole

	return hole;
}

HumanPlayer :: ~HumanPlayer() {

}

BadPlayer::BadPlayer(std::string name) : Player(name) {} //constructor

int BadPlayer::chooseMove(const Board& b, Side s) const {
	for (int i = 1; i <= b.holes(); i++) {
		if (b.beans(s, i) > 0) {
			return i;
		}
	}
	return -1; // No valid move
}
	
BadPlayer :: ~BadPlayer() {

}

SmartPlayer::SmartPlayer(std::string name) : Player(name) {} //constructor

int SmartPlayer::chooseMove(const Board& b, Side s) const {
	int bestHole = 1;
	int highestVal = -10002;
	int newVal = 1;



    if (b.beansInPlay(s) == 0) // make sure there are beans on their side of the board
    {
        return -1;
    }

    double timeLimit = 4990;  // 4.99 seconds; allow 10 ms for wrapping up
    JumpyTimer jt(1000);


	for (int i = 1; i <= b.holes(); i++)
	{
		if (b.beans(s, i) > 0)
		{
            
			Board tBoard(b); // make a temporary board
			bool canSow = sowable(tBoard, i, s); //sow the beans and determine pot value
			bool maxTurn;

			if (canSow)
			{
				maxTurn = true;
				newVal = minmax(tBoard, s, maxTurn, m_depth, jt); // m_depth is 7
			}
			else
			{
				maxTurn = false;
				newVal = minmax(tBoard, s, maxTurn, m_depth, jt);
			}

			if (newVal >= highestVal)
			{
				highestVal = newVal;
				bestHole = i; // if a hole provides a better outcome many move in the future, set this playing hole to that one
			}
		}
	}

//	cerr << this->name() << " chooses hole: " << bestHole << endl;

    //cerr << "Time Elapsed: " << jt.actualElapsed() << endl;

	return bestHole; //bestHole
}

bool SmartPlayer::sowable(Board& board, int move, Side side) const
{
	Side endSide;
	int endHole;

	bool didSow = board.sow(side, move, endSide, endHole);
	if (!didSow)
	{
		return false;// if you cannot sow
	}

	//do capture
	if (side == endSide && board.beans(endSide, endHole) == 1 && board.beans(opponent(endSide), endHole) > 0) // do a capture
	{
		board.moveToPot(side, endHole, side);
		board.moveToPot(opponent(side), endHole, side);
		return false;
	}
	//can it make another turn?
	if (endHole == POT &&
		endSide == side)
	{
		return true;
	}

	return false; // can't make another turn
}

int SmartPlayer::minmax(const Board& board, Side side, bool isMax, int depth, JumpyTimer& jt) const
{
    int bestHoleVal;

    unsigned long callCount = 1;
    if (depth == 0)
        callCount += 0;  /// do a base case computation
    else
    {

    }
    //    if no move for player exists (i.e., game is over),
    //    or if the criterion says we should not search below this node
    if (board.beansInPlay(side) == 0 || board.beansInPlay(opponent(side)) == 0 || depth == 0)
    {
        return eval(board, side);
    }
    if (depth < 0)
    {
        exit(13);
    }
    if (jt.elapsed())
    {
        //cerr << "time\n";
        return eval(board, side);
    }

    if (isMax)
    {
        bestHoleVal = -10001;
        for (int i = 1; i <= board.holes(); i++)// loop through possible moves
        {
            if (jt.elapsed())
            {
                
            }
            if (board.beans(side, i) > 0)
            {
                Board tBoard(board);
                bool canSowAgain = sowable(tBoard, i, side);// make move
                bool oppIsMax;
                int newMaxHole;
                if (jt.elapsed())
                {
                    return eval(board, side);
                }
                if (canSowAgain) // can take another turn
                {
                    oppIsMax = true;
                    newMaxHole = minmax(tBoard, side, oppIsMax, depth - 1, jt);
                }
                else // you cannot take another turn
                {
                    oppIsMax = false;
                    newMaxHole = minmax(tBoard, opponent(side), oppIsMax, depth - 1, jt);
                }
                if (bestHoleVal <= newMaxHole) // if it is a new max val
                {
                    bestHoleVal = newMaxHole;
                }
            }
        }
    }
    else // is minimizer
    {
        bestHoleVal = 10001;
        for (int i = 1; i <= board.holes(); i++)// loop through possible moves
        {
            if (jt.elapsed())
            {
                return eval(board, side);
            }
            if (board.beans(side, i) > 0)
            {
                Board tBoard(board);
                bool canSowAgain = sowable(tBoard, i, side);
                bool oppMaxTurn;
                int newMinHole;
                if (jt.elapsed())
                {
                    return eval(board, side);
                }
                if (canSowAgain) // if you can take another turn
                {
                    oppMaxTurn = false;
                    newMinHole = minmax(tBoard, opponent(side), oppMaxTurn, depth - 1, jt);
                }
                else // you cannot take another turn
                {
                    oppMaxTurn = true;
                    newMinHole = minmax(tBoard, opponent(side), oppMaxTurn, depth - 1, jt);
                }

                if (bestHoleVal >= newMinHole) // if it is a new min value
                {
                    bestHoleVal = newMinHole;
                }
            }
        }
    }
    //    std::cout << bestHoleVal << '\n';
    return bestHoleVal;
}

int SmartPlayer::eval(const Board& board, Side side) const
{
    if (board.beansInPlay(side) <= 0 || board.beansInPlay(opponent(side)) <= 0)
    {
        if (board.beans(side, POT) > board.beans(opponent(side), POT))
        {
            return 10000; //this side won;
        }
        else if (board.beans(side, POT) < board.beans(opponent(side), POT))
        {
            return -10000; //other side won
        }
        else
        {
            return 0; //tie
        }
    }
    else //game not over
    {
        if (board.beans(side, POT) > board.totalBeans() / 2)
        {
            return 10000;
        }
        else if (board.beans(opponent(side), POT) > board.totalBeans() / 2)
        {
            return -10000;
        }
    }
        
        //return difference between pots
    return (board.beans(side, 0) - board.beans(opponent(side), 0)) * (board.beans(side, 0) - board.beans(opponent(side), 0));
}


SmartPlayer :: ~SmartPlayer() {

}