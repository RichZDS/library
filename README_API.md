# 图书管理系统 - Web界面使用说明

## 项目结构

```
Library/
├── Main_API.cpp          # API接口程序（新增）
├── server.js             # Node.js后端服务器（新增）
├── package.json          # Node.js依赖配置（新增）
├── public/
│   └── index.html        # 前端界面（新增）
├── datastruct/
│   ├── btree_book.cpp    # B树算法（已添加详细注释）
│   └── btree_author.cpp  # 作者索引B树（已添加详细注释）
└── ...                   # 其他原有文件
```

## 功能说明

### 1. B树算法注释
- `datastruct/btree_book.cpp` - 图书B树实现，包含详细的中文注释
- `datastruct/btree_author.cpp` - 作者索引B树实现，包含详细的中文注释

### 2. API接口
- `Main_API.cpp` - 提供命令行接口，接收参数并输出JSON格式结果

### 3. Web界面
- 前端：`public/index.html` - 现代化的Web界面
- 后端：`server.js` - Express服务器，调用C++程序

## 安装和运行步骤

### 步骤1：编译C++ API程序

```bash
# 使用g++编译（需要C++11或更高版本）
g++ -std=c++11 Main_API.cpp controller/library.h datastruct/btree_book.cpp datastruct/btree_author.cpp logic/*.cpp -o Library_API.exe

# 或者使用Visual Studio编译器
cl /EHsc Main_API.cpp datastruct/btree_book.cpp datastruct/btree_author.cpp logic/*.cpp /Fe:Library_API.exe
```

**注意**：需要包含所有必要的源文件。如果编译出错，可能需要调整包含路径或添加其他依赖文件。

### 步骤2：安装Node.js依赖

```bash
npm install
```

### 步骤3：启动服务器

```bash
npm start
```

服务器将在 `http://localhost:3000` 启动。

### 步骤4：打开浏览器

访问 `http://localhost:3000` 即可使用Web界面。

## API接口说明

### 添加图书
```
POST /api/books
Body: { id, title, author, publisher, price, totalCopies }
```

### 查询图书（按ID）
```
GET /api/books/:id
```

### 查询图书（按作者）
```
GET /api/books/author/:author
```

### 借阅图书
```
POST /api/books/:id/borrow
```

### 归还图书
```
POST /api/books/:id/return
```

### 列出所有图书
```
GET /api/books
```

### 删除图书
```
DELETE /api/books/:id
```

## 命令行直接调用C++程序

也可以直接通过命令行调用C++程序：

```bash
# 添加图书
Library_API.exe add "B001" "算法导论" "Thomas H. Cormen" "机械工业出版社" "89.00" "10"

# 查询图书
Library_API.exe query "B001"

# 借阅图书
Library_API.exe borrow "B001"

# 归还图书
Library_API.exe return "B001"

# 按作者查询
Library_API.exe queryAuthor "Thomas H. Cormen"

# 列出所有图书
Library_API.exe list
```

## B树算法学习要点

### 1. B树的基本概念
- **2-3树**：每个节点最多有2个键（3个键值区间），最少有1个键
- **最小度数t**：对于2-3树，t=2
- **节点容量**：最多2*t-1=3个键，最少t-1=1个键

### 2. 关键操作

#### 搜索（Search）
- 从根节点开始
- 在当前节点中查找键的位置
- 如果找到，返回记录
- 如果未找到且是叶子节点，返回null
- 否则递归搜索相应的子节点

#### 插入（Insert）
- 如果根节点已满，先分裂根节点（增加树高）
- 在非满节点中插入
- 如果目标子节点已满，先分裂再插入
- 在叶子节点中保持有序插入

#### 分裂（Split）
- 当节点有2*t-1个键时触发
- 将节点分成两部分：左节点（t-1个键）和右节点（t-1个键）
- 中间键提升到父节点
- 如果节点有子节点，子节点也要相应分配

#### 删除（当前实现为简化版）
- 采用重建法：收集所有数据，删除目标，重建树
- 真正的B树删除算法更复杂，需要处理多种情况

### 3. 时间复杂度
- **搜索**：O(log n)，其中n是键的数量
- **插入**：O(log n)
- **删除**：O(log n)（标准实现），当前简化实现为O(n)

## 注意事项

1. **数据持久化**：当前实现中，数据存储在内存中，程序重启后数据会丢失。如需持久化，需要添加文件读写功能。

2. **并发安全**：当前实现不支持多线程并发访问。如需支持，需要添加锁机制。

3. **错误处理**：已添加基本的错误处理，但可能需要根据实际需求进一步完善。

4. **编译依赖**：确保所有源文件都能正确编译，可能需要调整包含路径。

## 故障排除

### 问题1：编译错误
- 检查是否包含所有必要的源文件
- 确保C++编译器支持C++11标准
- 检查头文件路径是否正确

### 问题2：Node.js服务器无法启动
- 检查是否已安装Node.js（需要v12或更高版本）
- 检查是否已运行 `npm install`
- 检查端口3000是否被占用

### 问题3：无法调用C++程序
- 确保 `Library_API.exe` 已正确编译
- 检查 `server.js` 中的 `CPP_EXE` 路径是否正确
- 在Windows上，确保可执行文件路径使用正确的分隔符

### 问题4：前端无法连接后端
- 检查后端服务器是否正在运行
- 检查浏览器控制台是否有CORS错误
- 确保API_BASE URL正确（默认：http://localhost:3000）

