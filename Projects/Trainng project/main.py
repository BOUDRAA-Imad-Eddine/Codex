from fastapi import FastAPI, Depends, HTTPException, status
from sqlalchemy.orm import Session
import database
from database import engine, Base, get_db
import models
import bcrypt
import auth
from cache import blacklist_token
from fastapi.middleware.cors import CORSMiddleware
from fastapi.security import OAuth2PasswordRequestForm # Add this import

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# This ensures tables are created in the database when the app starts
models.Base.metadata.create_all(bind=database.engine)
@app.get("/current_user")
def current_user(token: str = Depends(auth.oauth2_scheme)):
    user = auth.get_current_user(token)
    return {"username": user}

@app.post("/register")
def register(username: str, password: str, db: Session = Depends(database.get_db)):
    # 1. Hash the password correctly using bcrypt
    hashed_password = auth.get_password_hash(password)
    
    # 2. Create the user object
    new_user = models.create_user(db, username=username, hashed_password=hashed_password)
    
    # 3. Add and commit to the database
    db.add(new_user)
    db.commit()
    db.refresh(new_user)
    
    return {"message": "User registered successfully"}

@app.post("/login")
def login(form_data: OAuth2PasswordRequestForm = Depends(), db: Session = Depends(database.get_db)):
    # 1. Fetch user from DB using form_data.username
    user = db.query(models.User).filter(models.User.username == form_data.username).first()
    
    # 2. Verify password using form_data.password
    if not user or not auth.verify_password(form_data.password, user.hashed_password):
        raise HTTPException(status_code=401, detail="Invalid credentials")
    
    # 3. Create token
    token = auth.create_access_token(data={"sub": form_data.username})
    return {"access_token": token, "token_type": "bearer"}

@app.post("/logout")
def logout(token: str = Depends(auth.oauth2_scheme)):
    blacklist_token(token)
    return {"message": "Successfully logged out"}