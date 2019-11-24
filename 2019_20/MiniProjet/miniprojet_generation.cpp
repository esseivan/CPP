#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include <iomanip>
#include <algorithm>

#define ShowLog false

#define matrice vector<vector<bool>>

using namespace std;

struct Point {
	double p;
	double pPrime;
	
	Point(double p, double pPrime) {
		this->p = p;
		this->pPrime = pPrime;
	}
};

struct Range {
	double min;
	double max;
};
	
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
void construire_passage(int line, int column, const matrice &libre, matrice &passage, bool &isLastLineReached);

int calculate_all_grids(int n, double p, int nbt, bernoulli_distribution &b, default_random_engine &e);

void calcul_dichotomique(double min, double max, vector<Point> &points, double minPrime, double maxPrime,
			// Suite des paramètres pour l'appel de calculate_all_grids
			int n, int nbt, default_random_engine &e);

bool comparePoints(Point p1, Point p2);


int main() {
	// Initialisation du random
	srand(time(0));
	
	// Lecture du mode
	string mode;
	cin >> mode;
	
	// Mode a
	if(mode ==  "a") {
		matrice libre;
		matrice passage;
	
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
		int nombreTerrainPassage = calculate_all_grids(n, p, nbt, b, e);
		
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
		//int NBP(102);
		
		// dichotomique
		vector<Point> points;
		points.push_back(Point(0,0));
		points.push_back(Point(1,1));
		calcul_dichotomique(0, 1, points, 0, 1, n, nbt, e);
		
		sort(points.begin(), points.end(), comparePoints);
		
		for (size_t i = 0; i < points.size(); i++)
		{
			cout << points[i].p << " " << points[i].pPrime << endl;
		}
		
		// normale
		//constexpr double step(1. / (NBP - 1));
		//double p(0);
		
		//for (int i = 0; i < NBP; i++)
		//{
			//bernoulli_distribution b(p); //booléen true avec une probabilité p
			
			//cout << p << " " << (double)calculate_all_grids(n, p, nbt, b, e)/nbt << endl;
			//p += step;
		//}
		
	} else {
		cout << "ERROR : Unknown mode : " << mode << endl;
		return 1;
	}
	
	return 0;
}

bool comparePoints(Point p1, Point p2) {
	return (p1.p < p2.p);
}

// Génération de nbt grilles et retourne le nombre de grilles qui ont une 'traversée'
int calculate_all_grids(int n, double p, int nbt, bernoulli_distribution &b, default_random_engine &e) {
	matrice libre;
	matrice passage;
	
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
			construire_passage(0, i, libre, passage, isTraversee);
		}
	}
	return isTraversee;
}

// Valider le passage pour cette cellule et vérifier celles adjacentes
void construire_passage(int line, int column, const matrice &libre, matrice &passage, bool &isLastLineReached) {
	// Si passage déjà fait, cellule déjà visitée
	if(passage[line][column])
		return;
	
	passage[line][column] = true;
	
	int lines = libre.size();
	int columns = libre[0].size();
	
	// Vérifier la cellule :
	// en haut
	if(line > 0)
		if(libre[line-1][column])
			construire_passage(line-1, column, libre, passage, isLastLineReached);
			
	// à gauche
	if(column > 0)
		if(libre[line][column-1])
			construire_passage(line, column-1, libre, passage, isLastLineReached);
			
	// en bas
	if((line+1) < lines)
		if(libre[line+1][column])
			construire_passage(line+1, column, libre, passage, isLastLineReached);
	
	// à droite
	if((column+1) < columns)
		if(libre[line][column+1])
			construire_passage(line, column+1, libre, passage, isLastLineReached);
			
	if((line+1) == lines)
		isLastLineReached = true;
}

void calcul_dichotomique(double min, double max, vector<Point> &points, double minPrime, double maxPrime,
			// Suite des paramètres pour l'appel de calculate_all_grids
			int n, int nbt, default_random_engine &e) {
	constexpr double MIN_DELTA_P(0.000001); // 10^-6
	constexpr double MAX_ERROR(0.01); // 10^-2
	
	// Recherche des points
	double p((min+max)/2);
	bernoulli_distribution b(p); //booléen true avec une probabilité p
	int nb_traversee = calculate_all_grids(n, p, nbt, b, e);
	double pPrime((double)nb_traversee/nbt);
	
	double error(pPrime - (minPrime+maxPrime)/2);
	
	// Sauvegarde du point (p,p')
	Point currentPoint(p,pPrime);
	points.push_back(currentPoint);
	
	// Vérification des conditions
	if((max-min) < MIN_DELTA_P)
		return;
	
	if(error < -MAX_ERROR)
		calcul_dichotomique(p, max, points, pPrime, maxPrime, n, nbt, e);
	else if(error > MAX_ERROR)
		calcul_dichotomique(min, p, points, minPrime, pPrime, n, nbt, e);
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
