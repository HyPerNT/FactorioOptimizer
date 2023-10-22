import math
AllItems = {}

"""
    A module for constructing a primitive item in Factorio and some (no longer) useful functions

    Dependancies:
        math
"""

class Item():
    """
        Class for internal representation of an item. Can be either a "raw" item or a craftable one

        Attributes:
            - name (str): The name of this item
            - quantity (int): The output quantity for one instance of this recipe
            - recipe (list): The list of items (as dicts) required to produce this item. Dicts look like ("name":, "quantity":) pairs for each ingredient
            - duration (float): The time taken to produce this 
            
        Methods:
            raw_materials (dict): Returns a list of Items and quantities of each that are not craftable but required by this recipe
            full_recipe_list (dict): Returns a list of all Items and quantities of each required by this recipe
            craftable_recipes (dict): Returns a list of Items and quantities of each that are craftable and required by this recipe
            chefs (dict): Returns a list of all items, final quantities of each, and some notion of multiplicity
            *** Item.chefs() is considered OBE and should not be used for optimization *** 
    """
    def __init__(self, name: str, quantity: int, duration: int, recipe: list):
        """
            Standard constructor, see class docs for attribute layout
        """
        self.name = name
        self.quantity = quantity
        self.recipe = recipe
        self.duration = duration

    def __repr__(self):
        return f"'name': {self.name}, 'quantity': {self.quantity}, 'recipe': {self.recipe}, 'duration': {self.duration}"

    def raw_materials(self, print_err=True) -> dict:
        """
            Returns a list of raw items and quanties of each required for manufacturing this recipe

            Parameters:
                print_err (bool): Flag to enable verbose output on craftablity or presence of items while traversing recipes
        """
        # Steal full list
        big_list = self.full_recipe_list(print_err)
        crafting_process = {}
        for item, costs in big_list.items():
            # Only add raw items to the dict
            if costs[1] == 0:
                crafting_process[item] = big_list[item]
        return crafting_process
    
    def full_recipe_list(self, print_err=True) -> dict:
        """
            Returns a complete list of all required items and quantities of each for this recipe.

            Parameters:
                print_err (bool): Flag to enable verbose output
        """
        if (len(self.recipe) == 0) or (self.quantity == None) or (self.duration == None): # Base case, raw ingredient
            return {self.name: [self.quantity, self.duration]}
        crafting_process = {}
        failed_yet = False

        # Iterate over all components of _this_ item
        for component in self.recipe:
            name = component["name"]

            # Check if the recipe is known to the master list
            if name not in AllItems.keys():
                # Not known, relay if desired
                if not failed_yet:
                    if print_err:
                        print(f'Cannot make \"{self.name}\"')
                    failed_yet = True
                # Relay required ingredients for bad recipe if desired
                if print_err:
                    print(f'\tRequires: \"{name}\"')
                continue
            
            # Calculate a number of "batches", basically an internal measure of how many instances of each recipe is needed for a parent recipe
            c_item = AllItems[name]
            if (c_item.quantity == None):
                batches = component['quantity']
            elif component['quantity'] == None:
                batches = c_item.quantity
            else:
                batches = math.ceil(component["quantity"] / c_item.quantity)

            # Get the intermediate recipes for this component
            intermediate_steps = c_item.full_recipe_list()

            # If this doesn't exist, error and relay info if desired
            if not intermediate_steps:
                if not failed_yet:
                    if print_err:
                        print(f'Cannot make \"{self.name}\"')
                    failed_yet = True
                if print_err:
                    print(f'\tRequires: \"{name}\"')
                continue

            # With subcomponents of each recipe, perform the same as above
            for subcomponent in intermediate_steps.keys():
                qty = batches
                
                # Check for raw materials
                if (AllItems[subcomponent].quantity == None) or (AllItems[subcomponent].duration == None):
                  qty = qty  

                # Then perform calculations on craftable recipes that are required by _this_ recipe
                elif (subcomponent in c_item.recipe) and (subcomponent == c_item.recipe[indexOf := c_item.recipe.index(subcomponent)]["name"]):

                    # Raw mat required
                    if (c_item.recipe[indexOf]['quantity'] == None):
                        qty = component["quantity"] / (AllItems[subcomponent].quantity * c_item.recipe["quantity"])
                        # This code looks dead... what is this even doing?!?
                        # if (c_item.recipe["quantity"] == None):
                        #     qty = component['quantity'] / AllItems[subcomponent].quantity
                    # Non-raw mat
                    else:
                        qty = min(batches, component["quantity"] * c_item.recipe[indexOf]["quantity"] / (AllItems[subcomponent].quantity * c_item.recipe["quantity"]))

                # Admit new items not yet seen into the list
                if subcomponent not in crafting_process:
                    crafting_process[subcomponent] = intermediate_steps[subcomponent]
                    crafting_process[subcomponent][0] *= qty
                    crafting_process[subcomponent][1] *= qty
                    continue

                # Slight adjustment since we need to account for the presence of this mat in lower-level recipes (i.e.: Something else needed this too)
                crafting_process[subcomponent][0] += (intermediate_steps[subcomponent][0] * qty)
                crafting_process[subcomponent][1] += (intermediate_steps[subcomponent][1] * qty)
            crafting_process[self.name] = [self.quantity, self.duration]
            # We know how to make the component if we didn't already now
        return crafting_process
    
    def craftable_recipes(self) -> dict:
        """
            Returns a list of all craftable items and quantities of each for a desired item
        """
        # Cheat
        all_recipes = self.full_recipe_list()
        
        # Cheat again
        raw_mats = self.raw_materials(False)
        
        # Shortcut for efficiency
        if (raw_mats is None):
            return all_recipes

        # Throw away what we don't need
        for raw_mat in raw_mats:
            all_recipes.pop(raw_mat)
        return all_recipes
    
    def chefs(self) -> dict:
        """
            An attempt at optimizing... and a poor one at that.
            This code isn't commmented, and I'm not going to bother since Item_node will do this better
        """
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