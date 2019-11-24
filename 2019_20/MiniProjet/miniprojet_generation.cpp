#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include <iomanip>

#define ShowLog false

#define matrice vector<vector<bool>>

using namespace std;

// Prototypes
/**** a ****/
// Lecture d'une grille depuis l'entrée
int ReadGrid(matrice &grid);

/**** b ****/
// Génération d'une grile aléatoire
matrice GenerateGrid(int lines, int cols, bernoulli_distribution &b, default_random_engine &e);

// Affiche dans l'output la grille au format pbm
void OutputGrid(const matrice &grid);
// Exporte le fichie map.ppm pour une carte plus détaillée
bool ExportMap(const matrice &libre, const matrice &passage, string path);

// Calcul du passage : méthode récursive
bool calculer_passage(const matrice &libre, matrice &passage);
bool construire_passage(int line, int column, const matrice &libre, matrice &passage);

int calcualte_all_grids(matrice &libre, matrice &passage, int n, double p, int nbt, bernoulli_distribution &b, default_random_engine &e);

int main() {
	// Initialisation du random
	srand(time(0));
	
	matrice libre;
	matrice passage;
	
	// Lecture du mode
	string mode;
	cin >> mode;
	
	// Mode a
	if(mode ==  "a") {
		// Importation depuis cin
		int n = ReadGrid(libre);
		
		// Vérification n valide
		if(n <= 0)
			return 2;
		
		// Initialisation de la grille de passage
		passage = vector<vector<bool>>(n, vector<bool>(n));
		
		// Calcul de la grille de passage : méthode récursive
		calculer_passage(libre, passage);
		
		// Exporter la grille
		OutputGrid(passage);
		
		// Exporter la carte
		ExportMap(libre, passage, "output/map.ppm");


	// Mode b
	} else if (mode == "b") {
		// Génération d'une grille aléatoire
		// Récupération des paramètres
		int n;
		double p;
		int nbt;
		cin >> n;
		cin >> p;
		cin >> nbt;
		
		cout << setprecision(6); // 6 chiffres à droite de la virgule
		cout << fixed;
		
		// Initialisation des générateur de random
		bernoulli_distribution b(p); //booléen true avec une probabilité p
		default_random_engine e;
		
		// Le nombre de terrains ayant un passage
		int nombreTerrainPassage = calcualte_all_grids(libre, passage, n, p, nbt, b, e);
		
		// Output p'
		double pPrime = (double)nombreTerrainPassage/nbt;
		cout << pPrime << endl;
			
	// Mode c
	} else if (mode == "c") {
		int n;
		int nbt;
		cin >> n;
		cin >> nbt;
		
		// Initialisation des générateur de random
		default_random_engine e;
		
		// Nombre de points
		constexpr int NBP(102);
		constexpr double step(1. / (NBP - 1));
		double p(0);
		
		for (int i = 0; i < NBP; i++)
		{
			bernoulli_distribution b(p); //booléen true avec une probabilité p
			
			cout << p << " " << (double)calcualte_all_grids(libre, passage, n, p, nbt, b, e)/nbt << endl;
			p += step;
		}
	} else {
		cout << "ERROR : Unknown mode : " << mode << endl;
		return 1;
	}
	
	return 0;
}

// Génération de nbt grilles et retourne le nombre de grilles qui ont une 'traversée'
int calcualte_all_grids(matrice &libre, matrice &passage, int n, double p, int nbt, bernoulli_distribution &b, default_random_engine &e) {
	// Le nombre de terrains ayant un passage
	int nombreTerrainPassage = 0;
	
	// Génération de nbt terrains random
	for (int i = 0; i < nbt; i++)
	{
		// génération de la grille libre
		libre = GenerateGrid(n, n, b, e);
		// initialisation de la grille passage
		passage = vector<vector<bool>>(n, vector<bool>(n));
		
		// Calcul du passage
		if(calculer_passage(libre, passage))
			nombreTerrainPassage++;
	}
	return nombreTerrainPassage;
}

