const fs=require('fs'),path=require('path'),assert=require('assert');
const {chromium}=require('/home/user/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
 const fixture=JSON.parse(fs.readFileSync(path.join(__dirname,'pty_fixture.json'),'utf8'));
 const browser=await chromium.launch({executablePath:'/opt/google/chrome/chrome',headless:true,args:['--no-sandbox','--disable-dev-shm-usage']});
 try{
  const page=await browser.newPage({viewport:{width:1450,height:1100}}),errors=[];page.on('pageerror',e=>errors.push(e.message));
  let nonce=0;const delay=ms=>new Promise(r=>setTimeout(r,ms));
  function stimulus(x){const q=path.join(__dirname,'stimulus.json');fs.writeFileSync(q+'.tmp',JSON.stringify({...x,nonce:++nonce}));fs.renameSync(q+'.tmp',q);}
  function commands(){try{return JSON.parse(fs.readFileSync(path.join(__dirname,'pty_commands.json'),'utf8'));}catch(e){return [];}}
  const coops=()=>commands().filter(x=>x.command==='coop');
  async function idle(){await page.waitForFunction(()=>!coopStartBusy);}
  await page.goto(fixture.url,{waitUntil:'domcontentloaded'});await page.waitForFunction(()=>typeof token==='string'&&token.length>10);
  await page.locator('#port').fill(fixture.port);await page.locator('#connect').click();
  await page.waitForFunction(()=>snapshot?.state.input_diag===1&&snapshot.state.players===3);
  assert(await page.locator('#captureKeys').isDisabled());assert((await page.locator('#coopHelp').textContent()).includes('仍在只检测按键'));
  await page.locator('#partyModule').screenshot({path:path.join(__dirname,'before_recovery.png')});
  await page.locator('#startCoopCarrot').click();
  await page.waitForFunction(()=>!coopStartBusy&&snapshot.state.input_diag===0&&snapshot.state.phase===2&&snapshot.state.players===3&&document.getElementById('captureKeys').checked);
  assert.deepEqual(commands().filter(x=>['status','input_diag','coop'].includes(x.command)).slice(-3).map(x=>[x.command,x.value]),[['status',null],['input_diag',0],['coop',1]]);
  await page.keyboard.down('w');await page.waitForFunction(()=>snapshot.state.p1_held===1);await page.keyboard.up('w');await page.waitForFunction(()=>snapshot.state.p1_held===0);
  stimulus({diag:{test_keyboard:2,keyboard_down:1,test_matrix:64}});await page.waitForFunction(()=>snapshot.state.p2_held===2&&snapshot.state.p3_held===8);
  stimulus({diag:{test_keyboard:0,keyboard_down:0,test_matrix:0}});await page.waitForFunction(()=>snapshot.state.p2_held===0&&snapshot.state.p3_held===0);
  // No all-zero 10 Hz heartbeat once released.
  await delay(400);const idleStart=commands().length;await delay(1200);assert(!commands().slice(idleStart).some(x=>x.command==='move'));
  await page.locator('#partyModule').screenshot({path:path.join(__dirname,'after_recovery.png')});
  // Calibration is never silently cancelled by game startup.
  stimulus({diag:{input_diag:1,matrix_cal:1,matrix_step:3}});await page.waitForFunction(()=>snapshot.state.matrix_cal===1);assert(await page.locator('#startCoopTouhou').isDisabled());
  assert((await page.locator('#coopHelp').textContent()).includes('先完成并应用映射'));
  stimulus({diag:{matrix_cal:0,matrix_step:16,keyboard_down:1,test_keyboard:1}});await page.waitForFunction(()=>snapshot.state.matrix_cal===0&&snapshot.state.keyboard_down===1);
  let before=coops().length;await page.locator('#startCoopTouhou').click();await idle();assert((await page.locator('#notice').textContent()).includes('松开'));assert(coops().length===before);
  stimulus({diag:{keyboard_down:0,test_keyboard:0,test_reject_exit:1}});await page.waitForFunction(()=>snapshot.state.keyboard_down===0);await delay(200);
  await page.locator('#startCoopTouhou').click();await idle();assert((await page.locator('#notice').textContent()).includes('拒绝'));assert(coops().length===before);
  // An ACK without the requested diagnostic state is not sufficient.
  stimulus({diag:{test_reject_exit:0,test_keep_diag:1}});await delay(500);await page.locator('#startCoopTouhou').click();await idle();assert((await page.locator('#notice').textContent()).includes('尚未确认输入检测已退出'));assert(coops().length===before);
  stimulus({diag:{test_keep_diag:0,test_drop_i9:1}});await delay(500);await page.locator('#startCoopTouhou').click();await idle();assert((await page.locator('#notice').textContent()).includes('超时'));assert(coops().length===before);
  stimulus({diag:{test_drop_i9:0,test_reboot_on_status:1}});await delay(500);await page.locator('#startCoopTouhou').click();await idle();assert((await page.locator('#notice').textContent()).includes('已变化'));assert(coops().length===before);
  await page.locator('#startCoopTouhou').click();await page.waitForFunction(()=>!coopStartBusy&&snapshot.state.scene===0&&snapshot.state.players===3&&snapshot.state.input_diag===0&&document.getElementById('captureKeys').checked);
  assert(coops().length===before+1);
  // Entering diagnostics releases an already held PC key.
  await page.keyboard.down('d');await page.waitForFunction(()=>snapshot.state.p1_held===8);
  stimulus({diag:{input_diag:1}});await page.waitForFunction(()=>snapshot.state.input_diag===1&&snapshot.state.p1_held===0);await page.keyboard.up('d');
  assert(await page.locator('#captureKeys').isDisabled());
  await page.setViewportSize({width:390,height:900});assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
  await page.locator('#partyModule').screenshot({path:path.join(__dirname,'mobile.png')});
  await page.locator('#disconnect').click();await page.waitForFunction(()=>!snapshot.connected);
  assert(!errors.length,errors.join('\n'));
  fs.writeFileSync(path.join(__dirname,'browser_results.json'),JSON.stringify({passed:true,physical_board:false,checks:['reproduced diagnostic lock from supplied CSV','fresh request-tagged status and ACK order','diagnostic exit before co-op','auto-enable PC keyboard capture','PC key down/release serialized','USB and matrix independent source telemetry','calibration preserved','held-key guard','negative ACK stops start','ACK with wrong diagnostic state stops start','missing I9 stops start','firmware reboot aborts pending operation','both game entries','PC held release on diagnostic entry','no idle zero heartbeat','mobile layout'],errors},null,2));
  console.log('PASS co-op diagnostic exit, keyboard capture, exact response guards, failure paths and independent input telemetry (PTY only)');
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exit(1)});
