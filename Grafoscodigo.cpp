#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class NodoVertice;

// arista es nodo de la lista de ady de un vertice

class NodoArista {
private:
    NodoVertice *destino;
    int peso;
    NodoArista *siguiente;

    friend class NodoVertice;
    friend class Grafo;
public:
    NodoArista(NodoVertice *d, int p) {
        destino = d;
        peso = p;
        siguiente = NULL;
    }
};

// 
// vertice es ciudad + su lista de aristas (osea carreteras)

class NodoVertice {
private:
    string codigo;
    string nombre;
    NodoArista *aristas;// primero de la lista de ady
    NodoVertice *siguiente; // siguiente vertice del grafo
    friend class Grafo;
public:
    NodoVertice(string c, string n) {
    	// vertice tiene codigo (A,B,C,D) y nombre (vetTibas, vetCartago, vetLimon, vetTurribalba)
        codigo = c;
        nombre = n;
        aristas = NULL;
        siguiente = NULL;
    }

    bool existeArista(NodoVertice *d) {
        NodoArista *aux = aristas;
        while (aux != NULL) {
            if (aux->destino == d)
                return true;
            aux = aux->siguiente;
        }
        return false;
    }

    // inserta al final para q quede en orden. Igual no es como q importe, pero si luego quisiera hacerle un print al grafo o algo
    // x el estilo, quedaria bien bonito
    void insertarArista(NodoVertice *d, int p) {
        NodoArista *nuevo = new NodoArista(d, p);
        if (aristas == NULL) {
            aristas = nuevo;
        } else {
            NodoArista *aux = aristas;
            while (aux->siguiente != NULL)
                aux = aux->siguiente;
            aux->siguiente = nuevo;
        }
    }
};

// lista de vertices, cada uno con lista de ady
class Grafo {
private:
    NodoVertice *primero;
    
    NodoVertice *buscarVertice(string codigo) {
        NodoVertice *aux = primero;
        while (aux != NULL) {
            if (aux->codigo == codigo)
                return aux;
            aux = aux->siguiente;
        }
        return NULL;
    }

    bool insertarVertice(string codigo, string nombre) {
        if (buscarVertice(codigo) != NULL)
            return false; // si ese cod ya existe (IGUAL en teoria no deberia de pasar pero bueno)
        NodoVertice *nuevo = new NodoVertice(codigo, nombre);
        if (primero == NULL) {
            primero = nuevo;
        } else {
            NodoVertice *aux = primero;
            while (aux->siguiente != NULL)
                aux = aux->siguiente;
            aux->siguiente = nuevo;
        }
        return true;
    }

    // hice q carreteras se muestren en vertice de origen Y destino
    bool insertarCarretera(string origen, string destino, int peso) {
        NodoVertice *vo = buscarVertice(origen);
        NodoVertice *vd = buscarVertice(destino);
        if (vo == NULL or vd == NULL or vo == vd)
            return false;
        if (vo->existeArista(vd))
            return false;// carretera repetida
        vo->insertarArista(vd, peso);
        vd->insertarArista(vo, peso);
        return true;
    }

public:
    Grafo() { 
		primero = NULL; 
	}

    // A;VetTibas
    bool cargarCiudades(string nombreArchivo) {
        ifstream archivo(nombreArchivo.c_str());
        if (!archivo) {
            cout << "No se pudo abrir " << nombreArchivo << endl;
            return false;
        }
        string codigo, nombre;
        getline(archivo, codigo, ';');
        while (archivo.good()) {
            getline(archivo, nombre);
            if (!insertarVertice(codigo, nombre))
                cout << "Ciudad repetida, se ignora: " << codigo << endl;
            getline(archivo, codigo, ';');
        }
        archivo.close();
        return true;
    }

    bool cargarCarreteras(string nombreArchivo) {
        ifstream archivo(nombreArchivo.c_str());
        if (!archivo) {
            cout << "No se pudo abrir " << nombreArchivo << endl;
            return false;
        }
        string origen, destino;
        int peso;
        getline(archivo, origen, ';');
        while (archivo.good()) { // lo q hace .good() es q en el while, lee codigo hasta punto y coma del getline, y cuando se termina (osea ya no hay nada para leer) good() manda false
            peso = 1;
            getline(archivo, destino, ';');
            archivo >> peso;
            if (!insertarCarretera(origen, destino, peso))
                cout << "Carretera invalida o repetida, se ignora: " << origen << "-" << destino << endl;
            getline(archivo, origen, ';');
        }
        archivo.close();
        return true;
    }

    void imprimir() {
        NodoVertice *v = primero;
        while (v != NULL) {
            cout << v->codigo << " (" << v->nombre << ") -> ";
            NodoArista *a = v->aristas;
            while (a != NULL) {
                cout << "[" << a->destino->codigo << "," << a->peso << "] ";
                a = a->siguiente;
            }
            cout << endl;
            v = v->siguiente;
        }
    }
};

int main() {
    Grafo g1;
    cout << "--- Grafo 1 ---" << endl;
    if (g1.cargarCiudades("Ciudades1.csv") and g1.cargarCarreteras("Carreteras1.csv"))
        g1.imprimir();

    Grafo g2;
    cout << endl << "--- Grafo 2  ---" << endl;
    if (g2.cargarCiudades("Ciudades2.csv") and g2.cargarCarreteras("Carreteras2.csv"))
        g2.imprimir();

    return 0;
}
