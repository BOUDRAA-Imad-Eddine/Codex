import playsound
import time
from datetime import datetime



# Demander à l'utilisateur d'entrer une date et une heure
date_str = input("Entrez la date et l'heure (format YYYY-MM-DD HH:MM:SS) : ")

try:
    # Convertir la chaîne en objet datetime
    dt = datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
    print(f"votre alarme ce declancheras le : {dt}")
except ValueError:
    print("Le format de la date et de l'heure est incorrect. Veuillez utiliser le format YYYY-MM-DD HH:MM:SS.")



#fait sonner l'alarme 
while True :
    now = datetime.now()

    if now >= dt:
        print("Reveille toi gros batata")

        playsound.playsound('C:/Users/PC/Downloads/ALRMClok_Reveil electronique sonnerie 1 (ID 0035)_LS.mp3')
        break
    
input("Voulez vous stopper l'alarme")

