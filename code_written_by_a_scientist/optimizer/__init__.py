import json
from optimizer.item import Item, AllItems
from optimizer.item_node import Item_node

"""
    A common set of functions and things needed for this project

    Dependancies:
        json
"""

def importBigRecipeList(path: str):
    """
        Imports the massive JSON file into our known list of items
    """
    with open(path, "r") as f:
        big_dict = json.load(f)
        for entry in big_dict:
            recipe = []
            for recipe_item in entry['recipe']['ingredients']:
                recipe.append({'name': recipe_item['id'], 'quantity': recipe_item['amount']})
            item = Item(entry['id'], entry['recipe']['yield'] if recipe else 1, entry['recipe']['time'] if recipe else 0, recipe)
            AllItems[entry['id']] = item

def importJsonRecipes(path: str):
    """
        Imports a hand-crafted JSON file into our known list of items
    """
    with open(path, "r") as f:
        json_dict = json.load(f)
        for item, details in json_dict.items():
            AllItems[item] = Item(item, details["quantity"], details["duration"], details["recipe"])