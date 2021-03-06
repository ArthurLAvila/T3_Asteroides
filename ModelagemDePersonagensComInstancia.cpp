// **********************************************************************
// PUCRS/Escola Polit?cnica
// COMPUTA??O GR?FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Modelo.h"
#include "Instancia.h"


#include "Temporizador.h"
Temporizador T;
double AccumDeltaT=0;

Modelo MeiaSeta;
Modelo Mastro;

Instancia Universo[10];

// Limites l?gicos da ?rea de desenho
Ponto Min, Max;

bool desenha = false;

float angulo=0.0;
int nInstancias=0;

// **********************************************************************
//    Calcula o produto escalar entre os vetores V1 e V2
// **********************************************************************
double ProdEscalar(Ponto v1, Ponto v2)
{
    return v1.x*v2.x + v1.y*v2.y+ v1.z*v2.z;
}
// **********************************************************************
//    Calcula o produto vetorial entre os vetores V1 e V2
// **********************************************************************
void ProdVetorial (Ponto v1, Ponto v2, Ponto &vresult)
{
    vresult.x = v1.y * v2.z - (v1.z * v2.y);
    vresult.y = v1.z * v2.x - (v1.x * v2.z);
    vresult.z = v1.x * v2.y - (v1.y * v2.x);
}
/* ********************************************************************** */
/*                                                                        */
/*  Calcula a interseccao entre 2 retas (no plano "XY" Z = 0)             */
/*                                                                        */
/* k : ponto inicial da reta 1                                            */
/* l : ponto final da reta 1                                              */
/* m : ponto inicial da reta 2                                            */
/* n : ponto final da reta 2                                              */
/*                                                                        */
/* s: valor do par?metro no ponto de interse??o (sobre a reta KL)         */
/* t: valor do par?metro no ponto de interse??o (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;

    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // n?o h? intersec??o

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // h? intersec??o
}

// **********************************************************************
bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n)
{
    int ret;
    double s,t;
    ret = intersec2d( k,  l,  m,  n, s, t);
    if (!ret) return false;
    if (s>=0.0 && s <=1.0 && t>=0.0 && t<=1.0)
        return true;
    else return false;
}
// **********************************************************************
// **********************************************************************

void LeObjeto(const char *nome, Poligono &p)
{
    ifstream input;
    input.open(nome, ios::in);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    int nLinha = 0;
    unsigned int qtdVertices;
    
    input >> qtdVertices;
    double x,y;
    
    // Le a primeira linha apenas para facilitar o calculo do limites
    input >> x >> y;
    
    Min = Ponto(x,y);
    Max = Ponto(x,y);
    p.insereVertice(Ponto(x,y));

    for (int i=0; i< qtdVertices; i++)
    {
        // Le cada elemento da linha
        input >> x >> y;
        // atualiza os limites
        if (x<Min.x) Min.x = x;
        if (y<Min.y) Min.y = y;
        
        if (x>Max.x) Max.x = x;
        if (y>Max.y) Max.y = y;

        if(!input)
            break;
        nLinha++;
        //cout << "Arquivo: " << x << " " << y << endl;
        p.insereVertice(Ponto(x,y));
    }
    cout << "leitura concluida." << endl;
    //cout << "Linhas lidas: " << nLinha << endl;
    //cout << "Limites:" << endl;
    cout << "Minimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();
    cout << endl;

}
void CarregaModelos()
{
    
}
// No trabalho, esta fun??o dever instanciar todos os
// personagens do cen?rio
void CarregaInstancias()
{
    Universo[0].posicao = Ponto (0,0);
    Universo[0].rotacao = 0;
    Universo[0].modelo = 0;

    Universo[1].posicao = Ponto (10,10);
    Universo[1].rotacao = 45;
    Universo[1].modelo = 0;
    nInstancias = 2;

}
void DesenhaCenario()
{
    for(int i=0; i< nInstancias;i++)
    {
        Universo[i].desenha();
    }
}
// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
    //LeObjeto("EstadoRS.txt");
    //LeObjeto("PoligonoDeTeste.txt", Seta);
    //LeObjeto("MeiaSeta.txt", MeiaSeta);
    //LeObjeto("Mastro.txt", Mastro);
    //Min.x--;Min.y--;
    //Max.x++;Max.y++;
    
    CarregaModelos();
    CarregaInstancias();
    cout << "Vertices no Vetor: " << MeiaSeta.getNVertices() << endl;
    Min = Ponto(-50,-50);
    Max = Ponto(50,50);

}

double nFrames=0;
double TempoTotal=0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;


    if (AccumDeltaT > 1.0/30) // fixa a atualiza??o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=1;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x,
            Min.y,Max.y,
            -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}

void DesenhaSeta()
{
    glPushMatrix();
        //glScaled(2, 1, 1);
        MeiaSeta.desenhaLinhas();
        glScaled(1,-1, 1);
        MeiaSeta.desenhaLinhas();
    glPopMatrix();
}
void DesenhaApontador()
{
    glPushMatrix();
        glTranslated(-4, 0, 0);
        //glScalef(1.2,1,1);
        DesenhaSeta();
    glPopMatrix();
}
void DesenhaHelice()
{
    glPushMatrix();
    for(int i=0;i < 4; i++)
    {
        glRotatef(90, 0, 0, 1);
        DesenhaApontador();
    }
    glPopMatrix();
}
void DesenhaHelicesGirando()
{
    glPushMatrix();
        glRotatef(angulo, 0, 0, 1);
        DesenhaHelice();
   glPopMatrix();
}
void DesenhaMastro()
{
    Mastro.desenhaPoligono();
}
void DesenhaCatavento()
{
    glPushMatrix();
        DesenhaMastro();
        glPushMatrix();
            glColor3f(1,0,0); // R, G, B  [0..1]
            glTranslated(0,3,0);
            glScaled(0.2, 0.2, 1);
            DesenhaHelicesGirando();
        glPopMatrix();
    glPopMatrix();
}
#define LARG
void DesenhaRetangulo()
{
    glBegin(GL_QUADS);
        glVertex2d(-1, -1);
        glVertex2d(-1, 1);
        glVertex2d(1, 1);
        glVertex2d(1, -1);
    glEnd();
}
void DesenhaPersonagem()  // modelo - veio de arquivo
{
    glPushMatrix();
        glColor3f(1, 1, 1);
        glTranslatef(-1, -1, 0);
        DesenhaRetangulo();

        glTranslatef(2, 0, 0);
        glColor3f(1, 0, 0);
        DesenhaRetangulo();

        glTranslatef(0, 2, 0);
        glColor3f(0, 0, 1);
        DesenhaRetangulo();

        glTranslatef(-2, 0, 0);
        glColor3f(1, 1, 0);
        DesenhaRetangulo();
    glPopMatrix();
}

// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{
    cout << __FUNCTION__ << endl;
	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l?gicos da ?rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();
    
    glLineWidth(2);
   
    //DesenhaCatavento();
    
    DesenhaCenario();

	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n?mero de segundos e informa quanto frames
// se passaram neste per?odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }

}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            desenha = !desenha;
        break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t?tulo da janela.
    glutCreateWindow    ( "Primeiro Programa em OpenGL" );

    // executa algumas inicializa??es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser? chamada automaticamente quando
    // for necess?rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida??o da tela. A funcao "display"
    // ser? chamada automaticamente sempre que a
    // m?quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser? chamada automaticamente quando
    // o usu?rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser? chamada automaticamente sempre
    // o usu?rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser? chamada
    // automaticamente sempre o usu?rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
