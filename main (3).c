#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>


//Node is the structure in the Huffman Tree
typedef struct Node{
	char key; //The alphabetic character to encode, or decode
	int value; //The frequency of the alphabetic character
	struct Node *Nleft; //The left-connected node in huffman tree
	struct Node *Nright; //The right-connected node in huffman tree
    struct Node *next; //The next node in the priority queue 
}Node; 

typedef struct PriorityQueue {
    Node *front;
} PriorityQueue;


//To create a new node
Node *newNode(char letter, int frequency, Node *Nr, Node *Nl){
	Node *n = malloc(sizeof(Node));
	n->key = letter;
	n->value = frequency;
	n->Nleft = Nl; 
	n->Nright = Nr; 
	return n;
}

//To create the priority queue
PriorityQueue createPriorityQueue() {
    PriorityQueue pq;
    pq.front = NULL;
    return pq;
}

// To insert a node in the priority queue
void enqueue(PriorityQueue *pq, Node *n) {
    Node *newnode = newNode(n->key, n->value, n->Nright, n->Nleft);

    // To check if the priority queue is empty or if priority of n less than first one
    if (pq->front == NULL || newnode->value < pq->front->value) {
        newnode->next = pq->front; //Add the node to the front of the queue
        pq->front = newnode;
    } else {
        Node *current = pq->front; //Go to the next node of the queue

        // Find the node with priority greater than priority of n, to know where to insert n
        while (current->next != NULL && current->next->value <= newnode->value) {
            current = current->next;
        }

        // Insert n at the right place
        newnode->next = current->next;
        current->next = newnode;
    }
}

//To delete the node with higher priority (smaller value) 
void dequeue(PriorityQueue *pq) {
    if (pq->front != NULL) 
    {
        Node *temp = pq->front;
        pq->front = pq->front->next;
        free(temp);
    }
}

//To return the first element of priority queue : with higher priority (smaller value) 
Node *front(PriorityQueue pq) {
    return pq.front;
}

//To create the huffman Tree
Node  *CreateHuffmanTree(PriorityQueue q){
    while (front(q) && front(q)->next) {//While there is at least 2 elements in the queue
        Node *A = newNode(front(q)->key, front(q)->value, front(q)->Nright, front(q)->Nleft); //We save the first element
        dequeue(&q); //We delete the first element
        Node *B = newNode(front(q)->key, front(q)->value, front(q)->Nright, front(q)->Nleft); //We save the new fisrt element
        dequeue(&q); //We delete the new first element
        Node *C = newNode('\0', B->value + A->value, A, B); //We create the new tree / node with those two elements
        enqueue(&q, C); //We enqueue it
    }
    Node *H = newNode(front(q)->key, front(q)->value, front(q)->Nright, front(q)->Nleft); //We save the root element of teh huffman tree
    return H; 
}
//To encode the message 'char a'
char *Encodemsg(Node *root, char a, int j, char *encodedMessage, bool *characterfind) {
    if (root->Nright != NULL) { //Go to the right
        encodedMessage[j++] = '1'; //Set the encoded message to 1

        if (*characterfind == true) { //Stop the function and return encodmessage if char find
            return encodedMessage;
        }
        Encodemsg(root->Nright, a, j, encodedMessage, characterfind); 
      
    }
    if (*characterfind == true) {//Stop the function and return encodmessage if char find
            return encodedMessage;
        }

    if (root->Nleft == NULL) { //If the current noode is a terminal one

        for(int i = j; i < strlen(encodedMessage); i++){ 
            encodedMessage[i] = '\0'; 
        }

        if (root->key == a) { //If the current node is the one we search
            *characterfind = true;
            return encodedMessage;
        }
        root = NULL; //To goback to the previous node, (this case : this was the wrong terminal node)
    } else {  
        j--; //To remove, replace the latest element of encoded message
        encodedMessage[j++] = '0'; //We went to left so set the encoded message to 0
        if (*characterfind == true) {//Stop the function and return encodmessage if char find
            return encodedMessage;
        }
        Encodemsg(root->Nleft, a, j, encodedMessage, characterfind);
       
    }
            return encodedMessage;
}

