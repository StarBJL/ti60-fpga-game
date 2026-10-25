const path=require('path'),fs=require('fs'),assert=require('assert');
const {chromium}=require('/home/user/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{const b=await chromium.launch({executablePath:'/opt/google/chrome/chrome',headless:true,args:['--no-sandbox','--disable-dev-shm-usage']}),p=await b.newPage(),errors=[];p.on('pageerror',x=>errors.push(x.message));
for(const width of [1400,390]){await p.setViewportSize({width,height:1000});await p.goto('file://'+path.resolve(__dirname,'../../docs/R58_BOARD_FIX.html'));assert(await p.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));await p.screenshot({path:path.join(__dirname,`report_${width}.png`)});}
assert(!errors.length);await b.close();fs.writeFileSync(path.join(__dirname,'docs_check.json'),JSON.stringify({passed:true,widths:[1400,390],errors},null,2));console.log('PASS R5.8 report responsive browser check');})().catch(e=>{console.error(e);process.exit(1)});
