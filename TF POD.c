/*
 * @file main.c
 * @author: Gustavo Botezini (2311100062)
 * @author: Leonardo de Oliveira Klitzke (2311100019)
 * @brief: Codificação e decodificação de Huffman
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct huff {
  unsigned char ctr;
  int freq;
  struct huff *esquerda;
  struct huff *direita;
  struct huff *next;
} Huff;

// Funções Utilizadas.
void Cria_Lista(Huff **head, unsigned char data);
void Compara_Freq(Huff **head);
void Libera_Lista(Huff *head);
Huff *Remove_Lista(Huff **head);
int Len(Huff *Head);
void Inserir_Ordenado(Huff **head, Huff *novo);
Huff *Montar_Arvore(Huff *head);
int Altura_Arvore(Huff *raiz);
char **Aloca_Dicionario(int colunas);
void Gerar_Dicionario(Huff *raiz, char **dicionario, char *caminho, int colunas);
char *Codificar_Texto(char *filename, char **dicionario);
char *Ler_Arquivo_Codificado(const char *filename);
char *Decodificar(const char *texto_codificado, Huff *raiz);
void escreve(FILE *fp, const char *texto);

//---------------------------------------------------- MAIN
//--------------------------------------------------------//

int main(void) {
  Huff *Primeiro = NULL;
  FILE *fp = fopen("amostra.txt", "r");
  if (fp == NULL) {
    return 1;
  } else {
    int c;
    while ((c = fgetc(fp)) != EOF) {
      if (c != '\n') {
        Cria_Lista(&Primeiro, (unsigned char)c);
      }
    }
    fclose(fp);

    Compara_Freq(&Primeiro);

    Huff *ArvoreHuffman = Montar_Arvore(Primeiro);
    int colunas = Altura_Arvore(ArvoreHuffman) + 1;

    char **dicionario = Aloca_Dicionario(colunas);
    Gerar_Dicionario(ArvoreHuffman, dicionario, "", colunas);

    // Codificação do texto
    char *codificado = Codificar_Texto("amostra.txt", dicionario);
    FILE *arq = fopen("codificado.txt", "w");
    if (arq != NULL) {
      escreve(arq, codificado);
      fclose(arq);
    }
    free(codificado);

    // Decodificação do texto
    char *texto_codificado = Ler_Arquivo_Codificado("codificado.txt");
    char *decodificado = Decodificar(texto_codificado, ArvoreHuffman);
    FILE *arq2 = fopen("decodificado.txt", "w");
    if (arq2 != NULL) {
      escreve(arq2, decodificado);
      fclose(arq2);
    }
    free(texto_codificado);
    free(decodificado);

    // Libera a memória da lista e das strings
    Libera_Lista(Primeiro);

    // Libera a memória do dicionário
    for (int i = 0; i < 256; i++) {
      free(dicionario[i]);
    }
    free(dicionario);
  }
  return 0;
}

// Função para criar e adicionar os caracteres na lista de Huffman.
void Cria_Lista(Huff **head,unsigned char dado) // Utilza como base a função utilizada nos código anteriores.
{
  Huff *temp = *head;
  while (temp != NULL) {
    if (temp->ctr ==
        dado) // Caso seja o mesmo caracter, apenas incrementa a frequência.
    {
      temp->freq++;
      return;
    }
    temp = temp->next;
  }

  Huff *NovoNo =
      (Huff *)calloc(1, sizeof(Huff)); // Aloca memória para o novo nó.
  if (!NovoNo) {
    exit(1);
  }
  NovoNo->ctr = dado;
  NovoNo->freq = 1;
  NovoNo->next = NULL;

  if (*head == NULL) {
    *head = NovoNo;
  } else {
    temp = *head;
    while (temp->next != NULL) {
      temp = temp->next;
    }
    temp->next = NovoNo;
  }
}

// Função para comparar as frequências e ordenar a lista.
void Compara_Freq(Huff **head) {
  Huff *sorted = NULL;
  Huff *current = *head;
  while (current != NULL) {
    Huff *next = current->next;
    Inserir_Ordenado(&sorted, current);
    current = next;
  }
  *head = sorted;
}

// Função para liberar a memória da lista encadeada.
void Libera_Lista(Huff *head) {
  Huff *temp;
  while (head != NULL) {
    temp = head;
    head = head->next;
    free(temp);
  }
}

int Len(Huff *Head) // Função para contar o número de nós na lista encadeada.
{
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

Huff *Remove_Lista(Huff **head) // Função para remover um nó da lista encadeada.
{
  Huff *aux = NULL;
  if (*head != NULL) {
    aux = *head;
    *head = aux->next;
    aux->next = NULL;
  }
  return aux;
}

void Inserir_Ordenado(
    Huff **head,
    Huff *novo) // Função para inserir um nó na lista encadeada ordenada.
{
  if (*head == NULL || (*head)->freq >= novo->freq) // Caso a lista esteja vazia ou a frequência do novo nó seja menor ou igual
  {
    novo->next = *head;
    *head = novo;
  } else // Caso contrário, percorre a lista até encontrar a posição correta para inserir o nó.
  {
    Huff *current = *head;
    while (current->next != NULL && current->next->freq < novo->freq) // Enquanto o próximo nó tiver uma frequência menor que o novo nó
    {
      current = current->next;
    }
    novo->next = current->next;
    current->next = novo;
  }
}

Huff *Montar_Arvore(Huff *head) // Função para montar a árvore de Huffman a partir da lista encadeada.
{
  Huff *Primeiro = NULL, *Segundo = NULL;
  Huff *Aux = NULL, *ListaHuff = NULL;

  while (head != NULL) // Enquanto a lista não estiver vazia
  {
    Huff *novo =(Huff *)calloc(1, sizeof(Huff)); // Aloca memória para o novo nó.
    if (!novo) {
      exit(1);
    }
    novo->ctr = head->ctr;   // Copia o caractere do nó atual para o novo nó.
    novo->freq = head->freq; // Copia a frequência do nó atual para o novo nó.
    novo->esquerda =NULL; // Inicializa os ponteiros esquerda e direita como NULL.
    novo->direita =NULL;          // Inicializa os ponteiros esquerda e direita como NULL.
    novo->next = NULL; // Inicializa o ponteiro next como NULL.
    Inserir_Ordenado(&ListaHuff,novo); // Insere o novo nó na Árvore de Huffman.
    head = head->next;      // Move para o próximo nó na lista.
  }

  while (Len(ListaHuff) > 1) // Enquanto a lista de nós não estiver vazia
  {
    Primeiro = Remove_Lista(&ListaHuff);   // Remove o primeiro nó da lista.
    Segundo = Remove_Lista(&ListaHuff);    // Remove o segundo nó da lista.
    Aux = (Huff *)calloc(1, sizeof(Huff)); // Aloca memória para o novo nó.
    if (Aux == NULL) {
      exit(1);
    }
    Aux->ctr = '+';
    Aux->freq = Primeiro->freq + Segundo->freq; // Calcula a frequência do novo nó como a soma das frequências dos dois nós remo
    Aux->esquerda = Primeiro;  // Define o ponteiro esquerda do novo nó como o primeiro nó removido.
    Aux->direita = Segundo;    // Define o ponteiro direita do novo nó como o segundo nó removido.
    Aux->next = NULL;          // Inicializa o ponteiro next como NULL.
    Inserir_Ordenado(&ListaHuff, Aux); // Insere o novo nó na lista.
  }
  return ListaHuff;
}

int Altura_Arvore(Huff *raiz) // Percorre a árvore de Huffman e retorna a altura da árvore.
{
  if (raiz == NULL) {
    return -1;
  } else {
    int esquerda = Altura_Arvore(raiz->esquerda) +1; // Percorre a subárvore esquerda recursivamente.
    int direita = Altura_Arvore(raiz->direita) +1; // Percorre a subárvore direita recursivamente.
    return (esquerda > direita) ? esquerda: direita; // Retorna a altura máxima entre as subárvores esquerda e direita.
  }
}

char **
Aloca_Dicionario(int colunas) // Aloca memória para o dicionário de caracteres.
{
  char **dicionario = (char **)malloc(sizeof(char *) * 256); // Aloca memória para o dicionário.
  for (int i = 0; i < 256; i++) {
    dicionario[i] = calloc(colunas, sizeof(char)); // Aloca memória para cada string do dicionário.
  }
  return dicionario;
}

void Gerar_Dicionario(Huff *raiz, char **dicionario, char *caminho,int colunas) // Função para gerar o dicionário de caracteres.
{
  char esq[colunas], dir[colunas];
  if (raiz->esquerda == NULL && raiz->direita == NULL) // Verifica se o nó é uma folha.
  {
    strcpy(dicionario[raiz->ctr],caminho); // Copia o caminho atual para o dicionário.
  } else             
  {
    strcpy(esq, caminho); // Copia o caminho atual para a string esq.
    strcpy(dir, caminho); // Copia o caminho atual para a string dir.
    strcat(esq, "0");     // Concatena "0" ao caminho esq.
    strcat(dir, "1");     // Concatena "1" ao caminho dir.

    Gerar_Dicionario(raiz->esquerda, dicionario, esq,colunas); // Gera o dicionário recursivamente para a subárvore esquerda.
    Gerar_Dicionario(raiz->direita, dicionario, dir,colunas); // Gera o dicionário recursivamente para a subárvore direita.
  }
}

int tamanho_Str(char **dicionario,char *texto) // Função para calcular o tamanho do texto codificado.
{
  int i = 0, tamanho = 0;
  while (texto[i] != '\0') {
    tamanho += strlen(dicionario[texto[i]]); // Calcula o tamanho da string correspondente ao caractere no dicionário.
    i++;
  }
  return tamanho;
}

char *Codificar_Texto(char *arquivo,char **dicionario) // Função para codificar o texto.
{
  FILE *fp = fopen(arquivo, "r"); 
  if (fp == NULL) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);   // Move o ponteiro do arquivo para o final.
  long tamanho = ftell(fp); // Obtém o tamanho do arquivo.
  rewind(fp);               // Move o ponteiro do arquivo para o início.

  char *texto = (char *)calloc(tamanho + 1, sizeof(char)); // Aloca memória para o texto codificado.
  if (!texto) // Verifica se a alocação de memória foi bem-sucedida.
  {
    fclose(fp);
    return NULL;
  }

  fread(texto, sizeof(char), tamanho,fp);  // Lê o conteúdo do arquivo.
  texto[tamanho] = '\0'; // Adiciona o caractere nulo no final da string.
  fclose(fp);            // Fecha o arquivo.

  int tam = tamanho_Str(dicionario, texto); // Calcula o tamanho do texto codificado.
  char *codigo = (char *)calloc(tam + 1, sizeof(char)); // Aloca memória para o texto codificado.
  if (!codigo) {
    free(texto);
    return NULL;
  }

  for (int i = 0; texto[i] != '\0'; i++) // Percorre o texto codificado.
  {
    strcat(codigo, dicionario[texto[i]]); // Concatena o código correspondente ao caractere no texto codificado.
  }

  free(texto);
  return codigo;
}

char *Ler_Arquivo_Codificado(
const char *arquivo) // Função para ler o arquivo codificado.
{
  FILE *fp = fopen(arquivo, "r");
  if (fp == NULL) {
    return NULL;
  }

  fseek(fp, 0, SEEK_END);   // Move o ponteiro do arquivo para o final.
  long tamanho = ftell(fp); // Obtém o tamanho do arquivo.
  rewind(fp); // A função rewind() é usada para reposicionar o ponteiro do arquivo no início do arquivo.
  char *texto = (char *)calloc(tamanho + 1, sizeof(char));
  if (!texto) {
    fclose(fp);
    return NULL;
  }

  fread(texto, sizeof(char), tamanho, fp);
  texto[tamanho] = '\0';
  fclose(fp);

  return texto;
}

char *Decodificar(const char *texto_codificado,Huff *raiz) // Função para decodificar o texto codificado.
{
  Huff *atual = raiz;
  int len = strlen(texto_codificado); // Obtém o tamanho do texto codificado.
  char *decodificado = (char *)calloc(len + 1, sizeof(char)); // Aloca memória para o texto decodificado.
  if (!decodificado) {
    return NULL;
  }
  int j = 0;

  for (int i = 0; i < len; i++) // Percorre o texto codificado.
  {
    if (texto_codificado[i] =='0') // Se o caractere for '0', vai para a esquerda, caso contrário, vai para a direita.
    {
      atual = atual->esquerda;
    } else {
      atual = atual->direita;
    }

    if (atual->esquerda == NULL && atual->direita ==NULL) // Se o nó atual for uma folha, adiciona o caractere correspondente ao texto decodificado.
    {
      decodificado[j++] = atual->ctr; // Adiciona o caractere ao texto decodificado.
      atual = raiz;   // Volta para a raiz para continuar a decodificação.
    }
  }

  decodificado[j] ='\0'; // Adiciona o caractere nulo ao final do texto decodificado.
  return decodificado; // Retorna o texto decodificado.
}

void escreve(FILE *fp, const char *texto) {
  fwrite(texto, sizeof(char), strlen(texto), fp);
}