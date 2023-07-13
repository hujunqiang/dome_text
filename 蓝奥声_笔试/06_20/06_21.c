// // #include "stdio.h"
// // #include "stdlib.h"

// // typedef struct Node{
// //     int data;
// //     struct Node* next;
// // }Node;

// // //头插法
// // void insert_head(Node** head,int value){
// //     Node* newNode = (Node*)malloc(sizeof(Node));
// //     newNode->data = value;
// //     newNode->next = *head;
// //     *head = newNode;
// // }
// // //尾插法
// // void insert_end(Node** head,int value){
// //     Node* newNode = (Node*)malloc(sizeof(Node));
// //     newNode->data = value;
// //     newNode->next = NULL;
// //     if(*head == NULL){
// //         *head = newNode;
// //     }else{
// //         Node*current = *head;
// //         while(current->next != NULL){
// //             current = current->next;
// //         }
// //         current->next = newNode;
// //     }
// // } 
// #include <stdio.h>
// #include <stdlib.h>

// // 链表节点定义
// struct Node {
//     int data;
//     struct Node* next;
// };

// // 插入节点（从头部插入）
// void insertAtHead(struct Node** head, int value) {
//     // 创建新节点
//     struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
//     newNode->data = value;

//     // 将新节点插入链表头部
//     newNode->next = *head;//将新节点的指针域(next指针)指向当前链表的头节点
//     *head = newNode;//将指向头节点的指针(head)指向新节点
// }

// // 插入节点（从尾部插入）
// void insertAtTail(struct Node** head, int value) {
//     // 创建新节点
//     struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
//     newNode->data = value;
//     newNode->next = NULL;//将新节点的指针域设置为NULL，表示它是链表的最后一个节点。

//     // 如果链表为空，直接将新节点作为头节点
//     if (*head == NULL) {//如果链表为空，即头指针指向NULL
//         *head = newNode;
//         return;//将新节点作为头节点，然后返回。
//     }

//     // 找到链表尾部节点
//     struct Node* tail = *head;//创建一个指针tail并将其初始化为头指针*head指向的节点。
//     //将tail指针移动到链表的最后一个节点。
//     while (tail->next != NULL) {//表示当前节点的下一个节点不为空，即当前节点不是链表的最后一个节点。
//         tail = tail->next;//将tail指针后移，直到找到最后一个节点为止。
//     }

//     // 将新节点 newNode 连接到链表的尾部，使其成为新的最后一个节点
//     //在此之前，当前链表的尾部节点的 next 指针指向的是 NULL，表示链表的末尾。
//     //将新节点赋值给尾部节点的 next 指针，相当于将新节点放置在链表的最后。
//     tail->next = newNode;
// }

// // 删除任意节点（指定删除）
// void deleteNode(struct Node** head, int value) {
//     // 处理空链表的情况
//     if (*head == NULL) {
//         return;//表示链表为空，没有可删除的节点，所以直接返回。
//     }

//     //寻找要删除的节点    如果要删除的节点是头节点
//     //比较头节点的数据 (*head)->data 是否等于指定的值 value(删除之前必须要有头节点)
//     if ((*head)->data == value) {//如果相等，则说明要删除的节点是头节点
//         struct Node* temp = *head;//创建一个临时指针 temp 指向头节点
//         *head = (*head)->next;//将头节点的 next 指针赋值给头指针 *head
//         free(temp);
//         return;
//     }

//     // 寻找要删除的节点   如果是其他节点
//     struct Node* current = *head;//创建一个指针 current 并将其指向头节点 *head
//     while (current->next != NULL && current->next->data != value) {
//         //即当前节点的下一个节点不为空current->next != NULL
//         //检查当前节点的下一个节点的数据 current->next->data 是否等于指定的值 value，
//         //如果相等，则找到了要删除的节点。
//         current = current->next;//每次将 current 指针更新为当前节点的下一个节点
//         //如果找到了要删除的节点，则循环结束，current 指针将指向要删除节点的前一个节点。
//         //为了在删除操作中维持链表的连接，将当前节点的 next 指针指向要删除节点的下一个节点
//     }

