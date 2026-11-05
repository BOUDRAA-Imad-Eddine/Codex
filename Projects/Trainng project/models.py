# models.py
from sqlalchemy import Column, Integer, String
from database import Base

class User(Base):
    __tablename__ = 'users'
    id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True, nullable=False)
    hashed_password = Column(String, nullable=False)

def create_user(db, username: str, hashed_password: str):
    return User(username=username, hashed_password=hashed_password)