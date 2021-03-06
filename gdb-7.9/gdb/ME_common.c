#include "ME_common.h"
#include "ME_RLI_IR.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/time.h>

#include <stdbool.h>

#include <assert.h>

#include <fcntl.h>

#include <jansson.h>

char** str_split(char* a_str_O, const char a_delim)
{  
  char* a_str;
  char** result    = 0;
  size_t count     = 0;
  char* tmp;
  char* last_comma = 0;
  char delim[2];
  
  a_str = malloc((strlen(a_str_O)+1)*sizeof(char));
  memcpy(a_str, a_str_O, (strlen(a_str_O)+1)*sizeof(char));
  tmp = a_str;
  
  delim[0] = a_delim;
  delim[1] = 0;

  /* Count how many elements will be extracted. */
  while (*tmp)
    {
      if (a_delim == *tmp)
	{
	  count++;
	  last_comma = tmp;
	}
      tmp++;
    }

  /* Add space for trailing token. */
  count += last_comma < (a_str + strlen(a_str) - 1);

  /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
  count++;

  result = malloc(sizeof(char*) * count);

  if (result)
    {
      size_t idx  = 0;
      char* token = strtok(a_str, delim);

      while (token)
	{
	  assert(idx < count);
	  *(result + idx++) = strdup(token);
	  token = strtok(0, delim);
	}
      assert(idx == count - 1);
      *(result + idx) = 0;
    }

  free(a_str);
  
  return result;
}

void free_str_split(char **tokens) {
  int i;
  for (i = 0; *(tokens + i); i++)
  {
    free(*(tokens + i));
  }
  free(tokens);
}

/*====================================================
SOCKET STUFF
======================================================*/

int ME_sock_server_connect(int port)
{
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;

  printf("Listening for client...\n");

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
  {
    printf("ERROR: Could not create server socket!\n");
    exit(1);
  }
  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
    printf("ERROR: Could not bind server socket!\n");
    exit(-1);
  }

  listen(listenfd, 10);
  
  connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

  if (connfd < 0) {
    printf("ERROR: Could not accept client!\n");
    exit(-1);
  }
  
  printf("Connected to client!\n");

  fcntl(connfd, F_SETFL, O_NONBLOCK);

  close(listenfd);
  
  return connfd;
}

int ME_sock_recv(int sockfd, char * message)
{
  char recvBuff[1024];
  int n;

  memset(recvBuff, 0, sizeof(recvBuff));
  
  n = read(sockfd, recvBuff, sizeof(recvBuff));

  
  if (n < 0) {
    if (errno==EAGAIN) return n;
    printf("\n Error: Read error [%d]!\n",errno);
    exit(-1);
  }
  if (n == 0) {
    printf("\n Driver closed!\n");
    close(sockfd);
    exit(-1);
  }
  printf("Received %d bytes:\"%s\"\n",n,recvBuff);
      
  //recvBuff[strlen(recvBuff)-1] = 0;

  strcpy(message,recvBuff);

  return n;
}

void ME_sock_send(int sockfd, char * message)
{
  char sendBuff[1025];
  int count;
  
  memset(sendBuff, 0 ,sizeof(sendBuff));

  snprintf(sendBuff, sizeof(sendBuff), "%s", message);
  printf("Sending:%s\n", sendBuff);

  count = write(sockfd, sendBuff, sizeof(sendBuff)-1);

  //printf("Sent %d bytes:\"%s\"\n",count,sendBuff);

}

void ME_sock_recv_dynamic(int sockfd, int * n, char ** message)
{
  int n1, n2;
  n1 = read(sockfd, n, sizeof(int));

  if (!(n1>0&&n))
  {
    (*n)=0;
    return;
  }
      
  (*message) = (char *)malloc(sizeof(char)*(*n));

  n2 = read(sockfd, (*message), sizeof(char)*(*n));

  //printf("Received %d bytes:\"%s\"\n",(*n),(*message));
  
}


