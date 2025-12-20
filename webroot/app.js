async function api(path, options){ const r = await fetch(path, options); return r }

function toast(msg, ok=true){
  const el = document.getElementById('msg'); el.textContent = msg; el.style.color = ok ? '#34d399' : '#f87171';
  setTimeout(()=>{ el.textContent='' }, 2200);
}

function resetForm(){ ['id','title','author','publisher','price','total'].forEach(id=>document.getElementById(id).value='') }

async function create(){
  const dto = {
    id: id.value.trim(), title: title.value.trim(), author: author.value.trim(),
    publisher: publisher.value.trim(), price: parseFloat(price.value||0),
    totalCopies: parseInt(total.value||0), availableCopies: parseInt(total.value||0)
  }
  const r = await api('/books', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify(dto) })
  toast(r.ok ? '创建成功' : '创建失败', r.ok); if(r.ok){ resetForm(); list() }
}

async function list(){
  const r = await api('/books'); const data = r.ok ? await r.json() : []
  const tbody = document.querySelector('#table tbody'); tbody.innerHTML = ''
  data.forEach(b=>{
    const tr = document.createElement('tr')
    tr.innerHTML = `
      <td>${b.id||''}</td>
      <td>${b.title||''}</td>
      <td>${b.author||''}</td>
      <td>${b.publisher||''}</td>
      <td>${b.price??0}</td>
      <td>${b.totalCopies??0}</td>
      <td>${b.availableCopies??0}</td>
      <td>
        <button class="btn ok" onclick="borrow('${b.id}')">借阅</button>
        <button class="btn" onclick="giveback('${b.id}')">归还</button>
        <button class="btn danger" onclick="removeBook('${b.id}')">删除</button>
      </td>`
    tbody.appendChild(tr)
  })
}

async function searchById(){
  const key = document.getElementById('search').value.trim(); if(!key){ list(); return }
  const r = await api('/books/'+encodeURIComponent(key))
  const tbody = document.querySelector('#table tbody'); tbody.innerHTML = ''
  if(!r.ok){ toast('未找到', false); return }
  const b = await r.json()
  const tr = document.createElement('tr')
  tr.innerHTML = `
    <td>${b.id||''}</td>
    <td>${b.title||''}</td>
    <td>${b.author||''}</td>
    <td>${b.publisher||''}</td>
    <td>${b.price??0}</td>
    <td>${b.totalCopies??0}</td>
    <td>${b.availableCopies??0}</td>
    <td>
      <button class="btn ok" onclick="borrow('${b.id}')">借阅</button>
      <button class="btn" onclick="giveback('${b.id}')">归还</button>
      <button class="btn danger" onclick="removeBook('${b.id}')">删除</button>
    </td>`
  tbody.appendChild(tr)
}

async function borrow(id){
  const r = await api(`/books/${encodeURIComponent(id)}/borrow`, { method:'PUT' })
  toast(r.ok ? '借阅成功' : '借阅失败', r.ok); if(r.ok) list()
}
async function giveback(id){
  const r = await api(`/books/${encodeURIComponent(id)}/return`, { method:'PUT' })
  toast(r.ok ? '归还成功' : '归还失败', r.ok); if(r.ok) list()
}
async function removeBook(id){
  const r = await api(`/books/${encodeURIComponent(id)}`, { method:'DELETE' })
  toast(r.ok ? '删除成功' : '删除失败', r.ok); if(r.ok) list()
}

async function viewTree(){
  const r = await api('/tree');
  if(r.ok) {
    const text = await r.text();
    displayTreeStructure(text);
    document.getElementById('treeSection').style.display = 'block';
    // 滚动到B树结构区域
    document.getElementById('treeSection').scrollIntoView({ behavior: 'smooth' });
  } else {
    toast('获取失败', false);
  }
}

function closeTree(){
  document.getElementById('treeSection').style.display = 'none';
}

