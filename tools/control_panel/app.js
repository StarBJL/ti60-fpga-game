"use strict";
const $=id=>document.getElementById(id);
const pendingCommands=new Map();let statusSynced=false;
let pressureCapability=-1;
let coopStartBusy=false,lastInputDiagnostic=0;
let presetCatalog=[],presetCatalogError="",presetRequestPending=false,presetRequestedOperation=null;
let visiblePresetGroup="萝卜弹幕";
let token="",cursor=0,connected=false,compatible=false,dualvideo=false,currentScene=-1,snapshot=null,lastAck=0,queue=Promise.resolve(),polling=false,held=0,lastHeld=-1,lastError="";
const tracks={bgm_title:"像素晨光",bgm_menu2:"星灯菜单",bgm_stage1:"云端疾行",bgm_stage2:"月下航线",bgm_boss1:"赤色风暴",bgm_boss2:"终幕回响",bgm_garden:"萝卜田野",bgm_garden2:"森林夜行",bgm_stress:"压力脉冲",bgm_stress2:"数字方阵","?":"未播放"};
const sceneNames=["东方","萝卜","方块","图片"];
const boolFields={alpha:"alpha",vsync:"vsync",hardware:"hw",autoplay:"auto",table:"table",bgm:"bgm",tone:"tone",verbose:"verbose",stats:"stats"};
function notice(text,kind=""){const e=$("notice");e.textContent=text;e.className="notice "+kind;}
async function api(path,body){
  const opts=body===undefined?{}:{method:"POST",headers:{"Content-Type":"application/json","X-Ti60-Token":token},body:JSON.stringify(body)};
  const response=await fetch(path,opts),data=await response.json();if(!response.ok)throw new Error(data.error||"请求失败");return data;
}
function cmd(command,value){
  queue=queue.catch(()=>{}).then(async()=>{
    if(!connected||!compatible)throw new Error("请连接设备并等待识别 R4 / R5 控制协议");
    const data=await api("/api/command",{command,...(value===undefined?{}:{value})});
    pendingCommands.set(data.id,command);if(pendingCommands.size>200)pendingCommands.delete(pendingCommands.keys().next().value);
    if(command!=="move"&&command!=="stop")notice(`指令 ${data.id} 已发送，等待设备回执。`);
    return data;
  });queue.catch(e=>notice(e.message,"error"));return queue;
}
function setHeld(mask){held=mask;sendHeld();document.querySelectorAll("[data-held]").forEach(e=>e.classList.toggle("held",!!(held&Number(e.dataset.held))));}
const padClient="keys_"+Math.random().toString(36).slice(2);let padSequence=0,padSending=false;
async function sendHeld(force=false){
  if(!connected||!compatible||padSending||(!force&&!held&&lastHeld===held))return;
  const mask=held;padSending=true;
  try{
    if(snapshot?.pad4)await api("/api/pad",{mask,seq:++padSequence,client:padClient});
    else await cmd("move",mask);
    lastHeld=mask;
  }catch(e){notice(e.message,"error");lastHeld=-1;}
  finally{padSending=false;if(connected&&compatible&&lastHeld>=0&&held!==lastHeld)sendHeld();}
}
function release(){keys.clear();pointerBits.clear();padHeldBits=0;held=0;lastHeld=-1;setHeld(0);}
const keys=new Map(),pointerBits=new Map();
let padHeldBits=0;   /* 手柄（Gamepad API）按住的键，和键盘 / 屏幕按钮合起来 */
function updateHeld(){let value=padHeldBits;for(const v of keys.values())value|=v;for(const v of pointerBits.values())value|=v;setHeld(value);}
const keyBits={ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,w:1,W:1,s:2,S:2,a:4,A:4,d:8,D:8,z:16,Z:16," ":16,x:32,X:32,j:32,J:32,Shift:64,Escape:128,Enter:256};
/* 可打印键 -> 串口字符。中文输入法下标点会变成全角（' -> ’）或 key = "Process"：按物理键位（e.code）换回 ASCII（2026-10-28） */
const codeAscii={Quote:["'",'"'],Semicolon:[";",":"],Comma:[",","<"],Period:[".",">"],Slash:["/","?"],Backquote:["`","~"],BracketLeft:["[","{"],BracketRight:["]","}"],Backslash:["\\","|"],Minus:["-","_"],Equal:["=","+"]};
function asciiKey(e){
  let k=e.key;
  if(!(k.length===1&&k>=" "&&k<="~")){const m=codeAscii[e.code];k=m?m[e.shiftKey?1:0]:/^Key[A-Z]$/.test(e.code)?(e.shiftKey?e.code[3]:e.code[3].toLowerCase()):/^Digit[0-9]$/.test(e.code)?e.code[5]:"";}
  return k&&k!==":"?k:"";
}
const physicalBits={ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyW:1,KeyS:2,KeyA:4,KeyD:8,KeyZ:16,Space:16,KeyX:32,KeyJ:32,ShiftLeft:64,ShiftRight:64,Escape:128,Enter:256,NumpadEnter:256,ControlLeft:512,ControlRight:512};
function typing(){return /^(INPUT|SELECT|TEXTAREA)$/.test(document.activeElement?.tagName||"")&&document.activeElement?.id!=="captureKeys";}
window.addEventListener("keydown",e=>{
  if(!$("captureKeys").checked||typing()||!connected||!compatible||snapshot?.state.input_diag||coopStartBusy)return;
  if(physicalBits[e.code]!==undefined){e.preventDefault();keys.set(e.code,physicalBits[e.code]);updateHeld();}
  else if(!e.repeat&&(e.key==="+"||e.key==="="||e.key==="-")){e.preventDefault();cmd("delta",e.key==="-"?-1:1);}
  else if(!e.repeat&&["1","2","3"].includes(e.key)){e.preventDefault();cmd("pick",Number(e.key));}
  else if(!e.repeat&&(e.key==="Control"||e.key===";"||e.key===">")){e.preventDefault();if(snapshot?.state.scene===0)cmd("key",59);}   /* 东方跳过：; 不用 Shift（同串口 ; / pc_pad Ctrl，2026-10-28） */
  else if(!e.repeat&&e.code==="KeyM"){e.preventDefault();cmd("menu");}
  else if(!e.repeat&&e.code==="F1"){e.preventDefault();cmd("status");}
  else if(!e.repeat&&!e.ctrlKey&&!e.altKey&&!e.metaKey&&asciiKey(e)){e.preventDefault();cmd("key",asciiKey(e).charCodeAt(0));}   /* 其余可打印键 = 串口按键（n m g v e p r l ` ...，2026-10-28） */
  else if(!e.repeat&&["F6","F7","F8"].includes(e.key)){e.preventDefault();cmd("step",{F6:100,F7:10,F8:1}[e.key]);}
  else if(!e.repeat&&(e.key==="["||e.key==="]")){e.preventDefault();const s=$("bulletSize"),i=(s.selectedIndex+s.options.length+(e.key==="]"?1:-1))%s.options.length;cmd("bullet",Number(s.options[i].value));}
  else if(!e.repeat&&(e.key==="{"||e.key==="}")){e.preventDefault();const s=$("squareSize"),i=(s.selectedIndex+s.options.length+(e.key==="}"?1:-1))%s.options.length;cmd("square",Number(s.options[i].value));}

});
window.addEventListener("keyup",e=>{if(keys.has(e.code)){keys.delete(e.code);updateHeld();}});
document.addEventListener("focusin",()=>{if(typing())release();});
window.addEventListener("blur",release);document.addEventListener("visibilitychange",()=>{if(document.hidden)release();});
window.addEventListener("beforeunload",()=>{if(connected&&compatible)fetch(snapshot?.pad4?"/api/pad":"/api/command",{method:"POST",headers:{"Content-Type":"application/json","X-Ti60-Token":token},body:JSON.stringify(snapshot?.pad4?{mask:0,seq:++padSequence,client:padClient}:{command:"move",value:0}),keepalive:true}).catch(()=>{});});
$("captureKeys").addEventListener("change",release);
document.querySelectorAll("[data-held]").forEach(button=>{
  button.addEventListener("pointerdown",e=>{if(!connected||!compatible)return;e.preventDefault();button.setPointerCapture(e.pointerId);pointerBits.set(e.pointerId,Number(button.dataset.held));updateHeld();});
  for(const type of ["pointerup","pointercancel","lostpointercapture"])button.addEventListener(type,e=>{pointerBits.delete(e.pointerId);updateHeld();});
});
document.querySelectorAll("[data-command]").forEach(button=>button.addEventListener("click",()=>{
  if(button.dataset.command==="coop"){startCoopGame(Number(button.dataset.value));return;}
  if(coopStartBusy&&button.dataset.command!=="stop")return;
  if(button.dataset.command==="stop")release();
  cmd(button.dataset.command,button.dataset.value===undefined?undefined:Number(button.dataset.value));
}));
document.querySelectorAll("[data-scene]").forEach(button=>button.addEventListener("click",()=>{release();cmd("scene",Number(button.dataset.scene));}));
document.querySelectorAll("[data-step]").forEach(button=>button.addEventListener("click",()=>cmd("step",Number(button.dataset.step))));
document.querySelectorAll("[data-setting]").forEach(input=>input.addEventListener("change",()=>cmd(input.dataset.setting,input.checked?1:0)));
document.querySelectorAll("[data-resolution]").forEach(button=>button.addEventListener("click",()=>cmd("resolution",Number(button.dataset.resolution))));
$("bulletSize").addEventListener("change",e=>cmd("bullet",Number(e.target.value)));
$("imagePeriod").addEventListener("change",e=>cmd("period",Number(e.target.value)));
$("squareSize").addEventListener("change",e=>cmd("square",Number(e.target.value)));
$("decrease").addEventListener("click",()=>cmd("delta",-1));$("increase").addEventListener("click",()=>cmd("delta",1));
$("setCount").addEventListener("click",()=>{const n=Number($("targetCount").value);if(!Number.isInteger(n))return notice("数量必须为整数","error");cmd("count",n);});
$("stressStart").addEventListener("click",()=>cmd("stress",currentScene===1?Number($("carrotTier").value):currentScene===0?Number($("thStage").value):0));
$("volume").addEventListener("input",e=>$("volumeText").textContent=Math.round(e.target.value/256*100)+"%");
$("volume").addEventListener("change",e=>cmd("volume",Number(e.target.value)));
let availablePorts=[],portsRefreshGeneration=0,portInputEpoch=0;
function portHint(){
  const p=availablePorts.find(p=>p.device===$("port").value.trim());
  $("portInfo").textContent=p ? `${p.label||p.description}${p.last_verified?" · 上次已验证":""}${p.location?" · USB "+p.location:""}${p.instance_id?" · "+p.instance_id:""}` : "可手动填写串口路径；设备编号会随 USB 插拔变化。";
}
async function refreshPorts(){
  const generation=++portsRefreshGeneration,epoch=portInputEpoch,initialValue=$("port").value;
  try{
    const ports=await api("/api/ports");if(generation!==portsRefreshGeneration)return;availablePorts=ports;
    $("ports").replaceChildren(...ports.map(port=>{const e=document.createElement("option");e.value=port.device;e.label=port.label||port.description;return e;}));
    const candidates=ports.filter(p=>p.recommended),remembered=ports.filter(p=>p.last_verified);
    if(!initialValue&&epoch===portInputEpoch&&!$("port").value&&!connected){const pick=remembered.length===1?remembered:candidates;if(pick.length===1)$("port").value=pick[0].device;}
    portHint();
    if(candidates.length===1)notice(`日志口候选：${candidates[0].device}（FT4232H 通道 C）。选择后点击连接；固件由收到的数据确认。`);
    else if(candidates.length>1)notice(`发现 ${candidates.length} 个通道 C，未自动选择。请按 USB 位置或序列号选择开发板。`);
    else notice(ports.length?"没有确定的 Ti60 日志口候选，未自动选择。请核对通道 C 或手动填写串口。":"未发现 USB 串口，可手动填写设备路径并刷新。");
  }catch(e){notice(e.message,"error");}
}
$("port").addEventListener("input",()=>{++portInputEpoch;portHint();});
$("refreshPorts").addEventListener("click",refreshPorts);
$("connect").addEventListener("click",async()=>{
  try{await api("/api/connect",{port:$("port").value.trim(),baud:Number($("baud").value),label:$("recordLabel").value});cursor=0;lastAck=0;lastError="";statusSynced=false;presetRequestPending=false;presetRequestedOperation=null;pendingCommands.clear();$("log").textContent="";notice("串口已连接，正在被动识别固件。旧版固件可记录日志，扩展控制需 R4 / R5；分辨率切换需 R5 配套 bit。");await poll();}catch(e){notice(e.message,"error");}
});
$("autoConnect").addEventListener("click",async()=>{
  $("autoConnect").disabled=true;notice("正在识别 Ti60 日志口，接收遥测后连接…");
  try{const data=await api("/api/auto-connect",{baud:115200,label:$("recordLabel").value});$("port").value=data.port;cursor=0;lastAck=0;lastError="";statusSynced=false;pendingCommands.clear();await poll();$("autoConnect").focus();notice("已识别并连接 "+data.port+"；电脑键盘已就绪。","success");}
  catch(e){notice(e.message,"error");}finally{$("autoConnect").disabled=connected;}
});
$("disconnect").addEventListener("click",async()=>{try{release();await queue.catch(()=>{});await api("/api/disconnect",{});presetRequestPending=false;presetRequestedOperation=null;notice("串口已断开，记录已保存。");await poll();}catch(e){notice(e.message,"error");}});
$("help").addEventListener("click",async()=>{try{await api("/api/help",{});notice("已发送兼容旧版的帮助键 h。");}catch(e){notice(e.message,"error");}});
$("newRecord").addEventListener("click",async()=>{try{await api("/api/record",{label:$("recordLabel").value});notice("已保存上一份记录并建立新记录。","success");}catch(e){notice(e.message,"error");}});
$("mark").addEventListener("click",async()=>{try{if(!connected)throw new Error("请先连接串口");const text=$("markText").value.trim();if(!text)return;await api("/api/mark",{text});$("markText").value="";notice("标记已写入操作记录。","success");}catch(e){notice(e.message,"error");}});
$("clearView").addEventListener("click",()=>{$("log").textContent="";notice("已清空显示；磁盘记录继续保留。");});
document.querySelectorAll("a[href^='/api/export']").forEach(a=>a.addEventListener("click",e=>{if(!snapshot?.record){e.preventDefault();notice("连接后才会建立记录文件。","error");}}));
function updateScene(scene){
  const extended=!!snapshot?.state.pressure_supported;
  if(scene===currentScene&&pressureCapability===extended)return;currentScene=scene;pressureCapability=extended;
  document.querySelectorAll("[data-scene]").forEach(b=>b.classList.toggle("selected",Number(b.dataset.scene)===scene));
  $("imagePeriodField").classList.toggle("hidden",scene!==3);
  $("carrotTier").classList.toggle("hidden",scene!==1);$("thStage").classList.toggle("hidden",scene!==0);$("densityControls").classList.toggle("hidden",scene!==0);
  const choices=scene===0?[[75,"75% · 小"],[100,"100% · 原尺寸"],[125,"125% · 大"],[150,"150% · 特大"]]:[[9,"9 px · 小"],[13,"13 px · 标准"],[17,"17 px · 大"],[25,"25 px · 特大"]];
  if(extended)choices.unshift([1,"1 px"],[3,"3 px"],[6,"6 px"]);
  const squares=(extended?[0,1,3,6,8,12,16,24,32,40]:[0,6,8,12,16,24,32,40]);
  $("squareSize").replaceChildren(...squares.map(value=>{const o=document.createElement("option");o.value=value;o.textContent=value?`${value} px`:"随机 6–40 px";return o;}));
  $("bulletSize").replaceChildren(...choices.map(([value,text])=>{const option=document.createElement("option");option.value=value;option.textContent=text;return option;}));
  $("targetCount").max=scene===2?35200:32768;
  $("targetCaption").textContent=scene===2?"方块数量":"敌弹上限";
  $("sceneHint").textContent=sceneNames[scene]||"等待读取场景";
  $("quantityNote").textContent=extended?(scene===2?"1–35,200 个；容量上限不代表稳定 60 帧上限。":scene===0?"东方只调密度倍率（下方按钮），不改数量。":"1–32,768 颗；设置数量会直接进入萝卜自定义压力档。") : scene===2?"1–30,000 个；固定尺寸与原随机尺寸均使用同一对象表。":scene===0?"1–16,384 颗上限；调节上限会关闭自动负载，密度倍率独立设置。":"1–40,000 颗上限；需要先进入萝卜自定义压力档。";
}
function number(id,value,unit="",decimals=1){$(id).textContent=value===undefined||value===null?"—":Number(value).toFixed(decimals)+unit;}
function configuringPreset(){return presetRequestPending||["applying","verifying"].includes(snapshot?.preset?.status);}
async function loadPresets(){
  try{
    const data=await api("/api/presets");presetCatalog=data.profiles;presetCatalogError="";
    renderPresetCatalog();
  }catch(e){presetCatalogError="预设服务未加载，请重启上位机服务并刷新页面。";}
  if(snapshot)renderPresetState(snapshot);
}
function renderPresetCatalog(){
  const cards=$("r2PresetGroups"),verify=$("r2Verification");cards.replaceChildren();verify.replaceChildren();
  document.querySelectorAll("[data-preset-group]").forEach(b=>{
    const selected=b.dataset.presetGroup===visiblePresetGroup;b.classList.toggle("selected",selected);b.setAttribute("aria-pressed",String(selected));
  });
  const makeButton=(p,card)=>{
    const button=document.createElement("button");button.type="button";button.dataset.preset=p.id;button.disabled=true;
    button.setAttribute("aria-label",`${p.group} ${p.label}`);
    if(card){
      button.className="preset-limit-card";
      const size=document.createElement("span"),amount=document.createElement("strong"),unit=document.createElement("span");
      size.className="preset-size";size.textContent=p.size_label;amount.textContent=p.count.toLocaleString();unit.textContent="个";
      button.append(size,amount,unit);
    }else button.textContent=`60 帧呈现验证 · ${p.label}`;
    button.addEventListener("click",()=>applyPreset(p.id));return button;
  };
  for(const p of presetCatalog.filter(x=>x.group===visiblePresetGroup&&x.category==="limit"))cards.append(makeButton(p,true));
  const history=presetCatalog.filter(x=>x.group===visiblePresetGroup&&x.category==="verification");
  if(history.length){const title=document.createElement("strong");title.textContent="独立验证";verify.append(title,...history.map(p=>makeButton(p,false)));}
  if(snapshot)renderPresetState(snapshot);
}
document.querySelectorAll("[data-preset-group]").forEach(b=>b.addEventListener("click",()=>{visiblePresetGroup=b.dataset.presetGroup;renderPresetCatalog();}));
async function applyPreset(id){
  if(configuringPreset())return;
  presetRequestPending=true;presetRequestedOperation=null;$("captureKeys").checked=false;release();
  if(snapshot)render(snapshot);
  try{
    await queue.catch(()=>{});
    const accepted=await api("/api/preset",{id});presetRequestedOperation=accepted.operation;notice("正在设置 R2 对照条件，等待设备回执和新的测量窗口。");
  }catch(e){presetRequestPending=false;presetRequestedOperation=null;notice(e.message,"error");}
  finally{await poll();if(snapshot)render(snapshot);}
}
function renderPresetState(data){
  const p=data.preset||{status:"idle"},busy=configuringPreset(),el=$("r2PresetStatus");
  for(const b of document.querySelectorAll("[data-preset]")){
    b.disabled=busy||!!data.preset_unavailable||!data.presets_supported;
    b.classList.toggle("selected",p.status==="applied"&&p.conditions_match&&p.id===b.dataset.preset);
  }
  el.className="preset-status";
  if(presetCatalogError){el.textContent=presetCatalogError;return;}
  if(presetRequestPending){el.textContent="正在请求预设并等待执行状态…";return;}
  if(p.status==="applying"){el.textContent=`${p.group} ${p.label}：设置中 ${p.step} / ${p.total}。逐条等待设备确认。`;return;}
  if(p.status==="verifying"){el.textContent=`${p.group} ${p.label}：指令已确认，正在核对新测量窗口。`;return;}
  if(p.status==="failed"||p.status==="cancelled"){
    el.classList.add("error");el.textContent=`预设${p.status==="failed"?"未完成":"已取消"}：${p.error}。已执行的设置请以设备回报为准。`;return;
  }
  if(p.status==="applied"){
    if(!data.connected){el.textContent="上次预设已完成；当前串口未连接。";return;}
    if(!p.conditions_match){el.classList.add("warning");el.textContent="基准条件已改变："+(p.mismatches||[]).slice(0,5).map(x=>`${x.label} 当前 ${x.actual??"未回报"}，应为 ${x.expected}`).join("；")+"。可重新点击预设恢复。";return;}
    el.classList.add("confirmed");el.textContent=`已确认 ${p.group} · ${p.label}（窗口 ${p.confirmed_seq}）。当前目标 ${p.current_target?.toLocaleString()||"—"}${p.current_target!==p.initial_target?`，预设初始 ${p.initial_target.toLocaleString()}`:""}；成绩以实际呈现和渲染数据为准。`;return;
  }
  el.textContent=data.preset_unavailable||"选择一个基准负载；执行完成后会核对设备实际生效的条件。";
}
function engineText(s){
  if(s.submit_mode===undefined||s.band===undefined||s.cache===undefined)return "旧固件未报告完整加速状态，请使用配套 R5.2 ELF。";
  const on=x=>x===undefined?"未知":x?"开":"关";
  return `提交 ${["逐对象 calls","APB 列表","DDR 硬件列表","APB 列表字"][s.submit_mode]||s.submit_mode} · 分带${on(s.band)} · 源缓存${on(s.cache)} · K 通路${on(s.kpath)} · 弹幕表${on(s.seed_table)} · 逻辑建表${on(s.seed_build)} · 重叠${on(s.overlap)}（本场景${on(s.overlap_active)}） · LIMIT ${s.rd_bursts}/${s.wr_bursts}/${s.rd_words}`;
}
function render(data){
  if(presetRequestedOperation!==null&&data.preset?.operation===presetRequestedOperation){presetRequestPending=false;presetRequestedOperation=null;}
  if(!data.connected){presetRequestPending=false;presetRequestedOperation=null;}
  snapshot=data;connected=data.connected;compatible=data.compatible;dualvideo=!!data.dualvideo;token=data.token;const state=data.state,configuring=configuringPreset()||coopStartBusy;
  $("autoConnect").disabled=connected||!!data.discovering;
  if(data.discovering)$("portInfo").textContent=(data.discovery||[]).map(p=>p.port+"："+p.status).join("；");
  if(connected&&data.port){$("port").value=data.port;portHint();}
  $("consoleVersion").textContent=data.console_version||"旧版服务（重启后加载端口修复）";
  $("connectionBadge").textContent=!connected?"串口未连接":compatible?(String(state.firmware||"").includes("PTY")?"伪终端测试 · 非实板":(dualvideo?"R5 已连接":"兼容协议已连接")):"已连接 · 识别中";
  $("connectionBadge").className="badge "+(connected?(compatible?"online":"warning"):"");
  $("connect").disabled=connected;$("disconnect").disabled=!connected;$("help").disabled=!connected||configuring;$("port").disabled=connected;$("baud").disabled=connected;
  for(const el of document.querySelectorAll("[data-command],[data-scene],[data-step],[data-setting],[data-held],[data-players],#decrease,#increase,#setCount,#bulletSize,#squareSize,#stressStart,#volume,#captureKeys,#targetCount,#imagePeriod"))el.disabled=!connected||!compatible||configuring;
  $("videoState").textContent=!connected?"输出 1280×720 · 60 Hz。":`设备回报：${state.output_w||"—"}×${state.output_h||"—"}，${state.output_hz||state.nominal_hz||60} Hz。${state.bars?"当前为彩条测试。":""}`;
  for(const id of ["restoreFast","restoreFastDiag","testBarsOn","testBarsOff"])$(id).disabled=!connected||!state.accel_supported||configuring;
  for(const id of ["volumeDown","volumeUp"])$(id).disabled=!connected||!state.volume_step_supported||configuring;
  $("captureKeys").disabled=!connected||!compatible||configuring||!!state.input_diag;
  $("keyboardCaptureState").textContent=!connected?"连接串口后自动接收电脑键盘。":!compatible?"正在识别固件，稍候即可使用键盘。":state.input_diag?"只检测按键中：电脑键盘捕获暂停。请先结束检测，或松开全部按键后启动三人新局。":$("captureKeys").checked?"电脑键盘已就绪：保持本页面在前台，直接使用 WASD／方向键；编辑文本时暂停接收。":"电脑键盘控制未启用；勾选上方开关后用电脑键盘操作（P1）。";
  // 三人合作（2026-10-26，萝卜）
  $("startCoopCarrot").disabled=!connected||!state.coop_start_supported||configuring||!!state.matrix_cal||matrixRestoreBusy;
  $("coopHelp").textContent=coopStartBusy?"正在确认输入状态并启动三人新局，请稍候。":state.matrix_cal?"正在校准矩阵：先完成并应用映射，或取消本次校准，再启动游戏。":state.input_diag?"仍在只检测按键：扩展键盘不驱动角色，电脑键盘捕获也暂停。松开全部按键后，点击三人启动按钮会先退出检测；也可点击下方“结束检测”。":state.scene===1&&state.phase===5?"本局已失败（三人都倒下）。点击三人启动按钮重新开局。":state.auto?"当前自动演示保持单人。点击三人启动按钮会关闭自动演示、开始新局并启用电脑键盘。":state.players===3?"三人模式已启用。P1：网页键盘控制 / 串口；P2：USB 键盘；P3：矩阵板。升级时轮到的玩家用自己的 1/2/3，P1 也可以代选。":"点击上方按钮开三人新局；压力模式不会显示三个角色。";
  $("coopHelp").classList.toggle("input-blocked",!!state.input_diag||!!state.matrix_cal);
  $("inputState").textContent=`矩阵原始键位：${state.matrix_keys===undefined?"等待状态":"0x"+Number(state.matrix_keys).toString(16).padStart(4,"0")} · USB键盘报告 ${state.keyboard_reports??"—"} · UART错误 ${state.uart_error??"—"} · 鬼键 ${state.ghosts??"—"}。零键位不能单独证明未连接。`;
  renderInputDiagnostics(state,configuring);
  renderLangParty(state,configuring);
  $("partyMode").textContent=state.input_diag?"输入检测中":state.players===3?"同屏三人":state.players===1?(state.auto?"自动演示 · 单人":"单人 / 压力"):"菜单 / 其他场景";
  $("partyMode").className="badge "+(state.input_diag?"warning":"neutral");
  for(let p=1;p<=3;p++){
    const life=state[`p${p}_life`],score=state[`p${p}_score`];
    $("party"+p+"Stats").textContent=p>(state.players||0)?"当前场景未启用":life===undefined?"等待角色状态":`生命 ${life<=0?"倒下":life} · 击败 ${score??"—"}${state.players>=1&&state["p"+p+"_skill"]!==undefined?" · 大招"+(state["p"+p+"_skill"]?"就绪":"冷却"):""}`;
    if(p<=(state.players||0)&&life!==undefined){
      if(p===1)$("party"+p+"Stats").textContent+=" · 疾风移速被动";
      if(p===2&&state.p2_parry_cd!==undefined)$("party"+p+"Stats").textContent+=` · 反弹 ${Math.ceil(state.p2_parry_cd/60)}秒 / 已反弹 ${state.p2_reflected||0}`;
      if(p===3&&state.p3_shield!==undefined)$("party"+p+"Stats").textContent+=` · 护罩 ${state.p3_shield}层 / 冷却 ${Math.ceil((state.p3_shield_cd||0)/60)}秒`;
    }
    $("party"+p+"Input").textContent=state[`p${p}_held`]===undefined?"等待独立输入状态":`逻辑位置 (${state[`p${p}_x`]}, ${state[`p${p}_y`]}) · 按键 ${state[`p${p}_held`]}`;
  }
  $("coreClock").textContent=state.core_hz?`处理时钟：${(state.core_hz/1e6).toFixed(6)} MHz。请使用对应 bit / ELF。`:"处理时钟：当前固件未回报。";
  $("engineState").textContent=engineText(state);
  const degraded=state.accel_supported&&(!state.hw||state.submit_mode!==2||!state.band||!state.cache||!state.kpath||!state.seed_table||!state.seed_build||!state.overlap||(state.scene===2&&!state.table));
  $("engineWarning").textContent=degraded?"当前存在关闭的加速项。比较稳定 60 帧上限前，可先恢复加速配置，并保持场景、尺寸、Alpha 和同步条件一致。":state.accel_supported?"加速条件已报告。":"当前固件未报告完整加速项，无法判定测试基线。";
  $("engineWarning").classList.toggle("degraded",!!degraded);
  const on=x=>x===undefined?"未知":x?"开":"关";
  const submitButton=document.querySelector('[data-command="key"][data-value="107"]');
  submitButton.textContent=`提交路径：${["逐对象调用","APB 列表","DDR 硬件列表","APB 列表字"][state.submit_mode]||"未知"} · 切换`;
  for(const [key,field,title] of [[48,"cache","源缓存"],[53,"band","分带"],[113,"overlap","逻辑重叠"],[122,"seed_build","逻辑建表"],[106,"sort","键排序"]]){
    const b=document.querySelector(`[data-command="key"][data-value="${key}"]`);b.textContent=`${title}：${on(state[field])} · 切换`;
  }
  const bandButton=document.querySelector('[data-command="key"][data-value="53"]');
  if(state.submit_mode!==undefined&&state.submit_mode!==2)bandButton.textContent="分带未生效：需要 DDR 硬件列表";
  $("audioState").textContent=`音库 ${state.bank_sounds??"—"} 个，混音器${on(state.mixer)}，测试音${on(state.tone)}。测试音直接由 HDMI 模块产生。`;
  $("newRecord").disabled=!connected||configuring;$("mark").disabled=!connected;
  if(state.profile===0)$("stats").disabled=true;
  if(state.scene!==undefined)updateScene(Number(state.scene));
  if(connected&&compatible&&!configuring){$("thSkip").disabled=currentScene!==0;$("sqSort").disabled=currentScene!==2;}   /* 跳过只在东方，排序只在方块 */
  if(connected&&compatible&&!configuring){$("stressStart").disabled=currentScene!==0&&currentScene!==1;$("bulletSize").disabled=currentScene!==0&&currentScene!==1;$("squareSize").disabled=currentScene!==2;for(const id of ["decrease","increase","setCount","targetCount"])$(id).disabled=currentScene===3;}
  for(const [id,key] of Object.entries(boolFields))if(state[key]!==undefined&&document.activeElement!==$(id))$(id).checked=!!state[key];
  if(state.step!==undefined)document.querySelectorAll("[data-step]").forEach(e=>e.classList.toggle("selected",Number(e.dataset.step)===state.step));
  if(state.bullet!==undefined&&document.activeElement!==$("bulletSize"))$("bulletSize").value=state.bullet;
  if(state.period!==undefined&&document.activeElement!==$("imagePeriod"))$("imagePeriod").value=state.period;
  if(state.square!==undefined&&document.activeElement!==$("squareSize"))$("squareSize").value=state.square;
  if(state.target!==undefined&&document.activeElement!==$("targetCount"))$("targetCount").value=state.target;
  if(state.volume!==undefined&&document.activeElement!==$("volume")){$("volume").value=state.volume;$("volumeText").textContent=Math.round(state.volume/256*100)+"%";}
  $("trackName").textContent=state.track?(tracks[state.track]||state.track):"等待读取当前曲目";
  if(data.record)$("recordPath").textContent=data.record.raw;
  if(data.error&&data.error!==lastError){lastError=data.error;notice(data.error,"error");}
  for(const a of data.acks){if(a.id<=lastAck)continue;lastAck=a.id;const command=pendingCommands.get(a.id);pendingCommands.delete(a.id);if(!a.ok)notice(`设备拒绝指令 ${a.id}：当前场景不支持或参数超出范围。`,"error");else if(command&&command!=="move"&&command!=="stop")notice(command==="status"?"设备状态已同步。":`设备已确认指令 ${a.id}。`,"success");}
  if(data.lines.length){const log=$("log");log.textContent+=(log.textContent?"\n":"")+data.lines.map(x=>x.text).join("\n");if(log.textContent.length>70000)log.textContent=log.textContent.slice(-60000);if($("followLog").checked)log.scrollTop=log.scrollHeight;cursor=data.cursor;}
  const s=data.samples.at(-1);
  if(s){
    const age=(Date.now()-Date.parse(s.timestamp))/1000;$("measurementBadge").textContent=!connected?"历史记录":age>3?"数据等待更新":`${sceneNames[s.scene]||"场景"} · 窗口 ${s.seq}`;$("measurementBadge").className="badge "+(connected&&age<=3?"online":"neutral");
    number("presentFps",s.present_fps);number("renderFps",s.render_fps);number("logicHz",s.logic_hz);
    number("liveCount",s.count,"",0);number("liveTarget",s.target,"",0);number("workMs",s.work_us/1000," ms",2);number("maxMs",s.work_max_us/1000," ms",2);number("videoUf",s.video_uf,"",0);number("audioUf",s.audio_uf,"",0);
    const values=[s.logic_us||0,s.submit_us||0,s.wait_us||0],total=values.reduce((a,b)=>a+b,0)||1;
    [...$("costBar").children].forEach((e,i)=>e.style.width=values[i]/total*100+"%");$("costText").textContent=values.map(x=>(x/1000).toFixed(2)).join(" / ")+" ms";
    $("conditionLine").textContent=`渲染 ${s.render_w||s.w||1280}×${s.render_h||s.h||720}　输出 ${s.output_w||s.w||1280}×${s.output_h||s.h||720}　${s.hw?"硬件":"CPU"}绘制　透明${s.alpha?"开":"关"}　同步${s.vsync?"开":"关"}　${s.scene===2?"方块 "+(s.square||"随机")+"px":"弹体 "+s.bullet+(s.scene===0&&s.bullet>6?"%":"px")}　步长 ${s.step}　超预算 ${s.over||0} 帧`;
    $("conditionLine").textContent+="\n"+engineText(s);
    $("pressureMetrics").textContent=`敌人与角色绘制 ${s.enemy_us??"—"} μs，玩家子弹逻辑 ${s.player_logic_us??"—"} μs，敌弹逻辑 ${s.hostile_logic_us??"—"} μs，HUD ${s.hud_us??"—"} μs；分带提前结束 ${s.band_splits??"—"} 次，桶溢出 ${s.band_overflow??"—"}。（STATS=1 固件才有分段计时）`;
  }
  renderPresetState(data);
  chart(data.samples);
  if(connected&&compatible&&data.samples.length&&!statusSynced){statusSynced=true;cmd("status").catch(()=>{statusSynced=false;});}
}
function chart(samples){
  const canvas=$("chart"),box=canvas.getBoundingClientRect(),dpr=window.devicePixelRatio||1,w=Math.max(220,box.width),h=210;
  if(canvas.width!==Math.round(w*dpr)||canvas.height!==h*dpr){canvas.width=Math.round(w*dpr);canvas.height=h*dpr;}const c=canvas.getContext("2d");c.setTransform(dpr,0,0,dpr,0,0);c.clearRect(0,0,w,h);
  const left=32,right=w-10,top=12,bottom=184,max=Math.max(70,Math.ceil(Math.max(0,...samples.map(s=>s.render_fps||0))/30)*30);c.font="10px sans-serif";
  for(let i=0;i<4;i++){let value=max*i/3,y=bottom-(bottom-top)*value/max;c.strokeStyle="#e2eaf0";c.beginPath();c.moveTo(left,y);c.lineTo(right,y);c.stroke();c.fillStyle="#7d8d98";c.fillText(value.toFixed(0),3,y+3);}
  const y60=bottom-(bottom-top)*60/max;c.setLineDash([4,4]);c.strokeStyle="#afbfca";c.beginPath();c.moveTo(left,y60);c.lineTo(right,y60);c.stroke();c.setLineDash([]);c.fillStyle="#7d8d98";c.fillText("60",right-15,y60-4);
  for(const [field,color] of [["render_fps","#24547c"],["logic_hz","#b18a40"],["present_fps","#087e8b"]]){
    c.strokeStyle=color;c.lineWidth=field==="present_fps"?2.2:1.4;c.beginPath();let pen=false;
    samples.forEach((s,i)=>{if(s[field]===null||s[field]===undefined){pen=false;return;}const x=left+(right-left)*i/Math.max(1,samples.length-1),y=bottom-(bottom-top)*s[field]/max;if(pen)c.lineTo(x,y);else c.moveTo(x,y);pen=true;});c.stroke();
  }
  if(!samples.length){c.fillStyle="#7b91a0";c.textAlign="center";c.fillText("连接设备后显示真实测量曲线",w/2,105);c.textAlign="left";}
}
async function poll(){if(polling)return;polling=true;try{render(await api("/api/state?after="+cursor));}catch(e){notice("本地服务未响应："+e.message,"error");}finally{polling=false;}}
window.addEventListener("resize",()=>chart(snapshot?.samples||[]));setInterval(poll,500);setInterval(()=>{if($("captureKeys").checked||held)sendHeld();},100);
Promise.all([poll(),loadPresets()]).then(refreshPorts);

