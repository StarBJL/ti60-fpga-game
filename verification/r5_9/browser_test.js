const fs=require('fs'),path=require('path'),assert=require('assert');
const {chromium}=require('/home/user/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
 const fixture=JSON.parse(fs.readFileSync(path.join(__dirname,'pty_fixture.json'),'utf8'));
 const browser=await chromium.launch({executablePath:'/opt/google/chrome/chrome',headless:true,args:['--no-sandbox','--disable-dev-shm-usage']});
 try{
 const page=await browser.newPage({viewport:{width:1500,height:1000}}),errors=[];page.on('pageerror',e=>errors.push(e.message));
 await page.goto(fixture.url,{waitUntil:'domcontentloaded'});await page.waitForFunction(()=>typeof token==='string'&&token.length>10);
 await page.locator('#port').fill(fixture.port);await page.locator('#connect').click();
 await page.waitForFunction(()=>snapshot?.state.input_diag_supported===1);
 assert((await page.locator('#consoleVersion').textContent()).includes('R5.9'));
 await page.locator('#matrixCalBegin').click();await page.waitForFunction(()=>snapshot.state.input_diag===1&&snapshot.state.matrix_cal===1);
 assert(await page.locator('#inputDiagOff').isDisabled());
 let nonce=0;function stimulus(value){const q=path.join(__dirname,'stimulus.json');fs.writeFileSync(q+'.tmp',JSON.stringify({...value,nonce:++nonce}));fs.renameSync(q+'.tmp',q);}
 function press(bit){stimulus({bit});}
 const mapping=[];for(let i=0;i<16;i++){const row=Math.floor(i/4);mapping.push((row===1?3:row===3?1:row)*4+i%4);}
 for(let i=0;i<16;i++){
  press(mapping[i]);await page.waitForFunction(n=>snapshot.state.matrix_step===n,i+1);
  if(i===0){press(mapping[i]);await page.waitForFunction(()=>snapshot.state.matrix_cal_error===2);assert((await page.locator('#matrixCalPrompt').textContent()).includes('已录过'));}
  if(i===6)await page.locator('#inputDiagnostics').screenshot({path:path.join(__dirname,'calibration.png')});
 }
 assert(await page.locator('#matrixCalApply').isEnabled());await page.locator('#matrixCalApply').click();await page.waitForFunction(()=>snapshot.state.matrix_mapped===1&&!snapshot.state.matrix_cal);
 assert((await page.locator('#matrixSavedState').textContent()).includes('本机已存'));
 await page.locator('#inputDiagOff').click();await page.waitForFunction(()=>snapshot.state.input_diag===0);
 await page.reload({waitUntil:'domcontentloaded'});await page.waitForFunction(()=>snapshot?.state.input_diag_supported===1);
 await page.locator('#matrixRestore').click();await page.waitForFunction(()=>!matrixRestoreBusy&&snapshot.state.matrix_cal===0&&snapshot.state.input_diag===1,{},{timeout:45000});
 stimulus({keyboard:true});await page.waitForFunction(()=>snapshot.state.keyboard_bytes===22);
 await page.locator('#keyboardSync').click();await page.waitForFunction(()=>snapshot.state.keyboard_acks===1);
 assert((await page.locator('#keyboardByteTail').textContent()).includes('57 AB 01'));
 assert((await page.locator('#keyboardDiagnosticHint').textContent()).includes('已解析'));
 await page.locator('#inputDiagnostics').screenshot({path:path.join(__dirname,'input_diagnostics.png')});
 const download=page.waitForEvent('download');await page.locator('a[href="/api/export?kind=metrics"]').click();await(await download).saveAs(path.join(__dirname,'controls_export.csv'));
 const csv=fs.readFileSync(path.join(__dirname,'controls_export.csv'),'utf8');for(const key of ['matrix_last','matrix_events','matrix_map','keyboard_bytes','keyboard_tail','keyboard_faults'])assert(csv.includes(key));
 await page.setViewportSize({width:390,height:900});assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
 await page.locator('#inputDiagnostics').screenshot({path:path.join(__dirname,'diagnostics_mobile.png')});
 await page.locator('#disconnect').click();await page.waitForFunction(()=>!snapshot.connected);
 await page.goto(fixture.url+'/wiring',{waitUntil:'load'});assert((await page.textContent('body')).includes('20、22')||(await page.textContent('body')).includes('20/22'));
 assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
 await page.setViewportSize({width:1300,height:1000});await page.locator('.diagram').screenshot({path:path.join(__dirname,'wiring.png')});
 assert(!errors.length,errors.join('\n'));
 const commands=JSON.parse(fs.readFileSync(path.join(__dirname,'pty_commands.json'),'utf8'));
 assert(!commands.some(x=>x.command==='resolution'));assert(commands.filter(x=>x.command==='matrix_bind').length===16);
 fs.writeFileSync(path.join(__dirname,'browser_results.json'),JSON.stringify({passed:true,physical_board:false,checks:['16-key step-by-step calibration','duplicate rejection','atomic apply','diagnostic release guard','profile saved across reload','ACK-checked profile restore','raw UART bytes','status sync button','CSV retention','mobile layout','corrected wiring route'],errors},null,2));
 console.log('PASS R5.9 serial PTY, input calibration/restore, diagnostics, CSV and responsive wiring');
 }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exit(1)});
