#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "all_items.h"

/// data type to store a rational number as a fraction
typedef struct {
  uint16_t one;
  uint16_t two;
} pair_t;

/// data type to represent the vertexes that make up the graph
typedef struct {
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

pair_t calc_delta(set_t* V) {

  float delta = 0.0f;
  float delta_abs = 0.0f;

  for(uint8_t i = 0; i < V->cardinality; i++) {

    vertex_t* consumer = &V->elements[i];

    for(uint8_t ii = 0; ii < consumer->c_len; ii++) {
      
      vertex_t* producer = &V->elements[consumer->c_vec[ii].one];

      delta += (1.0f * producer->m * producer->p / producer->t) - (1.0f * consumer->m * consumer->c_vec[ii].two / consumer->t);
      delta_abs += fabs((1.0f * producer->m * producer->p / producer->t) - (1.0f * consumer->m * consumer->c_vec[ii].two / consumer->t));

    }

  }

  printf("delta: %f\r\ndelta_abs: %f\r\n", delta, delta_abs);

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
void optimize_q_limit(set_t* V, uint16_t Q) {

}

/// attempt to optimize the graph for a given final output P where P is the 
/// number of units per second
void optimize_set_rate(set_t* V, uint16_t P) {

  if(P * V->elements[0].t / V->elements[0].p) {

    printf("Multiplicity below 1 required, currently not supported.\r\n");

    return;

  }

    for(uint8_t i = 0; i < V->cardinality; i++) {



    vertex_t* consumer = &V->elements[i];

    for(uint8_t ii = 0; ii < consumer->c_len; ii++) {
      
      vertex_t* producer = &V->elements[consumer->c_vec[ii].one];

    }

  }

}

void optimize(recipe_t* node, uint8_t type, uint16_t num) {

  if(type > 2) {
    printf("Invalid Optimizer choice. Exiting...\r\n");
    return;
  }
  
  set_t V;

  V.cardinality = cardinality(node);

  V.elements = malloc(V.cardinality * sizeof(vertex_t));

  build_set(node, &V);

  calc_delta(&V);

  void* kill_me[3] = {optimize_exact, optimize_q_limit, optimize_set_rate};

  /// this is definitely the improper way to handle functions with a variable 
  /// number of parameters, but up yours. It offers *some* benefits
  ( (void (*)()) kill_me[type])(&V, num);

  free(V.elements);
}

int main(void) {

  optimize(&logistic_science_pack, 2, 100);

}