//To decode the message encodedMessage
char *Decodemsg(Node *root, int j, char encodedMessage[], int *lengthEncodMsg, char *decodedMessage, char *a, int *len, Node *rootnext, bool *characterfind) {

    if (root->Nright != NULL) { //Go to the right
        decodedMessage[j++] = '1';
        if (*characterfind == true) {//Stop the function and return a = decodedMessage if char find
            return a;
        }
        Decodemsg(root->Nright, j, encodedMessage, lengthEncodMsg, decodedMessage, a, len, rootnext, characterfind);
    }

    if (root->Nleft == NULL) { //If the current noode is a terminal one
        int k = 0; //Nb of correspondance between the encodedMsg and the path to go where we are
        for (int i = 0; i < j; i++) {
            if (decodedMessage[i] == encodedMessage[i]) {
                k++;
            }
        }

        if (k == j) { //Means we find the decodedmessage of teh first part of encodedmessage
            a[*len] = root->key; //We set the decodedmessage with the character
            a[(*len) + 1] = '\0'; //We set the final character at the decoded message
            
            (*len)++;
            if (*lengthEncodMsg != j) { //If there is still message to decode
                for (int i = 0; i < (*lengthEncodMsg - j); i++) { //We create the new encoded message by shifting everything and removing what has already been decoded."
                    encodedMessage[i] = encodedMessage[i + j];
                }

                for (int i = *lengthEncodMsg - j; i < *lengthEncodMsg; i++) {
                    encodedMessage[i] = ' ';
                }

                *lengthEncodMsg -= j; //We set the new length of encodedMessage
                //We need to restart from the root of the huffman tree to decod the next character of the encoded message
                Decodemsg(rootnext, 0, encodedMessage, lengthEncodMsg, decodedMessage, a, len, rootnext, characterfind); 
            } else {//We have decode all the mesage 
                *characterfind = true; 
                return a;
            }
        }
        root = NULL; //To goback to the previous node, (this case : this was the wrong terminal node)
    } else { //We go to the left
        j--;
        decodedMessage[j++] = '0';
        if (*characterfind == true) {//Stop the function and return a = decodedMessage if char find
            return a;
        }
        Decodemsg(root->Nleft, j, encodedMessage, lengthEncodMsg, decodedMessage, a, len, rootnext, characterfind);
    }
    return a;
}



int main(){
	Node *a = newNode('a', 10, NULL, NULL);
	Node *b = newNode('b', 7, NULL, NULL);
	Node *c = newNode('c', 4, NULL, NULL);
	Node *d = newNode('d', 5, NULL, NULL);
	Node *e = newNode('e', 4, NULL, NULL);
	Node *f = newNode('f', 8, NULL, NULL);
	PriorityQueue q = createPriorityQueue();
	enqueue(&q, a);
	enqueue(&q, b);
	enqueue(&q, c);
	enqueue(&q, d);
	enqueue(&q, e);
	enqueue(&q, f);


    Node *H = CreateHuffmanTree(q);

	
    bool characterfind = false; 
    char encodedMessage[100];
    char MessageToCode[] = "abcdef";
    printf("Message encodé pour is :");
    for(int i = 0; i<strlen(MessageToCode); i++)
    {characterfind = false;
        char *result = Encodemsg(H, MessageToCode[i], 0, encodedMessage,  &characterfind);
        printf("%s",result);
    }
    printf("\n");

    char MessageToEncode[]="0100010100110011"; 
    char decodedMessage[strlen(MessageToEncode)];
    int lengthEncodMsg = strlen(MessageToEncode);
    char chal[100] = " ";
    characterfind = false; 
    int len = 0; 
    char *cha = Decodemsg(H, 0, MessageToEncode, &lengthEncodMsg,decodedMessage, chal, &len, H, &characterfind);
    printf("char is %s\n", cha);


 }

