# 快速启动指南

## 三步启动Web界面

### 1. 编译C++ API程序

**方法A：使用编译脚本（推荐）**
```bash
compile_api.bat
```

**方法B：手动编译**
```bash
g++ -std=c++11 -I. Main_API.cpp datastruct/btree_book.cpp datastruct/btree_author.cpp -o Library_API.exe
```

### 2. 安装Node.js依赖
```bash
npm install
```

### 3. 启动服务器
```bash
npm start
```

然后在浏览器中打开：**http://localhost:3000**

## 功能测试

1. **添加图书**：在"添加图书"标签页填写信息并提交
2. **查询图书**：在"查询图书"标签页输入图书编号
3. **借阅/归还**：在"借阅/归还"标签页操作
4. **查看列表**：在"图书列表"标签页查看所有图书
5. **作者查询**：在"作者查询"标签页按作者名搜索

## 学习B树算法

打开以下文件查看详细的中文注释：
- `datastruct/btree_book.cpp` - 图书B树实现
- `datastruct/btree_author.cpp` - 作者索引B树实现

注释包括：
- B树的基本概念和原理
- 搜索、插入、分裂等关键操作的详细说明
- 每个步骤的算法逻辑

## 常见问题

**Q: 编译失败怎么办？**
A: 确保已安装C++编译器（g++或Visual Studio），检查所有源文件是否存在。

**Q: Node.js服务器无法启动？**
A: 确保已运行 `npm install`，检查端口3000是否被占用。

**Q: 前端无法连接后端？**
A: 确保后端服务器正在运行，检查浏览器控制台的错误信息。

更多详细信息请查看 `README_API.md`

