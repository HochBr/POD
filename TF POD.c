/*
 * @file main.c
 * @author: Gustavo Botezini (2311100062)
 * @author: Leonardo de Oliveira Klitzke (2311100019)
 * @brief: Codificação e decodificação de Huffman
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct para a árvore de Huffman
typedef struct huff {
    unsigned char ctr;
    int freq;
    struct huff *esquerda;
    struct huff *direita;
    struct huff *next;
} Huff;

// Function declarations
void Cria_Lista(Huff **head, unsigned char data);
void Frequencia(Huff **head);
void Compara_Freq(Huff *head);
void Compara_Caractere(Huff *head);
void Libera_Lista(Huff *head);
Huff *Remove_Lista(Huff **head);
int Len(Huff *Head);
Huff *Remove_Lista(Huff **head);
void Inserir_Ordenado(Huff **head, Huff *novo);
Huff *Montar_Arvore(Huff *head);
void Imprime_Arvore(Huff *raiz, int tamanho);
int Altura_Arvore(Huff *raiz);
char **Aloca_Dicionario(int colunas);
void Gerar_Dicionario(Huff *raiz, char **dicionario, char *caminho, int colunas);
void Imprime_Dicionario(char **dicionario);
char* Escrever_Arquivo(char **dicionario, Huff *texto);
void escreve(FILE *fp, const char *texto);
char* Codificar_Texto(char *texto, char **dicionario);
char* Ler_Arquivo_Codificado(const char *filename);
char* Decodificar(const char *texto_codificado, Huff *raiz);

//---------------------------------------------------- MAIN --------------------------------------------------------//

int main(void) {
    Huff *Primeiro = NULL;
    FILE *fp = fopen("amostra.txt", "rb"); // Abre o arquivo em modo binário
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    } else {
        printf("Arquivo aberto com sucesso!\n");
        int c;
        while ((c = fgetc(fp)) != EOF) {
            if (c != '\n') {
                Cria_Lista(&Primeiro, (unsigned char)c);
            }
        }
        fclose(fp); // Fecha o arquivo de leitura
        Frequencia(&Primeiro);
        Compara_Freq(Primeiro);      // Descomente para ordenar por frequência
        Compara_Caractere(Primeiro); // Ordenar por caractere

        Huff *ArvoreHuffman = Montar_Arvore(Primeiro);
        printf("Huffman montado com sucesso!\n");
        Imprime_Arvore(ArvoreHuffman, 1);
        int colunas = Altura_Arvore(ArvoreHuffman) + 1;
        printf("Tamanho: %d\n", colunas);

        char **dicionario = Aloca_Dicionario(colunas);
        Gerar_Dicionario(ArvoreHuffman, dicionario, "", colunas);
        Imprime_Dicionario(dicionario);

        printf("Altura da arvore: %d\n", Altura_Arvore(ArvoreHuffman));

        // Codificação do texto
        char *codificado = Codificar_Texto("amostra.txt", dicionario);
        FILE *arq = fopen("codificado.txt", "w");
        if (arq == NULL) {
            printf("Erro ao abrir o arquivo.\n");
        } else {
            printf("Arquivo aberto com sucesso!\n");
            escreve(arq, codificado);
            fclose(arq);
        }
        free(codificado);

        // Decodificação do texto
        char *texto_codificado = Ler_Arquivo_Codificado("codificado.txt");
        
        char *decodificado = Decodificar(texto_codificado, ArvoreHuffman);
        printf("Texto Decodificado: %s\n", decodificado);
        FILE *arq2 = fopen("decodificado.txt", "w");
        if (arq == NULL) {
            printf("Erro ao abrir o arquivo.\n");
        } else {
            printf("Arquivo aberto com sucesso!\n");
            escreve(arq, decodificado);
            fclose(arq);
        }
        free(codificado);
        // Libera a memória da lista e das strings
        Libera_Lista(Primeiro);
        free(texto_codificado);
        free(decodificado);

        // Libera a memória do dicionário
        for (int i = 0; i < 256; i++) {
            free(dicionario[i]);
        }
        free(dicionario);
    }
    return 0;
}

// Função para criar e adicionar os caracteres na lista de Huffman
void Cria_Lista(Huff **head, unsigned char data) {
    Huff *temp = *head;
    while (temp != NULL) {
        if (temp->ctr == data) {
            temp->freq++;
            return;
        }
        temp = temp->next;
    }

    Huff *newNode = (Huff *)calloc(1, sizeof(Huff));
    if (!newNode) {
        printf("Erro na função Cria_Lista ou erro de alocação\n");
        exit(1);
    }
    newNode->ctr = data;
    newNode->freq = 1;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Função para processar a lista e consolidar as frequências dos caracteres
void Frequencia(Huff **head) {
    printf("Chegou na função Frequencia\n");
    Huff *atual = *head;
    while (atual != NULL) {
        Huff *percorre = atual;
        while (percorre->next != NULL) {
            if (percorre->next->ctr == atual->ctr) {
                atual->freq += percorre->next->freq;
                Huff *temp = percorre->next;
                percorre->next = percorre->next->next;
                free(temp);
            } else {
                percorre = percorre->next;
            }
        }
        atual = atual->next;
    }
}

// Função para comparar as frequências e ordenar a lista
void Compara_Freq(Huff *head) {
    printf("Comparando frequências...\n");
    if (head == NULL)
        return;

    Huff *atual = head;
    while (atual != NULL) {
        Huff *minNode = atual;
        Huff *nextNode = atual->next;

        while (nextNode != NULL) {
            if (nextNode->freq < minNode->freq) {
                minNode = nextNode;
            }
            nextNode = nextNode->next;
        }

        if (minNode != atual) {
            int tempFreq = atual->freq;
            unsigned char tempCtr = atual->ctr;
            atual->freq = minNode->freq;
            atual->ctr = minNode->ctr;
            minNode->freq = tempFreq;
            minNode->ctr = tempCtr;
        }

        atual = atual->next;
    }
}

// Função para comparar os caracteres e ordenar a lista
void Compara_Caractere(Huff *head) {
    printf("Comparando caracteres...\n");
    if (head == NULL)
        return;

    Huff *atual = head;
    while (atual != NULL) {
        Huff *minNode = atual;
        Huff *nextNode = atual->next;

        while (nextNode != NULL) {
            if (nextNode->freq == atual->freq && nextNode->ctr < minNode->ctr) {
                minNode = nextNode;
            }
            nextNode = nextNode->next;
        }

        if (minNode != atual) {
            unsigned char tempCtr = atual->ctr;
            int tempFreq = atual->freq;
            atual->ctr = minNode->ctr;
            atual->freq = minNode->freq;
            minNode->ctr = tempCtr;
            minNode->freq = tempFreq;
        }

        atual = atual->next;
    }
}

// Função para liberar a memória da lista encadeada
void Libera_Lista(Huff *head) {
    printf("Liberando memória...\n");
    Huff *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int Len(Huff *Head) {
    if (Head == NULL) {
        return 0;
    }
    int count = 0;
    Huff *Aux = Head;
    while (Aux != NULL) {
        count++;
        Aux = Aux->next;
    }
    return count;
}

Huff *Remove_Lista(Huff **head) {
    Huff *aux = NULL;
    if (*head != NULL) {
        aux = *head;
        *head = aux->next;
        aux->next = NULL;
    }
    return aux;
}

// Inserir um nó Huff na lista de forma ordenada por frequência
void Inserir_Ordenado(Huff **head, Huff *novo) {
    if (*head == NULL || (*head)->freq >= novo->freq) {
        novo->next = *head;
        *head = novo;
    } else {
        Huff *current = *head;
        while (current->next != NULL && current->next->freq < novo->freq) {
            current = current->next;
        }
        novo->next = current->next;
        current->next = novo;
    }
}

Huff *Montar_Arvore(Huff *head) {
    printf("Montando a arvore de Huffman\n");
    Huff *Primeiro = NULL, *Segundo = NULL;
    Huff *Aux = NULL, *ListaHuff = NULL;

    while (head != NULL) {
        Huff *novo = (Huff *)malloc(sizeof(Huff));
        if (!novo) {
            printf("Erro ao alocar memória\n");
            exit(1);
        }
        novo->ctr = head->ctr;
        novo->freq = head->freq;
        novo->esquerda = NULL;
        novo->direita = NULL;
        novo->next = NULL;
        Inserir_Ordenado(&ListaHuff, novo);
        head = head->next;
    }

    while (Len(ListaHuff) > 1) {
        Primeiro = Remove_Lista(&ListaHuff);
        Segundo = Remove_Lista(&ListaHuff);
        Aux = (Huff *)malloc(sizeof(Huff));
        if (Aux == NULL) {
            printf("Erro ao alocar memória\n");
            exit(1);
        }
        Aux->ctr = '+';
        Aux->freq = Primeiro->freq + Segundo->freq;
        Aux->esquerda = Primeiro;
        Aux->direita = Segundo;
        Aux->next = NULL;
        Inserir_Ordenado(&ListaHuff, Aux);
    }
    return ListaHuff;
}

void Imprime_Arvore(Huff *raiz, int tamanho) {
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        printf("Folha: %c, Tamanho: %d\n", raiz->ctr, tamanho);
    } else {
        Imprime_Arvore(raiz->esquerda, tamanho + 1);
        Imprime_Arvore(raiz->direita, tamanho + 1);
    }
}

int Altura_Arvore(Huff *raiz) {
    if (raiz == NULL) {
        return -1;
    } else {
        int esquerda = Altura_Arvore(raiz->esquerda) + 1;
        int direita = Altura_Arvore(raiz->direita) + 1;
        return (esquerda > direita) ? esquerda : direita;
    }
}

char **Aloca_Dicionario(int colunas) {
    char **dicionario = (char **)malloc(sizeof(char *) * 256);
    for (int i = 0; i < 256; i++) {
        dicionario[i] = calloc(colunas, sizeof(char));
    }
    return dicionario;
}

void Gerar_Dicionario(Huff *raiz, char **dicionario, char *caminho, int colunas) {
    char esq[colunas], dir[colunas];
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        strcpy(dicionario[raiz->ctr], caminho);
    } else {
        strcpy(esq, caminho);
        strcpy(dir, caminho);
        strcat(esq, "0");
        strcat(dir, "1");

        Gerar_Dicionario(raiz->esquerda, dicionario, esq, colunas);
        Gerar_Dicionario(raiz->direita, dicionario, dir, colunas);
    }
}

void Imprime_Dicionario(char **dicionario) {
    printf("Imprimindo o dicionario\n");
    for (int i = 0; i < 256; i++) {
        if (strlen(dicionario[i]) > 0) {
            printf("Caractere: %c, Código: %s\n", i, dicionario[i]);
        }
    }
}

int tamanho_Str(char **dicionario, char *texto) {
    int i = 0, tamanho = 0;
    while (texto[i] != '\0') {
        tamanho += strlen(dicionario[texto[i]]);
        i++;
    }
    return tamanho;
}

char* Codificar_Texto(char *filename, char **dicionario) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo para codificação.\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    rewind(fp);

    char *texto = (char *)malloc(tamanho + 1);
    if (!texto) {
        printf("Erro de alocação de memória.\n");
        fclose(fp);
        return NULL;
    }

    fread(texto, sizeof(char), tamanho, fp);
    texto[tamanho] = '\0';
    fclose(fp);

    int tam = tamanho_Str(dicionario, texto);
    char *codigo = (char *)calloc(tam + 1, sizeof(char)); // +1 para o caractere nulo final
    if (!codigo) {
        printf("Erro de alocação de memória.\n");
        free(texto);
        return NULL;
    }

    for (int i = 0; texto[i] != '\0'; i++) {
        strcat(codigo, dicionario[texto[i]]);
    }

    free(texto);
    return codigo;
}

char* Ler_Arquivo_Codificado(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo codificado.\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    rewind(fp);

    char *texto = (char *)malloc(tamanho + 1);
    if (!texto) {
        printf("Erro de alocação de memória.\n");
        fclose(fp);
        return NULL;
    }

    fread(texto, sizeof(char), tamanho, fp);
    texto[tamanho] = '\0';
    fclose(fp);

    return texto;
}

char* Decodificar(const char *texto_codificado, Huff *raiz) {
    Huff *atual = raiz;
    int len = strlen(texto_codificado);
    char *decodificado = (char *)malloc(len + 1); // +1 para o caractere nulo final
    if (!decodificado) {
        printf("Erro de alocação de memória.\n");
        return NULL;
    }
    int j = 0;

    for (int i = 0; i < len; i++) {
        if (texto_codificado[i] == '0') {
            atual = atual->esquerda;
        } else {
            atual = atual->direita;
        }

        if (atual->esquerda == NULL && atual->direita == NULL) {
            decodificado[j++] = atual->ctr;
            atual = raiz;
        }
    }

    decodificado[j] = '\0';
    return decodificado;
}

void escreve(FILE *fp, const char *texto) {
    size_t len = strlen(texto);
    if (fwrite(texto, sizeof(char), len, fp) != len) {
        fprintf(stderr, "Erro ao escrever no arquivo.\n");
    }
}

