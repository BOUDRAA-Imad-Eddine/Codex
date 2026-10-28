import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import classification_report
import streamlit as st

# Charger un dataset fictif (remplace par ton propre dataset)
data = {
    'symptoms': ['fever, cough, muscle pain', 'sore throat, difficulty swallowing', 'abdominal pain, diarrhea'],
    'disease': ['Flu', 'Strep Throat', 'Gastroenteritis'],
    'treatment': ['Paracetamol, rest', 'Antibiotics, painkiller', 'Rehydration, anti-diarrheal']
}

# Créer un DataFrame à partir des données
df = pd.DataFrame(data)

# Prétraitement des données
df.drop_duplicates(inplace=True)
df.fillna('', inplace=True)

# Séparer les symptômes (X) et les maladies (y)
X = df['symptoms']
y = df['disease']

# Séparer en ensemble d'entraînement et de test
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Transformation des symptômes en vecteurs (TF-IDF)
vectorizer = TfidfVectorizer(stop_words='english')
X_train_tfidf = vectorizer.fit_transform(X_train)
X_test_tfidf = vectorizer.transform(X_test)

# Entraîner le modèle
model = LogisticRegression(max_iter=1000)
model.fit(X_train_tfidf, y_train)

# Prédiction sur l'ensemble de test
y_pred = model.predict(X_test_tfidf)

# Affichage des résultats du modèle
print(classification_report(y_test, y_pred))

# Fonction pour recommander un traitement
def recommander_traitement(maladie):
    traitements = {
        'Flu': 'Paracetamol, rest',
        'Strep Throat': 'Antibiotics, painkiller',
        'Gastroenteritis': 'Rehydration, anti-diarrheal'
    }
    return traitements.get(maladie, 'Treatment unknown. Please consult a healthcare professional.')

# Interface utilisateur avec Streamlit
st.title('Assistant Médical IA')
st.write("Entrez vos symptômes pour obtenir un diagnostic")

# Zone de texte pour les symptômes
symptomes = st.text_area("Symptômes", "fever, cough, muscle pain")

# Bouton pour prédire la maladie et recommander un traitement
if st.button('Prédire la maladie'):
    # Transformer les symptômes et faire la prédiction
    symptomes_tfidf = vectorizer.transform([symptomes])
    prediction = model.predict(symptomes_tfidf)[0]
    st.write(f"Maladie prédite : {prediction}")
    
    # Recommander le traitement
    st.write(f"Traitement recommandé : {recommander_traitement(prediction)}")
