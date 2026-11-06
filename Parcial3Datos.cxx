/*
 * TP Unidad 3 - Pilas y Colas con interfaz de consola
 * Enunciado 2 - Puerto de contenedores (pilas)
 * Alumno/a: Alvarez, Sebastian
 * Uso de IA: Claude y ChatGPT para hacer el codigo y resolver dudas de sintaxis y logica. Se le dio el enunciado y se le pidio que genere el codigo completo.
 * link chats: https://claude.ai/chat/ae38db56-b6ea-4106-bfd2-4240dc4ef4b2 y https://chatgpt.com/c/6aeaa28d-ed6c-83ea-9149-b6dc3407dd63
*/
#include <iostream>
#include <string>
#include <stack>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

//Constantes
const int CAPACIDAD_MIN = 30;       // capacidad minima camion (toneladas)
const int CAPACIDAD_MAX = 60;       // capacidad maxima camion (toneladas)
const int PESO_MIN = 1;             // peso minimo contenedor
const int PESO_MAX = 30;            // peso maximo contenedor
const int MAX_CONTENEDORES = 6;     // contenedores que entran en una pila
const int CIERRE = 0;               // valor que cierra una pila

const int ANCHO_PANTALLA = 60;      // ancho del titulo enmarcado
const int ANCHO_BARRA = 20;         // ancho de la barra de carga del camion

//Colores
const string RESET    = "\033[0m";
const string NEGRITA  = "\033[1m";
const string ROJO     = "\033[91m";   // errores y cierres (pila cerrada, pila llena, camion que sale)
const string VERDE    = "\033[92m";   // acciones realizadas (retirar un contenedor)
const string AMARILLO = "\033[93m";   // tope de cada pila, decisiones y turnos
const string AZUL     = "\033[94m";   // pila A
const string MAGENTA  = "\033[95m";   // pila B
const string CYAN     = "\033[96m";   // titulos, camion y avisos de ENTER

