/** Auteur : Esseiva Nicolas | Date : 03.12.2019 | version : 1.0 **/
#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include <iomanip>
#include <algorithm>

using namespace std;

constexpr double min_delta_p = 0.000001;
constexpr double max_error = 0.01;

// Définition de types récurrents
typedef vector<vector<bool>> matrice;
typedef vector<bool> ligne;

// Type Point : utilisé pour le mode c pour créer une liste de paire (p,p')
struct Point {
	double p;
	double pPrime;
	
	Point(double p, double pPrime) {
		this->p = p;
		this->pPrime = pPrime;
	}
};

// Lecture de la grille et retourne n ou -1 si n <= 0
int read_grid(matrice &grid);
// Génération d'une grille lines x columns (lines et columns >= 0)
void generate_grid(matrice &grid, int lines, int columns,
					 bernoulli_distribution &b,	default_random_engine &e);

// Affiche dans la console la grille au format pbm
void output_grid_pbm(const matrice &grid);

// Résolution du passage par méthode récursive
bool resoudre_passage(const matrice &libre, matrice &passage);
// Valide le passage pour cette cellule et vérifie pour pour celles adjacentes
void construire_passage(int line, int column,
						const matrice &libre, matrice &passage,
						bool &isLastLineReached);

// résolution de 'nbt' grilles avec p défini
int resoudre_nbt_grilles(int n, double p, int nbt,
						bernoulli_distribution &b, default_random_engine &e);

// résolution dichotomique de p'
void resoudre_dichotomique(double min, double max, double minPrime, double maxPrime,
						   vector<Point> &points, int n, int nbt,
						   default_random_engine &e);

// Comparaison de deux Points (p,p') pour trier la liste
bool comparer_points(Point p1, Point p2);

// Mode c normal : pas constant de 1/(NBP-1)
void echantillonnage_constant();
// Mode c dichotomique : pas variable
void echantillonnage_adaptif();

/***************** IMPLEMENTATIONS *****************/
// Méthode principale, entrée du programme
int main() {
	srand(time(0));
	string mode;
	cin >> mode;
	
	if(mode ==  "a") {
		matrice libre, passage;
		int n = read_grid(libre); // Lecture de la grille
		if(n <= 0) return 2;
		passage = matrice(n, ligne(n));
		
		resoudre_passage(libre, passage); // Résolution de la grille
		output_grid_pbm(passage); // Affichage du résultat dans la console
		
	} else if (mode == "b") {
		int n, nbt;
		double p;
		cin >> n; // Demande de n, p, nbt
		cin >> p;
		cin >> nbt;
		bernoulli_distribution b(p);
		default_random_engine e;
		if(n <= 0 || nbt <= 0) return 3;
		
		// Affichage de p'
		cout << setprecision(6); // 6 chiffres à droite de la virgule
		cout << fixed;
		cout << (double)resoudre_nbt_grilles(n, p, nbt, b, e)/nbt << endl;
		
	} else if (mode == "c") {
		echantillonnage_adaptif(); // Résolution par méthode dichotomique (3.3.2)
		//echantillonnage_constant(); // Résolution par méthode normale (3.3.1)
		
	} else { // Mode inconnu
		cout << "ERROR : Unknown mode : " << mode << endl;
		return 1;
	}
	return 0;
}

// Méthode permettant de comparer deux Points (p,p')
bool comparer_points(Point p1, Point p2) {
	return (p1.p < p2.p);
}
// Mode c normal : pas constant (3.3.1)
void echantillonnage_constant() {
	constexpr int nbp(102); // Nombre de points
	 // pas constant (NBP de 3 doit donner un pas de 1/2, donc NBP-1 et non NBP-2)
	constexpr double step(1. / (nbp - 1));
	int n, nbt;
	cin >> n; // Demande de n, nbt
	cin >> nbt;
	default_random_engine e;
	double p(0);
	
	cout << setprecision(6); // 6 chiffres à droite de la virgule
	cout << fixed;
	for (int i = 0; i < nbp; i++)
	{
		bernoulli_distribution b(p);
		// affichage de "<p> <p'>"
		cout << p << " " << (double)resoudre_nbt_grilles(n, p, nbt, b, e)/nbt << endl;
		p += step;
	}
}

