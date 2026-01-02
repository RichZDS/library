# 图书管理系统 (Library Management System)

基于 B 树数据结构的命令行图书管理系统，使用 C++17 实现。

## 📋 目录

- [项目简介](#项目简介)
- [项目结构](#项目结构)
- [编译与运行](#编译与运行)
- [功能特性](#功能特性)
- [B 树数据结构详解](#b-树数据结构详解)
  - [B 树基本概念](#b-树基本概念)
  - [BTreeBook 类函数详解](#btreebook-类函数详解)
  - [BTreeAuthor 类函数详解](#btreeauthor-类函数详解)
- [数据结构说明](#数据结构说明)
- [使用说明](#使用说明)

---

## 项目简介

这是一个使用 B 树（B-Tree）数据结构实现的图书管理系统。系统采用 B 树来存储图书信息，支持高效的查找、插入和删除操作。同时维护一个按作者索引的 B 树，方便按作者查询图书。

**核心特性：**
- 使用 B 树作为主要数据结构，保证 O(log n) 的时间复杂度
- 支持图书的增删改查
- 支持借阅和预约管理
- 提供作者索引，支持按作者快速查询
- 命令行交互界面，支持管理员和用户两种模式

---

## 项目结构

```
Library/
├── Main.cpp                    # 主程序入口，包含命令行界面
├── Makefile                    # 编译配置文件
├── README.md                   # 项目文档（本文件）
│
├── library_core/               # 核心库目录
│   ├── library.h              # 数据结构和函数声明
│   ├── library.cpp            # 业务逻辑实现
│   ├── btree_book.h           # 图书 B 树类声明
│   ├── btree_book.cpp         # 图书 B 树实现
│   ├── btree_author.h         # 作者索引 B 树类声明
│   └── btree_author.cpp       # 作者索引 B 树实现
│
└── datastruct/                 # 数据结构目录（兼容）
    └── library.h              # 数据结构的头文件
```

### 文件说明

#### 核心文件

1. **Main.cpp**
   - 程序入口点
   - 实现命令行用户界面
   - 处理用户输入和菜单选择
   - 集成管理员和用户两种模式

2. **library_core/library.h & library.cpp**
   - 定义核心数据结构：`Book`、`BorrowRecord`、`ReservationRecord`
   - 实现业务逻辑函数：图书管理、借阅管理、查询功能
   - 提供交互式和非交互式两种 API

3. **library_core/btree_book.h & btree_book.cpp**
   - 实现基于书号的 B 树索引
   - 支持图书的插入、删除、查找操作
   - 提供中序遍历和树结构可视化功能

4. **library_core/btree_author.h & btree_author.cpp**
   - 实现基于作者名的 B 树索引
   - 维护作者与图书 ID 的映射关系
   - 支持按作者快速查询图书

---

## 编译与运行

### 前置要求

- **编译器**：支持 C++17 标准的编译器（如 GCC 7+、Clang 5+、MSVC 2017+）
- **操作系统**：Windows、Linux、macOS 均可

### 使用 Makefile 编译

```bash
# 编译项目
make

# 运行程序
make run

# 清理编译产物
make clean
```

### 手动编译

```bash
# Linux/macOS
g++ -std=c++17 -Wall -Wextra -O2 -I library_core \
    Main.cpp library_core/library.cpp \
    library_core/btree_book.cpp library_core/btree_author.cpp \
    -o library_cli

# Windows (MinGW/MSVC)
# 使用类似命令，将 g++ 替换为 g++ (MinGW) 或 cl (MSVC)
```

### 直接运行

```bash
./library_cli    # Linux/macOS
library_cli.exe  # Windows
```

---

## 功能特性

### 管理员模式

1. **图书录入**：添加新书或增加现有图书的册数
2. **图书清除**：从系统中删除图书
3. **查询所有借阅者信息**：查看所有借阅记录
4. **查询所有预约者信息**：查看所有预约记录
5. **查看某书借阅与预约情况**：查询特定图书的详细信息
6. **作者索引查询**：按作者名查找图书
7. **书号查询**：按书号查找图书
8. **书名查询**：按书名查找图书

### 用户模式

1. **借书**：借阅指定图书
2. **还书**：归还已借阅的图书
3. **预约书籍**：预约当前无库存的图书
4. **查看所有图书信息**：浏览所有图书
5. **作者索引查询**：按作者查找图书
6. **书号查询**：按书号查找图书
7. **书名查询**：按书名查找图书

---

## B 树数据结构详解

### B 树基本概念

**B 树（B-Tree）** 是一种自平衡的树数据结构，广泛应用于数据库和文件系统中。本项目使用的是 **2-3 树**（B 树的一种特殊情况，最小度数 t=2）。

#### B 树的特性

1. **节点键数限制**：
   - 每个节点最多有 `2*t-1` 个键（对于 t=2，最多 3 个键）
   - 每个节点至少有 `t-1` 个键（对于 t=2，至少有 1 个键）
   - 根节点例外，可以少于 `t-1` 个键

2. **子节点数限制**：
   - 每个节点最多有 `2*t` 个子节点
   - 每个节点至少有 `t` 个子节点（除非是叶子节点）
   - 根节点例外，可以有任意个子节点

3. **排序性质**：
   - 节点内的键是有序的（从小到大）
   - 对于节点内的键 `keys[i]`：
     - `children[i]` 子树中的所有键都小于 `keys[i]`
     - `children[i+1]` 子树中的所有键都大于 `keys[i]`

4. **高度平衡**：
   - 所有叶子节点都在同一层
   - 树的高度是 O(log n)，其中 n 是键的总数

#### 为什么使用 B 树？

- **高效查找**：O(log n) 时间复杂度
- **适合磁盘存储**：减少磁盘 I/O 次数（相比二叉搜索树）
- **范围查询**：支持高效的范围查询和中序遍历
- **动态调整**：自动保持平衡，无需额外的平衡操作

---

## BTreeBook 类函数详解

`BTreeBook` 类使用 B 树结构，以书号（`book.id`）作为键来存储和管理图书信息。

### 节点结构

```cpp
struct BTreeBookNode {
    bool leaf;                          // 是否为叶子节点
    vector<string> keys;                // 书号列表（有序）
    vector<Book> records;               // 对应的图书记录
    vector<BTreeBookNode*> children;    // 子节点指针列表
};
```

### 构造函数和析构函数

#### `BTreeBook(int t = 2)`

**功能**：创建 B 树对象

**参数**：
- `t`：最小度数（degree），默认为 2（即 2-3 树）

**说明**：
- 初始化根节点为 `nullptr`（空树）
- 最小度数决定了每个节点键的数量范围

**示例**：
```cpp
BTreeBook books;  // 创建 2-3 树（默认）
```

---

#### `~BTreeBook()`

**功能**：析构函数，释放整个 B 树的内存

**说明**：
- 调用 `destroy(root)` 递归释放所有节点
- 使用后序遍历确保先释放子节点再释放父节点

---

### 搜索操作

#### `Book* search(const string& id) const`

**功能**：在 B 树中搜索指定书号的图书

**参数**：
- `id`：要查找的书号

**返回值**：
- 找到：返回指向 `Book` 对象的指针
- 未找到：返回 `nullptr`

**时间复杂度**：O(log n)

**算法流程**：
1. 从根节点开始
2. 在当前节点中找到第一个大于等于 `id` 的键的位置
3. 如果找到完全匹配的键，返回对应的记录
4. 如果是叶子节点且未找到，返回 `nullptr`
5. 否则递归搜索相应的子节点

**示例**：
```cpp
Book* book = books.search("B001");
if (book) {
    cout << "找到图书：" << book->title << endl;
}
```

---

#### `Book* searchNode(BTreeBookNode* x, const string& id) const`

**功能**：在指定节点及其子树中递归搜索

**参数**：
- `x`：当前搜索的节点
- `id`：要查找的书号

**返回值**：找到返回 `Book*`，否则返回 `nullptr`

**说明**：这是 `search()` 的递归实现，是核心搜索算法

---

### 插入操作

#### `void insert(const Book& book)`

**功能**：向 B 树中插入一本新书

**参数**：
- `book`：要插入的图书对象

**时间复杂度**：O(log n)

**算法流程**：
1. 如果树为空，创建根节点并插入
2. 如果根节点已满（有 `2*t-1` 个键）：
   - 创建新的根节点
   - 将原根节点作为新根的子节点
   - 分裂原根节点（这是 B 树**唯一**会增加树高度的操作）
3. 调用 `insertNonFull()` 在非满节点中插入

**示例**：
```cpp
Book newBook;
newBook.id = "B004";
newBook.title = "计算机网络";
// ... 设置其他属性
books.insert(newBook);
```

---

#### `void insertNonFull(BTreeBookNode* x, const Book& book)`

**功能**：在非满节点中插入新记录（递归函数）

**参数**：
- `x`：当前节点（保证不满）
- `book`：要插入的图书

**算法流程**：
1. **情况 1：叶子节点**
   - 找到正确的插入位置
   - 从右向左移动所有大于 `book.id` 的键
   - 在正确位置插入新键和记录

2. **情况 2：内部节点**
   - 找到应该插入的子节点索引 `i`
   - 如果子节点已满，先调用 `splitChild()` 分裂它
   - 分裂后需要判断新键应该插入左子节点还是右子节点
   - 递归调用 `insertNonFull()` 插入到子节点

**说明**：这是 B 树插入的核心递归函数，保证在插入过程中，目标节点始终不满

---

#### `void splitChild(BTreeBookNode* x, int i)`

**功能**：分裂已满的子节点（B 树保持平衡的关键操作）

**参数**：
- `x`：父节点
- `i`：要分裂的子节点在 `x->children` 中的索引

**操作流程**：
1. 获取已满的子节点 `y = x->children[i]`
2. 创建新节点 `z` 存放 `y` 的后半部分
3. 将 `y` 的后 `t-1` 个键和记录移动到 `z`
4. 如果 `y` 不是叶子节点，将后 `t` 个子节点也移动到 `z`
5. 将 `y` 的中间键（第 `t-1` 个）提升到父节点 `x` 的位置 `i`
6. 将新节点 `z` 插入到 `x->children` 的位置 `i+1`
7. 收缩 `y` 节点，只保留前 `t-1` 个键和 `t` 个子节点

**图示**（t=2，分裂前 y 有 3 个键）：
```
分裂前：
x: [key1]
    |
    y: [k1, k2, k3]  ← 已满

分裂后：
x: [key1, k2]  ← k2 被提升
    |    |
    y    z
[k1]   [k3]   ← y 和 z 各保留 1 个键
```

**说明**：分裂操作确保了 B 树的结构性质始终满足

---

### 删除操作

#### `bool remove(const string& id)`

**功能**：从 B 树中删除指定书号的图书

**参数**：
- `id`：要删除的书号

**返回值**：
- 成功删除返回 `true`
- 未找到返回 `false`

**时间复杂度**：O(n)（因为使用了重建法）

**算法说明**：
本项目使用的是**简化实现（重建法）**，而不是标准的 B 树删除算法：

1. 中序遍历收集所有图书数据
2. 从数据中删除目标图书
3. 销毁原树
4. 重建整个 B 树（重新插入所有数据）

**优点**：实现简单，易于理解

**缺点**：效率较低，但对于学习目的足够

**示例**：
```cpp
if (books.remove("B001")) {
    cout << "删除成功！" << endl;
}
```

**注意**：标准的 B 树删除算法更复杂，需要处理多种情况（叶子节点删除、内部节点删除、合并节点等），但时间复杂度是 O(log n)

---

### 遍历操作

#### `void inorder(vector<Book>& out) const`

**功能**：中序遍历 B 树，按书号顺序收集所有图书

**参数**：
- `out`：输出向量，用于存储遍历结果

**时间复杂度**：O(n)

**说明**：中序遍历保证了结果按书号的字典序排列

**示例**：
```cpp
vector<Book> allBooks;
books.inorder(allBooks);
for (const auto& book : allBooks) {
    cout << book.id << ": " << book.title << endl;
}
```

---

#### `void inorderNode(BTreeBookNode* x, vector<Book>& out) const`

**功能**：递归实现中序遍历

**算法**：
1. 对于节点中的每个键 `keys[i]`：
   - 如果不是叶子节点，先递归遍历 `children[i]`（左子树）
   - 访问当前键对应的记录 `records[i]`
2. 如果不是叶子节点，最后递归遍历 `children.back()`（右子树）

**说明**：这是 B 树中序遍历的核心递归函数

---

### 树结构可视化

#### `string getStructure() const`

**功能**：获取 B 树的凹入表（Indented Table）表示

**返回值**：字符串形式的树结构

**说明**：凹入表通过缩进层级表示树的父子关系，便于可视化 B 树结构

**示例输出**：
```
[ B001 B002 ]
    [ B001 ] (leaf)
    [ B002 B003 ] (leaf)
```

---

#### `void getStructureNode(BTreeBookNode* x, int depth, stringstream& ss) const`

**功能**：递归生成凹入表字符串

**参数**：
- `x`：当前节点
- `depth`：当前深度（用于控制缩进）
- `ss`：字符串流，用于拼接结果

---

### 辅助函数

#### `void destroy(BTreeBookNode* x)`

**功能**：递归销毁 B 树节点及其所有子节点

**参数**：
- `x`：要销毁的节点

**说明**：使用后序遍历，先销毁所有子节点，再销毁当前节点

---

## BTreeAuthor 类函数详解

`BTreeAuthor` 类使用 B 树结构，以作者名作为键，存储该作者的所有图书 ID 列表。这是一个**倒排索引**（Inverted Index）的实现。

### 节点结构

```cpp
struct BTreeAuthorNode {
    bool leaf;                          // 是否为叶子节点
    vector<string> keys;                // 作者名列表（有序）
    vector<vector<string>> ids;         // 每个作者对应的图书 ID 列表
    vector<BTreeAuthorNode*> children;  // 子节点指针列表
};
```

**注意**：与 `BTreeBookNode` 不同，这里每个键对应的是一个 `vector<string>`（图书 ID 列表），而不是单个记录。这样设计是为了支持一个作者有多本图书的情况。

---

### 构造函数和析构函数

#### `BTreeAuthor(int t = 2)`

**功能**：创建作者索引 B 树

**参数**：
- `t`：最小度数，默认为 2

---

#### `~BTreeAuthor()`

**功能**：析构函数，释放整个 B 树的内存

---

### 查找函数

#### `int findKeyIndex(BTreeAuthorNode* x, const string& key) const`

**功能**：在当前节点中查找键的位置

**参数**：
- `x`：当前节点
- `key`：要查找的作者名

**返回值**：第一个大于等于 `key` 的键的索引

**说明**：这是一个辅助函数，用于定位键的位置

---

#### `BTreeAuthorNode* searchNode(BTreeAuthorNode* x, const string& author, int& idx) const`

**功能**：搜索包含指定作者的节点

**参数**：
- `x`：当前搜索的节点
- `author`：要查找的作者名
- `idx`：输出参数，返回键在节点中的索引

**返回值**：
- 找到：返回包含该作者的节点指针
- 未找到：返回 `nullptr`

**说明**：这是作者搜索的核心递归函数

---

### 插入操作

#### `void add(const string& author, const string& id)`

**功能**：添加作者-图书 ID 的关联

**参数**：
- `author`：作者名
- `id`：图书 ID

**算法流程**：
1. 如果树为空，创建根节点并插入
2. 查找作者是否已存在：
   - **如果存在**：将图书 ID 添加到该作者的 ID 列表中（避免重复）
   - **如果不存在**：插入新作者节点

**示例**：
```cpp
authorIndex.add("王小明", "B001");
authorIndex.add("王小明", "B002");  // 同一作者的第二本书
```

---

#### `void insertNonFull(BTreeAuthorNode* x, const string& author, const string& id)`

**功能**：在非满节点中插入新的作者-图书 ID 关联

**说明**：与 `BTreeBook::insertNonFull()` 类似，但处理的是作者名和 ID 列表

---

#### `void splitChild(BTreeAuthorNode* x, int i)`

**功能**：分裂已满的子节点

**说明**：与 `BTreeBook::splitChild()` 类似，但需要同时处理键和 ID 列表

---

### 删除操作

#### `void removePair(const string& author, const string& id)`

**功能**：删除作者-图书 ID 的关联

**参数**：
- `author`：作者名
- `id`：要删除的图书 ID

**算法流程**：
1. 查找包含该作者的节点
2. 从该作者的 ID 列表中删除指定的 ID
3. 如果删除后 ID 列表为空，从 B 树中删除该作者节点（使用重建法）

**示例**：
```cpp
authorIndex.removePair("王小明", "B001");
```

---

### 查询操作

#### `vector<string> getIds(const string& author) const`

**功能**：根据作者名获取该作者的所有图书 ID 列表

**参数**：
- `author`：作者名

**返回值**：图书 ID 列表，如果作者不存在返回空列表

**时间复杂度**：O(log n)

**示例**：
```cpp
vector<string> bookIds = authorIndex.getIds("王小明");
for (const auto& id : bookIds) {
    // 使用 bookIds 在 BTreeBook 中查找对应的图书
}
```

---

### 辅助函数

#### `void inorderAuthors(BTreeAuthorNode* x, vector<pair<string, vector<string>>>& out) const`

**功能**：中序遍历，收集所有作者及其图书 ID 列表

**说明**：用于重建树或查询所有数据

---

#### `void destroy(BTreeAuthorNode* x)`

**功能**：递归销毁 B 树节点

---

## 数据结构说明

### Book（图书信息）

```cpp
struct Book {
    string id;              // 书号（唯一标识，作为 B 树键）
    string title;           // 书名
    string author;          // 作者
    string publisher;       // 出版社
    double price;           // 定价
    string publishDate;     // 出版时间
    int totalCopies;        // 总存量
    int availableCopies;    // 现存量
    vector<BorrowRecord> borrowRecords;        // 借阅记录列表
    vector<ReservationRecord> reservationRecords;  // 预约记录列表
};
```

---

### BorrowRecord（借阅记录）

```cpp
struct BorrowRecord {
    string borrowerId;          // 借阅者图书证号
    string borrowerName;        // 借阅者姓名
    time_t borrowTime;          // 借阅时间
    time_t expectedReturnTime;  // 预期归还时间
    time_t actualReturnTime;    // 实际归还时间（0 表示未归还）
};
```

---

### ReservationRecord（预约记录）

```cpp
struct ReservationRecord {
    string reserverId;          // 预约者图书证号
    string reserverName;        // 预约者姓名
    time_t reserveTime;         // 预约时间
    time_t expectedPickupTime;  // 预期取书时间
    time_t actualPickupTime;    // 实际取书时间（0 表示未取书）
};
```

---

## 使用说明

### 启动程序

编译并运行程序后，首先会看到主菜单：

```
========================================
        图书管理系统
========================================
1. 管理员
2. 用户
0. 退出
========================================
请选择模式:
```

### 管理员模式

选择 `1` 进入管理员模式，可以：
- 录入新书或增加图书册数
- 删除图书
- 查看所有借阅/预约记录
- 查询图书信息

### 用户模式

选择 `2` 进入用户模式，需要输入姓名和借阅证号，然后可以：
- 借阅图书
- 归还图书
- 预约图书
- 查询图书信息

### 退出程序

在任何菜单输入 `0` 即可退出程序。

---

## B 树操作可视化示例

### 插入过程示例

假设我们有一棵空的 2-3 树（t=2），依次插入书号 "B001", "B002", "B003"：

```
插入 B001:
[B001] (leaf)

插入 B002:
[B001 B002] (leaf)

插入 B003（节点已满，需要分裂）:
       [B002]  ← 新根节点
        /   \
   [B001]   [B003]  ← 分裂后的两个节点
```

### 搜索过程示例

在以下树中搜索 "B003"：

```
       [B002]
        /   \
   [B001]   [B003]
```

1. 从根节点 `[B002]` 开始
2. "B003" > "B002"，所以搜索右子树
3. 在 `[B003]` 节点中找到匹配，返回对应的记录

---

## 技术要点总结

1. **B 树分裂**：当节点满时，通过分裂保持树的平衡
2. **分裂提升**：中间键提升到父节点，确保树的高度增长只在根节点满时发生
3. **插入策略**：自上而下分裂已满的节点，确保插入路径上的节点都不满
4. **中序遍历**：按照 `children[i] → keys[i] → children[i+1]` 的顺序遍历，保证结果有序

---

## 扩展建议

1. **改进删除算法**：实现标准的 B 树删除算法（合并、借用等），提高删除效率
2. **持久化存储**：将 B 树数据保存到文件，实现数据持久化
3. **性能优化**：对于小数据集，可以考虑使用 B+ 树或其他数据结构
4. **并发支持**：添加读写锁，支持多线程并发访问

---

## 许可证

本项目仅供学习和研究使用。

---

**最后更新**：2024年
