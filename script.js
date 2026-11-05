// ---- Student + comparator (higher marks first; tie -> earlier entry first)
function Student(r,n,m,o){this.roll=r;this.name=n;this.marks=m;this.heapIndex=-1;this.order=o}
function better(a,b){return a.marks!==b.marks?a.marks>b.marks:a.order<b.order}

// ---- Hash table, separate chaining
function HashTable(){this.b=Array.from({length:11},()=>[]);this.count=0}
HashTable.prototype.hf=function(r){let h=0;for(let i=0;i<r.length;i++)h=(h*31+r.charCodeAt(i))%this.b.length;return h};
HashTable.prototype.search=function(r){return this.b[this.hf(r)].find(s=>s.roll===r)||null};
HashTable.prototype.resize=function(){const old=this.b;this.b=Array.from({length:old.length*2+1},()=>[]);old.forEach(c=>c.forEach(s=>this.b[this.hf(s.roll)].push(s)))};
HashTable.prototype.insert=function(s){if(this.search(s.roll))return false;if(this.count/this.b.length>0.75)this.resize();this.b[this.hf(s.roll)].push(s);this.count++;return true};
HashTable.prototype.remove=function(r){const c=this.b[this.hf(r)],i=c.findIndex(s=>s.roll===r);if(i<0)return false;c.splice(i,1);this.count--;return true};

// ---- Max heap with heapIndex tracking
function Heap(){this.a=[]}
Heap.prototype.sw=function(i,j){const a=this.a;[a[i],a[j]]=[a[j],a[i]];a[i].heapIndex=i;a[j].heapIndex=j};
Heap.prototype.up=function(i){while(i>0){const p=(i-1)>>1;if(better(this.a[i],this.a[p])){this.sw(i,p);i=p}else break}};
Heap.prototype.down=function(i){const n=this.a.length;for(;;){let m=i,l=2*i+1,r=2*i+2;if(l<n&&better(this.a[l],this.a[m]))m=l;if(r<n&&better(this.a[r],this.a[m]))m=r;if(m===i)break;this.sw(i,m);i=m}};
Heap.prototype.insert=function(s){s.heapIndex=this.a.length;this.a.push(s);this.up(s.heapIndex)};
Heap.prototype.removeAt=function(i){const last=this.a.length-1;if(i!==last)this.sw(i,last);this.a.pop();if(i<this.a.length){this.up(i);this.down(i)}};
Heap.prototype.fix=function(i){this.up(i);this.down(i)};
// Top K without disturbing the real heap: extract from a copy
Heap.prototype.topK=function(k){const c=new Heap();c.a=this.a.map(s=>({s,i:0}));
  // copy as lightweight proxies so heapIndex of real students is untouched
  const px=this.a.map(s=>({ref:s,marks:s.marks,order:s.order,heapIndex:0}));c.a=px;px.forEach((p,i)=>p.heapIndex=i);
  const out=[];while(c.a.length&&out.length<k){out.push(c.a[0].ref);c.removeAt(0)}return out};

let ht=new HashTable(),hp=new Heap(),seq=0,view=Infinity;
const $=id=>document.getElementById(id);
function say(t,ok){const m=$('msg');m.textContent=t;m.className=ok?'ok':'er'}
function esc(s){return String(s).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]))}

function addS(){
  const r=$('roll').value.trim(),n=$('name').value.trim(),mv=$('marks').value.trim();
  if(!r||!n||mv===''){return say('Please enter roll number, name and marks.')}
  const m=Number(mv);
  if(isNaN(m)||m<0||m>100)return say('Marks must be between 0 and 100.');
  const s=new Student(r,n,m,seq);
  if(!ht.insert(s))return say('Roll number '+r+' already exists.');
  seq++;hp.insert(s);say('Student added.',1);render();$('roll').value='';$('name').value='';$('marks').value='';$('roll').focus();
}
function updS(){
  const r=$('roll').value.trim(),mv=$('marks').value.trim();
  const s=ht.search(r);if(!s)return say('Student not found.');
  const m=Number(mv);if(mv===''||isNaN(m)||m<0||m>100)return say('Marks must be between 0 and 100.');
  s.marks=m;hp.fix(s.heapIndex);say('Marks updated.',1);render();
}
function delS(){
  const r=$('roll').value.trim();const s=ht.search(r);
  if(!s)return say('Student not found.');
  hp.removeAt(s.heapIndex);ht.remove(r);say('Student deleted.',1);render();
}
function srch(){
  const s=ht.search($('sroll').value.trim());
  $('sres').innerHTML=s?'<b>'+esc(s.roll)+'</b> │ '+esc(s.name)+' │ '+s.marks:'<span class="er">Student not found.</span>';
}
function show(k){view=k;render()}
function loadSample(){clearAll(1);[['101','Asha',88],['102','Ravi',92],['103','Meena',75],['104','Kabir',92],['105','Zoya',60],['106','Neha',81]].forEach(([r,n,m])=>{const s=new Student(r,n,m,seq++);ht.insert(s);hp.insert(s)});say('Sample data loaded (6 students from the report).',1);render()}
function clearAll(q){ht=new HashTable();hp=new Heap();seq=0;$('sres').innerHTML='';if(!q){say('All records cleared.',1);render()}}

function render(){
  const list=hp.topK(view===Infinity?hp.a.length:view);
  $('rt').textContent=view===Infinity?'Full ranking':'Top 10 students';
  let rk=0,prev=null,html='';
  list.forEach((s,i)=>{if(s.marks!==prev){rk=i+1;prev=s.marks}html+='<tr><td>'+rk+'</td><td>'+esc(s.roll)+'</td><td>'+esc(s.name)+'</td><td>'+s.marks+'</td></tr>'});
  $('rank').innerHTML=html||'<tr><td colspan="4" style="color:var(--mu)">No students yet. Add one or load the sample data.</td></tr>';
  $('heapv').innerHTML=hp.a.map((s,i)=>'<div class="cell"><small>['+i+']</small>'+esc(s.roll)+'<small>'+s.marks+'</small></div>').join('')||'<span style="color:var(--mu)">empty</span>';
  $('bn').textContent=ht.b.length;
  $('hashv').innerHTML=ht.b.map((c,i)=>'<div class="b"><span class="i">bucket '+i+'</span>'+(c.map(s=>'<div class="cell">'+esc(s.roll)+'</div>').join('')||'<span style="color:var(--mu)">–</span>')+'</div>').join('');
}
render();
