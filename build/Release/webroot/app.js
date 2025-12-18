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

list()