// ---- 界面语言 / 萝卜人数（2026-10-28）
$("language").addEventListener("change",e=>cmd("lang",Number(e.target.value)));
document.querySelectorAll("[data-players]").forEach(b=>b.addEventListener("click",()=>cmd("players",Number(b.dataset.players))));
function renderLangParty(state,configuring){
  $("language").disabled=!connected||!state.lang_supported||configuring;
  if(document.activeElement!==$("language"))$("language").value=state.lang??0;
  const sel=state.selected_players;
  document.querySelectorAll("[data-players]").forEach(b=>{b.disabled=!connected||!state.party_supported||configuring;b.classList.toggle("selected",Number(b.dataset.players)===sel);});
  document.querySelectorAll('[data-command="campaign"]').forEach(b=>b.disabled=!connected||!state.campaign_supported||configuring);
  $("campaignState").textContent=state.campaign_supported?(state.campaign_endless?`第 6 / 6 关 · 无限模式 · 第 ${(state.endless_wave||0)+1} 波 · 蚀日龙皇已登场 ${state.ultimate_spawned||0} 次`:`第 ${state.campaign_scene} / 6 关 · 剩余 ${state.campaign_remaining} 秒${state.campaign_timeout?" · 超时结束":""}`):"新版体验使用 V6 固件";
  $("startParty").disabled=!connected||!state.party_supported||configuring;
  $("partySelected").textContent=sel?`下一局人数：${sel} 人${state.scene===1&&state.players&&state.players!==sel?`（当前这局 ${state.players} 人，开新局后生效）`:""}${state.team_level!==undefined&&state.players>1?` · 全队 ${state.team_level} 级，经验 ${state.team_xp}/${state.team_next_xp}${state.upgrade_wait?" · 等待选卡":""}`:""}`:"下一局人数：等待设备回报";
}

