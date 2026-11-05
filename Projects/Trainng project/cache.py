from redis import Redis
import json

redis_client = Redis(host='localhost', port=6379, db=0)

def blacklist_token(token: str, ttl: int = 3600):
    # Revoke a token by adding it to a Redis set
    redis_client.setex(f"blacklist:{token}", ttl, "true")

def is_token_blacklisted(token: str) -> bool:
    return redis_client.exists(f"blacklist:{token}") > 0