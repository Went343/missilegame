#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <random>

using namespace std;

int** gameBoard;
int t1 = 0;
int t2 = 0;
int rows = 0;
int cols = 0;
int totalPlayers = 0;
bool gameOver = false;
random_device rd;
mt19937 gen(rd());

void print() {

	cout << endl;
	cout << "Current board:" << endl;

	for (int i = 0; i < rows; i++) {

		for (int j = 0; j < cols; j++) {

			if (gameBoard[i][j] == 0) {

				cout << "[ ]";

			} else if (gameBoard[i][j] == 1) {

				cout << "[A]";

			} else if (gameBoard[i][j] == 2) {

				cout << "[B]";

			} else if (gameBoard[i][j] == 3) {

				cout << "[a]";

			} else if (gameBoard[i][j] == 4) {

				cout << "[b]";

			} else {

				cout << "An error has occurred. Please try again." << endl;
				exit(-1);

			}

		}

		cout << endl;

	}

	cout << endl;

}

void winCheck() {

	int a = 0;
	int b = 0;

	for (int i = 0; i < rows; i ++) {

		for (int j = 0; j < cols; j++) {

			if (gameBoard[i][j] == 1 || gameBoard[i][j] == 3) {

				a += 1;

			} else if (gameBoard[i][j] == 2 || gameBoard[i][j] == 4) {

				b += 1;

			}

		}

	}

	cout << "Here is the final game board." << endl;
	print();

	if (a > b) {

		cout << "The winner of the game is Team A." << endl;

	} else if (b > a) {

		cout << "The winner of the game is Team B." << endl;

	} else if (b == a) {

		cout << "The game has ended in a tie." << endl;

	}

}

void* playerTurn(void * playerID) {

	int team = 0;
	int id = (int)(long)playerID;
	fflush(stdout);

	if (id < t1) {

		team = 3;

	} else if (id < t2 && id >= t1) {

		team = 4;

	}

	while (!gameOver) {

		uniform_int_distribution<> disR(0, rows-1);
		int randRow = disR(gen);

		uniform_int_distribution<> disC(0, cols-1);
		int randCol = disC(gen);

		if (gameBoard[randRow][randCol] == 1 || gameBoard[randRow][randCol] == 2) {

			cout << "Chose a starter space. Missile Wasted." << endl;

		} else {


			if (gameBoard[randRow][randCol] != team || gameBoard[randRow][randCol] != team - 2) {

				gameBoard[randRow][randCol] = team;

			} else {

				gameBoard[randRow][randCol] = 0;

			}

			int a = 0;
			int b = 0;
			int blank = 0;

			for (int i = 0; i < 3; i++) {

				for (int j = 0; j < 3; j++) {

					if (randRow != 0 && i != 0) {

						if (randRow != rows-1 && i != 2) {

							if (randCol != 0 && j != 0) {

								if (randCol != cols-1 && j != 2) {

									if (gameBoard[randRow + i - 1][randCol + j - 1] == 1 || gameBoard[randRow + i - 1][randCol + j - 1] == 3) {

										a += 1;

									} else if (gameBoard[randRow + i - 1][randCol + j - 1] == 2 || gameBoard[randRow + i - 1][randCol + j - 1] == 4) {

										b += 1;

									} else {

										blank += 1;

									}

								}

							}

						}

					}

				}

			}


			if (a > b && a > blank) {

				//Fill all spaces with a

			} else if (b > a && b > blank) {

				//Fill all spaces with b

			}

			print();

		}

		uniform_int_distribution<> slp(1, 3);
		sleep(slp(gen));

	}

	return (void*)0;

}

void* supervisor(void * superID) {

	int id = (int)(long)superID;
	fflush(stdout);

	while (!gameOver) {

		bool broken = false;

		for (int i = 0; i < rows; i++) {

			if (broken) {

				break;

			}

			for (int j = 0; j < cols; j++) {

				if (gameBoard[i][j] == 0) {

					broken = true;
					break;

				} else if (gameBoard[i][j] == 1) {

					continue;

				} else if (gameBoard[i][j] == 2) {

					continue;

				} else if (gameBoard[i][j] == 3) {

					continue;

				} else if (gameBoard[i][j] == 4) {

					continue;

				} else {

					system("Color 07");
					cout << "An error has occurred. Please try again." << endl;
					exit(-1);

				}

			}

		}

		if (!broken) {

			gameOver = true;

		}

		sleep(3);

	}


	sleep(4);

	winCheck();

	return (void*)0;
}

int main(int argc, char* argv[]) {

	// Check that the user provided the correct number of command line arguments
	if (argc != 5) {
		cout << " Here's the correct way to call the game: " << argv[0] << " <num_team_1_players> <num_team_2_players> <num_rows> <num_cols>" << endl;
		return 1;
	}

	t1 = atoi(argv[1]);
	t2 = atoi(argv[2]);
	rows = atoi(argv[3]);
	cols = atoi(argv[4]);
	totalPlayers = t1 + t2;

	int playerIDs[totalPlayers];
	int a = 1;

	for (int i = 0; i < totalPlayers; i++) {

		playerIDs[i] = a;
		a++;

	}

	pthread_t playerThreads[totalPlayers + 1];

    	gameBoard = new int*[rows];
	for (int i = 0; i < rows; i++) {
		gameBoard[i] = new int[cols];
	}

	for (int i = 0; i < rows; i++) {

		for (int j = 0; j < cols; j++) {

			gameBoard[i][j] = 0;

		}

	}

	for (int i = 0; i < totalPlayers; i++) {

		pthread_create(&playerThreads[i], 0, playerTurn, (void *)(long)i);

	}

	pthread_create(&playerThreads[totalPlayers], 0, supervisor, (void *)(long)totalPlayers);

	for(long i = 0; i == totalPlayers; i++) {

		pthread_join(playerThreads[i],NULL);

	}

	return 0;
}
