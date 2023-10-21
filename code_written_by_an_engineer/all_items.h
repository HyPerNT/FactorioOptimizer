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

recipe_t coal = {
  "Coal",
  1,
  1,
  0,
  NULL
};

recipe_t steel_plate = {
  "Steel Plate",
  1,
  1,
  0,
  NULL
};

recipe_t stone = {
  "Stone",
  1,
  1,
  0,
  NULL
};

recipe_t stone_brick = {
  "Stone brick",
  1,
  1,
  0,
  NULL
};

recipe_t sulfur = {
  "Sulfur",
  1,
  1,
  0,
  NULL
};

recipe_t plastic_bar = {
  "Plastic bar",
  1,
  1,
  0,
  NULL
};

recipe_t sulfuric_acid = {
  "Sulfuric acid",
  1,
  1,
  0,
  NULL
};

recipe_t battery = {
  "Battery",
  1,
  1,
  0,
  NULL
};

recipe_t lubricant = {
  "Lubricant",
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

inputs_t ammo_iron_plate = {&iron_plate, 4};
recipe_t ammo = {
  "Firearm magazine",
  1,
  1,
  1,
  {&ammo_iron_plate}
};

inputs_t wall_stone_brick = {&stone_brick, 10};
recipe_t wall = {
  "Wall",
  2,
  1,
  1,
  {&wall_stone_brick}
};

inputs_t iron_stick_iron_plate = {&iron_plate, 2};
recipe_t iron_stick = {
  "Iron stick",
  4,
  1,
  1,
  {&iron_stick_iron_plate}
};

inputs_t rail_iron_stick = {&iron_stick, 2};
inputs_t rail_steel_plate = {&steel_plate, 2};
inputs_t rail_stone = {&stone, 2};
recipe_t rail = {
  "Rail",
  4,
  1,
  3,
  {&rail_iron_stick, &rail_steel_plate, &rail_stone}
};

inputs_t pipe_steel_plate = {&steel_plate, 2};
recipe_t pipe_ = {
  "Pipe",
  2,
  1,
  1,
  {&pipe_steel_plate}
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

inputs_t grenade_coal = {&coal, 10};
inputs_t grenade_iron_plate = {&iron_plate, 5};
recipe_t grenade = {
  "Grenade",
  1,
  8,
  2,
  {&grenade_coal, &grenade_iron_plate}
};

inputs_t piercing_ammo_copper_plate = {&copper_plate, 5};
inputs_t piercing_ammo_ammo = {&ammo, 1};
inputs_t piercing_ammo_steel_plate = {&steel_plate, 1};
recipe_t piercing_ammo = {
  "Piercing Rounds Magazine",
  1,
  3,
  3,
  {&piercing_ammo_copper_plate, &piercing_ammo_ammo, &piercing_ammo_steel_plate}
};

inputs_t advanced_circuit_copper_cable = {&copper_cable, 4};
inputs_t advanced_circuit_circuit = {&electronic_circuit, 2};
inputs_t advanced_circuit_plastic_bar = {&plastic_bar, 2};
recipe_t advanced_circuit = {
  "Advanced circuit",
  1,
  6,
  3,
  {&advanced_circuit_copper_cable, &advanced_circuit_circuit, &advanced_circuit_plastic_bar}
};

inputs_t processing_unit_advanced_circuit = {&advanced_circuit, 2};
inputs_t processing_unit_electronic_circuit = {&electronic_circuit, 20};
inputs_t processing_unit_sulfuric_acid = {&sulfuric_acid, 5};
recipe_t processing_unit = {
  "Processing unit",
  1,
  10,
  3,
  {&processing_unit_advanced_circuit, &processing_unit_electronic_circuit, &processing_unit_sulfuric_acid}
};

inputs_t engine_iron_gear_wheel = {&iron_gear_wheel, 1};
inputs_t engine_pipe = {&pipe_, 2};
inputs_t engine_steel_plate = {&steel_plate, 1};
recipe_t engine = {
  "Engine unit",
  1,
  10,
  3,
  {&engine_iron_gear_wheel, &engine_pipe, &engine_steel_plate}
};

inputs_t productivity_module_advanced_circuit = {&advanced_circuit, 5};
inputs_t productivity_module_circuit = {&electronic_circuit, 5};
recipe_t productivity_module = {
  "Productivity module",
  1,
  15,
  2,
  {&productivity_module_advanced_circuit, &productivity_module_circuit}
};

inputs_t electric_furnace_advanced_circuit = {&advanced_circuit, 5};
inputs_t electric_furnace_steel_plate = {&steel_plate, 10};
inputs_t electric_furnace_stone_brick = {&stone_brick, 10};
recipe_t electric_furnace = {
  "Electric furnace",
  1,
  5,
  3,
  {&electric_furnace_advanced_circuit, &electric_furnace_steel_plate, &electric_furnace_stone_brick}
};

inputs_t low_density_structure_copper_plate = {&copper_plate, 20};
inputs_t low_density_structure_plastic_bar = {&plastic_bar, 5};
inputs_t low_density_structure_steel_plate = {&steel_plate, 2};
recipe_t low_density_structure = {
  "Low density structure",
  1,
  20,
  3,
  {&low_density_structure_copper_plate, &low_density_structure_plastic_bar, &low_density_structure_steel_plate}
};

inputs_t electric_engine_unit_electronic_circuit = {&electronic_circuit, 2};
inputs_t electric_engine_unit_engine = {&engine, 1};
inputs_t electric_engine_unit_lubricant = {&lubricant, 5};
recipe_t electric_engine_unit = {
  "Electric engine unit",
  1,
  10,
  3,
  {&electric_engine_unit_electronic_circuit, &electric_engine_unit_engine, &electric_engine_unit_lubricant}
};

inputs_t flying_robot_frame_battery = {&battery, 2};
inputs_t flying_robot_frame_electric_engine_unit = {&electric_engine_unit, 1};
inputs_t flying_robot_frame_electronic_circuit = {&electronic_circuit, 3};
inputs_t flying_robot_frame_steel_plate = {&steel_plate, 1};
recipe_t flying_robot_frame = {
  "Flying robot frame",
  1,
  20,
  4,
  {&flying_robot_frame_battery, &flying_robot_frame_electric_engine_unit, &flying_robot_frame_electronic_circuit, &flying_robot_frame_steel_plate}
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

inputs_t military_science_pack_grenade = {&grenade, 1};
inputs_t military_science_pack_piercing_ammo = {&piercing_ammo, 1};
inputs_t military_science_pack_wall = {&wall, 2};
recipe_t military_science_pack = {
  "Military Science Pack",
  2,
  10,
  3,
  {&military_science_pack_grenade, &military_science_pack_piercing_ammo, &military_science_pack_wall}
};

inputs_t chemical_science_pack_advanced_circuit = {&advanced_circuit, 3};
inputs_t chemical_science_pack_engine = {&engine, 2};
inputs_t chemical_science_pack_sulfur = {&sulfur, 1};
recipe_t chemical_science_pack = {
  "Chemical science pack",
  2,
  24,
  3,
  {&chemical_science_pack_advanced_circuit, &chemical_science_pack_engine, &chemical_science_pack_sulfur}
};

inputs_t production_science_pack_electric_furnace = {&electric_furnace, 1};
inputs_t production_science_pack_productivity_module = {&productivity_module, 1};
inputs_t production_science_pack_rail = {&rail, 30};
recipe_t production_science_pack = {
  "Production science pack",
  3,
  21,
  3,
  {&production_science_pack_electric_furnace, &production_science_pack_productivity_module, &production_science_pack_rail}
};

inputs_t utility_science_pack_flying_robot_frame = {&flying_robot_frame, 1};
inputs_t utility_science_pack_low_density_structure = {&low_density_structure, 3};
inputs_t utility_science_pack_processing_unit = {&processing_unit, 2};
recipe_t utility_science_pack = {
  "Utility science pack",
  3,
  21,
  3,
  {&utility_science_pack_flying_robot_frame, &utility_science_pack_low_density_structure, &utility_science_pack_processing_unit}
};