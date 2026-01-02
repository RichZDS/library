#include "btree_author.h"
#include <algorithm>

using namespace std;

// 构造函数：初始化作者索引B树（用于按作者查询图书）
// 这个B树以作者名为键，存储该作者的所有图书ID列表
BTreeAuthor::BTreeAuthor(int t): t(t), root(nullptr) {}

// 析构函数：释放整个B树的内存
BTreeAuthor::~BTreeAuthor() { destroy(root); }

// 递归销毁B树节点及其所有子节点
void BTreeAuthor::destroy(BTreeAuthorNode* x) {
    if (!x) return;
    for (auto c : x->children) destroy(c);  // 递归销毁所有子节点
    delete x;  // 销毁当前节点
}

// 在当前节点中查找键的位置
// 返回第一个大于等于key的键的索引
int BTreeAuthor::findKeyIndex(BTreeAuthorNode* x, const string& key) const {
    int i = 0;
    while (i < (int)x->keys.size() && key > x->keys[i])// 找到第一个大于等于key的键
     ++i;
    return i;
}

// 搜索包含指定作者的节点
// 参数：x是当前节点，author是要查找的作者名，idx返回键在节点中的索引
// 返回：找到的节点指针，如果未找到返回nullptr
BTreeAuthorNode* BTreeAuthor::searchNode(BTreeAuthorNode* x, const string& author, int& idx) const {
    if (!x) return nullptr;  // 空节点，未找到
    
    // 在当前节点中查找作者名
    idx = findKeyIndex(x, author);
    
    // 如果找到完全匹配的键，返回当前节点
    if (idx < (int)x->keys.size() && x->keys[idx] == author) return x;
    
    // 如果是叶子节点且没找到，说明不存在
    if (x->leaf) return nullptr;
    
    // 否则，递归搜索子节点
    return searchNode(x->children[idx], author, idx);
}

// B树分裂操作：与BTreeBook中的分裂操作类似
// 当子节点y已满时，将其分裂成两个节点
// 注意：这里每个键对应的是一个图书ID列表（vector<string>），而不是单个记录
void BTreeAuthor::splitChild(BTreeAuthorNode* x, int i) {
    BTreeAuthorNode* y = x->children[i];  // 已满的子节点
    BTreeAuthorNode* z = new BTreeAuthorNode(y->leaf);  // 新节点
    
    // 将y的后t-1个键和对应的ID列表移动到z
    for (int j = 0; j < t - 1; ++j) {
        z->keys.push_back(y->keys[j + t]);
        z->ids.push_back(y->ids[j + t]);
    }
    
    // 如果y不是叶子节点，移动子节点指针
    if (!y->leaf) {
        for (int j = 0; j < t; ++j) z->children.push_back(y->children[j + t]);
    }
    
    // 将y的中间键和ID列表提升到父节点x
    x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
    x->ids.insert(x->ids.begin() + i, y->ids[t - 1]);
    
    // 将新节点z插入到x的children数组中
    x->children.insert(x->children.begin() + i + 1, z);
    
    // 收缩y节点
    y->keys.resize(t - 1);
    y->ids.resize(t - 1);
    if (!y->leaf) y->children.resize(t);
}

// 添加作者-图书ID的关联
// 如果作者已存在，将图书ID添加到该作者的ID列表中
// 如果作者不存在，创建新的作者节点并插入
void BTreeAuthor::add(const string& author, const string& id) {
    // 情况1：空树，创建根节点
    if (!root) {
        root = new BTreeAuthorNode(true);
        root->keys.push_back(author);
        root->ids.push_back(vector<string>{id});  // 初始ID列表只包含这一个ID
        return;
    }
    
    // 情况2：查找作者是否已存在
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (node) {
        // 作者已存在，只需将图书ID添加到该作者的ID列表中
        auto& vec = node->ids[idx];
        // 检查ID是否已存在，避免重复添加
        if (find(vec.begin(), vec.end(), id) == vec.end()) {
            vec.push_back(id);
        }
        return;
    }
    
    // 情况3：作者不存在，需要插入新节点
    // 如果根节点已满，先分裂根节点
    if ((int)root->keys.size() == 2 * t - 1) {
        BTreeAuthorNode* s = new BTreeAuthorNode(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    
    // 在非满的树中插入新作者
    insertNonFull(root, author, id);
}

// 在非满节点x中插入新的作者-图书ID关联
// 与BTreeBook的insertNonFull类似，但这里处理的是作者名和ID列表
void BTreeAuthor::insertNonFull(BTreeAuthorNode* x, const string& author, const string& id) {
    int i = (int)x->keys.size() - 1;
    
    if (x->leaf) {
        // 叶子节点：直接插入，保持有序
        x->keys.push_back("");
        x->ids.push_back(vector<string>{});
        
        // 从右向左移动，为新键腾出位置
        while (i >= 0 && author < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->ids[i + 1] = x->ids[i];
            --i;
        }
        
        // 在正确位置插入新作者和对应的ID列表
        x->keys[i + 1] = author;
        x->ids[i + 1] = vector<string>{id};
    } else {
        // 内部节点：找到应该插入的子节点
        while (i >= 0 && author < x->keys[i]) --i;
        ++i;
        
        // 如果目标子节点已满，先分裂
        if ((int)x->children[i]->keys.size() == 2 * t - 1) {
            splitChild(x, i);
            if (author > x->keys[i]) ++i;
        }
        
        // 递归插入到子节点
        insertNonFull(x->children[i], author, id);
    }
}

// 删除作者-图书ID的关联
// 如果删除后该作者没有其他图书了，则从B树中删除该作者节点
void BTreeAuthor::removePair(const string& author, const string& id) {
    // 查找包含该作者的节点
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (!node) return;  // 作者不存在，直接返回
    
    // 从该作者的ID列表中删除指定的ID
    auto& vec = node->ids[idx];
    vec.erase(remove(vec.begin(), vec.end(), id), vec.end());
    
    // 如果该作者的所有图书ID都被删除了，需要从B树中删除该作者节点
    if (vec.empty()) {
        // 采用重建法：收集所有数据，删除目标作者，重建树
        vector<pair<string, vector<string>>> items;
        inorderAuthors(root, items);
        
        // 删除目标作者的所有关联
        items.erase(remove_if(items.begin(), items.end(), 
            [&](auto& p){ return p.first == author; }), items.end());
        
        // 销毁原树并重建
        destroy(root); 
        root = nullptr;
        for (auto& p : items) {
            for (auto& bid : p.second) {
                add(p.first, bid);
            }
        }
    }
}

// 中序遍历：收集所有作者及其图书ID列表
// 用于重建树或查询所有数据
void BTreeAuthor::inorderAuthors(BTreeAuthorNode* x, vector<pair<string, vector<string>>>& out) const {
    if (!x) return;
    
    // 遍历当前节点的所有键
    for (size_t i = 0; i < x->keys.size(); ++i) {
        // 如果不是叶子节点，先遍历左子树
        if (!x->leaf) inorderAuthors(x->children[i], out);
        
        // 访问当前键（作者名）和对应的ID列表
        out.push_back({x->keys[i], x->ids[i]});
    }
    
    // 如果不是叶子节点，最后遍历右子树
    if (!x->leaf) inorderAuthors(x->children.back(), out);
}

// 根据作者名获取该作者的所有图书ID列表
// 这是作者索引的核心查询功能
vector<string> BTreeAuthor::getIds(const string& author) const {
    int idx;
    BTreeAuthorNode* node = searchNode(root, author, idx);
    if (!node) return {};  // 作者不存在，返回空列表
    return node->ids[idx];  // 返回该作者的图书ID列表
}
