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
    //para puntos de articulacion
    bool visitado;
    int num;
    int bajo;
    NodoVertice *padre;
    int hijos;
    friend class Grafo;
public:
    NodoVertice(string c, string n) {
    	// vertice tiene codigo (A,B,C,D) y nombre (vetTibas, vetCartago, vetLimon, vetTurribalba)
        codigo = c;
        nombre = n;
        aristas = NULL;
        siguiente = NULL;
        visitado = false;
        num = 0;
        bajo = 0;
        padre = NULL;
        hijos = 0;
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
    //puntos articulacion
    void reiniciarVisitas() {
        NodoVertice *v = primero;
        while (v != NULL) {
            v->visitado = false;
            v->num = 0;
            v->bajo = 0;
            v->padre = NULL;
            v->hijos = 0;
            v = v->siguiente;
        }
    }

    //profundidad recursiva
    void profundidadArticulacion(NodoVertice *v, int &contador, string &preorden,
                                 string &arbol, string &retroceso) {
        v->visitado = true;
        contador++;
        v->num = contador;
        v->bajo = v->num;
        preorden += v->codigo + " ";

        NodoArista *a = v->aristas;
        while (a != NULL) {
            NodoVertice *w = a->destino;
            if (!w->visitado) {
                //arista del arbol
                w->padre = v;
                v->hijos++;
                arbol += "  " + v->codigo + " - " + w->codigo + "\n";
                profundidadArticulacion(w, contador, preorden, arbol, retroceso);
                if (w->bajo < v->bajo)
                    v->bajo = w->bajo;
            } else if (w != v->padre and w->num < v->num) {
                retroceso += "  " + v->codigo + " - " + w->codigo + "\n";
                if (w->num < v->bajo)
                    v->bajo = w->num;
            }
            a = a->siguiente;
        }
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
    
    //genera el archivo PuntosArticulacion.txt
    void puntosArticulacion(string inicio) {
        NodoVertice *raiz = buscarVertice(inicio);
        if (raiz == NULL) {
            cout << "El vertice " << inicio << " no existe" << endl;
            return;
        }
        reiniciarVisitas();
        int contador = 0;
        string preorden = "", arbol = "", retroceso = "";
        profundidadArticulacion(raiz, contador, preorden, arbol, retroceso);

        ofstream archivo("PuntosArticulacion.txt");
        archivo << "PUNTOS DE ARTICULACION - Punto de partida: " << inicio << endl << endl;
        archivo << "Recorrido en preorden: " << preorden << endl << endl;

        archivo << "Arbol de profundidad (aristas del arbol):" << endl;
        archivo << (arbol == "" ? "  (ninguna)\n" : arbol) << endl;

        archivo << "Aristas de retroceso:" << endl;
        archivo << (retroceso == "" ? "  (ninguna)\n" : retroceso) << endl;

        archivo << "Vertice\tNum\tBajo" << endl;
        NodoVertice *v = primero;
        while (v != NULL) {
            if (v->visitado)
                archivo << v->codigo << "\t" << v->num << "\t" << v->bajo << endl;
            else
                archivo << v->codigo << "\t-\t-\t(no alcanzable desde " << inicio << ")" << endl;
            v = v->siguiente;
        }

        archivo << endl << "ANALISIS:" << endl;
        string puntos = "";
        v = primero;
        while (v != NULL) {
            if (v->visitado) {
                if (v == raiz) {
                    archivo << v->codigo << " es la raiz y tiene " << v->hijos << " hijo(s): ";
                    if (v->hijos >= 2) {
                        archivo << "SI es punto de articulacion." << endl;
                        puntos += v->codigo + " ";
                    } else {
                        archivo << "NO es punto de articulacion." << endl;
                    }
                } else {
                    bool esPunto = false;
                    NodoArista *a = v->aristas;
                    while (a != NULL) {
                        NodoVertice *w = a->destino;
                        if (w->padre == v) {
                            archivo << v->codigo << ": hijo " << w->codigo << ", bajo(" << w->codigo
                                    << ")=" << w->bajo << (w->bajo >= v->num ? " >= " : " < ")
                                    << "num(" << v->codigo << ")=" << v->num << endl;
                            if (w->bajo >= v->num)
                                esPunto = true;
                        }
                        a = a->siguiente;
                    }
                    if (v->hijos == 0)
                        archivo << v->codigo << ": es hoja, NO es punto de articulacion." << endl;
                    else if (esPunto) {
                        archivo << "   -> " << v->codigo << " SI es punto de articulacion." << endl;
                        puntos += v->codigo + " ";
                    } else
                        archivo << "   -> " << v->codigo << " NO es punto de articulacion." << endl;
                }
            }
            v = v->siguiente;
        }
        archivo << endl << "Puntos de articulacion: " << (puntos == "" ? "ninguno" : puntos) << endl;
        archivo.close();

        cout << "Puntos de articulacion: " << (puntos == "" ? "ninguno" : puntos) << endl;
        cout << "Detalle guardado en PuntosArticulacion.txt" << endl;
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

	//PRUEBA PUNTO 5
    string inicio;
    cout << endl << "Punto de partida para puntos de articulacion: ";
    cin >> inicio;
    g2.puntosArticulacion(inicio);
    //---
    
    
    
    
    
    
    
    
        /*
    // ===================== MENU =====================
    //Grafo 1 (Ciudades1/Carreteras1): Prim, Kruskal, Dijkstra, Anchura, Profundidad
    //Grafo 2 (Ciudades2/Carreteras2): Puntos de articulacion
    Grafo g1, g2;
    bool cargado = false;
    int opcion = -1;
    string inicio, fin;

    while (opcion != 0) {
        cout << endl << "========== MENU ==========" << endl;
        cout << "1. Leer Archivos" << endl;
        cout << "2. Prim" << endl;
        cout << "3. Kruskal" << endl;
        cout << "4. Dijkstra" << endl;
        cout << "5. Puntos de Articulacion" << endl;
        cout << "6. Anchura" << endl;
        cout << "7. Profundidad" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion >= 2 and opcion <= 7 and !cargado) {
            cout << "Primero debe leer los archivos (opcion 1)" << endl;
            continue;
        }

        switch (opcion) {
        case 1:
            if (g1.cargarCiudades("Ciudades1.csv") and g1.cargarCarreteras("Carreteras1.csv")
                and g2.cargarCiudades("Ciudades2.csv") and g2.cargarCarreteras("Carreteras2.csv")) {
                cargado = true;
                cout << "Archivos cargados" << endl;
                cout << "--- Grafo 1 ---" << endl;
                g1.imprimir();
                cout << "--- Grafo 2 ---" << endl;
                g2.imprimir();
            }
            break;
        case 2:
            cout << "Punto de partida: ";
            cin >> inicio;
            // g1.prim(inicio);
            break;
        case 3:
            // g1.kruskal();
            break;
        case 4:
            cout << "Punto de partida: ";
            cin >> inicio;
            cout << "Punto de llegada: ";
            cin >> fin;
            // g1.dijkstra(inicio, fin);
            break;
        case 5:
            cout << "Punto de partida: ";
            cin >> inicio;
            g2.puntosArticulacion(inicio);
            break;
        case 6:
            cout << "Punto de partida: ";
            cin >> inicio;
            // g1.anchura(inicio);
            break;
        case 7:
            cout << "Punto de partida: ";
            cin >> inicio;
            // g1.profundidad(inicio);
            break;
        case 0:
            cout << "Saliendo..." << endl;
            break;
        default:
            cout << "Opcion invalida" << endl;
        }
    }
    */
    
    return 0;
}
