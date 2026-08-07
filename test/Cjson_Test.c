#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct{
    char* name;
    int age;
}Person;

/**
 * 
 * json两种格式:
 *   1、结构体对应的格式: 
 *         由{}包裹, 里面由多个属性键值对组成,属性名固定是字符串类型,由""包裹, 属性值由类型决定[数字、布尔类型直接写,字符串类型""包裹,数组遵循json数组格式,结构体类型遵循json结构体格式]
 *          属性名和属性值之间通过:分割,多个属性键值对通过逗号分割
 *          typedef struct{
 *              char* cityName;
 *              char* provinceName;
 *          }City;
 *          typedef struct{
                char* name;
                int age;
                City city;
            }Person;
 *          Person p = {"lisi",20, {"深圳","广东省"}}
 *          json字符串: { "name": "lisi" , "age" : 20, "city": {"cityName":"深圳","provinceName":"广东省"}}
 *   2、数组对应的格式:
 *      由[]包裹,里面是由一个个的元素组成, 元素和元素之间逗号分割,写法元素类型决定[数字、布尔类型直接写,字符串类型""包裹,数组遵循json数组格式,结构体类型遵循json结构体格式]
 *      int arr[10] = {1,2,3,4,5,6,7,8,9,10}
 *      json写法: [1,2,3,4,5,6,7,8,9,10]
 *      Person p[2] ;
 *      p[0] = {"lisi1",20, {"深圳1","广东省"}}
 *      p[1] = {"lisi2",21, {"深圳2","广东省"}}
 *      json写法: [ { "name": "lisi1" , "age" : 20, "city": {"cityName":"深圳1","provinceName":"广东省"}}, { "name": "lisi2" , "age" : 21, "city": {"cityName":"深圳2","provinceName":"广东省"}} ]
 */
int main(void){

    Person p = {"lisi",20};
    //将结构体转成json
    //{}
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name" ,p.name);
    cJSON_AddNumberToObject(root, "age", p.age);
    char* json = cJSON_PrintUnformatted(root);
    printf("%s\n",json);

    cJSON_Delete(root);
    //解析json字符串
    cJSON* obj = cJSON_ParseWithLength(json,strlen(json));
    cJSON* name = cJSON_GetObjectItemCaseSensitive(obj,"name");
    printf("name=%s\n",name->valuestring);

    cJSON_Delete(obj);
    free(json);
}