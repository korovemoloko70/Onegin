#include <TXlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef double StackElem;

//подобраны так, чтобы занимать все 4 бита(не было нулей по краям)
#define canary1 (double)17878103347812890625
#define canary2 (double)17878103347812890600


enum StackStatus{

    SUCCESS = 0,// Успешное выполнение
    STACK_ERROR,
    // Ошибки стека
    NULL_STACK_POINTER,    // Стек по нулевому указателю
    LEFT_ATTACK,           // Атака слева
    RIGHT_ATTACK,          // Атака справа
    STACK_EMPTY,           // Стек пуст
    STACK_OVERFLOW,        // Переполнение стека
    MEMORY_ALLOCATION_ERROR // Ошибка выделения памяти

};

struct MyStack
{
    StackElem* data;         // массив для хранения элементов
    size_t capacity;         // вместимость стека на данный момент
    size_t read_size;        // текущий размер стека (клеток из капасити заполнено)
    size_t element_size;     // размер элемента в байтах
};


int MyStackInit(MyStack* st, size_t initial_capacity);// Инициализация стека
int MyStackRealloc(MyStack* st);// Увеличение емкости data стека
int MyPush(MyStack* st, StackElem const value);// Добавление элемента в стек
int MyPop(MyStack* stack, StackElem* value);// Извлечение элемента из стека
int MyStackTop(MyStack* st, StackElem* value); //просмотр верхнего элемента
int MyStackVeryFun(MyStack* st);//Проверка испорченности канареек
void MyStackDump(MyStack* st); //печать всех данных из всех полей стека
void MyStackDestroy(MyStack* st);// Уничтожение стека

int main()
{
    printf("=== ТЕСТИРОВАНИЕ СТЕКА ===\n");

    // Тест 1: Инициализация стека
    printf("\n1. Тест инициализации стека:\n");
    MyStack stk1 = {};
    int result = MyStackInit(&stk1, 5);
    printf("Инициализация: %s\n", result == SUCCESS ? "УСПЕХ" : "ОШИБКА");
    MyStackDump(&stk1);

    // Тест 2: Добавление элементов
    printf("\n2. Тест добавления элементов:\n");
    for (int i = 1; i <= 7; i++) {
        result = MyPush(&stk1, i * 1.5);
        printf("Push %d: %s (значение: %.1f)\n", i,
               result == SUCCESS ? "УСПЕХ" : "ОШИБКА", i * 1.5);
        if (result != SUCCESS) break;
    }
    MyStackDump(&stk1);

    // Тест 3: Просмотр верхнего элемента
    printf("\n3. Тест просмотра верхнего элемента:\n");
    StackElem top_value;
    result = MyStackTop(&stk1, &top_value);
    if (result == SUCCESS) {
        printf("Верхний элемент: %.1f\n", top_value);
    } else {
        printf("Ошибка при получении верхнего элемента: %d\n", result);
    }

    // Тест 4: Извлечение элементов
    printf("\n4. Тест извлечения элементов:\n");
    StackElem popped_value;
    for (int i = 0; i < 3; i++) {
        result = MyPop(&stk1, &popped_value);
        printf("Pop %d: %s (извлечено: %.1f)\n", i + 1,
               result == SUCCESS ? "УСПЕХ" : "ОШИБКА", popped_value);
        if (result != SUCCESS) break;
    }
    MyStackDump(&stk1);


    // Тест 6: Проверка канареек
    printf("\n6. Тест целостности канареек:\n");
    result = MyStackVeryFun(&stk1);
    printf("Проверка канареек: %s\n", result == SUCCESS ? "ЦЕЛ" : "ПОВРЕЖДЕН");

    // Тест 7: Повторное добавление
    printf("\n7. Тест повторного добавления:\n");
    for (int i = 1; i <= 3; i++) {
        MyPush(&stk1, i * 2.5);
        printf("Push %.1f\n", i * 2.5);
    }
    MyStackDump(&stk1);

    // Тест 8: Уничтожение стека
    printf("\n8. Тест уничтожения стека:\n");
    MyStackDestroy(&stk1);
    printf("Стек уничтожен\n");

    // Тест 9: Проверка на нулевой указатель
    printf("\n9. Тест нулевого указателя:\n");
    result = MyStackVeryFun(NULL);
    printf("Проверка NULL стека: %s (код ошибки: %d)\n",
           result != SUCCESS ? "ОЖИДАЕМАЯ ОШИБКА" : "НЕОЖИДАННЫЙ УСПЕХ", result);

    printf("\n=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n");

/*    MyStack stk1 = {};
    MyStackInit (&stk1, 5);
    MyStackDestroy(&stk1);  */
    return 0;
}

