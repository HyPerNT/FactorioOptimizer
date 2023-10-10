#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "all_items.h"

enum {
  EXACT,
  Q_LIMIT,
  OUTPUT_RATE
};

/// data type to store a rational number as a fraction
typedef struct {
  uint16_t one;
  uint16_t two;
} pair_t;

typedef struct {
  uint8_t lv;
  uint8_t mod_num;
  uint8_t mod_lv;
} ass_param_t;

/// data type to represent the vertexes that make up the graph
typedef struct {
  recipe_t* destraction; /// Spelled this way very deliberately
  uint16_t m;
  uint16_t p;
  uint16_t t;
  uint8_t c_len;
  pair_t c_vec[16]; /// first number shall be the index. second shall be the quantity consumed
} vertex_t;

/// data type to store the set of vertexes V
typedef struct {
  uint16_t cardinality;
  vertex_t* elements;
} set_t;

/// Uses Euler's method to find the GCD of two numbers. I know there are faster
/// ways but I'm lazy.
int gcd(int a, int b) {
    int max = (a > b) ? a : b;
    int min = (a < b) ? a : b;
    int tmp;
    for(;min != 0;) {
        tmp = max;
        max = min;
        min = tmp % min;
    }
    return max;
}

// LCM of fractions = LCM of numerators / GCD of denominators
void lcm_rational(pair_t *ans, pair_t* list, int len) {

  /// simplify all fractions
  int factor;
  for(int i = 0; i < len; i++) {
    factor = gcd( (list + i)->one, (list + i)->two );
    (list + i)->one = (list + i)->one / factor;
    (list + i)->two = (list + i)->two / factor;
  }

  /// calc the lcm of the numerators and stores in the numerator of the answer.
  ans->one = list->one * ((list + 1)->one / gcd(list->one, (list + 1)->one));
  for(int i = 2; i < len; i++) {
    ans->one = ans->one * ((list + i)->one / gcd(ans->one, (list + i)->one));
  }

  /// calc the gcd of the denominators and stores in the denominator of the
  /// answer.
  ans->two = gcd(list->two, (list + 1)->two);
  for(int i = 2; i < len; i++) {
    ans->two = gcd(ans->two, (list + i)->two);
  }

}

void calc_delta(set_t* V) {

  float delta = 0.0f;
  float delta_abs = 0.0f;

  for(uint8_t i = 0; i < V->cardinality; i++) {

    vertex_t* consumer = &V->elements[i];

    for(uint8_t ii = 0; ii < consumer->c_len; ii++) {
      
      vertex_t* producer = &V->elements[consumer->c_vec[ii].one];

      delta += (1.0f * producer->m * producer->p / producer->t) -
               (1.0f * consumer->m * consumer->c_vec[ii].two / consumer->t);
      delta_abs += fabs((1.0f * producer->m * producer->p / producer->t) -
                   (1.0f * consumer->m * consumer->c_vec[ii].two /
                   consumer->t));

    }

  }

  printf("         Error: %f\r\nAbsolute Error: %f\r\n", delta, delta_abs);

}

/// Step through a given recipe to determine the number of vertexes in the
/// resultant graph.
uint16_t cardinality(recipe_t* node) {

  uint16_t tmp = 1;

  static uint16_t index = 0;

  node->index = index++;
  
  for(int i = 0; i < node->inputs_len; i++)
    tmp += cardinality(node->inputs_vec[i]->item);

  return tmp;

}

void build_set(recipe_t* node, set_t* V) {

  V->elements[node->index].destraction = node;
  V->elements[node->index].m = 1;
  V->elements[node->index].p = node->produced;
  V->elements[node->index].t = node->time;
  V->elements[node->index].c_len = node->inputs_len;

  for(uint8_t i = 0; i < node->inputs_len; i++) {

    V->elements[node->index].c_vec[i].one = node->inputs_vec[i]->item->index;
    V->elements[node->index].c_vec[i].two = node->inputs_vec[i]->item_amount;

    build_set(node->inputs_vec[i]->item, V);

  }

}

/// attempt to optimize the graph for a delta of zero, size be damned
void optimize_exact(set_t* V) {

}

/// attempt to optimize the graph while staying at or below a given Q
void optimize_q_limit(set_t* V, uint16_t* Q) {

}

