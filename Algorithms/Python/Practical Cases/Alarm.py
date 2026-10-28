import tkinter as tk
import playsound
from datetime import datetime


def check_alarm(dt):
    now = datetime.now()
    if now >= dt:


        label = tk.Label(root, text="Réveille-toi gros Batata!")
        label.pack()

        playsound.playsound("ALRMClok_Reveil electronique sonnerie 1 (ID 0035)_LS.mp3")


    else:

        root.after(1000, check_alarm, dt)  # Vérifie toutes les secondes


def get_input():
    date_str = entry.get()

    label = tk.Label(root, text="Votre alarme va sonner à : " + date_str)
    label.pack()
    
    entry.delete(0, tk.END)


    try:
        dt = datetime.strptime(date_str, '%H:%M:%S')
        root.after(1000, check_alarm, dt)  # Démarre la vérification de l'alarme


    except ValueError:
        label = tk.Label(root, text="Le format de l'heure est incorrect !")
        label.pack()


root = tk.Tk()
root.title("Alarm Clock")
root.geometry("350x200")


label = tk.Label(root, text="Entrez la date et l'heure (format HH:MM:SS) : ")
label.pack()


entry = tk.Entry(root, width=50)
entry.pack(pady=5)


button = tk.Button(root, text="Valider l'heure de la sonnerie", command=get_input)
button.pack(pady=10)


root.mainloop()