// Mode c dichotomique : pas variable (3.3.2)
void echantillonnage_adaptif() {
	int n, nbt;
	cin >> n; // Demande de n, nbt
	cin >> nbt;
	default_random_engine e;
	if(n <= 0 || nbt <= 0) return;
	// Liste des points initialisé avec (0,0) et (1,1)
	vector<Point> points({Point(0,0),Point(1,1)});
	
	// Résolution dichotomique et récursive
	resoudre_dichotomique(0, 1, 0, 1, points, n, nbt, e);
	
	// tri croissant de liste avec la méthode de tri comparer_points
	sort(points.begin(), points.end(), comparer_points);
	
	cout << setprecision(6); // 6 chiffres à droite de la virgule
	cout << fixed;
	for (size_t i = 0; i < points.size(); i++)
	{
		// Affichage de "<p> <p'>"
		cout << points[i].p << " " << points[i].pPrime << endl;
	}
}








// Résolution de 'nbt' grilles, retourne le nombre de grilles qui ont une 'traversée'
int resoudre_nbt_grilles(int n, double p, int nbt,
						 bernoulli_distribution &b, default_random_engine &e) {
	matrice libre, passage;
	int compteur_traversee(0);
	
	for (int i = 0; i < nbt; i++)
	{
		generate_grid(libre, n, n, b, e);
		passage = matrice(n, ligne(n));
		// Si une cellule sur la dernière ligne est accessible, augmenter le compteur
		if(resoudre_passage(libre, passage))
			compteur_traversee++;
	}
	return compteur_traversee;
}

// Valide le passage pour cette cellule et vérifie pour celles adjacentes
void construire_passage(int line, int column,
						const matrice &libre, matrice &passage,
						bool &isLastLineReached) {
	// si cellule déjà visitée
	if(passage[line][column]) return;
	
	passage[line][column] = true;
	
	int lines(libre.size());
	int columns(libre[0].size());
	
	// Si dernière ligne
	if((line+1) == lines) isLastLineReached = true;
	
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
}

// Résolution dichotomique de p'
void resoudre_dichotomique(double min, double max, double minPrime, double maxPrime,
						   vector<Point> &points, int n, int nbt,
						   default_random_engine &e) {
	double p((min+max)/2); // Point milieu
	bernoulli_distribution b(p);
	
	// Calcul de p'
	double pPrime((double)resoudre_nbt_grilles(n, p, nbt, b, e)/nbt);
	// Calcul de l'erreur absolue
	double error(pPrime - (minPrime+maxPrime)/2);
	
	points.push_back(Point(p, pPrime)); // Sauvegarde du point (p,p')
	
	// Vérification des conditions
	if((max-min) < min_delta_p)
		return;
	
	if(error < -max_error)
		resoudre_dichotomique(p, max, pPrime, maxPrime, points, n, nbt, e);
	else if(error > max_error)
		resoudre_dichotomique(min, p, minPrime, pPrime, points, n, nbt, e);
}

// Lecture de la grille et retourne n ou -1 si n <= 0
int read_grid(matrice &grid) {
	int n;
	cin >> n;
	
	if(n <= 0) return -1;
	
	grid = matrice(n, vector<bool>(n));
	
	// Lire chaque cellule, et stocker l'inverse logique
	bool valeur;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> valeur;
			grid[i][j] = !valeur;
		}
	}
	return n;
}









// Résolution du passage en partant de la première ligne
bool resoudre_passage(const matrice &libre, matrice &passage) {
	// Indique si la dernière ligne est accessible
	bool isTraversee = false;
	int columns = libre[0].size();
	
	// Recherche sur la première ligne
	for (int i = 0; i < columns; i++)
	{
		if(libre[0][i])
			construire_passage(0, i, libre, passage, isTraversee);
	}
	return isTraversee;
}

// Génération d'une grille lines x columns (lines et columns >= 0)
void generate_grid(matrice &grid, int lines, int columns,
				   bernoulli_distribution &b, default_random_engine &e) {
	grid = matrice(lines, ligne(columns, false));
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			grid[i][j] = b(e); // Stockage d'un booléen aléatoire
		}
	}
}

// Affiche dans l'output la grille au format pbm
void output_grid_pbm(const matrice &grid) {
	int lines = grid.size();
	int columns = grid[0].size();
	
	cout << "P1" << endl; // 1e ligne : P1
	cout << lines << " " << columns << endl; // 2e ligne : <colonnes> <lignes>
	
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			cout << !grid[i][j] << " ";
			if((j+1) % 35 == 0) // Si trop de caractères, retour à la ligne
				cout << endl;
		}
		cout << endl;
	}
}
