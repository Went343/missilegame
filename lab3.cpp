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
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

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

	if (id < t1) {

		team = 3;

	} else if (id < (t2+t1) && id >= t1) {

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

			bool shot = false;

			if (gameBoard[randRow][randCol] == 1 || gameBoard[randRow][randCol] == 2) {

				//Do nothing since they're starting positions

			} else if (gameBoard[randRow][randCol] == team) {

				gameBoard[randRow][randCol] = 0;

			} else {

				shot = true;
				gameBoard[randRow][randCol] = team;

			}

			int a = 0;
			int b = 0;

			if (shot) {

				for (int i = max(0, randRow - 1); i <= min(rows - 1, randRow + 1); i++) {

					for (int j = max(0, randCol - 1); j <= min(cols - 1, randCol + 1); j++) {

						if (gameBoard[i][j] == 1 || gameBoard[i][j] == 3) {

							a += 1;

						} else if (gameBoard[i][j] == 2 || gameBoard[i][j] == 4) {

							b += 1;

						}

					}

				}

				cout << "a:" << a << " b:" << b << endl;

				if (a > b) {

					for (int i = max(0, randRow - 1); i <= min(rows - 1, randRow + 1); i++) {

						for (int j = max(0, randCol - 1); j <= min(cols - 1, randCol + 1); j++) {

							if (gameBoard[i][j] == 4 || gameBoard[i][j] == 0) {

								if (team == 3) {

									gameBoard[i][j] = 3;

								}

							}

						}

					}

				} else if (b > a) {

					for (int i = max(0, randRow - 1); i <= min(rows - 1, randRow + 1); i++) {

						for (int j = max(0, randCol - 1); j <= min(cols - 1, randCol + 1); j++) {

							if (gameBoard[i][j] == 3 || gameBoard[i][j] == 0) {

								if (team == 4) {

									gameBoard[i][j] = 4;

								}

							}

						}

					}

				}

			}

			pthread_mutex_lock(&myMutex);
			print();
			pthread_mutex_unlock(&myMutex);

		}

		uniform_int_distribution<> slp(1, 3);
		sleep(slp(gen));

	}

	fflush(stdout);

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

				}
			}

		}

		if (!broken) {

			gameOver = true;

		}

	}

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

		bool placed = false;

		while (!placed) {

			uniform_int_distribution<> disR(0, rows-1);
			int randRow = disR(gen);
			uniform_int_distribution<> disC(0, cols-1);
			int randCol = disC(gen);

			if (gameBoard[randRow][randCol] == 0) {

				if (i < t1) {

					placed = true;
					gameBoard[randRow][randCol] = 1;

				} else if (i < (t1+t2) && i >= t1) {

					placed = true;
					gameBoard[randRow][randCol] = 2;

				}

			}

		}

	}

	for (int i = 0; i < totalPlayers; i++) {

		pthread_create(&playerThreads[i], 0, playerTurn, (void *)(long)i);

	}

	pthread_create(&playerThreads[totalPlayers], 0, supervisor, (void *)(long)totalPlayers);

	while(!gameOver) {

		sleep(1);

	}

	for(long i = 0; i == totalPlayers; i++) {

		pthread_join(playerThreads[i],NULL);

	}

	winCheck();

	return 0;
}
