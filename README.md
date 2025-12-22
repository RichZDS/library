# 图书管理系统

基于2-3树（B树）实现的图书管理系统，使用C++后端（Oat++框架）和Web前端。

## 功能特性

### 1. 采编入库
- 新书购入，将书号、书名、著者、册数、出版时间等信息添加入图书账目
- 如果图书已存在，则自动增加总库存量
- 每新增一个书号，可以查看B树结构（凹入表形式）

### 2. 清除库存
- 实现某本书的全部信息删除操作
- 每清除一个书号，可以查看B树结构（凹入表形式）

### 3. 图书借阅
- 当书的库存量大于零时执行出借
- 登记借阅者的图书证号和姓名
- 系统自动记录当前借阅时间和计算归还时间（30天借期）

### 4. 图书预约
- 当某书库存为零时，记录预约者姓名和证号
- 系统自动记录当前预约时间和取书时间（7天保留期）

### 5. 图书归还
- 注销借阅者信息
- 自动更新该书的现存量

### 6. 作者专区
- 输入作者名字，系统将查找并显示该作者的全部著作

### 7. 图书信息查询
- 可以根据书号查阅图书基本信息、借阅信息和预约信息
- 可以查找全部图书基本信息
- 支持按编号搜索和按作者搜索

## 技术架构

### 后端
- **语言**: C++17
- **框架**: Oat++ (Web框架)
- **数据结构**: 2-3树（B树，最小度数t=2）
  - `BTreeBook`: 按书号索引的B树
  - `BTreeAuthor`: 按作者名索引的B树

### 前端
- HTML + CSS + JavaScript
- 响应式设计，支持模态框交互

### 项目结构
```
.
├── library_core/          # 核心数据结构
│   ├── library.h          # 图书、借阅记录、预约记录数据结构
│   ├── btree_book.h       # 按书号索引的B树头文件
│   ├── btree_book.cpp     # B树实现
│   ├── btree_author.h     # 按作者索引的B树头文件
│   └── btree_author.cpp   # 作者B树实现
├── web/                   # Web后端
│   ├── BookService.hpp    # 业务逻辑服务
│   ├── BookController.hpp # API控制器
│   ├── BookDto.hpp        # 数据传输对象
│   ├── AppComponent.hpp   # 应用组件配置
│   └── main_server.cpp    # 服务器入口
├── webroot/               # Web前端
│   ├── index.html         # 主页面
│   ├── app.js             # 前端逻辑
│   └── style.css          # 样式文件
└── CMakeLists.txt         # CMake构建配置
```

## 数据结构

### Book（图书信息）
- `id`: 书号（主键）
- `title`: 书名
- `author`: 作者
- `publisher`: 出版社
- `price`: 定价
- `publishDate`: 出版时间
- `totalCopies`: 总存量
- `availableCopies`: 现存量
- `borrowRecords`: 借阅记录列表
- `reservationRecords`: 预约记录列表

### BorrowRecord（借阅记录）
- `borrowerId`: 借阅者图书证号
- `borrowerName`: 借阅者姓名
- `borrowTime`: 借阅时间（Unix时间戳）
- `expectedReturnTime`: 预期归还时间（Unix时间戳）
- `actualReturnTime`: 实际归还时间（0表示未归还）

### ReservationRecord（预约记录）
- `reserverId`: 预约者图书证号
- `reserverName`: 预约者姓名
- `reserveTime`: 预约时间（Unix时间戳）
- `expectedPickupTime`: 预期取书时间（Unix时间戳）
- `actualPickupTime`: 实际取书时间（0表示未取书）

## 编译和运行

### 前置要求
- CMake 3.16+
- C++17 兼容的编译器（MSVC, GCC, Clang）
- Windows/Linux/macOS

### 编译步骤

1. 创建构建目录：
```bash
mkdir build
cd build
```

2. 运行CMake配置：
```bash
cmake ..
```

3. 编译项目：
```bash
cmake --build . --config Release
```

### 运行

1. 启动服务器：
```bash
./library_web  # Linux/macOS
library_web.exe  # Windows
```

2. 打开浏览器访问：
```
http://localhost:8000
```

## API接口

### 图书管理
- `GET /books` - 获取所有图书列表
- `GET /books/{id}` - 根据书号获取图书详细信息（包括借阅和预约记录）
- `POST /books` - 添加新书或增加库存
- `DELETE /books/{id}` - 删除图书

### 借阅管理
- `PUT /books/{id}/borrow` - 借阅图书
  - 请求体: `{ "borrowerId": "...", "borrowerName": "..." }`

### 归还管理
- `PUT /books/{id}/return` - 归还图书
  - 请求体: `{ "borrowerId": "..." }`

### 预约管理
- `PUT /books/{id}/reserve` - 预约图书（仅当库存为0时）
  - 请求体: `{ "reserverId": "...", "reserverName": "..." }`

### 查询功能
- `GET /books/author/{author}` - 根据作者名查询所有图书

### B树结构
- `GET /tree` - 获取B树结构（凹入表格式）

## 使用说明

1. **添加图书**: 在"新书入库"表单中填写图书信息，点击"提交"按钮
2. **查看图书**: 在图书列表中查看所有图书，或使用搜索功能
3. **借阅图书**: 点击"借阅"按钮，输入借阅者证号和姓名
4. **归还图书**: 点击"归还"按钮，输入借阅者证号
5. **预约图书**: 当图书库存为0时，点击"预约"按钮，输入预约者信息
6. **查看详情**: 点击"详情"按钮查看图书的完整信息，包括借阅和预约记录
7. **按作者查询**: 在搜索框输入作者名，点击"按作者搜索"
8. **查看B树结构**: 点击"查看B树结构"按钮查看当前B树的凹入表表示

## 注意事项

- 所有数据存储在内存中，服务器重启后数据会丢失
- 借阅期限为30天
- 预约保留期为7天
- B树的最小度数为2（2-3树）

## 开发说明

代码遵循以下规范：
- 使用 `using namespace std;` 简化代码
- 变量命名清晰，有明确含义
- 代码注释完善，便于阅读
- 遵循C++17标准

## 许可证

本项目仅供学习和研究使用。