//     // 删除节点
//     if (current->next != NULL) {
//         struct Node* temp = current->next;//创建一个临时指针 temp 并将其指向要删除的节点
//         //将当前节点的 next 指针指向要删除节点的下一个节点，相当于绕过要删除的节点，将链表连接起来。
//         current->next = temp->next;
//         free(temp);
//     }
// }

// // 从头开始删除节点（顺序删除）
// void deleteFromHead(struct Node** head) {
//     // 处理空链表的情况
//     if (*head == NULL) {
//         return;
//     }

//     struct Node* temp = *head;
//     *head = (*head)->next;
//     free(temp);
// }

// // 查找节点
// struct Node* searchNode(struct Node* head, int value) {
//     struct Node* current = head;
//     while (current != NULL) {
//         if (current->data == value) {
//             return current;
//         }
//         current = current->next;
//     }
//     return NULL;
// }

// // 修改节点数据
// void modifyNode(struct Node* node, int newValue) {
//     if (node != NULL) {
//         node->data = newValue;
//     }
// }

// // 遍历节点
// void traverseList(struct Node* head) {
//     struct Node* current = head;
//     while (current != NULL) {
//         printf("%d ", current->data);
//         current = current->next;
//     }
//     printf("\n");
// }

// // 释放链表
// void freeList(struct Node** head) {
//     struct Node* current = *head;
//     while (current != NULL) {
//         struct Node* temp = current;
//         current = current->next;
//         free(temp);
//     }
//     *head = NULL;
// }

// // 测试示例
// int main() {
//     struct Node* head = NULL;

//     // 插入节点（从头部插入）
//     insertAtHead(&head, 1);
//     insertAtHead(&head, 2);
//     insertAtHead(&head, 3);
//     insertAtHead(&head, 4);
//     insertAtHead(&head, 5);

//     // 遍历节点
//     printf("链表节点：");
//     traverseList(head);

//     // 插入节点（从尾部插入）
//     insertAtTail(&head, 6);
//     insertAtTail(&head, 7);

//     // 遍历节点
//     printf("链表节点：");
//     traverseList(head);

//     // 查找节点
//     int valueToFind = 4;
//     struct Node* foundNode = searchNode(head, valueToFind);
//     if (foundNode != NULL) {
//         printf("找到节点 %d\n", foundNode->data);
//     } else {
//         printf("未找到节点 %d\n", valueToFind);
//     }

//     // 修改节点数据
//     int newValue = 8;
//     modifyNode(foundNode, newValue);
//     printf("修改节点 %d 的数据为 %d\n", foundNode->data, newValue);

//     // 遍历节点
//     printf("链表节点：");
//     traverseList(head);

//     // 删除任意节点（指定删除）
//     int valueToDelete = 7;
//     deleteNode(&head, valueToDelete);
//     printf("删除节点 %d\n", valueToDelete);

//     // 从头开始删除节点（顺序删除）
//     deleteFromHead(&head);

//     // 遍历节点
//     printf("遍历节点---链表节点：");
//     traverseList(head);

//     // 释放链表
//     freeList(&head);

//     return 0;
// }


#include <stdio.h>

void findMostFrequentElement(const int arr[], int length) {
    // Initialize hash table
    int hash[100000] = {0};

    // Count occurrences of each element
    for (int i = 0; i < length; i++) {
        hash[arr[i]]++;
    }

    // Find element with maximum count
    int max_count = 0;
    int max_elem = 0;
    for (int i = 0; i < length; i++) {
        if (hash[arr[i]] > max_count) {
            max_count = hash[arr[i]];
            max_elem = arr[i];
        }
    }

    // Print result
    printf("Most frequent element: %d (count = %d)\n", max_elem, max_count);
}

int main() {
    int list[] = {12345, 95555, 12345, 12345};
    int length = sizeof(list) / sizeof(list[0]);

    findMostFrequentElement(list, length);

    return 0;
}
