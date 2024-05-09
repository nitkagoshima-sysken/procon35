# ソースコードのディレクトリ
SRC_DIR = src

# インクルードパスにsrc以下を追加
INC_PATH = -Isrc

# オブジェクトファイルの出力ディレクトリ
BUILD_DIR = build

# コンパイラのフラグ
CC = g++
CFLAGS = -Wall -Wextra -std=c++17

# メインファイル以外のソースコードファイルのリスト
SOURCES = $(wildcard $(SRC_DIR)/lib/*.cpp)

# オブジェクトファイルのリスト
OBJECTS = $(patsubst $(SRC_DIR)/lib/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# デフォルトターゲット
all: main

# プロジェクトのビルド
main: $(BUILD_DIR)/main.o $(OBJECTS)
	$(CC) $(CFLAGS) $(INC_PATH) $^ -o $@ -O2

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@ -O2

# オブジェクトファイルのビルド
$(BUILD_DIR)/%.o: $(SRC_DIR)/lib/%.cpp
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@ -O2

# NN学習データセット生成
gen: dataset_gen

# プロジェクトのビルド
dataset_gen: $(BUILD_DIR)/dataset_gen.o $(OBJECTS)
	$(CC) $(CFLAGS) $(INC_PATH) $^ -o $@ -O2

$(BUILD_DIR)/dataset_gen.o: $(SRC_DIR)/dataset_gen.cpp
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@ -O2

# クリーンアップ
clean:
# Windows用
# del $(BUILD_DIR) main.exe dataset_gen.exe
# Linux用
	rm -rf $(BUILD_DIR)/* main dataset_gen

# 再コンパイル
re:
	make clean
	make
