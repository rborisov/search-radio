/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "json.h"

typedef struct {
    int id;
    char name[128];
    char description[128];
    char image[128];
    char stream[128];
} DIRSTREAM;

typedef struct {
    size_t size;
    int num;
    DIRSTREAM **streams;
} DIRSTREAMS;

DIRSTREAMS dirstreams;

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
int process_int(json_value* value, int depth);

int append_stream()
{
    dirstreams.streams = realloc(dirstreams.streams, sizeof(DIRSTREAM*)*(dirstreams.num+1));
    if (dirstreams.streams == NULL)
        return -1;
    dirstreams.streams[dirstreams.num] = malloc(sizeof(DIRSTREAM));
    if (dirstreams.streams[dirstreams.num] == NULL)
        return -2;
    return dirstreams.num++;
}

void free_streams()
{
    while (dirstreams.num--) {
        printf("%d %d\n", dirstreams.num, dirstreams.streams[dirstreams.num]->id);
        free(dirstreams.streams[dirstreams.num]);
    }
    free(dirstreams.streams);
}

static void process_object(json_value* value, int depth)
{
    int length, x, i;
    if (value == NULL) {
        return;
    }
    length = value->u.object.length;
    printf("{\n");
    for (x = 0; x < length; x++) {
        print_depth_shift(depth);
        printf("(%d).name = %s\n", x, value->u.object.values[x].name);
        if (strcmp(value->u.object.values[x].name, "id") == 0 && depth == 2) {
            //alloc new dirstr and get id
            i = append_stream();
            if (i < 0) {
                return;
            }
            dirstreams.streams[i]->id = process_int(value->u.object.values[x].value, depth+1);
        } else {
            process_value(value->u.object.values[x].value, depth+1);
        }
    }
}

static void process_array(json_value* value, int depth)
{
    int length, x;
    if (value == NULL) {
        return;
    }
    length = value->u.array.length;
    printf("[\n");
    for (x = 0; x < length; x++) {
        process_value(value->u.array.values[x], depth);
    }
}

int process_int(json_value* value, int depth)
{
    int j;
    if (value == NULL) {
        printf("%s: value == NULL\n", __func__);
        return 0;
    }
    if (value->type == json_integer) {
        printf("int: %d\n", value->u.integer);
        return value->u.integer;
    } else {
        printf("%s: value->type != json_integer\n", __func__);
        process_value(value, depth);
    }
    return 0;
}


static void process_value(json_value* value, int depth)
{
    int j;
    if (value == NULL) {
        return;
    }
    if (depth > 6)
        return;
    if (value->type != json_object) {
        print_depth_shift(depth);
    }
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
    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */

   dirstreams.num = 0;
   dirstreams.size = 0;
//   dirstreams.streams = (void*)malloc(sizeof(dirstream_ *));
  
    sprintf(search_str, "http://api.dirble.com/v2/search/%s?token=acd53b8af2e49415ce696bac4e", "Paradise");
 
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
    json = (json_char*)chunk.memory;
    value = json_parse(json, chunk.size);
    if (value == NULL) {
        fprintf(stderr, "Unable to parse data\n");
        free(chunk.memory);
        curl_global_cleanup();
        return 0;
    }
    process_value(value, 0);

    free_streams();

    json_value_free(value);
    free(chunk.memory);
    curl_global_cleanup();
    return 0;
}