void ME_sock_send_dynamic(int sockfd, int n, char * message)
{
  int count = write(sockfd, &n, sizeof(int));
  
  count = write(sockfd, message, sizeof(char)*n);

  //printf("Sent %d bytes:\"%s\"\n",count,message);
}



/*====================================================
CALL GRAPH STUFF
======================================================*/

typedef struct ME_CG
{
  struct ME_CG * child;
  struct ME_CG * sibling;
  int symbol;
}
ME_CG;

void ME_CG_toJSON_h(struct ME_CG *cg, struct ME_FT * ft, json_t * json_siblings) {
  if (!cg) return;
  json_t * json_cg = json_object();
  json_array_append(json_siblings, json_cg);
  json_object_set_new(json_cg, "symbol", json_string(ME_FT_get(ft,cg->symbol)));
  ME_CG_toJSON_h(cg->sibling, ft, json_siblings);
  json_t * json_children = json_array();
  ME_CG_toJSON_h(cg->child, ft, json_children);
  json_object_set_new(json_cg, "children", json_children);
}

json_t * ME_CG_toJSON(struct ME_CG * cg, struct ME_FT * ft) {
  if (!cg) return json_null();

  json_t * json_cg = json_object();
  json_object_set_new(json_cg, "symbol", json_string(ME_FT_get(ft,cg->symbol)));

  json_t * json_children = json_array();
  ME_CG_toJSON_h(cg->child, ft, json_children);
  json_object_set_new(json_cg, "children", json_children);
  
  return json_cg;
}

ME_CG * ME_CG_create(int symbol)
{
  ME_CG* cg = (ME_CG*)malloc(sizeof(ME_CG));
  cg->child = NULL;
  cg->sibling = NULL;
  cg->symbol = symbol;
  return cg;
}

void ME_CG_add_child(struct ME_CG * parent, struct ME_CG * child)
{
  ME_CG * curr;
  
  if (parent->child == NULL) {
    parent->child = child;
    return;
  }

  curr = parent->child;
  while (curr->sibling) {
    curr = curr->sibling;
  }
  curr->sibling = child;
}

ME_CG * ME_CG_copy(struct ME_CG * cg)
{
  ME_CG * copy;
    
  if (cg == NULL) return NULL;

  copy = ME_CG_create(cg->symbol);
  copy->sibling = ME_CG_copy(cg->sibling);
  copy->child = ME_CG_copy(cg->child);

  return copy;
}

void ME_CG_delete(struct ME_CG * cg)
{
  if (cg==NULL) return;
  
  if (cg->sibling)
    {
      ME_CG_delete(cg->sibling);
    }
  if (cg->child)
    {
      ME_CG_delete(cg->child);
    }
  free(cg);
}

void ME_CG_merge_stack(struct ME_CG * cg, struct ME_CG * stack)
{
  ME_CG * curr;
  
  if (!cg) {
    printf("No cg to merge to!!!\n");
    exit(-1);
  }

  if (!stack) return;

  curr = cg;
  while (curr->sibling) {
    if (curr->symbol == stack->symbol) {
      if (curr->child==NULL) {
	curr->child = ME_CG_copy(stack->child);
	return;
      }
      ME_CG_merge_stack(curr->child,stack->child);
      return;
    }
    curr = curr->sibling;
  }
  if (curr->symbol == stack->symbol) {
    if (curr->child==NULL) {
      curr->child = ME_CG_copy(stack->child);
      return;
    }
    ME_CG_merge_stack(curr->child,stack->child);
    return;
  }
  curr->sibling = ME_CG_copy(stack);
}

void ME_CG_print_s_h(struct ME_CG * cg) {
  printf("(%d ", cg->symbol);

  if (cg->child) {
    ME_CG * curr = cg->child;
    ME_CG_print_s_h(curr);
    while (curr->sibling) {
      curr = curr->sibling;
      ME_CG_print_s_h(curr);
    }
  }

  printf(")");

}

