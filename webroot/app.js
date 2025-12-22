async function api(path, options){ const r = await fetch(path, options); return r }

function toast(msg, ok=true){
  const el = document.getElementById('msg'); el.textContent = msg; el.style.color = ok ? '#34d399' : '#f87171';
  setTimeout(()=>{ el.textContent='' }, 2200);
}

function resetForm(){ ['id','title','author','publisher','price','publishDate','total'].forEach(id=>document.getElementById(id).value='') }

async function create(){
  const dto = {
    id: id.value.trim(), title: title.value.trim(), author: author.value.trim(),
    publisher: publisher.value.trim(), price: parseFloat(price.value||0),
    publishDate: publishDate.value.trim() || new Date().toISOString().split('T')[0],
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
      <td>${b.publishDate||''}</td>
      <td>${b.totalCopies??0}</td>
      <td>${b.availableCopies??0}</td>
      <td>
        <button class="btn ok" onclick="showBorrowDialog('${b.id}')">借阅</button>
        <button class="btn" onclick="showReturnDialog('${b.id}')">归还</button>
        <button class="btn warning" onclick="showReserveDialog('${b.id}')">预约</button>
        <button class="btn info" onclick="showBookDetail('${b.id}')">详情</button>
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
    <td>${b.publishDate||''}</td>
    <td>${b.totalCopies??0}</td>
    <td>${b.availableCopies??0}</td>
    <td>
      <button class="btn ok" onclick="showBorrowDialog('${b.id}')">借阅</button>
      <button class="btn" onclick="showReturnDialog('${b.id}')">归还</button>
      <button class="btn warning" onclick="showReserveDialog('${b.id}')">预约</button>
      <button class="btn info" onclick="showBookDetail('${b.id}')">详情</button>
      <button class="btn danger" onclick="removeBook('${b.id}')">删除</button>
    </td>`
  tbody.appendChild(tr)
}

async function searchByAuthor(){
  const author = document.getElementById('searchAuthor').value.trim(); 
  if(!author){ toast('请输入作者姓名', false); return }
  const r = await api('/books/author/'+encodeURIComponent(author))
  const tbody = document.querySelector('#table tbody'); tbody.innerHTML = ''
  if(!r.ok){ toast('查询失败', false); return }
  const data = await r.json()
  if(data.length === 0){ toast('未找到该作者的图书', false); return }
  data.forEach(b=>{
    const tr = document.createElement('tr')
    tr.innerHTML = `
      <td>${b.id||''}</td>
      <td>${b.title||''}</td>
      <td>${b.author||''}</td>
      <td>${b.publisher||''}</td>
      <td>${b.price??0}</td>
      <td>${b.publishDate||''}</td>
      <td>${b.totalCopies??0}</td>
      <td>${b.availableCopies??0}</td>
      <td>
        <button class="btn ok" onclick="showBorrowDialog('${b.id}')">借阅</button>
        <button class="btn" onclick="showReturnDialog('${b.id}')">归还</button>
        <button class="btn warning" onclick="showReserveDialog('${b.id}')">预约</button>
        <button class="btn info" onclick="showBookDetail('${b.id}')">详情</button>
        <button class="btn danger" onclick="removeBook('${b.id}')">删除</button>
      </td>`
    tbody.appendChild(tr)
  })
  toast(`找到 ${data.length} 本图书`)
}

// 显示借阅对话框
function showBorrowDialog(bookId){
  document.getElementById('actionModalTitle').textContent = '借阅图书'
  document.getElementById('actionModalContent').innerHTML = `
    <p>图书编号：${bookId}</p>
    <div style="margin: 10px 0;">
      <label>借阅者证号：<input id="borrowerId" style="width: 200px;" /></label>
    </div>
    <div style="margin: 10px 0;">
      <label>借阅者姓名：<input id="borrowerName" style="width: 200px;" /></label>
    </div>
    <div style="margin-top: 20px;">
      <button class="btn primary" onclick="doBorrow('${bookId}')">确认借阅</button>
      <button class="btn" onclick="closeActionModal()">取消</button>
    </div>
  `
  document.getElementById('actionModal').style.display = 'block'
}

// 执行借阅
async function doBorrow(bookId){
  const borrowerId = document.getElementById('borrowerId').value.trim()
  const borrowerName = document.getElementById('borrowerName').value.trim()
  if(!borrowerId || !borrowerName){ toast('请输入借阅者信息', false); return }
  const r = await api(`/books/${encodeURIComponent(bookId)}/borrow`, {
    method:'PUT',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({borrowerId, borrowerName})
  })
  toast(r.ok ? '借阅成功' : '借阅失败：'+(await r.text()), r.ok)
  if(r.ok){ closeActionModal(); list() }
}

// 显示归还对话框
function showReturnDialog(bookId){
  document.getElementById('actionModalTitle').textContent = '归还图书'
  document.getElementById('actionModalContent').innerHTML = `
    <p>图书编号：${bookId}</p>
    <div style="margin: 10px 0;">
      <label>借阅者证号：<input id="returnBorrowerId" style="width: 200px;" /></label>
    </div>
    <div style="margin-top: 20px;">
      <button class="btn primary" onclick="doReturn('${bookId}')">确认归还</button>
      <button class="btn" onclick="closeActionModal()">取消</button>
    </div>
  `
  document.getElementById('actionModal').style.display = 'block'
}

// 执行归还
async function doReturn(bookId){
  const borrowerId = document.getElementById('returnBorrowerId').value.trim()
  if(!borrowerId){ toast('请输入借阅者证号', false); return }
  const r = await api(`/books/${encodeURIComponent(bookId)}/return`, {
    method:'PUT',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({borrowerId})
  })
  toast(r.ok ? '归还成功' : '归还失败：'+(await r.text()), r.ok)
  if(r.ok){ closeActionModal(); list() }
}

// 显示预约对话框
function showReserveDialog(bookId){
  document.getElementById('actionModalTitle').textContent = '预约图书'
  document.getElementById('actionModalContent').innerHTML = `
    <p>图书编号：${bookId}</p>
    <div style="margin: 10px 0;">
      <label>预约者证号：<input id="reserverId" style="width: 200px;" /></label>
    </div>
    <div style="margin: 10px 0;">
      <label>预约者姓名：<input id="reserverName" style="width: 200px;" /></label>
    </div>
    <div style="margin-top: 20px;">
      <button class="btn primary" onclick="doReserve('${bookId}')">确认预约</button>
      <button class="btn" onclick="closeActionModal()">取消</button>
    </div>
  `
  document.getElementById('actionModal').style.display = 'block'
}

// 执行预约
async function doReserve(bookId){
  const reserverId = document.getElementById('reserverId').value.trim()
  const reserverName = document.getElementById('reserverName').value.trim()
  if(!reserverId || !reserverName){ toast('请输入预约者信息', false); return }
  const r = await api(`/books/${encodeURIComponent(bookId)}/reserve`, {
    method:'PUT',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({reserverId, reserverName})
  })
  toast(r.ok ? '预约成功' : '预约失败：'+(await r.text()), r.ok)
  if(r.ok){ closeActionModal(); list() }
}

// 显示图书详情
async function showBookDetail(bookId){
  const r = await api('/books/'+encodeURIComponent(bookId))
  if(!r.ok){ toast('获取详情失败', false); return }
  const b = await r.json()
  
  let html = `
    <h3>${b.title||''}</h3>
    <table style="width:100%; margin-top: 10px;">
      <tr><th style="text-align:left; width:100px;">编号：</th><td>${b.id||''}</td></tr>
      <tr><th style="text-align:left;">作者：</th><td>${b.author||''}</td></tr>
      <tr><th style="text-align:left;">出版社：</th><td>${b.publisher||''}</td></tr>
      <tr><th style="text-align:left;">定价：</th><td>${b.price??0} 元</td></tr>
      <tr><th style="text-align:left;">出版时间：</th><td>${b.publishDate||''}</td></tr>
      <tr><th style="text-align:left;">总存量：</th><td>${b.totalCopies??0}</td></tr>
      <tr><th style="text-align:left;">现存量：</th><td>${b.availableCopies??0}</td></tr>
    </table>
  `
  
  // 借阅记录
  if(b.borrowRecords && b.borrowRecords.length > 0){
    html += '<h4 style="margin-top: 20px;">借阅记录：</h4><table style="width:100%; border-collapse: collapse;"><thead><tr><th>借阅者证号</th><th>借阅者姓名</th><th>借阅时间</th><th>预期归还时间</th><th>实际归还时间</th><th>状态</th></tr></thead><tbody>'
    b.borrowRecords.forEach(record=>{
      const borrowTime = new Date(record.borrowTime * 1000).toLocaleString('zh-CN')
      const expectedReturnTime = new Date(record.expectedReturnTime * 1000).toLocaleString('zh-CN')
      const actualReturnTime = record.actualReturnTime ? new Date(record.actualReturnTime * 1000).toLocaleString('zh-CN') : '未归还'
      const status = record.actualReturnTime ? '已归还' : '借阅中'
      html += `<tr><td>${record.borrowerId}</td><td>${record.borrowerName}</td><td>${borrowTime}</td><td>${expectedReturnTime}</td><td>${actualReturnTime}</td><td>${status}</td></tr>`
    })
    html += '</tbody></table>'
  } else {
    html += '<p style="margin-top: 20px;">暂无借阅记录</p>'
  }
  
  // 预约记录
  if(b.reservationRecords && b.reservationRecords.length > 0){
    html += '<h4 style="margin-top: 20px;">预约记录：</h4><table style="width:100%; border-collapse: collapse;"><thead><tr><th>预约者证号</th><th>预约者姓名</th><th>预约时间</th><th>预期取书时间</th><th>实际取书时间</th><th>状态</th></tr></thead><tbody>'
    b.reservationRecords.forEach(record=>{
      const reserveTime = new Date(record.reserveTime * 1000).toLocaleString('zh-CN')
      const expectedPickupTime = new Date(record.expectedPickupTime * 1000).toLocaleString('zh-CN')
      const actualPickupTime = record.actualPickupTime ? new Date(record.actualPickupTime * 1000).toLocaleString('zh-CN') : '未取书'
      const status = record.actualPickupTime ? '已取书' : '预约中'
      html += `<tr><td>${record.reserverId}</td><td>${record.reserverName}</td><td>${reserveTime}</td><td>${expectedPickupTime}</td><td>${actualPickupTime}</td><td>${status}</td></tr>`
    })
    html += '</tbody></table>'
  } else {
    html += '<p style="margin-top: 20px;">暂无预约记录</p>'
  }
  
  document.getElementById('bookDetailContent').innerHTML = html
  document.getElementById('bookDetailModal').style.display = 'block'
}

// 关闭模态框
function closeActionModal(){
  document.getElementById('actionModal').style.display = 'none'
}

function closeBookDetail(){
  document.getElementById('bookDetailModal').style.display = 'none'
}

// 点击模态框外部关闭
window.onclick = function(event) {
  const actionModal = document.getElementById('actionModal')
  const bookDetailModal = document.getElementById('bookDetailModal')
  if (event.target == actionModal) {
    actionModal.style.display = 'none'
  }
  if (event.target == bookDetailModal) {
    bookDetailModal.style.display = 'none'
  }
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
        publishDate: new Date(2020 + Math.floor(Math.random() * 5), Math.floor(Math.random() * 12), Math.floor(Math.random() * 28) + 1).toISOString().split('T')[0],
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
