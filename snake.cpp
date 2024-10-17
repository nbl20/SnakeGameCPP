#include <iostream>
#include <windows.h> // Pour Sleep(ms) et SetConsoleTextAttribute
#include <conio.h>
#include <cstdlib> // Pour rand() et srand()
#include <ctime>   // Pour time()
#include <fstream> // Pour la gestion des fichiers
using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
int fruitsEaten;  // Compteur de fruits mangés sans grandir
const int fruitsToGrow = 3;  // Nombre de fruits nécessaires pour que le serpent grandisse

enum eDirection
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
eDirection dir;

void SaveGame(); // Déclaration de la fonction de sauvegarde
void LoadGame(); // Déclaration de la fonction de chargement

// Fonction pour changer la couleur dans la console
void SetColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Setup()
{
    srand(time(0)); 
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;  // Initialiser la taille de la queue à 0
    fruitsEaten = 0;  // Initialiser le compteur à 0
}

void Draw()
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});  // Efface l'écran (ou "clear" sous Unix/Linux)
    
    // Dessin de la bordure supérieure
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    // Dessin de la zone de jeu
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << "#";  // Bord gauche
            
            if (i == y && j == x) {
                SetColor(10);  // Vert pour le serpent
                cout << "O";  // Position du joueur
                SetColor(7);   // Remettre la couleur par défaut
            } else if (i == fruitY && j == fruitX) {
                SetColor(12);  // Rouge pour les pommes
                cout << "F";  // Position du fruit
                SetColor(7);   // Remettre la couleur par défaut
            } else { 
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        SetColor(10);  // Vert pour la queue du serpent
                        cout << "o";
                        SetColor(7);   // Remettre la couleur par défaut
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";  // Espace vide
            }

            if (j == width - 1)
                cout << "#";  // Bord droit
        }
        cout << endl;
    }

    // Dessin de la bordure inférieure
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    
    // Affiche le score
    cout << "Score: " << score << endl;
}

void Input()
{
    if (_kbhit()) {
        switch (_getch()) {
            case 'q':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;    
            case 'z':
                dir = UP;
                break;
            case 's':
                dir = DOWN;
                break;    
            case 'x':
                gameOver = true;
                break;    
            case 'v': // Sauvegarder la partie
                cout << "Sauvegarde en cours..." << endl;
                SaveGame();
                break;
            case 'l': // Charger la partie
                cout << "Chargement de la partie..." << endl;
                LoadGame();
                break;
        }
    }
}

void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    
    switch (dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }
    
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;
    
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 1;
        fruitX = rand() % width;
        fruitY = rand() % height;
        fruitsEaten++;  // Augmenter le compteur de fruits mangés
        
        // Le serpent grandit seulement après avoir mangé fruitsToGrow fruits
        if (fruitsEaten == fruitsToGrow) {
            nTail++;
            fruitsEaten = 0;  // Réinitialiser le compteur après avoir grandi
        }
    }

    Sleep(50);  // Pause de 50 millisecondes
}

// Fonction pour sauvegarder la partie dans un fichier
void SaveGame() {
    ofstream saveFile("savegame.txt");
    if (saveFile.is_open()) {
        // Sauvegarde des variables du jeu
        saveFile << x << " " << y << endl;
        saveFile << fruitX << " " << fruitY << endl;
        saveFile << score << endl;
        saveFile << nTail << endl;
        saveFile << fruitsEaten << endl;
        for (int i = 0; i < nTail; i++) {
            saveFile << tailX[i] << " " << tailY[i] << endl;
        }
        saveFile << dir << endl;  // Sauvegarder la direction actuelle
        saveFile.close();
        cout << "Jeu sauvegardé avec succès !" << endl;
    } else {
        cout << "Erreur lors de la sauvegarde du jeu !" << endl;
    }
}

// Fonction pour charger la partie depuis un fichier
void LoadGame() {
    ifstream loadFile("savegame.txt");
    if (loadFile.is_open()) {
        // Chargement des variables du jeu
        loadFile >> x >> y;
        loadFile >> fruitX >> fruitY;
        loadFile >> score;
        loadFile >> nTail;
        loadFile >> fruitsEaten;
        for (int i = 0; i < nTail; i++) {
            loadFile >> tailX[i] >> tailY[i];
        }
        int dirInt;
        loadFile >> dirInt;
        dir = static_cast<eDirection>(dirInt);  // Reconvertir en eDirection
        loadFile.close();
        cout << "Jeu chargé avec succès !" << endl;
    } else {
        cout << "Erreur lors du chargement du jeu !" << endl;
    }
}

int main()
{
    Setup();
    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
    }

    return 0;
}
 