function displayTreeStructure(structureText) {
  const container = document.getElementById('treeContainer');
  
  if (!structureText || structureText.trim() === '') {
    container.innerHTML = '<div class="tree-empty">B树为空</div>';
    return;
  }

  // 解析凹入表文本
  const lines = structureText.split('\n').filter(line => line.trim() !== '');
  
  if (lines.length === 0) {
    container.innerHTML = '<div class="tree-empty">B树为空</div>';
    return;
  }

  let html = '';

  for (const line of lines) {
    // 计算缩进层级（每4个空格为一级）
    const indentMatch = line.match(/^(\s*)/);
    const indent = indentMatch ? indentMatch[1] : '';
    const depth = Math.floor(indent.length / 4);
    
    // 提取节点内容
    const content = line.trim();
    
    // 判断节点类型
    let nodeClass = 'tree-node-internal';
    if (content.includes('(leaf)')) {
      nodeClass = 'tree-node-leaf';
    } else if (depth === 0) {
      nodeClass = 'tree-node-root';
    }

    // 提取键值（方括号内的内容）
    let displayContent = content;
    const keyMatch = content.match(/\[(.*?)\]/);
    
    if (keyMatch) {
      const keys = keyMatch[1].trim().split(/\s+/).filter(k => k);
      // 高亮显示键值
      displayContent = content.replace(/\[(.*?)\]/g, (match, keysStr) => {
        const keysArray = keysStr.trim().split(/\s+/).filter(k => k);
        const highlightedKeys = keysArray.map(k => 
          `<span class="tree-key">${escapeHtml(k)}</span>`
        ).join(' ');
        return `[ ${highlightedKeys} ]`;
      });
    }

    // 创建带缩进的HTML
    const indentHtml = '&nbsp;'.repeat(depth * 4);
    html += `<div class="tree-line ${nodeClass}" style="padding-left: ${depth * 20}px">${displayContent}</div>`;
  }

  container.innerHTML = html;
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

// 自动生成数据的计数器，限制最多点击10次
let autoFillCount = 0;
const MAX_AUTO_FILL = 10;
const BATCH_SIZE = 5;

// 预定义的测试数据池
const SAMPLE_BOOKS = [
  { t: "C++ Primer", a: "Stanley B. Lippman", p: "Addison-Wesley", price: 128.00 },
  { t: "Effective C++", a: "Scott Meyers", p: "Addison-Wesley", price: 68.00 },
  { t: "Thinking in Java", a: "Bruce Eckel", p: "Prentice Hall", price: 108.00 },
  { t: "Clean Code", a: "Robert C. Martin", p: "Prentice Hall", price: 58.00 },
  { t: "Design Patterns", a: "Erich Gamma", p: "Addison-Wesley", price: 88.00 },
  { t: "The Pragmatic Programmer", a: "Andrew Hunt", p: "Addison-Wesley", price: 78.00 },
  { t: "Introduction to Algorithms", a: "Thomas H. Cormen", p: "MIT Press", price: 158.00 },
  { t: "Refactoring", a: "Martin Fowler", p: "Addison-Wesley", price: 98.00 },
  { t: "Head First Design Patterns", a: "Eric Freeman", p: "O'Reilly", price: 68.00 },
  { t: "JavaScript: The Good Parts", a: "Douglas Crockford", p: "O'Reilly", price: 48.00 }
];

async function autoFill() {
  if (autoFillCount >= MAX_AUTO_FILL) {
    toast(`已达到最大生成限制 (${MAX_AUTO_FILL}次/50本)，请刷新页面重置`, false);
    return;
  }

  const btn = document.querySelector('button[onclick="autoFill()"]');
  const originalText = btn.textContent;
  btn.disabled = true;
  btn.textContent = "生成中...";

  try {
    const promises = [];
    const startIdx = autoFillCount * BATCH_SIZE;
    
    for (let i = 0; i < BATCH_SIZE; i++) {
      // 随机选择一本书作为模板，加上随机后缀避免ID冲突
      const template = SAMPLE_BOOKS[Math.floor(Math.random() * SAMPLE_BOOKS.length)];
      const id = `AUTO-${startIdx + i + 1}-${Math.floor(Math.random()*1000)}`;
      
      const dto = {
        id: id,
        title: `${template.t} #${startIdx + i + 1}`,
        author: template.a,
        publisher: template.p,
        price: template.price,
        totalCopies: 5,
        availableCopies: 5
      };
      
      promises.push(api('/books', { 
        method: 'POST', 
        headers: {'Content-Type': 'application/json'}, 
        body: JSON.stringify(dto) 
      }));
    }

    await Promise.all(promises);
    autoFillCount++;
    toast(`成功生成 ${BATCH_SIZE} 本书 (第 ${autoFillCount}/${MAX_AUTO_FILL} 批)`);
    list(); // 刷新列表
  } catch (e) {
    console.error(e);
    toast("生成失败", false);
  } finally {
    btn.disabled = false;
    btn.textContent = originalText;
    if (autoFillCount >= MAX_AUTO_FILL) {
      btn.textContent = "已达生成上限";
      btn.disabled = true;
    }
  }
}

list()