void ME_CG_print_s(struct ME_CG * cg) {
  if (cg==NULL) return;
  ME_CG_print_s_h(cg);
  printf("\n");
}

void ME_CG_print_h(struct ME_CG * cg, struct ME_FT * ft) {
  printf("(%s ", ME_FT_get(ft,cg->symbol));

  if (cg->child) {
    ME_CG * curr = cg->child;
    ME_CG_print_h(curr, ft);
    while (curr->sibling) {
      curr = curr->sibling;
      ME_CG_print_h(curr, ft);
    }
  }

  printf(")");

}

void ME_CG_print(struct ME_CG * cg, struct ME_FT * ft) {
  if (cg==NULL) return;
  ME_CG_print_h(cg, ft);
  printf("\n");
}

int ME_CG_count(struct ME_CG * cg) {
  int i = 1;
  
  if (cg == NULL) return 0;
  
  i += ME_CG_count(cg->sibling);
  i += ME_CG_count(cg->child);

  return i;
}

int ME_CG_encode_h(struct ME_CG * cg, int next, int * result)
{
  int i_this, i_child, i_sibling;
  
  if (cg == NULL) return next;

  i_this = next;
  next += 3;

  result[i_this] = cg->symbol;

  if (cg->child) {
    i_child = next;
    next = ME_CG_encode_h(cg->child, i_child, result);
    result[i_this+1] = i_child;
  } else {
    result[i_this+1] = -1;
  }

  if (cg->sibling) {
    i_sibling = next;
    next = ME_CG_encode_h(cg->sibling, next, result);
    result[i_this+2] = i_sibling;
  } else {
    result[i_this+2] = -1;
  }

  return next;
}

void ME_CG_encode(struct ME_CG * cg, int * count, char ** result) {
  int n, n3;
  n = ME_CG_count(cg);
  printf("count = %d\n", n);

  (*result) = (char *)malloc(sizeof(int) * n * 3);

  n3 = ME_CG_encode_h(cg, 0, (int *)(*result));

  if (n*3!=n3)
    {
      printf("Count and encode count are not equivalent!\n");
      //exit(-1);
    }

  (*count) = (n3) * (sizeof(int)/sizeof(char));
}

void ME_CG_print_encoded(int n, char * encoded_cg)
{
  int * cg_encoded_i;
  int n_i, i;
  cg_encoded_i = (int *)encoded_cg;
  n_i = n * (sizeof(int)/sizeof(char));
  for (i=0; i<n; i+=3)
    printf("[%d]%d,%d,%d\n",i,cg_encoded_i[i],cg_encoded_i[i+1],cg_encoded_i[i+2]);
}

ME_CG * ME_CG_decode_h(int * a, int i)
{
  ME_CG * cg = ME_CG_create(a[i]);
  //printf("create sym %d from index %i\n", a[i],i);
  if (a[i+1]!=-1) {
    cg->child = ME_CG_decode_h(a, a[i+1]);
  }
  if (a[i+2]!=-1) {
    cg->sibling = ME_CG_decode_h(a, a[i+2]);
  }
  return cg;
}


void ME_CG_decode(char * encoded_cg, struct ME_CG ** decoded_cg)
{
  (*decoded_cg) = ME_CG_decode_h((int *)encoded_cg, 0);
}

/*====================================================
FUNC TABLE STUFF
======================================================*/

typedef struct ME_FT
{
  struct ME_FT * next;
  char * name;
}
  ME_FT;

ME_FT* ME_FT_create_entry(char * name)
{  
  ME_FT* ft = (ME_FT*)malloc(sizeof(ME_FT));
  ft->next = NULL;
  
  ft->name = malloc((strlen(name)+1)*sizeof(char));
  memcpy(ft->name,name, (strlen(name)+1)*sizeof(char));
  
  return ft;
}

ME_FT* ME_FT_create() {
  return ME_FT_create_entry("root");
}

