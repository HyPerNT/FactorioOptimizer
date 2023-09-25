import json
import math
AllItems = {}

class Item():
    def __init__(self, name: str, quantity: int, duration: int, recipe: list):
        self.name = name
        self.quantity = quantity
        self.recipe = recipe
        self.duration = duration
    def __repr__(self):
        return f"'name': {self.name}, 'quantity': {self.quantity}, 'recipe': {self.recipe}, 'duration': {self.duration}"

    def raw_materials(self, print_err=True):
        big_list = self.full_recipe_list(print_err)
        crafting_process = {}
        for item, costs in big_list.items():
            if costs[1] == 0:
                crafting_process[item] = big_list[item]
        return crafting_process
    
    def full_recipe_list(self, print_err=True):
        if (len(self.recipe) == 0) or (self.quantity == None) or (self.duration == None): # Base case
            return {self.name: [self.quantity, self.duration]}
        crafting_process = {}
        failed_yet = False
        for component in self.recipe:
            name = component["name"]
            if name not in AllItems.keys():
                if not failed_yet:
                    if print_err:
                        print(f'Cannot make \"{self.name}\"')
                    failed_yet = True
                if print_err:
                    print(f'\tRequires: \"{name}\"')
                continue
            c_item = AllItems[name]
            if (c_item.quantity == None):
                batches = component['quantity']
            elif component['quantity'] == None:
                batches = c_item.quantity
            else:
                batches = math.ceil(component["quantity"] / c_item.quantity)
            intermediate_steps = c_item.full_recipe_list()
            if not intermediate_steps:
                if not failed_yet:
                    if print_err:
                        print(f'Cannot make \"{self.name}\"')
                    failed_yet = True
                if print_err:
                    print(f'\tRequires: \"{name}\"')
                continue
            for subcomponent in intermediate_steps.keys():
                qty = batches
                if (AllItems[subcomponent].quantity == None) or (AllItems[subcomponent].duration == None):
                  qty = qty  
                elif (subcomponent in c_item.recipe) and (subcomponent == c_item.recipe[indexOf := c_item.recipe.index(subcomponent)]["name"]):
                    if (c_item.recipe[indexOf]['quantity'] == None):
                        qty = component["quantity"] / (AllItems[subcomponent].quantity * c_item.recipe["quantity"])
                        if (c_item.recipe["quantity"] == None):
                            qty = component['quantity'] / AllItems[subcomponent].quantity
                    else:
                        qty = min(batches, component["quantity"] * c_item.recipe[indexOf]["quantity"] / (AllItems[subcomponent].quantity * c_item.recipe["quantity"]))
                if subcomponent not in crafting_process:
                    crafting_process[subcomponent] = intermediate_steps[subcomponent]
                    crafting_process[subcomponent][0] *= qty
                    crafting_process[subcomponent][1] *= qty
                    continue
                # Dict looks like {"item_name": [qty, total_time]}
                crafting_process[subcomponent][0] += (intermediate_steps[subcomponent][0] * qty)
                crafting_process[subcomponent][1] += (intermediate_steps[subcomponent][1] * qty)
            crafting_process[self.name] = [self.quantity, self.duration]
            # We know how to make the component if we didn't already   
        # print(f'Item: {self.name}\n\tRecipes: {crafting_process}') 
        return crafting_process
    
    def craftable_recipes(self):
        all_recipes = self.full_recipe_list()
        raw_mats = self.raw_materials(False)
        if (raw_mats is None):
            return all_recipes
        for raw_mat in raw_mats:
            all_recipes.pop(raw_mat)
        return all_recipes
    
    def chefs(self):
        all_recipes = self.full_recipe_list()
        all_craftable_components = self.craftable_recipes()
        non_limiting = {} # Total time < this.duration
        limiting = {} # Total time > this.duration
        for item, costs in all_craftable_components.items():
            if costs[1] > self.duration:
                limiting[item] = costs
            else:
                non_limiting[item] = costs
        # Have list of limiting and non_limiting items
        #Optimize limiting to make this recipe feasible
        while len(limiting.keys()) > 0:
            first_limiting_recipe = list(limiting.keys())[0]
            factor = math.ceil(limiting[first_limiting_recipe][1] / self.duration)
            base_list = AllItems[first_limiting_recipe].craftable_recipes()
            for item in all_craftable_components.keys():
                if item in base_list:
                    continue
                all_craftable_components[item][0] *= factor
                all_craftable_components[item][1] *= factor
                all_recipes[item][0] *= factor
                all_recipes[item][1] *= factor
            non_limiting = {} # Total time < this.duration
            limiting = {} # Total time > this.duration
            for item, costs in all_craftable_components.items():
                if costs[1] > all_craftable_components[self.name][1]:
                    limiting[item] = costs
                else:
                    non_limiting[item] = costs
                    
        for item, _ in all_recipes.items():
            if item in all_craftable_components.items():
                all_recipes.pop(item)
        print(f'To maximize efficiency, make the following in parallel (produces {all_craftable_components[self.name][1] / self.duration} {self.name} at a time)')
        for item, costs in all_craftable_components.items():
            print(f'\t{item}: {costs[0]} in {costs[1]} secs')
        print("Requires:")
        for item, costs in all_recipes.items():
            print(f'\t{item}: {costs[0]}')
        return all_craftable_components

    def chefs_rate(self, quantity, time):
        result = self.chefs()
        # Do some math to scale this up
        return result
        
def importBigRecipeList(path: str):
    with open(path, "r") as f:
        big_dict = json.load(f)
        for entry in big_dict:
            recipe = []
            for recipe_item in entry['recipe']['ingredients']:
                recipe.append({'name': recipe_item['id'], 'quantity': recipe_item['amount']})
            item = Item(entry['id'], entry['recipe']['yield'] if recipe else 1, entry['recipe']['time'] if recipe else 0, recipe)
            AllItems[entry['id']] = item

def importJsonRecipes(path: str):
    with open(path, "r") as f:
        json_dict = json.load(f)
        for item, details in json_dict.items():
            AllItems[item] = Item(item, details["quantity"], details["duration"], details["recipe"])