/**
 * @file Factorio_Recipe_Optimizer.c
 * @author Jared Jackson
 * @brief A short side track while playing a game of Factorio to automatically
 * calculate the number of assemblers need to automate items cause I was running
 * out of paper in the journal I keep at my desk.
 * @date 2023-10-11
 * @todo Look through the code and implement anything marked as todo.
**/

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "all_items.h"

/// @brief A struct to hold two related ints so they can be passed around
/// together. Mostly used for storing rationals.
typedef struct {
  uint16_t one; /// Self
  uint16_t two; /// Explanitory
} pair_t;

/// @brief A struct to hold the parameters about the assemblers used.
typedef struct {
  int lv;      /// The level of the assemblers.
  int mod_num; /// The number of modules installed in the assemblers.
  int mod_lv;  /// The level of the installed modules.
} ass_param_t;

/// @brief A struct to hold the parameters of an edge.
typedef struct {
  uint16_t c_index;  /// The graph index of the "consumer" vertex.
  uint16_t cc_index; /// The c_vec index corresponding to the input on the
                     /// "consumer" vertex.
  float delta;       /// The error between the two vertexes.
} edge_t;

/// @brief A struct to hold the parameters of a vertex.
/// @todo Implement a system of tiers for better final output formatting.
typedef struct {
  recipe_t* destraction; /// Spelled this way very deliberately.
  uint16_t m;            /// The "multiplicity".
  uint16_t p;            /// The "output quantity".
  uint16_t t;            /// The "period".
  uint8_t c_len;         /// The length of the c vector.
  pair_t c_vec[16];      /// The "input". The first number shall be the index
                         /// corresponding to the node that corresponds to this
                         /// input. The second number shall be the input 
                         /// quantity.
} vertex_t;

/// @brief  A struct to hold a recipe graph as defined in the paper and a few
/// extras to make the program more efficient.
typedef struct {
  uint16_t cardinality; /// The cardinality of the vertex set.
  vertex_t* vertexes;   /// The set of all vertexes.
  edge_t* edges;        /// The set of all edges.
  float delta;          /// The sum of the differences in inputs and outputs 
                        /// for the whole graph.
  float delta_abs;      /// The sum of the absolute difference in inputs and
                        /// outputs in the graph.
} graph;

/// @brief Uses Euler's method to find the GCD of two numbers. I know there are
/// faster ways but I'm lazy.
/// @param a The first of two ints.
/// @param b The second of two ints.
/// @return The GCD of the two ints.
int gcd(uint16_t* a, uint16_t* b) {
    int max = (*a > *b) ? *a : *b; /// Look
    int min = (*a < *b) ? *a : *b; /// it
    int tmp; /// up
    for(;min != 0;) { /// on
        tmp = max; /// Wikipedia
        max = min; /// if
        min = tmp % min; /// you
    }
    return max; /// care.
}

/// @brief Calculates the least common multiple of an array of rational numbers.
/// Uses the fact that:                      LCM of numerators
///                     LCM of Fractions = ---------------------
///                                         GCD of denominators
/// to avoid floating point errors.
/// @param ans A passed pair_t to store the answer.
/// @param list An array of rationals to calculate the LCM of.
/// @param len The number of rationals in the array.
void lcm_rational(pair_t *ans, pair_t* list, int* len) {

  /// Simplify all fractions or else the LCM will return the LCM times a factor.
  int factor;
  for(int i = 0; i < *len; i++) {
    factor = gcd( &(list + i)->one, &(list + i)->two );
    (list + i)->one = (list + i)->one / factor;
    (list + i)->two = (list + i)->two / factor;
  }

  /// Calculate the LCM of the numerators.
  ans->one = list->one * ((list + 1)->one / gcd(&list->one, &(list + 1)->one));
  for(int i = 2; i < *len; i++) {
    ans->one = ans->one * ((list + i)->one / gcd(&ans->one, &(list + i)->one));
  }

  /// Calculate the GCD of the denominators.
  ans->two = gcd(&list->two, &(list + 1)->two);
  for(int i = 2; i < *len; i++) {
    ans->two = gcd(&ans->two, &(list + i)->two);
  }

}

