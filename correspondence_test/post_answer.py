import requests
import json

url = "http://localhost:8080/answer"
headers = {
    "Procon-Token": "token1",
    "Content-Type": "application/json"
}
data = {
  "n": 3,
  "ops": [
    {
      "p": 4,
      "x": 1,
      "y": 1,
      "s": 2
    },
    {
      "p": 11,
      "x": 3,
      "y": -12,
      "s": 3
    },
    {
      "p": 25,
      "x": 3,
      "y": 3,
      "s": 0
    }
  ]
}

response = requests.post(url, headers=headers, data=json.dumps(data))

print(response.status_code)
print(response.json())

#あああああああああああーーーーーーーーーー