// Calculer le passage pour la première ligne et appel ensuite de construire_passage_recursif
bool calculer_passage(const matrice &libre, matrice &passage) {
	bool isTraversee = false;
	int columns = libre[0].size();
	// Recherche sur la première ligne
	for (int i = 0; i < columns; i++)
	{
		// Si cette cellule est libre
		if(libre[0][i])
		{
			// Appeler la fonction construire_passage_recursif pour cette cellule
			if(construire_passage(0, i, libre, passage) && !isTraversee)
				isTraversee = true;
		}
	}
	return isTraversee;
}

// Indique si la traversée est faisable
bool passage_traversee(const matrice &passage) {
	int columns = passage[0].size();
	int lastLine = passage.size() - 1;
	for (int i = 0; i < columns; i++)
	{
		if(passage[lastLine][i])
			return true;
	}
	return false;
}

// Valider le passage pour cette cellule et vérifier celles adjacentes
bool construire_passage(int line, int column, const matrice &libre, matrice &passage) {
	// Si passage déjà fait, cellule déjà visitée
	if(passage[line][column])
		return false;
	
	passage[line][column] = true;
	
	int lines = libre.size();
	int columns = libre[0].size();
	
	// Vérifier la cellule :
	// en haut
	if(line > 0)
		if(libre[line-1][column])
			construire_passage(line-1, column, libre, passage);
			
	// à gauche
	if(column > 0)
		if(libre[line][column-1])
			construire_passage(line, column-1, libre, passage);
			
	// en bas
	if((line+1) < lines)
		if(libre[line+1][column])
			construire_passage(line+1, column, libre, passage);
	
	// à droite
	if((column+1) < columns)
		if(libre[line][column+1])
			construire_passage(line, column+1, libre, passage);
			
	if((line+1) == lines)
		return true;
	return false;
}

// Lecture d'une grille depuis l'entrée
int ReadGrid(matrice &grid) {
	// Demander les n^2 entrées
	int n;
	cin >> n;
	
	// Vérifier que n strictement positif
	if(n <= 0) {
		return -1;
	}
	
	// Initialisation de la grille nxn à faux
	matrice grid2(n, vector<bool>(n));
	
	// Lire chaque élément à la suite
	bool temp;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> temp;
			grid2[i][j] = !temp;
		}
	}
	
	// Affecter la grille
	grid = grid2;
	
	return n;
}

// Génération d'une grille aléatoire
matrice GenerateGrid(int lines, int cols, bernoulli_distribution &b, default_random_engine &e) {
	// Initialisation
	matrice grid(lines, vector<bool>(cols, false));
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			grid[i][j] = b(e);
		}
	}
	return grid;
}

// Output the map (fichier ppm avec les places bloqués en noir, les places passages en blanc et les places inacessibles en rouge)
bool ExportMap(const matrice &libre, const matrice &passage, string path) {
	ofstream fs(path);
	
	if(!fs) {
		cerr << "Unable to export to file" << endl;
		return false;
	}
	
	int lines = libre.size();
	int columns = libre[0].size();
	
	fs << "P3" << endl;
	fs << lines << " " << columns << endl;
	fs << "255" << endl;
	
	string colInaccessible = "255 0 0";
	string colAccessible = "255 255 255";
	string colInvalid = "0 0 0";
	string colCurrent;
	
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			if(libre[i][j]) {
				if(passage[i][j])
					colCurrent = colAccessible;
				else
					colCurrent = colInaccessible;
			} else
				colCurrent = colInvalid;
			
			fs << colCurrent << " ";
		}
		fs << endl;
	}
	
	fs.close();
	
	return true;
}

// Affiche dans l'output la grille au format pbm
void OutputGrid(const matrice &grid) {
	int lines = grid.size();
	int columns = grid[0].size();
	
	cout << "P1" << endl;
	cout << lines << " " << columns << endl;
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			cout << !grid[i][j] << " ";
			if((j+1) % 70 == 0)
				cout << endl;
		}
		cout << endl;
	}
}
