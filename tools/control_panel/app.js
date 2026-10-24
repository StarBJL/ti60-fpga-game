"use strict";
const $=id=>document.getElementById(id);
const pendingCommands=new Map();let statusSynced=false;
let pressureCapability=-1;
let presetCatalog=[],presetCatalogError="",presetRequestPending=false,presetRequestedOperation=null;
let token="",cursor=0,connected=false,compatible=false,dualvideo=false,currentScene=-1,snapshot=null,lastAck=0,queue=Promise.resolve(),polling=false,held=0,lastHeld=-1,lastError="";
const tracks={bgm_title:"像素晨光",bgm_menu2:"星灯菜单",bgm_stage1:"云端疾行",bgm_stage2:"月下航线",bgm_boss1:"赤色风暴",bgm_boss2:"终幕回响",bgm_garden:"萝卜田野",bgm_garden2:"森林夜行",bgm_stress:"压力脉冲",bgm_stress2:"数字方阵","?":"未播放"};
const sceneNames=["东方","萝卜","方块","图片"];
const boolFields={alpha:"alpha",vsync:"vsync",hardware:"hw",autoplay:"auto",motion:"motion",table:"table",bgm:"bgm",tone:"tone",verbose:"verbose",stats:"stats"};
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
function sendHeld(force=false){if(connected&&compatible&&(force||held||lastHeld!==held)){lastHeld=held;cmd("move",held).catch(()=>{});}}
function release(){held=0;lastHeld=-1;setHeld(0);keys.clear();pointerBits.clear();}
const keys=new Map(),pointerBits=new Map();
function updateHeld(){let value=0;for(const v of keys.values())value|=v;for(const v of pointerBits.values())value|=v;setHeld(value);}
const keyBits={ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,w:1,W:1,s:2,S:2,a:4,A:4,d:8,D:8,z:16,Z:16," ":16,x:32,X:32,j:32,J:32,Shift:64,Escape:128,Enter:256};
function typing(){return /^(INPUT|SELECT|TEXTAREA)$/.test(document.activeElement?.tagName||"")&&document.activeElement?.id!=="captureKeys";}
window.addEventListener("keydown",e=>{
  if(!$("captureKeys").checked||typing()||!connected||!compatible)return;
  if(keyBits[e.key]!==undefined){e.preventDefault();keys.set(e.code,keyBits[e.key]);updateHeld();}
  else if(!e.repeat&&(e.key==="+"||e.key==="="||e.key==="-")){e.preventDefault();cmd("delta",e.key==="-"?-1:1);}
  else if(!e.repeat&&e.key==="F9"){e.preventDefault();if(dualvideo&&!snapshot?.state.pending)cmd("resolution",snapshot?.state.resolution===1?0:1);else notice("等待 R5 显示能力或当前切换完成。","error");}
  else if(!e.repeat&&e.key==="F10"){e.preventDefault();if(snapshot?.state.video_trial)cmd("video_confirm");}
  else if(!e.repeat&&["1","2","3"].includes(e.key)){e.preventDefault();cmd("pick",Number(e.key));}
  else if(!e.repeat&&["F6","F7","F8"].includes(e.key)){e.preventDefault();cmd("step",{F6:100,F7:10,F8:1}[e.key]);}
  else if(!e.repeat&&(e.key==="["||e.key==="]")){e.preventDefault();const s=$("bulletSize"),i=(s.selectedIndex+s.options.length+(e.key==="]"?1:-1))%s.options.length;cmd("bullet",Number(s.options[i].value));}
  else if(!e.repeat&&(e.key==="{"||e.key==="}")){e.preventDefault();const s=$("squareSize"),i=(s.selectedIndex+s.options.length+(e.key==="}"?1:-1))%s.options.length;cmd("square",Number(s.options[i].value));}
  else if(!e.repeat&&e.key==="\\"){e.preventDefault();cmd("next");}

});
window.addEventListener("keyup",e=>{if(keyBits[e.key]!==undefined){keys.delete(e.code);updateHeld();}});
window.addEventListener("blur",release);document.addEventListener("visibilitychange",()=>{if(document.hidden)release();});
window.addEventListener("beforeunload",()=>{if(connected&&compatible)fetch("/api/command",{method:"POST",headers:{"Content-Type":"application/json","X-Ti60-Token":token},body:JSON.stringify({command:"move",value:0}),keepalive:true}).catch(()=>{});});
$("captureKeys").addEventListener("change",release);
document.querySelectorAll("[data-held]").forEach(button=>{
  button.addEventListener("pointerdown",e=>{if(!connected||!compatible)return;e.preventDefault();button.setPointerCapture(e.pointerId);pointerBits.set(e.pointerId,Number(button.dataset.held));updateHeld();});
  for(const type of ["pointerup","pointercancel","lostpointercapture"])button.addEventListener(type,e=>{pointerBits.delete(e.pointerId);updateHeld();});
});
document.querySelectorAll("[data-command]").forEach(button=>button.addEventListener("click",()=>{
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
$("stressStart").addEventListener("click",()=>cmd("stress",currentScene===1?Number($("carrotTier").value):0));
$("volume").addEventListener("input",e=>$("volumeText").textContent=Math.round(e.target.value/256*100)+"%");
$("volume").addEventListener("change",e=>cmd("volume",Number(e.target.value)));
$("musicSelect").addEventListener("change",e=>cmd("music",Number(e.target.value)));
let availablePorts=[],portsRefreshGeneration=0,portInputEpoch=0;
function portHint(){
  const p=availablePorts.find(p=>p.device===$("port").value.trim());
  $("portInfo").textContent=p ? `${p.label||p.description}${p.location?" · USB "+p.location:""}` : "可手动填写串口路径；设备编号会随 USB 插拔变化。";
}
async function refreshPorts(){
  const generation=++portsRefreshGeneration,epoch=portInputEpoch,initialValue=$("port").value;
  try{
    const ports=await api("/api/ports");if(generation!==portsRefreshGeneration)return;availablePorts=ports;
    $("ports").replaceChildren(...ports.map(port=>{const e=document.createElement("option");e.value=port.device;e.label=port.label||port.description;return e;}));
    const candidates=ports.filter(p=>p.recommended);
    if(!initialValue&&epoch===portInputEpoch&&!$("port").value&&!connected&&candidates.length===1)$("port").value=candidates[0].device;
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
  $("carrotTier").classList.toggle("hidden",scene!==1);$("densityControls").classList.toggle("hidden",scene!==0);
  const choices=scene===0?[[75,"75% · 小"],[100,"100% · 原尺寸"],[125,"125% · 大"],[150,"150% · 特大"]]:[[9,"9 px · 小"],[13,"13 px · 标准"],[17,"17 px · 大"],[25,"25 px · 特大"]];
  if(extended)choices.unshift([1,"1 px"],[3,"3 px"],[6,"6 px"]);
  const squares=(extended?[0,1,3,6,8,12,16,24,32,40]:[0,6,8,12,16,24,32,40]);
  $("squareSize").replaceChildren(...squares.map(value=>{const o=document.createElement("option");o.value=value;o.textContent=value?`${value} px`:"随机 6–40 px";return o;}));
  $("bulletSize").replaceChildren(...choices.map(([value,text])=>{const option=document.createElement("option");option.value=value;option.textContent=text;return option;}));
  $("targetCount").max=extended?99999:scene===2?30000:scene===0?16384:40000;
  $("targetCaption").textContent=scene===2?"方块数量":"敌弹上限";
  $("sceneHint").textContent=sceneNames[scene]||"等待读取场景";
  $("quantityNote").textContent=extended?(scene===2?"1–99,999 个；容量上限不代表稳定 60 帧上限。":"1–99,999 颗；先进入压力模式。萝卜压力关闭角色射击，普通游戏保留射击与技能。") : scene===2?"1–30,000 个；固定尺寸与原随机尺寸均使用同一对象表。":scene===0?"1–16,384 颗上限；调节上限会关闭自动负载，密度倍率独立设置。":"1–40,000 颗上限；需要先进入萝卜自定义压力档。";
}
function number(id,value,unit="",decimals=1){$(id).textContent=value===undefined||value===null?"—":Number(value).toFixed(decimals)+unit;}
function configuringPreset(){return presetRequestPending||["applying","verifying"].includes(snapshot?.preset?.status);}
async function loadPresets(){
  try{
    const data=await api("/api/presets");presetCatalog=data.profiles;presetCatalogError="";
    const groups=$("r2PresetGroups");groups.replaceChildren();
    for(const group of [...new Set(presetCatalog.map(x=>x.group))]){
      const box=document.createElement("div");box.className="preset-group";
      const heading=document.createElement("h3");heading.textContent=group;box.append(heading);
      const profiles=presetCatalog.filter(x=>x.group===group),detail=document.createElement("p");detail.textContent=profiles[0].detail;box.append(detail);
      for(const p of profiles){
        const button=document.createElement("button");button.dataset.preset=p.id;button.disabled=true;
        button.setAttribute("aria-label",`${group} ${p.label}`);
        const count=document.createElement("strong"),mode=document.createElement("span");count.textContent=p.count.toLocaleString();mode.textContent=p.vsync?"60 帧呈现验证":"自由吞吐对照";
        button.append(count,mode);button.addEventListener("click",()=>applyPreset(p.id));box.append(button);
      }groups.append(box);
    }
  }catch(e){presetCatalogError="预设服务未加载，请重启上位机服务并刷新页面。";}
  if(snapshot)renderPresetState(snapshot);
}
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
  snapshot=data;connected=data.connected;compatible=data.compatible;dualvideo=!!data.dualvideo;token=data.token;const state=data.state,configuring=configuringPreset();
  if(connected&&data.port){$("port").value=data.port;portHint();}
  $("consoleVersion").textContent=data.console_version||"旧版服务（重启后加载端口修复）";
  $("connectionBadge").textContent=!connected?"串口未连接":compatible?(String(state.firmware||"").includes("PTY")?"伪终端测试 · 非实板":(dualvideo?"R5 已连接":"兼容协议已连接")):"已连接 · 识别中";
  $("connectionBadge").className="badge "+(connected?(compatible?"online":"warning"):"");
  $("connect").disabled=connected;$("disconnect").disabled=!connected;$("help").disabled=!connected||configuring;$("port").disabled=connected;$("baud").disabled=connected;
  for(const el of document.querySelectorAll("[data-command],[data-scene],[data-step],[data-setting],[data-held],#decrease,#increase,#setCount,#bulletSize,#squareSize,#stressStart,#volume,#captureKeys,#targetCount,#imagePeriod"))el.disabled=!connected||!compatible||configuring;
  for(const button of document.querySelectorAll("[data-resolution]")){
    button.disabled=!connected||!dualvideo||!!state.pending||configuring;
    button.classList.toggle("selected",dualvideo&&Number(button.dataset.resolution)===state.resolution);
  }
  $("videoState").textContent=!connected?"等待读取 R5 显示能力。":!dualvideo?"当前固件 / bit 未声明双模式输出，分辨率控制不可用。":state.pending?"切换已请求，等待硬件完成；以设备回报的输出状态为准。":`设备回报：${state.output_w||"—"}×${state.output_h||"—"}，标称 ${state.output_hz||state.nominal_hz||"—"} Hz；渲染 ${state.render_w||1280}×${state.render_h||720}。${state.bars?"当前为彩条测试。":""}扫描状态不代表显示器已锁定。`;
  for(const id of ["restoreFast","restoreFastDiag","testBarsOn","testBarsOff"])$(id).disabled=!connected||!state.accel_supported||configuring;
  for(const id of ["volumeDown","volumeUp"])$(id).disabled=!connected||!state.volume_step_supported||configuring;
  for(const id of ["musicSelect","musicPrev"])$(id).disabled=!connected||!state.music_supported||configuring;
  if(state.music_supported){
    if(document.activeElement!==$("musicSelect"))$("musicSelect").value=state.music_auto?-1:state.music_index;
    $("musicMode").textContent=state.music_auto?"跟随场景自动配乐；点击上一首 / 下一首进入全曲库手动切换。":`手动曲目 ${(state.music_index??0)+1} / ${state.music_total||10}；切换场景仍保持所选曲目。`;
  }else $("musicMode").textContent="当前固件只支持场景内两首备选曲；全曲库选择需要新版 ELF。";
  $("confirmVideo").disabled=!connected||!state.video_trial||configuring;
  $("videoTrial").textContent=state.video_trial?`试切中：约 ${Math.ceil((state.trial_ms||0)/1000)} 秒后自动返回 720P。看见正常画面后再确认。`:state.trial_supported?"1080P 试切保护已就绪；黑屏可等待自动返回 720P。":"自动回退需 R5.7 ELF。";
  $("partyMode").textContent=state.players===3?"同屏三人":state.players===1?"单人 / 压力":"菜单 / 其他场景";
  $("coreClock").textContent=state.core_hz?`处理时钟：${(state.core_hz/1e6).toFixed(6)} MHz。请使用对应 bit / ELF。`:"处理时钟：当前固件未回报。";
  for(let p=1;p<=3;p++){
    const life=state[`p${p}_life`],skill=state[`p${p}_skill`],score=state[`p${p}_score`];
    $("party"+p+"Stats").textContent=p>(state.players||0)?"当前场景未启用":life===undefined?"等待角色状态":state.scene===1?`生命 ${life} · 技能 ${(skill/60).toFixed(1)}s · 击败 ${score}`:`残机 ${life<0?"退出":life} · 符卡 ${skill} · 得分 ${score}${p===3&&state.matrix_auto?" · 自动射击":""}`;
    $("party"+p+"Input").textContent=state[`p${p}_held`]===undefined?"等待独立输入状态":`逻辑位置 (${state[`p${p}_x`]}, ${state[`p${p}_y`]}) · 按键 ${state[`p${p}_held`]}`;
  }
  $("engineState").textContent=engineText(state);
  const degraded=state.accel_supported&&(!state.hw||state.submit_mode!==2||!state.band||!state.cache||!state.kpath||!state.seed_table||!state.seed_build||!state.overlap||(state.scene===2&&(!state.motion||!state.table)));
  $("engineWarning").textContent=degraded?"当前存在关闭的加速项。比较稳定 60 帧上限前，可先恢复加速配置，并保持场景、尺寸、Alpha 和同步条件一致。":state.accel_supported?"加速条件已报告。1080P 输出为 30 Hz；重叠是否实际生效由场景和分辨率决定。":"当前固件未报告完整加速项，无法判定测试基线。";
  $("engineWarning").classList.toggle("degraded",!!degraded);
  const on=x=>x===undefined?"未知":x?"开":"关";
  const submitButton=document.querySelector('[data-command="key"][data-value="107"]');
  submitButton.textContent=`提交路径：${["逐对象调用","APB 列表","DDR 硬件列表","APB 列表字"][state.submit_mode]||"未知"} · 切换`;
  for(const [key,field,title] of [[48,"cache","源缓存"],[53,"band","分带"],[113,"overlap","逻辑重叠"],[122,"seed_build","逻辑建表"],[126,"sort","键排序"]]){
    const b=document.querySelector(`[data-command="key"][data-value="${key}"]`);b.textContent=`${title}：${on(state[field])} · 切换`;
  }
  const bandButton=document.querySelector('[data-command="key"][data-value="53"]');
  if(state.submit_mode!==undefined&&state.submit_mode!==2)bandButton.textContent="分带未生效：需要 DDR 硬件列表";
  if(state.motion_min_px>1&&state.square>0&&state.square<state.motion_min_px){$("engineWarning").textContent="当前 bit 的硬件运动最小尺寸为 6px。小方块正在由 CPU 更新；加载 R5.5 bit 后可使用硬件运动。";$("motion").disabled=true;}
  const revision=state.video_rev===328193?"R5.2.1 HDMI 修正版":state.video_rev?`视频修订 ${state.video_rev}`:"未确认 HDMI 修正版 bit";
  $("audioState").textContent=`${revision}；音库 ${state.bank_sounds??"—"} 个，混音器${on(state.mixer)}，测试音${on(state.tone)}。测试音直接由 HDMI 模块产生；开启时不播放音乐。`;
  $("newRecord").disabled=!connected||configuring;$("mark").disabled=!connected;
  if(state.profile===0)$("stats").disabled=true;
  if(state.scene!==undefined)updateScene(Number(state.scene));
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
    $("pressureMetrics").textContent=s.pressure_supported?`角色子弹 ${s.player_shots??"—"}；轮廓${s.locator_on===0?"关闭":s.locator_on===1?"开启":"状态未报告"} / 计算 ${s.locator_us??"—"} μs，敌人与角色绘制 ${s.enemy_us??"—"} μs，玩家子弹逻辑 ${s.player_logic_us??"—"} μs，敌弹逻辑 ${s.hostile_logic_us??"—"} μs；分带提前结束 ${s.band_splits??"—"} 次，桶溢出 ${s.band_overflow??"—"}。`:"新增小尺寸与 99,999 容量需 R5.5 ELF；旧固件保留其原上限。";
    $("fontLine").textContent=`字体累计命中 ${s.font_hit} / 生成 ${s.font_miss} / 重置 ${s.font_reset}；静态文字命中 ${s.label_hit} / 生成 ${s.label_miss}。`;
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
window.addEventListener("resize",()=>chart(snapshot?.samples||[]));setInterval(poll,500);setInterval(()=>{if($("captureKeys").checked)sendHeld(true);else if(held)sendHeld();},100);
Promise.all([poll(),loadPresets()]).then(refreshPorts);
