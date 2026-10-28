while True:
    try:
        term = int(input("Quel terme de la suite de fibonacci voulez vous connaitre : "))
        
        def fibonacci(n: int):

            if n == 0 :
                return 0
            if n == 1 :
                return 1
            else:
                return fibonacci(n - 1) + fibonacci(n - 2)
            
        print(fibonacci(term))   
    except ValueError:
        print("Donner une Valeur Valide")
    

    