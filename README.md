# Plazza
The purpose of this project is to make a simulation of a pizzeria, which is composed of the
reception that accepts new commands, of several kitchens, themselves with several cooks, themselves
cooking several pizzas
 
## Usage
```
USAGE
    ./plazza c n r

DESCRIPTION
    c	multiplier for the cooking time of the pizzas
    n	number of cooks per kitchen
    r	time in milliseconds used by the kitchen stock to replace ingredients
```

Pizza ordering MUST respect the following grammar :  

S := TYPE SIZE NUMBER [; TYPE SIZE NUMBER ]*  
TYPE := [ a .. zA .. Z ]+  
SIZE := S | M | L | XL | XXL  
NUMBER := x [1..9][0..9]*  

Ordering example which is grammatically valid :  

regina XXL x2; fantasia M x3; margarita S x1


## Contributors

- [Martin Olivier](https://github.com/tocola)
- [Benjamin Reigner](https://github.com/Breigner01)
- [Coline Seguret](https://github.com/Cleopha)
