import requests
import json

url = ""
response = requests.get(url)

while response.status_code != 200:
    requests.get(url)

#問題のjsonファイルを代入
problem_board = response.json

