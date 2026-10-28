t1 = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
t2 = ['Janvier', 'Février', 'Mars', 'Avril', 'Mai', 'Juin',
 'Juillet', 'Août', 'Septembre', 'Octobre', 'Novembre', 'Décembre']

t3 = []
x = list(range(12))

for i in x:
    t3.append("Il y a " + str(t1[i]) + " jours en " + t2[i])
print(t3)