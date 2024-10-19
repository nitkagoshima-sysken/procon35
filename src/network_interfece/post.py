import requests
import json
import time

url = "http://172.29.1.2:80/answer"#"https://localhost:8080/answer"

# postする関数
def post(url):
    # トークン
    token = "kagoshima15370fe7115e2c9680987187e634b2ed068f1274d3942407c30b019"#"token1"

    # リクエストヘッダーにトークンを追加
    headers = {
        #"Authorization": f"Bearer {token}"
        "Procon-Token" : token,
        "Content-Type": "application/json"
    }

    answer_json = None
    with open("./json/answer.json", "r") as f:
        answer_json = json.load(f)

    print(f"{answer_json=}")

    response = requests.post(url, answer_json, headers=headers)

    while response.status_code != 200:
        print("送れん・・・")

        response = requests.post(url, answer_json)
        print(f"{response.status_code=}")

        if response.status_code == 200:
            print("送れた！！")
            break
        elif response.status_code == 400:
            print("リクエストの内容が不正")
        elif response.status_code == 401:
            print("トークンが指定されてないか不正と認識されてる")
        elif response.status_code == 403:
            print("競技時間外だ気を取り直してやり直そう")
        time.sleep(5)

    return 0


if __name__ == "__main__":
    post(url)
