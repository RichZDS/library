#include "btree_book.h"
#include <algorithm>

// 构造函数：初始化B树，t是最小度数（对于2-3树，t=2）
// 2-3树意味着每个节点最多有2个键（2*t-1=3个键），最少有1个键（t-1=1个键）
BTreeBook::BTreeBook(int t): t(t), root(nullptr) {}

// 析构函数：释放整个B树的内存
BTreeBook::~BTreeBook() { destroy(root); }

// 递归销毁B树节点及其所有子节点
// 采用后序遍历：先销毁所有子节点，再销毁当前节点
void BTreeBook::destroy(BTreeBookNode* x) {
    if (!x) return;
    for (auto c : x->children) destroy(c);  // 递归销毁所有子节点
    delete x;  // 销毁当前节点
}

// 公开的搜索接口：从根节点开始搜索
Book* BTreeBook::search(const std::string& id) const {
    return searchNode(root, id);
}

// B树搜索算法（递归实现）
// 核心思想：在B树中，每个节点内部是有序的，可以快速定位到下一个要搜索的子节点
Book* BTreeBook::searchNode(BTreeBookNode* x, const std::string& id) const {
    if (!x) return nullptr;  // 空树或到达空节点，未找到
    
    // 在当前节点中查找：找到第一个大于等于id的键的位置
    // 由于节点内的键是有序的，可以用线性搜索（节点内键的数量通常不多）
    int i = 0;
    while (i < (int)x->keys.size() && id > x->keys[i]) ++i;
    
    // 如果找到了完全匹配的键，返回对应的记录
    if (i < (int)x->keys.size() && id == x->keys[i]) {
        return const_cast<Book*>(&x->records[i]);
    }
    
    // 如果是叶子节点且没找到，说明不存在
    if (x->leaf) return nullptr;
    
    // 否则，递归搜索第i个子节点（因为id应该在keys[i-1]和keys[i]之间）
    // 注意：children[i]对应的是keys[i]之前的子树
    return searchNode(x->children[i], id);
}

// B树分裂操作：这是B树保持平衡的关键操作
// 当某个子节点y已满（有2*t-1个键）时，需要将其分裂成两个节点
// 参数：x是父节点，i是子节点y在x的children数组中的索引
void BTreeBook::splitChild(BTreeBookNode* x, int i) {
    BTreeBookNode* y = x->children[i];  // y是已满的子节点（有2*t-1个键）
    BTreeBookNode* z = new BTreeBookNode(y->leaf);  // 创建新节点z来存放y的后半部分
    
    // 将y的后t-1个键和记录移动到新节点z
    // y的键索引范围：[0, 1, ..., t-2, t-1, t, ..., 2*t-2]
    // 中间键是y->keys[t-1]，它会被提升到父节点x
    // z将获得：[t, t+1, ..., 2*t-2] 共t-1个键
    for (int j = 0; j < t - 1; ++j) {
        z->keys.push_back(y->keys[j + t]);
        z->records.push_back(y->records[j + t]);
    }
    
    // 如果y不是叶子节点，还需要移动子节点指针
    // y有2*t个子节点，z将获得后t个子节点
    if (!y->leaf) {
        for (int j = 0; j < t; ++j) {
            z->children.push_back(y->children[j + t]);
        }
    }
    
    // 将y的中间键（第t-1个，索引从0开始）提升到父节点x的位置i
    // 这个键是y和z之间的分界点
    x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
    x->records.insert(x->records.begin() + i, y->records[t - 1]);
    
    // 将新节点z插入到x的children数组中，位置在i+1（在提升的键之后）
    x->children.insert(x->children.begin() + i + 1, z);
    
    // 收缩y节点：只保留前t-1个键和对应的t个子节点
    // 分裂后：y有t-1个键，z有t-1个键，父节点x增加1个键
    y->keys.resize(t - 1);
    y->records.resize(t - 1);
    if (!y->leaf) y->children.resize(t);  // y保留前t个子节点
}