// ---- 三人合作 / 输入诊断（2026-10-26，原样取自队友版面板，去掉东方入口）
const mapStorageKey="ti60-r59-matrix-map-v1";
let savedMatrixMap=null,matrixRestoreBusy=false,lastSavedMatrixMap="";
try{const x=JSON.parse(localStorage.getItem(mapStorageKey)||"null");if(x&&validMatrixMap(x.map))savedMatrixMap=x;}catch(e){}
function validMatrixMap(x){return typeof x==="string"&&/^x[0-9a-f]{16}$/.test(x)&&new Set(x.slice(1)).size===16;}
function hexKeys(x){return "0x"+Number(x||0).toString(16).padStart(4,"0");}
for(let i=0;i<16;i++){const cell=document.createElement("span");cell.textContent="S"+(i+1);$("matrixCalibrationGrid").append(cell);}
async function confirmedInputCommand(command,value,requiredTags=[]){
  const boot=snapshot?.boot,record=snapshot?.record?.raw;
  const result=await cmd(command,value),deadline=Date.now()+7000;
  while(Date.now()<deadline){
    if(!connected||snapshot?.boot!==boot||snapshot?.record?.raw!==record)throw new Error("连接或固件状态已变化，本次操作已停止，请重新读取状态。");
    const ack=snapshot?.acks?.find(x=>x.id===result.id),response=snapshot?.responses?.[result.id];
    if(ack){
      if(!ack.ok)throw new Error("设备拒绝控制命令，请确认校准已结束并松开全部按键后重试。");
      if(requiredTags.every(tag=>response?.tags?.includes(tag)))return response?.values||{};
    }
    await new Promise(resolve=>setTimeout(resolve,100));
  }
  throw new Error("等待设备回执或完整状态超时，本次操作已停止，请同步状态后重试。");
}
async function startCoopGame(scene){
  if(coopStartBusy||matrixRestoreBusy)return;
  const wasCapture=$("captureKeys").checked;coopStartBusy=true;$("captureKeys").checked=false;release();
  try{
    if(!snapshot?.state.coop_start_supported)throw new Error("当前固件不支持三人启动入口。");
    const diagnosticCapable=snapshot.state.input_diag_supported===1;
    if(diagnosticCapable&&(!snapshot.responses||snapshot.boot===undefined))throw new Error("请重启上位机服务后再启动三人游戏。");
    // Require the state tagged with this exact request, not a stale periodic
    // sample or merely an ACK that preceded the following telemetry lines.
    const tags=["S4","T7",...(diagnosticCapable?["I5","I9"]:[])];
    const state=await confirmedInputCommand("status",undefined,tags);
    if(state.matrix_cal)throw new Error("矩阵校准尚未结束，请先应用完整映射或取消本次校准。");
    if(state.input_diag){
      if(state.matrix_keys||(state.keyboard_down??state.keyboard_held))throw new Error("请先松开矩阵板和 USB 键盘的全部按键，再点击三人启动。");
      const enabled=await confirmedInputCommand("input_diag",0,["I9"]);
      if(enabled.input_diag!==0)throw new Error("尚未确认输入检测已退出，已停止启动游戏。");
    }
    const game=await confirmedInputCommand("coop",scene,tags);
    if(game.players!==3||game.scene!==scene||game.auto!==0||game.input_diag)throw new Error("尚未确认三人新局和输入状态，请同步设备状态后重试。");
    $("captureKeys").checked=true;
    $("startCoopCarrot").focus();
    notice("三人新局已启动，输入检测已退出，电脑键盘控制已启用。","success");
  }catch(e){$("captureKeys").checked=wasCapture;notice(e.message,"error");}
  finally{coopStartBusy=false;}
}
$("matrixRestore").addEventListener("click",async()=>{
  if(!savedMatrixMap||!validMatrixMap(savedMatrixMap.map)||matrixRestoreBusy)return;
  matrixRestoreBusy=true;release();
  try{
    await confirmedInputCommand("matrix_cal",1);
    for(let i=0;i<16;i++)await confirmedInputCommand("matrix_bind",(i<<4)|parseInt(savedMatrixMap.map[i+1],16));
    await confirmedInputCommand("matrix_cal",2);
    notice("已恢复本机保存的映射。先检测 S2/S5/S6/S7，再结束检测。","success");
  }catch(e){notice(e.message,"error");}
  finally{matrixRestoreBusy=false;}
});
function renderInputDiagnostics(s,configuring){
  if(s.input_diag===1&&lastInputDiagnostic!==1)release();
  lastInputDiagnostic=s.input_diag;
  const available=connected&&compatible&&s.input_diag_supported===1&&s.input_supported===1&&!configuring&&!matrixRestoreBusy;
  const active=!!s.matrix_cal,step=Number(s.matrix_step||0),released=!s.matrix_keys&&!(s.keyboard_down??s.keyboard_held);
  $("inputDiagOn").disabled=!available||!!s.input_diag;
  $("inputDiagOff").disabled=!available||!s.input_diag||active||!released;
  $("matrixCalBegin").disabled=!available;
  $("matrixCalUndo").disabled=!available||!active||!step||!released;
  $("matrixCalCancel").disabled=!available||!active;
  $("matrixCalApply").disabled=!available||!active||step!==16||!released;
  $("keyboardSync").disabled=!available||!s.input_diag;
  $("matrixRestore").disabled=!available||!savedMatrixMap||!released||active;
  $("inputDiagState").textContent=!s.input_diag_supported?"需要带输入页（rtl/input/input_hub.v）的新 bit；旧 bit 只能单人或用上位机强制三人。":s.input_diag?"检测中：扩展键盘和网页电脑键盘捕获都暂停。校准应用后仍保留检测状态；松开全部键，点结束检测，或点三人启动按钮自动退出。":"游戏输入已启用。首次接线请先进入只检测按键模式。";
  $("matrixCalPrompt").textContent=active?(step===16?"16 键已记录，全部松开后点击“应用完整映射”。":`已记录 ${step}/16。${s.matrix_wait?"请先松开全部按键，然后":"请"}只按板上 S${step+1}，松开并等待进度变化。`):s.matrix_mapped?"已应用校准。可在检测模式验证每一键后恢复游戏操作。":"当前为默认行列映射。点击开始校准，按板上丝印从 S1 到 S16 逐个记录。";
  if(active&&s.matrix_cal_error)$("matrixCalPrompt").textContent+=(s.matrix_cal_error===1?" 本次检测到多键，请松开重按当前键。":" 这条原始键位已录过，请松开后按当前提示的键；接触短路也会导致重复。");
  [...$("matrixCalibrationGrid").children].forEach((cell,i)=>{cell.classList.toggle("recorded",active&&i<step);cell.classList.toggle("current",active&&i===step);const m=String(s.matrix_candidate||"").slice(1);cell.textContent="S"+(i+1)+(active&&i<step?" · "+m[i].toUpperCase():"");});
  $("matrixRawState").textContent=`当前原始 ${hexKeys(s.matrix_keys)}；最近按下 ${hexKeys(s.matrix_last)}；累计按下事件 ${s.matrix_events??"—"}。映射后逻辑 ${hexKeys(s.matrix_logical)}。短按后松开仍保留最近记录。`;
  if(s.matrix_mapped&&validMatrixMap(s.matrix_map)&&s.matrix_map!==lastSavedMatrixMap){
    const item={map:s.matrix_map,saved_at:new Date().toISOString()};
    try{localStorage.setItem(mapStorageKey,JSON.stringify(item));savedMatrixMap=item;lastSavedMatrixMap=s.matrix_map;}catch(e){$("matrixSavedState").textContent="浏览器无法保存校准，请记录日志中的 matrix_map。";}
  }
  $("matrixSavedState").textContent=savedMatrixMap?`本机已存：${savedMatrixMap.map.slice(1)}。FPGA 断电或重载 ELF 后，需点击“载入本机已存校准”；不会自动写入板载 Flash。`:"校准应用后保存在当前浏览器；关闭网页不会丢失。更换接线后请重新校准。";
  const bytes=s.keyboard_bytes,errors=Number(s.keyboard_faults||0);
  $("keyboardByteState").textContent=`接收字节 ${bytes??"—"} · 57 AB 帧头 ${s.keyboard_headers??"—"} · 键盘报告 ${s.keyboard_reports??"—"} · 鼠标报告 ${s.mouse_reports??"—"} · 累计 UART 错误 ${s.keyboard_faults??"—"} · 应答已提交 ${s.keyboard_acks??"—"} · 发送超时 ${s.keyboard_tx_timeouts??"—"}`;
  const tail=String(s.keyboard_tail||"x").slice(1);
  $("keyboardByteTail").textContent=tail?tail.match(/.{1,2}/g).join(" ").toUpperCase():"尚未收到原始字节";
  $("keyboardDiagnosticHint").textContent=bytes===undefined?"新固件会显示最近 16 个原始字节。":errors?"检测到 UART 错误：核对 115200 8N1、共地和信号电平；日志保留累计错误，即使硬件错误位已清除。":!bytes?"尚未从接收 FIFO 取到字节。按 W 并松开，观察计数；若仍为 0，按接线说明做 TX→RX 和 USB 端枚举检查。":!s.keyboard_reports?"已有串口字节，但尚无完整键盘报告。查看是否有 57 AB 01；57 AB 82/88 通常说明模块仍处于状态0/1。":"已解析到键盘报告。按 W 时 keyboard_held 应含 1，松开归零；结束检测后进入三人游戏验证 P2。";
}

