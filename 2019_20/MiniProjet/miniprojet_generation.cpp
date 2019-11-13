#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>

#define ShowLog false

#define matrice vector<vector<bool>>

#define MODE 1

using namespace std;

// Prototypes
bool ReadGrid(matrice &grid);
matrice GenerateGrid(int lines, int cols, int librePercentage);
bool ExportGrid(const matrice &grid, int lines, int cols, string path);
void Write(string msg);
void WriteLn(string msg);

int main() {
	WriteLn("Application started");
	// Initialisation du random
	srand(time(0));
	
	matrice libre;
	matrice passage;
	
	string mode;
	cin >> mode;
	
	if(mode ==  "a") {
		// Importation depuis cin
		if(!ReadGrid(libre))
			return 2;
			
			
	} else {
		cout << "ERROR : Unknown mode : " << mode << endl;
		return 1;
	}
	
	
		
		
	
	
	
	WriteLn("Application ended");
	
	return 0;
}

bool ReadGrid(matrice &grid) {
	// Generate grid map
	
	// Demander les entrées
	int n;
	WriteLn("Enter the side of the square:");
	cin >> n;
	
	if(n <= 0) {
		WriteLn("Invalid value : " + to_string(n));
		return false;
	}
	
	matrice grid2(n, vector<bool>(n));
	
	bool temp;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> temp;
			grid2[i][j] = temp;
			
			Write(to_string(temp) + " ");
			if(j == 70)
				WriteLn("");
		}
		WriteLn("");
	}
	
	grid = grid2;
	
	return true;
}

// Génération d'une grille aléatoire
matrice GenerateGrid(int lines, int cols, int librePercentage) {
	matrice grid(lines, vector<bool>(cols, false));
	
	int rndValue;
	WriteLn("Grid initialized as follow : ");
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			rndValue = rand() % 100 + 1;
			grid[i][j] = (rndValue <= librePercentage);
			
			Write(grid[i][j] + " ");
		}
		WriteLn("");
	}
	return grid;
}

// Exportation de la grille en un fichier pbm
bool ExportGrid(const matrice &grid, int lines, int cols, string path) {
	ofstream fs(path);
	
	if(!fs) {
		WriteLn("Cannot open the output file");
		return false;
	}
	
	fs << "P1" << endl;
	cout << "P1" << endl;
	fs << lines << " " << cols << endl;
	cout << lines << " " << cols << endl;
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			fs << !grid[i][j] << " ";
			cout << !grid[i][j] << " ";
		}
		cout << endl;
	}
	
	fs.close();
	
	WriteLn("Grid exported to " + path);
	return true;
}

void Write(string msg) {
	if(ShowLog)
		cout << msg;
}

void WriteLn(string msg) {
	if(ShowLog)
		cout << msg << endl;
}
