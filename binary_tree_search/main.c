//#include <math.h>
//#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


//using namespace std;


#define N 15


typedef struct node
{
    int data;
    struct node *left;
    struct node *right;
} Node;

void añadir_valor(Node* nodo, int val)
{
    nodo->data = val;
}


Node* crear_node()
{
    Node* newNode = (Node*)malloc(sizeof(Node));

    if (newNode == NULL) 
    {
        printf("Error de memoria\n");
        exit(1);
    }

    newNode->data = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    //printf("Se ha creado un nodo: %p\n", (void*)newNode);

    return newNode;
}





void añadir_left(Node* nodo)
{
    nodo->left = crear_node();
}

void añadir_right(struct node* nodo)
{
    nodo->right = crear_node();
}

void añadir_N_profundidad_rec(Node* nodo, int n)
{
    if(n == 0)
    {
        return;
    }
    else 
    {
        if(nodo->left != NULL)
        {
            añadir_N_profundidad_rec(nodo->left, n - 1);
        }
        else
        {
            nodo->left = crear_node();
            añadir_N_profundidad_rec(nodo->left, n - 1);
        }


        if (nodo->right != NULL) 
        {
            añadir_N_profundidad_rec(nodo->right, n - 1);
        }
        else 
        {
            nodo->right = crear_node();
            añadir_N_profundidad_rec(nodo->right, n - 1);
        }
    }
}


void random_valor_recurs(Node* nodo)
{
    nodo->data = (int)rand();
    //printf("El nodo %p = %d", (void*)nodo, nodo->data);
    if(nodo->left != NULL)
    {
        random_valor_recurs(nodo->left);
    }
    
    if (nodo->right != NULL) 
    {
        random_valor_recurs(nodo->right);
    }
}

void search_arbol_rec(Node* nodo, int val)
{
    if(nodo-> data == val)
    {
        printf("El nodo %p tiene el valor %d\n", nodo, val);
    }

    if(nodo->left != NULL)
    {
        search_arbol_rec(nodo->left, val);
    }
    
    if (nodo->right != NULL) 
    {
        search_arbol_rec(nodo->right, val);
    }
}




int main()
{
    srand(time(NULL));
    Node* a = crear_node();
    printf("nodo master creado %p\n\n\n", (void*)a);
    
    añadir_N_profundidad_rec(a, N);
    printf("arbol creado pero vacio\n\n\n");


    random_valor_recurs(a);
    printf("arbol con valores YA\n\n\n");

    #pragma omp parallel for schedule(dynamic, 512)
    for (int i = 0; i < (1 << N); i++) 
    {
        search_arbol_rec(a, i);
    }
    //search_arbol_rec(a, 10);
    







    return 0;
}