/// @brief Calculates delta for each edge and the overall graph.
/// @param G A pointer to a recipe graph.
void update_deltas(graph* G) {

  /// Wipe the slate clean.
  G->delta = 0.0f;
  G->delta_abs = 0.0f;

  /// Loops through every edge.
  for(uint8_t i = 0; i < G->cardinality - 1; i++) {

    /// Label to make the code easier to read.
    vertex_t* consumer = &G->vertexes[G->edges[i].c_index];

    /// Label to make the code easier to read.
    vertex_t* producer = &G->vertexes[i + 1];

    /// Finds the difference between production and consumption.
    G->edges[i].delta = (1.0f * producer->m * producer->p / producer->t) -
                           (1.0f * consumer->m *
                           consumer->c_vec[G->edges[i].cc_index].two /
                                                                   consumer->t);

    /// Update the running total as we go.
    G->delta += G->edges[i].delta;
    G->delta_abs += fabs(G->edges[i].delta);

  }

}

/// @brief Recursively walk through the recipe to find the number of vertexes
/// needed to represent the recipe as a graph.
/// @param node A pointer to the recipe to be optimized.
/// @return An int containing the cardinality.
uint16_t cardinality(recipe_t* node) {

  /// Stores the cardinality while in progress.
  uint16_t tmp = 1;
  
  /// Recursively step through the recipe.
  for(int i = 0; i < node->inputs_len; i++)
    tmp += cardinality(node->inputs_vec[i][0]);

  /// Returns the cardinality at each step such that it adds up and returns the 
  /// true value when the recursion is over.
  return tmp;

}

/// @brief Takes in the two empty sets and populates them with the information
/// from the recipe
/// @param node The recipe to use to fill in the graph.
/// @param V A pointer to the set of all vertexes.
/// @param E A pointer to the set of all edges.
/// @todo Fill out edge information
void construct_graph(recipe_t* node, graph* G) {

  /// Keeps track of where the next empty index in the elements array is.
  uint16_t frontier = 1;

  /// Set the initial pointer before entering the loop.
  G->vertexes[0].destraction = node;

  /// Loops through all vertexes in the graph.
  for(uint8_t i = 0; i < G->cardinality; i++) {

    /// The pointers are set before hand, so just follow it now.
    node = G->vertexes[i].destraction;

    /// Fill out the information for the vertex.
    G->vertexes[i].m = 1;
    G->vertexes[i].p = node->produced;
    G->vertexes[i].t = node->time;
    G->vertexes[i].c_len = node->inputs_len;

    /// Loop through all inputs to the current vertex.
    for(uint8_t ii = 0; ii < node->inputs_len; ii++) {

      /// Fill in the pointers ahead of time.
      G->vertexes[frontier].destraction = node->inputs_vec[ii][0];

      /// Fill out the input information for the vertex.
      G->vertexes[i].c_vec[ii].one = frontier;
      G->vertexes[i].c_vec[ii].two = (uint16_t)node->inputs_vec[ii][1];

      /// Fill out the information for the edge between the current vertex and
      /// the current input vertex.
      G->edges[frontier - 1].c_index = i;
      G->edges[frontier - 1].cc_index = ii;

      /// Move up to the next array position.
      frontier++;

    }

  }

}

/// @brief Attempts to optimize the graph for the lowest possible delta. Size be
/// damned.
/// @param V A pointer to the set of all vertexes.
/// @todo Implement this fuction.
void optimize_exact(graph* G) {}

/// @brief Attempts to optimize the graph for the lowest delta while staying
/// under the Q limit.
/// @param V A pointer to the set of all vertexes.
/// @param E A pointer to the set of all edges.
/// @param Q The limit to the summation of all vertexes times there
/// multiplicity.
/// @todo Implement this fuction.
void optimize_q_limit(graph* G, uint16_t* Q) {}

