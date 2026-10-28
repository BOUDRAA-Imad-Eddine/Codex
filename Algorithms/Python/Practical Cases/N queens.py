def is_safe(board, row, col, N) :
  
    for i in range(row):
        if board[i] == col or abs(board[i] - col) == abs(i - row):
            return False
    return True

def solve_n_queens(board, row, N, solutions) :

    if row == N:
        solutions.append(board[:]) 
        return

    for col in range(N):
        if is_safe(board, row, col, N):
            board[row] = col 
            solve_n_queens(board, row + 1, N, solutions)
            board[row] = -1 

def solutions_n_queens(N) :
  
    solutions = []
    board = [-1] * N 
    solve_n_queens(board, 0, N, solutions)
    return solutions

def print_board(solution):
   
    N = len(solution)
    for row in range(N):
        line = ["." for _ in range(N)]
        line[solution[row]] = "Q"
        print(" ".join(line))
    print("\n")

def menu():
   
    while True:
        try:
            N = int(input("Entrez la valeur de N (taille de l'échiquier) : "))
            if N < 1:
                print("Veuillez entrer un entier positif !")
                continue

            solutions = solutions_n_queens(N)

            while True:
                print("\n--- Menu ---")
                print("1. Voir le nombre de solutions")
                print("2. Afficher toutes les solutions")
                print("3. Changer la valeur de N")
                print("4. Quitter")
                choix = input("Choisissez une option : ")

                if choix == "1":
                    print(f"\nNombre de solutions pour N={N} : {len(solutions)}")
                elif choix == "2":
                    if solutions:
                        print(f"\nSolutions pour N={N} :")
                        for i, sol in enumerate(solutions):
                            print(f"\nSolution {i + 1}:")
                            print_board(sol)
                    else:
                        print("Aucune solution trouvée.")
                elif choix == "3":
                    break  
                elif choix == "4":
                    print("Au revoir !")
                    return
                else:
                    print("Option invalide, veuillez réessayer.")

        except ValueError:
            print("Veuillez entrer un nombre valide !")


menu()
