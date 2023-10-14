#pragma once

#include <stdint.h>

typedef struct recipe_t recipe_t;
typedef struct inputs_t inputs_t;

struct inputs_t {
  recipe_t* item;
  uint16_t item_amount;
};

struct recipe_t {
  char* name;
  uint8_t produced;
  uint8_t time;
  uint8_t inputs_len;
  inputs_t* inputs_vec[];
};

recipe_t copper_plate = {
  "Copper Plate",
  1,
  1,
  0,
  NULL
};

recipe_t iron_plate = {
  "Iron Plate",
  1,
  1,
  0,
  NULL
};

inputs_t iron_gear_wheel_iron_plate = {&iron_plate, 4};
recipe_t iron_gear_wheel = {
  "Iron Gear Wheel",
  2,
  1,
  1,
  {&iron_gear_wheel_iron_plate}
};

inputs_t copper_cable_copper_plate = {&copper_plate, 2};
recipe_t copper_cable = {
  "Copper Cable",
  4,
  1,
  1,
  {&copper_cable_copper_plate}
};

inputs_t electronic_circuit_copper_cable = {&copper_cable, 6};
inputs_t electronic_circuit_iron_plate = {&iron_plate, 2};
recipe_t electronic_circuit = {
  "Electronic Circuit",
  2,
  1,
  2,
  {&electronic_circuit_copper_cable, &electronic_circuit_iron_plate}
};

inputs_t inserter_electronic_circuit = {&electronic_circuit, 2};
inputs_t inserter_iron_gear_wheel = {&iron_gear_wheel, 2};
inputs_t inserter_iron_plate = {&iron_plate, 2};
recipe_t inserter = {
  "Inserter",
  2,
  1,
  3,
  {&inserter_electronic_circuit, &inserter_iron_gear_wheel, &inserter_iron_plate}
};

inputs_t transport_belt_iron_gear_wheel = {&iron_gear_wheel, 2};
inputs_t transport_belt_iron_plate = {&iron_plate, 2};
recipe_t transport_belt = {
  "Transport Belt",
  4,
  1,
  2,
  {&transport_belt_iron_gear_wheel, &transport_belt_iron_plate}
};

inputs_t automation_science_pack_copper_plate = {&copper_plate, 1};
inputs_t automation_science_pack_iron_gear_wheel = {&iron_gear_wheel, 1};
recipe_t automation_science_pack = {
  "Automation Science Pack",
  1,
  5,
  2,
  {&automation_science_pack_iron_gear_wheel, &automation_science_pack_copper_plate}
};

inputs_t logistic_science_pack_inserter = {&inserter, 1};
inputs_t logistic_science_pack_transport_belt = {&transport_belt, 1};
recipe_t logistic_science_pack = {
  "Logistic Science Pack",
  1,
  6,
  2,
  {&logistic_science_pack_inserter, &logistic_science_pack_transport_belt}
};