/// @brief Attempts to optimize the graph to have a final output of at least P.
/// @param V A pointer to the set of all vertexes.
/// @param P The desired output in units per minute.
/// @param scaling_factor A value that scales the final output. Calculated from
/// the assemblers parameters.
void optimize_set_rate(graph* G, uint16_t* P, float* scaling_factor) {

  /// Calculates the final output rate to shoot for given the user request and
  /// scaling factor
  float true_rate = *P / (60 * *scaling_factor);

  /// A multiplicity of below one is not allows per the defined problem. It
  /// is possible in-game though so I may revisit this later.
  if((true_rate * G->vertexes[0].t / G->vertexes[0].p) < 1.0f) {
    printf("Multiplicity below 1 required, currently not supported.\r\n");
    return;
  }

  /// Sets the multiplicity of the first vertex to be the smallest integer that
  /// still results in at least the user desired output rate.
  G->vertexes[0].m = (uint16_t)ceil(true_rate * G->vertexes[0].t /
                     G->vertexes[0].p);

  /// Loops through all vertexes in the graph.
  for(uint8_t i = 0; i < G->cardinality; i++) {

    /// Label to make the code easier to read.
    vertex_t* consumer = &G->vertexes[i];

    /// Loop through ever input of the current vertex.
    for(uint8_t ii = 0; ii < consumer->c_len; ii++) {
      
      /// Label to make the code easier to read.
      vertex_t* producer = &G->vertexes[consumer->c_vec[ii].one];

      /// The minimum multiplicity is set by what the vertex above consumes.
      producer->m = (uint16_t)ceil(1.0f * consumer->m *
                    consumer->c_vec[ii].two * producer->t / consumer->t /
                    producer->p);

    }

  }

}

/// @brief Prints out the multiplicity of every vertex in the graph. Because the
/// vertexes do not map to unique recipes, this will print out the total
/// multiplicity for a recipe in chunks.
/// @param V A pointer to the set of all vertexes.
void print_multiplicity(graph* G) {

  /// Formatting.
  printf("\r\n");

  /// Loops through all vertexes in the graph.
  for(uint8_t i = 0; i < G->cardinality; i++) {

    /// Find the length of the name of the item represented by the vertex.
    uint8_t name_len = 0;
    while( *(G->vertexes[i].destraction->name + name_len++) ) {}

    /// Print out the multiplicity. Some extra formatting to be nicer.
    if(G->vertexes[i].c_len)
      printf("%d assemblers for %.*s\r\n", G->vertexes[i].m, name_len,
                                          &G->vertexes[i].destraction->name[0]);
    else
      printf("%d %.*s/s per second\r\n", G->vertexes[i].m, name_len,
                                          &G->vertexes[i].destraction->name[0]);

  }

}

/// @brief Prints out the total multiplicity for each unique item. In order to 
/// save memory and run faster, it destroys the graph so this function should
/// only be called once at the end.
/// @param V A pointer to the set of all vertexes.
void print_destructively(graph* G) {

  /// Formatting.
  printf("\r\n");

  /// Loops through all vertexes in the graph.
  for(uint8_t i = 0; i < G->cardinality; i++) {

    /// If the vertex has already been counted, skip it.
    if(!G->vertexes[i].destraction)
      continue;

    /// Loop through the rest of the graph.
    for(uint16_t ii = i + 1; ii < G->cardinality; ii++) {

      /// Check if the item is a duplicate.
      if(G->vertexes[i].destraction == G->vertexes[ii].destraction) {

        /// Add the duplicate to the original.
        G->vertexes[i].m += G->vertexes[ii].m;

        /// Erase the duplicate's recipe association as a way to keep track of
        /// already counted vertexes.
        G->vertexes[ii].destraction = NULL;

      }

    }

    /// Find the length of the name of the item represented by the vertex.
    uint8_t name_len = 0;
    while( *(G->vertexes[i].destraction->name + name_len++) ) {}

    /// Print out the multiplicity. Some extra formatting to be nicer.
    if(G->vertexes[i].c_len)
      printf("%d assemblers for %.*s\r\n", G->vertexes[i].m, name_len,
                                          &G->vertexes[i].destraction->name[0]);
    else
      printf("%d %.*s/s per second\r\n", G->vertexes[i].m, name_len,
                                          &G->vertexes[i].destraction->name[0]);

  }

}