/// attempt to optimize the graph for a given final output of at least P where P
/// is the number of units per minute
void optimize_set_rate(set_t* V, uint16_t* P, float* scaling_factor) {

  float true_rate = *P / (60 * *scaling_factor);

  if((true_rate * V->elements[0].t / V->elements[0].p) < 1.0f) {
    printf("Multiplicity below 1 required, currently not supported.\r\n");
    return;
  }

  V->elements[0].m = (uint16_t)ceil(true_rate * V->elements[0].t /
                     V->elements[0].p);

  for(uint8_t i = 0; i < V->cardinality; i++) {

    vertex_t* consumer = &V->elements[i];

    for(uint8_t ii = 0; ii < consumer->c_len; ii++) {
      
      vertex_t* producer = &V->elements[consumer->c_vec[ii].one];

      producer->m = (uint16_t)ceil(1.0f * consumer->m *
                    consumer->c_vec[ii].two * producer->t / consumer->t /
                    producer->p);
      printf(" ");

    }

  }

}

void print_multiplicity(set_t* V) {

  printf("\r\n");

  for(uint8_t i = 0; i < V->cardinality; i++) {

    uint8_t name_len = 0;
    while( *(V->elements[i].destraction->name + name_len++) ) {}

    if(V->elements[i].c_len)
      printf("%d assemblers for %.*s\r\n", V->elements[i].m, name_len, &V->elements[i].destraction->name[0]);
    else
      printf("%d %.*s/s per second\r\n", V->elements[i].m, name_len, &V->elements[i].destraction->name[0]);

  }

}

void optimize(recipe_t* node, ass_param_t* specs, uint8_t optimizer, uint16_t num) {

  float speed_lvs[3] = {0.5f, 0.75f, 1.25f};
  float mod_lvs[3] = {0.2f, 0.3f, 0.5f};

  float scaling_factor = speed_lvs[specs->lv - 1] * (specs->mod_num * mod_lvs[specs->mod_lv - 1] + 1);
  
  set_t V;

  V.cardinality = cardinality(node);

  V.elements = (vertex_t*)malloc(V.cardinality * sizeof(vertex_t));

  build_set(node, &V);

  printf("Initial, unoptimized error:\r\n");
  calc_delta(&V);

  void* kill_me[3] = {optimize_exact, optimize_q_limit, optimize_set_rate};

  /// this is definitely the improper way to handle functions with a variable 
  /// number of parameters, but up yours. It offers *some* benefits
  ( (void (*)()) kill_me[optimizer])(&V, &num, &scaling_factor);

  printf("\r\nOptimized error:\r\n");
  calc_delta(&V);

  print_multiplicity(&V);

  free(V.elements);
}

int main(uint8_t argc, char** argv) {

  /// Set default recipe
  recipe_t* to_optimize = &automation_science_pack;

  /// Set default assembler parameters
  ass_param_t assembler_param = {
    3, /// Level 3 assemblers
    0, /// 0 installed modules
    1  /// Level 1 modules
  };

  /// Set default optimizer to be invalid since this is a required parameter.
  int optimizer = 3;

  /// Set default targets for optimizers 2 and 3
  int q_target = 0;
  int p_target = 0;

  for(char opt; (opt = getopt(argc, argv, "r:a:n:m:o:q:p:")) != -1; ) {
    switch(opt) {
      case 'r':
        break;
      case 'a':
        sscanf(optarg, "%i", &assembler_param.lv);
        break;
      case 'n':
        sscanf(optarg, "%i", &assembler_param.mod_num);
        break;
      case 'm':
        sscanf(optarg, "%i", &assembler_param.mod_lv);
        break;
      case 'o':
        sscanf(optarg, "%i", &optimizer);
        break;
      case 'q':
        sscanf(optarg, "%i", &q_target);
        break;
      case 'p':
        sscanf(optarg, "%i", &p_target);
        break;
      case '?':
        printf("Unkown argument, try again.\r\n");
        return 1;
    }
  }

  switch(optimizer) {
    case 0:
      if(q_target + p_target != 0) {
        printf("-q and -p should not be passed when optimizer 0 is selected.\r\n");
        return 2;
      }
      break;
    case 1:
      if(p_target) {
        printf("-p should not be passed when optimizer 1 is selected.\r\n");
        return 2;
      }
      break;
    case 2:
      if(q_target) {
        printf("-q should not be passed when optimizer 2 is selected.\r\n");
        return 2;
      }
      break;
    default:
      printf("No optimizer selected or invalid choice.\r\n");
      return 2;
  }

  optimize(to_optimize, &assembler_param, OUTPUT_RATE, 150);

  return 0;

}