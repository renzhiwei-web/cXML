#include <stdio.h>
#include <stdlib.h>
#include "cxml.h"

cXML* create_objects(){
    cXML *root = cXML_Create("bookstore");
    cXML_AddPropertiesToNode(root,"id","1");
    cXML_AddPropertiesToNode(root,"name","wud");
    printf("%d\n",root->n);
    for (int i = 0;i < root->n;i++){
        printf("%s\n",root->properties[i].key);
        printf("%s\n",root->properties[i].value);
    }
    cXML *book1 = cXML_Create("book");
    cXML_AddValueToNode(book1,"fsd");
    cXML_AddItemToNode(root,book1);
    cXML *book2 = cXML_Create("book");
    cXML_AddValueToNode(book2,"fds");
    cXML_AddItemToNode(root,book2);
    return root;
}

int main(int argc,const char* argv[]){
    cXML *root = create_objects();
    char *s1 = cXML_Print(root);
    char *s2 = cXML__PrintUnformatted(root);
    printf("无格式打印如下：\n");
    printf("%s\n",s1);
    free(s1);
    printf("格式化打印如下:\n");
    printf("%s\n",s2);
    free(s2);
    return 0;
}