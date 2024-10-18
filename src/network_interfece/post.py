import requests
import json

url = "localhost8080/problem"

# postする関数
def post(url):
    # トークン
    token = "kagoshima15370fe7115e2c9680987187e634b2ed068f1274d3942407c30b019"

    # リクエストヘッダーにトークンを追加
    headers = {
        "Authorization": f"Bearer {token}"
    }

    answer_json = None
    with open("./json/answer.json", "r") as f:
        answer_json = json.load(f)

    print(f"{answer_json=}")

    response = requests.post(url, answer_json, headers=headers)

    while response.status_code != 200:
        print("送れん・・・")

        response = requests.post(url, answer_json)

        if response.status_code == 200:
            print("送れた！！")
            break
        elif response.status_code == 400:
            print("リクエストの内容が不正")
        elif response.status_code == 401:
            print("トークンが指定されてないか不正と認識されてる")
        elif response.status_code == 403:
            print("競技時間外だ気を取り直してやり直そう")

    return 0