// B树插入操作的入口函数
// 策略：如果根节点已满，先分裂根节点（这会增加树的高度），然后再插入
void BTreeBook::insert(const Book& book) {
    // 情况1：空树，直接创建根节点
    if (!root) {
        root = new BTreeBookNode(true);  // 创建叶子节点
        root->keys.push_back(book.id);
        root->records.push_back(book);
        return;
    }
    
    // 情况2：根节点已满（有2*t-1个键），需要分裂
    // 这是B树唯一会增加高度的操作
    if ((int)root->keys.size() == 2 * t - 1) {
        BTreeBookNode* s = new BTreeBookNode(false);  // 创建新的根节点（非叶子）
        s->children.push_back(root);  // 将原根节点作为新根的第一个子节点
        splitChild(s, 0);  // 分裂原根节点
        root = s;  // 更新根节点指针
    }
    
    // 现在根节点肯定不满，可以安全插入
    insertNonFull(root, book);
}

// 在非满节点x中插入新记录
// 这是B树插入的核心递归函数
void BTreeBook::insertNonFull(BTreeBookNode* x, const Book& book) {
    int i = (int)x->keys.size() - 1;  // 从最后一个键开始查找插入位置
    
    if (x->leaf) {
        // 情况1：x是叶子节点
        // 在叶子节点中，需要找到正确的插入位置并保持有序
        
        // 先扩展数组，为新键腾出空间
        x->keys.push_back(""); 
        x->records.push_back(Book{});
        
        // 从右向左移动所有大于book.id的键，为新键腾出位置
        // 这是一个插入排序的过程
        while (i >= 0 && book.id < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->records[i + 1] = x->records[i];
            --i;
        }
        
        // 在正确的位置插入新键和记录
        x->keys[i + 1] = book.id;
        x->records[i + 1] = book;
    } else {
        // 情况2：x是内部节点（非叶子节点）
        // 需要找到应该插入到哪个子节点中
        
        // 找到第一个大于等于book.id的键的位置
        while (i >= 0 && book.id < x->keys[i]) --i;
        ++i;  // i现在指向应该插入的子节点索引
        
        // 关键步骤：如果目标子节点已满，先分裂它
        // 这样可以保证在递归插入时，子节点一定不满
        if ((int)x->children[i]->keys.size() == 2 * t - 1) {
            splitChild(x, i);  // 分裂子节点
            
            // 分裂后，x在位置i增加了一个新键
            // 需要判断book.id应该插入到哪个子节点：
            // - 如果book.id > x->keys[i]，应该插入到新创建的右子节点（索引i+1）
            // - 否则，插入到原来的左子节点（索引i）
            if (book.id > x->keys[i]) ++i;
        }
        
        // 递归插入到子节点中
        insertNonFull(x->children[i], book);
    }
}

// 中序遍历接口：按书号顺序收集所有图书
void BTreeBook::inorder(std::vector<Book>& out) const {
    inorderNode(root, out);
}

// 中序遍历B树节点（递归实现）
// B树的中序遍历：对于每个节点，先遍历左子树，再访问当前键，最后遍历右子树
// 在B树中：children[i]对应keys[i]之前的子树，children[i+1]对应keys[i]之后的子树
void BTreeBook::inorderNode(BTreeBookNode* x, std::vector<Book>& out) const {
    if (!x) return;
    
    // 遍历当前节点的所有键
    for (size_t i = 0; i < x->keys.size(); ++i) {
        // 如果不是叶子节点，先遍历第i个子树（在keys[i]之前）
        if (!x->leaf) inorderNode(x->children[i], out);
        
        // 访问当前键对应的记录
        out.push_back(x->records[i]);
    }
    
    // 如果不是叶子节点，最后遍历最后一个子树（在所有键之后）
    if (!x->leaf) inorderNode(x->children.back(), out);
}

// B树删除操作（简化实现：重建法）
// 注意：这是一个简化的实现，真正的B树删除算法更复杂，需要处理多种情况
// 这里采用重建法：先中序遍历收集所有数据，删除目标项，然后重建整个树
// 优点：实现简单；缺点：效率较低，但适合学习目的
bool BTreeBook::remove(const std::string& id) {
    // 步骤1：中序遍历收集所有图书数据
    std::vector<Book> items;
    inorder(items);
    
    // 步骤2：从数据中删除目标图书
    bool found = false;
    for (auto it = items.begin(); it != items.end(); ) {
        if (it->id == id) { 
            it = items.erase(it);  // 删除找到的项
            found = true; 
        } else {
            ++it;
        }
    }
    
    // 步骤3：销毁原树
    destroy(root); 
    root = nullptr;
    
    // 步骤4：重建B树（重新插入所有剩余的数据）
    for (const auto& b : items) insert(b);
    
    return found;
}

