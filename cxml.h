// 避免头文件重复引用
#ifndef cxml__h
#define cxml__h

#ifdef __cplusplus
// 兼容c++语言
extern "C"
{
#endif

// 定义xml数据类型
#define cXML_False 0
#define cXML_True 1
#define cXML_NULL 2
#define cXML_Number 3
#define cXML_String 4
#define cXML_Array 5
#define cXML_Object 6

typedef struct entry
{
    char* key;
    char* value;
} entry;


typedef struct cXML
{
    struct cXML *next,*prev;
    struct cXML *child;
    entry *properties;
    // 节点属性个数
    int n;
    char *name;
    char *value;
} cXML;

typedef struct cXML_Hooks
{
    void *(*malloc_fn)(size_t sz);
    void (*free_fn)(void *ptr);
} cXML_Hooks;

extern void cXML_InitHooks(cXML_Hooks* hooks);

// 创建一个无属性节点
extern cXML *cXML_Create(const char *name);

// 给无属性节点添加属性
extern void cXML_AddPropertiesToNode(cXML *item,const char *key,const char *value);
// 给节点添加值
extern void cXML_AddValueToNode(cXML *node,char *value);
// 添加子节点
extern void cXML_AddItemToNode(cXML *node,cXML *item);
// 无格式打印xml
extern char *cXML_Print(cXML *item);
// 格式化打印xml
extern char *cXML__PrintUnformatted(cXML *item);
// 解析xml
extern cXML *cXML_Parse(const char* value);

#ifdef __cplusplus
}
#endif

#endif