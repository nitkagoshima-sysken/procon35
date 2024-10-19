import requests
import time
import json

url = "http://172.29.1.2:80/problem"#"http://localhost:8080/problem"

def get(url):
    #トークン
    token = "kagoshima15370fe7115e2c9680987187e634b2ed068f1274d3942407c30b019"#"token1"

    #リクエストヘッダーにトークンを追加
    headers={
        #"Authorization": f"Bearer {token}",
        "Procon-Token" : token,
        "Content-Type": "application/json"
    }

    response = requests.get(url, headers=headers)

    while response.status_code != 200:
        print("ファイルの取得に失敗しました")

        response = requests.get(url)

        if response.status_code == 200:
            print("ファイルとれたぜイェーーーーーーイ")
            break
        elif response.status_code == 400:
            print("リクエストの内容が不正")
        elif response.status_code == 401:
            print("トークンが指定されてないか不正と認識されてる")
        elif response.status_code == 403:
            print("競技時間外だ気を取り直してやり直そう")
        
        time.sleep(5)
    
    #問題のjsonファイルを代入
    problem_board = response.json()
    print(f"{problem_board=}")
    
    with open("./json/problem.json", "w") as f:
        json.dump(problem_board, f)


if __name__ == "__main__":
    get(url)
