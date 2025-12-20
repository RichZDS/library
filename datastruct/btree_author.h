#include <string>
#include <vector>

struct BTreeAuthorNode {
    bool leaf;
    std::vector<std::string> keys;               // author name
    std::vector<std::vector<std::string>> ids;   // book id list per author
    std::vector<BTreeAuthorNode*> children;
    BTreeAuthorNode(bool leaf): leaf(leaf) {}
};

class BTreeAuthor {
public:
    explicit BTreeAuthor(int t = 2);
    ~BTreeAuthor();
    void add(const std::string& author, const std::string& id);
    void removePair(const std::string& author, const std::string& id);
    std::vector<std::string> getIds(const std::string& author) const;
private:
    int t;
    BTreeAuthorNode* root;
    void destroy(BTreeAuthorNode* x);
    int findKeyIndex(BTreeAuthorNode* x, const std::string& key) const;
    void splitChild(BTreeAuthorNode* x, int i);
    void insertNonFull(BTreeAuthorNode* x, const std::string& author, const std::string& id);
    BTreeAuthorNode* searchNode(BTreeAuthorNode* x, const std::string& author, int& idx) const;
    void inorderAuthors(BTreeAuthorNode* x, std::vector<std::pair<std::string, std::vector<std::string>>>& out) const;
};

