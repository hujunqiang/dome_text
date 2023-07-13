#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 链表节点结构
typedef struct Node {
    char code[20];//用于存储字符串的字符数组
    struct Node* next;
} Node;

// 创建节点
Node* createNode(const char* code) {
    //将 malloc 返回的内存地址强制转换为节点类型的指针。
    Node* newNode = (Node*)malloc(sizeof(Node));//返回分配内存的首地址
    //使用 strcpy 函数将参数 code 的字符串内容复制到新节点的 code 字符数组中。
    //这里假设节点结构体中有一个名为 code 的字符数组用于存储字符串。
    strcpy(newNode->code, code);
    newNode->next = NULL;//将新节点的 next 指针设置为 NULL，表示该节点没有指向其他节点。
    return newNode;//返回指向新节点的指针 newNode，以便在其他地方使用该节点。
}

// 在链表末尾插入节点
void insertEnd(Node** head, const char* code) {
    //调用了前面提到的 createNode 函数来创建一个新的节点，
    //并将其赋值给 newNode 变量。这个新节点将包含给定的代码字符串。
    Node* newNode = createNode(code);

    if (*head == NULL) {// 如果链表为空，将新节点 newNode 设置为链表的头节点。
        *head = newNode;
    } else {
        //创建一个临时指针变量 temp，并将其指向链表的头节点。
        //这个临时指针用于遍历链表，找到链表的末尾节点。
        Node* temp = *head;
        while (temp->next != NULL) {
            // 使用一个循环来遍历链表，条件是当前节点的 next 指针不为空。
            //这意味着只要当前节点不是链表的最后一个节点，就继续循环。
            temp = temp->next;
        } 
        //将新节点 newNode 连接到最后一个节点的 next 指针上，将新节点插入到链表的末尾。
        temp->next = newNode;
    }
}

// 查找节点
//接受一个链表的头节点指针 head 和一个字符串 code
int searchNode(Node* head, const char* code) {
//创建一个临时指针变量 temp，并将其初始化为链表的头节点。
//这个临时指针用于遍历链表，从头节点开始。
    Node* temp = head;
    while (temp != NULL) 
    //只要当前节点不是链表的最后一个节点，就继续循环。
    {//将当前节点的 code 字符串与给定的 code 字符串进行比较。
    //如果两个字符串相等（即返回值为0），表示找到了匹配的节点。
        if (strcmp(temp->code, code) == 0) {
            //如果找到了匹配的节点，函数返回1，表示节点存在于链表中。
            return 1; // 找到节点
        }//在每次循环结束时，将 temp 指针移动到链表的下一个节点，继续搜索下一个节点。
        temp = temp->next;
    }
    // 如果循环结束后仍未找到匹配的节点，
    //表示节点不存在于链表中，函数返回0，表示节点不存在。
    return 0; // 未找到节点
}

// 删除节点
void deleteNode(Node** head, const char* code) {
    Node* temp = *head;
    Node* prev = NULL;
    // 处理头节点
    if (temp != NULL && strcmp(temp->code, code) == 0) {
        *head = temp->next;
        free(temp);
        return;
    }
    // 遍历链表查找要删除的节点
    while (temp != NULL && strcmp(temp->code, code) != 0) {
        prev = temp;
        temp = temp->next;
    }
    // 删除节点
    if (temp != NULL) {
        prev->next = temp->next;
        free(temp);
    }
}

// 将新节点插入链表末尾，超过指定长度则溢出
void insertOrUpdateNode(Node** head, const char* code, int maxLength) {
    // 查找节点是否已存在
    if (searchNode(*head, code)) {
        // 若存在则先删除再插入
        deleteNode(head, code);
    } else if (maxLength > 0) {
        // 若不存在且指定长度大于0，则判断队列长度是否超过指定长度
        Node* temp = *head;
        Node* prev = NULL;
        int count = 0;
        while (temp != NULL) {
            prev = temp;
            temp = temp->next;
            count++;
        }
        if (count >= maxLength) {
            // 队列长度超过指定长度，删除队列首个节点
            Node* temp = *head;
            *head = (*head)->next;
            free(temp);
        }
    }
    // 将新节点插入链表末尾
    insertEnd(head, code);
}

