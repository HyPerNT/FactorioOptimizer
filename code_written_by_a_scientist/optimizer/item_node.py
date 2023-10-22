import math
from fractions import Fraction
from optimizer.item import AllItems


"""
    A module for handling tree generation, traversal, and modification to model the recipe production chain for Factorio items
    
    Dependancies:
        math
        fractions
        common
"""

class Item_node():
    """
        A class that stores the internal representation of "assemblers", "items", "recipes", and such for graph traversal and optimization

        Attributes:
            - name (str): The name for this item, as stored in the JSON
            - item (Item): The Item object for this particular recipe
            - assemblers (int): The number of assemblers used for this recipe
            - isOptimized (bool): Whether this node has been traversed and optimized yet
            - children (list): A list of other Item_node's that are direct descendants of this node
            - duration (float): The amount of time (in seconds) to process this recipe
            - qty (int): The total output quantity (self.assemblers * self.item.quantity) for this node
            - eff_rate (float): The effective rate (in items / sec) for this node.
            - deltas (list): The list of calculated deltas for this node relative to its children
            
        Methods:
            naive_sum_delta(self):
                Calculates and returns the signed sum of deltas for the subtree

            abs_sum_delta(self):
                Calculates and returns the unsigned sum of deltas for the subtree

            print_soln(self):
                Pretty-prints the complete solution
            
            print_assemblers(self):
                Pretty-prints the assemblers (and eff_rates) of nodes in the graph
            
            print_raw(self):
                Pretty-prints the required raw material rates of leaf nodes in the tree
            
            update_deltas(self):
                Recursively recalculates deltas for the subtree

            get_child_item(self, name):
                Returns the dictionary corresponding to an item as an ingredient for a recipe

            LCM(self, n1, n2):
                Returns the (integral) LCM of two rational numbers

            LCM_list(self, numbers):
                Returns the (integral) LCM of any number of rational numbers
                
            optimize(self):
                The entrypoint to optimizing a subgraph/subtree. Will minimize delta (and abs(delta)) of itself and descendents before exiting.

            message(self, factor):
                An entrypoint during optimization of subgraphs that returns to parents (or ancestors? Need to read the paper) a factor by which is necessary
                to increase to ensure minimum delta. 
                Nodes receiving a message() call should broadcast to their children and inform them propagation is needed downwards, but initial messages are passed upwards.

            LCM_list()
    """
    def __init__(self, name: str):
        """
            Naive constructor. Holds references to children and immediate Item object
        """
        self.name = name
        self.item = AllItems[self.name]
        self.assemblers = (1 if len(self.item.recipe) > 0 else 0)
        self.isOptimized = False
        self.children = []

        # Must instantiate our kiddos first
        for item in self.item.recipe:
            kid_name = item["name"]
            self.children.append(Item_node(kid_name))

        # Instantiate internal tree data from recipe information
        self.duration = (self.item.duration if len(self.item.recipe) > 0 else 0)

        # Is dynamically updated. Not necessarily always the "raw" recipe output
        self.qty = self.item.quantity

        # Is useful for fast \Delta calculation while optimizing
        self.eff_rate = ( 0 if self.duration == 0 else self.qty/self.duration )
        self.deltas = []

        # TL;DR: We must traverse the list of recipes + children in parallel in a wacky way because my Item implementation is shit
        for child in self.children:
            child_name = child.name
            child_item = self.get_child_item(child_name)
            if len(AllItems[child_name].recipe) == 0:
                self.deltas.append(0)
            # Finally, run the calculation
            self.deltas.append(child.eff_rate - (self.assemblers * child_item["quantity"]))

    def naive_sum_delta(self) -> float:
        """
            Returns the naive (signed) sum of deltas for this subtree
        """
        tot = 0
        for child in self.children:
            tot += child.naive_sum_delta()
        tot += sum(self.deltas)
        return tot

    def abs_sum_delta(self) -> float:
        """
            Returns the absolute (unsigned) sum of deltas for this subtree
        """
        tot = 0
        for child in self.children:
            tot += child.abs_sum_delta()
        for delta in self.deltas:
            tot += abs(delta)
        return tot

    def print_soln(self) -> None:
        """
            Prints out the desired solution from optimization
        """
        self.print_assemblers()
        self.print_raw()
                   
    def print_assemblers(self) -> None:
        """
            Prints out the assemblers from optimization
        """
        if self.assemblers != 0:    
            print(f"{int(self.assemblers)} Assemblers for {self.name} (eff_rate: {self.eff_rate:.3f})")

        for child in self.children:
            child.print_assemblers()
        
    def print_raw(self) -> None:
        """
            Prints out the raw mat rates from optimization    
        """
        if self.assemblers == 0:    
            print(f"{int(self.qty)} {self.name} per second")

        for child in self.children:
            child.print_raw()

    def update_deltas(self) -> None:
        """
            Internal implementation. **SHOULD NOT** be called directly by an outsider since it wastes time/memory and only needs to be done when message()s are 
            recieved or in optimization

            Unless you want to print stuff out... maybe
        """
        if (self.assemblers == 0):
            return
        self.eff_rate = self.qty/self.duration
        self.deltas = [(self.eff_rate - i.eff_rate) for i in self.children]

    def get_child_item(self, name: str) -> dict:
        """
            Returns the internal recipe dict for the appropriate child
        """
        for child_item in self.item.recipe:
            if child_item["name"] == name:
                return child_item
        raise Exception(f"Could not find child '{name}' for item '{self.name}'\n\
                            item: {self.item}")

    def LCM(self, n1: float, n2: float) -> int:
        """
            Returns the LCM of two (rational) numbers
        """
        f1 = Fraction(n1).limit_denominator(20)
        f2 = Fraction(n2).limit_denominator(20)
        try:
            return math.lcm(f1.numerator, f2.numerator) / math.gcd(f1.denominator, f2.denominator)
        except:
            raise Exception(f"Cannot LCM the following rational numbers: {n1}, {n2}")
    
    def LCM_list(self, numbers: list) -> int:
        """
            Returns _thee_ LCM of many numbers
        """
        length = len(numbers)
        match length:
            case 0:
                return 1
            case 1:
                return numbers[0]
            case 2:
                return self.LCM(numbers[0], numbers[1])
        lcm = self.LCM(numbers[0], numbers[1])
        for no in numbers[2:]:
            lcm = self.LCM(no, lcm)
        return lcm

    def optimize(self, mode="Exact", **kwargs):
        """
            Optimizes itself and children. In theory, will recursively optimize() children and broadcast/update itself with message() calls as necessary in optimization.
            Upon termination, will have been optimized (as are its subtrees). Any future optimization calls are **unneccessary**, and indicate an error.

            Optimization can take place in several modes as the following:
                - Exact: Minimize delta of all internal nodes to 0
                - Q-limit: Minimize delta within given q (# of assemblers)
                - P-limit: Minimize delta to achieve desired output rate (of final product)
        """
        if self.isOptimized:
            raise Exception(f"Subtree with root {self.name} is being optimized unnecessarily")

        # Base case for leaf nodes / raw ingredient
        if len(self.item.recipe) == 0:
            self.isOptimized = True
            return

        # Let everything else do the work
        for child in self.children:
            child.optimize(mode, **kwargs)

        # Okay, now we might need to optimize ourselves, _BUT_, children are all Gucci
        # Reading their assembler counts/recipe quantities, we can infer what we must do
        match mode:
            case "Exact":
                candidate_multiplicities = []
                for child in self.children:
                    child_item = self.get_child_item(child.name)
                    # Child is a raw material, update it and proceed
                    if len(AllItems[child.name].recipe) == 0:
                        # child.message(child_item["quantity"])
                        continue

                    # We basically find LCM per-child, then find the LCM of that
                    # Then, factor out the prior known LCM-per-child and message the result to each child
                    lhs = child_item["quantity"] * self.duration
                    rhs = child.qty * child.duration
                    candidate_multiplicities.append(self.LCM(lhs, rhs))
                big_lcm = self.LCM_list(candidate_multiplicities)
                for child, lcm in zip(self.children, candidate_multiplicities):
                    child.message(big_lcm / lcm)
                self.assemblers *= big_lcm
                self.qty *= big_lcm
            case "Q-limit":
                # TODO: Implement
                raise Exception("Q-limit has not been implemented yet")
            case "P-limit":
                # TODO: Implement
                raise Exception("P-limit has not been implemented yet")
            case _:
                Exception(f"Invalid optimization mode selected: {mode}")                
        self.isOptimized = True

    def message(self, factor: int):
        """
            Used internally during the optimization process. When a message() call is received, we must always broadcast it to our children
            The actual factor is multiplicative and affects our number of assemblers
        """
        if not self.isOptimized:
            raise Exception(f"Received a message() update before optimized. This shouldn't happen\n\t\tchild receiving message: {self.name}\n\t\tchildren: {self.children}")
        
        self.assemblers *= factor
        for child in self.children:
            child.message(factor)
        return