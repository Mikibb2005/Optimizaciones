//#include <math.h>
//#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>


//using namespace std;


#define N 100

typedef struct node
{
    int data;
    struct node *left;
    struct node *right;
} Node;

void añadir_valor(struct node* nodo, int val)
{
    nodo->data = val;
}


Node* crear_node()
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = 0;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

void añadir_left(struct node* nodo)
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
        
    }
}

int main()
{
    Node* a = crear_node();
    
    añadir_N_profundidad_rec(a, N);
    







    return 0;
}