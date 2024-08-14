import requests
import json

url = "http://localhost:8080/problem"
headers = {
    "Procon-Token": "token1"
}

response = requests.get(url, headers=headers)

# ステータスコードを表示
print("ステータスコード")
print(response.status_code)

# 受け取ったJSONデータをテキストとして表示
print("文章データ")
print(response.text)

# JSONデータをパース
json_data = response.json()

# 'start'を2次元配列に変換
start_2d = [list(row) for row in json_data["board"]["start"]]

# 'goal'を2次元配列に変換
goal_2d = [list(row) for row in json_data["board"]["goal"]]

# 'patterns'を2次元配列に変換
patterns_2d = []
for pattern in json_data["general"]["patterns"]:
    pattern_cells = [list(row) for row in pattern["cells"]]
    patterns_2d.append(pattern_cells)

# 2次元配列の内容を表示
print("Start 2次元配列:")
for row in start_2d:
    print(row)

print("Goal 2次元配列:")
for row in goal_2d:
    print(row)

print("Patterns 2次元配列:")
for i, pattern_2d in enumerate(patterns_2d):
    print(f"Pattern {i + 1}:")
    for row in pattern_2d:
        print(row)
