# Importation des bibliothèques nécessaires
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from Bio import SeqIO
from collections import Counter

print("=============================Bases de Python=======================================")

# Variables et Types de Données
a = 5
b = 3.14
s = "Hello, World!"
print(a, b, s)



# Listes
my_list = [1, 2, 3, 4, 5]
print("Liste:", my_list)

# Tuples
my_tuple = (1, 2, 3)
print("Tuple:", my_tuple)

# Dictionnaires
Schumi = {"name": "Schumacher", "titles": 7}
print("GOAT:", Schumi)

# Conditions
if a > 3:
    print("a est plus grand que 3")
else:
    print("a est inférieur ou égal à 3")

# Boucles
# Boucle for
for i in range(5):
    print("For loop:", i)

# Boucle while
count = 0
while count < 5:
    print("While loop:", count)
    count += 1


print("=============================PARTIE BIOLOGIE ET AFFICHAGE DE DONNEES===============================================")

# Utilisation de NumPy pour les calculs scientifiques
array = np.array([1, 2, 3, 4, 5]) 
print("\nTableau NumPy:", array)
mean_value = np.mean(array)
print("Moyenne du tableau:", mean_value)

# Utilisation de Pandas pour la manipulation de données
data = {'Géne': ['géne1', 'géne2', 'géne3'], 'Expression': [10, 17, 7]}
df = pd.DataFrame(data)
print("\nDataFrame Pandas:")
print(df)
mean_expression = df['Expression'].mean()
print("Moyenne d'expression génique:", mean_expression)

# Utilisation de Matplotlib pour la visualisation des données

plt.figure(figsize=(10, 7))
plt.bar(df['Géne'], df['Expression'], color='blue')
plt.xlabel('Géne')
plt.ylabel('Niveau d''Expression')
plt.title('Niveau d''Expression des génes')
plt.show()






# Lecture d'un fichier CSV
data = pd.read_csv("donnees_biologie.csv")

# Affichage des premières lignes du fichier
print(data.head())

# Nettoyage des données en supprimant les lignes avec des valeurs manquantes
data_cleaned = data.dropna()

# Affichage des informations générales sur les données
print(data_cleaned.info())
