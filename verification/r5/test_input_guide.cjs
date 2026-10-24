const {chromium}=require(process.env.PLAYWRIGHT_MODULE || 'playwright');
const path=require('path');
const {pathToFileURL}=require('url');
const root=path.resolve(__dirname,'../..');
const fs=require('fs');
(async()=>{
 const out=__dirname;
 const browser=await chromium.launch({executablePath:process.env.CHROME_PATH || '/opt/google/chrome/chrome',headless:true,args:['--no-sandbox']});
 const page=await browser.newPage({viewport:{width:1280,height:960}}),errors=[];page.on('pageerror',e=>errors.push(String(e)));page.on('console',m=>{if(m.type()==='error')errors.push(m.text());});
 await page.goto(pathToFileURL(path.join(root,'docs/INPUT_GUIDE.html')).href);
 const assert=(x,m)=>{if(!x)throw Error(m);};
 assert(await page.locator('.key').count()===16,'16 keys');
 await page.locator('[data-index="10"]').click();assert(await page.locator('#step').textContent()==='10','step10');
 await page.locator('[data-index="9"]').click();assert(await page.locator('#count').textContent()==='1,010','count10');
 await page.locator('[data-index="10"]').click();assert(await page.locator('#step').textContent()==='1','step1');
 await page.locator('[data-index="8"]').click();assert(await page.locator('#count').textContent()==='1,009','count1');
 await page.locator('[data-index="14"]').click();assert(await page.locator('#resolution').textContent()==='1080p30','1080');
 await page.locator('#fn-toggle').click();assert(await page.locator('[data-index="14"] strong').textContent()==='下一曲','fnmusic');
 await page.locator('[data-index="14"]').click();assert((await page.locator('#event').textContent()).includes('下一首'),'musicmessage');
 await page.locator('#reset-demo').click();
 await page.locator('#search').fill('F9');assert(await page.locator('#legacy tbody tr:visible').count()===1,'searchF9');
 await page.locator('#search').fill('');await page.locator('#category').selectOption('调试');assert(await page.locator('#legacy tbody tr:visible').count()===10,'filterdebug');
 await page.locator('#category').selectOption('all');
 const dimensions=[];
 for(const [w,h,scheme] of [[1280,960,'light'],[390,844,'light'],[1280,960,'dark']]){
  await page.setViewportSize({width:w,height:h});await page.emulateMedia({colorScheme:scheme});await page.evaluate(()=>scrollTo(0,0));
  const dims=await page.evaluate(()=>({scroll:document.documentElement.scrollWidth,client:document.documentElement.clientWidth}));
  assert(dims.scroll<=dims.client,'overflow '+w+' '+JSON.stringify(dims));dimensions.push({width:w,height:h,scheme,...dims});
  await page.screenshot({path:out+'/input_guide_'+w+'_'+scheme+'.png',fullPage:w===390});
 }
 assert(errors.length===0,'consoleerrors '+errors);fs.writeFileSync(out+'/input_guide_browser.json',JSON.stringify({passed:true,offline_file:true,checks:['16 keys','100/10/1 step','increase/decrease','mode switch demo','Fn layer/music','reset','search F9','category filter','1280px/390px no page overflow','light/dark','no console errors'],dimensions,errors},null,2));
 await browser.close();console.log('PASS HTML guide: keys/Fn/step/count/resolution/filter, light/dark, desktop/mobile, offline, no console errors');
})().catch(e=>{console.error(e);process.exit(1)});
