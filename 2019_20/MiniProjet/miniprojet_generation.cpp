#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>

#define ShowLog true

using namespace std;

// Prototypes
int GenerateGrid(vector<std::vector<bool>> &grid, int lines, int cols, int librePercentage);
int ExportGrid(const vector<std::vector<bool>> &grid, int lines, int cols, std::string path);
	
int main() {
	cout << "Application started" << endl;
	srand(time(0));
	
	// Generate grid map
	int nLine;
	int nCol;
	int perc;
	cout << "Enter the number of lines" << endl;
	cin >> nLine;
	cout << "Enter the number of columns : " << endl;
	cin >> nCol;
	cout << "Enter the percentage of available cell : " << endl;
	cin >> perc;
	
	if(nLine <= 0 || nCol <= 0 || perc < 0 || perc > 100) {
		cout << "Invalid row / line / percentage amount" << endl;
		return 2;
	}
	cout << "Valid row & lines" << endl;
	cout << nLine << " line(s)" << endl;
	cout << nCol << " col(s)" << endl;
	
	vector<vector<bool>> libre(nLine, vector<bool>(nCol, false));
	
	GenerateGrid(libre, nLine, nCol, perc);
	ExportGrid(libre, nLine, nCol, "grid.pbm");
	
	cout << "Application ended" << endl;
	
	return 0;
}

// Génération d'une grille aléatoire
int GenerateGrid(vector<vector<bool>> &grid, int lines, int cols, int librePercentage) {
	int rndValue;
	if(ShowLog)
		cout << "Grid initialized as follow : " << endl;
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			rndValue = rand() % 100 + 1;
			grid[i][j] = (rndValue <= librePercentage);
			
			if(ShowLog)
				cout << grid[i][j] << " ";
		}
		if(ShowLog)
			cout << endl;
	}
	return 0;
}

// Exportation de la grille en un fichier pbm
int ExportGrid(const vector<vector<bool>> &grid, int lines, int cols, string path) {
	ofstream fs(path);
	
	if(!fs) {
		cout << "Cannot open the output file" << endl;
		return 3;
	}
	
	fs << "P1" << endl;
	fs << lines << " " << cols << endl;
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			fs << !grid[i][j] << " ";
		}
		fs << endl;
	}
	
	fs.close();
	
	cout << "Grid exported to " << path << endl;
	return 0;
}
