from optimizer import importBigRecipeList, AllItems, Item_node
from os import system
import readline

"""
    Main interface for the project, provides a command-line-like interface for use
    Can be launched from the command-line

    Dependancies:
        os
        readline

    Usage:
        list [name]: Lists all recognized items, optionally filters by [name] when provided
        dict [name]: Lists all recognized items as internally stored, optionally filters by [name] when provided
        clear: Clears the prompt
        exit: Exits the prompt
                
"""

def doRecipeImport():
    importBigRecipeList("recipe_list/recipes.json")

def main():
    # Get recipes
    doRecipeImport()
    print("Enter desired product\nEnter \"exit\" to quit")
    while(True):
        selection = input(" >> ")

        # Exit
        if selection.lower() == "exit":
            exit()
        
        # Clear
        if selection.lower() == "clear":
            _ = system("clear")
            continue

        # Dict
        if 'dict' in selection.lower():
            substring = ' '.join(selection.split(" ")[1:])
            for name, dict in AllItems.items():
                if substring in name:
                    print(dict)
            continue

        # List
        if 'list' in selection.lower():
            substring = ' '.join(selection.split(" ")[1:])
            for name, dict in AllItems.items():
                if substring in name:
                    print(name)
            continue

        # Actual optimizing
        string = []
        for sub in selection.split(" "):
            string.append(sub.lower())
        selection = " ".join(string)
        if selection in AllItems:
            opt = Item_node(selection)
            opt.update_deltas()
            print(f'Prior to optimization:\nDelta: {opt.naive_sum_delta():.3f}\nAbs Delta: {opt.abs_sum_delta():.3f}n')
            opt.optimize()
            opt.update_deltas()
            print(f'After optimization:\nDelta: {opt.naive_sum_delta():.3f}\nAbs Delta: {opt.abs_sum_delta():.3}\n')
            opt.print_soln()

        else:
            print(f"\"{selection}\" is not a valid recipe")


if __name__ == "__main__":
    main()