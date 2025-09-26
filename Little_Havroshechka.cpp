#include <TXlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Swap(char** a, char** b);   // чтобы изменить сам указатель а не то, что что он см
void BubbleSort(char** arr, long int n);
long int OpenAndGetFileSize(FILE** file, const char* filename);
char* ReadFileIntoBuffer(FILE* file, long int fileLength, long int* lineCount);
char** CreatePointersArray (char* buffer, size_t fileLength, long int lineCount);

int main()
{
    FILE* file = NULL;
    FILE* outputFile = NULL;
    char* buffer = NULL;
    char** pointers = NULL;
    long int lineCount = 0; //шт строк

    long int fileLength = OpenAndGetFileSize(&file, "Evgeniy_Onegin.txt");
    if (fileLength <= 0)
    {
        printf("Не удалось открыть файл или файл пуст\n");
        return 1;
    }
    assert(file != NULL);

    buffer = ReadFileIntoBuffer(file, fileLength, &lineCount);
    if (buffer == NULL)
    {
        printf("Ошибка выделения памяти для буфера\n");
        return 1;
    }

    pointers = CreatePointersArray(buffer, (size_t) fileLength, lineCount);
    if (pointers == NULL)
    {
        printf("Ошибка выделения памяти для указателей\n");
        free(buffer);
        return 1;
    }

    BubbleSort(pointers, lineCount);

    outputFile = fopen("sortedOnegin.txt", "w");
    if (outputFile == NULL)
    {
        printf("Не удалось создать файл sortedOnegin.txt\n");
        free(buffer);
        free(pointers);
        return 1;
    }

    for (int i = 0; i < lineCount; i++)
        fprintf(outputFile, "%s\n", pointers[i]);

    fclose(outputFile);
    free(buffer);
    free(pointers);

    printf("Отсортированные строки записаны в файл sortedOnegin.txt\n");

    return 0;
}



long int OpenAndGetFileSize(FILE** file, const char* filename)
{
    assert(file != NULL);
    assert(filename != NULL);

    *file = fopen(filename, "r");
    if (*file == NULL)
        return -1;

    fseek(*file, 0, SEEK_END);
    long fileLength = ftell(*file);
    fseek(*file, 0, SEEK_SET);

    return fileLength;
}

char* ReadFileIntoBuffer(FILE* file, long int fileLength, long int* lineCount)
{

    assert(file != NULL);
    assert(lineCount != NULL);
    assert(fileLength > 0);

    char* buffer = (char*)calloc(fileLength + 1, sizeof(char));
    if (buffer == NULL)
        return NULL;

    *lineCount = 1;

    size_t bytesRead = fread(buffer, 1, fileLength, file);
    buffer[bytesRead] = '\0';

    for (size_t i = 0; i < bytesRead; i++)
        if (buffer[i] == '\n')
            (*lineCount)++;
    fclose(file);
    return buffer;
}

char** CreatePointersArray (char* buffer, size_t fileLength, long int lineCount)
{
    assert(buffer != NULL);
    assert(fileLength > 0);
    assert(lineCount > 0);

    char** pointers = (char**)calloc(lineCount + 1, sizeof(char*));
    if (pointers == NULL)
        return NULL;

    int pointerIndex = 0;
    pointers[pointerIndex++] = buffer;

    for (size_t i = 0; i < fileLength; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
            if (i + 1 < fileLength)
                pointers[pointerIndex++] = &buffer[i + 1];
        }
    }
    pointers[pointerIndex] = NULL;

    return pointers;
}


void BubbleSort(char** arr, long int n)
{
    assert(arr != NULL);

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (strcmp(arr[j], arr[j + 1]) > 0) Swap(&arr[j], &arr[j + 1]);  // 0, если строки =; положительное число, если 1>2 лексикограф; отриц, если 1<2
        }
    }
    return;
}


void Swap(char** a, char** b)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(*a != NULL);
    assert(*b != NULL);

    char* temp = *a;
    *a = *b;
    *b = temp;
    return;
}