void ME_FT_delete(struct ME_FT * ft)
{
  if (ft==NULL) return;
  
  ME_FT_delete(ft->next);

  free(ft->name);
  free(ft);
}

int ME_FT_add(ME_FT * ft, char * name) {
  if (!name) return -1;

  int i = 0;
  ME_FT  *curr, *new_entry;
  
  curr = ft;
  while (curr->next) {
    i++;
    curr = curr->next;
    if (strcmp(curr->name,name)==0) return i;
  }
  i++;

  new_entry = ME_FT_create_entry(name);
  curr->next = new_entry;
  return i;
}

void ME_FT_print(ME_FT * ft)
{
  ME_FT * curr = ft;
  int i = 0;
  while (curr->next) {
    i++;
    curr = curr->next;
    printf("%d:%s ", i, curr->name);
  }
  printf("\n");

}

int ME_FT_get_index(ME_FT * ft, char * name)
{
  if (!name) return -1;

  int i = 0;
  ME_FT * curr = ft;
  while (curr->next) {
    i++;
    curr = curr->next;
    if (strcmp(curr->name,name)==0) return i;
  }
  return -1;
}

char * ME_FT_get(struct ME_FT * ft, int i)
{ 
  if (i==-1) return NULL;

  ME_FT * curr = ft;
  while (curr->next) {
    i--;
    curr = curr->next;
    if (i==0) return curr->name;
  }
  return NULL;

}

void ME_FT_encode(struct ME_FT * ft, int * count, char ** result)
{
  int next, curr_count;
  ME_FT * curr = ft;

  (*count) = 0;
  
  while (curr->next) {
    curr = curr->next;
    //printf("len:%d\n",strlen(curr->name));
    (*count) += strlen(curr->name) + 1;
  }

  ME_FT_print(ft);
  
  printf("count = %d\n", (*count));

  (*result) = malloc(sizeof(char) * (*count));

  next  = 0;
  
  curr = ft;
  while (curr->next) {
    curr = curr->next;
    curr_count = strlen(curr->name);
    memcpy((*result)+next*sizeof(char),curr->name,sizeof(char)*curr_count);
    next+=curr_count;
    (*result)[next] = ' ';
    next++;
  }
  (*result)[next-1]=0;

  if (next!=(*count)) {
      printf("next != count !!!\n");
      exit(-1);
  }
  printf("next=%d,count=%d,result=%s\n",next,(*count),(*result));
  ME_FT_print_encoded((*result));
}

void ME_FT_decode(char * ft_encoded, ME_FT ** ft)
{
  char** names;
  int i;

  (*ft) = ME_FT_create();
    
  names = str_split(ft_encoded, ' ');

  for (i = 0; *(names + i); i++)
  {
    //printf("%i = \"%s\"\n",i,names[i]);
    ME_FT_add((*ft),names[i]);
  }

  free_str_split(names);
  
}
  
void ME_FT_print_encoded(char * ft_encoded)
{
  printf("encoded-ft{%s}\n", ft_encoded);
}

/*====================================================
MEASUREMENT STUFF
======================================================*/

typedef enum ME_measurement_type {
  ME_MEASUREMENT_CALLSTACK, ME_MEASUREMENT_STRING
} ME_measurement_type;

typedef struct ME_CG_AND_FT {
  struct ME_CG * cg;
  struct ME_FT * ft;
} ME_CG_AND_FT;

typedef union ME_measurement_data {
  struct ME_CG_AND_FT cgft;
  char * string_val;
  
} ME_measurement_data;
  
typedef struct ME_measurement
{
  //reference to command???
  //when
  int measured; //measurement taken?
  ME_measurement_type type;
  ME_measurement_data data;

  struct ME_measurement * next;
}
ME_measurement;

