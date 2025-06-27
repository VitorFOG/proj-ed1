#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 5
#define FILENAME "data.txt"

typedef struct Title {
    char name[100];
    char status[20]; // "Para Assistir", "Assistido", "Abandonado"
} Title;

// Estrutura de nó para a implementação de Listas Ligadas.
typedef struct Node {
    Title data;
    struct Node* next;
} Node;

// Estrutura de nó para a Árvore Binária de Busca (BST).
typedef struct TreeNode {
    Title data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Estrutura de nó para a Pilha (Stack).
typedef struct StackNode {
    Title data;
    struct StackNode* next;
} StackNode;

// Estrutura de nó para a Fila (Queue).
typedef struct QueueNode {
    Title data;
    struct QueueNode* next;
} QueueNode;

// Listas ligadas para cada categoria de filmes/séries.
Node* toWatchList = NULL;
Node* watchedList = NULL;
Node* droppedList = NULL;

// Raiz da Árvore Binária de Busca para pesquisa otimizada.
TreeNode* root = NULL;

// Topo da Pilha para o histórico de adições.
StackNode* historyStack = NULL;
int historyCount = 0;

// Ponteiros para o início e fim da Fila de "assistir a seguir".
QueueNode* front = NULL;
QueueNode* rear = NULL;

void addTitle();
void moveTitle();
void viewLists();
void searchTitle();
void viewHistory();
void addToWatchNext();
void viewWatchNext();
void printList(Node* head, const char* listName);
void freeLists();
void freeList(Node* head);
Node* removeNode(Node** head, const char* name);
void addNode(Node** head, Node* node);
TreeNode* insertTreeNode(TreeNode* node, Title data);
TreeNode* searchTreeNode(TreeNode* node, const char* name);
TreeNode* deleteTreeNode(TreeNode* node, const char* name);
TreeNode* findMin(TreeNode* node);
void pushHistory(Title data);
void printHistory();
void enqueueWatchNext(Title data);
void printWatchNext();
void saveData();
void loadData();
void saveList(FILE* file, Node* head);
void freeTree(TreeNode* node);

int main() {
    int choice;

    loadData();

    while (1) {
        printf("\nGerenciador de Filmes/Séries\n");
        printf("1. Adicionar Título\n");
        printf("2. Mover Título\n");
        printf("3. Visualizar Listas\n");
        printf("4. Buscar Título\n");
        printf("5. Visualizar Histórico\n");
        printf("6. Adicionar à Fila 'Assistir a Seguir'\n");
        printf("7. Visualizar Fila 'Assistir a Seguir'\n");
        printf("8. Sair\n");
        printf("Digite sua escolha: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addTitle();
                break;
            case 2:
                moveTitle();
                break;
            case 3:
                viewLists();
                break;
            case 4:
                searchTitle();
                break;
            case 5:
                viewHistory();
                break;
            case 6:
                addToWatchNext();
                break;
            case 7:
                viewWatchNext();
                break;
            case 8:
                printf("Saindo...\n");
                saveData();
                freeLists();
                exit(0);
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}

void addTitle() {
    Title newTitle;
    printf("Digite o nome do título: ");
    while (getchar() != '\n');
    scanf("%[^\n]", newTitle.name);
    strcpy(newTitle.status, "Para Assistir");

    // Alocação dinâmica de memória para um novo nó da lista.
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Erro: Falha na alocação de memória.\n");
        return;
    }
    newNode->data = newTitle;
    newNode->next = toWatchList;
    toWatchList = newNode;

    root = insertTreeNode(root, newTitle);
    pushHistory(newTitle);

    printf("Título adicionado à lista 'Para Assistir'.\n");
}

void moveTitle() {
    char name[100];
    int choice;
    printf("Digite o nome do título para mover: ");
    while (getchar() != '\n');
    scanf("%[^\n]", name);

    TreeNode* bstNode = searchTreeNode(root, name);
    if (bstNode == NULL) {
        printf("Título não encontrado.\n");
        return;
    }

    Node* nodeToMove = NULL;
    nodeToMove = removeNode(&toWatchList, name);
    if (nodeToMove == NULL) {
        nodeToMove = removeNode(&watchedList, name);
    }
    if (nodeToMove == NULL) {
        nodeToMove = removeNode(&droppedList, name);
    }

    if (nodeToMove == NULL) {
        printf("Título não encontrado em nenhuma lista, mas estava na árvore. Inconsistência de dados detectada.\n");
        root = deleteTreeNode(root, name);
        return;
    }

    printf("Mover para:\n");
    printf("1. Para Assistir\n");
    printf("2. Assistido\n");
    printf("3. Abandonado\n");
    printf("Digite sua escolha: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            strcpy(nodeToMove->data.status, "Para Assistir");
            addNode(&toWatchList, nodeToMove);
            break;
        case 2:
            strcpy(nodeToMove->data.status, "Assistido");
            addNode(&watchedList, nodeToMove);
            break;
        case 3:
            strcpy(nodeToMove->data.status, "Abandonado");
            addNode(&droppedList, nodeToMove);
            break;
        default:
            printf("Opção inválida. Retornando título para a lista original.\n");
            addNode(&toWatchList, nodeToMove);
            break;
    }
    strcpy(bstNode->data.status, nodeToMove->data.status);

    printf("Título movido com sucesso.\n");
}

void viewLists() {
    printList(toWatchList, "Para Assistir");
    printList(watchedList, "Assistidos");
    printList(droppedList, "Abandonados");
}

// A função de busca utiliza a Árvore Binária para maior eficiência (escalabilidade).
void searchTitle() {
    char name[100];
    printf("Digite o nome do título para buscar: ");
    while (getchar() != '\n');
    scanf("%[^\n]", name);

    TreeNode* foundNode = searchTreeNode(root, name);
    if (foundNode != NULL) {
        printf("Título encontrado: %s, Status: %s\n", foundNode->data.name, foundNode->data.status);
    } else {
        printf("Título não encontrado.\n");
    }
}

void viewHistory() {
    printHistory();
}

void addToWatchNext() {
    char name[100];
    printf("Digite o nome do título para adicionar à fila 'Assistir a Seguir': ");
    while (getchar() != '\n');
    scanf("%[^\n]", name);

    TreeNode* bstNode = searchTreeNode(root, name);
    if (bstNode == NULL) {
        printf("Título não encontrado.\n");
        return;
    }

    enqueueWatchNext(bstNode->data);
    printf("Título adicionado à fila 'Assistir a Seguir'.\n");
}

void viewWatchNext() {
    printWatchNext();
}

void printList(Node* head, const char* listName) {
    printf("\n--- %s ---\n", listName);
    if (head == NULL) {
        printf("Lista está vazia.\n");
        return;
    }
    Node* current = head;
    while (current != NULL) {
        printf("%s\n", current->data.name);
        current = current->next;
    }
}

void freeLists() {
    freeList(toWatchList);
    freeList(watchedList);
    freeList(droppedList);
    freeTree(root);
}

void freeList(Node* head) {
    Node* current = head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

Node* removeNode(Node** head, const char* name) {
    Node* current = *head;
    Node* prev = NULL;

    while (current != NULL && strcmp(current->data.name, name) != 0) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return NULL; // Not found
    }

    if (prev == NULL) {
        *head = current->next;
    } else {
        prev->next = current->next;
    }

    current->next = NULL;

    return current;
}

void addNode(Node** head, Node* node) {
    node->next = *head;
    *head = node;
}

// Inserção na Árvore Binária de Busca. Complexidade O(log n) em média.
TreeNode* insertTreeNode(TreeNode* node, Title data) {
    if (node == NULL) {
        // Alocação dinâmica para um novo nó da árvore.
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = data;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if (strcmp(data.name, node->data.name) < 0) {
        node->left = insertTreeNode(node->left, data);
    } else if (strcmp(data.name, node->data.name) > 0) {
        node->right = insertTreeNode(node->right, data);
    }

    return node;
}

TreeNode* searchTreeNode(TreeNode* node, const char* name) {
    if (node == NULL || strcmp(node->data.name, name) == 0) {
        return node;
    }

    if (strcmp(name, node->data.name) < 0) {
        return searchTreeNode(node->left, name);
    }

    return searchTreeNode(node->right, name);
}

TreeNode* deleteTreeNode(TreeNode* node, const char* name) {
    if (node == NULL) {
        return node;
    }

    if (strcmp(name, node->data.name) < 0) {
        node->left = deleteTreeNode(node->left, name);
    } else if (strcmp(name, node->data.name) > 0) {
        node->right = deleteTreeNode(node->right, name);
    } else {
        // Nó com apenas um filho ou nenhum filho
        if (node->left == NULL) {
            TreeNode* temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            TreeNode* temp = node->left;
            free(node);
            return temp;
        }

        // Nó com dois filhos: Pega o sucessor em ordem (o menor na subárvore direita)
        TreeNode* temp = findMin(node->right);

        // Copia o conteúdo do sucessor em ordem para este nó
        node->data = temp->data;

        // Deleta o sucessor em ordem
        node->right = deleteTreeNode(node->right, temp->data.name);
    }
    return node;
}

TreeNode* findMin(TreeNode* node) {
    TreeNode* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}

// Adiciona um item à Pilha (LIFO - Last-In, First-Out).
void pushHistory(Title data) {
    // Alocação dinâmica para um novo nó da pilha.
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }
    newNode->data = data;
    newNode->next = historyStack;
    historyStack = newNode;

    historyCount++;
    if (historyCount > HISTORY_SIZE) {
        StackNode* current = historyStack;
        while (current->next->next != NULL) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
        historyCount--;
    }
}

void printHistory() {
    printf("\n--- Títulos Adicionados Recentemente ---\n");
    if (historyStack == NULL) {
        printf("Histórico está vazio.\n");
        return;
    }
    StackNode* current = historyStack;
    while (current != NULL) {
        printf("%s\n", current->data.name);
        current = current->next;
    }
}

// Adiciona um item à Fila (FIFO - First-In, First-Out).
void enqueueWatchNext(Title data) {
    // Alocação dinâmica para um novo nó da fila.
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Erro: Falha na alocação de memória.\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (rear == NULL) {
        front = rear = newNode;
        return;
    }

    rear->next = newNode;
    rear = newNode;
}

void printWatchNext() {
    printf("\n--- Fila Assistir a Seguir ---\n");
    if (front == NULL) {
        printf("Fila está vazia.\n");
        return;
    }
    QueueNode* current = front;
    while (current != NULL) {
        printf("%s\n", current->data.name);
        current = current->next;
    }
}

void saveData() {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Erro: Não foi possível abrir o arquivo para escrita.\n");
        return;
    }

    saveList(file, toWatchList);
    saveList(file, watchedList);
    saveList(file, droppedList);

    fclose(file);
    printf("Dados salvos com sucesso.\n");
}

void saveList(FILE* file, Node* head) {
    Node* current = head;
    while (current != NULL) {
        fprintf(file, "%s;%s\n", current->data.name, current->data.status);
        current = current->next;
    }
}

void loadData() {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Nenhum dado salvo encontrado.\n");
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        Title newTitle;
        char* name = strtok(line, ";");
        char* status = strtok(NULL, "\n");

        if (name != NULL && status != NULL) {
            strcpy(newTitle.name, name);
            strcpy(newTitle.status, status);

            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->data = newTitle;

            if (strcmp(status, "Para Assistir") == 0) {
                newNode->next = toWatchList;
                toWatchList = newNode;
            } else if (strcmp(status, "Assistido") == 0) {
                newNode->next = watchedList;
                watchedList = newNode;
            } else if (strcmp(status, "Abandonado") == 0) {
                newNode->next = droppedList;
                droppedList = newNode;
            }

            root = insertTreeNode(root, newTitle);
        }
    }

    fclose(file);
    printf("Dados carregados com sucesso.\n");
}

void freeTree(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    freeTree(node->left);
    freeTree(node->right);
    free(node);
} 