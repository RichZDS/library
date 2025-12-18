const express = require('express');
const { exec } = require('child_process');
const path = require('path');
const cors = require('cors');

const app = express();
const PORT = 2778;

// 中间件
app.use(cors());
app.use(express.json());
app.use(express.static('public')); // 静态文件服务

// C++可执行文件路径（需要先编译Main_API.cpp）
const CPP_EXE = path.join(__dirname, 'Library_API.exe');

// 执行C++程序的辅助函数
function executeCpp(action, args = []) {
    return new Promise((resolve, reject) => {
        // 构建命令：action arg1 arg2 ...
        const cmd = `"${CPP_EXE}" ${action} ${args.map(arg => `"${arg}"`).join(' ')}`;
        
        exec(cmd, { encoding: 'utf8', maxBuffer: 10 * 1024 * 1024 }, (error, stdout, stderr) => {
            if (error) {
                console.error('执行错误:', error);
                reject({ success: false, message: '执行C++程序失败: ' + error.message });
                return;
            }
            
            if (stderr) {
                console.error('stderr:', stderr);
            }
            
            try {
                // 解析JSON输出
                const result = JSON.parse(stdout.trim());
                resolve(result);
            } catch (e) {
                console.error('JSON解析错误:', e);
                console.error('输出:', stdout);
                reject({ success: false, message: '解析C++程序输出失败', output: stdout });
            }
        });
    });
}

// API路由

// 添加图书
app.post('/api/books', async (req, res) => {
    try {
        const { id, title, author, publisher, price, totalCopies } = req.body;
        
        if (!id || !title || !author) {
            return res.json({ success: false, message: '缺少必要参数' });
        }
        
        const result = await executeCpp('add', [
            id,
            title || '',
            author || '',
            publisher || '',
            (price || 0).toString(),
            (totalCopies || 1).toString()
        ]);
        
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 删除图书
app.delete('/api/books/:id', async (req, res) => {
    try {
        const { id } = req.params;
        const result = await executeCpp('remove', [id]);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 借阅图书
app.post('/api/books/:id/borrow', async (req, res) => {
    try {
        const { id } = req.params;
        const result = await executeCpp('borrow', [id]);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 归还图书
app.post('/api/books/:id/return', async (req, res) => {
    try {
        const { id } = req.params;
        const result = await executeCpp('return', [id]);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 根据ID查询图书
app.get('/api/books/:id', async (req, res) => {
    try {
        const { id } = req.params;
        const result = await executeCpp('query', [id]);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 根据作者查询图书
app.get('/api/books/author/:author', async (req, res) => {
    try {
        const { author } = req.params;
        const result = await executeCpp('queryAuthor', [author]);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 列出所有图书
app.get('/api/books', async (req, res) => {
    try {
        const result = await executeCpp('list', []);
        res.json(result);
    } catch (error) {
        res.json(error);
    }
});

// 启动服务器
app.listen(PORT, () => {
    console.log(`服务器运行在 http://localhost:${PORT}`);
    console.log(`请确保已编译 Main_API.cpp 为 Library_API.exe`);
});

