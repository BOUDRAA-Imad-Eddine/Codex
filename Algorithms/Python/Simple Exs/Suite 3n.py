
#Voici un programme en Python qui génère et affiche une suite de 12 nombres où chaque terme est le triple du terme précédent :


term_p,term_c = 0, 1
term_s = term_p + term_c




nombre_termes = int(input("donner le nombre de termes : "))

suite = []




for _ in range(nombre_termes):
    term_p = term_c
    term_c = term_s
    
    term_s = term_c + term_p
    suite.append(term_s)


print("Suite de 12 nombres où chaque terme est le triple du terme précédent :")
print(suite)