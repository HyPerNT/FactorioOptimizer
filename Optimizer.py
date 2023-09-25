from Recipes import AllItems, Item, importJsonRecipes, importBigRecipeList
from os import system


def doRecipeImport():
    # importJsonRecipes("recipe_list/recipe_list.json")
    importBigRecipeList("recipe_list/recipes.json")

def main():
    doRecipeImport()
    print("Enter desired product\nEnter \"exit\" to quit")
    while(True):
        selection = input(" >> ")
        if selection.lower() == "exit":
            exit()
        if selection.lower() == "clear":
            _ = system("clear")
            continue
        if 'list' in selection.lower():
            if selection.lower() == 'list':
                for name in AllItems.keys():
                    print(name)
                continue
            if 'dict' in selection.lower():
                for name, dict in AllItems.items():
                    print(dict)
                continue            
            substring = ' '.join(selection.split(" ")[1:])
            for name, dict in AllItems.items():
                if substring in name:
                    print(name)
            continue
        string = []
        for sub in selection.split(" "):
            string.append(sub.lower())
        selection = " ".join(string)
        if selection in AllItems:
          AllItems[selection].chefs()
        else:
            print(f"\"{selection}\" is not a valid recipe")


if __name__ == "__main__":
    main()