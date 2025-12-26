#include <string>
#include <vector>
using namespace std;
struct BTreeAuthorNode {
    bool leaf;
    vector<string> keys;               // author name
    vector<vector<string>> ids;   // book id list per author
    vector<BTreeAuthorNode*> children;
    BTreeAuthorNode(bool leaf): leaf(leaf) {}
};

class BTreeAuthor {
public:
    explicit BTreeAuthor(int t = 2);
    ~BTreeAuthor();
    void add(const string& author, const string& id);
    void removePair(const string& author, const string& id);
    vector<string> getIds(const string& author) const;
private:
    int t;
    BTreeAuthorNode* root;
    void destroy(BTreeAuthorNode* x);
    int findKeyIndex(BTreeAuthorNode* x, const string& key) const;
    void splitChild(BTreeAuthorNode* x, int i);
    void insertNonFull(BTreeAuthorNode* x, const string& author, const string& id);
    BTreeAuthorNode* searchNode(BTreeAuthorNode* x, const string& author, int& idx) const;
    void inorderAuthors(BTreeAuthorNode* x, vector<pair<string, vector<string>>>& out) const;
};