int ME_measurement_equal(ME_measurement * ms1, ME_measurement * ms2) {
  if (!ms1 || !ms2) return !ms1 && !ms2;
  
  if (ms1->type != ms2->type) return false;
  if (ms1->type == ME_MEASUREMENT_CALLSTACK) {
    //to do...
    return false;
  } else if (ms1->type == ME_MEASUREMENT_STRING) {
    if (strcmp(ms1->data.string_val,ms2->data.string_val)!=0)
      return false;
  }
  return ME_measurement_equal(ms1->next, ms2->next);
}

ME_CG * ME_CG_fromJSON(json_t * json_cg, struct ME_FT * ft) {
  json_t * json_symbol = json_object_get(json_cg, "symbol");
  int sym_i = ME_FT_add(ft, json_string_value(json_symbol));
  struct ME_CG * cg = ME_CG_create(sym_i);
  
  json_t * json_children = json_object_get(json_cg, "json_children");
  int i;
  for (i=0; i<json_array_size(json_children); i++) {
    ME_CG_add_child(cg, ME_CG_fromJSON(json_array_get(json_children,i),ft));    
  }
  return cg;
}

ME_CG_AND_FT ME_CGFT_fromJSON(json_t * json_cg) {
  ME_CG_AND_FT cgft;
  struct ME_FT * ft = ME_FT_create();
  struct ME_CG * cg = ME_CG_fromJSON(json_cg, ft);
  cgft.ft = ft;
  cgft.cg = cg;
  return cgft;
}

json_t * ME_measurement_toJSON(struct ME_measurement *ms) {
  if (!ms) return json_null();

  json_t * json_ms = json_object();
  //set type
  if (ms->type == ME_MEASUREMENT_CALLSTACK) {
    json_object_set_new(json_ms, "type", json_string("callstack"));
    json_object_set_new(json_ms, "data", ME_CG_toJSON(ms->data.cgft.cg, ms->data.cgft.ft));
  } else if (ms->type == ME_MEASUREMENT_STRING) {
    json_object_set_new(json_ms, "type", json_string("string"));
    json_object_set_new(json_ms, "data", json_string(ms->data.string_val));
  }

  //set next
  json_object_set_new(json_ms, "next", ME_measurement_toJSON(ms->next));
  
  return json_ms;
}

ME_measurement * ME_measurement_fromJSON(json_t * json_ms) {
  if (json_is_null(json_ms)) return NULL;
  
  ME_measurement* ms = (ME_measurement*)malloc(sizeof(ME_measurement));
  //get type
  json_t * json_type = json_object_get(json_ms, "type");
  json_t * json_data = json_object_get(json_ms, "data");
  char * type = json_string_value(json_type);
  if (strcmp(type, "callstack")==0) {
    ms->type = ME_MEASUREMENT_CALLSTACK;
    ms->data.cgft = ME_CGFT_fromJSON(json_data);
  } else if (strcmp(type, "string")==0) {
    ms->type = ME_MEASUREMENT_STRING;
    ms->data.string_val = json_string_value(json_data);
  }

  ms->next = ME_measurement_fromJSON(json_object_get(json_ms, "next"));
  
  return ms;
  
}

ME_measurement * ME_measurement_create(ME_measurement_type type)
{
  ME_measurement* ms = (ME_measurement*)malloc(sizeof(ME_measurement));
  if (type == ME_MEASUREMENT_CALLSTACK) {
    ms->data.cgft.cg = NULL;
    ms->data.cgft.ft = NULL;
  }
  else if (type == ME_MEASUREMENT_STRING) {
    ms->data.string_val = NULL;
  }
  ms->measured = 0;
  ms->type = type;
  ms->next = NULL;
  return ms;
}

ME_measurement * ME_measurement_create_string(char * string_val)
{
  ME_measurement* ms = ME_measurement_create(ME_MEASUREMENT_STRING);
  ms->data.string_val = malloc((strlen(string_val)+1)*sizeof(char));
  memcpy(ms->data.string_val,string_val,((strlen(string_val)+1)*sizeof(char)));
  return ms;
}


