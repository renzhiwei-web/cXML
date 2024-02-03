#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "cxml.h"

static void *(*cXML_malloc)(size_t sz) = malloc;
static void (*cXML_free)(void *ptr) = free;

void cXML_InitHooks(cXML_Hooks* hooks){
    if (!hooks)
    {
        cXML_malloc = malloc;
        cXML_free = free;
        return;
    }
    cXML_malloc = (hooks->malloc_fn) ? hooks->malloc_fn : malloc;
    cXML_free = (hooks->free_fn) ? hooks->free_fn : free;
}

// 将字符串的内容存放到动态内存指定的位置
// 复制一段字符串
static char* cXML_strdup(const char *str){
    size_t len;
    char *copy;
    len = strlen(str) + 1;
    // 加1的原因是需要多申请一个字节的长度存放'\0'
    if (!(copy = (char*)cXML_malloc(len)))
    {
        return 0;
    }
    // 从str中赋值len长度的内容，会自动在copy后面加'\0',而strncpy则需要手动赋字符串末尾为'\0'
    memcpy(copy,str,len);
    return copy;
}

static void suffix_object(cXML *prev,cXML *item){
    prev->next = item;
    item->prev = prev;
}

cXML *cXML_Create(const char *name){
    cXML* node = (cXML*)cXML_malloc(sizeof(cXML));
    if (node)
    {
        memset(node,0,sizeof(cXML));
    }
    node->name = cXML_strdup(name);
    return node;
}

void cXML_AddPropertiesToNode(cXML *item,const char *key,const char *value){
    // 如果是第一个属性，则先申请内存
    if (item->n == 0)
    {
        item->properties = (entry*)cXML_malloc(sizeof(entry));
    }// 如果不是第一个属性，则先扩容
    else{
        entry *temp = (entry*)realloc(item->properties,sizeof(entry));
        // 扩容失败，则直接返回
        if (!temp)
        {
            return;
        }
        item->properties = temp;
    }
    item->properties[item->n].key = cXML_strdup(key);
    item->properties[item->n].value = cXML_strdup(value);
    item->n++;
}

void cXML_AddItemToNode(cXML *node,cXML *item){
    cXML *c = node->child;
    if (!item)
    {
        return;
    }
    if (!c)
    {
        node->child = item;
    }else{
        while (c && c->next)
        {
            c = c->next;
        }
        suffix_object(c,item);
    }
}

static char *print_properties(entry property){
    char *out,*ptr;
    char *key = property.key;
    char *value = property.value;
    int len = strlen(key) + strlen(value) + 3;
    out = (char*)cXML_malloc(len);
    ptr = out;
    strcpy(ptr,key);
    ptr += strlen(key);
    *ptr++ = '=';
    *ptr++ = '\"';
    strcpy(ptr,value);
    ptr += strlen(value);
    *ptr = '\"';
    return out;
}

static char *print_value(cXML *item,int fmt,int depth){
    char** entries;
    char** properties;
    char *out = 0,*ptr,*ret;
    int name_len = strlen(item->name);
    int len = (name_len + 2) * 2 + 1,i = 0,fail = 0;
    int numentries = 0;
    cXML *child = item->child;
    size_t tmplen = 0;
    // 打印属性,属性名不加双引号，而属性值加双引号
    if (item->properties)
    {
        properties = (char**)cXML_malloc(item->n * sizeof(char*));
    }
    if (!properties)
    {
        return 0;
    }
    memset(properties,0,item->n * sizeof(char*));
    for (int i = 0;i < item->n;i++){
        ret = print_properties(item->properties[i]);
        properties[i] = ret;
        if (ret)
        {
            // 1是指用来隔开各个属性的空格
            len += strlen(ret) + 1;
        }else{
            fail = 1;
        }
    }
    
    while (child)
    {
        numentries++;
        child = child->next;
    }
    // 没有节点就打印<name></name>格式
    if (!numentries)
    {
        out = (char *) malloc(fmt ? len + depth + 1 : len);
        if (!out)
        {
            return 0;
        }
        ptr = out;
        *ptr++ = '<';
        strcpy(ptr,item->name);
        ptr += name_len;
        for (int i = 0;i < item->n;i++){
            *ptr++ = ' ';
            tmplen = strlen(properties[i]);
            memcpy(ptr,properties[i],tmplen);
            ptr += tmplen;
            cXML_free(properties[i]);
        }
        *ptr++ = '>';
        if (fmt)
        {
            *ptr++ = '\n';
            for (i = 0;i < depth;i++){
                *ptr++ = '\t';
            }
        }
        *ptr++ = '<';
        *ptr++ = '/';
        strcpy(ptr,item->name);
        ptr += name_len;
        *ptr++ = '>';
        *ptr++ = 0;
        return out;
    }
    entries = (char**)cXML_malloc(numentries * sizeof(char*));
    if (!entries)
    {
        return 0;
    }
    memset(entries,0,numentries * sizeof(char*));
    child = item->child;
    while (child && !fail)
    {
        ret = print_value(child,fmt,depth + 1);
        entries[i++] = ret;
        if (ret)
        {
            len += strlen(ret) + (fmt ? 1 : 0);
        }else{
            fail = 1;
        }
        child = child->next;
    }
    if (!fail)
    {
        out = (char*)cXML_malloc(len);
    }
    if (!out)
    {
        fail = 1;
    }
    if (fail)
    {
        for(int i = 0;i < numentries;i++){
            if (entries[i])
            {
                cXML_free(entries[i]);
            }
            cXML_free(entries);
            return 0;
        }
    }
    *out = '<';
    ptr = out + 1;
    strcpy(ptr,item->name);
    ptr += name_len;
    for (int i = 0;i < item->n;i++){
        *ptr++ = ' ';
        tmplen = strlen(properties[i]);
        memcpy(ptr,properties[i],tmplen);
        ptr += tmplen;
        cXML_free(properties[i]);
    }
    *ptr++ = '>';
    *ptr = 0;
    for (int i = 0;i < numentries;i++){
        tmplen = strlen(entries[i]);
        memcpy(ptr,entries[i],tmplen);
        ptr += tmplen;
        cXML_free(entries[i]);
    }
    cXML_free(entries);
    *ptr++ = '<';
    *ptr++ = '/';
    strcpy(ptr,item->name);
    ptr += name_len;
    *ptr++ = '>';
    *ptr++ = 0;
    return out;
    
}

char *cXML_Print(cXML *item){
    if (!item)
    {
        return 0;
    }
    return print_value(item,0,0);
}