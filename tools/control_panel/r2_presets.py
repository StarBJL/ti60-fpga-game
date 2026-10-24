"""Repeatable R2 comparison settings, implemented with existing firmware commands."""
BASE = dict(resolution=0, output_w=1280, output_h=720, pending=0, bars=0,
            alpha=1, auto=0, hw=1, submit_mode=2, band=1, cache=1,
            overlap=1, seed_table=1, seed_build=1, sort=1, kpath=1,
            rd_bursts=16, wr_bursts=128, rd_words=64, wide=16781312,
            bgm=0, tone=0, stats=1, verbose=1, profile=1)
PROFILES = {}
for key, scene, count, sync in [
    ('carrot_15000_free',1,15000,0), ('carrot_16000_free',1,16000,0),
    ('carrot_15000_sync',1,15000,1), ('squares_8800_free',2,8800,0),
    ('squares_8900_free',2,8900,0), ('squares_8800_sync',2,8800,1),
]:
    kind='萝卜弹幕' if scene==1 else '运动方块'
    mode='60 帧呈现验证' if sync else '自由吞吐对照'
    expected=dict(BASE,scene=scene,phase=2 if scene==1 else 0,vsync=sync,
                  overlap_active=1 if scene==1 else 0)
    if scene==1:expected['bullet']=13
    else:expected.update(square=0,motion=1,table=1)
    PROFILES[key]=dict(id=key,scene=scene,count=count,vsync=sync,
        label=f'{count:,} · {mode}',group=kind,
        detail=('13px · 自定义压力' if scene==1 else '混合 6–40px · 硬件运动'),
        expected=expected)

# Only fields actually reported per measurement window establish the baseline.
REQUIRED_FIELDS = ('auto','stats','verbose','submit_mode','band','cache','kpath',
                   'seed_table','seed_build','sort','overlap','rd_bursts','wr_bursts','rd_words','wide')
LABELS=dict(scene='场景',phase='运行阶段',resolution='分辨率',output_w='输出宽度',output_h='输出高度',
            pending='切换等待',bars='彩条',alpha='透明合成',auto='自动演示',hw='硬件绘制',
            submit_mode='提交路径',band='分带',cache='源缓存',overlap='逻辑重叠',
            overlap_active='当前场景重叠',seed_table='引擎弹幕表',seed_build='逻辑建表',sort='排序',
            kpath='K 通路',rd_bursts='读突发额度',wr_bursts='写突发额度',rd_words='读字额度',
            wide='宽通路设置',bgm='背景音乐',tone='测试音',stats='屏幕统计',verbose='详细报告',
            profile='测量固件',bullet='弹幕尺寸',square='方块尺寸',motion='硬件运动',table='紧凑表',target='目标数量')

def public_profiles():
    return [{k:v for k,v in p.items() if k!='expected'} for p in PROFILES.values()]

def command_plan(profile):
    # Stop held PC input, restore the accelerated path, and remove stale display,
    # autoplay and audio modes before selecting the workload. No legacy toggles.
    plan=[('stop',None),('fast',None),('resolution',0),('bars',0),('auto',0),
          ('tone',0),('bgm',0),('alpha',1),('vsync',0),('stats',1),
          ('verbose',1),('step',100),('scene',profile['scene'])]
    if profile['scene']==1:plan += [('bullet',13),('stress',4)]
    else:plan += [('square',0),('motion',1),('table',1)]
    return plan+[('count',profile['count']),('vsync',profile['vsync']),('status',None)]

def mismatches(profile, values, include_target=False):
    expected=dict(profile['expected'])
    if include_target:expected['target']=profile['count']
    return [{'field':k,'label':LABELS.get(k,k),'expected':v,'actual':values.get(k)}
            for k,v in expected.items() if values.get(k)!=v]
