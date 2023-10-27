#pragma once

#include <stdint.h>

typedef struct recipe_t recipe_t;

struct recipe_t {
  char* name;
  uint8_t tier;
  uint8_t produced;
  uint8_t time;
  uint8_t inputs_len;
  recipe_t* inputs_vec[][2];
};

char* machines[] = {"Assembler", "Chemical Plant", "Miner"};

recipe_t copper_plate = {
  "Copper Plate",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t iron_plate = {
  "Iron Plate",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t coal = {
  "Coal",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t steel_plate = {
  "Steel Plate",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t stone = {
  "Stone",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t stone_brick = {
  "Stone brick",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t sulfur = {
  "Sulfur",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t plastic_bar = {
  "Plastic bar",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t sulfuric_acid = {
  "Sulfuric acid",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t battery = {
  "Battery",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t lubricant = {
  "Lubricant",
  100,
  1,
  1,
  0,
  {{NULL}, {NULL}}
};

recipe_t iron_gear_wheel = {
  "Iron Gear Wheel",
  100,
  2,
  1,
  1,
  {{&iron_plate, (recipe_t*)4}}
};

recipe_t copper_cable = {
  "Copper Cable",
  100,
  4,
  1,
  1,
  {{&copper_plate, (recipe_t*)2}}
};

recipe_t ammo = {
  "Firearm magazine",
  100,
  1,
  1,
  1,
  {{&iron_plate, (recipe_t*)4}}
};

recipe_t wall = {
  "Wall",
  100,
  2,
  1,
  1,
  {{&stone_brick, (recipe_t*)10}}
};

recipe_t iron_stick = {
  "Iron stick",
  100,
  4,
  1,
  1,
  {{&iron_plate, (recipe_t*)2}}
};

recipe_t rail = {
  "Rail",
  100,
  4,
  1,
  3,
  {{&iron_stick, (recipe_t*)2}, {&steel_plate, (recipe_t*)2}, {&stone, (recipe_t*)2}}
};

recipe_t pipe_ = {
  "Pipe",
  100,
  2,
  1,
  1,
  {{&steel_plate, (recipe_t*)2}}
};

recipe_t electronic_circuit = {
  "Electronic Circuit",
  100,
  2,
  1,
  2,
  {{&copper_cable, (recipe_t*)6}, {&iron_plate, (recipe_t*)2}}
};

recipe_t inserter = {
  "Inserter",
  100,
  2,
  1,
  3,
  {{&electronic_circuit, (recipe_t*)2}, {&iron_gear_wheel, (recipe_t*)2}, {&iron_plate, (recipe_t*)2}}
};

recipe_t transport_belt = {
  "Transport Belt",
  100,
  4,
  1,
  2,
  {{&iron_gear_wheel, (recipe_t*)2}, {&iron_plate, (recipe_t*)2}}
};

recipe_t grenade = {
  "Grenade",
  100,
  1,
  8,
  2,
  {{&coal, (recipe_t*)10}, {&iron_plate, (recipe_t*)5}}
};

recipe_t piercing_ammo = {
  "Piercing Rounds Magazine",
  100,
  1,
  3,
  3,
  {{&copper_plate, (recipe_t*)5}, {&ammo, (recipe_t*)1}, {&steel_plate, (recipe_t*)1}}
};

recipe_t advanced_circuit = {
  "Advanced circuit",
  100,
  1,
  6,
  3,
  {{&copper_cable, (recipe_t*)4}, {&electronic_circuit, (recipe_t*)2}, {&plastic_bar, (recipe_t*)2}}
};

recipe_t processing_unit = {
  "Processing unit",
  100,
  1,
  10,
  3,
  {{&advanced_circuit, (recipe_t*)2}, {&electronic_circuit, (recipe_t*)20}, {&sulfuric_acid, (recipe_t*)5}}
};

recipe_t engine = {
  "Engine unit",
  100,
  1,
  10,
  3,
  {{&iron_gear_wheel, (recipe_t*)1}, {&pipe_, (recipe_t*)2}, {&steel_plate, (recipe_t*)1}}
};

recipe_t productivity_module = {
  "Productivity module",
  100,
  1,
  15,
  2,
  {{&advanced_circuit, (recipe_t*)5}, {&electronic_circuit, (recipe_t*)5}}
};

recipe_t electric_furnace = {
  "Electric furnace",
  100,
  1,
  5,
  3,
  {{&advanced_circuit, (recipe_t*)5}, {&steel_plate, (recipe_t*)10}, {&stone_brick, (recipe_t*)10}}
};

recipe_t low_density_structure = {
  "Low density structure",
  100,
  1,
  20,
  3,
  {{&copper_plate, (recipe_t*)20}, {&plastic_bar, (recipe_t*)5}, {&steel_plate, (recipe_t*)2}}
};

recipe_t electric_engine_unit = {
  "Electric engine unit",
  100,
  1,
  10,
  3,
  {{&electronic_circuit, (recipe_t*)2}, {&engine, (recipe_t*)1}, {&lubricant, (recipe_t*)5}}
};

recipe_t flying_robot_frame = {
  "Flying robot frame",
  100,
  1,
  20,
  4,
  {{&battery, (recipe_t*)2}, {&electric_engine_unit, (recipe_t*)1}, {&electronic_circuit, (recipe_t*)3}, {&steel_plate, (recipe_t*)1}}
};

recipe_t automation_science_pack = {
  "Automation Science Pack",
  100,
  1,
  5,
  2,
  {{&iron_gear_wheel, (recipe_t*)1}, {&copper_plate, (recipe_t*)1}}
};

recipe_t logistic_science_pack = {
  "Logistic Science Pack",
  100,
  1,
  6,
  2,
  {{&inserter, (recipe_t*)1}, {&transport_belt, (recipe_t*)1}}
};

recipe_t military_science_pack = {
  "Military Science Pack",
  100,
  2,
  10,
  3,
  {{&grenade, (recipe_t*)1}, {&piercing_ammo, (recipe_t*)1}, {&wall, (recipe_t*)2}}
};

recipe_t chemical_science_pack = {
  "Chemical science pack",
  100,
  2,
  24,
  3,
  {{&advanced_circuit, (recipe_t*)3}, {&engine, (recipe_t*)2}, {&sulfur, (recipe_t*)1}}
};

recipe_t production_science_pack = {
  "Production science pack",
  100,
  3,
  21,
  3,
  {{&electric_furnace, (recipe_t*)1}, {&productivity_module, (recipe_t*)1}, {&rail, (recipe_t*)30}}
};

recipe_t utility_science_pack = {
  "Utility science pack",
  100,
  3,
  21,
  3,
  {{&flying_robot_frame, (recipe_t*)1}, {&low_density_structure, (recipe_t*)3}, {&processing_unit, (recipe_t*)2}}
};