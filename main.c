/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "json.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
             
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static void print_depth_shift(int depth)
{
    int j;
    printf("\n%d", depth);
    for (j=0; j < depth; j++) {
        printf("-");
    }
}

static void process_value(json_value* value, int depth);

static void process_object(json_value* value, int depth)
{
    int length, x;
    if (value == NULL) {
        return;
    }
    length = value->u.object.length;
    for (x = 0; x < length; x++) {
        print_depth_shift(depth);
        printf("[%d].%s = ", x, value->u.object.values[x].name);
        process_value(value->u.object.values[x].value, depth);
    }
}

static void process_array(json_value* value, int depth)
{
    int length, x;
    if (value == NULL) {
        return;
    }
    length = value->u.array.length;
    print_depth_shift(depth);
    printf(" array ");
    for (x = 0; x < length; x++) {
        process_value(value->u.array.values[x], depth);
    }
}

static void process_value(json_value* value, int depth)
{
    int j;
    if (value == NULL) {
        return;
    }
/*    if (value->type != json_object) {
        print_depth_shift(depth);
    }*/
    switch (value->type) {
        case json_none:
            printf("none");
            break;
        case json_object:
            process_object(value, depth+1);
            break;
        case json_array:
            process_array(value, depth);
            break;
        case json_integer:
            printf("int: %d", value->u.integer);
            break;
        case json_double:
            printf("double: %f", value->u.dbl);
            break;
        case json_string:
            printf("string: %s", value->u.string.ptr);
            break;
        case json_boolean:
            printf("bool: %d", value->u.boolean);
            break;
        case json_null:
            printf("null");
            break;
        default:
            printf("type?: %d", value->type);
    }
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    char search_str[128] = "";
    struct MemoryStruct chunk;
    json_char* json;
    json_value* value;

char * string = "{\"sitename\" : \"joys of programming\", \"categories\" : [ \"c\" , [\"c++\" , \"c\" ], \"java\", \"PHP\" ], \"author-details\": { \"admin\": false, \"name\" : \"Joys of Programming\", \"Number of Posts\" : 10 } }";

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
  
    sprintf(search_str, "http://api.dirble.com/v2/search/%s?token=acd53b8af2e49415ce696bac4e", "paradise");
 
    /* In windows, this will init the winsock stuff */ 
    curl_global_init(CURL_GLOBAL_ALL);
 
    /* get a curl handle */ 
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */ 
        curl_easy_setopt(curl, CURLOPT_URL, search_str);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */ 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
 
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    //printf("%s\n", chunk.memory);
    
    //json_object * jobj = json_tokener_parse(chunk.memory);
    //json_parse(jobj);
   
    json = (json_char*)chunk.memory;
    value = json_parse(json, chunk.size);
    if (value == NULL) {
        fprintf(stderr, "Unable to parse data\n");
        free(chunk.memory);
        curl_global_cleanup();
        return 0;
    }
    process_value(value, 0);
    json_value_free(value);

    free(chunk.memory);
    curl_global_cleanup();
    return 0;
}
