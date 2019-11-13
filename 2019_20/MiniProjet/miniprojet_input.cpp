#include <iostream>
#include <vector>

#define Matrix vector<vector<bool>>

using namespace std;

int main() {
	cout << "Application started" << endl;
	
	// Load grid map
	// First input is P1, verify it
	string pbmVerify;
	cin >> pbmVerify;
	if(pbmVerify != "P1")
	{
		cout << "Invalid file format" << endl;
		return -1;
	}
	int nLine;
	int nCol;
	cin >> nCol;
	cin >> nLine;
	
	if(nLine <= 0 || nCol <= 0) {
		cout << "Invalid row / line amount" << endl;
		return -2;
	}
	cout << "Valid row & lines" << endl;
	cout << nLine << " line(s)" << endl;
	cout << nCol << " col(s)" << endl;
	
	Matrix libre(nLine, vector<bool>(nCol, false));
	int temp(0);
	
	cout << "Grid initialized as follow : " << endl;
	for (int i = 0; i < nLine; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			cin >> temp;
			temp = (temp > 0);
			libre[i][j] = (temp > 0);
			
			cout << temp << " ";
		}
		cout << endl;
	}
	
	cout << "Application ended" << endl;
	return 0;
}
