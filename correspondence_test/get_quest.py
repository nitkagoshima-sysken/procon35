import requests
import json

url = "http://localhost:8080/problem"
headers = {
    "Procon-Token": "token1"
}

response = requests.get(url, headers=headers)

print(response.status_code)
print("文章データ")
print(response.text)
print("2次元配列")