// 打印链表
void printList(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        printf("%s ", temp->code);
        temp = temp->next;
    }
    printf("\n");
}

int main() {
    Node* head = NULL; // 链表头节点
    int maxLength = 5; // 指定队列长度

    insertOrUpdateNode(&head, "X1", maxLength);
    insertOrUpdateNode(&head, "X2", maxLength);
    insertOrUpdateNode(&head, "X3", maxLength);
    insertOrUpdateNode(&head, "X4", maxLength);
    insertOrUpdateNode(&head, "X5", maxLength);
    insertOrUpdateNode(&head, "X6", maxLength);
    insertOrUpdateNode(&head, "X2", maxLength);
    insertOrUpdateNode(&head, "X7", maxLength);

    printf("list : ");
    printList(head);

    return 0;
}




/*

设*p0指向一个由若干字符串代码构成的先进先出的链表队列，所述队列初始为空，每个单元变量为一个字符串代码 X。
请尝试以下数据处理，使得当新增一个新的字符串代码 X1 时:
1)查找判断 X1 是否已经存在于队列;
2)若不存在，将 X1 押人队列末位:若队列单元数量超过指定长度 N 则队列首个单元溢出;
3)已存在(Xi=X1)，先删除队列中存在的代码 Xi，再将 X1 押人队列末尾:
4)尝试设计一个哈希索引表，进行快速查找 (代替顺序查找)。

这段代码实现了一个链表队列的数据结构，并提供了相应的操作函数来处理字符串代码的插入、查找和删除。

1. 定义了链表节点结构 `Node`，其中包含一个字符串类型的 `code` 字段和一个指向下一个节点的指针 `next`。

2. `createNode` 函数用于创建一个新的节点，将传入的字符串代码复制到节点的 `code` 字段中，
并将 `next` 指针初始化为 `NULL`。然后返回新创建的节点。

3. `insertEnd` 函数用于在链表的末尾插入一个新节点。它首先调用 `createNode` 函数创建一个新节点，然后判断链表是否为空。
如果链表为空，则将新节点设置为头节点。如果链表不为空，则通过遍历找到链表的最后一个节点，并将最后一个节点的 `next` 指针指向新节点。

4. `searchNode` 函数用于在链表中查找指定的代码。它遍历链表，逐个比较节点的 `code` 字段
与给定的代码字符串，如果找到了匹配的节点，则返回1表示找到。
如果遍历完整个链表仍未找到匹配的节点，则返回0表示未找到。

5. `deleteNode` 函数用于删除链表中的指定节点。它首先处理头节点的情况，如果头节点即为要删除的节点，
则将头节点指向下一个节点，并释放要删除的节点的内存空间。
然后遍历链表，找到要删除的节点的前一个节点和要删除的节点本身，将前一个节点的 `next` 
指针指向要删除节点的下一个节点，并释放要删除节点的内存空间。

6. `insertOrUpdateNode` 函数用于将新节点插入队列中，同时根据指定的长度控制队列的溢出。
它首先调用 `searchNode` 函数查找要插入的代码是否已经存在于队列中，
如果存在，则调用 `deleteNode` 函数将已存在的代码节点从队列中删除。然后根据指定的长度判断队列是否已满。
如果队列已满，则调用 `deleteNode` 函数删除队列的首个节点。
最后，将新节点插入队列的末尾。

7. `printList` 函数用于打印链表中所有节点的代码字段。它遍历链表，逐个输出节点的代码字符串。

8. 在 `main` 函数中，首先定义了链表队列的头节点指针 `head` 和指定的队列长度 `maxLength`。
然后通过调用 `insertOrUpdateNode` 函数来插入或更新代码节点。按照要求插入了一系列代码节点，并设置了指定长度为5。
最后，调用 `printList` 函数打印链表中所有节点的代码字段。

这段代码的思路是利用链表实现一个先进先出的队列结构，通过遍历链表来查找和删除指定的代码节点，并根据指定的长度控制队列的溢出。



*/