// Funciones de Pantalla. Activa los colores ANSI en la consola de Windows
void prepararConsola()
{
#ifdef _WIN32
    HANDLE salida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (GetConsoleMode(salida, &modo))
    {
        SetConsoleMode(salida, modo | 0x0004);  // 0x0004 = ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
#endif
}

//Borra la pantalla y deja el cursor arriba a la izquierda
void limpiarPantalla()
{
    cout << "\033[2J\033[3J\033[H";
}

// Pausa el programa hasta que el usuario apriete ENTER
void esperarEnter()
{
    string basura;
    cout << "\n" << CYAN << "  Presiona ENTER para continuar..." << RESET;
    getline(cin, basura);
}
// Dibuja el titulo enmarcado y centrado (el titulo tiene que tener menos de 58 letras)
void mostrarEncabezado(const string& titulo)
{
    int largo = titulo.length();
    int espacios = ANCHO_PANTALLA - 2 - largo;
    int izquierda = espacios / 2;
    int derecha = espacios - izquierda;

    cout << CYAN << NEGRITA;
    cout << "+" << string(ANCHO_PANTALLA - 2, '=') << "+\n";
    cout << "|" << string(izquierda, ' ') << titulo << string(derecha, ' ') << "|\n";
    cout << "+" << string(ANCHO_PANTALLA - 2, '=') << "+\n";
    cout << RESET << "\n";
}
// Muestra la pila de tope a base
void mostrarPila(const string& nombre, stack<int> pila, const string& color)
{
    cout << color << NEGRITA << "  Pila " << nombre << RESET
        << " (" << pila.size() << "/" << MAX_CONTENEDORES << " contenedores)\n";
    cout << "    TOPE -> ";

    if (pila.empty())
    {
        cout << "(vacia)";
    }

    bool esTope = true;
    while (!pila.empty())
    {
        if (esTope)
        {
            cout << AMARILLO << NEGRITA;
        }
        else
        {
            cout << color;
        }
        cout << "[" << setw(2) << pila.top() << " t]" << RESET << " ";
        pila.pop();
        esTope = false;
    }
    cout << "<- BASE\n";
}

// Muestra el camion actual con una barra que muestra cuanto va cargado
void mostrarCamion(int numero, int pesoActual, int capacidad)
{
    int llenos = pesoActual * ANCHO_BARRA / capacidad;
    cout << CYAN << NEGRITA << "  Camion " << numero << RESET << CYAN
        << "  [" << string(llenos, '#') << string(ANCHO_BARRA - llenos, '-') << "]  "
        << pesoActual << "/" << capacidad << " t" << RESET << "\n";
}

// Intenta convertir un texto en entero. Devuelve true si el texto es un numero valido (solo digitos, con un '-' opcional adelante). El numero sale por la referencia 'numero'.
bool convertirAEntero(const string& texto, int& numero)
{
    int inicio = 0;
    int fin = (int)texto.length() - 1;

    // se ignoran los espacios del principio y del final
    while (inicio <= fin && texto[inicio] == ' ')
    {
        inicio++;
    }
    while (fin >= inicio && texto[fin] == ' ')
    {
        fin--;
    }

    bool negativo = false;
    if (inicio <= fin && texto[inicio] == '-')
    {
        negativo = true;
        inicio++;
    }

    // sin digitos, o con demasiados digitos (evita desbordar el int)
    if (inicio > fin || fin - inicio + 1 > 6)
    {
        return false;
    }

    int valor = 0;
    for (int i = inicio; i <= fin; i++)
    {
        if (texto[i] < '0' || texto[i] > '9')
        {
            return false;   // hay una letra o un simbolo
        }
        valor = valor * 10 + (texto[i] - '0');
    }

    if (negativo)
    {
        valor = -valor;
    }
    numero = valor;
    return true;
}

// Pide un entero entre 'minimo' y 'maximo' (incluidos) y repite hasta que sea valido.
int leerEnteroEnRango(const string& mensaje, int minimo, int maximo)
{
    string linea;
    int numero = 0;

    while (true)
    {
        cout << "  " << mensaje << " [" << minimo << " a " << maximo << "]: ";
        if (!getline(cin, linea))
        {
            cout << "\n  Entrada cerrada. Fin del programa.\n";
            exit(0);
        }

        if (!convertirAEntero(linea, numero))
        {
            cout << ROJO << "  ERROR: ingresa solo un numero entero (sin letras ni simbolos)." << RESET << "\n";
        }
        else if (numero < minimo || numero > maximo)
        {
            cout << ROJO << "  ERROR: el valor esta fuera de rango. Debe estar entre "
                << minimo << " y " << maximo << "." << RESET << "\n";
        }
        else
        {
            return numero;
        }
    }
}
// Carga las dos pilas de forma alternada (A, B, A...).
// Las pilas entran POR REFERENCIA (&) porque la funcion tiene que llenarlas de verdad.
void cargarContenedores(stack<int>& pilaA, stack<int>& pilaB)
{
    bool cerradaA = false;
    bool cerradaB = false;
    char turno = 'A';

    while (!cerradaA || !cerradaB)
    {
        // si le toca a una pila cerrada, el turno pasa a la otra
        if (turno == 'A' && cerradaA)
        {
            turno = 'B';
        }
        if (turno == 'B' && cerradaB)
        {
            turno = 'A';
        }

        // alias de la pila y de la bandera que tienen el turno
        stack<int>& pilaActual = (turno == 'A') ? pilaA : pilaB;
        bool& cerradaActual = (turno == 'A') ? cerradaA : cerradaB;

        limpiarPantalla();
        mostrarEncabezado("CARGA DE CONTENEDORES");
        mostrarPila("A", pilaA, AZUL);
        if (cerradaA)
        {
            cout << ROJO << "    (pila A cerrada)" << RESET << "\n";
        }
        mostrarPila("B", pilaB, MAGENTA);
        if (cerradaB)
        {
            cout << ROJO << "    (pila B cerrada)" << RESET << "\n";
        }

        cout << "\n  Turno de carga: " << AMARILLO << NEGRITA << "Pila " << turno << RESET << "\n";
        cout << "  Peso del contenedor de " << PESO_MIN << " a " << PESO_MAX
            << " t, o " << CIERRE << " para cerrar la pila.\n\n";

        int peso = leerEnteroEnRango("Peso en toneladas", CIERRE, PESO_MAX);

        if (peso == CIERRE)
        {
            cerradaActual = true;
            cout << ROJO << NEGRITA << "\n  Pila " << turno << " cerrada." << RESET << "\n";
            esperarEnter();
        }
        else
        {
            pilaActual.push(peso);
            if ((int)pilaActual.size() == MAX_CONTENEDORES)
            {
                cerradaActual = true;
                cout << ROJO << NEGRITA << "\n  Pila llena (" << MAX_CONTENEDORES
                    << " contenedores). La pila " << turno << " se cierra." << RESET << "\n";
                esperarEnter();
            }
        }

        // el turno de carga pasa a la otra pila
        if (turno == 'A')
        {
            turno = 'B';
        }
        else
        {
            turno = 'A';
        }
    }
}

// Decide de que pila retira la grua: devuelve 'A' o 'B'. Las pilas entran por referencia CONSTANTE: solo se miran (no se copian ni se modifican).turnoEmpate' y 'motivo' entran por referencia porque la funcion los cambia: el turno de desempate se alterna y el motivo se devuelve para mostrarlo en pantalla.
char decidirOrigen(const stack<int>& pilaA, const stack<int>& pilaB, char& turnoEmpate, string& motivo)
{
    if (pilaA.empty())
    {
        motivo = "La pila A esta vacia, se retira de la pila B.";
        return 'B';
    }
    if (pilaB.empty())
    {
        motivo = "La pila B esta vacia, se retira de la pila A.";
        return 'A';
    }
    if (pilaA.top() < pilaB.top())
    {
        motivo = "El tope de A es mas liviano que el tope de B.";
        return 'A';
    }
    if (pilaB.top() < pilaA.top())
    {
        motivo = "El tope de B es mas liviano que el tope de A.";
        return 'B';
    }

    // empate: se usa el turno de desempate y despues se alterna
    char elegida = turnoEmpate;
    motivo = "Los topes pesan lo mismo: desempata el turno, le tocaba a la pila ";
    motivo = motivo + elegida + ".";
    if (turnoEmpate == 'A')
    {
        turnoEmpate = 'B';
    }
    else
    {
        turnoEmpate = 'A';
    }
    return elegida;
}

// Retira el tope de 'pila' y lo sube al camion actual. Si no entra, primero sale el camion lleno y llega uno nuevo vacio. Devuelve true si el camion tuvo que salir.
// Todo lo que cambia (pila, pesoCamion, camiones, pesoTotal) va por referencia;'capacidad' solo se lee, asi que va por valor.
bool procesarContenedor(stack<int>& pila, int capacidad, int& pesoCamion, int& camiones, int& pesoTotal)
{
    int peso = pila.top();
    bool camionSalio = false;

    if (pesoCamion + peso > capacidad)
    {
        camionSalio = true;
        camiones++;        // llega un camion nuevo
        pesoCamion = 0;    // y viene vacio
    }

    pila.pop();
    pesoCamion = pesoCamion + peso;
    pesoTotal = pesoTotal + peso;
    return camionSalio;
}

// Pantalla final con los totales
void mostrarResumen(int cargados, int camiones, int pesoTotal, int pesoUltimoCamion)
{
    limpiarPantalla();
    mostrarEncabezado("RESUMEN FINAL");

    cout << "  Las pilas A y B quedaron " << VERDE << NEGRITA << "vacias" << RESET << ".\n\n";
    cout << "  Contenedores cargados y retirados : " << NEGRITA << cargados << RESET << "\n";
    cout << "  Camiones usados                   : " << NEGRITA << camiones << RESET << "\n";
    cout << "  Peso total transportado           : " << NEGRITA << pesoTotal << " t" << RESET << "\n";
    if (camiones > 0)
    {
        cout << "  El ultimo camion salio con        : " << NEGRITA << pesoUltimoCamion << " t" << RESET << "\n";
    }
    cout << "\n";
}

// Main
int main()
{
    prepararConsola();

    stack<int> pilaA;
    stack<int> pilaB;

    // pantalla inicial
    limpiarPantalla();
    mostrarEncabezado("PUERTO DE CONTENEDORES");
    cout << "  Significado de los colores:\n";
    cout << "    " << ROJO << "Rojo" << RESET << "     errores y cierres (pila cerrada, camion que sale)\n";
    cout << "    " << VERDE << "Verde" << RESET << "    acciones realizadas\n";
    cout << "    " << AMARILLO << "Amarillo" << RESET << " tope de cada pila, decisiones y turnos\n";
    cout << "    " << AZUL << "Azul" << RESET << "     pila A      " << MAGENTA << "Magenta" << RESET << " pila B\n";
    cout << "    " << CYAN << "Cyan" << RESET << "     titulos, camion y avisos\n\n";

    int capacidad = leerEnteroEnRango("Capacidad de cada camion en toneladas", CAPACIDAD_MIN, CAPACIDAD_MAX);

    // cargar datos
    cargarContenedores(pilaA, pilaB);

    int cargados = (int)(pilaA.size() + pilaB.size());
    int camiones = 0;          // camiones usados
    int pesoCamion = 0;        // peso que lleva el camion actual
    int pesoTotal = 0;         // peso total transportado
    int pasos = 0;             // contenedores retirados hasta ahora
    char turnoEmpate = 'A';    // a quien le toca si hay empate de pesos

    if (cargados > 0)
    {
        camiones = 1;          // el primer camion ya esta esperando
    }
    else
    {
        limpiarPantalla();
        mostrarEncabezado("SIN CONTENEDORES");
        cout << AMARILLO << "  No se cargo ningun contenedor: no hay nada para retirar." << RESET << "\n";
        esperarEnter();
    }

    // simulacion: la grua retira de a un contenedor
    while (!pilaA.empty() || !pilaB.empty())
    {
        pasos++;

        limpiarPantalla();
        mostrarEncabezado("RETIRO DE CONTENEDORES");
        cout << "  Paso " << pasos << " de " << cargados << "\n\n";
        mostrarPila("A", pilaA, AZUL);
        mostrarPila("B", pilaB, MAGENTA);
        cout << "\n";
        mostrarCamion(camiones, pesoCamion, capacidad);
        cout << "\n  Turno de desempate: " << AMARILLO << NEGRITA << "Pila " << turnoEmpate << RESET << "\n";

        // 1) decidir de que pila se retira
        string motivo;
        char origen = decidirOrigen(pilaA, pilaB, turnoEmpate, motivo);
        stack<int>& pilaOrigen = (origen == 'A') ? pilaA : pilaB;

        // 2) guardar datos antes de procesar (para poder informar si el camion sale)
        int pesoRetirado = pilaOrigen.top();
        int camionAnterior = camiones;
        int pesoAnterior = pesoCamion;

        // 3) procesar
        bool salio = procesarContenedor(pilaOrigen, capacidad, pesoCamion, camiones, pesoTotal);

        // 4) informar accion y motivo
        cout << "\n  " << AMARILLO << "Motivo: " << RESET << motivo << "\n";
        if (salio)
        {
            cout << "  " << ROJO << NEGRITA << "El camion " << camionAnterior << " no tiene lugar para "
                << pesoRetirado << " t: sale lleno con " << pesoAnterior
                << " t. Llega el camion " << camiones << " vacio." << RESET << "\n";
        }
        cout << "  " << VERDE << NEGRITA << "Accion: se retiran " << pesoRetirado << " t de la pila "
            << origen << " y se suben al camion " << camiones << "." << RESET << "\n";
        cout << "  Peso transportado hasta ahora: " << pesoTotal << " t\n";

        esperarEnter();
    }

    // mostrar resumen final
    mostrarResumen(cargados, camiones, pesoTotal, pesoCamion);
    esperarEnter();
    return 0;
}
