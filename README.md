# SIC Assembler（兩段式組譯器

這份專案是一個以 C++ 撰寫的 **SIC（Simplified Instructional Computer）兩段式組譯器**範例。程式會讀取 SIC 組合語言輸入檔，進行 **Pass 1 / Pass 2**，建立各種表格（Symbol / Integer / String），最後輸出每一行的 **位置（LOC）、原始指令、以及物件碼（Object Code）** 到 `SIC_output.txt`。

> 備註：程式會優先讀 `SIC_input.txt`，若不存在則改讀 `SICXE_input.txt`（但 SIC/XE 相關流程目前在 main 內被註解掉）。

---

## 功能特色

- **Pass 1**
  - 讀取輸入檔並切 token（支援 `\t`、空白、`\n`、delimiter）。
  - 處理 `START` 起始位置並建立 LOC。
  - 建立/更新：
    - `Table5.table`：Symbol Table（符號與位址）
    - `Table6.table`：Integer Table（遇到純數字就放進來）
    - `Table7.table`：String Table（BYTE 的字串常數）
  - 針對部分指令/虛指令直接產生 objCode（如 `BYTE C'...' / X'...'`、`WORD`、`RSUB` 等）。

- **Pass 2**
  - 針對 Pass 1 尚未完成物件碼的行（`done == false`）：
    - 到 Symbol Table 查 operand 對應位址
    - 處理 `,X` indexed addressing（會把位址加上 `8000`）
    - 找不到 symbol 會輸出 `Error: Undefined Symbol!`

- **輸出**
  - `SIC_output.txt`：含欄位 `Line / Loc / Source statement / Object code`
  - `Table5.table / Table6.table / Table7.table`：三種表格輸出

---

## 專案檔案結構

程式碼本體可命名為 `main.cpp` 或 `sic.cpp`，並且需要下列 table 檔在同一個目錄：

- `Table1.table`：instruction（指令表）
- `Table2.table`：pseudo（虛指令表，例如 START/WORD/RESB...）
- `Table3.table`：register（暫存器表，若你的輸入會用到）
- `Table4.table`：delimiter（分隔符表，用來判斷特殊字元）
- `SIC_input.txt`（或 `SICXE_input.txt`）

執行後會產生：
- `SIC_output.txt`
- `Table5.table`
- `Table6.table`
- `Table7.table`

---

## 編譯與執行

### Linux / macOS（g++）
```bash
g++ -std=c++11 -O2 -o sic SIC.cpp
./sic
```

### Windows（MinGW g++）
```bash
g++ -std=c++11 -O2 -o sic.exe SIC.cpp
sic.exe
```

執行後請檢查輸出檔：
- `SIC_output.txt`


---

## 主要流程說明

程式進入點在 `main()`：
1. `readFile_and_Pass1()`
讀檔、切 token、建立 LOC、寫入各 table、初步產生 objCode

2. `Pass2()`
補齊尚未完成的 objCode（尤其是需要 symbol 位址解析者）

3. `writeFile()`
將結果輸出到 `SIC_output.txt`

---

## Object Code 產生規則（簡述）

- `BYTE X'..'`：直接使用十六進位字串當 objCode
- `BYTE C'..'`：把字元轉 ASCII，再轉成十六進位（補 0、並轉大寫）
- `WORD n`：把十進位整數轉成 6 碼十六進位（不足補 0）
- `RSUB`：固定輸出 `4C0000`
- 其他指令：使用內建 `Opcode_Table` 找到 opcode，後面接 symbol 位址（Pass1 找得到就直接完成，否則 Pass2 再補）