void ME_measurement_delete(struct ME_measurement * ms)
{
  if (!ms) return;

  ME_measurement_delete(ms->next);

  if (ms->type == ME_MEASUREMENT_CALLSTACK) {
    ME_CG_delete((ME_CG *)ms->data.cgft.cg);
    ME_FT_delete((ME_FT *)ms->data.cgft.ft);
  }
  else if (ms->type == ME_MEASUREMENT_STRING) {
    free(ms->data.string_val);
  }
    
  free(ms);  
}

void ME_measurement_print(struct ME_measurement * ms)
{
  if (!ms) {
    printf("NULL\n");
    return;
  }
  
  printf("M{");
  printf("type=%d",ms->type);
  printf(", measured=%d", ms->measured);

  if (ms->type == ME_MEASUREMENT_CALLSTACK) {
    printf(", cg=");
    ME_CG_print(ms->data.cgft.cg, ms->data.cgft.ft);
    
    printf(", ft=");
    ME_FT_print(ms->data.cgft.ft);
  }
  else if (ms->type == ME_MEASUREMENT_STRING) {
    printf(", string_val=\"%s\"",ms->data.string_val);
  }
    
  printf(", next=\n");
  ME_measurement_print(ms->next);
  
  printf("}");
}

void ME_measurement_send(int sockfd, struct ME_measurement * ms) {
  int i, n, encoded_cg_count, encoded_ft_count;
  char *encoded_cg, *encoded_ft;
  if (!ms) {
    i = -1;
    ME_sock_send_dynamic(sockfd, sizeof(int)/sizeof(char), (char*)&i);
    return;
  }     

  //send type
  ME_sock_send_dynamic(sockfd, sizeof(ms->type)/sizeof(char), (char*)&(ms->type));  

  if (ms->type == ME_MEASUREMENT_CALLSTACK) {
    //Send data 1 (callgraph)
    ME_CG_encode(ms->data.cgft.cg, &n, &encoded_cg);
    encoded_cg_count = n * (sizeof(int)/sizeof(char));

    ME_sock_send_dynamic(sockfd, encoded_cg_count, encoded_cg);

    //send data 2 (ft)
    ME_FT_encode(ms->data.cgft.ft, &encoded_ft_count, &encoded_ft);
    ME_sock_send_dynamic(sockfd, encoded_ft_count, encoded_ft);

    free(encoded_cg);
    free(encoded_ft);
  }
  else if (ms->type == ME_MEASUREMENT_STRING) {
    //send string
    ME_sock_send_dynamic(sockfd, strlen(ms->data.string_val)+1, ms->data.string_val);
  }
    
  ME_measurement_send(sockfd, ms->next);
  
}

void ME_measurement_send_temp(int sockfd, struct ME_measurement * ms) {
  char response_type[1];
  response_type[0] = 2;
  ME_sock_send_dynamic(sockfd, 1, response_type);
  
  ME_measurement_send(sockfd, ms);
}

ME_measurement * ME_measurement_receive(int sockfd) {

  int message_type_size;
  char * message_type=NULL;   
  int encoded_cg_count;
  char * encoded_cg;
  int encoded_ft_count;
  char * encoded_ft;
  ME_FT * decoded_ft;
  ME_CG * decoded_cg;
  ME_measurement_type type;
  ME_measurement * ms;
  int string_val_count;
  char * string_val;
    
  ME_sock_recv_dynamic(sockfd, &message_type_size, &message_type);

  type = *((ME_measurement_type*)(message_type));
  
  if (type==-1)
    return NULL;
    
   ms = ME_measurement_create(type);

  if (ms->type == ME_MEASUREMENT_CALLSTACK) {
    
    ME_sock_recv_dynamic(sockfd, &encoded_cg_count, &encoded_cg);
    ME_sock_recv_dynamic(sockfd, &encoded_ft_count, &encoded_ft);

    ME_FT_decode(encoded_ft, &decoded_ft);
    ME_CG_decode(encoded_cg, &decoded_cg);
    printf("callgraph = ");
    ME_CG_print(decoded_cg,decoded_ft);
    printf("\n");

    free(encoded_cg);
    free(encoded_ft);

    ms->data.cgft.cg = decoded_cg;
    ms->data.cgft.ft = decoded_ft;
  }
  else if (ms->type == ME_MEASUREMENT_STRING) {
    ME_sock_recv_dynamic(sockfd, &string_val_count, &string_val);
    ms->data.string_val = string_val;
    //printf("String val received is \"%s\"\n",string_val);
  }
  
  ms->next = ME_measurement_receive(sockfd);

}