/* 手柄（2026-10-28，浏览器 Gamepad API，冰原狼 2 用 XInput / PC 模式，标准布局）：算 P1，和电脑键盘一样要勾"启用键盘控制"。
 * 摇杆 / 十字键 移动，A 射击 / 确定，B 符卡，RB / RT / LT 低速，LB 东方跳过，Menu 暂停，View 回菜单；
 * 萝卜升级时 X / Y / B = 选项 1 / 2 / 3。 */
let padPrev=[],padName="";
function pollGamepad(){
  const pads=navigator.getGamepads?[...navigator.getGamepads()].filter(Boolean):[],p=pads[0];
  const name=p?p.id:"";if(name!==padName){padName=name;const el=$("gamepadState");if(el)el.textContent=name?`手柄：${name}（P1）`:"手柄：未检测到（插上后按任意键）";}
  const active=p&&document.hasFocus()&&!document.hidden&&$("captureKeys").checked&&!typing()&&connected&&compatible&&!snapshot?.state.input_diag&&!coopStartBusy;
  if(!active){if(padHeldBits){padHeldBits=0;updateHeld();}padPrev=[];return;}
  const b=i=>!!(p.buttons[i]&&(p.buttons[i].pressed||p.buttons[i].value>0.5)),ax=i=>p.axes[i]||0;
  let m=0;
  if(b(12)||ax(1)<-0.5)m|=1; if(b(13)||ax(1)>0.5)m|=2; if(b(14)||ax(0)<-0.5)m|=4; if(b(15)||ax(0)>0.5)m|=8;
  if(b(0))m|=16; if(b(1))m|=32; if(b(5)||b(6)||b(7))m|=64; if(b(9))m|=128; if(b(4))m|=512;
  if(m!==padHeldBits){padHeldBits=m;updateHeld();}
  const edge=i=>b(i)&&!padPrev[i];
  if(snapshot?.state.scene===1){if(edge(2))cmd("pick",1);if(edge(3))cmd("pick",2);if(edge(1))cmd("pick",3);}
  if(edge(8))cmd("menu");
  padPrev=p.buttons.map((x,i)=>b(i));
}
window.addEventListener("gamepadconnected",pollGamepad);window.addEventListener("gamepaddisconnected",pollGamepad);
setInterval(pollGamepad,16);
