import requests
import json

url = "http://localhost:8080/problem"
headers = {
    "Procon-Token": "token1"
}

response = requests.get(url, headers=headers)

print(response.status_code)
print("���̓f�[�^")
print(response.text)
print("2�����z��")