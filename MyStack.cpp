#include <TXlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef double StackElem;

//��������� ���, ����� �������� ��� 4 ����(�� ���� ����� �� �����)
#define canary1 (double)17878103347812890625
#define canary2 (double)17878103347812890600


enum StackStatus{

    SUCCESS = 0,// �������� ����������
    STACK_ERROR,
    // ������ �����
    NULL_STACK_POINTER,    // ���� �� �������� ���������
    LEFT_ATTACK,           // ����� �����
    RIGHT_ATTACK,          // ����� ������
    STACK_EMPTY,           // ���� ����
    STACK_OVERFLOW,        // ������������ �����
    MEMORY_ALLOCATION_ERROR // ������ ��������� ������

};

struct MyStack
{
    StackElem* data;         // ������ ��� �������� ���������
    size_t capacity;         // ����������� ����� �� ������ ������
    size_t read_size;        // ������� ������ ����� (������ �� �������� ���������)
    size_t element_size;     // ������ �������� � ������
};


int MyStackInit(MyStack* st, size_t initial_capacity);// ������������� �����
int MyStackRealloc(MyStack* st);// ���������� ������� data �����
int MyPush(MyStack* st, StackElem const value);// ���������� �������� � ����
int MyPop(MyStack* stack, StackElem* value);// ���������� �������� �� �����
int MyStackTop(MyStack* st, StackElem* value); //�������� �������� ��������
int MyStackVeryFun(MyStack* st);//�������� ������������� ��������
void MyStackDump(MyStack* st); //������ ���� ������ �� ���� ����� �����
void MyStackDestroy(MyStack* st);// ����������� �����

int main()
{
    printf("=== ������������ ����� ===\n");

    // ���� 1: ������������� �����
    printf("\n1. ���� ������������� �����:\n");
    MyStack stk1 = {};
    int result = MyStackInit(&stk1, 5);
    printf("�������������: %s\n", result == SUCCESS ? "�����" : "������");
    MyStackDump(&stk1);

    // ���� 2: ���������� ���������
    printf("\n2. ���� ���������� ���������:\n");
    for (int i = 1; i <= 7; i++) {
        result = MyPush(&stk1, i * 1.5);
        printf("Push %d: %s (��������: %.1f)\n", i,
               result == SUCCESS ? "�����" : "������", i * 1.5);
        if (result != SUCCESS) break;
    }
    MyStackDump(&stk1);

    // ���� 3: �������� �������� ��������
    printf("\n3. ���� ��������� �������� ��������:\n");
    StackElem top_value;
    result = MyStackTop(&stk1, &top_value);
    if (result == SUCCESS) {
        printf("������� �������: %.1f\n", top_value);
    } else {
        printf("������ ��� ��������� �������� ��������: %d\n", result);
    }

    // ���� 4: ���������� ���������
    printf("\n4. ���� ���������� ���������:\n");
    StackElem popped_value;
    for (int i = 0; i < 3; i++) {
        result = MyPop(&stk1, &popped_value);
        printf("Pop %d: %s (���������: %.1f)\n", i + 1,
               result == SUCCESS ? "�����" : "������", popped_value);
        if (result != SUCCESS) break;
    }
    MyStackDump(&stk1);


    // ���� 6: �������� ��������
    printf("\n6. ���� ����������� ��������:\n");
    result = MyStackVeryFun(&stk1);
    printf("�������� ��������: %s\n", result == SUCCESS ? "���" : "���������");

    // ���� 7: ��������� ����������
    printf("\n7. ���� ���������� ����������:\n");
    for (int i = 1; i <= 3; i++) {
        MyPush(&stk1, i * 2.5);
        printf("Push %.1f\n", i * 2.5);
    }
    MyStackDump(&stk1);

    // ���� 8: ����������� �����
    printf("\n8. ���� ����������� �����:\n");
    MyStackDestroy(&stk1);
    printf("���� ���������\n");

    // ���� 9: �������� �� ������� ���������
    printf("\n9. ���� �������� ���������:\n");
    result = MyStackVeryFun(NULL);
    printf("�������� NULL �����: %s (��� ������: %d)\n",
           result != SUCCESS ? "��������� ������" : "����������� �����", result);

    printf("\n=== ������������ ��������� ===\n");

/*    MyStack stk1 = {};
    MyStackInit (&stk1, 5);
    MyStackDestroy(&stk1);  */
    return 0;
}