/// @brief Sets up a lot of boiler plate stuff before calling the function
/// corresponding to the requested optimizer.
/// @param node The recipe to optimize.
/// @param specs A struct containing the relevant parameters of the assemblers
/// being used.
/// @param optimizer Which optimizer to use. Currently only 0, 1, or 2 is
/// accepted.
/// @param num An additional numerical parameter that is interpreted based on
/// which optimizer is requested.
void optimize(recipe_t* node, ass_param_t* specs, uint8_t optimizer,
                                                                 uint16_t num) {

  /// Calculate a scaling factor based on the parameters of the assemblers.
  float speed_lvs[3] = {0.5f, 0.75f, 1.25f};
  float mod_lvs[3] = {0.2f, 0.3f, 0.5f};
  float scaling_factor = speed_lvs[specs->lv - 1] *
                              (specs->mod_num * mod_lvs[specs->mod_lv - 1] + 1);
  
  /// Allocate the memory for the graph
  graph G;
  G.cardinality = cardinality(node);
  G.vertexes = (vertex_t*)malloc(G.cardinality * sizeof(vertex_t));
  G.edges = (edge_t*)malloc((G.cardinality - 1) * sizeof(edge_t));

  /// Populate the graph with all the actual information.
  construct_graph(node, &G);

  /// Just a nice reference to see the initial error.
  update_deltas(&G);
  printf("Initial, unoptimized error:\r\n"
         "Delta: %+02.3f\r\nAbsolute Delta: %+02.3f\r\n", G.delta, G.delta_abs);

  /// Black Pointer Magic. This is an efficient was to call different functions
  /// easily and pass a variable number of parameters. This *potentially*,
  /// **might**, ***maybe*** have a ****few**** drawbacks but whatever.
  void* kill_me[3] = {optimize_exact, optimize_q_limit, optimize_set_rate};
  ( (void (*)()) kill_me[optimizer])(&G, &num, &scaling_factor);

  /// See how the error changes after optimization.
  update_deltas(&G);
  printf("\r\nOptimized error:\r\n"
         "Delta: %+02.3f\r\n"
         "Absolute Delta: %+02.3f\r\n", G.delta, G.delta_abs);

  /// Print the results.
  print_destructively(&G);

  /// Release the memory.
  free(G.vertexes);
  free(G.edges);
}

/// @brief The main function.
/// @param argc The number of command line arguments.
/// @param argv The command line arguments themselves.
/// @return An integer value encoding some information about the exit status of
/// the program.
int main(uint8_t argc, char** argv) {

  /// Set default recipe.
  recipe_t* to_optimize = &automation_science_pack;

  /// Set default assembler parameters.
  ass_param_t assembler_param = {
    3, /// Level 3 assemblers
    0, /// 0 installed modules
    3  /// Level 3 modules
  };

  /// Set default optimizer to be invalid since this is a required parameter.
  int optimizer = 3;

  /// Set default targets for optimizers 2 and 3. Invalid since these are
  /// required parameters.
  int q_target = 0;
  int p_target = 0;
  int num;

  /// Command line argument processing.
  /// @todo Implement the -r argument.
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

  /// Input checking to make sure the requested optimizer and values are good.
  switch(optimizer) {
    case 0:
      if(q_target + p_target != 0) {
        printf("-q and -p should not be passed when optimizer 0 "
                                                            "is selected.\r\n");
        return 2;
      }
      break;
    case 1:
      num = q_target;
      if(p_target) {
        printf("-p should not be passed when optimizer 1 is selected.\r\n");
        return 2;
      }
      break;
    case 2:
      num = p_target;
      if(q_target) {
        printf("-q should not be passed when optimizer 2 is selected.\r\n");
        return 2;
      }
      break;
    default:
      printf("No optimizer selected or invalid choice.\r\n");
      return 2;
  }

  /// Call the requested optimizer with all the given parameters.
  optimize(to_optimize, &assembler_param, optimizer, num);

  /// Exit successfully.
  return 0;

}