#include "shell.h"


using namespace std;

void shell()
{
	string input, cmd;
	vector<string> tokens;
	Create("init", 0);
	//cout << "shell> ";
	while(getline(cin, input)) {
		if (input == "quit")
			exit(0);

		if (input == "") {
			continue;
		}
		
		tokens = tokenize(input);
		if (tokens.size() > 0)
			cmd = toLower(tokens[0]);
		if (cmd == "cr") {
			if (tokens.size() != 3)
				printError();
			else {
				try {
					int priority = stoi(tokens[2]);
					if (priority == 0)
						printError();
					else {
						if (!Create(tokens[1], priority))
							printError();
					}
				}
				catch (invalid_argument) {
					printError();
				}
			}

		}
		else if (cmd == "to") {
			Timeout();
		}
		else if (cmd == "de") {
			if (tokens.size() != 2)
				printError();
			else if(!Destroy(tokens[1]))
				printError();
		}

		else if(cmd == "req") {
			if (tokens.size() != 3)
				printError();
			else {
				try {
					int amount = stoi(tokens[2]);
					if (!Request(tokens[1], amount)) {
						printError();
					}
				}
				catch (invalid_argument) {
					printError();
				}
			}
		}
		
		else if (cmd == "rel") {
			if (tokens.size() != 3)
				printError();
			else {
				try {
					int amount = stoi(tokens[2]);
					if(!Release(tokens[1], amount))
						printError();
				}
				catch (invalid_argument) {
					printError();
				}
			}
		}

		else if (cmd == "init") {
			cout << endl;
			init();
		}

		else if (cmd == "pr") {
			printResources();
			printResourcesWaitingList();
		}

		else if (cmd == "pt") {
			printCreationTree();
		}
			
		else if (cmd == "prl")
			printReadyList();

		else { //command does not exist
			printError();
		}

		//cout << "shell> ";
	}
}

void printError() {
	cout << "error" << " ";
}