int MyStackInit(MyStack* st, size_t initial_capacity)// ������������� �����
{
    assert(st != NULL);

    if (initial_capacity == 0) initial_capacity = 10;  // ������� �� ���������
    initial_capacity += 2;

    st->data = (StackElem*)calloc(initial_capacity, sizeof(StackElem));
    if (st->data == NULL) return MEMORY_ALLOCATION_ERROR;      // �� ������� ������

    st->data[0] = canary1;
    st->data[initial_capacity - 1] = canary2;
    st->capacity = initial_capacity;
    st->read_size = 1;
    st->element_size = sizeof(StackElem);

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}

int MyStackRealloc(MyStack* st)// ���������� ������� data �����
{
    if (MyStackVeryFun(st))  return ERROR;

    st->data[st->capacity - 1] = 0;
    size_t new_capacity = (st->capacity) * 2;
    StackElem* new_data = (StackElem*)realloc(st->data, new_capacity*sizeof(StackElem));
    if (new_data == NULL) return MEMORY_ALLOCATION_ERROR;// �� ������� ������


    st->data = new_data;
    st->capacity = new_capacity;
    st->data[new_capacity - 1] = canary2;

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}


int MyPush(MyStack* st, StackElem const value)// ���������� �������� � ����
{
    if (MyStackVeryFun(st)) return ERROR;

    if (st->read_size >= st->capacity-1)// ����������� �������, ���� ������ ����� ������
    {

        if (MyStackRealloc(st) != 0) return MEMORY_ALLOCATION_ERROR; // �� ������� �������� ������
    }

    st->data[st->read_size] = value;
    st->read_size++;
    if (MyStackVeryFun(st))  return ERROR;
    return SUCCESS;
}

int MyPop(MyStack* st, StackElem* value)// ���������� �������� �� �����
{
    if (MyStackVeryFun(st) || st->read_size == 1) return ERROR;
    assert(value != NULL);

    st->read_size--;
    *value = st->data[st->read_size];

    if (MyStackVeryFun(st)) return ERROR;
    return SUCCESS;
}

int MyStackTop(MyStack* st, StackElem* value)//�������� �������� ��������
{
    if (MyStackVeryFun(st))  return ERROR;
    assert(value != NULL);

    *value = st->data[st->read_size-1];
    if (MyStackVeryFun(st))  return ERROR;
    return SUCCESS;
}

void MyStackDestroy(MyStack* st)// ����������� �����
{
    assert(st != NULL);

    for(int i = 0; i < (st->read_size)+1; i++)
    {
        st->data[i] = 0;      // ������� ������ ������ ������a ��� �������� ���������
    }
    st->capacity = 0;         // ����������� ����� �� ������ ������
    st->read_size = 0;        // ������� ������ ����� (������ �� �������� ���������)
    st->element_size = 0;
    st->data = NULL;
    /*if (st->data != NULL) free(st->data);*/ //���� ��� ���������?
}


void MyStackDump(MyStack* st)// ������ ���� ����� �����
{
    assert(st != NULL);
    printf("���� (������� ���������: %ld, �������: %ld, ������ ��������(����): %ld):\n",
     st->read_size-1, st->capacity, st->element_size);

    for (size_t i = 1; i < st->read_size; i++)
    {
        printf("%lf ", st->data[i]);//����� ������������, ���� �������� ��� ������?
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

