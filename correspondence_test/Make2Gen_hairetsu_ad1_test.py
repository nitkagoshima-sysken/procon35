import requests
import json

url = "http://localhost:8080/problem"
headers = {
    "Procon-Token": "token1"
}

response = requests.get(url, headers=headers)

# �X�e�[�^�X�R�[�h��\��
print("�X�e�[�^�X�R�[�h")
print(response.status_code)

# �󂯎����JSON�f�[�^���e�L�X�g�Ƃ��ĕ\��
print("���̓f�[�^")
print(response.text)

# JSON�f�[�^���p�[�X
json_data = response.json()

# 'start'��2�����z��ɕϊ�
start_2d = [list(row) for row in json_data["board"]["start"]]

# 'goal'��2�����z��ɕϊ�
goal_2d = [list(row) for row in json_data["board"]["goal"]]

# 'patterns'��2�����z��ɕϊ�
patterns_2d = []
for pattern in json_data["general"]["patterns"]:
    pattern_cells = [list(row) for row in pattern["cells"]]
    patterns_2d.append(pattern_cells)

# 2�����z��̓��e��\��
print("Start 2�����z��:")
for row in start_2d:
    print(row)

print("Goal 2�����z��:")
for row in goal_2d:
    print(row)

print("Patterns 2�����z��:")
for i, pattern_2d in enumerate(patterns_2d):
    print(f"Pattern {i + 1}:")
    for row in pattern_2d:
        print(row)
