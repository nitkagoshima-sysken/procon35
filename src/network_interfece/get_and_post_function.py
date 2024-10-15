import requests

def get(url):
    response = requests.get(url)

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


    #問題のjsonファイルを代入
    problem_board = response.json
    
    return problem_board



#postする関数
def post(url, answer_json):
    response = requests.post(url, answer_json)

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