"""Repeatable R2 comparison settings, implemented with existing firmware commands.

2026-10-26 adapted to the main project firmware (09_Ti60F225_hardjtag_demo): no 1080P, no
hardware square motion (squares use the FTAB fill table instead), logic overlap is active in
every scene, vsync presets use the vsync band pipeline. BASE_LIMITS are the historical
measurements; LIMITS are the requested 225 MHz targets, not new 60 fps measurements."""
BASE = dict(resolution=0, output_w=1280, output_h=720, pending=0, bars=0,
            alpha=1, auto=0, hw=1, submit_mode=2, band=1, cache=1,
            overlap=1, seed_table=1, seed_build=1, sort=1, kpath=1,
            rd_bursts=16, wr_bursts=128, rd_words=64, wide=16781312,
            bgm=0, tone=0, stats=1, verbose=1, profile=1)
PROFILES = {}
BASE_LIMITS = {
    '萝卜弹幕': [(1,14800),(3,14600),(6,14200),(9,14000),(13,13700),(17,13600),(25,8200)],
    '运动方块': [(1,32000),(3,31000),(6,30000),(8,29000),(12,28000),
               (16,27000),(24,19000),(32,12600),(40,8900),(0,18300)],
}
# 225 MHz requested targets: multiply the original values by 1.1 and floor
# to a multiple of 100. Integer arithmetic avoids floating-point truncation.
# These are requested loads, not newly measured stable-60-FPS results.
LIMITS = {group: [(size, count * 11 // 1000 * 100) for size, count in entries]
          for group, entries in BASE_LIMITS.items()}
# Explicit follow-up request: the 25px bullet target is 11000.
LIMITS['萝卜弹幕'] = [(size, 11000 if size == 25 else count)
                     for size, count in LIMITS['萝卜弹幕']]
for group, entries in LIMITS.items():
    scene = 1 if group == '萝卜弹幕' else 2
    for size, count in entries:
        size_label = f'{size}px' if size else '随机 6–40px'
        key = f'limit_{"carrot" if scene == 1 else "squares"}_{size}'
        expected = dict(BASE, scene=scene, phase=2 if scene == 1 else 0,
                        vsync=1, overlap_active=1, core_hz=225000000)
        if scene == 1:
            expected['bullet'] = size
        else:
            expected.update(square=size, motion=0, table=1)
        PROFILES[key] = dict(id=key, scene=scene, count=count, size=size,
                             size_label=size_label, vsync=1, category='limit',
                             label=f'{size_label} · {count:,} 个', group=group,
                             detail=('225 MHz · 25px目标11,000' if scene == 1 and size == 25
                                     else '225 MHz · 原上限×1.1，向下取整到100'), expected=expected)
for key, scene, count, sync in [
    ('carrot_15000_free',1,15000,0), ('carrot_16000_free',1,16000,0),
    ('carrot_15000_sync',1,15000,1), ('squares_8800_free',2,8800,0),
    ('squares_19000_free',2,19000,0), ('squares_19000_sync',2,19000,1),
]:
    kind='萝卜弹幕' if scene==1 else '运动方块'
    mode='60 帧呈现验证' if sync else '自由吞吐对照'
    expected=dict(BASE,scene=scene,phase=2 if scene==1 else 0,vsync=sync,overlap_active=1)
    if scene==1:expected['bullet']=13
    else:expected.update(square=0,motion=0,table=1)
    PROFILES[key]=dict(id=key,scene=scene,count=count,vsync=sync,
        category='verification' if sync else 'legacy_free',
        label=f'{count:,} · {mode}',group=kind,
        detail=('13px · 自定义压力' if scene==1 else '混合 6–40px · 填充表'),
        expected=expected)

# Only fields actually reported per measurement window establish the baseline.
REQUIRED_FIELDS = ('auto','stats','verbose','submit_mode','band','cache','kpath',
                   'seed_table','seed_build','sort','overlap','rd_bursts','wr_bursts','rd_words','wide')
LABELS=dict(core_hz='核心时钟',scene='场景',phase='运行阶段',resolution='分辨率',output_w='输出宽度',output_h='输出高度',
            pending='切换等待',bars='彩条',alpha='透明合成',auto='自动演示',hw='硬件绘制',
            submit_mode='提交路径',band='分带',cache='源缓存',overlap='逻辑重叠',
            overlap_active='当前场景重叠',seed_table='引擎弹幕表',seed_build='逻辑建表',sort='排序',
            kpath='K 通路',rd_bursts='读突发额度',wr_bursts='写突发额度',rd_words='读字额度',
            wide='宽通路设置',bgm='背景音乐',tone='测试音',stats='屏幕统计',verbose='详细报告',
            profile='测量固件',bullet='弹幕尺寸',square='方块尺寸',motion='硬件运动',table='紧凑表',target='目标数量')

def public_profiles():
    return [{k:v for k,v in p.items() if k!='expected'} for p in PROFILES.values()
            if p['category'] != 'legacy_free']

def command_plan(profile):
    # Stop held PC input, restore the accelerated path, and remove stale display,
    # autoplay and audio modes before selecting the workload. No legacy toggles.
    # (no 'resolution' / 'motion 1': this firmware has neither; 'motion 0' is accepted)
    plan=[('stop',None),('fast',None),('bars',0),('auto',0),
          ('tone',0),('bgm',0),('alpha',1),('vsync',0),('stats',1),
          ('verbose',1),('step',100),('scene',profile['scene'])]
    size = profile.get('size', 13 if profile['scene'] == 1 else 0)
    if profile['scene']==1:plan += [('bullet',size),('stress',4)]
    else:plan += [('square',size),('motion',0),('table',1)]
    return plan+[('count',profile['count']),('vsync',profile['vsync']),('status',None)]

def mismatches(profile, values, include_target=False):
    expected=dict(profile['expected'])
    if include_target:expected['target']=profile['count']
    return [{'field':k,'label':LABELS.get(k,k),'expected':v,'actual':values.get(k)}
            for k,v in expected.items() if values.get(k)!=v]