/*====================================================
  FEATURE STUFF
  ====================================================*/

typedef enum {ME_FEATURE_CALLSTACK, ME_FEATURE_VARIABLE, ME_FEATURE_MEMORY} ME_feature_type;

typedef struct ME_feature {
  ME_feature_type type;
  union fdata {
    char var_name[64]; //TODO max length of var_name
    struct ME_feature_memory {
      char address[64];
      char format[64];
    } m;
  } fdata;
} ME_feature;

ME_feature * ME_feature_create_callstack() {
  ME_feature * feature = (ME_feature*)malloc(sizeof(ME_feature));
  feature->type = ME_FEATURE_CALLSTACK;
  return feature;
}

ME_feature * ME_feature_create_variable(char * var_name) {
  ME_feature * feature = (ME_feature*)malloc(sizeof(ME_feature));
  feature->type = ME_FEATURE_VARIABLE;
  strcpy(feature->fdata.var_name,var_name);//TODO enforce size limit
  return feature;
}

ME_feature * ME_feature_create_memory(char * address, char * format) {
  ME_feature * feature = (ME_feature*)malloc(sizeof(ME_feature));
  feature->type = ME_FEATURE_MEMORY;
  strcpy(feature->fdata.m.address,address);
  strcpy(feature->fdata.m.format,format);
  return feature;
}

void ME_feature_print(ME_feature * feature)
{
  if (feature->type == ME_FEATURE_CALLSTACK) {
    printf("{type=callstack}");
  } else if (feature->type == ME_FEATURE_VARIABLE) {
    printf("{type=variable,name=%s}",feature->fdata.var_name);
  } else if (feature->type == ME_FEATURE_MEMORY) {
    printf("{type=memory,address=%s,format=%s}",feature->fdata.m.address,feature->fdata.m.format);
  }
}

/*====================================================
  EVENT STUFF
  ====================================================*/
typedef enum {BE_EVENT_T, BE_EVENT_B} BE_event_type;

typedef struct BE_event_t {
  int delay; //int time when
  clock_t start; //int time start
} BE_event_t;

typedef struct BE_event_b {
  int bp_id;
} BE_event_b;

typedef struct BE_event {
  BE_event_type type;
  bool repeat;
  union edata {
    struct BE_event_t t;
    struct BE_event_b b;
  } edata;
} BE_event;

typedef struct BE_hook
{
  BE_event * event;
  struct ME_RLI_IR_expr * action;
  bool enabled;
} BE_hook;

BE_event * BE_event_t_create(int delay, int repeat) {
  BE_event * event = (BE_event*)malloc(sizeof(BE_event));
  event->type = BE_EVENT_T;
  event->edata.t.delay = delay;
  event->edata.t.start = clock();
  event->repeat = repeat;
  return event;
}

BE_event * BE_event_b_create(int bp_id, int repeat) {
  BE_event * event = (BE_event*)malloc(sizeof(BE_event));
  event->type = BE_EVENT_B;
  event->edata.b.bp_id = bp_id;
  event->repeat = repeat;
  return event;
}

void BE_event_print(BE_event * event) {

  if (event->type == BE_EVENT_T) {
    printf("{type=T,delay=%d}",event->edata.t.delay);
  } else if (event->type == BE_EVENT_B) {
    printf("{type=B,bp_id=%d}",event->edata.b.bp_id);
  }
}
    
