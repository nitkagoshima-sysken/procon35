# ターゲットバイナリの名前
TARGET = main

# ソースコードのディレクトリ
SRC_DIR = src

# インクルードパスにnlohmann/json.hppを追加
INC_PATH = -Isrc
#INC_PATH = -Isrc $(shell python3.10 -m pybind11 --includes)

# オブジェクトファイルの出力ディレクトリ
BUILD_DIR = build

# コンパイラのフラグ
CC = g++
CFLAGS = -Wall -Wextra -O3 -std=c++17 -I /usr/include/python3.10#-Wextra -shared -fPIC python3.10のところは自分が入れてるバージョンを
CFLAGS += $(shell python3 -m pybind11 --includes)

# CFLAGS = -Wall -Wextra -std=c++17

# ソースコードファイルのリスト
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# オブジェクトファイルのリスト
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# デフォルトターゲット
all: $(TARGET)

# プロジェクトのビルド
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(INC_PATH) $^ -o $@ -O2

# オブジェクトファイルのビルド
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@ -O2

# クリーンアップ
clean:
# Windows用
#del $(BUILD_DIR) $(TARGET).exe
# Linux用
	rm -rf $(BUILD_DIR) $(TARGET)

# 再コンパイル
re:
	make clean
	make