int MyStackInit(MyStack* st, size_t initial_capacity)// Инициализация стека
{
    assert(st != NULL);

    if (initial_capacity == 0) initial_capacity = 10;  // емкость по умолчанию
    initial_capacity += 2;

    st->data = (StackElem*)calloc(initial_capacity, sizeof(StackElem));
    if (st->data == NULL) return MEMORY_ALLOCATION_ERROR;      // не хватило памяти

    st->data[0] = canary1;
    st->data[initial_capacity - 1] = canary2;
    st->capacity = initial_capacity;
    st->read_size = 1;
    st->element_size = sizeof(StackElem);

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}

int MyStackRealloc(MyStack* st)// Увеличение емкости data стека
{
    if (MyStackVeryFun(st))  return ERROR;

    st->data[st->capacity - 1] = 0;
    size_t new_capacity = (st->capacity) * 2;
    StackElem* new_data = (StackElem*)realloc(st->data, new_capacity*sizeof(StackElem));
    if (new_data == NULL) return MEMORY_ALLOCATION_ERROR;// не хватило памяти


    st->data = new_data;
    st->capacity = new_capacity;
    st->data[new_capacity - 1] = canary2;

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}


int MyPush(MyStack* st, StackElem const value)// Добавление элемента в стек
{
    if (MyStackVeryFun(st)) return ERROR;

    if (st->read_size >= st->capacity-1)// увеличиваем емкость, если класть новое некуда
    {

        if (MyStackRealloc(st) != 0) return MEMORY_ALLOCATION_ERROR; // не удалось выделить память
    }

    st->data[st->read_size] = value;
    st->read_size++;
    if (MyStackVeryFun(st))  return ERROR;
    return SUCCESS;
}

int MyPop(MyStack* st, StackElem* value)// Извлечение элемента из стека
{
    if (MyStackVeryFun(st) || st->read_size == 1) return ERROR;
    assert(value != NULL);

    st->read_size--;
    *value = st->data[st->read_size];

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}

int MyStackTop(MyStack* st, StackElem* value)//просмотр верхнего элемента
{
    if (MyStackVeryFun(st))  return ERROR;
    assert(value != NULL);

    *value = st->data[st->read_size-1];
    if (MyStackVeryFun(st))  return ERROR;
    return SUCCESS;
}

void MyStackDestroy(MyStack* st)// Уничтожение стека
{
    assert(st != NULL);

    for(int i = 0; i < (st->read_size)+1; i++)
    {
        st->data[i] = 0;      // сделать нулями клетки массивa для хранения элементов
    }
    st->capacity = 0;         // вместимость стека на данный момент
    st->read_size = 0;        // текущий размер стека (клеток из капасити заполнено)
    st->element_size = 0;
    st->data = NULL;
    /*if (st->data != NULL) free(st->data);*/ //надо или обнуление?
}


void MyStackDump(MyStack* st)// Печать всех полей стека
{
    assert(st != NULL);
    printf("Стек (считано элементов: %ld, емкость: %ld, размер элемента(байт): %ld):\n",
     st->read_size-1, st->capacity, st->element_size);

    for (size_t i = 1; i < st->read_size; i++)
    {
        printf("%lf ", st->data[i]);//КАКОЙ СПЕЦИФИКАТОР, ЕСЛИ СТАКЭЛЕМ ТИП ДАННЫХ?
    }

    printf("\n");
}


int MyStackVeryFun(MyStack* st)
{
    if (st == NULL)
    {
        printf("NULL_STACK_POINTER\n");
        return NULL_STACK_POINTER;
    }

    if (st->data[0] != canary1)
    {
        printf("LEFT_ATTACK\n");
        return LEFT_ATTACK;
    }

    if(st->data[st->capacity - 1] != canary2)
    {
        printf("RIGHT_ATTACK\n");
        return RIGHT_ATTACK;
    }

    if (st->read_size == 0)
    {
        printf("STACK_EMPTY\n");
        return STACK_EMPTY;
    }

    if (st->read_size >= st->capacity)
    {
        printf("STACK_OVERFLOW\n");
        return STACK_OVERFLOW;
    }

    return SUCCESS;
}

