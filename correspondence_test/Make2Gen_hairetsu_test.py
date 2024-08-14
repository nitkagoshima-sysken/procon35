import requests
import json

# JSONデータのサンプル
json_data = {
    "board": {
        "width": 6,
        "height": 4,
        "start": [
            "220103",
            "213033",
            "022103",
            "322033"
        ],
        "goal": [
            "000000",
            "111222",
            "222233",
            "333333"
        ]
    },
    "general": {
        "n": 2,
        "patterns": [
            {
                "p": 25,
                "width": 4,
                "height": 2,
                "cells": [
                    "0111",
                    "1001"
                ]
            },
            {
                "p": 26,
                "width": 2,
                "height": 2,
                "cells": [
                    "10",
                    "01"
                ]
            }
        ]
    }
}

# 'start'を2次元配列に変換
start_2d = [list(row) for row in json_data["board"]["start"]]

# 'goal'を2次元配列に変換
goal_2d = [list(row) for row in json_data["board"]["goal"]]

# 'patterns'を2次元配列に変換
patterns_2d = []
for pattern in json_data["general"]["patterns"]:
    pattern_cells = [list(row) for row in pattern["cells"]]
    patterns_2d.append(pattern_cells)

# 変換結果を表示
print("Start 2D Array:")
print(start_2d)

print("Goal 2D Array:")
print(goal_2d)

print("Patterns 2D Arrays:")
for pattern_2d in patterns_2d:
    print(pattern_2d)
