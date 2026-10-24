
../../../../../../../../../analysis/flicker_probe/build_clean/game.elf:     file format elf32-littleriscv


Disassembly of section .init:

00001000 <_start>:
    1000:	00013197          	auipc	gp,0x13
    1004:	c0818193          	addi	gp,gp,-1016 # 13c08 <__global_pointer$>

00001008 <init>:
    1008:	000e7117          	auipc	sp,0xe7
    100c:	19810113          	addi	sp,sp,408 # e81a0 <__freertos_irq_stack_top>
    1010:	00010517          	auipc	a0,0x10
    1014:	72c50513          	addi	a0,a0,1836 # 1173c <_data>
    1018:	00010597          	auipc	a1,0x10
    101c:	72458593          	addi	a1,a1,1828 # 1173c <_data>
    1020:	00012617          	auipc	a2,0x12
    1024:	44460613          	addi	a2,a2,1092 # 13464 <bot_wait>
    1028:	00c5fc63          	bgeu	a1,a2,1040 <init+0x38>
    102c:	00052283          	lw	t0,0(a0)
    1030:	0055a023          	sw	t0,0(a1)
    1034:	00450513          	addi	a0,a0,4
    1038:	00458593          	addi	a1,a1,4
    103c:	fec5e8e3          	bltu	a1,a2,102c <init+0x24>
    1040:	00012517          	auipc	a0,0x12
    1044:	42450513          	addi	a0,a0,1060 # 13464 <bot_wait>
    1048:	000e3597          	auipc	a1,0xe3
    104c:	15058593          	addi	a1,a1,336 # e4198 <_end>
    1050:	00b57863          	bgeu	a0,a1,1060 <init+0x58>
    1054:	00052023          	sw	zero,0(a0)
    1058:	00450513          	addi	a0,a0,4
    105c:	feb56ce3          	bltu	a0,a1,1054 <init+0x4c>
    1060:	010000ef          	jal	1070 <__libc_init_array>
    1064:	3ac000ef          	jal	1410 <main>

00001068 <mainDone>:
    1068:	0000006f          	j	1068 <mainDone>

0000106c <_init>:
    106c:	00008067          	ret

Disassembly of section .text:

00001070 <__libc_init_array>:
    1070:	ff010113          	addi	sp,sp,-16
    1074:	00812423          	sw	s0,8(sp)
    1078:	01212023          	sw	s2,0(sp)
    107c:	00010797          	auipc	a5,0x10
    1080:	6c078793          	addi	a5,a5,1728 # 1173c <_data>
    1084:	00010417          	auipc	s0,0x10
    1088:	6b840413          	addi	s0,s0,1720 # 1173c <_data>
    108c:	00112623          	sw	ra,12(sp)
    1090:	00912223          	sw	s1,4(sp)
    1094:	40878933          	sub	s2,a5,s0
    1098:	02878063          	beq	a5,s0,10b8 <__libc_init_array+0x48>
    109c:	40295913          	srai	s2,s2,0x2
    10a0:	00000493          	li	s1,0
    10a4:	00042783          	lw	a5,0(s0)
    10a8:	00148493          	addi	s1,s1,1
    10ac:	00440413          	addi	s0,s0,4
    10b0:	000780e7          	jalr	a5
    10b4:	ff24e8e3          	bltu	s1,s2,10a4 <__libc_init_array+0x34>
    10b8:	00010797          	auipc	a5,0x10
    10bc:	68478793          	addi	a5,a5,1668 # 1173c <_data>
    10c0:	00010417          	auipc	s0,0x10
    10c4:	67c40413          	addi	s0,s0,1660 # 1173c <_data>
    10c8:	40878933          	sub	s2,a5,s0
    10cc:	40295913          	srai	s2,s2,0x2
    10d0:	00878e63          	beq	a5,s0,10ec <__libc_init_array+0x7c>
    10d4:	00000493          	li	s1,0
    10d8:	00042783          	lw	a5,0(s0)
    10dc:	00148493          	addi	s1,s1,1
    10e0:	00440413          	addi	s0,s0,4
    10e4:	000780e7          	jalr	a5
    10e8:	ff24e8e3          	bltu	s1,s2,10d8 <__libc_init_array+0x68>
    10ec:	00c12083          	lw	ra,12(sp)
    10f0:	00812403          	lw	s0,8(sp)
    10f4:	00412483          	lw	s1,4(sp)
    10f8:	00012903          	lw	s2,0(sp)
    10fc:	01010113          	addi	sp,sp,16
    1100:	00008067          	ret

00001104 <memset>:
    1104:	00f00313          	li	t1,15
    1108:	00050713          	mv	a4,a0
    110c:	02c37e63          	bgeu	t1,a2,1148 <memset+0x44>
    1110:	00f77793          	andi	a5,a4,15
    1114:	0a079063          	bnez	a5,11b4 <memset+0xb0>
    1118:	08059263          	bnez	a1,119c <memset+0x98>
    111c:	ff067693          	andi	a3,a2,-16
    1120:	00f67613          	andi	a2,a2,15
    1124:	00e686b3          	add	a3,a3,a4
    1128:	00b72023          	sw	a1,0(a4)
    112c:	00b72223          	sw	a1,4(a4)
    1130:	00b72423          	sw	a1,8(a4)
    1134:	00b72623          	sw	a1,12(a4)
    1138:	01070713          	addi	a4,a4,16
    113c:	fed766e3          	bltu	a4,a3,1128 <memset+0x24>
    1140:	00061463          	bnez	a2,1148 <memset+0x44>
    1144:	00008067          	ret
    1148:	40c306b3          	sub	a3,t1,a2
    114c:	00269693          	slli	a3,a3,0x2
    1150:	00000297          	auipc	t0,0x0
    1154:	005686b3          	add	a3,a3,t0
    1158:	00c68067          	jr	12(a3)
    115c:	00b70723          	sb	a1,14(a4)
    1160:	00b706a3          	sb	a1,13(a4)
    1164:	00b70623          	sb	a1,12(a4)
    1168:	00b705a3          	sb	a1,11(a4)
    116c:	00b70523          	sb	a1,10(a4)
    1170:	00b704a3          	sb	a1,9(a4)
    1174:	00b70423          	sb	a1,8(a4)
    1178:	00b703a3          	sb	a1,7(a4)
    117c:	00b70323          	sb	a1,6(a4)
    1180:	00b702a3          	sb	a1,5(a4)
    1184:	00b70223          	sb	a1,4(a4)
    1188:	00b701a3          	sb	a1,3(a4)
    118c:	00b70123          	sb	a1,2(a4)
    1190:	00b700a3          	sb	a1,1(a4)
    1194:	00b70023          	sb	a1,0(a4)
    1198:	00008067          	ret
    119c:	0ff5f593          	zext.b	a1,a1
    11a0:	00859693          	slli	a3,a1,0x8
    11a4:	00d5e5b3          	or	a1,a1,a3
    11a8:	01059693          	slli	a3,a1,0x10
    11ac:	00d5e5b3          	or	a1,a1,a3
    11b0:	f6dff06f          	j	111c <memset+0x18>
    11b4:	00279693          	slli	a3,a5,0x2
    11b8:	00000297          	auipc	t0,0x0
    11bc:	005686b3          	add	a3,a3,t0
    11c0:	00008293          	mv	t0,ra
    11c4:	fa0680e7          	jalr	-96(a3)
    11c8:	00028093          	mv	ra,t0
    11cc:	ff078793          	addi	a5,a5,-16
    11d0:	40f70733          	sub	a4,a4,a5
    11d4:	00f60633          	add	a2,a2,a5
    11d8:	f6c378e3          	bgeu	t1,a2,1148 <memset+0x44>
    11dc:	f3dff06f          	j	1118 <memset+0x14>

000011e0 <memcpy>:
    11e0:	00a5c7b3          	xor	a5,a1,a0
    11e4:	0037f793          	andi	a5,a5,3
    11e8:	00c508b3          	add	a7,a0,a2
    11ec:	06079463          	bnez	a5,1254 <memcpy+0x74>
    11f0:	00300793          	li	a5,3
    11f4:	06c7f063          	bgeu	a5,a2,1254 <memcpy+0x74>
    11f8:	00357793          	andi	a5,a0,3
    11fc:	00050713          	mv	a4,a0
    1200:	06079a63          	bnez	a5,1274 <memcpy+0x94>
    1204:	ffc8f613          	andi	a2,a7,-4
    1208:	40e606b3          	sub	a3,a2,a4
    120c:	02000793          	li	a5,32
    1210:	08d7ce63          	blt	a5,a3,12ac <memcpy+0xcc>
    1214:	00058693          	mv	a3,a1
    1218:	00070793          	mv	a5,a4
    121c:	02c77863          	bgeu	a4,a2,124c <memcpy+0x6c>
    1220:	0006a803          	lw	a6,0(a3)
    1224:	00478793          	addi	a5,a5,4
    1228:	00468693          	addi	a3,a3,4
    122c:	ff07ae23          	sw	a6,-4(a5)
    1230:	fec7e8e3          	bltu	a5,a2,1220 <memcpy+0x40>
    1234:	fff60793          	addi	a5,a2,-1
    1238:	40e787b3          	sub	a5,a5,a4
    123c:	ffc7f793          	andi	a5,a5,-4
    1240:	00478793          	addi	a5,a5,4
    1244:	00f70733          	add	a4,a4,a5
    1248:	00f585b3          	add	a1,a1,a5
    124c:	01176863          	bltu	a4,a7,125c <memcpy+0x7c>
    1250:	00008067          	ret
    1254:	00050713          	mv	a4,a0
    1258:	05157863          	bgeu	a0,a7,12a8 <memcpy+0xc8>
    125c:	0005c783          	lbu	a5,0(a1)
    1260:	00170713          	addi	a4,a4,1
    1264:	00158593          	addi	a1,a1,1
    1268:	fef70fa3          	sb	a5,-1(a4)
    126c:	fee898e3          	bne	a7,a4,125c <memcpy+0x7c>
    1270:	00008067          	ret
    1274:	0005c683          	lbu	a3,0(a1)
    1278:	00170713          	addi	a4,a4,1
    127c:	00377793          	andi	a5,a4,3
    1280:	fed70fa3          	sb	a3,-1(a4)
    1284:	00158593          	addi	a1,a1,1
    1288:	f6078ee3          	beqz	a5,1204 <memcpy+0x24>
    128c:	0005c683          	lbu	a3,0(a1)
    1290:	00170713          	addi	a4,a4,1
    1294:	00377793          	andi	a5,a4,3
    1298:	fed70fa3          	sb	a3,-1(a4)
    129c:	00158593          	addi	a1,a1,1
    12a0:	fc079ae3          	bnez	a5,1274 <memcpy+0x94>
    12a4:	f61ff06f          	j	1204 <memcpy+0x24>
    12a8:	00008067          	ret
    12ac:	ff010113          	addi	sp,sp,-16
    12b0:	00812623          	sw	s0,12(sp)
    12b4:	02000413          	li	s0,32
    12b8:	0005a383          	lw	t2,0(a1)
    12bc:	0045a283          	lw	t0,4(a1)
    12c0:	0085af83          	lw	t6,8(a1)
    12c4:	00c5af03          	lw	t5,12(a1)
    12c8:	0105ae83          	lw	t4,16(a1)
    12cc:	0145ae03          	lw	t3,20(a1)
    12d0:	0185a303          	lw	t1,24(a1)
    12d4:	01c5a803          	lw	a6,28(a1)
    12d8:	0205a683          	lw	a3,32(a1)
    12dc:	02470713          	addi	a4,a4,36
    12e0:	40e607b3          	sub	a5,a2,a4
    12e4:	fc772e23          	sw	t2,-36(a4)
    12e8:	fe572023          	sw	t0,-32(a4)
    12ec:	fff72223          	sw	t6,-28(a4)
    12f0:	ffe72423          	sw	t5,-24(a4)
    12f4:	ffd72623          	sw	t4,-20(a4)
    12f8:	ffc72823          	sw	t3,-16(a4)
    12fc:	fe672a23          	sw	t1,-12(a4)
    1300:	ff072c23          	sw	a6,-8(a4)
    1304:	fed72e23          	sw	a3,-4(a4)
    1308:	02458593          	addi	a1,a1,36
    130c:	faf446e3          	blt	s0,a5,12b8 <memcpy+0xd8>
    1310:	00058693          	mv	a3,a1
    1314:	00070793          	mv	a5,a4
    1318:	02c77863          	bgeu	a4,a2,1348 <memcpy+0x168>
    131c:	0006a803          	lw	a6,0(a3)
    1320:	00478793          	addi	a5,a5,4
    1324:	00468693          	addi	a3,a3,4
    1328:	ff07ae23          	sw	a6,-4(a5)
    132c:	fec7e8e3          	bltu	a5,a2,131c <memcpy+0x13c>
    1330:	fff60793          	addi	a5,a2,-1
    1334:	40e787b3          	sub	a5,a5,a4
    1338:	ffc7f793          	andi	a5,a5,-4
    133c:	00478793          	addi	a5,a5,4
    1340:	00f70733          	add	a4,a4,a5
    1344:	00f585b3          	add	a1,a1,a5
    1348:	01176863          	bltu	a4,a7,1358 <memcpy+0x178>
    134c:	00c12403          	lw	s0,12(sp)
    1350:	01010113          	addi	sp,sp,16
    1354:	00008067          	ret
    1358:	0005c783          	lbu	a5,0(a1)
    135c:	00170713          	addi	a4,a4,1
    1360:	00158593          	addi	a1,a1,1
    1364:	fef70fa3          	sb	a5,-1(a4)
    1368:	fee882e3          	beq	a7,a4,134c <memcpy+0x16c>
    136c:	0005c783          	lbu	a5,0(a1)
    1370:	00170713          	addi	a4,a4,1
    1374:	00158593          	addi	a1,a1,1
    1378:	fef70fa3          	sb	a5,-1(a4)
    137c:	fce89ee3          	bne	a7,a4,1358 <memcpy+0x178>
    1380:	fcdff06f          	j	134c <memcpy+0x16c>

00001384 <strlen>:
    1384:	00357793          	andi	a5,a0,3
    1388:	00050713          	mv	a4,a0
    138c:	04079c63          	bnez	a5,13e4 <strlen+0x60>
    1390:	7f7f86b7          	lui	a3,0x7f7f8
    1394:	f7f68693          	addi	a3,a3,-129 # 7f7f7f7f <__freertos_irq_stack_top+0x7f70fddf>
    1398:	fff00593          	li	a1,-1
    139c:	00072603          	lw	a2,0(a4)
    13a0:	00470713          	addi	a4,a4,4
    13a4:	00d677b3          	and	a5,a2,a3
    13a8:	00d787b3          	add	a5,a5,a3
    13ac:	00c7e7b3          	or	a5,a5,a2
    13b0:	00d7e7b3          	or	a5,a5,a3
    13b4:	feb784e3          	beq	a5,a1,139c <strlen+0x18>
    13b8:	ffc74683          	lbu	a3,-4(a4)
    13bc:	40a707b3          	sub	a5,a4,a0
    13c0:	04068463          	beqz	a3,1408 <strlen+0x84>
    13c4:	ffd74683          	lbu	a3,-3(a4)
    13c8:	02068c63          	beqz	a3,1400 <strlen+0x7c>
    13cc:	ffe74503          	lbu	a0,-2(a4)
    13d0:	00a03533          	snez	a0,a0
    13d4:	00f50533          	add	a0,a0,a5
    13d8:	ffe50513          	addi	a0,a0,-2
    13dc:	00008067          	ret
    13e0:	fa0688e3          	beqz	a3,1390 <strlen+0xc>
    13e4:	00074783          	lbu	a5,0(a4)
    13e8:	00170713          	addi	a4,a4,1
    13ec:	00377693          	andi	a3,a4,3
    13f0:	fe0798e3          	bnez	a5,13e0 <strlen+0x5c>
    13f4:	40a70733          	sub	a4,a4,a0
    13f8:	fff70513          	addi	a0,a4,-1
    13fc:	00008067          	ret
    1400:	ffd78513          	addi	a0,a5,-3
    1404:	00008067          	ret
    1408:	ffc78513          	addi	a0,a5,-4
    140c:	00008067          	ret

00001410 <main>:
    1410:	d4010113          	addi	sp,sp,-704
    1414:	2a112e23          	sw	ra,700(sp)
    1418:	2a812c23          	sw	s0,696(sp)
    141c:	2a912a23          	sw	s1,692(sp)
    1420:	2b212823          	sw	s2,688(sp)
    1424:	2b312623          	sw	s3,684(sp)
    1428:	2b412423          	sw	s4,680(sp)
    142c:	2b512223          	sw	s5,676(sp)
    1430:	2b612023          	sw	s6,672(sp)
    1434:	29712e23          	sw	s7,668(sp)
    1438:	29812c23          	sw	s8,664(sp)
    143c:	29912a23          	sw	s9,660(sp)
    1440:	29a12823          	sw	s10,656(sp)
    1444:	29b12623          	sw	s11,652(sp)
    1448:	06b00693          	li	a3,107
    144c:	f8010737          	lui	a4,0xf8010
    1450:	00d72423          	sw	a3,8(a4) # f8010008 <__freertos_irq_stack_top+0xf7f27e68>
    1454:	f80107b7          	lui	a5,0xf8010
    1458:	00700713          	li	a4,7
    145c:	00012537          	lui	a0,0x12
    1460:	00e7a623          	sw	a4,12(a5) # f801000c <__freertos_irq_stack_top+0xf7f27e6c>
    1464:	06400593          	li	a1,100
    1468:	07050513          	addi	a0,a0,112 # 12070 <_data+0x934>
    146c:	5e1080ef          	jal	a24c <bsp_printf>
    1470:	f81007b7          	lui	a5,0xf8100
    1474:	01c7a683          	lw	a3,28(a5) # f810001c <__freertos_irq_stack_top+0xf8017e7c>
    1478:	48445737          	lui	a4,0x48445
    147c:	d4970713          	addi	a4,a4,-695 # 48444d49 <__freertos_irq_stack_top+0x4835cba9>
    1480:	00e69463          	bne	a3,a4,1488 <main+0x78>
    1484:	6d40106f          	j	2b58 <main+0x1748>
    1488:	01c7a583          	lw	a1,28(a5)
    148c:	00012537          	lui	a0,0x12
    1490:	01c78793          	addi	a5,a5,28
    1494:	0bc50513          	addi	a0,a0,188 # 120bc <_data+0x980>
    1498:	5b5080ef          	jal	a24c <bsp_printf>
    149c:	7f40f0ef          	jal	10c90 <blit_init>
    14a0:	86a1aa23          	sw	a0,-1932(gp) # 1347c <hw_avail>
    14a4:	40050ee3          	beqz	a0,20c0 <main+0xcb0>
    14a8:	000e4c37          	lui	s8,0xe4
    14ac:	f81017b7          	lui	a5,0xf8101
    14b0:	154c0c13          	addi	s8,s8,340 # e4154 <blit_st>
    14b4:	00c7a583          	lw	a1,12(a5) # f810100c <__freertos_irq_stack_top+0xf8018e6c>
    14b8:	02cc2603          	lw	a2,44(s8)
    14bc:	00012537          	lui	a0,0x12
    14c0:	13050513          	addi	a0,a0,304 # 12130 <_data+0x9f4>
    14c4:	589080ef          	jal	a24c <bsp_printf>
    14c8:	00012537          	lui	a0,0x12
    14cc:	016007b7          	lui	a5,0x1600
    14d0:	00096737          	lui	a4,0x96
    14d4:	014006b7          	lui	a3,0x1400
    14d8:	01200637          	lui	a2,0x1200
    14dc:	010005b7          	lui	a1,0x1000
    14e0:	1b050513          	addi	a0,a0,432 # 121b0 <_data+0xa74>
    14e4:	569080ef          	jal	a24c <bsp_printf>
    14e8:	00012537          	lui	a0,0x12
    14ec:	00100713          	li	a4,1
    14f0:	00100693          	li	a3,1
    14f4:	00100613          	li	a2,1
    14f8:	00100593          	li	a1,1
    14fc:	1f850513          	addi	a0,a0,504 # 121f8 <_data+0xabc>
    1500:	54d080ef          	jal	a24c <bsp_printf>
    1504:	00096637          	lui	a2,0x96
    1508:	00000593          	li	a1,0
    150c:	01000537          	lui	a0,0x1000
    1510:	bf5ff0ef          	jal	1104 <memset>
    1514:	00096637          	lui	a2,0x96
    1518:	00000593          	li	a1,0
    151c:	01200537          	lui	a0,0x1200
    1520:	be5ff0ef          	jal	1104 <memset>
    1524:	00096637          	lui	a2,0x96
    1528:	00000593          	li	a1,0
    152c:	01400537          	lui	a0,0x1400
    1530:	bd5ff0ef          	jal	1104 <memset>
    1534:	00012537          	lui	a0,0x12
    1538:	23c50513          	addi	a0,a0,572 # 1223c <_data+0xb00>
    153c:	511080ef          	jal	a24c <bsp_printf>
    1540:	13810593          	addi	a1,sp,312
    1544:	00000513          	li	a0,0
    1548:	060090ef          	jal	a5a8 <bandwidth_test.constprop.0>
    154c:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    1550:	00078463          	beqz	a5,1558 <main+0x148>
    1554:	6840106f          	j	2bd8 <main+0x17c8>
    1558:	00000d13          	li	s10,0
    155c:	f8010cb7          	lui	s9,0xf8010
    1560:	00013ab7          	lui	s5,0x13
    1564:	00013a37          	lui	s4,0x13
    1568:	010d0793          	addi	a5,s10,16
    156c:	000ae9b7          	lui	s3,0xae
    1570:	000018b7          	lui	a7,0x1
    1574:	040a8a93          	addi	s5,s5,64 # 13040 <bench_name>
    1578:	13810413          	addi	s0,sp,312
    157c:	030a0a13          	addi	s4,s4,48 # 13030 <bench_bytes>
    1580:	000d0d93          	mv	s11,s10
    1584:	02f12423          	sw	a5,40(sp)
    1588:	f5098993          	addi	s3,s3,-176 # adf50 <tx_buf>
    158c:	00a00b13          	li	s6,10
    1590:	004c8c93          	addi	s9,s9,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    1594:	fff88b93          	addi	s7,a7,-1 # fff <CUSTOM2+0xfa4>
    1598:	00042703          	lw	a4,0(s0)
    159c:	000a2803          	lw	a6,0(s4)
    15a0:	000aa583          	lw	a1,0(s5)
    15a4:	000127b7          	lui	a5,0x12
    15a8:	02e856b3          	divu	a3,a6,a4
    15ac:	00070613          	mv	a2,a4
    15b0:	28478513          	addi	a0,a5,644 # 12284 <_data+0xb48>
    15b4:	02e12823          	sw	a4,48(sp)
    15b8:	03012623          	sw	a6,44(sp)
    15bc:	410090ef          	jal	a9cc <tx_printf>
    15c0:	320d00e3          	beqz	s10,20e0 <main+0xcd0>
    15c4:	03012703          	lw	a4,48(sp)
    15c8:	000da603          	lw	a2,0(s11)
    15cc:	02c12803          	lw	a6,44(sp)
    15d0:	00271793          	slli	a5,a4,0x2
    15d4:	00e786b3          	add	a3,a5,a4
    15d8:	00169693          	slli	a3,a3,0x1
    15dc:	02c6d6b3          	divu	a3,a3,a2
    15e0:	00060593          	mv	a1,a2
    15e4:	000127b7          	lui	a5,0x12
    15e8:	2a078513          	addi	a0,a5,672 # 122a0 <_data+0xb64>
    15ec:	0366f733          	remu	a4,a3,s6
    15f0:	02c85633          	divu	a2,a6,a2
    15f4:	0366d6b3          	divu	a3,a3,s6
    15f8:	3d4090ef          	jal	a9cc <tx_printf>
    15fc:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    1600:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    1604:	f8010637          	lui	a2,0xf8010
    1608:	06f70463          	beq	a4,a5,1670 <main+0x260>
    160c:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1610:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    1614:	04f70863          	beq	a4,a5,1664 <main+0x254>
    1618:	000ca783          	lw	a5,0(s9)
    161c:	0107d713          	srli	a4,a5,0x10
    1620:	0ff77713          	zext.b	a4,a4
    1624:	04070063          	beqz	a4,1664 <main+0x254>
    1628:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    162c:	00f987b3          	add	a5,s3,a5
    1630:	0007c683          	lbu	a3,0(a5)
    1634:	000ca783          	lw	a5,0(s9)
    1638:	0107d713          	srli	a4,a5,0x10
    163c:	0ff77713          	zext.b	a4,a4
    1640:	fe070ae3          	beqz	a4,1634 <main+0x224>
    1644:	00d62023          	sw	a3,0(a2) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    1648:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    164c:	00178793          	addi	a5,a5,1
    1650:	0177f7b3          	and	a5,a5,s7
    1654:	88f1a823          	sw	a5,-1904(gp) # 13498 <tx_tail>
    1658:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    165c:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    1660:	faf71ce3          	bne	a4,a5,1618 <main+0x208>
    1664:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    1668:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    166c:	faf710e3          	bne	a4,a5,160c <main+0x1fc>
    1670:	02812783          	lw	a5,40(sp)
    1674:	004d8d93          	addi	s11,s11,4
    1678:	004a8a93          	addi	s5,s5,4
    167c:	00440413          	addi	s0,s0,4
    1680:	004a0a13          	addi	s4,s4,4
    1684:	f0fd9ae3          	bne	s11,a5,1598 <main+0x188>
    1688:	4e1010ef          	jal	3368 <render_init>
    168c:	01600537          	lui	a0,0x1600
    1690:	7f50d0ef          	jal	f684 <demo_init>
    1694:	000af7b7          	lui	a5,0xaf
    1698:	00c105b7          	lui	a1,0xc10
    169c:	f5078513          	addi	a0,a5,-176 # aef50 <game>
    16a0:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    16a4:	55c0b0ef          	jal	cc00 <game_init>
    16a8:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    16ac:	00096637          	lui	a2,0x96
    16b0:	00f037b3          	snez	a5,a5
    16b4:	00000593          	li	a1,0
    16b8:	01000537          	lui	a0,0x1000
    16bc:	86f1a823          	sw	a5,-1936(gp) # 13478 <use_hw>
    16c0:	a45ff0ef          	jal	1104 <memset>
    16c4:	0ff0000f          	fence
    16c8:	f81007b7          	lui	a5,0xf8100
    16cc:	01000737          	lui	a4,0x1000
    16d0:	00e7a223          	sw	a4,4(a5) # f8100004 <__freertos_irq_stack_top+0xf8017e64>
    16d4:	00100693          	li	a3,1
    16d8:	f8100737          	lui	a4,0xf8100
    16dc:	00012537          	lui	a0,0x12
    16e0:	00d72023          	sw	a3,0(a4) # f8100000 <__freertos_irq_stack_top+0xf8017e60>
    16e4:	f2c50513          	addi	a0,a0,-212 # 11f2c <_data+0x7f0>
    16e8:	629080ef          	jal	a510 <tx_puts>
    16ec:	00012537          	lui	a0,0x12
    16f0:	2c450513          	addi	a0,a0,708 # 122c4 <_data+0xb88>
    16f4:	61d080ef          	jal	a510 <tx_puts>
    16f8:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    16fc:	00079463          	bnez	a5,1704 <main+0x2f4>
    1700:	4240106f          	j	2b24 <main+0x1714>
    1704:	000127b7          	lui	a5,0x12
    1708:	00012737          	lui	a4,0x12
    170c:	e7078693          	addi	a3,a5,-400 # 11e70 <_data+0x734>
    1710:	e7078513          	addi	a0,a5,-400
    1714:	e8c70793          	addi	a5,a4,-372 # 11e8c <_data+0x750>
    1718:	06d12023          	sw	a3,96(sp)
    171c:	06f12223          	sw	a5,100(sp)
    1720:	5f1080ef          	jal	a510 <tx_puts>
    1724:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    1728:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    172c:	06f70e63          	beq	a4,a5,17a8 <main+0x398>
    1730:	f8010737          	lui	a4,0xf8010
    1734:	000015b7          	lui	a1,0x1
    1738:	00470713          	addi	a4,a4,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    173c:	f8010537          	lui	a0,0xf8010
    1740:	fff58593          	addi	a1,a1,-1 # fff <CUSTOM2+0xfa4>
    1744:	8901a683          	lw	a3,-1904(gp) # 13498 <tx_tail>
    1748:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    174c:	04f68863          	beq	a3,a5,179c <main+0x38c>
    1750:	00072783          	lw	a5,0(a4)
    1754:	0107d693          	srli	a3,a5,0x10
    1758:	0ff6f693          	zext.b	a3,a3
    175c:	04068063          	beqz	a3,179c <main+0x38c>
    1760:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1764:	00f987b3          	add	a5,s3,a5
    1768:	0007c603          	lbu	a2,0(a5)
    176c:	00072783          	lw	a5,0(a4)
    1770:	0107d693          	srli	a3,a5,0x10
    1774:	0ff6f693          	zext.b	a3,a3
    1778:	fe068ae3          	beqz	a3,176c <main+0x35c>
    177c:	00c52023          	sw	a2,0(a0) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    1780:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1784:	00178793          	addi	a5,a5,1
    1788:	00b7f7b3          	and	a5,a5,a1
    178c:	88f1a823          	sw	a5,-1904(gp) # 13498 <tx_tail>
    1790:	8901a683          	lw	a3,-1904(gp) # 13498 <tx_tail>
    1794:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    1798:	faf69ce3          	bne	a3,a5,1750 <main+0x340>
    179c:	8901a683          	lw	a3,-1904(gp) # 13498 <tx_tail>
    17a0:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    17a4:	faf690e3          	bne	a3,a5,1744 <main+0x334>
    17a8:	06800613          	li	a2,104
    17ac:	00000593          	li	a1,0
    17b0:	1a010513          	addi	a0,sp,416
    17b4:	951ff0ef          	jal	1104 <memset>
    17b8:	8201a583          	lw	a1,-2016(gp) # 13428 <show_stats>
    17bc:	02cc2703          	lw	a4,44(s8)
    17c0:	81c1a603          	lw	a2,-2020(gp) # 13424 <alpha_fx>
    17c4:	f8b0cbb7          	lui	s7,0xf8b0c
    17c8:	8741a683          	lw	a3,-1932(gp) # 1347c <hw_avail>
    17cc:	16012c23          	sw	zero,376(sp)
    17d0:	fff00793          	li	a5,-1
    17d4:	16012e23          	sw	zero,380(sp)
    17d8:	18012023          	sw	zero,384(sp)
    17dc:	18012223          	sw	zero,388(sp)
    17e0:	18012423          	sw	zero,392(sp)
    17e4:	18012623          	sw	zero,396(sp)
    17e8:	18012823          	sw	zero,400(sp)
    17ec:	18012a23          	sw	zero,404(sp)
    17f0:	18012c23          	sw	zero,408(sp)
    17f4:	18012e23          	sw	zero,412(sp)
    17f8:	ff8ba503          	lw	a0,-8(s7) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    17fc:	1af12c23          	sw	a5,440(sp)
    1800:	1af12e23          	sw	a5,444(sp)
    1804:	1ee12e23          	sw	a4,508(sp)
    1808:	00100793          	li	a5,1
    180c:	86f1a223          	sw	a5,-1948(gp) # 1346c <stats_reset>
    1810:	000ae737          	lui	a4,0xae
    1814:	f0870b13          	addi	s6,a4,-248 # adf08 <hud_w>
    1818:	000ae737          	lui	a4,0xae
    181c:	ec070a93          	addi	s5,a4,-320 # adec0 <uart_w>
    1820:	00100713          	li	a4,1
    1824:	04e12023          	sw	a4,64(sp)
    1828:	00013737          	lui	a4,0x13
    182c:	05070713          	addi	a4,a4,80 # 13050 <fb_addr>
    1830:	02e12423          	sw	a4,40(sp)
    1834:	000af737          	lui	a4,0xaf
    1838:	f5070713          	addi	a4,a4,-176 # aef50 <game>
    183c:	04e12423          	sw	a4,72(sp)
    1840:	00013737          	lui	a4,0x13
    1844:	00870713          	addi	a4,a4,8 # 13008 <gfx_hw_ops+0x26c>
    1848:	06e12623          	sw	a4,108(sp)
    184c:	00013737          	lui	a4,0x13
    1850:	ff470713          	addi	a4,a4,-12 # 12ff4 <gfx_hw_ops+0x258>
    1854:	06e12823          	sw	a4,112(sp)
    1858:	000af737          	lui	a4,0xaf
    185c:	00870713          	addi	a4,a4,8 # af008 <game+0xb8>
    1860:	02a12c23          	sw	a0,56(sp)
    1864:	1ab12a23          	sw	a1,436(sp)
    1868:	1cc12023          	sw	a2,448(sp)
    186c:	1cd12e23          	sw	a3,476(sp)
    1870:	02012e23          	sw	zero,60(sp)
    1874:	04012823          	sw	zero,80(sp)
    1878:	02012623          	sw	zero,44(sp)
    187c:	02012823          	sw	zero,48(sp)
    1880:	06e12a23          	sw	a4,116(sp)
    1884:	000b1737          	lui	a4,0xb1
    1888:	10870713          	addi	a4,a4,264 # b1108 <game+0x21b8>
    188c:	04e12223          	sw	a4,68(sp)
    1890:	000d2737          	lui	a4,0xd2
    1894:	52470713          	addi	a4,a4,1316 # d2524 <game+0x235d4>
    1898:	08e12423          	sw	a4,136(sp)
    189c:	00013737          	lui	a4,0x13
    18a0:	02070713          	addi	a4,a4,32 # 13020 <wander.0>
    18a4:	06e12423          	sw	a4,104(sp)
    18a8:	00012737          	lui	a4,0x12
    18ac:	b9470713          	addi	a4,a4,-1132 # 11b94 <_data+0x458>
    18b0:	06e12c23          	sw	a4,120(sp)
    18b4:	00012737          	lui	a4,0x12
    18b8:	b7070713          	addi	a4,a4,-1168 # 11b70 <_data+0x434>
    18bc:	04e12a23          	sw	a4,84(sp)
    18c0:	00012737          	lui	a4,0x12
    18c4:	05470713          	addi	a4,a4,84 # 12054 <_data+0x918>
    18c8:	06e12e23          	sw	a4,124(sp)
    18cc:	00012737          	lui	a4,0x12
    18d0:	05c70713          	addi	a4,a4,92 # 1205c <_data+0x920>
    18d4:	04e12c23          	sw	a4,88(sp)
    18d8:	00012737          	lui	a4,0x12
    18dc:	06470713          	addi	a4,a4,100 # 12064 <_data+0x928>
    18e0:	08e12023          	sw	a4,128(sp)
    18e4:	00012737          	lui	a4,0x12
    18e8:	ee070713          	addi	a4,a4,-288 # 11ee0 <_data+0x7a4>
    18ec:	04e12623          	sw	a4,76(sp)
    18f0:	00012737          	lui	a4,0x12
    18f4:	06c70713          	addi	a4,a4,108 # 1206c <_data+0x930>
    18f8:	08e12223          	sw	a4,132(sp)
    18fc:	00012737          	lui	a4,0x12
    1900:	d6c70713          	addi	a4,a4,-660 # 11d6c <_data+0x630>
    1904:	f8010437          	lui	s0,0xf8010
    1908:	00001a37          	lui	s4,0x1
    190c:	04e12e23          	sw	a4,92(sp)
    1910:	ff8b8b93          	addi	s7,s7,-8
    1914:	01000db7          	lui	s11,0x1000
    1918:	00440413          	addi	s0,s0,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    191c:	fffa0a13          	addi	s4,s4,-1 # fff <CUSTOM2+0xfa4>
    1920:	6a079863          	bnez	a5,1fd0 <main+0xbc0>
    1924:	14012c23          	sw	zero,344(sp)
    1928:	14012e23          	sw	zero,348(sp)
    192c:	16012023          	sw	zero,352(sp)
    1930:	16012223          	sw	zero,356(sp)
    1934:	16012423          	sw	zero,360(sp)
    1938:	16012623          	sw	zero,364(sp)
    193c:	16012823          	sw	zero,368(sp)
    1940:	16012a23          	sw	zero,372(sp)
    1944:	8241a783          	lw	a5,-2012(gp) # 1342c <vsync>
    1948:	000ba683          	lw	a3,0(s7)
    194c:	2a079ae3          	bnez	a5,2400 <main+0xff0>
    1950:	04012703          	lw	a4,64(sp)
    1954:	00300793          	li	a5,3
    1958:	000ba603          	lw	a2,0(s7)
    195c:	02f777b3          	remu	a5,a4,a5
    1960:	f8100737          	lui	a4,0xf8100
    1964:	40d606b3          	sub	a3,a2,a3
    1968:	02812603          	lw	a2,40(sp)
    196c:	14d12c23          	sw	a3,344(sp)
    1970:	00872683          	lw	a3,8(a4) # f8100008 <__freertos_irq_stack_top+0xf8017e68>
    1974:	00279713          	slli	a4,a5,0x2
    1978:	00e60733          	add	a4,a2,a4
    197c:	00072703          	lw	a4,0(a4)
    1980:	00e68463          	beq	a3,a4,1988 <main+0x578>
    1984:	44ed9ae3          	bne	s11,a4,25d8 <main+0x11c8>
    1988:	02c12783          	lw	a5,44(sp)
    198c:	00300713          	li	a4,3
    1990:	02812603          	lw	a2,40(sp)
    1994:	00278793          	addi	a5,a5,2
    1998:	02e7f7b3          	remu	a5,a5,a4
    199c:	00279713          	slli	a4,a5,0x2
    19a0:	00e60733          	add	a4,a2,a4
    19a4:	00072703          	lw	a4,0(a4)
    19a8:	00e68463          	beq	a3,a4,19b0 <main+0x5a0>
    19ac:	42ed96e3          	bne	s11,a4,25d8 <main+0x11c8>
    19b0:	02c12783          	lw	a5,44(sp)
    19b4:	00300713          	li	a4,3
    19b8:	02812603          	lw	a2,40(sp)
    19bc:	00378793          	addi	a5,a5,3
    19c0:	02e7f7b3          	remu	a5,a5,a4
    19c4:	00279713          	slli	a4,a5,0x2
    19c8:	00e60733          	add	a4,a2,a4
    19cc:	00072703          	lw	a4,0(a4)
    19d0:	00e68463          	beq	a3,a4,19d8 <main+0x5c8>
    19d4:	40ed92e3          	bne	s11,a4,25d8 <main+0x11c8>
    19d8:	02c12783          	lw	a5,44(sp)
    19dc:	02812703          	lw	a4,40(sp)
    19e0:	00279793          	slli	a5,a5,0x2
    19e4:	00f707b3          	add	a5,a4,a5
    19e8:	0007ad83          	lw	s11,0(a5)
    19ec:	00042783          	lw	a5,0(s0)
    19f0:	f8010d37          	lui	s10,0xf8010
    19f4:	0187d793          	srli	a5,a5,0x18
    19f8:	02078463          	beqz	a5,1a20 <main+0x610>
    19fc:	00042783          	lw	a5,0(s0)
    1a00:	0187d793          	srli	a5,a5,0x18
    1a04:	fe078ce3          	beqz	a5,19fc <main+0x5ec>
    1a08:	000d2503          	lw	a0,0(s10) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    1a0c:	0ff57513          	zext.b	a0,a0
    1a10:	1a0090ef          	jal	abb0 <handle_char>
    1a14:	00042783          	lw	a5,0(s0)
    1a18:	0187d793          	srli	a5,a5,0x18
    1a1c:	fe0790e3          	bnez	a5,19fc <main+0x5ec>
    1a20:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1a24:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    1a28:	f80106b7          	lui	a3,0xf8010
    1a2c:	04f70863          	beq	a4,a5,1a7c <main+0x66c>
    1a30:	00042783          	lw	a5,0(s0)
    1a34:	0107d713          	srli	a4,a5,0x10
    1a38:	0ff77713          	zext.b	a4,a4
    1a3c:	04070063          	beqz	a4,1a7c <main+0x66c>
    1a40:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1a44:	00f987b3          	add	a5,s3,a5
    1a48:	0007c783          	lbu	a5,0(a5)
    1a4c:	00042703          	lw	a4,0(s0)
    1a50:	01075613          	srli	a2,a4,0x10
    1a54:	0ff67613          	zext.b	a2,a2
    1a58:	fe060ae3          	beqz	a2,1a4c <main+0x63c>
    1a5c:	00f6a023          	sw	a5,0(a3) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    1a60:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1a64:	00178793          	addi	a5,a5,1
    1a68:	0147f7b3          	and	a5,a5,s4
    1a6c:	88f1a823          	sw	a5,-1904(gp) # 13498 <tx_tail>
    1a70:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    1a74:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    1a78:	faf71ce3          	bne	a4,a5,1a30 <main+0x620>
    1a7c:	8241a783          	lw	a5,-2012(gp) # 1342c <vsync>
    1a80:	000ba803          	lw	a6,0(s7)
    1a84:	00079863          	bnez	a5,1a94 <main+0x684>
    1a88:	03812783          	lw	a5,56(sp)
    1a8c:	40f807b3          	sub	a5,a6,a5
    1a90:	2807c2e3          	bltz	a5,2514 <main+0x1104>
    1a94:	8681a603          	lw	a2,-1944(gp) # 13470 <scene>
    1a98:	00100793          	li	a5,1
    1a9c:	16f12a23          	sw	a5,372(sp)
    1aa0:	220616e3          	bnez	a2,24cc <main+0x10bc>
    1aa4:	8281a703          	lw	a4,-2008(gp) # 13430 <autoplay>
    1aa8:	340712e3          	bnez	a4,25ec <main+0x11dc>
    1aac:	8881a783          	lw	a5,-1912(gp) # 13490 <dir_x>
    1ab0:	00100593          	li	a1,1
    1ab4:	0007c663          	bltz	a5,1ac0 <main+0x6b0>
    1ab8:	00f037b3          	snez	a5,a5
    1abc:	00179593          	slli	a1,a5,0x1
    1ac0:	8841a783          	lw	a5,-1916(gp) # 1348c <dir_y>
    1ac4:	0007d463          	bgez	a5,1acc <main+0x6bc>
    1ac8:	7c90006f          	j	2a90 <main+0x1680>
    1acc:	00078463          	beqz	a5,1ad4 <main+0x6c4>
    1ad0:	0085e593          	ori	a1,a1,8
    1ad4:	00000793          	li	a5,0
    1ad8:	8801a703          	lw	a4,-1920(gp) # 13488 <key_pressed>
    1adc:	000af637          	lui	a2,0xaf
    1ae0:	f5060513          	addi	a0,a2,-176 # aef50 <game>
    1ae4:	00e7e7b3          	or	a5,a5,a4
    1ae8:	00078613          	mv	a2,a5
    1aec:	03012a23          	sw	a6,52(sp)
    1af0:	12b12823          	sw	a1,304(sp)
    1af4:	8801a023          	sw	zero,-1920(gp) # 13488 <key_pressed>
    1af8:	12f12a23          	sw	a5,308(sp)
    1afc:	3900b0ef          	jal	ce8c <game_tick>
    1b00:	03412803          	lw	a6,52(sp)
    1b04:	03812703          	lw	a4,56(sp)
    1b08:	001977b7          	lui	a5,0x197
    1b0c:	e6a78793          	addi	a5,a5,-406 # 196e6a <__freertos_irq_stack_top+0xaecca>
    1b10:	00f707b3          	add	a5,a4,a5
    1b14:	02f12c23          	sw	a5,56(sp)
    1b18:	40f807b3          	sub	a5,a6,a5
    1b1c:	1ef050e3          	blez	a5,24fc <main+0x10ec>
    1b20:	000ba783          	lw	a5,0(s7)
    1b24:	03012c23          	sw	a6,56(sp)
    1b28:	02f12a23          	sw	a5,52(sp)
    1b2c:	41078833          	sub	a6,a5,a6
    1b30:	15012e23          	sw	a6,348(sp)
    1b34:	8201a683          	lw	a3,-2016(gp) # 13428 <show_stats>
    1b38:	81c1a703          	lw	a4,-2020(gp) # 13424 <alpha_fx>
    1b3c:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    1b40:	1ad12a23          	sw	a3,436(sp)
    1b44:	1ce12023          	sw	a4,448(sp)
    1b48:	1cf12c23          	sw	a5,472(sp)
    1b4c:	66079463          	bnez	a5,21b4 <main+0xda4>
    1b50:	03c12783          	lw	a5,60(sp)
    1b54:	1a078ae3          	beqz	a5,2508 <main+0x10f8>
    1b58:	0000500f          	.word	0x0000500f
    1b5c:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    1b60:	020c2e23          	sw	zero,60(s8)
    1b64:	030c2d03          	lw	s10,48(s8)
    1b68:	02f12e23          	sw	a5,60(sp)
    1b6c:	64079a63          	bnez	a5,21c0 <main+0xdb0>
    1b70:	1e000693          	li	a3,480
    1b74:	28000613          	li	a2,640
    1b78:	000d8593          	mv	a1,s11
    1b7c:	17810513          	addi	a0,sp,376
    1b80:	0d4070ef          	jal	8c54 <gfx_begin>
    1b84:	8681a683          	lw	a3,-1944(gp) # 13470 <scene>
    1b88:	02012e23          	sw	zero,60(sp)
    1b8c:	00100793          	li	a5,1
    1b90:	64f68a63          	beq	a3,a5,21e4 <main+0xdd4>
    1b94:	00200613          	li	a2,2
    1b98:	1a010793          	addi	a5,sp,416
    1b9c:	0b010713          	addi	a4,sp,176
    1ba0:	54c68863          	beq	a3,a2,20f0 <main+0xce0>
    1ba4:	0107a683          	lw	a3,16(a5)
    1ba8:	0007a803          	lw	a6,0(a5)
    1bac:	0047a503          	lw	a0,4(a5)
    1bb0:	0087a583          	lw	a1,8(a5)
    1bb4:	00c7a603          	lw	a2,12(a5)
    1bb8:	00d72823          	sw	a3,16(a4)
    1bbc:	01072023          	sw	a6,0(a4)
    1bc0:	00a72223          	sw	a0,4(a4)
    1bc4:	00b72423          	sw	a1,8(a4)
    1bc8:	00c72623          	sw	a2,12(a4)
    1bcc:	01478793          	addi	a5,a5,20
    1bd0:	20410693          	addi	a3,sp,516
    1bd4:	01470713          	addi	a4,a4,20
    1bd8:	fcd796e3          	bne	a5,a3,1ba4 <main+0x794>
    1bdc:	20412783          	lw	a5,516(sp)
    1be0:	000af6b7          	lui	a3,0xaf
    1be4:	0b010613          	addi	a2,sp,176
    1be8:	17810593          	addi	a1,sp,376
    1bec:	f5068513          	addi	a0,a3,-176 # aef50 <game>
    1bf0:	00f72023          	sw	a5,0(a4)
    1bf4:	160040ef          	jal	5d54 <render_game>
    1bf8:	17810513          	addi	a0,sp,376
    1bfc:	0c4070ef          	jal	8cc0 <gfx_end>
    1c00:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    1c04:	54079863          	bnez	a5,2154 <main+0xd44>
    1c08:	030c2603          	lw	a2,48(s8)
    1c0c:	000ba683          	lw	a3,0(s7)
    1c10:	03412783          	lw	a5,52(sp)
    1c14:	41a60733          	sub	a4,a2,s10
    1c18:	16e12223          	sw	a4,356(sp)
    1c1c:	40f687b3          	sub	a5,a3,a5
    1c20:	62f77063          	bgeu	a4,a5,2240 <main+0xe30>
    1c24:	40c787b3          	sub	a5,a5,a2
    1c28:	01a787b3          	add	a5,a5,s10
    1c2c:	16f12023          	sw	a5,352(sp)
    1c30:	000ba783          	lw	a5,0(s7)
    1c34:	40d787b3          	sub	a5,a5,a3
    1c38:	16f12423          	sw	a5,360(sp)
    1c3c:	0ff0000f          	fence
    1c40:	f81006b7          	lui	a3,0xf8100
    1c44:	01b6a223          	sw	s11,4(a3) # f8100004 <__freertos_irq_stack_top+0xf8017e64>
    1c48:	16012f03          	lw	t5,352(sp)
    1c4c:	16412e83          	lw	t4,356(sp)
    1c50:	16812e03          	lw	t3,360(sp)
    1c54:	004b2283          	lw	t0,4(s6)
    1c58:	00cb2f83          	lw	t6,12(s6)
    1c5c:	018b2803          	lw	a6,24(s6)
    1c60:	01cb2503          	lw	a0,28(s6)
    1c64:	020b2583          	lw	a1,32(s6)
    1c68:	028b2603          	lw	a2,40(s6)
    1c6c:	03012683          	lw	a3,48(sp)
    1c70:	15812083          	lw	ra,344(sp)
    1c74:	16c12383          	lw	t2,364(sp)
    1c78:	17012703          	lw	a4,368(sp)
    1c7c:	02cb2883          	lw	a7,44(s6)
    1c80:	00128293          	addi	t0,t0,1 # 11b9 <memset+0xb5>
    1c84:	001f8fb3          	add	t6,t6,ra
    1c88:	01e80833          	add	a6,a6,t5
    1c8c:	01d50533          	add	a0,a0,t4
    1c90:	01c585b3          	add	a1,a1,t3
    1c94:	00760633          	add	a2,a2,t2
    1c98:	00168693          	addi	a3,a3,1
    1c9c:	01df07b3          	add	a5,t5,t4
    1ca0:	005b2223          	sw	t0,4(s6)
    1ca4:	01fb2623          	sw	t6,12(s6)
    1ca8:	010b2c23          	sw	a6,24(s6)
    1cac:	00ab2e23          	sw	a0,28(s6)
    1cb0:	02bb2023          	sw	a1,32(s6)
    1cb4:	02cb2423          	sw	a2,40(s6)
    1cb8:	02d12823          	sw	a3,48(sp)
    1cbc:	15c12f83          	lw	t6,348(sp)
    1cc0:	17412283          	lw	t0,372(sp)
    1cc4:	01c787b3          	add	a5,a5,t3
    1cc8:	00e8f463          	bgeu	a7,a4,1cd0 <main+0x8c0>
    1ccc:	02eb2623          	sw	a4,44(s6)
    1cd0:	024b2683          	lw	a3,36(s6)
    1cd4:	00f6f463          	bgeu	a3,a5,1cdc <main+0x8cc>
    1cd8:	02fb2223          	sw	a5,36(s6)
    1cdc:	02028463          	beqz	t0,1d04 <main+0x8f4>
    1ce0:	008b2603          	lw	a2,8(s6)
    1ce4:	010b2683          	lw	a3,16(s6)
    1ce8:	014b2583          	lw	a1,20(s6)
    1cec:	00160613          	addi	a2,a2,1
    1cf0:	01f686b3          	add	a3,a3,t6
    1cf4:	00cb2423          	sw	a2,8(s6)
    1cf8:	00db2823          	sw	a3,16(s6)
    1cfc:	01f5f463          	bgeu	a1,t6,1d04 <main+0x8f4>
    1d00:	01fb2a23          	sw	t6,20(s6)
    1d04:	004aa883          	lw	a7,4(s5)
    1d08:	00caa803          	lw	a6,12(s5)
    1d0c:	018aa503          	lw	a0,24(s5)
    1d10:	01caa583          	lw	a1,28(s5)
    1d14:	020aa603          	lw	a2,32(s5)
    1d18:	028aa683          	lw	a3,40(s5)
    1d1c:	02caad03          	lw	s10,44(s5)
    1d20:	00188893          	addi	a7,a7,1
    1d24:	00180833          	add	a6,a6,ra
    1d28:	01e50533          	add	a0,a0,t5
    1d2c:	01d585b3          	add	a1,a1,t4
    1d30:	01c60633          	add	a2,a2,t3
    1d34:	007686b3          	add	a3,a3,t2
    1d38:	011aa223          	sw	a7,4(s5)
    1d3c:	010aa623          	sw	a6,12(s5)
    1d40:	00aaac23          	sw	a0,24(s5)
    1d44:	00baae23          	sw	a1,28(s5)
    1d48:	02caa023          	sw	a2,32(s5)
    1d4c:	02daa423          	sw	a3,40(s5)
    1d50:	00ed7463          	bgeu	s10,a4,1d58 <main+0x948>
    1d54:	02eaa623          	sw	a4,44(s5)
    1d58:	024aa703          	lw	a4,36(s5)
    1d5c:	00f77463          	bgeu	a4,a5,1d64 <main+0x954>
    1d60:	02faa223          	sw	a5,36(s5)
    1d64:	02028463          	beqz	t0,1d8c <main+0x97c>
    1d68:	008aa703          	lw	a4,8(s5)
    1d6c:	010aa783          	lw	a5,16(s5)
    1d70:	014aa683          	lw	a3,20(s5)
    1d74:	00170713          	addi	a4,a4,1
    1d78:	01f787b3          	add	a5,a5,t6
    1d7c:	00eaa423          	sw	a4,8(s5)
    1d80:	00faa823          	sw	a5,16(s5)
    1d84:	01f6f463          	bgeu	a3,t6,1d8c <main+0x97c>
    1d88:	01faaa23          	sw	t6,20(s5)
    1d8c:	000bad03          	lw	s10,0(s7)
    1d90:	000b2703          	lw	a4,0(s6)
    1d94:	02faf7b7          	lui	a5,0x2faf
    1d98:	07f78793          	addi	a5,a5,127 # 2faf07f <__freertos_irq_stack_top+0x2ec6edf>
    1d9c:	40ed0733          	sub	a4,s10,a4
    1da0:	4ae7e463          	bltu	a5,a4,2248 <main+0xe38>
    1da4:	000aa703          	lw	a4,0(s5)
    1da8:	05f5e7b7          	lui	a5,0x5f5e
    1dac:	0ff78793          	addi	a5,a5,255 # 5f5e0ff <__freertos_irq_stack_top+0x5e75f5f>
    1db0:	40ed0733          	sub	a4,s10,a4
    1db4:	20e7fa63          	bgeu	a5,a4,1fc8 <main+0xbb8>
    1db8:	000ae7b7          	lui	a5,0xae
    1dbc:	ec078513          	addi	a0,a5,-320 # adec0 <uart_w>
    1dc0:	20810613          	addi	a2,sp,520
    1dc4:	000d0593          	mv	a1,s10
    1dc8:	7c5070ef          	jal	9d8c <win_end>
    1dcc:	21012703          	lw	a4,528(sp)
    1dd0:	8701a503          	lw	a0,-1936(gp) # 13478 <use_hw>
    1dd4:	00a00793          	li	a5,10
    1dd8:	02f755b3          	divu	a1,a4,a5
    1ddc:	05412683          	lw	a3,84(sp)
    1de0:	02f77633          	remu	a2,a4,a5
    1de4:	00050463          	beqz	a0,1dec <main+0x9dc>
    1de8:	07812683          	lw	a3,120(sp)
    1dec:	8241a503          	lw	a0,-2012(gp) # 1342c <vsync>
    1df0:	8681a703          	lw	a4,-1944(gp) # 13470 <scene>
    1df4:	05812783          	lw	a5,88(sp)
    1df8:	00050463          	beqz	a0,1e00 <main+0x9f0>
    1dfc:	07c12783          	lw	a5,124(sp)
    1e00:	8281a503          	lw	a0,-2008(gp) # 13430 <autoplay>
    1e04:	04c12803          	lw	a6,76(sp)
    1e08:	00050463          	beqz	a0,1e10 <main+0xa00>
    1e0c:	08012803          	lw	a6,128(sp)
    1e10:	81c1a503          	lw	a0,-2020(gp) # 13424 <alpha_fx>
    1e14:	04c12883          	lw	a7,76(sp)
    1e18:	00050463          	beqz	a0,1e20 <main+0xa10>
    1e1c:	08412883          	lw	a7,132(sp)
    1e20:	f8100e37          	lui	t3,0xf8100
    1e24:	018e2503          	lw	a0,24(t3) # f8100018 <__freertos_irq_stack_top+0xf8017e78>
    1e28:	04812303          	lw	t1,72(sp)
    1e2c:	06432f03          	lw	t5,100(t1)
    1e30:	00032e83          	lw	t4,0(t1)
    1e34:	00a12223          	sw	a0,4(sp)
    1e38:	21412503          	lw	a0,532(sp)
    1e3c:	01e12623          	sw	t5,12(sp)
    1e40:	01d12423          	sw	t4,8(sp)
    1e44:	00a12023          	sw	a0,0(sp)
    1e48:	00012537          	lui	a0,0x12
    1e4c:	3d050513          	addi	a0,a0,976 # 123d0 <_data+0xc94>
    1e50:	37d080ef          	jal	a9cc <tx_printf>
    1e54:	24012e03          	lw	t3,576(sp)
    1e58:	00a00e93          	li	t4,10
    1e5c:	23c12503          	lw	a0,572(sp)
    1e60:	03de7f33          	remu	t5,t3,t4
    1e64:	23012883          	lw	a7,560(sp)
    1e68:	00a12423          	sw	a0,8(sp)
    1e6c:	23812503          	lw	a0,568(sp)
    1e70:	21812803          	lw	a6,536(sp)
    1e74:	22c12783          	lw	a5,556(sp)
    1e78:	00a12223          	sw	a0,4(sp)
    1e7c:	23412503          	lw	a0,564(sp)
    1e80:	22812703          	lw	a4,552(sp)
    1e84:	22412683          	lw	a3,548(sp)
    1e88:	22012603          	lw	a2,544(sp)
    1e8c:	21c12583          	lw	a1,540(sp)
    1e90:	00a12023          	sw	a0,0(sp)
    1e94:	00012537          	lui	a0,0x12
    1e98:	41850513          	addi	a0,a0,1048 # 12418 <_data+0xcdc>
    1e9c:	03de5e33          	divu	t3,t3,t4
    1ea0:	01e12823          	sw	t5,16(sp)
    1ea4:	01c12623          	sw	t3,12(sp)
    1ea8:	325080ef          	jal	a9cc <tx_printf>
    1eac:	25412783          	lw	a5,596(sp)
    1eb0:	25812803          	lw	a6,600(sp)
    1eb4:	25012703          	lw	a4,592(sp)
    1eb8:	24c12683          	lw	a3,588(sp)
    1ebc:	24812603          	lw	a2,584(sp)
    1ec0:	24412583          	lw	a1,580(sp)
    1ec4:	00012537          	lui	a0,0x12
    1ec8:	4a050513          	addi	a0,a0,1184 # 124a0 <_data+0xd64>
    1ecc:	301080ef          	jal	a9cc <tx_printf>
    1ed0:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    1ed4:	66079863          	bnez	a5,2544 <main+0x1134>
    1ed8:	1ec12503          	lw	a0,492(sp)
    1edc:	19412783          	lw	a5,404(sp)
    1ee0:	18c12583          	lw	a1,396(sp)
    1ee4:	00a12223          	sw	a0,4(sp)
    1ee8:	1e412503          	lw	a0,484(sp)
    1eec:	1e812883          	lw	a7,488(sp)
    1ef0:	1e012803          	lw	a6,480(sp)
    1ef4:	19c12703          	lw	a4,412(sp)
    1ef8:	19812683          	lw	a3,408(sp)
    1efc:	19012603          	lw	a2,400(sp)
    1f00:	00a12023          	sw	a0,0(sp)
    1f04:	00012537          	lui	a0,0x12
    1f08:	56850513          	addi	a0,a0,1384 # 12568 <_data+0xe2c>
    1f0c:	2c1080ef          	jal	a9cc <tx_printf>
    1f10:	88c1a583          	lw	a1,-1908(gp) # 13494 <tx_dropped>
    1f14:	62059063          	bnez	a1,2534 <main+0x1124>
    1f18:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    1f1c:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    1f20:	00200713          	li	a4,2
    1f24:	00d00613          	li	a2,13
    1f28:	fff78793          	addi	a5,a5,-1
    1f2c:	40d787b3          	sub	a5,a5,a3
    1f30:	0147f7b3          	and	a5,a5,s4
    1f34:	05c12683          	lw	a3,92(sp)
    1f38:	5ef77663          	bgeu	a4,a5,2524 <main+0x1114>
    1f3c:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    1f40:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    1f44:	00168693          	addi	a3,a3,1
    1f48:	00e98733          	add	a4,s3,a4
    1f4c:	00c70023          	sb	a2,0(a4)
    1f50:	00178793          	addi	a5,a5,1
    1f54:	0006c603          	lbu	a2,0(a3)
    1f58:	0147f7b3          	and	a5,a5,s4
    1f5c:	88f1aa23          	sw	a5,-1900(gp) # 1349c <tx_head>
    1f60:	fc061ee3          	bnez	a2,1f3c <main+0xb2c>
    1f64:	000ae7b7          	lui	a5,0xae
    1f68:	ec478513          	addi	a0,a5,-316 # adec4 <uart_w+0x4>
    1f6c:	04400613          	li	a2,68
    1f70:	00000593          	li	a1,0
    1f74:	990ff0ef          	jal	1104 <memset>
    1f78:	f81007b7          	lui	a5,0xf8100
    1f7c:	01aaa023          	sw	s10,0(s5)
    1f80:	0107a683          	lw	a3,16(a5) # f8100010 <__freertos_irq_stack_top+0xf8017e70>
    1f84:	8741a703          	lw	a4,-1932(gp) # 1347c <hw_avail>
    1f88:	02daa823          	sw	a3,48(s5)
    1f8c:	02070663          	beqz	a4,1fb8 <main+0xba8>
    1f90:	f81017b7          	lui	a5,0xf8101
    1f94:	0407a683          	lw	a3,64(a5) # f8101040 <__freertos_irq_stack_top+0xf8018ea0>
    1f98:	f8101737          	lui	a4,0xf8101
    1f9c:	02daaa23          	sw	a3,52(s5)
    1fa0:	04872683          	lw	a3,72(a4) # f8101048 <__freertos_irq_stack_top+0xf8018ea8>
    1fa4:	f8101737          	lui	a4,0xf8101
    1fa8:	04c70713          	addi	a4,a4,76 # f810104c <__freertos_irq_stack_top+0xf8018eac>
    1fac:	02daac23          	sw	a3,56(s5)
    1fb0:	00072703          	lw	a4,0(a4)
    1fb4:	02eaae23          	sw	a4,60(s5)
    1fb8:	034c2703          	lw	a4,52(s8)
    1fbc:	028c2783          	lw	a5,40(s8)
    1fc0:	04eaa223          	sw	a4,68(s5)
    1fc4:	04faa023          	sw	a5,64(s5)
    1fc8:	8641a783          	lw	a5,-1948(gp) # 1346c <stats_reset>
    1fcc:	94078ce3          	beqz	a5,1924 <main+0x514>
    1fd0:	000bad03          	lw	s10,0(s7)
    1fd4:	000ae7b7          	lui	a5,0xae
    1fd8:	04400613          	li	a2,68
    1fdc:	f0c78513          	addi	a0,a5,-244 # adf0c <hud_w+0x4>
    1fe0:	00000593          	li	a1,0
    1fe4:	8601a223          	sw	zero,-1948(gp) # 1346c <stats_reset>
    1fe8:	91cff0ef          	jal	1104 <memset>
    1fec:	f81007b7          	lui	a5,0xf8100
    1ff0:	01ab2023          	sw	s10,0(s6)
    1ff4:	0107a703          	lw	a4,16(a5) # f8100010 <__freertos_irq_stack_top+0xf8017e70>
    1ff8:	8741a603          	lw	a2,-1932(gp) # 1347c <hw_avail>
    1ffc:	02eb2823          	sw	a4,48(s6)
    2000:	028c2683          	lw	a3,40(s8)
    2004:	034c2703          	lw	a4,52(s8)
    2008:	01078793          	addi	a5,a5,16
    200c:	58060463          	beqz	a2,2594 <main+0x1184>
    2010:	f8101e37          	lui	t3,0xf8101
    2014:	040e2603          	lw	a2,64(t3) # f8101040 <__freertos_irq_stack_top+0xf8018ea0>
    2018:	f81018b7          	lui	a7,0xf8101
    201c:	f8101837          	lui	a6,0xf8101
    2020:	02cb2a23          	sw	a2,52(s6)
    2024:	0488a603          	lw	a2,72(a7) # f8101048 <__freertos_irq_stack_top+0xf8018ea8>
    2028:	08f12c23          	sw	a5,152(sp)
    202c:	000ae7b7          	lui	a5,0xae
    2030:	02cb2c23          	sw	a2,56(s6)
    2034:	04c82e83          	lw	t4,76(a6) # f810104c <__freertos_irq_stack_top+0xf8018eac>
    2038:	04888893          	addi	a7,a7,72
    203c:	04c80813          	addi	a6,a6,76
    2040:	040e0e13          	addi	t3,t3,64
    2044:	ec478513          	addi	a0,a5,-316 # adec4 <uart_w+0x4>
    2048:	04400613          	li	a2,68
    204c:	00000593          	li	a1,0
    2050:	09112a23          	sw	a7,148(sp)
    2054:	09012823          	sw	a6,144(sp)
    2058:	09c12623          	sw	t3,140(sp)
    205c:	04db2023          	sw	a3,64(s6)
    2060:	04d12823          	sw	a3,80(sp)
    2064:	04eb2223          	sw	a4,68(s6)
    2068:	02e12a23          	sw	a4,52(sp)
    206c:	03db2e23          	sw	t4,60(s6)
    2070:	894ff0ef          	jal	1104 <memset>
    2074:	09812783          	lw	a5,152(sp)
    2078:	01aaa023          	sw	s10,0(s5)
    207c:	08c12e03          	lw	t3,140(sp)
    2080:	0007a783          	lw	a5,0(a5)
    2084:	09412883          	lw	a7,148(sp)
    2088:	09012803          	lw	a6,144(sp)
    208c:	02faa823          	sw	a5,48(s5)
    2090:	000e2783          	lw	a5,0(t3)
    2094:	05012683          	lw	a3,80(sp)
    2098:	03412703          	lw	a4,52(sp)
    209c:	02faaa23          	sw	a5,52(s5)
    20a0:	0008a783          	lw	a5,0(a7)
    20a4:	02faac23          	sw	a5,56(s5)
    20a8:	00082783          	lw	a5,0(a6)
    20ac:	02faae23          	sw	a5,60(s5)
    20b0:	04daa023          	sw	a3,64(s5)
    20b4:	04eaa223          	sw	a4,68(s5)
    20b8:	04012823          	sw	zero,80(sp)
    20bc:	869ff06f          	j	1924 <main+0x514>
    20c0:	f81017b7          	lui	a5,0xf8101
    20c4:	0007a583          	lw	a1,0(a5) # f8101000 <__freertos_irq_stack_top+0xf8018e60>
    20c8:	00012537          	lui	a0,0x12
    20cc:	16450513          	addi	a0,a0,356 # 12164 <_data+0xa28>
    20d0:	000e4c37          	lui	s8,0xe4
    20d4:	178080ef          	jal	a24c <bsp_printf>
    20d8:	154c0c13          	addi	s8,s8,340 # e4154 <blit_st>
    20dc:	becff06f          	j	14c8 <main+0xb8>
    20e0:	000127b7          	lui	a5,0x12
    20e4:	d6c78513          	addi	a0,a5,-660 # 11d6c <_data+0x630>
    20e8:	428080ef          	jal	a510 <tx_puts>
    20ec:	d10ff06f          	j	15fc <main+0x1ec>
    20f0:	0107a683          	lw	a3,16(a5)
    20f4:	0007a803          	lw	a6,0(a5)
    20f8:	0047a503          	lw	a0,4(a5)
    20fc:	0087a583          	lw	a1,8(a5)
    2100:	00c7a603          	lw	a2,12(a5)
    2104:	00d72823          	sw	a3,16(a4)
    2108:	01072023          	sw	a6,0(a4)
    210c:	00a72223          	sw	a0,4(a4)
    2110:	00b72423          	sw	a1,8(a4)
    2114:	00c72623          	sw	a2,12(a4)
    2118:	01478793          	addi	a5,a5,20
    211c:	20410693          	addi	a3,sp,516
    2120:	01470713          	addi	a4,a4,20
    2124:	fcd796e3          	bne	a5,a3,20f0 <main+0xce0>
    2128:	20412783          	lw	a5,516(sp)
    212c:	8141a683          	lw	a3,-2028(gp) # 1341c <img_period>
    2130:	03012603          	lw	a2,48(sp)
    2134:	0b010593          	addi	a1,sp,176
    2138:	00f72023          	sw	a5,0(a4)
    213c:	17810513          	addi	a0,sp,376
    2140:	44d0d0ef          	jal	fd8c <demo_images_render>
    2144:	17810513          	addi	a0,sp,376
    2148:	379060ef          	jal	8cc0 <gfx_end>
    214c:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    2150:	aa078ce3          	beqz	a5,1c08 <main+0x7f8>
    2154:	2f90e0ef          	jal	10c4c <blit_queue_used>
    2158:	03cc2783          	lw	a5,60(s8)
    215c:	16a12623          	sw	a0,364(sp)
    2160:	030c2583          	lw	a1,48(s8)
    2164:	16f12823          	sw	a5,368(sp)
    2168:	000ba683          	lw	a3,0(s7)
    216c:	03412783          	lw	a5,52(sp)
    2170:	41a58733          	sub	a4,a1,s10
    2174:	16e12223          	sw	a4,356(sp)
    2178:	40f687b3          	sub	a5,a3,a5
    217c:	8701a503          	lw	a0,-1936(gp) # 13478 <use_hw>
    2180:	00000613          	li	a2,0
    2184:	00f77663          	bgeu	a4,a5,2190 <main+0xd80>
    2188:	40b787b3          	sub	a5,a5,a1
    218c:	01a78633          	add	a2,a5,s10
    2190:	f8101737          	lui	a4,0xf8101
    2194:	16c12023          	sw	a2,352(sp)
    2198:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
    219c:	a8050ae3          	beqz	a0,1c30 <main+0x820>
    21a0:	00072783          	lw	a5,0(a4)
    21a4:	0017f793          	andi	a5,a5,1
    21a8:	fe079ce3          	bnez	a5,21a0 <main+0xd90>
    21ac:	020c2223          	sw	zero,36(s8)
    21b0:	a81ff06f          	j	1c30 <main+0x820>
    21b4:	030c2d03          	lw	s10,48(s8)
    21b8:	020c2e23          	sw	zero,60(s8)
    21bc:	02f12e23          	sw	a5,60(sp)
    21c0:	28000693          	li	a3,640
    21c4:	1e000713          	li	a4,480
    21c8:	000d8613          	mv	a2,s11
    21cc:	12810593          	addi	a1,sp,296
    21d0:	17810513          	addi	a0,sp,376
    21d4:	39d070ef          	jal	9d70 <gfx_begin_hw>
    21d8:	8681a683          	lw	a3,-1944(gp) # 13470 <scene>
    21dc:	00100793          	li	a5,1
    21e0:	9af69ae3          	bne	a3,a5,1b94 <main+0x784>
    21e4:	1a010793          	addi	a5,sp,416
    21e8:	0b010713          	addi	a4,sp,176
    21ec:	0107a683          	lw	a3,16(a5)
    21f0:	0007a803          	lw	a6,0(a5)
    21f4:	0047a503          	lw	a0,4(a5)
    21f8:	0087a583          	lw	a1,8(a5)
    21fc:	00c7a603          	lw	a2,12(a5)
    2200:	00d72823          	sw	a3,16(a4)
    2204:	01072023          	sw	a6,0(a4)
    2208:	00a72223          	sw	a0,4(a4)
    220c:	00b72423          	sw	a1,8(a4)
    2210:	00c72623          	sw	a2,12(a4)
    2214:	01478793          	addi	a5,a5,20
    2218:	20410693          	addi	a3,sp,516
    221c:	01470713          	addi	a4,a4,20
    2220:	fcd796e3          	bne	a5,a3,21ec <main+0xddc>
    2224:	20412783          	lw	a5,516(sp)
    2228:	8181a603          	lw	a2,-2024(gp) # 13420 <sq_count>
    222c:	0b010593          	addi	a1,sp,176
    2230:	17810513          	addi	a0,sp,376
    2234:	00f72023          	sw	a5,0(a4)
    2238:	18d0d0ef          	jal	fbc4 <demo_squares_render>
    223c:	9bdff06f          	j	1bf8 <main+0x7e8>
    2240:	16012023          	sw	zero,352(sp)
    2244:	9edff06f          	j	1c30 <main+0x820>
    2248:	000ae7b7          	lui	a5,0xae
    224c:	f0878513          	addi	a0,a5,-248 # adf08 <hud_w>
    2250:	20810613          	addi	a2,sp,520
    2254:	000d0593          	mv	a1,s10
    2258:	335070ef          	jal	9d8c <win_end>
    225c:	21012683          	lw	a3,528(sp)
    2260:	8701a803          	lw	a6,-1936(gp) # 13478 <use_hw>
    2264:	00a00793          	li	a5,10
    2268:	00568693          	addi	a3,a3,5
    226c:	02f6d6b3          	divu	a3,a3,a5
    2270:	01080793          	addi	a5,a6,16
    2274:	00279793          	slli	a5,a5,0x2
    2278:	09010513          	addi	a0,sp,144
    227c:	1f078793          	addi	a5,a5,496
    2280:	00a787b3          	add	a5,a5,a0
    2284:	23c12603          	lw	a2,572(sp)
    2288:	21c12503          	lw	a0,540(sp)
    228c:	23412883          	lw	a7,564(sp)
    2290:	03260713          	addi	a4,a2,50
    2294:	1aa12423          	sw	a0,424(sp)
    2298:	06400513          	li	a0,100
    229c:	1ac12823          	sw	a2,432(sp)
    22a0:	22012603          	lw	a2,544(sp)
    22a4:	02cc2583          	lw	a1,44(s8)
    22a8:	03012a23          	sw	a6,52(sp)
    22ac:	1cc12223          	sw	a2,452(sp)
    22b0:	22412603          	lw	a2,548(sp)
    22b4:	1b112623          	sw	a7,428(sp)
    22b8:	1eb12e23          	sw	a1,508(sp)
    22bc:	1cc12423          	sw	a2,456(sp)
    22c0:	22812603          	lw	a2,552(sp)
    22c4:	00000593          	li	a1,0
    22c8:	1cc12623          	sw	a2,460(sp)
    22cc:	22c12603          	lw	a2,556(sp)
    22d0:	1cc12823          	sw	a2,464(sp)
    22d4:	21812603          	lw	a2,536(sp)
    22d8:	1cc12a23          	sw	a2,468(sp)
    22dc:	26c12603          	lw	a2,620(sp)
    22e0:	02a75733          	divu	a4,a4,a0
    22e4:	000ae537          	lui	a0,0xae
    22e8:	1ec12823          	sw	a2,496(sp)
    22ec:	25c12603          	lw	a2,604(sp)
    22f0:	1ad12023          	sw	a3,416(sp)
    22f4:	f0c50513          	addi	a0,a0,-244 # adf0c <hud_w+0x4>
    22f8:	1ec12a23          	sw	a2,500(sp)
    22fc:	26012603          	lw	a2,608(sp)
    2300:	1ec12c23          	sw	a2,504(sp)
    2304:	27012603          	lw	a2,624(sp)
    2308:	20c12023          	sw	a2,512(sp)
    230c:	27412603          	lw	a2,628(sp)
    2310:	20c12223          	sw	a2,516(sp)
    2314:	04400613          	li	a2,68
    2318:	1ae12223          	sw	a4,420(sp)
    231c:	f2d7a023          	sw	a3,-224(a5)
    2320:	f317a423          	sw	a7,-216(a5)
    2324:	de1fe0ef          	jal	1104 <memset>
    2328:	f81007b7          	lui	a5,0xf8100
    232c:	01ab2023          	sw	s10,0(s6)
    2330:	0107a703          	lw	a4,16(a5) # f8100010 <__freertos_irq_stack_top+0xf8017e70>
    2334:	8741a683          	lw	a3,-1932(gp) # 1347c <hw_avail>
    2338:	02eb2823          	sw	a4,48(s6)
    233c:	034c2783          	lw	a5,52(s8)
    2340:	028c2703          	lw	a4,40(s8)
    2344:	24068263          	beqz	a3,2588 <main+0x1178>
    2348:	f81016b7          	lui	a3,0xf8101
    234c:	0406a583          	lw	a1,64(a3) # f8101040 <__freertos_irq_stack_top+0xf8018ea0>
    2350:	f8101637          	lui	a2,0xf8101
    2354:	02bb2a23          	sw	a1,52(s6)
    2358:	04862583          	lw	a1,72(a2) # f8101048 <__freertos_irq_stack_top+0xf8018ea8>
    235c:	f8101637          	lui	a2,0xf8101
    2360:	03412803          	lw	a6,52(sp)
    2364:	02bb2c23          	sw	a1,56(s6)
    2368:	04c62583          	lw	a1,76(a2) # f810104c <__freertos_irq_stack_top+0xf8018eac>
    236c:	86c1a603          	lw	a2,-1940(gp) # 13474 <auto_compare>
    2370:	04eb2023          	sw	a4,64(s6)
    2374:	02bb2e23          	sw	a1,60(s6)
    2378:	04fb2223          	sw	a5,68(s6)
    237c:	a20604e3          	beqz	a2,1da4 <main+0x994>
    2380:	05012783          	lw	a5,80(sp)
    2384:	00178793          	addi	a5,a5,1
    2388:	00078713          	mv	a4,a5
    238c:	04f12823          	sw	a5,80(sp)
    2390:	00700793          	li	a5,7
    2394:	a0e7f8e3          	bgeu	a5,a4,1da4 <main+0x994>
    2398:	00183793          	seqz	a5,a6
    239c:	a0f804e3          	beq	a6,a5,1da4 <main+0x994>
    23a0:	86f1a823          	sw	a5,-1936(gp) # 13478 <use_hw>
    23a4:	00100793          	li	a5,1
    23a8:	86f1a223          	sw	a5,-1948(gp) # 1346c <stats_reset>
    23ac:	70081a63          	bnez	a6,2ac0 <main+0x16b0>
    23b0:	06012703          	lw	a4,96(sp)
    23b4:	01a00613          	li	a2,26
    23b8:	8941a583          	lw	a1,-1900(gp) # 1349c <tx_head>
    23bc:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    23c0:	05b00693          	li	a3,91
    23c4:	fff78793          	addi	a5,a5,-1
    23c8:	40b787b3          	sub	a5,a5,a1
    23cc:	0147f7b3          	and	a5,a5,s4
    23d0:	6ef67e63          	bgeu	a2,a5,2acc <main+0x16bc>
    23d4:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    23d8:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    23dc:	00170713          	addi	a4,a4,1
    23e0:	00c98633          	add	a2,s3,a2
    23e4:	00d60023          	sb	a3,0(a2)
    23e8:	00178793          	addi	a5,a5,1
    23ec:	00074683          	lbu	a3,0(a4)
    23f0:	0147f7b3          	and	a5,a5,s4
    23f4:	88f1aa23          	sw	a5,-1900(gp) # 1349c <tx_head>
    23f8:	fc069ee3          	bnez	a3,23d4 <main+0xfc4>
    23fc:	9a9ff06f          	j	1da4 <main+0x994>
    2400:	f8100737          	lui	a4,0xf8100
    2404:	00c72783          	lw	a5,12(a4) # f810000c <__freertos_irq_stack_top+0xf8017e6c>
    2408:	f8010d37          	lui	s10,0xf8010
    240c:	00c70713          	addi	a4,a4,12
    2410:	0017f793          	andi	a5,a5,1
    2414:	d2078e63          	beqz	a5,1950 <main+0x540>
    2418:	09712623          	sw	s7,140(sp)
    241c:	02d12a23          	sw	a3,52(sp)
    2420:	00040b93          	mv	s7,s0
    2424:	00070413          	mv	s0,a4
    2428:	8901a683          	lw	a3,-1904(gp) # 13498 <tx_tail>
    242c:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    2430:	04f68863          	beq	a3,a5,2480 <main+0x1070>
    2434:	000ba783          	lw	a5,0(s7)
    2438:	0107d713          	srli	a4,a5,0x10
    243c:	0ff77713          	zext.b	a4,a4
    2440:	04070063          	beqz	a4,2480 <main+0x1070>
    2444:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    2448:	00f987b3          	add	a5,s3,a5
    244c:	0007c783          	lbu	a5,0(a5)
    2450:	000ba703          	lw	a4,0(s7)
    2454:	01075693          	srli	a3,a4,0x10
    2458:	0ff6f693          	zext.b	a3,a3
    245c:	fe068ae3          	beqz	a3,2450 <main+0x1040>
    2460:	00fd2023          	sw	a5,0(s10) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    2464:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    2468:	00178793          	addi	a5,a5,1
    246c:	0147f7b3          	and	a5,a5,s4
    2470:	88f1a823          	sw	a5,-1904(gp) # 13498 <tx_tail>
    2474:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    2478:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    247c:	faf71ce3          	bne	a4,a5,2434 <main+0x1024>
    2480:	000ba783          	lw	a5,0(s7)
    2484:	0187d793          	srli	a5,a5,0x18
    2488:	02078463          	beqz	a5,24b0 <main+0x10a0>
    248c:	000ba783          	lw	a5,0(s7)
    2490:	0187d793          	srli	a5,a5,0x18
    2494:	fe078ce3          	beqz	a5,248c <main+0x107c>
    2498:	000d2503          	lw	a0,0(s10)
    249c:	0ff57513          	zext.b	a0,a0
    24a0:	710080ef          	jal	abb0 <handle_char>
    24a4:	000ba783          	lw	a5,0(s7)
    24a8:	0187d793          	srli	a5,a5,0x18
    24ac:	fe0790e3          	bnez	a5,248c <main+0x107c>
    24b0:	00042783          	lw	a5,0(s0)
    24b4:	0017f793          	andi	a5,a5,1
    24b8:	f60798e3          	bnez	a5,2428 <main+0x1018>
    24bc:	000b8413          	mv	s0,s7
    24c0:	03412683          	lw	a3,52(sp)
    24c4:	08c12b83          	lw	s7,140(sp)
    24c8:	c88ff06f          	j	1950 <main+0x540>
    24cc:	e2f61c63          	bne	a2,a5,1b04 <main+0x6f4>
    24d0:	8181a503          	lw	a0,-2024(gp) # 13420 <sq_count>
    24d4:	03012a23          	sw	a6,52(sp)
    24d8:	5e80d0ef          	jal	fac0 <demo_squares_tick>
    24dc:	03812703          	lw	a4,56(sp)
    24e0:	03412803          	lw	a6,52(sp)
    24e4:	001977b7          	lui	a5,0x197
    24e8:	e6a78793          	addi	a5,a5,-406 # 196e6a <__freertos_irq_stack_top+0xaecca>
    24ec:	00f707b3          	add	a5,a4,a5
    24f0:	02f12c23          	sw	a5,56(sp)
    24f4:	40f807b3          	sub	a5,a6,a5
    24f8:	e2f04463          	bgtz	a5,1b20 <main+0x710>
    24fc:	000ba783          	lw	a5,0(s7)
    2500:	02f12a23          	sw	a5,52(sp)
    2504:	e28ff06f          	j	1b2c <main+0x71c>
    2508:	030c2d03          	lw	s10,48(s8)
    250c:	020c2e23          	sw	zero,60(s8)
    2510:	e60ff06f          	j	1b70 <main+0x760>
    2514:	000ba783          	lw	a5,0(s7)
    2518:	16012a23          	sw	zero,372(sp)
    251c:	02f12a23          	sw	a5,52(sp)
    2520:	e14ff06f          	j	1b34 <main+0x724>
    2524:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    2528:	00178793          	addi	a5,a5,1
    252c:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    2530:	a35ff06f          	j	1f64 <main+0xb54>
    2534:	000127b7          	lui	a5,0x12
    2538:	5b878513          	addi	a0,a5,1464 # 125b8 <_data+0xe7c>
    253c:	490080ef          	jal	a9cc <tx_printf>
    2540:	9d9ff06f          	j	1f18 <main+0xb08>
    2544:	27c12503          	lw	a0,636(sp)
    2548:	02cc2783          	lw	a5,44(s8)
    254c:	27012883          	lw	a7,624(sp)
    2550:	00a12423          	sw	a0,8(sp)
    2554:	27812503          	lw	a0,632(sp)
    2558:	26412803          	lw	a6,612(sp)
    255c:	26012703          	lw	a4,608(sp)
    2560:	00a12223          	sw	a0,4(sp)
    2564:	27412503          	lw	a0,628(sp)
    2568:	25c12683          	lw	a3,604(sp)
    256c:	26812603          	lw	a2,616(sp)
    2570:	26c12583          	lw	a1,620(sp)
    2574:	00a12023          	sw	a0,0(sp)
    2578:	00012537          	lui	a0,0x12
    257c:	4f050513          	addi	a0,a0,1264 # 124f0 <_data+0xdb4>
    2580:	44c080ef          	jal	a9cc <tx_printf>
    2584:	955ff06f          	j	1ed8 <main+0xac8>
    2588:	04eb2023          	sw	a4,64(s6)
    258c:	04fb2223          	sw	a5,68(s6)
    2590:	815ff06f          	j	1da4 <main+0x994>
    2594:	08f12623          	sw	a5,140(sp)
    2598:	000ae7b7          	lui	a5,0xae
    259c:	ec478513          	addi	a0,a5,-316 # adec4 <uart_w+0x4>
    25a0:	04400613          	li	a2,68
    25a4:	00000593          	li	a1,0
    25a8:	04db2023          	sw	a3,64(s6)
    25ac:	04d12823          	sw	a3,80(sp)
    25b0:	04eb2223          	sw	a4,68(s6)
    25b4:	02e12a23          	sw	a4,52(sp)
    25b8:	b4dfe0ef          	jal	1104 <memset>
    25bc:	08c12783          	lw	a5,140(sp)
    25c0:	01aaa023          	sw	s10,0(s5)
    25c4:	03412703          	lw	a4,52(sp)
    25c8:	0007a783          	lw	a5,0(a5)
    25cc:	05012683          	lw	a3,80(sp)
    25d0:	02faa823          	sw	a5,48(s5)
    25d4:	addff06f          	j	20b0 <main+0xca0>
    25d8:	02f12623          	sw	a5,44(sp)
    25dc:	00178793          	addi	a5,a5,1
    25e0:	00070d93          	mv	s11,a4
    25e4:	04f12023          	sw	a5,64(sp)
    25e8:	c04ff06f          	j	19ec <main+0x5dc>
    25ec:	04812703          	lw	a4,72(sp)
    25f0:	00072583          	lw	a1,0(a4)
    25f4:	8601a703          	lw	a4,-1952(gp) # 13468 <bot_phase>
    25f8:	4ae58063          	beq	a1,a4,2a98 <main+0x1688>
    25fc:	86b1a023          	sw	a1,-1952(gp) # 13468 <bot_phase>
    2600:	84f1ae23          	sw	a5,-1956(gp) # 13464 <bot_wait>
    2604:	00400793          	li	a5,4
    2608:	00b7ec63          	bltu	a5,a1,2620 <main+0x1210>
    260c:	07012703          	lw	a4,112(sp)
    2610:	00259793          	slli	a5,a1,0x2
    2614:	00f707b3          	add	a5,a4,a5
    2618:	0007a783          	lw	a5,0(a5)
    261c:	00078067          	jr	a5
    2620:	00000793          	li	a5,0
    2624:	00000593          	li	a1,0
    2628:	cb0ff06f          	j	1ad8 <main+0x6c8>
    262c:	00100713          	li	a4,1
    2630:	00000793          	li	a5,0
    2634:	00000593          	li	a1,0
    2638:	19000693          	li	a3,400
    263c:	c8e6de63          	bge	a3,a4,1ad8 <main+0x6c8>
    2640:	8401ae23          	sw	zero,-1956(gp) # 13464 <bot_wait>
    2644:	c94ff06f          	j	1ad8 <main+0x6c8>
    2648:	04812783          	lw	a5,72(sp)
    264c:	000af737          	lui	a4,0xaf
    2650:	07412583          	lw	a1,116(sp)
    2654:	0107a383          	lw	t2,16(a5)
    2658:	0147a783          	lw	a5,20(a5)
    265c:	00005eb7          	lui	t4,0x5
    2660:	41f3dd13          	srai	s10,t2,0x1f
    2664:	41f7d893          	srai	a7,a5,0x1f
    2668:	0ffd7d13          	zext.b	s10,s10
    266c:	0ff8f893          	zext.b	a7,a7
    2670:	007d0d33          	add	s10,s10,t2
    2674:	00f888b3          	add	a7,a7,a5
    2678:	f5070713          	addi	a4,a4,-176 # aef50 <game>
    267c:	408d5d13          	srai	s10,s10,0x8
    2680:	4088d893          	srai	a7,a7,0x8
    2684:	04e12423          	sw	a4,72(sp)
    2688:	00000f13          	li	t5,0
    268c:	00000f93          	li	t6,0
    2690:	09600e13          	li	t3,150
    2694:	7e4e8e93          	addi	t4,t4,2020 # 57e4 <__stack_size+0x17e4>
    2698:	04000093          	li	ra,64
    269c:	00300313          	li	t1,3
    26a0:	0005a703          	lw	a4,0(a1)
    26a4:	08070c63          	beqz	a4,273c <main+0x132c>
    26a8:	0045a683          	lw	a3,4(a1)
    26ac:	41f6d713          	srai	a4,a3,0x1f
    26b0:	0ff77713          	zext.b	a4,a4
    26b4:	00d70733          	add	a4,a4,a3
    26b8:	40875713          	srai	a4,a4,0x8
    26bc:	40ed0733          	sub	a4,s10,a4
    26c0:	41f75513          	srai	a0,a4,0x1f
    26c4:	00e546b3          	xor	a3,a0,a4
    26c8:	40a686b3          	sub	a3,a3,a0
    26cc:	06de4863          	blt	t3,a3,273c <main+0x132c>
    26d0:	0085a503          	lw	a0,8(a1)
    26d4:	41f55693          	srai	a3,a0,0x1f
    26d8:	0ff6f693          	zext.b	a3,a3
    26dc:	00a686b3          	add	a3,a3,a0
    26e0:	4086d693          	srai	a3,a3,0x8
    26e4:	40d886b3          	sub	a3,a7,a3
    26e8:	41f6d293          	srai	t0,a3,0x1f
    26ec:	00d2c533          	xor	a0,t0,a3
    26f0:	40550533          	sub	a0,a0,t0
    26f4:	04ae4463          	blt	t3,a0,273c <main+0x132c>
    26f8:	02e70533          	mul	a0,a4,a4
    26fc:	02d682b3          	mul	t0,a3,a3
    2700:	00550533          	add	a0,a0,t0
    2704:	02aecc63          	blt	t4,a0,273c <main+0x132c>
    2708:	00155463          	bge	a0,ra,2710 <main+0x1300>
    270c:	04000513          	li	a0,64
    2710:	0145a283          	lw	t0,20(a1)
    2714:	00628463          	beq	t0,t1,271c <main+0x130c>
    2718:	00100293          	li	t0,1
    271c:	01071713          	slli	a4,a4,0x10
    2720:	01069693          	slli	a3,a3,0x10
    2724:	02a74733          	div	a4,a4,a0
    2728:	02a6c6b3          	div	a3,a3,a0
    272c:	02570533          	mul	a0,a4,t0
    2730:	02568733          	mul	a4,a3,t0
    2734:	00af8fb3          	add	t6,t6,a0
    2738:	00ef0f33          	add	t5,t5,a4
    273c:	04412703          	lw	a4,68(sp)
    2740:	02c58593          	addi	a1,a1,44
    2744:	f4b71ee3          	bne	a4,a1,26a0 <main+0x1290>
    2748:	08f12823          	sw	a5,144(sp)
    274c:	000af7b7          	lui	a5,0xaf
    2750:	f5078513          	addi	a0,a5,-176 # aef50 <game>
    2754:	0b012023          	sw	a6,160(sp)
    2758:	09f12e23          	sw	t6,156(sp)
    275c:	09e12c23          	sw	t5,152(sp)
    2760:	08712a23          	sw	t2,148(sp)
    2764:	09112623          	sw	a7,140(sp)
    2768:	02c12a23          	sw	a2,52(sp)
    276c:	1c80a0ef          	jal	c934 <game_hostile_slots>
    2770:	03412603          	lw	a2,52(sp)
    2774:	08c12883          	lw	a7,140(sp)
    2778:	09012783          	lw	a5,144(sp)
    277c:	09412383          	lw	t2,148(sp)
    2780:	09812f03          	lw	t5,152(sp)
    2784:	09c12f83          	lw	t6,156(sp)
    2788:	0a012803          	lw	a6,160(sp)
    278c:	00050e93          	mv	t4,a0
    2790:	14a05063          	blez	a0,28d0 <main+0x14c0>
    2794:	000032b7          	lui	t0,0x3
    2798:	00000e13          	li	t3,0
    279c:	03b12a23          	sw	s11,52(sp)
    27a0:	09712623          	sw	s7,140(sp)
    27a4:	09812823          	sw	s8,144(sp)
    27a8:	08812a23          	sw	s0,148(sp)
    27ac:	f4428293          	addi	t0,t0,-188 # 2f44 <draw_player+0x12c>
    27b0:	00088d93          	mv	s11,a7
    27b4:	000f8b93          	mv	s7,t6
    27b8:	000f0c13          	mv	s8,t5
    27bc:	000e0413          	mv	s0,t3
    27c0:	0af12023          	sw	a5,160(sp)
    27c4:	000af7b7          	lui	a5,0xaf
    27c8:	00000593          	li	a1,0
    27cc:	f5078513          	addi	a0,a5,-176 # aef50 <game>
    27d0:	0a512623          	sw	t0,172(sp)
    27d4:	0ac12423          	sw	a2,168(sp)
    27d8:	0bd12223          	sw	t4,164(sp)
    27dc:	08712e23          	sw	t2,156(sp)
    27e0:	09012c23          	sw	a6,152(sp)
    27e4:	1b40a0ef          	jal	c998 <game_hostile_at>
    27e8:	00541593          	slli	a1,s0,0x5
    27ec:	00b50733          	add	a4,a0,a1
    27f0:	09812803          	lw	a6,152(sp)
    27f4:	09c12383          	lw	t2,156(sp)
    27f8:	0a012783          	lw	a5,160(sp)
    27fc:	0a412e83          	lw	t4,164(sp)
    2800:	0a812603          	lw	a2,168(sp)
    2804:	0ac12283          	lw	t0,172(sp)
    2808:	06e00313          	li	t1,110
    280c:	0a070063          	beqz	a4,28ac <main+0x149c>
    2810:	00072583          	lw	a1,0(a4)
    2814:	08058c63          	beqz	a1,28ac <main+0x149c>
    2818:	00472503          	lw	a0,4(a4)
    281c:	41f55593          	srai	a1,a0,0x1f
    2820:	0ff5f593          	zext.b	a1,a1
    2824:	00a585b3          	add	a1,a1,a0
    2828:	4085d593          	srai	a1,a1,0x8
    282c:	40bd05b3          	sub	a1,s10,a1
    2830:	41f5df93          	srai	t6,a1,0x1f
    2834:	00bfc533          	xor	a0,t6,a1
    2838:	41f50533          	sub	a0,a0,t6
    283c:	06a34863          	blt	t1,a0,28ac <main+0x149c>
    2840:	00872503          	lw	a0,8(a4)
    2844:	41f55713          	srai	a4,a0,0x1f
    2848:	0ff77713          	zext.b	a4,a4
    284c:	00a70733          	add	a4,a4,a0
    2850:	40875713          	srai	a4,a4,0x8
    2854:	40ed8733          	sub	a4,s11,a4
    2858:	41f75f93          	srai	t6,a4,0x1f
    285c:	00efc533          	xor	a0,t6,a4
    2860:	41f50533          	sub	a0,a0,t6
    2864:	04a34463          	blt	t1,a0,28ac <main+0x149c>
    2868:	02b58533          	mul	a0,a1,a1
    286c:	02e70fb3          	mul	t6,a4,a4
    2870:	01f50533          	add	a0,a0,t6
    2874:	02a2cc63          	blt	t0,a0,28ac <main+0x149c>
    2878:	04000693          	li	a3,64
    287c:	00d55463          	bge	a0,a3,2884 <main+0x1474>
    2880:	04000513          	li	a0,64
    2884:	01059593          	slli	a1,a1,0x10
    2888:	02a5c5b3          	div	a1,a1,a0
    288c:	01071713          	slli	a4,a4,0x10
    2890:	02a74733          	div	a4,a4,a0
    2894:	00159513          	slli	a0,a1,0x1
    2898:	00b505b3          	add	a1,a0,a1
    289c:	00bb8bb3          	add	s7,s7,a1
    28a0:	00171593          	slli	a1,a4,0x1
    28a4:	00e58733          	add	a4,a1,a4
    28a8:	00ec0c33          	add	s8,s8,a4
    28ac:	00140413          	addi	s0,s0,1
    28b0:	f08e98e3          	bne	t4,s0,27c0 <main+0x13b0>
    28b4:	000b8f93          	mv	t6,s7
    28b8:	000c0f13          	mv	t5,s8
    28bc:	000d8893          	mv	a7,s11
    28c0:	08c12b83          	lw	s7,140(sp)
    28c4:	03412d83          	lw	s11,52(sp)
    28c8:	09012c03          	lw	s8,144(sp)
    28cc:	09412403          	lw	s0,148(sp)
    28d0:	04812683          	lw	a3,72(sp)
    28d4:	000245b7          	lui	a1,0x24
    28d8:	08812703          	lw	a4,136(sp)
    28dc:	1d458593          	addi	a1,a1,468 # 241d4 <background_pixels+0xc314>
    28e0:	0000c2b7          	lui	t0,0xc
    28e4:	00b685b3          	add	a1,a3,a1
    28e8:	00000093          	li	ra,0
    28ec:	00000e13          	li	t3,0
    28f0:	d1028293          	addi	t0,t0,-752 # bd10 <hostile_shot+0x9c>
    28f4:	0dc00e93          	li	t4,220
    28f8:	03b12a23          	sw	s11,52(sp)
    28fc:	00072683          	lw	a3,0(a4)
    2900:	06068a63          	beqz	a3,2974 <main+0x1564>
    2904:	00472503          	lw	a0,4(a4)
    2908:	41f55693          	srai	a3,a0,0x1f
    290c:	0ff6f693          	zext.b	a3,a3
    2910:	00a686b3          	add	a3,a3,a0
    2914:	4086d693          	srai	a3,a3,0x8
    2918:	41a686b3          	sub	a3,a3,s10
    291c:	41f6d313          	srai	t1,a3,0x1f
    2920:	00d34533          	xor	a0,t1,a3
    2924:	40650533          	sub	a0,a0,t1
    2928:	04aec663          	blt	t4,a0,2974 <main+0x1564>
    292c:	00872303          	lw	t1,8(a4)
    2930:	41f35513          	srai	a0,t1,0x1f
    2934:	0ff57513          	zext.b	a0,a0
    2938:	00650533          	add	a0,a0,t1
    293c:	40855513          	srai	a0,a0,0x8
    2940:	41150533          	sub	a0,a0,a7
    2944:	41f55d93          	srai	s11,a0,0x1f
    2948:	00adc333          	xor	t1,s11,a0
    294c:	41b30333          	sub	t1,t1,s11
    2950:	026ec263          	blt	t4,t1,2974 <main+0x1564>
    2954:	02a50333          	mul	t1,a0,a0
    2958:	02d68db3          	mul	s11,a3,a3
    295c:	01b30333          	add	t1,t1,s11
    2960:	00535a63          	bge	t1,t0,2974 <main+0x1564>
    2964:	00050e13          	mv	t3,a0
    2968:	00068613          	mv	a2,a3
    296c:	00030293          	mv	t0,t1
    2970:	00100093          	li	ra,1
    2974:	01070713          	addi	a4,a4,16
    2978:	f8e592e3          	bne	a1,a4,28fc <main+0x14ec>
    297c:	03412d83          	lw	s11,52(sp)
    2980:	04008063          	beqz	ra,29c0 <main+0x15b0>
    2984:	2bc00713          	li	a4,700
    2988:	02e605b3          	mul	a1,a2,a4
    298c:	41f65e93          	srai	t4,a2,0x1f
    2990:	41fe5513          	srai	a0,t3,0x1f
    2994:	00cec6b3          	xor	a3,t4,a2
    2998:	01c54633          	xor	a2,a0,t3
    299c:	40a60633          	sub	a2,a2,a0
    29a0:	41d686b3          	sub	a3,a3,t4
    29a4:	00c686b3          	add	a3,a3,a2
    29a8:	00168693          	addi	a3,a3,1
    29ac:	02ee0733          	mul	a4,t3,a4
    29b0:	02d5c633          	div	a2,a1,a3
    29b4:	02d74733          	div	a4,a4,a3
    29b8:	00cf8fb3          	add	t6,t6,a2
    29bc:	00ef0f33          	add	t5,t5,a4
    29c0:	03012683          	lw	a3,48(sp)
    29c4:	05a00713          	li	a4,90
    29c8:	0fc00613          	li	a2,252
    29cc:	02e6d733          	divu	a4,a3,a4
    29d0:	41160633          	sub	a2,a2,a7
    29d4:	00161693          	slli	a3,a2,0x1
    29d8:	00c686b3          	add	a3,a3,a2
    29dc:	06812603          	lw	a2,104(sp)
    29e0:	14000e93          	li	t4,320
    29e4:	41ae8eb3          	sub	t4,t4,s10
    29e8:	001e9513          	slli	a0,t4,0x1
    29ec:	01d50533          	add	a0,a0,t4
    29f0:	15e00e13          	li	t3,350
    29f4:	000045b7          	lui	a1,0x4
    29f8:	5ff58593          	addi	a1,a1,1535 # 45ff <__stack_size+0x5ff>
    29fc:	00777713          	andi	a4,a4,7
    2a00:	00171713          	slli	a4,a4,0x1
    2a04:	00e60733          	add	a4,a2,a4
    2a08:	00170603          	lb	a2,1(a4)
    2a0c:	00070e83          	lb	t4,0(a4)
    2a10:	03c60633          	mul	a2,a2,t3
    2a14:	03ce8733          	mul	a4,t4,t3
    2a18:	01e60633          	add	a2,a2,t5
    2a1c:	00c686b3          	add	a3,a3,a2
    2a20:	01f70733          	add	a4,a4,t6
    2a24:	00e50733          	add	a4,a0,a4
    2a28:	0c75cc63          	blt	a1,t2,2b00 <main+0x16f0>
    2a2c:	04600593          	li	a1,70
    2a30:	41a585b3          	sub	a1,a1,s10
    2a34:	00259613          	slli	a2,a1,0x2
    2a38:	00b60633          	add	a2,a2,a1
    2a3c:	00361613          	slli	a2,a2,0x3
    2a40:	00c70733          	add	a4,a4,a2
    2a44:	00007637          	lui	a2,0x7
    2a48:	7ff60613          	addi	a2,a2,2047 # 77ff <render_game+0x1aab>
    2a4c:	08f64863          	blt	a2,a5,2adc <main+0x16cc>
    2a50:	07800613          	li	a2,120
    2a54:	41160633          	sub	a2,a2,a7
    2a58:	00261793          	slli	a5,a2,0x2
    2a5c:	00c787b3          	add	a5,a5,a2
    2a60:	00379793          	slli	a5,a5,0x3
    2a64:	00f686b3          	add	a3,a3,a5
    2a68:	12c00793          	li	a5,300
    2a6c:	00200593          	li	a1,2
    2a70:	00e7c463          	blt	a5,a4,2a78 <main+0x1668>
    2a74:	ed472593          	slti	a1,a4,-300
    2a78:	12c00793          	li	a5,300
    2a7c:	85c1a703          	lw	a4,-1956(gp) # 13464 <bot_wait>
    2a80:	0cd7d263          	bge	a5,a3,2b44 <main+0x1734>
    2a84:	0085e593          	ori	a1,a1,8
    2a88:	00000793          	li	a5,0
    2a8c:	badff06f          	j	2638 <main+0x1228>
    2a90:	0045e593          	ori	a1,a1,4
    2a94:	840ff06f          	j	1ad4 <main+0x6c4>
    2a98:	85c1a703          	lw	a4,-1956(gp) # 13464 <bot_wait>
    2a9c:	00500793          	li	a5,5
    2aa0:	00170713          	addi	a4,a4,1
    2aa4:	84e1ae23          	sw	a4,-1956(gp) # 13464 <bot_wait>
    2aa8:	b8b7e4e3          	bltu	a5,a1,2630 <main+0x1220>
    2aac:	06c12683          	lw	a3,108(sp)
    2ab0:	00259793          	slli	a5,a1,0x2
    2ab4:	00f687b3          	add	a5,a3,a5
    2ab8:	0007a783          	lw	a5,0(a5)
    2abc:	00078067          	jr	a5
    2ac0:	06412703          	lw	a4,100(sp)
    2ac4:	01000613          	li	a2,16
    2ac8:	8f1ff06f          	j	23b8 <main+0xfa8>
    2acc:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    2ad0:	00178793          	addi	a5,a5,1
    2ad4:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    2ad8:	accff06f          	j	1da4 <main+0x994>
    2adc:	00018637          	lui	a2,0x18
    2ae0:	1ff60613          	addi	a2,a2,511 # 181ff <background_pixels+0x33f>
    2ae4:	f8f652e3          	bge	a2,a5,2a68 <main+0x1658>
    2ae8:	e7f88893          	addi	a7,a7,-385
    2aec:	00289793          	slli	a5,a7,0x2
    2af0:	011787b3          	add	a5,a5,a7
    2af4:	00379793          	slli	a5,a5,0x3
    2af8:	40f686b3          	sub	a3,a3,a5
    2afc:	f6dff06f          	j	2a68 <main+0x1658>
    2b00:	00024637          	lui	a2,0x24
    2b04:	aff60613          	addi	a2,a2,-1281 # 23aff <background_pixels+0xbc3f>
    2b08:	f2765ee3          	bge	a2,t2,2a44 <main+0x1634>
    2b0c:	dc6d0d13          	addi	s10,s10,-570
    2b10:	002d1613          	slli	a2,s10,0x2
    2b14:	01a60633          	add	a2,a2,s10
    2b18:	00361613          	slli	a2,a2,0x3
    2b1c:	40c70733          	sub	a4,a4,a2
    2b20:	f25ff06f          	j	2a44 <main+0x1634>
    2b24:	00012737          	lui	a4,0x12
    2b28:	000127b7          	lui	a5,0x12
    2b2c:	e8c70693          	addi	a3,a4,-372 # 11e8c <_data+0x750>
    2b30:	e7078793          	addi	a5,a5,-400 # 11e70 <_data+0x734>
    2b34:	06d12223          	sw	a3,100(sp)
    2b38:	e8c70513          	addi	a0,a4,-372
    2b3c:	06f12023          	sw	a5,96(sp)
    2b40:	be1fe06f          	j	1720 <main+0x310>
    2b44:	ed400793          	li	a5,-300
    2b48:	0af6d863          	bge	a3,a5,2bf8 <main+0x17e8>
    2b4c:	0045e593          	ori	a1,a1,4
    2b50:	00000793          	li	a5,0
    2b54:	ae5ff06f          	j	2638 <main+0x1228>
    2b58:	f81007b7          	lui	a5,0xf8100
    2b5c:	f8100737          	lui	a4,0xf8100
    2b60:	0147a583          	lw	a1,20(a5) # f8100014 <__freertos_irq_stack_top+0xf8017e74>
    2b64:	01072603          	lw	a2,16(a4) # f8100010 <__freertos_irq_stack_top+0xf8017e70>
    2b68:	00012537          	lui	a0,0x12
    2b6c:	10c50513          	addi	a0,a0,268 # 1210c <_data+0x9d0>
    2b70:	6dc070ef          	jal	a24c <bsp_printf>
    2b74:	929fe06f          	j	149c <main+0x8c>
    2b78:	f4c70793          	addi	a5,a4,-180
    2b7c:	0017b793          	seqz	a5,a5
    2b80:	00479793          	slli	a5,a5,0x4
    2b84:	00000593          	li	a1,0
    2b88:	ab1ff06f          	j	2638 <main+0x1228>
    2b8c:	02d00793          	li	a5,45
    2b90:	aaf710e3          	bne	a4,a5,2630 <main+0x1220>
    2b94:	04812783          	lw	a5,72(sp)
    2b98:	00000593          	li	a1,0
    2b9c:	0547a703          	lw	a4,84(a5)
    2ba0:	00300793          	li	a5,3
    2ba4:	02f76733          	rem	a4,a4,a5
    2ba8:	08000793          	li	a5,128
    2bac:	00e797b3          	sll	a5,a5,a4
    2bb0:	f29fe06f          	j	1ad8 <main+0x6c8>
    2bb4:	fc470793          	addi	a5,a4,-60
    2bb8:	0017b793          	seqz	a5,a5
    2bbc:	00779793          	slli	a5,a5,0x7
    2bc0:	00000593          	li	a1,0
    2bc4:	a75ff06f          	j	2638 <main+0x1228>
    2bc8:	fa670793          	addi	a5,a4,-90
    2bcc:	0017b793          	seqz	a5,a5
    2bd0:	00479793          	slli	a5,a5,0x4
    2bd4:	a65ff06f          	j	2638 <main+0x1228>
    2bd8:	14810593          	addi	a1,sp,328
    2bdc:	00100513          	li	a0,1
    2be0:	1c9070ef          	jal	a5a8 <bandwidth_test.constprop.0>
    2be4:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    2be8:	00079463          	bnez	a5,2bf0 <main+0x17e0>
    2bec:	96dfe06f          	j	1558 <main+0x148>
    2bf0:	14810d13          	addi	s10,sp,328
    2bf4:	969fe06f          	j	155c <main+0x14c>
    2bf8:	00000793          	li	a5,0
    2bfc:	a3dff06f          	j	2638 <main+0x1228>

00002c00 <sprout>:
    2c00:	fe010113          	addi	sp,sp,-32
    2c04:	00812c23          	sw	s0,24(sp)
    2c08:	00068793          	mv	a5,a3
    2c0c:	00068413          	mv	s0,a3
    2c10:	00800713          	li	a4,8
    2c14:	00200693          	li	a3,2
    2c18:	00112e23          	sw	ra,28(sp)
    2c1c:	00912a23          	sw	s1,20(sp)
    2c20:	01212823          	sw	s2,16(sp)
    2c24:	01312623          	sw	s3,12(sp)
    2c28:	00050493          	mv	s1,a0
    2c2c:	00058913          	mv	s2,a1
    2c30:	00060993          	mv	s3,a2
    2c34:	0a8060ef          	jal	8cdc <gfx_fill_rect>
    2c38:	00040793          	mv	a5,s0
    2c3c:	ffe98613          	addi	a2,s3,-2
    2c40:	ffb90593          	addi	a1,s2,-5
    2c44:	00048513          	mv	a0,s1
    2c48:	00300713          	li	a4,3
    2c4c:	00500693          	li	a3,5
    2c50:	08c060ef          	jal	8cdc <gfx_fill_rect>
    2c54:	00040793          	mv	a5,s0
    2c58:	00198613          	addi	a2,s3,1
    2c5c:	ffd90593          	addi	a1,s2,-3
    2c60:	00048513          	mv	a0,s1
    2c64:	00200713          	li	a4,2
    2c68:	00400693          	li	a3,4
    2c6c:	070060ef          	jal	8cdc <gfx_fill_rect>
    2c70:	00040793          	mv	a5,s0
    2c74:	ffc98613          	addi	a2,s3,-4
    2c78:	00290593          	addi	a1,s2,2
    2c7c:	00048513          	mv	a0,s1
    2c80:	00300713          	li	a4,3
    2c84:	00500693          	li	a3,5
    2c88:	054060ef          	jal	8cdc <gfx_fill_rect>
    2c8c:	00040793          	mv	a5,s0
    2c90:	01812403          	lw	s0,24(sp)
    2c94:	01c12083          	lw	ra,28(sp)
    2c98:	fff98613          	addi	a2,s3,-1
    2c9c:	00190593          	addi	a1,s2,1
    2ca0:	00c12983          	lw	s3,12(sp)
    2ca4:	01012903          	lw	s2,16(sp)
    2ca8:	00048513          	mv	a0,s1
    2cac:	01412483          	lw	s1,20(sp)
    2cb0:	00200713          	li	a4,2
    2cb4:	00400693          	li	a3,4
    2cb8:	02010113          	addi	sp,sp,32
    2cbc:	0200606f          	j	8cdc <gfx_fill_rect>

00002cc0 <shadow>:
    2cc0:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    2cc4:	fe010113          	addi	sp,sp,-32
    2cc8:	00912a23          	sw	s1,20(sp)
    2ccc:	01212823          	sw	s2,16(sp)
    2cd0:	00112e23          	sw	ra,28(sp)
    2cd4:	00050913          	mv	s2,a0
    2cd8:	00060493          	mv	s1,a2
    2cdc:	08078463          	beqz	a5,2d64 <shadow+0xa4>
    2ce0:	00800793          	li	a5,8
    2ce4:	0ed7d263          	bge	a5,a3,2dc8 <shadow+0x108>
    2ce8:	00900793          	li	a5,9
    2cec:	0ef68663          	beq	a3,a5,2dd8 <shadow+0x118>
    2cf0:	00a00793          	li	a5,10
    2cf4:	0ef68a63          	beq	a3,a5,2de8 <shadow+0x128>
    2cf8:	00d00793          	li	a5,13
    2cfc:	0ed7de63          	bge	a5,a3,2df8 <shadow+0x138>
    2d00:	00f00793          	li	a5,15
    2d04:	10d7d263          	bge	a5,a3,2e08 <shadow+0x148>
    2d08:	b7018693          	addi	a3,gp,-1168 # 13778 <shadow_sprites+0x50>
    2d0c:	00500713          	li	a4,5
    2d10:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2d14:	00471713          	slli	a4,a4,0x4
    2d18:	00e787b3          	add	a5,a5,a4
    2d1c:	0047a603          	lw	a2,4(a5)
    2d20:	0007a703          	lw	a4,0(a5)
    2d24:	01c12083          	lw	ra,28(sp)
    2d28:	01f65893          	srli	a7,a2,0x1f
    2d2c:	00c888b3          	add	a7,a7,a2
    2d30:	01f75813          	srli	a6,a4,0x1f
    2d34:	4018d893          	srai	a7,a7,0x1
    2d38:	00e80833          	add	a6,a6,a4
    2d3c:	41148633          	sub	a2,s1,a7
    2d40:	00090513          	mv	a0,s2
    2d44:	01412483          	lw	s1,20(sp)
    2d48:	01012903          	lw	s2,16(sp)
    2d4c:	40185813          	srai	a6,a6,0x1
    2d50:	0ff00793          	li	a5,255
    2d54:	00000713          	li	a4,0
    2d58:	410585b3          	sub	a1,a1,a6
    2d5c:	02010113          	addi	sp,sp,32
    2d60:	4040606f          	j	9164 <gfx_blit_alpha>
    2d64:	00812c23          	sw	s0,24(sp)
    2d68:	01312623          	sw	s3,12(sp)
    2d6c:	00169413          	slli	s0,a3,0x1
    2d70:	40d589b3          	sub	s3,a1,a3
    2d74:	000027b7          	lui	a5,0x2
    2d78:	00040693          	mv	a3,s0
    2d7c:	00098593          	mv	a1,s3
    2d80:	92478793          	addi	a5,a5,-1756 # 1924 <main+0x514>
    2d84:	00500713          	li	a4,5
    2d88:	ffe60613          	addi	a2,a2,-2
    2d8c:	751050ef          	jal	8cdc <gfx_fill_rect>
    2d90:	ffa40693          	addi	a3,s0,-6
    2d94:	01812403          	lw	s0,24(sp)
    2d98:	01c12083          	lw	ra,28(sp)
    2d9c:	ffc48613          	addi	a2,s1,-4
    2da0:	00398593          	addi	a1,s3,3
    2da4:	01412483          	lw	s1,20(sp)
    2da8:	00c12983          	lw	s3,12(sp)
    2dac:	00090513          	mv	a0,s2
    2db0:	01012903          	lw	s2,16(sp)
    2db4:	000027b7          	lui	a5,0x2
    2db8:	92478793          	addi	a5,a5,-1756 # 1924 <main+0x514>
    2dbc:	00900713          	li	a4,9
    2dc0:	02010113          	addi	sp,sp,32
    2dc4:	7190506f          	j	8cdc <gfx_fill_rect>
    2dc8:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2dcc:	00000713          	li	a4,0
    2dd0:	b2018693          	addi	a3,gp,-1248 # 13728 <shadow_sprites>
    2dd4:	f41ff06f          	j	2d14 <shadow+0x54>
    2dd8:	b3018693          	addi	a3,gp,-1232 # 13738 <shadow_sprites+0x10>
    2ddc:	00100713          	li	a4,1
    2de0:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2de4:	f31ff06f          	j	2d14 <shadow+0x54>
    2de8:	b4018693          	addi	a3,gp,-1216 # 13748 <shadow_sprites+0x20>
    2dec:	00200713          	li	a4,2
    2df0:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2df4:	f21ff06f          	j	2d14 <shadow+0x54>
    2df8:	b5018693          	addi	a3,gp,-1200 # 13758 <shadow_sprites+0x30>
    2dfc:	00300713          	li	a4,3
    2e00:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2e04:	f11ff06f          	j	2d14 <shadow+0x54>
    2e08:	b6018693          	addi	a3,gp,-1184 # 13768 <shadow_sprites+0x40>
    2e0c:	00400713          	li	a4,4
    2e10:	b2018793          	addi	a5,gp,-1248 # 13728 <shadow_sprites>
    2e14:	f01ff06f          	j	2d14 <shadow+0x54>

00002e18 <draw_player>:
    2e18:	0105a703          	lw	a4,16(a1)
    2e1c:	0145a783          	lw	a5,20(a1)
    2e20:	fd010113          	addi	sp,sp,-48
    2e24:	02812423          	sw	s0,40(sp)
    2e28:	02912223          	sw	s1,36(sp)
    2e2c:	03212023          	sw	s2,32(sp)
    2e30:	41f7d493          	srai	s1,a5,0x1f
    2e34:	41f75913          	srai	s2,a4,0x1f
    2e38:	0285a403          	lw	s0,40(a1)
    2e3c:	0ff97913          	zext.b	s2,s2
    2e40:	0ff4f493          	zext.b	s1,s1
    2e44:	01412c23          	sw	s4,24(sp)
    2e48:	01712623          	sw	s7,12(sp)
    2e4c:	00e90933          	add	s2,s2,a4
    2e50:	00f484b3          	add	s1,s1,a5
    2e54:	02112623          	sw	ra,44(sp)
    2e58:	01312e23          	sw	s3,28(sp)
    2e5c:	01512a23          	sw	s5,20(sp)
    2e60:	01612823          	sw	s6,16(sp)
    2e64:	01912223          	sw	s9,4(sp)
    2e68:	00058b93          	mv	s7,a1
    2e6c:	00050a13          	mv	s4,a0
    2e70:	40895913          	srai	s2,s2,0x8
    2e74:	4084d493          	srai	s1,s1,0x8
    2e78:	02040063          	beqz	s0,2e98 <draw_player+0x80>
    2e7c:	0085a403          	lw	s0,8(a1)
    2e80:	00700793          	li	a5,7
    2e84:	02f44433          	div	s0,s0,a5
    2e88:	01f45793          	srli	a5,s0,0x1f
    2e8c:	00f40433          	add	s0,s0,a5
    2e90:	00147413          	andi	s0,s0,1
    2e94:	40f40433          	sub	s0,s0,a5
    2e98:	02cbab03          	lw	s6,44(s7)
    2e9c:	030baa83          	lw	s5,48(s7)
    2ea0:	0ff00993          	li	s3,255
    2ea4:	036b0533          	mul	a0,s6,s6
    2ea8:	035a87b3          	mul	a5,s5,s5
    2eac:	00f50533          	add	a0,a0,a5
    2eb0:	20d090ef          	jal	c8bc <game_isqrt>
    2eb4:	020ba783          	lw	a5,32(s7)
    2eb8:	00050c93          	mv	s9,a0
    2ebc:	02078463          	beqz	a5,2ee4 <draw_player+0xcc>
    2ec0:	008ba783          	lw	a5,8(s7)
    2ec4:	01800713          	li	a4,24
    2ec8:	00b00693          	li	a3,11
    2ecc:	02e7e7b3          	rem	a5,a5,a4
    2ed0:	16f6d263          	bge	a3,a5,3034 <draw_player+0x21c>
    2ed4:	40f70733          	sub	a4,a4,a5
    2ed8:	00371993          	slli	s3,a4,0x3
    2edc:	40e989b3          	sub	s3,s3,a4
    2ee0:	05a98993          	addi	s3,s3,90
    2ee4:	00d00693          	li	a3,13
    2ee8:	00a48613          	addi	a2,s1,10
    2eec:	00090593          	mv	a1,s2
    2ef0:	000a0513          	mv	a0,s4
    2ef4:	dcdff0ef          	jal	2cc0 <shadow>
    2ef8:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    2efc:	02079663          	bnez	a5,2f28 <draw_player+0x110>
    2f00:	020ba783          	lw	a5,32(s7)
    2f04:	12078263          	beqz	a5,3028 <draw_player+0x210>
    2f08:	008ba703          	lw	a4,8(s7)
    2f0c:	0ff00993          	li	s3,255
    2f10:	41f75793          	srai	a5,a4,0x1f
    2f14:	0037f793          	andi	a5,a5,3
    2f18:	00e787b3          	add	a5,a5,a4
    2f1c:	4027d793          	srai	a5,a5,0x2
    2f20:	0017f793          	andi	a5,a5,1
    2f24:	0c079a63          	bnez	a5,2ff8 <draw_player+0x1e0>
    2f28:	01812423          	sw	s8,8(sp)
    2f2c:	fea48613          	addi	a2,s1,-22
    2f30:	000136b7          	lui	a3,0x13
    2f34:	00098793          	mv	a5,s3
    2f38:	00100713          	li	a4,1
    2f3c:	3f468693          	addi	a3,a3,1012 # 133f4 <player>
    2f40:	40860633          	sub	a2,a2,s0
    2f44:	ff490593          	addi	a1,s2,-12
    2f48:	000a0513          	mv	a0,s4
    2f4c:	218060ef          	jal	9164 <gfx_blit_alpha>
    2f50:	01904463          	bgtz	s9,2f58 <draw_player+0x140>
    2f54:	00100c93          	li	s9,1
    2f58:	001a9413          	slli	s0,s5,0x1
    2f5c:	01540433          	add	s0,s0,s5
    2f60:	004b1c13          	slli	s8,s6,0x4
    2f64:	00241413          	slli	s0,s0,0x2
    2f68:	416c0c33          	sub	s8,s8,s6
    2f6c:	41540433          	sub	s0,s0,s5
    2f70:	039c4c33          	div	s8,s8,s9
    2f74:	000017b7          	lui	a5,0x1
    2f78:	00098813          	mv	a6,s3
    2f7c:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    2f80:	00700713          	li	a4,7
    2f84:	00900693          	li	a3,9
    2f88:	000a0513          	mv	a0,s4
    2f8c:	03944433          	div	s0,s0,s9
    2f90:	012c0c33          	add	s8,s8,s2
    2f94:	ffcc0593          	addi	a1,s8,-4
    2f98:	00940433          	add	s0,s0,s1
    2f9c:	ffd40613          	addi	a2,s0,-3
    2fa0:	715050ef          	jal	8eb4 <gfx_fill_alpha>
    2fa4:	000087b7          	lui	a5,0x8
    2fa8:	00098813          	mv	a6,s3
    2fac:	49178793          	addi	a5,a5,1169 # 8491 <render_game+0x273d>
    2fb0:	00400713          	li	a4,4
    2fb4:	00700693          	li	a3,7
    2fb8:	ffe40613          	addi	a2,s0,-2
    2fbc:	ffdc0593          	addi	a1,s8,-3
    2fc0:	000a0513          	mv	a0,s4
    2fc4:	6f1050ef          	jal	8eb4 <gfx_fill_alpha>
    2fc8:	0000f7b7          	lui	a5,0xf
    2fcc:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    2fd0:	00098813          	mv	a6,s3
    2fd4:	00200713          	li	a4,2
    2fd8:	00300693          	li	a3,3
    2fdc:	fff40613          	addi	a2,s0,-1
    2fe0:	ffec0593          	addi	a1,s8,-2
    2fe4:	000a0513          	mv	a0,s4
    2fe8:	6cd050ef          	jal	8eb4 <gfx_fill_alpha>
    2fec:	070ba783          	lw	a5,112(s7)
    2ff0:	04079a63          	bnez	a5,3044 <draw_player+0x22c>
    2ff4:	00812c03          	lw	s8,8(sp)
    2ff8:	02c12083          	lw	ra,44(sp)
    2ffc:	02812403          	lw	s0,40(sp)
    3000:	02412483          	lw	s1,36(sp)
    3004:	02012903          	lw	s2,32(sp)
    3008:	01c12983          	lw	s3,28(sp)
    300c:	01812a03          	lw	s4,24(sp)
    3010:	01412a83          	lw	s5,20(sp)
    3014:	01012b03          	lw	s6,16(sp)
    3018:	00c12b83          	lw	s7,12(sp)
    301c:	00412c83          	lw	s9,4(sp)
    3020:	03010113          	addi	sp,sp,48
    3024:	00008067          	ret
    3028:	01812423          	sw	s8,8(sp)
    302c:	0ff00993          	li	s3,255
    3030:	efdff06f          	j	2f2c <draw_player+0x114>
    3034:	00379993          	slli	s3,a5,0x3
    3038:	40f989b3          	sub	s3,s3,a5
    303c:	05a98993          	addi	s3,s3,90
    3040:	ea5ff06f          	j	2ee4 <draw_player+0xcc>
    3044:	001b1413          	slli	s0,s6,0x1
    3048:	01640433          	add	s0,s0,s6
    304c:	003a9613          	slli	a2,s5,0x3
    3050:	01560633          	add	a2,a2,s5
    3054:	00341413          	slli	s0,s0,0x3
    3058:	41640433          	sub	s0,s0,s6
    305c:	00161613          	slli	a2,a2,0x1
    3060:	03964633          	div	a2,a2,s9
    3064:	0000f7b7          	lui	a5,0xf
    3068:	00098813          	mv	a6,s3
    306c:	000a0513          	mv	a0,s4
    3070:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    3074:	00300713          	li	a4,3
    3078:	00900693          	li	a3,9
    307c:	03944433          	div	s0,s0,s9
    3080:	009604b3          	add	s1,a2,s1
    3084:	fff48613          	addi	a2,s1,-1
    3088:	01240433          	add	s0,s0,s2
    308c:	ffc40593          	addi	a1,s0,-4
    3090:	625050ef          	jal	8eb4 <gfx_fill_alpha>
    3094:	fff40593          	addi	a1,s0,-1
    3098:	02812403          	lw	s0,40(sp)
    309c:	00812c03          	lw	s8,8(sp)
    30a0:	02c12083          	lw	ra,44(sp)
    30a4:	02012903          	lw	s2,32(sp)
    30a8:	01412a83          	lw	s5,20(sp)
    30ac:	01012b03          	lw	s6,16(sp)
    30b0:	00c12b83          	lw	s7,12(sp)
    30b4:	00412c83          	lw	s9,4(sp)
    30b8:	00098813          	mv	a6,s3
    30bc:	ffc48613          	addi	a2,s1,-4
    30c0:	01c12983          	lw	s3,28(sp)
    30c4:	02412483          	lw	s1,36(sp)
    30c8:	000a0513          	mv	a0,s4
    30cc:	01812a03          	lw	s4,24(sp)
    30d0:	0000f7b7          	lui	a5,0xf
    30d4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    30d8:	00900713          	li	a4,9
    30dc:	00300693          	li	a3,3
    30e0:	03010113          	addi	sp,sp,48
    30e4:	5d10506f          	j	8eb4 <gfx_fill_alpha>

000030e8 <vnoise.constprop.0>:
    30e8:	27d4f7b7          	lui	a5,0x27d4f
    30ec:	b2f78793          	addi	a5,a5,-1233 # 27d4eb2f <__freertos_irq_stack_top+0x27c6698f>
    30f0:	4075d693          	srai	a3,a1,0x7
    30f4:	02f686b3          	mul	a3,a3,a5
    30f8:	165668b7          	lui	a7,0x16566
    30fc:	7b188893          	addi	a7,a7,1969 # 165667b1 <__freertos_irq_stack_top+0x1647e611>
    3100:	40755813          	srai	a6,a0,0x7
    3104:	5b144eb7          	lui	t4,0x5b144
    3108:	82e93737          	lui	a4,0x82e93
    310c:	333e8e93          	addi	t4,t4,819 # 5b144333 <__freertos_irq_stack_top+0x5b05c193>
    3110:	e6270713          	addi	a4,a4,-414 # 82e92e62 <__freertos_irq_stack_top+0x82daacc2>
    3114:	00151513          	slli	a0,a0,0x1
    3118:	0fe57513          	andi	a0,a0,254
    311c:	03180833          	mul	a6,a6,a7
    3120:	01d68fb3          	add	t6,a3,t4
    3124:	00f687b3          	add	a5,a3,a5
    3128:	00e686b3          	add	a3,a3,a4
    312c:	00159713          	slli	a4,a1,0x1
    3130:	18000613          	li	a2,384
    3134:	0fe77713          	andi	a4,a4,254
    3138:	40a60333          	sub	t1,a2,a0
    313c:	40e605b3          	sub	a1,a2,a4
    3140:	4bf1ae37          	lui	t3,0x4bf1a
    3144:	02a50633          	mul	a2,a0,a0
    3148:	011808b3          	add	a7,a6,a7
    314c:	010787b3          	add	a5,a5,a6
    3150:	010f8833          	add	a6,t6,a6
    3154:	01d787b3          	add	a5,a5,t4
    3158:	011f8fb3          	add	t6,t6,a7
    315c:	00d85f13          	srli	t5,a6,0xd
    3160:	f61e0e13          	addi	t3,t3,-159 # 4bf19f61 <__freertos_irq_stack_top+0x4be31dc1>
    3164:	011686b3          	add	a3,a3,a7
    3168:	010f4833          	xor	a6,t5,a6
    316c:	00dfde93          	srli	t4,t6,0xd
    3170:	00d7d513          	srli	a0,a5,0xd
    3174:	00d6d893          	srli	a7,a3,0xd
    3178:	01feceb3          	xor	t4,t4,t6
    317c:	00f547b3          	xor	a5,a0,a5
    3180:	03c80533          	mul	a0,a6,t3
    3184:	00d8c8b3          	xor	a7,a7,a3
    3188:	03ce86b3          	mul	a3,t4,t3
    318c:	01055813          	srli	a6,a0,0x10
    3190:	00a84833          	xor	a6,a6,a0
    3194:	0ff87813          	zext.b	a6,a6
    3198:	03c787b3          	mul	a5,a5,t3
    319c:	0106d513          	srli	a0,a3,0x10
    31a0:	00d54533          	xor	a0,a0,a3
    31a4:	0ff57513          	zext.b	a0,a0
    31a8:	41050533          	sub	a0,a0,a6
    31ac:	03c888b3          	mul	a7,a7,t3
    31b0:	0107d693          	srli	a3,a5,0x10
    31b4:	00f6c6b3          	xor	a3,a3,a5
    31b8:	0ff6f693          	zext.b	a3,a3
    31bc:	02c30633          	mul	a2,t1,a2
    31c0:	0108d793          	srli	a5,a7,0x10
    31c4:	0117c7b3          	xor	a5,a5,a7
    31c8:	0ff7f793          	zext.b	a5,a5
    31cc:	40d787b3          	sub	a5,a5,a3
    31d0:	02e70733          	mul	a4,a4,a4
    31d4:	40f65613          	srai	a2,a2,0xf
    31d8:	02c50533          	mul	a0,a0,a2
    31dc:	02c787b3          	mul	a5,a5,a2
    31e0:	40855513          	srai	a0,a0,0x8
    31e4:	01050533          	add	a0,a0,a6
    31e8:	02e585b3          	mul	a1,a1,a4
    31ec:	4087d793          	srai	a5,a5,0x8
    31f0:	00d787b3          	add	a5,a5,a3
    31f4:	40a787b3          	sub	a5,a5,a0
    31f8:	40f5d593          	srai	a1,a1,0xf
    31fc:	02b787b3          	mul	a5,a5,a1
    3200:	4087d793          	srai	a5,a5,0x8
    3204:	00a78533          	add	a0,a5,a0
    3208:	00008067          	ret

0000320c <panel>:
    320c:	fe010113          	addi	sp,sp,-32
    3210:	01612023          	sw	s6,0(sp)
    3214:	8101a803          	lw	a6,-2032(gp) # 13418 <fx_alpha>
    3218:	00812c23          	sw	s0,24(sp)
    321c:	00912a23          	sw	s1,20(sp)
    3220:	01212823          	sw	s2,16(sp)
    3224:	01312623          	sw	s3,12(sp)
    3228:	01412423          	sw	s4,8(sp)
    322c:	01512223          	sw	s5,4(sp)
    3230:	00112e23          	sw	ra,28(sp)
    3234:	00058413          	mv	s0,a1
    3238:	00060493          	mv	s1,a2
    323c:	00078a93          	mv	s5,a5
    3240:	00068913          	mv	s2,a3
    3244:	00050993          	mv	s3,a0
    3248:	00070a13          	mv	s4,a4
    324c:	00d585b3          	add	a1,a1,a3
    3250:	00560613          	addi	a2,a2,5
    3254:	000017b7          	lui	a5,0x1
    3258:	0a080863          	beqz	a6,3308 <panel+0xfc>
    325c:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    3260:	00500693          	li	a3,5
    3264:	06e00813          	li	a6,110
    3268:	44d050ef          	jal	8eb4 <gfx_fill_alpha>
    326c:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    3270:	00540593          	addi	a1,s0,5
    3274:	01448633          	add	a2,s1,s4
    3278:	ffb90693          	addi	a3,s2,-5
    327c:	0a078663          	beqz	a5,3328 <panel+0x11c>
    3280:	000017b7          	lui	a5,0x1
    3284:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    3288:	00500713          	li	a4,5
    328c:	06e00813          	li	a6,110
    3290:	00098513          	mv	a0,s3
    3294:	421050ef          	jal	8eb4 <gfx_fill_alpha>
    3298:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    329c:	00240593          	addi	a1,s0,2
    32a0:	00248613          	addi	a2,s1,2
    32a4:	ffc90693          	addi	a3,s2,-4
    32a8:	ffca0713          	addi	a4,s4,-4
    32ac:	0a078463          	beqz	a5,3354 <panel+0x148>
    32b0:	000017b7          	lui	a5,0x1
    32b4:	0d700813          	li	a6,215
    32b8:	10578793          	addi	a5,a5,261 # 1105 <memset+0x1>
    32bc:	00098513          	mv	a0,s3
    32c0:	3f5050ef          	jal	8eb4 <gfx_fill_alpha>
    32c4:	00040593          	mv	a1,s0
    32c8:	01812403          	lw	s0,24(sp)
    32cc:	01c12083          	lw	ra,28(sp)
    32d0:	00012b03          	lw	s6,0(sp)
    32d4:	000a8813          	mv	a6,s5
    32d8:	000a0713          	mv	a4,s4
    32dc:	00412a83          	lw	s5,4(sp)
    32e0:	00812a03          	lw	s4,8(sp)
    32e4:	00090693          	mv	a3,s2
    32e8:	00048613          	mv	a2,s1
    32ec:	01012903          	lw	s2,16(sp)
    32f0:	01412483          	lw	s1,20(sp)
    32f4:	00098513          	mv	a0,s3
    32f8:	00c12983          	lw	s3,12(sp)
    32fc:	00200793          	li	a5,2
    3300:	02010113          	addi	sp,sp,32
    3304:	2200606f          	j	9524 <gfx_border>
    3308:	8c378793          	addi	a5,a5,-1853
    330c:	00500693          	li	a3,5
    3310:	1cd050ef          	jal	8cdc <gfx_fill_rect>
    3314:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    3318:	00540593          	addi	a1,s0,5
    331c:	01448633          	add	a2,s1,s4
    3320:	ffb90693          	addi	a3,s2,-5
    3324:	f4079ee3          	bnez	a5,3280 <panel+0x74>
    3328:	000017b7          	lui	a5,0x1
    332c:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    3330:	00500713          	li	a4,5
    3334:	00098513          	mv	a0,s3
    3338:	1a5050ef          	jal	8cdc <gfx_fill_rect>
    333c:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    3340:	00240593          	addi	a1,s0,2
    3344:	00248613          	addi	a2,s1,2
    3348:	ffc90693          	addi	a3,s2,-4
    334c:	ffca0713          	addi	a4,s4,-4
    3350:	f60790e3          	bnez	a5,32b0 <panel+0xa4>
    3354:	000017b7          	lui	a5,0x1
    3358:	10578793          	addi	a5,a5,261 # 1105 <memset+0x1>
    335c:	00098513          	mv	a0,s3
    3360:	17d050ef          	jal	8cdc <gfx_fill_rect>
    3364:	f61ff06f          	j	32c4 <panel+0xb8>

00003368 <render_init>:
    3368:	f3010113          	addi	sp,sp,-208
    336c:	000185b7          	lui	a1,0x18
    3370:	1e000693          	li	a3,480
    3374:	28000613          	li	a2,640
    3378:	ec058593          	addi	a1,a1,-320 # 17ec0 <background_pixels>
    337c:	06810513          	addi	a0,sp,104
    3380:	0c112623          	sw	ra,204(sp)
    3384:	0b312e23          	sw	s3,188(sp)
    3388:	0b412c23          	sw	s4,184(sp)
    338c:	0b512a23          	sw	s5,180(sp)
    3390:	0b812423          	sw	s8,168(sp)
    3394:	0b912223          	sw	s9,164(sp)
    3398:	0ba12023          	sw	s10,160(sp)
    339c:	09b12e23          	sw	s11,156(sp)
    33a0:	0c812423          	sw	s0,200(sp)
    33a4:	0c912223          	sw	s1,196(sp)
    33a8:	0d212023          	sw	s2,192(sp)
    33ac:	0b612823          	sw	s6,176(sp)
    33b0:	0b712623          	sw	s7,172(sp)
    33b4:	0a1050ef          	jal	8c54 <gfx_begin>
    33b8:	000017b7          	lui	a5,0x1
    33bc:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    33c0:	1e000713          	li	a4,480
    33c4:	28000693          	li	a3,640
    33c8:	00000593          	li	a1,0
    33cc:	06810513          	addi	a0,sp,104
    33d0:	00000613          	li	a2,0
    33d4:	109050ef          	jal	8cdc <gfx_fill_rect>
    33d8:	06812e83          	lw	t4,104(sp)
    33dc:	00002737          	lui	a4,0x2
    33e0:	c121b537          	lui	a0,0xc121b
    33e4:	004e9793          	slli	a5,t4,0x4
    33e8:	41d787b3          	sub	a5,a5,t4
    33ec:	00279a93          	slli	s5,a5,0x2
    33f0:	00100793          	li	a5,1
    33f4:	00f12823          	sw	a5,16(sp)
    33f8:	07012783          	lw	a5,112(sp)
    33fc:	e8f6a837          	lui	a6,0xe8f6a
    3400:	082f38b7          	lui	a7,0x82f3
    3404:	01c78793          	addi	a5,a5,28
    3408:	04f12423          	sw	a5,72(sp)
    340c:	81770793          	addi	a5,a4,-2025 # 1817 <main+0x407>
    3410:	30042337          	lui	t1,0x30042
    3414:	02f12a23          	sw	a5,52(sp)
    3418:	000137b7          	lui	a5,0x13
    341c:	00000593          	li	a1,0
    3420:	0b188893          	addi	a7,a7,177 # 82f30b1 <__freertos_irq_stack_top+0x820af11>
    3424:	3c9a29b7          	lui	s3,0x3c9a2
    3428:	27d4fa37          	lui	s4,0x27d4f
    342c:	16566db7          	lui	s11,0x16566
    3430:	4bf1ad37          	lui	s10,0x4bf1a
    3434:	03c00693          	li	a3,60
    3438:	31d50513          	addi	a0,a0,797 # c121b31d <__freertos_irq_stack_top+0xc113317d>
    343c:	e4c80813          	addi	a6,a6,-436 # e8f69e4c <__freertos_irq_stack_top+0xe8e81cac>
    3440:	be030313          	addi	t1,t1,-1056 # 30041be0 <__freertos_irq_stack_top+0x2ff59a40>
    3444:	b9078793          	addi	a5,a5,-1136 # 12b90 <bayer.4>
    3448:	05112c23          	sw	a7,88(sp)
    344c:	06798993          	addi	s3,s3,103 # 3c9a2067 <__freertos_irq_stack_top+0x3c8b9ec7>
    3450:	00058893          	mv	a7,a1
    3454:	00012623          	sw	zero,12(sp)
    3458:	00200c13          	li	s8,2
    345c:	b2fa0a13          	addi	s4,s4,-1233 # 27d4eb2f <__freertos_irq_stack_top+0x27c6698f>
    3460:	7b1d8d93          	addi	s11,s11,1969 # 165667b1 <__freertos_irq_stack_top+0x1647e611>
    3464:	f61d0d13          	addi	s10,s10,-159 # 4bf19f61 <__freertos_irq_stack_top+0x4be31dc1>
    3468:	0ff00c93          	li	s9,255
    346c:	02f12c23          	sw	a5,56(sp)
    3470:	05d12623          	sw	t4,76(sp)
    3474:	04a12823          	sw	a0,80(sp)
    3478:	05012a23          	sw	a6,84(sp)
    347c:	04612e23          	sw	t1,92(sp)
    3480:	00068593          	mv	a1,a3
    3484:	00859793          	slli	a5,a1,0x8
    3488:	4067d713          	srai	a4,a5,0x6
    348c:	0fc77713          	andi	a4,a4,252
    3490:	4047d793          	srai	a5,a5,0x4
    3494:	02e70533          	mul	a0,a4,a4
    3498:	0f07f793          	andi	a5,a5,240
    349c:	18000693          	li	a3,384
    34a0:	40e68733          	sub	a4,a3,a4
    34a4:	40f686b3          	sub	a3,a3,a5
    34a8:	1ba00e13          	li	t3,442
    34ac:	4065d813          	srai	a6,a1,0x6
    34b0:	4045d613          	srai	a2,a1,0x4
    34b4:	40be0bb3          	sub	s7,t3,a1
    34b8:	04812e03          	lw	t3,72(sp)
    34bc:	02f787b3          	mul	a5,a5,a5
    34c0:	001a9313          	slli	t1,s5,0x1
    34c4:	006e0b33          	add	s6,t3,t1
    34c8:	03812303          	lw	t1,56(sp)
    34cc:	00289893          	slli	a7,a7,0x2
    34d0:	00098493          	mv	s1,s3
    34d4:	011308b3          	add	a7,t1,a7
    34d8:	03112823          	sw	a7,48(sp)
    34dc:	05012883          	lw	a7,80(sp)
    34e0:	00e00413          	li	s0,14
    34e4:	03480833          	mul	a6,a6,s4
    34e8:	00b12a23          	sw	a1,20(sp)
    34ec:	03312e23          	sw	s3,60(sp)
    34f0:	05512023          	sw	s5,64(sp)
    34f4:	05412223          	sw	s4,68(sp)
    34f8:	03460633          	mul	a2,a2,s4
    34fc:	011808b3          	add	a7,a6,a7
    3500:	03112623          	sw	a7,44(sp)
    3504:	05412883          	lw	a7,84(sp)
    3508:	01180833          	add	a6,a6,a7
    350c:	03012423          	sw	a6,40(sp)
    3510:	02a70733          	mul	a4,a4,a0
    3514:	05812503          	lw	a0,88(sp)
    3518:	00a60533          	add	a0,a2,a0
    351c:	02a12023          	sw	a0,32(sp)
    3520:	05c12503          	lw	a0,92(sp)
    3524:	00a60633          	add	a2,a2,a0
    3528:	02f686b3          	mul	a3,a3,a5
    352c:	40f75793          	srai	a5,a4,0xf
    3530:	00f12e23          	sw	a5,28(sp)
    3534:	02c12223          	sw	a2,36(sp)
    3538:	40f6d793          	srai	a5,a3,0xf
    353c:	00f12c23          	sw	a5,24(sp)
    3540:	40645813          	srai	a6,s0,0x6
    3544:	03b80833          	mul	a6,a6,s11
    3548:	40445613          	srai	a2,s0,0x4
    354c:	00841793          	slli	a5,s0,0x8
    3550:	4067d893          	srai	a7,a5,0x6
    3554:	4047d793          	srai	a5,a5,0x4
    3558:	0fc8f893          	andi	a7,a7,252
    355c:	0f07f793          	andi	a5,a5,240
    3560:	18000693          	li	a3,384
    3564:	41168733          	sub	a4,a3,a7
    3568:	40f686b3          	sub	a3,a3,a5
    356c:	03b60633          	mul	a2,a2,s11
    3570:	00d12423          	sw	a3,8(sp)
    3574:	02c12683          	lw	a3,44(sp)
    3578:	01b800b3          	add	ra,a6,s11
    357c:	02012303          	lw	t1,32(sp)
    3580:	00d80ab3          	add	s5,a6,a3
    3584:	00d08a33          	add	s4,ra,a3
    3588:	02812683          	lw	a3,40(sp)
    358c:	00e12223          	sw	a4,4(sp)
    3590:	00dad393          	srli	t2,s5,0xd
    3594:	00d80833          	add	a6,a6,a3
    3598:	00d080b3          	add	ra,ra,a3
    359c:	01b606b3          	add	a3,a2,s11
    35a0:	006609b3          	add	s3,a2,t1
    35a4:	00668933          	add	s2,a3,t1
    35a8:	02412303          	lw	t1,36(sp)
    35ac:	031888b3          	mul	a7,a7,a7
    35b0:	00d85f93          	srli	t6,a6,0xd
    35b4:	006686b3          	add	a3,a3,t1
    35b8:	00d6d713          	srli	a4,a3,0xd
    35bc:	00d746b3          	xor	a3,a4,a3
    35c0:	0153c3b3          	xor	t2,t2,s5
    35c4:	00d0df13          	srli	t5,ra,0xd
    35c8:	010fcfb3          	xor	t6,t6,a6
    35cc:	00da5293          	srli	t0,s4,0xd
    35d0:	001f4f33          	xor	t5,t5,ra
    35d4:	03a686b3          	mul	a3,a3,s10
    35d8:	0142c2b3          	xor	t0,t0,s4
    35dc:	00412703          	lw	a4,4(sp)
    35e0:	00d9de93          	srli	t4,s3,0xd
    35e4:	00660633          	add	a2,a2,t1
    35e8:	00d95e13          	srli	t3,s2,0xd
    35ec:	013eceb3          	xor	t4,t4,s3
    35f0:	00d65313          	srli	t1,a2,0xd
    35f4:	012e4e33          	xor	t3,t3,s2
    35f8:	00c34333          	xor	t1,t1,a2
    35fc:	03a383b3          	mul	t2,t2,s10
    3600:	01412583          	lw	a1,20(sp)
    3604:	00040513          	mv	a0,s0
    3608:	02f787b3          	mul	a5,a5,a5
    360c:	0103d613          	srli	a2,t2,0x10
    3610:	007643b3          	xor	t2,a2,t2
    3614:	0ff3f393          	zext.b	t2,t2
    3618:	03af8fb3          	mul	t6,t6,s10
    361c:	03a282b3          	mul	t0,t0,s10
    3620:	010fd613          	srli	a2,t6,0x10
    3624:	01f64fb3          	xor	t6,a2,t6
    3628:	0fffff93          	zext.b	t6,t6
    362c:	03af0f33          	mul	t5,t5,s10
    3630:	0102d813          	srli	a6,t0,0x10
    3634:	00584833          	xor	a6,a6,t0
    3638:	0ff87813          	zext.b	a6,a6
    363c:	40780833          	sub	a6,a6,t2
    3640:	03ae8eb3          	mul	t4,t4,s10
    3644:	010f5613          	srli	a2,t5,0x10
    3648:	01e64f33          	xor	t5,a2,t5
    364c:	0fff7f13          	zext.b	t5,t5
    3650:	41ff0f33          	sub	t5,t5,t6
    3654:	03170733          	mul	a4,a4,a7
    3658:	0106d893          	srli	a7,a3,0x10
    365c:	00d8c8b3          	xor	a7,a7,a3
    3660:	00812683          	lw	a3,8(sp)
    3664:	010ed613          	srli	a2,t4,0x10
    3668:	01d64eb3          	xor	t4,a2,t4
    366c:	0ffefe93          	zext.b	t4,t4
    3670:	0ff8f893          	zext.b	a7,a7
    3674:	03ae0e33          	mul	t3,t3,s10
    3678:	40f75713          	srai	a4,a4,0xf
    367c:	03a30333          	mul	t1,t1,s10
    3680:	010e5613          	srli	a2,t3,0x10
    3684:	01c64633          	xor	a2,a2,t3
    3688:	0ff67613          	zext.b	a2,a2
    368c:	41d60633          	sub	a2,a2,t4
    3690:	02f687b3          	mul	a5,a3,a5
    3694:	01035e13          	srli	t3,t1,0x10
    3698:	006e4e33          	xor	t3,t3,t1
    369c:	0ffe7e13          	zext.b	t3,t3
    36a0:	41c888b3          	sub	a7,a7,t3
    36a4:	02e80833          	mul	a6,a6,a4
    36a8:	40f7d793          	srai	a5,a5,0xf
    36ac:	02ef0733          	mul	a4,t5,a4
    36b0:	40885813          	srai	a6,a6,0x8
    36b4:	00780833          	add	a6,a6,t2
    36b8:	02f886b3          	mul	a3,a7,a5
    36bc:	40875713          	srai	a4,a4,0x8
    36c0:	02f60633          	mul	a2,a2,a5
    36c4:	01f707b3          	add	a5,a4,t6
    36c8:	41078733          	sub	a4,a5,a6
    36cc:	01c12783          	lw	a5,28(sp)
    36d0:	4086d693          	srai	a3,a3,0x8
    36d4:	01c686b3          	add	a3,a3,t3
    36d8:	02f707b3          	mul	a5,a4,a5
    36dc:	40865613          	srai	a2,a2,0x8
    36e0:	01812703          	lw	a4,24(sp)
    36e4:	01d60633          	add	a2,a2,t4
    36e8:	40c686b3          	sub	a3,a3,a2
    36ec:	02e686b3          	mul	a3,a3,a4
    36f0:	4087d793          	srai	a5,a5,0x8
    36f4:	010787b3          	add	a5,a5,a6
    36f8:	00279913          	slli	s2,a5,0x2
    36fc:	00f90933          	add	s2,s2,a5
    3700:	4086d693          	srai	a3,a3,0x8
    3704:	00c686b3          	add	a3,a3,a2
    3708:	00169793          	slli	a5,a3,0x1
    370c:	00d787b3          	add	a5,a5,a3
    3710:	00f90933          	add	s2,s2,a5
    3714:	9d5ff0ef          	jal	30e8 <vnoise.constprop.0>
    3718:	00d4d313          	srli	t1,s1,0xd
    371c:	00934333          	xor	t1,t1,s1
    3720:	03a30333          	mul	t1,t1,s10
    3724:	40395993          	srai	s3,s2,0x3
    3728:	00299893          	slli	a7,s3,0x2
    372c:	00399813          	slli	a6,s3,0x3
    3730:	00199593          	slli	a1,s3,0x1
    3734:	013888b3          	add	a7,a7,s3
    3738:	41380833          	sub	a6,a6,s3
    373c:	013585b3          	add	a1,a1,s3
    3740:	41f8d793          	srai	a5,a7,0x1f
    3744:	41f85613          	srai	a2,a6,0x1f
    3748:	41f5d693          	srai	a3,a1,0x1f
    374c:	00281813          	slli	a6,a6,0x2
    3750:	0ff6f693          	zext.b	a3,a3
    3754:	01035713          	srli	a4,t1,0x10
    3758:	00289893          	slli	a7,a7,0x2
    375c:	0ff7f793          	zext.b	a5,a5
    3760:	0ff67613          	zext.b	a2,a2
    3764:	00259593          	slli	a1,a1,0x2
    3768:	01060633          	add	a2,a2,a6
    376c:	00674733          	xor	a4,a4,t1
    3770:	00b68833          	add	a6,a3,a1
    3774:	011787b3          	add	a5,a5,a7
    3778:	00f77713          	andi	a4,a4,15
    377c:	4087d793          	srai	a5,a5,0x8
    3780:	40865613          	srai	a2,a2,0x8
    3784:	40885813          	srai	a6,a6,0x8
    3788:	f6a50693          	addi	a3,a0,-150
    378c:	ff870713          	addi	a4,a4,-8
    3790:	01e78793          	addi	a5,a5,30
    3794:	03260613          	addi	a2,a2,50
    3798:	02a80813          	addi	a6,a6,42
    379c:	08d05a63          	blez	a3,3830 <render_init+0x4c8>
    37a0:	00869513          	slli	a0,a3,0x8
    37a4:	03412683          	lw	a3,52(sp)
    37a8:	10000593          	li	a1,256
    37ac:	00a6c663          	blt	a3,a0,37b8 <render_init+0x450>
    37b0:	01800693          	li	a3,24
    37b4:	02d545b3          	div	a1,a0,a3
    37b8:	40795913          	srai	s2,s2,0x7
    37bc:	04c90313          	addi	t1,s2,76
    37c0:	40f30333          	sub	t1,t1,a5
    37c4:	03e90913          	addi	s2,s2,62
    37c8:	40c906b3          	sub	a3,s2,a2
    37cc:	02e00513          	li	a0,46
    37d0:	410508b3          	sub	a7,a0,a6
    37d4:	00171513          	slli	a0,a4,0x1
    37d8:	00e50533          	add	a0,a0,a4
    37dc:	01f55713          	srli	a4,a0,0x1f
    37e0:	00a70733          	add	a4,a4,a0
    37e4:	40175713          	srai	a4,a4,0x1
    37e8:	02b30333          	mul	t1,t1,a1
    37ec:	02b686b3          	mul	a3,a3,a1
    37f0:	41f35513          	srai	a0,t1,0x1f
    37f4:	0ff57513          	zext.b	a0,a0
    37f8:	00650533          	add	a0,a0,t1
    37fc:	40855513          	srai	a0,a0,0x8
    3800:	00a787b3          	add	a5,a5,a0
    3804:	02b885b3          	mul	a1,a7,a1
    3808:	41f6d513          	srai	a0,a3,0x1f
    380c:	0ff57513          	zext.b	a0,a0
    3810:	00d506b3          	add	a3,a0,a3
    3814:	4086d693          	srai	a3,a3,0x8
    3818:	00d60633          	add	a2,a2,a3
    381c:	41f5d693          	srai	a3,a1,0x1f
    3820:	0ff6f693          	zext.b	a3,a3
    3824:	00b686b3          	add	a3,a3,a1
    3828:	4086d693          	srai	a3,a3,0x8
    382c:	00d80833          	add	a6,a6,a3
    3830:	01f75693          	srli	a3,a4,0x1f
    3834:	00e686b3          	add	a3,a3,a4
    3838:	27100593          	li	a1,625
    383c:	4016d693          	srai	a3,a3,0x1
    3840:	408588b3          	sub	a7,a1,s0
    3844:	ff240593          	addi	a1,s0,-14
    3848:	00f70533          	add	a0,a4,a5
    384c:	010686b3          	add	a3,a3,a6
    3850:	00c70733          	add	a4,a4,a2
    3854:	00b8d463          	bge	a7,a1,385c <render_init+0x4f4>
    3858:	00088593          	mv	a1,a7
    385c:	01012783          	lw	a5,16(sp)
    3860:	00f5c463          	blt	a1,a5,3868 <render_init+0x500>
    3864:	00c12583          	lw	a1,12(sp)
    3868:	00bbd463          	bge	s7,a1,3870 <render_init+0x508>
    386c:	000b8593          	mv	a1,s7
    3870:	01b00793          	li	a5,27
    3874:	02b7cc63          	blt	a5,a1,38ac <render_init+0x544>
    3878:	01c00793          	li	a5,28
    387c:	02e00613          	li	a2,46
    3880:	40b787b3          	sub	a5,a5,a1
    3884:	02c787b3          	mul	a5,a5,a2
    3888:	fe400613          	li	a2,-28
    388c:	02c7c7b3          	div	a5,a5,a2
    3890:	10078793          	addi	a5,a5,256
    3894:	02f50533          	mul	a0,a0,a5
    3898:	02f70733          	mul	a4,a4,a5
    389c:	40855513          	srai	a0,a0,0x8
    38a0:	02f687b3          	mul	a5,a3,a5
    38a4:	40875713          	srai	a4,a4,0x8
    38a8:	4087d693          	srai	a3,a5,0x8
    38ac:	402c5793          	srai	a5,s8,0x2
    38b0:	00e787b3          	add	a5,a5,a4
    38b4:	401c5c13          	srai	s8,s8,0x1
    38b8:	06fcd463          	bge	s9,a5,3920 <render_init+0x5b8>
    38bc:	0ff00793          	li	a5,255
    38c0:	4027d793          	srai	a5,a5,0x2
    38c4:	00ac0533          	add	a0,s8,a0
    38c8:	00579793          	slli	a5,a5,0x5
    38cc:	06acd863          	bge	s9,a0,393c <render_init+0x5d4>
    38d0:	0ff00513          	li	a0,255
    38d4:	40355513          	srai	a0,a0,0x3
    38d8:	00b51513          	slli	a0,a0,0xb
    38dc:	00dc0c33          	add	s8,s8,a3
    38e0:	00a7e7b3          	or	a5,a5,a0
    38e4:	078cdc63          	bge	s9,s8,395c <render_init+0x5f4>
    38e8:	0ff00c13          	li	s8,255
    38ec:	403c5c13          	srai	s8,s8,0x3
    38f0:	0187e7b3          	or	a5,a5,s8
    38f4:	00fb1023          	sh	a5,0(s6)
    38f8:	00140413          	addi	s0,s0,1
    38fc:	27200793          	li	a5,626
    3900:	01b484b3          	add	s1,s1,s11
    3904:	002b0b13          	addi	s6,s6,2
    3908:	08f40063          	beq	s0,a5,3988 <render_init+0x620>
    390c:	03012703          	lw	a4,48(sp)
    3910:	00347793          	andi	a5,s0,3
    3914:	00f707b3          	add	a5,a4,a5
    3918:	00078c03          	lb	s8,0(a5)
    391c:	c25ff06f          	j	3540 <render_init+0x1d8>
    3920:	fff7c713          	not	a4,a5
    3924:	41f75713          	srai	a4,a4,0x1f
    3928:	00e7f7b3          	and	a5,a5,a4
    392c:	4027d793          	srai	a5,a5,0x2
    3930:	00ac0533          	add	a0,s8,a0
    3934:	00579793          	slli	a5,a5,0x5
    3938:	f8accce3          	blt	s9,a0,38d0 <render_init+0x568>
    393c:	fff54713          	not	a4,a0
    3940:	41f75713          	srai	a4,a4,0x1f
    3944:	00e57533          	and	a0,a0,a4
    3948:	40355513          	srai	a0,a0,0x3
    394c:	00b51513          	slli	a0,a0,0xb
    3950:	00dc0c33          	add	s8,s8,a3
    3954:	00a7e7b3          	or	a5,a5,a0
    3958:	f98cc8e3          	blt	s9,s8,38e8 <render_init+0x580>
    395c:	fffc4713          	not	a4,s8
    3960:	41f75713          	srai	a4,a4,0x1f
    3964:	00ec7c33          	and	s8,s8,a4
    3968:	403c5c13          	srai	s8,s8,0x3
    396c:	0187e7b3          	or	a5,a5,s8
    3970:	00fb1023          	sh	a5,0(s6)
    3974:	00140413          	addi	s0,s0,1
    3978:	27200793          	li	a5,626
    397c:	01b484b3          	add	s1,s1,s11
    3980:	002b0b13          	addi	s6,s6,2
    3984:	f8f414e3          	bne	s0,a5,390c <render_init+0x5a4>
    3988:	00c12703          	lw	a4,12(sp)
    398c:	04012a83          	lw	s5,64(sp)
    3990:	01412583          	lw	a1,20(sp)
    3994:	00170713          	addi	a4,a4,1
    3998:	00e12623          	sw	a4,12(sp)
    399c:	04c12703          	lw	a4,76(sp)
    39a0:	03c12983          	lw	s3,60(sp)
    39a4:	04412a03          	lw	s4,68(sp)
    39a8:	00ea8ab3          	add	s5,s5,a4
    39ac:	01012703          	lw	a4,16(sp)
    39b0:	00158593          	addi	a1,a1,1
    39b4:	1bb00793          	li	a5,443
    39b8:	00170713          	addi	a4,a4,1
    39bc:	00e12823          	sw	a4,16(sp)
    39c0:	014989b3          	add	s3,s3,s4
    39c4:	00f58e63          	beq	a1,a5,39e0 <render_init+0x678>
    39c8:	03812703          	lw	a4,56(sp)
    39cc:	0035f893          	andi	a7,a1,3
    39d0:	00289793          	slli	a5,a7,0x2
    39d4:	00f707b3          	add	a5,a4,a5
    39d8:	00278c03          	lb	s8,2(a5)
    39dc:	aa9ff06f          	j	3484 <render_init+0x11c>
    39e0:	00003737          	lui	a4,0x3
    39e4:	000060b7          	lui	ra,0x6
    39e8:	000047b7          	lui	a5,0x4
    39ec:	55667d37          	lui	s10,0x55667
    39f0:	00196cb7          	lui	s9,0x196
    39f4:	3c6efdb7          	lui	s11,0x3c6ef
    39f8:	26670713          	addi	a4,a4,614 # 3266 <panel+0x5a>
    39fc:	40908093          	addi	ra,ra,1033 # 6409 <render_game+0x6b5>
    3a00:	ac778793          	addi	a5,a5,-1337 # 3ac7 <render_init+0x75f>
    3a04:	2bc00b13          	li	s6,700
    3a08:	788d0d13          	addi	s10,s10,1928 # 55667788 <__freertos_irq_stack_top+0x5557f5e8>
    3a0c:	60dc8c93          	addi	s9,s9,1549 # 19660d <__freertos_irq_stack_top+0xae46d>
    3a10:	35fd8d93          	addi	s11,s11,863 # 3c6ef35f <__freertos_irq_stack_top+0x3c6071bf>
    3a14:	00e12423          	sw	a4,8(sp)
    3a18:	00112223          	sw	ra,4(sp)
    3a1c:	00f12623          	sw	a5,12(sp)
    3a20:	00c0006f          	j	3a2c <render_init+0x6c4>
    3a24:	fffb0b13          	addi	s6,s6,-1
    3a28:	280b0263          	beqz	s6,3cac <render_init+0x944>
    3a2c:	039d07b3          	mul	a5,s10,s9
    3a30:	25400713          	li	a4,596
    3a34:	01b787b3          	add	a5,a5,s11
    3a38:	03978d33          	mul	s10,a5,s9
    3a3c:	0087d793          	srli	a5,a5,0x8
    3a40:	01bd0d33          	add	s10,s10,s11
    3a44:	02e7fc33          	remu	s8,a5,a4
    3a48:	008d5493          	srli	s1,s10,0x8
    3a4c:	17000793          	li	a5,368
    3a50:	02f4f4b3          	remu	s1,s1,a5
    3a54:	016c0913          	addi	s2,s8,22
    3a58:	00090513          	mv	a0,s2
    3a5c:	04648413          	addi	s0,s1,70
    3a60:	00040593          	mv	a1,s0
    3a64:	e84ff0ef          	jal	30e8 <vnoise.constprop.0>
    3a68:	08c00793          	li	a5,140
    3a6c:	faa7cce3          	blt	a5,a0,3a24 <render_init+0x6bc>
    3a70:	00300713          	li	a4,3
    3a74:	01cd5793          	srli	a5,s10,0x1c
    3a78:	02e7e7b3          	rem	a5,a5,a4
    3a7c:	007d1713          	slli	a4,s10,0x7
    3a80:	fff00b93          	li	s7,-1
    3a84:	00378993          	addi	s3,a5,3
    3a88:	4019da13          	srai	s4,s3,0x1
    3a8c:	00075463          	bgez	a4,3a94 <render_init+0x72c>
    3a90:	00100b93          	li	s7,1
    3a94:	41390733          	sub	a4,s2,s3
    3a98:	00070913          	mv	s2,a4
    3a9c:	00000a93          	li	s5,0
    3aa0:	00e12a23          	sw	a4,20(sp)
    3aa4:	00000693          	li	a3,0
    3aa8:	41700733          	neg	a4,s7
    3aac:	00912c23          	sw	s1,24(sp)
    3ab0:	03312023          	sw	s3,32(sp)
    3ab4:	03712223          	sw	s7,36(sp)
    3ab8:	03b12423          	sw	s11,40(sp)
    3abc:	000a0b93          	mv	s7,s4
    3ac0:	01812823          	sw	s8,16(sp)
    3ac4:	000d0a13          	mv	s4,s10
    3ac8:	00f12e23          	sw	a5,28(sp)
    3acc:	000b0d13          	mv	s10,s6
    3ad0:	00068493          	mv	s1,a3
    3ad4:	000a8b13          	mv	s6,s5
    3ad8:	000c8d93          	mv	s11,s9
    3adc:	00040a93          	mv	s5,s0
    3ae0:	00070993          	mv	s3,a4
    3ae4:	00090413          	mv	s0,s2
    3ae8:	009a5933          	srl	s2,s4,s1
    3aec:	00397913          	andi	s2,s2,3
    3af0:	417b0733          	sub	a4,s6,s7
    3af4:	00173713          	seqz	a4,a4
    3af8:	004b0f93          	addi	t6,s6,4
    3afc:	00290913          	addi	s2,s2,2
    3b00:	00e90933          	add	s2,s2,a4
    3b04:	01fa5fb3          	srl	t6,s4,t6
    3b08:	412a8cb3          	sub	s9,s5,s2
    3b0c:	001fff93          	andi	t6,t6,1
    3b10:	00c12783          	lw	a5,12(sp)
    3b14:	00100693          	li	a3,1
    3b18:	00040593          	mv	a1,s0
    3b1c:	06810513          	addi	a0,sp,104
    3b20:	00090713          	mv	a4,s2
    3b24:	001c8613          	addi	a2,s9,1
    3b28:	000f9463          	bnez	t6,3b30 <render_init+0x7c8>
    3b2c:	0e90106f          	j	5414 <__stack_size+0x1414>
    3b30:	1ac050ef          	jal	8cdc <gfx_fill_rect>
    3b34:	00492593          	slti	a1,s2,4
    3b38:	0015b593          	seqz	a1,a1
    3b3c:	00412783          	lw	a5,4(sp)
    3b40:	40b005b3          	neg	a1,a1
    3b44:	00b9f5b3          	and	a1,s3,a1
    3b48:	00100713          	li	a4,1
    3b4c:	00100693          	li	a3,1
    3b50:	000c8613          	mv	a2,s9
    3b54:	008585b3          	add	a1,a1,s0
    3b58:	06810513          	addi	a0,sp,104
    3b5c:	180050ef          	jal	8cdc <gfx_fill_rect>
    3b60:	001b0813          	addi	a6,s6,1
    3b64:	01785463          	bge	a6,s7,3b6c <render_init+0x804>
    3b68:	0f90106f          	j	5460 <__stack_size+0x1460>
    3b6c:	01012c03          	lw	s8,16(sp)
    3b70:	01c12783          	lw	a5,28(sp)
    3b74:	000d0b13          	mv	s6,s10
    3b78:	013c0913          	addi	s2,s8,19
    3b7c:	000a8413          	mv	s0,s5
    3b80:	000a0d13          	mv	s10,s4
    3b84:	000d8c93          	mv	s9,s11
    3b88:	000b8a13          	mv	s4,s7
    3b8c:	02012983          	lw	s3,32(sp)
    3b90:	02412b83          	lw	s7,36(sp)
    3b94:	02812d83          	lw	s11,40(sp)
    3b98:	00080a93          	mv	s5,a6
    3b9c:	40f90933          	sub	s2,s2,a5
    3ba0:	01612e23          	sw	s6,28(sp)
    3ba4:	03c0006f          	j	3be0 <render_init+0x878>
    3ba8:	134050ef          	jal	8cdc <gfx_fill_rect>
    3bac:	01012e03          	lw	t3,16(sp)
    3bb0:	004c2593          	slti	a1,s8,4
    3bb4:	00412783          	lw	a5,4(sp)
    3bb8:	fff58593          	addi	a1,a1,-1
    3bbc:	00bbf5b3          	and	a1,s7,a1
    3bc0:	00100713          	li	a4,1
    3bc4:	00100693          	li	a3,1
    3bc8:	000e0613          	mv	a2,t3
    3bcc:	009585b3          	add	a1,a1,s1
    3bd0:	06810513          	addi	a0,sp,104
    3bd4:	001a8a93          	addi	s5,s5,1
    3bd8:	104050ef          	jal	8cdc <gfx_fill_rect>
    3bdc:	0b3ad063          	bge	s5,s3,3c7c <render_init+0x914>
    3be0:	001a9593          	slli	a1,s5,0x1
    3be4:	01558c33          	add	s8,a1,s5
    3be8:	018d5c33          	srl	s8,s10,s8
    3bec:	003c7c13          	andi	s8,s8,3
    3bf0:	415a0733          	sub	a4,s4,s5
    3bf4:	00173713          	seqz	a4,a4
    3bf8:	002c0c13          	addi	s8,s8,2
    3bfc:	00ec0c33          	add	s8,s8,a4
    3c00:	004a8813          	addi	a6,s5,4
    3c04:	41840e33          	sub	t3,s0,s8
    3c08:	010d5833          	srl	a6,s10,a6
    3c0c:	00b904b3          	add	s1,s2,a1
    3c10:	00187813          	andi	a6,a6,1
    3c14:	01c12823          	sw	t3,16(sp)
    3c18:	00c12783          	lw	a5,12(sp)
    3c1c:	00100693          	li	a3,1
    3c20:	06810513          	addi	a0,sp,104
    3c24:	000e0b13          	mv	s6,t3
    3c28:	000c0713          	mv	a4,s8
    3c2c:	001e0613          	addi	a2,t3,1
    3c30:	00048593          	mv	a1,s1
    3c34:	f6081ae3          	bnez	a6,3ba8 <render_init+0x840>
    3c38:	00812783          	lw	a5,8(sp)
    3c3c:	001e0613          	addi	a2,t3,1
    3c40:	00100693          	li	a3,1
    3c44:	06810513          	addi	a0,sp,104
    3c48:	094050ef          	jal	8cdc <gfx_fill_rect>
    3c4c:	004c2593          	slti	a1,s8,4
    3c50:	00412783          	lw	a5,4(sp)
    3c54:	fff58593          	addi	a1,a1,-1
    3c58:	00bbf5b3          	and	a1,s7,a1
    3c5c:	00100713          	li	a4,1
    3c60:	00100693          	li	a3,1
    3c64:	000b0613          	mv	a2,s6
    3c68:	009585b3          	add	a1,a1,s1
    3c6c:	06810513          	addi	a0,sp,104
    3c70:	001a8a93          	addi	s5,s5,1
    3c74:	068050ef          	jal	8cdc <gfx_fill_rect>
    3c78:	f73ac4e3          	blt	s5,s3,3be0 <render_init+0x878>
    3c7c:	01812483          	lw	s1,24(sp)
    3c80:	01c12b03          	lw	s6,28(sp)
    3c84:	01412583          	lw	a1,20(sp)
    3c88:	000027b7          	lui	a5,0x2
    3c8c:	94478793          	addi	a5,a5,-1724 # 1944 <main+0x534>
    3c90:	00100713          	li	a4,1
    3c94:	00199693          	slli	a3,s3,0x1
    3c98:	04748613          	addi	a2,s1,71
    3c9c:	06810513          	addi	a0,sp,104
    3ca0:	fffb0b13          	addi	s6,s6,-1
    3ca4:	038050ef          	jal	8cdc <gfx_fill_rect>
    3ca8:	d80b12e3          	bnez	s6,3a2c <render_init+0x6c4>
    3cac:	000047b7          	lui	a5,0x4
    3cb0:	00005637          	lui	a2,0x5
    3cb4:	00196cb7          	lui	s9,0x196
    3cb8:	3c6efc37          	lui	s8,0x3c6ef
    3cbc:	00004db7          	lui	s11,0x4
    3cc0:	b0778793          	addi	a5,a5,-1273 # 3b07 <render_init+0x79f>
    3cc4:	b8860613          	addi	a2,a2,-1144 # 4b88 <__stack_size+0xb88>
    3cc8:	07800913          	li	s2,120
    3ccc:	60dc8c93          	addi	s9,s9,1549 # 19660d <__freertos_irq_stack_top+0xae46d>
    3cd0:	35fc0c13          	addi	s8,s8,863 # 3c6ef35f <__freertos_irq_stack_top+0x3c6071bf>
    3cd4:	24e00b13          	li	s6,590
    3cd8:	16a00a93          	li	s5,362
    3cdc:	08c00a13          	li	s4,140
    3ce0:	348d8d93          	addi	s11,s11,840 # 4348 <__stack_size+0x348>
    3ce4:	00f12223          	sw	a5,4(sp)
    3ce8:	00c12423          	sw	a2,8(sp)
    3cec:	039d07b3          	mul	a5,s10,s9
    3cf0:	018787b3          	add	a5,a5,s8
    3cf4:	03978733          	mul	a4,a5,s9
    3cf8:	0087d413          	srli	s0,a5,0x8
    3cfc:	01870d33          	add	s10,a4,s8
    3d00:	008d5b93          	srli	s7,s10,0x8
    3d04:	03647433          	remu	s0,s0,s6
    3d08:	035bfbb3          	remu	s7,s7,s5
    3d0c:	01840493          	addi	s1,s0,24
    3d10:	00048513          	mv	a0,s1
    3d14:	046b8993          	addi	s3,s7,70
    3d18:	00098593          	mv	a1,s3
    3d1c:	bccff0ef          	jal	30e8 <vnoise.constprop.0>
    3d20:	00050893          	mv	a7,a0
    3d24:	000d8793          	mv	a5,s11
    3d28:	00200713          	li	a4,2
    3d2c:	00200693          	li	a3,2
    3d30:	00098613          	mv	a2,s3
    3d34:	00048593          	mv	a1,s1
    3d38:	06810513          	addi	a0,sp,104
    3d3c:	071a4463          	blt	s4,a7,3da4 <render_init+0xa3c>
    3d40:	79d040ef          	jal	8cdc <gfx_fill_rect>
    3d44:	00412783          	lw	a5,4(sp)
    3d48:	047b8493          	addi	s1,s7,71
    3d4c:	01a40593          	addi	a1,s0,26
    3d50:	00200713          	li	a4,2
    3d54:	00200693          	li	a3,2
    3d58:	00048613          	mv	a2,s1
    3d5c:	06810513          	addi	a0,sp,104
    3d60:	77d040ef          	jal	8cdc <gfx_fill_rect>
    3d64:	00812783          	lw	a5,8(sp)
    3d68:	01940413          	addi	s0,s0,25
    3d6c:	00200713          	li	a4,2
    3d70:	00200693          	li	a3,2
    3d74:	048b8613          	addi	a2,s7,72
    3d78:	00040593          	mv	a1,s0
    3d7c:	06810513          	addi	a0,sp,104
    3d80:	75d040ef          	jal	8cdc <gfx_fill_rect>
    3d84:	000067b7          	lui	a5,0x6
    3d88:	42a78793          	addi	a5,a5,1066 # 642a <render_game+0x6d6>
    3d8c:	00100713          	li	a4,1
    3d90:	00100693          	li	a3,1
    3d94:	00048613          	mv	a2,s1
    3d98:	00040593          	mv	a1,s0
    3d9c:	06810513          	addi	a0,sp,104
    3da0:	73d040ef          	jal	8cdc <gfx_fill_rect>
    3da4:	fff90913          	addi	s2,s2,-1
    3da8:	f40912e3          	bnez	s2,3cec <render_init+0x984>
    3dac:	00002637          	lui	a2,0x2
    3db0:	000065b7          	lui	a1,0x6
    3db4:	00007537          	lui	a0,0x7
    3db8:	00196db7          	lui	s11,0x196
    3dbc:	3c6ef9b7          	lui	s3,0x3c6ef
    3dc0:	90360613          	addi	a2,a2,-1789 # 1903 <main+0x4f3>
    3dc4:	aca58593          	addi	a1,a1,-1334 # 5aca <__stack_size+0x1aca>
    3dc8:	b0a50513          	addi	a0,a0,-1270 # 6b0a <render_game+0xdb6>
    3dcc:	10400913          	li	s2,260
    3dd0:	60dd8d93          	addi	s11,s11,1549 # 19660d <__freertos_irq_stack_top+0xae46d>
    3dd4:	35f98993          	addi	s3,s3,863 # 3c6ef35f <__freertos_irq_stack_top+0x3c6071bf>
    3dd8:	25600a93          	li	s5,598
    3ddc:	17200a13          	li	s4,370
    3de0:	000d0b93          	mv	s7,s10
    3de4:	00c12223          	sw	a2,4(sp)
    3de8:	00b12423          	sw	a1,8(sp)
    3dec:	00a12623          	sw	a0,12(sp)
    3df0:	03bb87b3          	mul	a5,s7,s11
    3df4:	013787b3          	add	a5,a5,s3
    3df8:	03b78833          	mul	a6,a5,s11
    3dfc:	0087d793          	srli	a5,a5,0x8
    3e00:	01380bb3          	add	s7,a6,s3
    3e04:	008bdc13          	srli	s8,s7,0x8
    3e08:	0357f433          	remu	s0,a5,s5
    3e0c:	014bdc93          	srli	s9,s7,0x14
    3e10:	003cfc93          	andi	s9,s9,3
    3e14:	002c8493          	addi	s1,s9,2
    3e18:	034c7c33          	remu	s8,s8,s4
    3e1c:	01440d13          	addi	s10,s0,20
    3e20:	000d0513          	mv	a0,s10
    3e24:	042c0c13          	addi	s8,s8,66
    3e28:	000c0593          	mv	a1,s8
    3e2c:	abcff0ef          	jal	30e8 <vnoise.constprop.0>
    3e30:	016bd893          	srli	a7,s7,0x16
    3e34:	0018f893          	andi	a7,a7,1
    3e38:	00050e13          	mv	t3,a0
    3e3c:	00288b13          	addi	s6,a7,2
    3e40:	09600813          	li	a6,150
    3e44:	00412783          	lw	a5,4(sp)
    3e48:	00100713          	li	a4,1
    3e4c:	00048693          	mv	a3,s1
    3e50:	016c0633          	add	a2,s8,s6
    3e54:	01540593          	addi	a1,s0,21
    3e58:	06810513          	addi	a0,sp,104
    3e5c:	01dbde93          	srli	t4,s7,0x1d
    3e60:	01c84463          	blt	a6,t3,3e68 <render_init+0xb00>
    3e64:	040e9863          	bnez	t4,3eb4 <render_init+0xb4c>
    3e68:	675040ef          	jal	8cdc <gfx_fill_rect>
    3e6c:	007b9893          	slli	a7,s7,0x7
    3e70:	00812783          	lw	a5,8(sp)
    3e74:	000b0713          	mv	a4,s6
    3e78:	00048693          	mv	a3,s1
    3e7c:	000c0613          	mv	a2,s8
    3e80:	000d0593          	mv	a1,s10
    3e84:	06810513          	addi	a0,sp,104
    3e88:	0008d463          	bgez	a7,3e90 <render_init+0xb28>
    3e8c:	00c12783          	lw	a5,12(sp)
    3e90:	64d040ef          	jal	8cdc <gfx_fill_rect>
    3e94:	000087b7          	lui	a5,0x8
    3e98:	40d78793          	addi	a5,a5,1037 # 840d <render_game+0x26b9>
    3e9c:	00100713          	li	a4,1
    3ea0:	001c8693          	addi	a3,s9,1
    3ea4:	000c0613          	mv	a2,s8
    3ea8:	000d0593          	mv	a1,s10
    3eac:	06810513          	addi	a0,sp,104
    3eb0:	62d040ef          	jal	8cdc <gfx_fill_rect>
    3eb4:	fff90913          	addi	s2,s2,-1
    3eb8:	f2091ce3          	bnez	s2,3df0 <render_init+0xa88>
    3ebc:	001965b7          	lui	a1,0x196
    3ec0:	0000cdb7          	lui	s11,0xc
    3ec4:	0000d637          	lui	a2,0xd
    3ec8:	0000b6b7          	lui	a3,0xb
    3ecc:	3c6efc37          	lui	s8,0x3c6ef
    3ed0:	02d00413          	li	s0,45
    3ed4:	60d58593          	addi	a1,a1,1549 # 19660d <__freertos_irq_stack_top+0xae46d>
    3ed8:	5f5d8d93          	addi	s11,s11,1525 # c5f5 <begin_run.constprop.0+0x19>
    3edc:	dcf60613          	addi	a2,a2,-561 # cdcf <game_init+0x1cf>
    3ee0:	51668693          	addi	a3,a3,1302 # b516 <handle_char+0x966>
    3ee4:	000b8993          	mv	s3,s7
    3ee8:	35fc0c13          	addi	s8,s8,863 # 3c6ef35f <__freertos_irq_stack_top+0x3c6071bf>
    3eec:	24a00b13          	li	s6,586
    3ef0:	16600a93          	li	s5,358
    3ef4:	08c00a13          	li	s4,140
    3ef8:	00040b93          	mv	s7,s0
    3efc:	00058913          	mv	s2,a1
    3f00:	01b12223          	sw	s11,4(sp)
    3f04:	00c12423          	sw	a2,8(sp)
    3f08:	00d12623          	sw	a3,12(sp)
    3f0c:	032987b3          	mul	a5,s3,s2
    3f10:	018787b3          	add	a5,a5,s8
    3f14:	03278833          	mul	a6,a5,s2
    3f18:	0087d793          	srli	a5,a5,0x8
    3f1c:	018809b3          	add	s3,a6,s8
    3f20:	0089d893          	srli	a7,s3,0x8
    3f24:	0367f4b3          	remu	s1,a5,s6
    3f28:	0358fdb3          	remu	s11,a7,s5
    3f2c:	01a48c93          	addi	s9,s1,26
    3f30:	000c8513          	mv	a0,s9
    3f34:	048d8413          	addi	s0,s11,72
    3f38:	00040593          	mv	a1,s0
    3f3c:	9acff0ef          	jal	30e8 <vnoise.constprop.0>
    3f40:	00050e93          	mv	t4,a0
    3f44:	000037b7          	lui	a5,0x3
    3f48:	01a9d313          	srli	t1,s3,0x1a
    3f4c:	28678793          	addi	a5,a5,646 # 3286 <panel+0x7a>
    3f50:	00200713          	li	a4,2
    3f54:	00100693          	li	a3,1
    3f58:	04ad8613          	addi	a2,s11,74
    3f5c:	000c8593          	mv	a1,s9
    3f60:	06810513          	addi	a0,sp,104
    3f64:	09da4063          	blt	s4,t4,3fe4 <render_init+0xc7c>
    3f68:	00300e93          	li	t4,3
    3f6c:	03d37333          	remu	t1,t1,t4
    3f70:	00412d03          	lw	s10,4(sp)
    3f74:	00100e93          	li	t4,1
    3f78:	00030863          	beqz	t1,3f88 <render_init+0xc20>
    3f7c:	00812d03          	lw	s10,8(sp)
    3f80:	01d30463          	beq	t1,t4,3f88 <render_init+0xc20>
    3f84:	00c12d03          	lw	s10,12(sp)
    3f88:	555040ef          	jal	8cdc <gfx_fill_rect>
    3f8c:	000d0793          	mv	a5,s10
    3f90:	00100713          	li	a4,1
    3f94:	00300693          	li	a3,3
    3f98:	00040613          	mv	a2,s0
    3f9c:	01948593          	addi	a1,s1,25
    3fa0:	06810513          	addi	a0,sp,104
    3fa4:	539040ef          	jal	8cdc <gfx_fill_rect>
    3fa8:	000d0793          	mv	a5,s10
    3fac:	00300713          	li	a4,3
    3fb0:	00100693          	li	a3,1
    3fb4:	047d8613          	addi	a2,s11,71
    3fb8:	000c8593          	mv	a1,s9
    3fbc:	06810513          	addi	a0,sp,104
    3fc0:	51d040ef          	jal	8cdc <gfx_fill_rect>
    3fc4:	0000d7b7          	lui	a5,0xd
    3fc8:	54a78793          	addi	a5,a5,1354 # d54a <game_tick+0x6be>
    3fcc:	00100713          	li	a4,1
    3fd0:	00100693          	li	a3,1
    3fd4:	00040613          	mv	a2,s0
    3fd8:	000c8593          	mv	a1,s9
    3fdc:	06810513          	addi	a0,sp,104
    3fe0:	4fd040ef          	jal	8cdc <gfx_fill_rect>
    3fe4:	fffb8b93          	addi	s7,s7,-1
    3fe8:	f20b92e3          	bnez	s7,3f0c <render_init+0xba4>
    3fec:	00005837          	lui	a6,0x5
    3ff0:	ae880813          	addi	a6,a6,-1304 # 4ae8 <__stack_size+0xae8>
    3ff4:	00200793          	li	a5,2
    3ff8:	17f00713          	li	a4,383
    3ffc:	26400693          	li	a3,612
    4000:	03c00613          	li	a2,60
    4004:	00e00593          	li	a1,14
    4008:	06810513          	addi	a0,sp,104
    400c:	518050ef          	jal	9524 <gfx_border>
    4010:	00002837          	lui	a6,0x2
    4014:	94480813          	addi	a6,a6,-1724 # 1944 <main+0x534>
    4018:	00100793          	li	a5,1
    401c:	17700713          	li	a4,375
    4020:	25c00693          	li	a3,604
    4024:	04000613          	li	a2,64
    4028:	01200593          	li	a1,18
    402c:	06810513          	addi	a0,sp,104
    4030:	00006937          	lui	s2,0x6
    4034:	000b8413          	mv	s0,s7
    4038:	4ec050ef          	jal	9524 <gfx_border>
    403c:	01000493          	li	s1,16
    4040:	36990913          	addi	s2,s2,873 # 6369 <render_game+0x615>
    4044:	27000993          	li	s3,624
    4048:	00048593          	mv	a1,s1
    404c:	00090793          	mv	a5,s2
    4050:	00600713          	li	a4,6
    4054:	00200693          	li	a3,2
    4058:	03c00613          	li	a2,60
    405c:	06810513          	addi	a0,sp,104
    4060:	47d040ef          	jal	8cdc <gfx_fill_rect>
    4064:	00048593          	mv	a1,s1
    4068:	00090793          	mv	a5,s2
    406c:	00600713          	li	a4,6
    4070:	00200693          	li	a3,2
    4074:	1b500613          	li	a2,437
    4078:	06810513          	addi	a0,sp,104
    407c:	02048493          	addi	s1,s1,32
    4080:	45d040ef          	jal	8cdc <gfx_fill_rect>
    4084:	fd3492e3          	bne	s1,s3,4048 <__stack_size+0x48>
    4088:	00006937          	lui	s2,0x6
    408c:	05000493          	li	s1,80
    4090:	36990913          	addi	s2,s2,873 # 6369 <render_game+0x615>
    4094:	1d000993          	li	s3,464
    4098:	00048613          	mv	a2,s1
    409c:	00090793          	mv	a5,s2
    40a0:	00200713          	li	a4,2
    40a4:	00600693          	li	a3,6
    40a8:	00e00593          	li	a1,14
    40ac:	06810513          	addi	a0,sp,104
    40b0:	42d040ef          	jal	8cdc <gfx_fill_rect>
    40b4:	00048613          	mv	a2,s1
    40b8:	00090793          	mv	a5,s2
    40bc:	00200713          	li	a4,2
    40c0:	00600693          	li	a3,6
    40c4:	26c00593          	li	a1,620
    40c8:	06810513          	addi	a0,sp,104
    40cc:	02048493          	addi	s1,s1,32
    40d0:	40d040ef          	jal	8cdc <gfx_fill_rect>
    40d4:	fd3492e3          	bne	s1,s3,4098 <__stack_size+0x98>
    40d8:	000185b7          	lui	a1,0x18
    40dc:	02000693          	li	a3,32
    40e0:	01800613          	li	a2,24
    40e4:	8c058593          	addi	a1,a1,-1856 # 178c0 <player_pixels>
    40e8:	06810513          	addi	a0,sp,104
    40ec:	369040ef          	jal	8c54 <gfx_begin>
    40f0:	000107b7          	lui	a5,0x10
    40f4:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    40f8:	02000713          	li	a4,32
    40fc:	01800693          	li	a3,24
    4100:	00000613          	li	a2,0
    4104:	00000593          	li	a1,0
    4108:	06810513          	addi	a0,sp,104
    410c:	3d1040ef          	jal	8cdc <gfx_fill_rect>
    4110:	000017b7          	lui	a5,0x1
    4114:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4118:	00300713          	li	a4,3
    411c:	00500693          	li	a3,5
    4120:	01c00613          	li	a2,28
    4124:	00500593          	li	a1,5
    4128:	06810513          	addi	a0,sp,104
    412c:	3b1040ef          	jal	8cdc <gfx_fill_rect>
    4130:	000017b7          	lui	a5,0x1
    4134:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4138:	00300713          	li	a4,3
    413c:	00500693          	li	a3,5
    4140:	01c00613          	li	a2,28
    4144:	00f00593          	li	a1,15
    4148:	06810513          	addi	a0,sp,104
    414c:	00004a37          	lui	s4,0x4
    4150:	00001bb7          	lui	s7,0x1
    4154:	389040ef          	jal	8cdc <gfx_fill_rect>
    4158:	00700b13          	li	s6,7
    415c:	931a0a13          	addi	s4,s4,-1743 # 3931 <render_init+0x5c9>
    4160:	8c3b8b93          	addi	s7,s7,-1853 # 8c3 <CUSTOM2+0x868>
    4164:	01600a93          	li	s5,22
    4168:	01e00993          	li	s3,30
    416c:	feeb0913          	addi	s2,s6,-18
    4170:	03290933          	mul	s2,s2,s2
    4174:	00000493          	li	s1,0
    4178:	ff548713          	addi	a4,s1,-11
    417c:	00148493          	addi	s1,s1,1
    4180:	02e70733          	mul	a4,a4,a4
    4184:	01270733          	add	a4,a4,s2
    4188:	00471693          	slli	a3,a4,0x4
    418c:	40e686b3          	sub	a3,a3,a4
    4190:	00369693          	slli	a3,a3,0x3
    4194:	00e68733          	add	a4,a3,a4
    4198:	00ea4463          	blt	s4,a4,41a0 <__stack_size+0x1a0>
    419c:	2d40106f          	j	5470 <__stack_size+0x1470>
    41a0:	fd549ce3          	bne	s1,s5,4178 <__stack_size+0x178>
    41a4:	001b0b13          	addi	s6,s6,1
    41a8:	fd3b12e3          	bne	s6,s3,416c <__stack_size+0x16c>
    41ac:	000029b7          	lui	s3,0x2
    41b0:	0000bc37          	lui	s8,0xb
    41b4:	00800b93          	li	s7,8
    41b8:	06400b13          	li	s6,100
    41bc:	fa498993          	addi	s3,s3,-92 # 1fa4 <main+0xb94>
    41c0:	3c7c0c13          	addi	s8,s8,967 # b3c7 <handle_char+0x817>
    41c4:	00900a13          	li	s4,9
    41c8:	01c00a93          	li	s5,28
    41cc:	feeb8793          	addi	a5,s7,-18
    41d0:	02f787b3          	mul	a5,a5,a5
    41d4:	ff700493          	li	s1,-9
    41d8:	00279913          	slli	s2,a5,0x2
    41dc:	00f90933          	add	s2,s2,a5
    41e0:	00491913          	slli	s2,s2,0x4
    41e4:	00f90933          	add	s2,s2,a5
    41e8:	00c0006f          	j	41f4 <__stack_size+0x1f4>
    41ec:	00148493          	addi	s1,s1,1
    41f0:	03448c63          	beq	s1,s4,4228 <__stack_size+0x228>
    41f4:	02948733          	mul	a4,s1,s1
    41f8:	03670733          	mul	a4,a4,s6
    41fc:	01270733          	add	a4,a4,s2
    4200:	fee9c6e3          	blt	s3,a4,41ec <__stack_size+0x1ec>
    4204:	00c48593          	addi	a1,s1,12
    4208:	000c0793          	mv	a5,s8
    420c:	00100713          	li	a4,1
    4210:	00100693          	li	a3,1
    4214:	000b8613          	mv	a2,s7
    4218:	06810513          	addi	a0,sp,104
    421c:	00148493          	addi	s1,s1,1
    4220:	2bd040ef          	jal	8cdc <gfx_fill_rect>
    4224:	fd4498e3          	bne	s1,s4,41f4 <__stack_size+0x1f4>
    4228:	001b8b93          	addi	s7,s7,1
    422c:	fb5b90e3          	bne	s7,s5,41cc <__stack_size+0x1cc>
    4230:	00001737          	lui	a4,0x1
    4234:	00001c37          	lui	s8,0x1
    4238:	00001bb7          	lui	s7,0x1
    423c:	00001b37          	lui	s6,0x1
    4240:	00001ab7          	lui	s5,0x1
    4244:	00001a37          	lui	s4,0x1
    4248:	fc070713          	addi	a4,a4,-64 # fc0 <CUSTOM2+0xf65>
    424c:	ff800493          	li	s1,-8
    4250:	f00c0c13          	addi	s8,s8,-256 # f00 <CUSTOM2+0xea5>
    4254:	dc0b8b93          	addi	s7,s7,-576 # dc0 <CUSTOM2+0xd65>
    4258:	00001d37          	lui	s10,0x1
    425c:	c40b0b13          	addi	s6,s6,-960 # c40 <CUSTOM2+0xbe5>
    4260:	70000c93          	li	s9,1792
    4264:	9c0a8a93          	addi	s5,s5,-1600 # 9c0 <CUSTOM2+0x965>
    4268:	c00a0a13          	addi	s4,s4,-1024 # c00 <CUSTOM2+0xba5>
    426c:	00812223          	sw	s0,4(sp)
    4270:	00070d93          	mv	s11,a4
    4274:	029487b3          	mul	a5,s1,s1
    4278:	01048413          	addi	s0,s1,16
    427c:	00679913          	slli	s2,a5,0x6
    4280:	00079463          	bnez	a5,4288 <__stack_size+0x288>
    4284:	0ad0106f          	j	5b30 <__stack_size+0x1b30>
    4288:	016909b3          	add	s3,s2,s6
    428c:	013d4463          	blt	s10,s3,4294 <__stack_size+0x294>
    4290:	0c50106f          	j	5b54 <__stack_size+0x1b54>
    4294:	012cc463          	blt	s9,s2,429c <__stack_size+0x29c>
    4298:	0dd0106f          	j	5b74 <__stack_size+0x1b74>
    429c:	012ac463          	blt	s5,s2,42a4 <__stack_size+0x2a4>
    42a0:	0f50106f          	j	5b94 <__stack_size+0x1b94>
    42a4:	012a4463          	blt	s4,s2,42ac <__stack_size+0x2ac>
    42a8:	10d0106f          	j	5bb4 <__stack_size+0x1bb4>
    42ac:	012bc463          	blt	s7,s2,42b4 <__stack_size+0x2b4>
    42b0:	1250106f          	j	5bd4 <__stack_size+0x1bd4>
    42b4:	012c4463          	blt	s8,s2,42bc <__stack_size+0x2bc>
    42b8:	13d0106f          	j	5bf4 <__stack_size+0x1bf4>
    42bc:	012dc463          	blt	s11,s2,42c4 <__stack_size+0x2c4>
    42c0:	1550106f          	j	5c14 <__stack_size+0x1c14>
    42c4:	0000e7b7          	lui	a5,0xe
    42c8:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    42cc:	00100713          	li	a4,1
    42d0:	00100693          	li	a3,1
    42d4:	00040613          	mv	a2,s0
    42d8:	00c00593          	li	a1,12
    42dc:	06810513          	addi	a0,sp,104
    42e0:	1fd040ef          	jal	8cdc <gfx_fill_rect>
    42e4:	012a4463          	blt	s4,s2,42ec <__stack_size+0x2ec>
    42e8:	2280106f          	j	5510 <__stack_size+0x1510>
    42ec:	012ac463          	blt	s5,s2,42f4 <__stack_size+0x2f4>
    42f0:	1f40106f          	j	54e4 <__stack_size+0x14e4>
    42f4:	012cc463          	blt	s9,s2,42fc <__stack_size+0x2fc>
    42f8:	1a00106f          	j	5498 <__stack_size+0x1498>
    42fc:	013d4463          	blt	s10,s3,4304 <__stack_size+0x304>
    4300:	1c00106f          	j	54c0 <__stack_size+0x14c0>
    4304:	00148493          	addi	s1,s1,1
    4308:	00800793          	li	a5,8
    430c:	f6f494e3          	bne	s1,a5,4274 <__stack_size+0x274>
    4310:	000107b7          	lui	a5,0x10
    4314:	eb178793          	addi	a5,a5,-335 # feb1 <demo_images_render+0x125>
    4318:	00200713          	li	a4,2
    431c:	00500693          	li	a3,5
    4320:	00a00613          	li	a2,10
    4324:	00600593          	li	a1,6
    4328:	06810513          	addi	a0,sp,104
    432c:	00412403          	lw	s0,4(sp)
    4330:	1ad040ef          	jal	8cdc <gfx_fill_rect>
    4334:	000086b7          	lui	a3,0x8
    4338:	d2968693          	addi	a3,a3,-727 # 7d29 <render_game+0x1fd5>
    433c:	00400613          	li	a2,4
    4340:	00b00593          	li	a1,11
    4344:	06810513          	addi	a0,sp,104
    4348:	8b9fe0ef          	jal	2c00 <sprout>
    434c:	000017b7          	lui	a5,0x1
    4350:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4354:	00400713          	li	a4,4
    4358:	00300693          	li	a3,3
    435c:	01000613          	li	a2,16
    4360:	00700593          	li	a1,7
    4364:	06810513          	addi	a0,sp,104
    4368:	175040ef          	jal	8cdc <gfx_fill_rect>
    436c:	000017b7          	lui	a5,0x1
    4370:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4374:	00400713          	li	a4,4
    4378:	00300693          	li	a3,3
    437c:	01000613          	li	a2,16
    4380:	00f00593          	li	a1,15
    4384:	06810513          	addi	a0,sp,104
    4388:	155040ef          	jal	8cdc <gfx_fill_rect>
    438c:	0000f7b7          	lui	a5,0xf
    4390:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4394:	00100713          	li	a4,1
    4398:	00100693          	li	a3,1
    439c:	01000613          	li	a2,16
    43a0:	00800593          	li	a1,8
    43a4:	06810513          	addi	a0,sp,104
    43a8:	135040ef          	jal	8cdc <gfx_fill_rect>
    43ac:	0000f7b7          	lui	a5,0xf
    43b0:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    43b4:	00100713          	li	a4,1
    43b8:	00100693          	li	a3,1
    43bc:	01000613          	li	a2,16
    43c0:	01000593          	li	a1,16
    43c4:	06810513          	addi	a0,sp,104
    43c8:	115040ef          	jal	8cdc <gfx_fill_rect>
    43cc:	000017b7          	lui	a5,0x1
    43d0:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    43d4:	00100713          	li	a4,1
    43d8:	00400693          	li	a3,4
    43dc:	01600613          	li	a2,22
    43e0:	00b00593          	li	a1,11
    43e4:	06810513          	addi	a0,sp,104
    43e8:	0f5040ef          	jal	8cdc <gfx_fill_rect>
    43ec:	0000b7b7          	lui	a5,0xb
    43f0:	38778793          	addi	a5,a5,903 # b387 <handle_char+0x7d7>
    43f4:	00300713          	li	a4,3
    43f8:	00300693          	li	a3,3
    43fc:	01600613          	li	a2,22
    4400:	00300593          	li	a1,3
    4404:	06810513          	addi	a0,sp,104
    4408:	0d5040ef          	jal	8cdc <gfx_fill_rect>
    440c:	0000a7b7          	lui	a5,0xa
    4410:	32778793          	addi	a5,a5,807 # a327 <bsp_printf+0xdb>
    4414:	00200713          	li	a4,2
    4418:	00200693          	li	a3,2
    441c:	01800613          	li	a2,24
    4420:	01000593          	li	a1,16
    4424:	06810513          	addi	a0,sp,104
    4428:	0b5040ef          	jal	8cdc <gfx_fill_rect>
    442c:	0000c7b7          	lui	a5,0xc
    4430:	46978793          	addi	a5,a5,1129 # c469 <begin_run+0x2d5>
    4434:	00200713          	li	a4,2
    4438:	00200693          	li	a3,2
    443c:	00e00613          	li	a2,14
    4440:	00400593          	li	a1,4
    4444:	06810513          	addi	a0,sp,104
    4448:	095040ef          	jal	8cdc <gfx_fill_rect>
    444c:	000175b7          	lui	a1,0x17
    4450:	01a00693          	li	a3,26
    4454:	01800613          	li	a2,24
    4458:	3e058593          	addi	a1,a1,992 # 173e0 <slime_pixels>
    445c:	06810513          	addi	a0,sp,104
    4460:	7f4040ef          	jal	8c54 <gfx_begin>
    4464:	000107b7          	lui	a5,0x10
    4468:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    446c:	01a00713          	li	a4,26
    4470:	01800693          	li	a3,24
    4474:	00000613          	li	a2,0
    4478:	00000593          	li	a1,0
    447c:	06810513          	addi	a0,sp,104
    4480:	00002ab7          	lui	s5,0x2
    4484:	00001c37          	lui	s8,0x1
    4488:	055040ef          	jal	8cdc <gfx_fill_rect>
    448c:	00500b93          	li	s7,5
    4490:	649a8a93          	addi	s5,s5,1609 # 2649 <main+0x1239>
    4494:	8c3c0c13          	addi	s8,s8,-1853 # 8c3 <CUSTOM2+0x868>
    4498:	01600b13          	li	s6,22
    449c:	01800a13          	li	s4,24
    44a0:	ff2b8793          	addi	a5,s7,-14
    44a4:	02f787b3          	mul	a5,a5,a5
    44a8:	00000913          	li	s2,0
    44ac:	00479993          	slli	s3,a5,0x4
    44b0:	40f989b3          	sub	s3,s3,a5
    44b4:	00399993          	slli	s3,s3,0x3
    44b8:	00f989b3          	add	s3,s3,a5
    44bc:	ff590693          	addi	a3,s2,-11
    44c0:	02d686b3          	mul	a3,a3,a3
    44c4:	00190913          	addi	s2,s2,1
    44c8:	00269713          	slli	a4,a3,0x2
    44cc:	00d70733          	add	a4,a4,a3
    44d0:	00471713          	slli	a4,a4,0x4
    44d4:	00d70733          	add	a4,a4,a3
    44d8:	01370733          	add	a4,a4,s3
    44dc:	00eac463          	blt	s5,a4,44e4 <__stack_size+0x4e4>
    44e0:	05c0106f          	j	553c <__stack_size+0x153c>
    44e4:	fd691ce3          	bne	s2,s6,44bc <__stack_size+0x4bc>
    44e8:	001b8b93          	addi	s7,s7,1
    44ec:	fb4b9ae3          	bne	s7,s4,44a0 <__stack_size+0x4a0>
    44f0:	00001a37          	lui	s4,0x1
    44f4:	00007c37          	lui	s8,0x7
    44f8:	00600b93          	li	s7,6
    44fc:	440a0a13          	addi	s4,s4,1088 # 1440 <main+0x30>
    4500:	54cc0c13          	addi	s8,s8,1356 # 754c <render_game+0x17f8>
    4504:	00900b13          	li	s6,9
    4508:	01600a93          	li	s5,22
    450c:	ff2b8793          	addi	a5,s7,-14
    4510:	02f787b3          	mul	a5,a5,a5
    4514:	ff700913          	li	s2,-9
    4518:	00279993          	slli	s3,a5,0x2
    451c:	00f989b3          	add	s3,s3,a5
    4520:	00499993          	slli	s3,s3,0x4
    4524:	00f989b3          	add	s3,s3,a5
    4528:	00c0006f          	j	4534 <__stack_size+0x534>
    452c:	00190913          	addi	s2,s2,1
    4530:	03690c63          	beq	s2,s6,4568 <__stack_size+0x568>
    4534:	03290733          	mul	a4,s2,s2
    4538:	00671713          	slli	a4,a4,0x6
    453c:	01370733          	add	a4,a4,s3
    4540:	feea46e3          	blt	s4,a4,452c <__stack_size+0x52c>
    4544:	00c90593          	addi	a1,s2,12
    4548:	000c0793          	mv	a5,s8
    454c:	00100713          	li	a4,1
    4550:	00100693          	li	a3,1
    4554:	000b8613          	mv	a2,s7
    4558:	06810513          	addi	a0,sp,104
    455c:	00190913          	addi	s2,s2,1
    4560:	77c040ef          	jal	8cdc <gfx_fill_rect>
    4564:	fd6918e3          	bne	s2,s6,4534 <__stack_size+0x534>
    4568:	001b8b93          	addi	s7,s7,1
    456c:	fb5b90e3          	bne	s7,s5,450c <__stack_size+0x50c>
    4570:	0000a7b7          	lui	a5,0xa
    4574:	65078793          	addi	a5,a5,1616 # a650 <bandwidth_test.constprop.0+0xa8>
    4578:	00300713          	li	a4,3
    457c:	00800693          	li	a3,8
    4580:	00800613          	li	a2,8
    4584:	00500593          	li	a1,5
    4588:	06810513          	addi	a0,sp,104
    458c:	750040ef          	jal	8cdc <gfx_fill_rect>
    4590:	000017b7          	lui	a5,0x1
    4594:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4598:	00300713          	li	a4,3
    459c:	00600693          	li	a3,6
    45a0:	01500613          	li	a2,21
    45a4:	00400593          	li	a1,4
    45a8:	06810513          	addi	a0,sp,104
    45ac:	730040ef          	jal	8cdc <gfx_fill_rect>
    45b0:	000017b7          	lui	a5,0x1
    45b4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    45b8:	00300713          	li	a4,3
    45bc:	00600693          	li	a3,6
    45c0:	01500613          	li	a2,21
    45c4:	00f00593          	li	a1,15
    45c8:	06810513          	addi	a0,sp,104
    45cc:	710040ef          	jal	8cdc <gfx_fill_rect>
    45d0:	000017b7          	lui	a5,0x1
    45d4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    45d8:	00400713          	li	a4,4
    45dc:	00300693          	li	a3,3
    45e0:	00c00613          	li	a2,12
    45e4:	00700593          	li	a1,7
    45e8:	06810513          	addi	a0,sp,104
    45ec:	6f0040ef          	jal	8cdc <gfx_fill_rect>
    45f0:	000017b7          	lui	a5,0x1
    45f4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    45f8:	00400713          	li	a4,4
    45fc:	00300693          	li	a3,3
    4600:	00c00613          	li	a2,12
    4604:	00f00593          	li	a1,15
    4608:	06810513          	addi	a0,sp,104
    460c:	6d0040ef          	jal	8cdc <gfx_fill_rect>
    4610:	0000f7b7          	lui	a5,0xf
    4614:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4618:	00100713          	li	a4,1
    461c:	00100693          	li	a3,1
    4620:	00c00613          	li	a2,12
    4624:	00800593          	li	a1,8
    4628:	06810513          	addi	a0,sp,104
    462c:	6b0040ef          	jal	8cdc <gfx_fill_rect>
    4630:	0000f7b7          	lui	a5,0xf
    4634:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4638:	00100713          	li	a4,1
    463c:	00100693          	li	a3,1
    4640:	00c00613          	li	a2,12
    4644:	01000593          	li	a1,16
    4648:	06810513          	addi	a0,sp,104
    464c:	690040ef          	jal	8cdc <gfx_fill_rect>
    4650:	000047b7          	lui	a5,0x4
    4654:	ac778793          	addi	a5,a5,-1337 # 3ac7 <render_init+0x75f>
    4658:	00200713          	li	a4,2
    465c:	00500693          	li	a3,5
    4660:	01200613          	li	a2,18
    4664:	00a00593          	li	a1,10
    4668:	06810513          	addi	a0,sp,104
    466c:	670040ef          	jal	8cdc <gfx_fill_rect>
    4670:	000057b7          	lui	a5,0x5
    4674:	b2878793          	addi	a5,a5,-1240 # 4b28 <__stack_size+0xb28>
    4678:	00500713          	li	a4,5
    467c:	00200693          	li	a3,2
    4680:	00200613          	li	a2,2
    4684:	00800593          	li	a1,8
    4688:	06810513          	addi	a0,sp,104
    468c:	650040ef          	jal	8cdc <gfx_fill_rect>
    4690:	0000a7b7          	lui	a5,0xa
    4694:	5ed78793          	addi	a5,a5,1517 # a5ed <bandwidth_test.constprop.0+0x45>
    4698:	00500713          	li	a4,5
    469c:	00400693          	li	a3,4
    46a0:	00000613          	li	a2,0
    46a4:	00d00593          	li	a1,13
    46a8:	06810513          	addi	a0,sp,104
    46ac:	630040ef          	jal	8cdc <gfx_fill_rect>
    46b0:	000087b7          	lui	a5,0x8
    46b4:	cca78793          	addi	a5,a5,-822 # 7cca <render_game+0x1f76>
    46b8:	00300713          	li	a4,3
    46bc:	00400693          	li	a3,4
    46c0:	00300613          	li	a2,3
    46c4:	00a00593          	li	a1,10
    46c8:	06810513          	addi	a0,sp,104
    46cc:	610040ef          	jal	8cdc <gfx_fill_rect>
    46d0:	000175b7          	lui	a1,0x17
    46d4:	01600693          	li	a3,22
    46d8:	01400613          	li	a2,20
    46dc:	07058593          	addi	a1,a1,112 # 17070 <runner_pixels>
    46e0:	06810513          	addi	a0,sp,104
    46e4:	570040ef          	jal	8c54 <gfx_begin>
    46e8:	000107b7          	lui	a5,0x10
    46ec:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    46f0:	01600713          	li	a4,22
    46f4:	01400693          	li	a3,20
    46f8:	00000613          	li	a2,0
    46fc:	00000593          	li	a1,0
    4700:	06810513          	addi	a0,sp,104
    4704:	00001ab7          	lui	s5,0x1
    4708:	00001c37          	lui	s8,0x1
    470c:	5d0040ef          	jal	8cdc <gfx_fill_rect>
    4710:	00300b93          	li	s7,3
    4714:	440a8a93          	addi	s5,s5,1088 # 1440 <main+0x30>
    4718:	8c3c0c13          	addi	s8,s8,-1853 # 8c3 <CUSTOM2+0x868>
    471c:	01200b13          	li	s6,18
    4720:	01400a13          	li	s4,20
    4724:	ff5b8793          	addi	a5,s7,-11
    4728:	02f787b3          	mul	a5,a5,a5
    472c:	00000913          	li	s2,0
    4730:	00279993          	slli	s3,a5,0x2
    4734:	00f989b3          	add	s3,s3,a5
    4738:	00499993          	slli	s3,s3,0x4
    473c:	00f989b3          	add	s3,s3,a5
    4740:	ff790713          	addi	a4,s2,-9
    4744:	02e70733          	mul	a4,a4,a4
    4748:	00190913          	addi	s2,s2,1
    474c:	00671713          	slli	a4,a4,0x6
    4750:	01370733          	add	a4,a4,s3
    4754:	60ead8e3          	bge	s5,a4,5564 <__stack_size+0x1564>
    4758:	ff6914e3          	bne	s2,s6,4740 <__stack_size+0x740>
    475c:	001b8b93          	addi	s7,s7,1
    4760:	fd4b92e3          	bne	s7,s4,4724 <__stack_size+0x724>
    4764:	0000ddb7          	lui	s11,0xd
    4768:	ffa00993          	li	s3,-6
    476c:	b29d8d93          	addi	s11,s11,-1239 # cb29 <game_hostile_count+0xf1>
    4770:	65400d13          	li	s10,1620
    4774:	5a000c93          	li	s9,1440
    4778:	6e400c13          	li	s8,1764
    477c:	36000b93          	li	s7,864
    4780:	4a400b13          	li	s6,1188
    4784:	033987b3          	mul	a5,s3,s3
    4788:	00a98a13          	addi	s4,s3,10
    478c:	00179913          	slli	s2,a5,0x1
    4790:	00f90933          	add	s2,s2,a5
    4794:	00491913          	slli	s2,s2,0x4
    4798:	00f90933          	add	s2,s2,a5
    479c:	00079463          	bnez	a5,47a4 <__stack_size+0x7a4>
    47a0:	23c0106f          	j	59dc <__stack_size+0x19dc>
    47a4:	51090a93          	addi	s5,s2,1296
    47a8:	015c4463          	blt	s8,s5,47b0 <__stack_size+0x7b0>
    47ac:	2500106f          	j	59fc <__stack_size+0x19fc>
    47b0:	012bc463          	blt	s7,s2,47b8 <__stack_size+0x7b8>
    47b4:	2640106f          	j	5a18 <__stack_size+0x1a18>
    47b8:	012b4463          	blt	s6,s2,47c0 <__stack_size+0x7c0>
    47bc:	2780106f          	j	5a34 <__stack_size+0x1a34>
    47c0:	012cc463          	blt	s9,s2,47c8 <__stack_size+0x7c8>
    47c4:	28c0106f          	j	5a50 <__stack_size+0x1a50>
    47c8:	012d4463          	blt	s10,s2,47d0 <__stack_size+0x7d0>
    47cc:	2a00106f          	j	5a6c <__stack_size+0x1a6c>
    47d0:	6c000793          	li	a5,1728
    47d4:	0127c463          	blt	a5,s2,47dc <__stack_size+0x7dc>
    47d8:	2b00106f          	j	5a88 <__stack_size+0x1a88>
    47dc:	000d8793          	mv	a5,s11
    47e0:	00100713          	li	a4,1
    47e4:	00100693          	li	a3,1
    47e8:	000a0613          	mv	a2,s4
    47ec:	00a00593          	li	a1,10
    47f0:	06810513          	addi	a0,sp,104
    47f4:	4e8040ef          	jal	8cdc <gfx_fill_rect>
    47f8:	5d2b58e3          	bge	s6,s2,55c8 <__stack_size+0x15c8>
    47fc:	592bd6e3          	bge	s7,s2,5588 <__stack_size+0x1588>
    4800:	5b5c54e3          	bge	s8,s5,55a8 <__stack_size+0x15a8>
    4804:	00198993          	addi	s3,s3,1
    4808:	00700793          	li	a5,7
    480c:	f6f99ce3          	bne	s3,a5,4784 <__stack_size+0x784>
    4810:	0000f7b7          	lui	a5,0xf
    4814:	4ac78793          	addi	a5,a5,1196 # f4ac <overlay.isra.0+0x1c8>
    4818:	00200713          	li	a4,2
    481c:	00800693          	li	a3,8
    4820:	00500613          	li	a2,5
    4824:	00500593          	li	a1,5
    4828:	06810513          	addi	a0,sp,104
    482c:	4b0040ef          	jal	8cdc <gfx_fill_rect>
    4830:	000017b7          	lui	a5,0x1
    4834:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4838:	00200713          	li	a4,2
    483c:	00500693          	li	a3,5
    4840:	00a00613          	li	a2,10
    4844:	00400593          	li	a1,4
    4848:	06810513          	addi	a0,sp,104
    484c:	490040ef          	jal	8cdc <gfx_fill_rect>
    4850:	000017b7          	lui	a5,0x1
    4854:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4858:	00200713          	li	a4,2
    485c:	00500693          	li	a3,5
    4860:	00a00613          	li	a2,10
    4864:	00c00593          	li	a1,12
    4868:	06810513          	addi	a0,sp,104
    486c:	470040ef          	jal	8cdc <gfx_fill_rect>
    4870:	0000f7b7          	lui	a5,0xf
    4874:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4878:	00200713          	li	a4,2
    487c:	00200693          	li	a3,2
    4880:	00c00613          	li	a2,12
    4884:	00700593          	li	a1,7
    4888:	06810513          	addi	a0,sp,104
    488c:	450040ef          	jal	8cdc <gfx_fill_rect>
    4890:	0000f7b7          	lui	a5,0xf
    4894:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4898:	00200713          	li	a4,2
    489c:	00200693          	li	a3,2
    48a0:	00c00613          	li	a2,12
    48a4:	00c00593          	li	a1,12
    48a8:	06810513          	addi	a0,sp,104
    48ac:	430040ef          	jal	8cdc <gfx_fill_rect>
    48b0:	000017b7          	lui	a5,0x1
    48b4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    48b8:	00300713          	li	a4,3
    48bc:	00400693          	li	a3,4
    48c0:	01200613          	li	a2,18
    48c4:	00400593          	li	a1,4
    48c8:	06810513          	addi	a0,sp,104
    48cc:	410040ef          	jal	8cdc <gfx_fill_rect>
    48d0:	000017b7          	lui	a5,0x1
    48d4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    48d8:	00300713          	li	a4,3
    48dc:	00400693          	li	a3,4
    48e0:	01200613          	li	a2,18
    48e4:	00d00593          	li	a1,13
    48e8:	06810513          	addi	a0,sp,104
    48ec:	3f0040ef          	jal	8cdc <gfx_fill_rect>
    48f0:	000077b7          	lui	a5,0x7
    48f4:	be978793          	addi	a5,a5,-1047 # 6be9 <render_game+0xe95>
    48f8:	00500713          	li	a4,5
    48fc:	00300693          	li	a3,3
    4900:	00000613          	li	a2,0
    4904:	00500593          	li	a1,5
    4908:	06810513          	addi	a0,sp,104
    490c:	3d0040ef          	jal	8cdc <gfx_fill_rect>
    4910:	000097b7          	lui	a5,0x9
    4914:	52a78793          	addi	a5,a5,1322 # 952a <gfx_border+0x6>
    4918:	00300713          	li	a4,3
    491c:	00500693          	li	a3,5
    4920:	00100613          	li	a2,1
    4924:	00a00593          	li	a1,10
    4928:	06810513          	addi	a0,sp,104
    492c:	3b0040ef          	jal	8cdc <gfx_fill_rect>
    4930:	000165b7          	lui	a1,0x16
    4934:	02600693          	li	a3,38
    4938:	02400613          	li	a2,36
    493c:	5c058593          	addi	a1,a1,1472 # 165c0 <brute_pixels>
    4940:	06810513          	addi	a0,sp,104
    4944:	310040ef          	jal	8c54 <gfx_begin>
    4948:	000107b7          	lui	a5,0x10
    494c:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    4950:	02600713          	li	a4,38
    4954:	02400693          	li	a3,36
    4958:	00000613          	li	a2,0
    495c:	00000593          	li	a1,0
    4960:	06810513          	addi	a0,sp,104
    4964:	0000eb37          	lui	s6,0xe
    4968:	00001d37          	lui	s10,0x1
    496c:	370040ef          	jal	8cdc <gfx_fill_rect>
    4970:	00600c93          	li	s9,6
    4974:	0c400c13          	li	s8,196
    4978:	d44b0b13          	addi	s6,s6,-700 # dd44 <game_tick+0xeb8>
    497c:	8c3d0d13          	addi	s10,s10,-1853 # 8c3 <CUSTOM2+0x868>
    4980:	02200b93          	li	s7,34
    4984:	02300a93          	li	s5,35
    4988:	fecc8793          	addi	a5,s9,-20
    498c:	02f787b3          	mul	a5,a5,a5
    4990:	00000913          	li	s2,0
    4994:	00379a13          	slli	s4,a5,0x3
    4998:	00fa0a33          	add	s4,s4,a5
    499c:	005a1a13          	slli	s4,s4,0x5
    49a0:	00fa0a33          	add	s4,s4,a5
    49a4:	fef90713          	addi	a4,s2,-17
    49a8:	02e70733          	mul	a4,a4,a4
    49ac:	00190913          	addi	s2,s2,1
    49b0:	03870733          	mul	a4,a4,s8
    49b4:	01470733          	add	a4,a4,s4
    49b8:	42eb5ae3          	bge	s6,a4,55ec <__stack_size+0x15ec>
    49bc:	ff7914e3          	bne	s2,s7,49a4 <__stack_size+0x9a4>
    49c0:	001c8c93          	addi	s9,s9,1
    49c4:	fd5c92e3          	bne	s9,s5,4988 <__stack_size+0x988>
    49c8:	00007bb7          	lui	s7,0x7
    49cc:	00009cb7          	lui	s9,0x9
    49d0:	0c400b13          	li	s6,196
    49d4:	e40b8b93          	addi	s7,s7,-448 # 6e40 <render_game+0x10ec>
    49d8:	bf2c8c93          	addi	s9,s9,-1038 # 8bf2 <gfx_fill_rect.part.0+0x166>
    49dc:	00e00c13          	li	s8,14
    49e0:	02000a93          	li	s5,32
    49e4:	fed98a13          	addi	s4,s3,-19
    49e8:	034a0a33          	mul	s4,s4,s4
    49ec:	ff200913          	li	s2,-14
    49f0:	036a0a33          	mul	s4,s4,s6
    49f4:	00c0006f          	j	4a00 <__stack_size+0xa00>
    49f8:	00190913          	addi	s2,s2,1
    49fc:	05890063          	beq	s2,s8,4a3c <__stack_size+0xa3c>
    4a00:	032906b3          	mul	a3,s2,s2
    4a04:	00369713          	slli	a4,a3,0x3
    4a08:	00d70733          	add	a4,a4,a3
    4a0c:	00471713          	slli	a4,a4,0x4
    4a10:	01470733          	add	a4,a4,s4
    4a14:	feebc2e3          	blt	s7,a4,49f8 <__stack_size+0x9f8>
    4a18:	01290593          	addi	a1,s2,18
    4a1c:	000c8793          	mv	a5,s9
    4a20:	00100713          	li	a4,1
    4a24:	00100693          	li	a3,1
    4a28:	00098613          	mv	a2,s3
    4a2c:	06810513          	addi	a0,sp,104
    4a30:	00190913          	addi	s2,s2,1
    4a34:	2a8040ef          	jal	8cdc <gfx_fill_rect>
    4a38:	fd8914e3          	bne	s2,s8,4a00 <__stack_size+0xa00>
    4a3c:	00198993          	addi	s3,s3,1
    4a40:	fb5992e3          	bne	s3,s5,49e4 <__stack_size+0x9e4>
    4a44:	00002b37          	lui	s6,0x2
    4a48:	0000acb7          	lui	s9,0xa
    4a4c:	00048c13          	mv	s8,s1
    4a50:	649b0b13          	addi	s6,s6,1609 # 2649 <main+0x1239>
    4a54:	4b5c8c93          	addi	s9,s9,1205 # a4b5 <bsp_printf+0x269>
    4a58:	00b00b93          	li	s7,11
    4a5c:	01a00a93          	li	s5,26
    4a60:	fefc0793          	addi	a5,s8,-17
    4a64:	02f787b3          	mul	a5,a5,a5
    4a68:	ff500993          	li	s3,-11
    4a6c:	00479a13          	slli	s4,a5,0x4
    4a70:	40fa0a33          	sub	s4,s4,a5
    4a74:	003a1a13          	slli	s4,s4,0x3
    4a78:	00fa0a33          	add	s4,s4,a5
    4a7c:	00c0006f          	j	4a88 <__stack_size+0xa88>
    4a80:	00198993          	addi	s3,s3,1
    4a84:	05798263          	beq	s3,s7,4ac8 <__stack_size+0xac8>
    4a88:	033986b3          	mul	a3,s3,s3
    4a8c:	00269713          	slli	a4,a3,0x2
    4a90:	00d70733          	add	a4,a4,a3
    4a94:	00471713          	slli	a4,a4,0x4
    4a98:	00d70733          	add	a4,a4,a3
    4a9c:	01470733          	add	a4,a4,s4
    4aa0:	feeb40e3          	blt	s6,a4,4a80 <__stack_size+0xa80>
    4aa4:	01098593          	addi	a1,s3,16
    4aa8:	000c8793          	mv	a5,s9
    4aac:	00100713          	li	a4,1
    4ab0:	00100693          	li	a3,1
    4ab4:	000c0613          	mv	a2,s8
    4ab8:	06810513          	addi	a0,sp,104
    4abc:	00198993          	addi	s3,s3,1
    4ac0:	21c040ef          	jal	8cdc <gfx_fill_rect>
    4ac4:	fd7992e3          	bne	s3,s7,4a88 <__stack_size+0xa88>
    4ac8:	001c0c13          	addi	s8,s8,1
    4acc:	f95c1ae3          	bne	s8,s5,4a60 <__stack_size+0xa60>
    4ad0:	000077b7          	lui	a5,0x7
    4ad4:	b2f78793          	addi	a5,a5,-1233 # 6b2f <render_game+0xddb>
    4ad8:	00900713          	li	a4,9
    4adc:	00500693          	li	a3,5
    4ae0:	00300613          	li	a2,3
    4ae4:	00500593          	li	a1,5
    4ae8:	06810513          	addi	a0,sp,104
    4aec:	1f0040ef          	jal	8cdc <gfx_fill_rect>
    4af0:	000077b7          	lui	a5,0x7
    4af4:	b2f78793          	addi	a5,a5,-1233 # 6b2f <render_game+0xddb>
    4af8:	00900713          	li	a4,9
    4afc:	00600693          	li	a3,6
    4b00:	00200613          	li	a2,2
    4b04:	01800593          	li	a1,24
    4b08:	06810513          	addi	a0,sp,104
    4b0c:	1d0040ef          	jal	8cdc <gfx_fill_rect>
    4b10:	000017b7          	lui	a5,0x1
    4b14:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4b18:	00300713          	li	a4,3
    4b1c:	00600693          	li	a3,6
    4b20:	00f00613          	li	a2,15
    4b24:	00900593          	li	a1,9
    4b28:	06810513          	addi	a0,sp,104
    4b2c:	1b0040ef          	jal	8cdc <gfx_fill_rect>
    4b30:	000017b7          	lui	a5,0x1
    4b34:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4b38:	00300713          	li	a4,3
    4b3c:	00600693          	li	a3,6
    4b40:	00f00613          	li	a2,15
    4b44:	01600593          	li	a1,22
    4b48:	06810513          	addi	a0,sp,104
    4b4c:	190040ef          	jal	8cdc <gfx_fill_rect>
    4b50:	0000f7b7          	lui	a5,0xf
    4b54:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    4b58:	00300713          	li	a4,3
    4b5c:	00300693          	li	a3,3
    4b60:	01200613          	li	a2,18
    4b64:	00c00593          	li	a1,12
    4b68:	06810513          	addi	a0,sp,104
    4b6c:	170040ef          	jal	8cdc <gfx_fill_rect>
    4b70:	0000f7b7          	lui	a5,0xf
    4b74:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    4b78:	00300713          	li	a4,3
    4b7c:	00300693          	li	a3,3
    4b80:	01200613          	li	a2,18
    4b84:	01600593          	li	a1,22
    4b88:	06810513          	addi	a0,sp,104
    4b8c:	150040ef          	jal	8cdc <gfx_fill_rect>
    4b90:	000017b7          	lui	a5,0x1
    4b94:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4b98:	00300713          	li	a4,3
    4b9c:	00900693          	li	a3,9
    4ba0:	01a00613          	li	a2,26
    4ba4:	00e00593          	li	a1,14
    4ba8:	06810513          	addi	a0,sp,104
    4bac:	130040ef          	jal	8cdc <gfx_fill_rect>
    4bb0:	000017b7          	lui	a5,0x1
    4bb4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4bb8:	00400713          	li	a4,4
    4bbc:	00900693          	li	a3,9
    4bc0:	02000613          	li	a2,32
    4bc4:	00600593          	li	a1,6
    4bc8:	06810513          	addi	a0,sp,104
    4bcc:	110040ef          	jal	8cdc <gfx_fill_rect>
    4bd0:	000017b7          	lui	a5,0x1
    4bd4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4bd8:	00400713          	li	a4,4
    4bdc:	00900693          	li	a3,9
    4be0:	02000613          	li	a2,32
    4be4:	01600593          	li	a1,22
    4be8:	06810513          	addi	a0,sp,104
    4bec:	0f0040ef          	jal	8cdc <gfx_fill_rect>
    4bf0:	000155b7          	lui	a1,0x15
    4bf4:	03800693          	li	a3,56
    4bf8:	03400613          	li	a2,52
    4bfc:	f0058593          	addi	a1,a1,-256 # 14f00 <elite_pixels>
    4c00:	06810513          	addi	a0,sp,104
    4c04:	050040ef          	jal	8c54 <gfx_begin>
    4c08:	000107b7          	lui	a5,0x10
    4c0c:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    4c10:	03800713          	li	a4,56
    4c14:	03400693          	li	a3,52
    4c18:	00000613          	li	a2,0
    4c1c:	00000593          	li	a1,0
    4c20:	06810513          	addi	a0,sp,104
    4c24:	0003dc37          	lui	s8,0x3d
    4c28:	00001db7          	lui	s11,0x1
    4c2c:	0b0040ef          	jal	8cdc <gfx_fill_rect>
    4c30:	27100b93          	li	s7,625
    4c34:	19000d13          	li	s10,400
    4c38:	090c0c13          	addi	s8,s8,144 # 3d090 <background_pixels+0x251d0>
    4c3c:	8c3d8d93          	addi	s11,s11,-1853 # 8c3 <CUSTOM2+0x868>
    4c40:	03200c93          	li	s9,50
    4c44:	03400b13          	li	s6,52
    4c48:	fe198a93          	addi	s5,s3,-31
    4c4c:	035a8ab3          	mul	s5,s5,s5
    4c50:	00000a13          	li	s4,0
    4c54:	037a8ab3          	mul	s5,s5,s7
    4c58:	fe7a0713          	addi	a4,s4,-25
    4c5c:	001a0a13          	addi	s4,s4,1
    4c60:	02e70733          	mul	a4,a4,a4
    4c64:	03a70733          	mul	a4,a4,s10
    4c68:	01570733          	add	a4,a4,s5
    4c6c:	1aec52e3          	bge	s8,a4,5610 <__stack_size+0x1610>
    4c70:	ff9a14e3          	bne	s4,s9,4c58 <__stack_size+0xc58>
    4c74:	00198993          	addi	s3,s3,1
    4c78:	fd6998e3          	bne	s3,s6,4c48 <__stack_size+0xc48>
    4c7c:	00026bb7          	lui	s7,0x26
    4c80:	00008db7          	lui	s11,0x8
    4c84:	00c00d13          	li	s10,12
    4c88:	1e400b13          	li	s6,484
    4c8c:	14400c93          	li	s9,324
    4c90:	490b8b93          	addi	s7,s7,1168 # 26490 <background_pixels+0xe5d0>
    4c94:	a90d8d93          	addi	s11,s11,-1392 # 7a90 <render_game+0x1d3c>
    4c98:	01600c13          	li	s8,22
    4c9c:	03000a93          	li	s5,48
    4ca0:	fe2d0a13          	addi	s4,s10,-30
    4ca4:	034a0a33          	mul	s4,s4,s4
    4ca8:	fea00993          	li	s3,-22
    4cac:	036a0a33          	mul	s4,s4,s6
    4cb0:	00c0006f          	j	4cbc <__stack_size+0xcbc>
    4cb4:	00198993          	addi	s3,s3,1
    4cb8:	03898c63          	beq	s3,s8,4cf0 <__stack_size+0xcf0>
    4cbc:	03398733          	mul	a4,s3,s3
    4cc0:	03970733          	mul	a4,a4,s9
    4cc4:	01470733          	add	a4,a4,s4
    4cc8:	feebc6e3          	blt	s7,a4,4cb4 <__stack_size+0xcb4>
    4ccc:	01a98593          	addi	a1,s3,26
    4cd0:	000d8793          	mv	a5,s11
    4cd4:	00100713          	li	a4,1
    4cd8:	00100693          	li	a3,1
    4cdc:	000d0613          	mv	a2,s10
    4ce0:	06810513          	addi	a0,sp,104
    4ce4:	00198993          	addi	s3,s3,1
    4ce8:	7f5030ef          	jal	8cdc <gfx_fill_rect>
    4cec:	fd8998e3          	bne	s3,s8,4cbc <__stack_size+0xcbc>
    4cf0:	001d0d13          	addi	s10,s10,1
    4cf4:	fb5d16e3          	bne	s10,s5,4ca0 <__stack_size+0xca0>
    4cf8:	00009b37          	lui	s6,0x9
    4cfc:	0000ac37          	lui	s8,0xa
    4d00:	899b0b13          	addi	s6,s6,-1895 # 8899 <render_game+0x2b45>
    4d04:	b53c0c13          	addi	s8,s8,-1197 # 9b53 <hw_fill_alpha+0xab>
    4d08:	01100b93          	li	s7,17
    4d0c:	02500a93          	li	s5,37
    4d10:	fe790793          	addi	a5,s2,-25
    4d14:	02f787b3          	mul	a5,a5,a5
    4d18:	fef00993          	li	s3,-17
    4d1c:	00379a13          	slli	s4,a5,0x3
    4d20:	00fa0a33          	add	s4,s4,a5
    4d24:	005a1a13          	slli	s4,s4,0x5
    4d28:	00fa0a33          	add	s4,s4,a5
    4d2c:	00c0006f          	j	4d38 <__stack_size+0xd38>
    4d30:	00198993          	addi	s3,s3,1
    4d34:	05798263          	beq	s3,s7,4d78 <__stack_size+0xd78>
    4d38:	033986b3          	mul	a3,s3,s3
    4d3c:	00469713          	slli	a4,a3,0x4
    4d40:	40d70733          	sub	a4,a4,a3
    4d44:	00371713          	slli	a4,a4,0x3
    4d48:	00d70733          	add	a4,a4,a3
    4d4c:	01470733          	add	a4,a4,s4
    4d50:	feeb40e3          	blt	s6,a4,4d30 <__stack_size+0xd30>
    4d54:	01798593          	addi	a1,s3,23
    4d58:	000c0793          	mv	a5,s8
    4d5c:	00100713          	li	a4,1
    4d60:	00100693          	li	a3,1
    4d64:	00090613          	mv	a2,s2
    4d68:	06810513          	addi	a0,sp,104
    4d6c:	00198993          	addi	s3,s3,1
    4d70:	76d030ef          	jal	8cdc <gfx_fill_rect>
    4d74:	fd7992e3          	bne	s3,s7,4d38 <__stack_size+0xd38>
    4d78:	00190913          	addi	s2,s2,1
    4d7c:	f9591ae3          	bne	s2,s5,4d10 <__stack_size+0xd10>
    4d80:	0000f7b7          	lui	a5,0xf
    4d84:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    4d88:	00d00713          	li	a4,13
    4d8c:	00500693          	li	a3,5
    4d90:	00500613          	li	a2,5
    4d94:	00900593          	li	a1,9
    4d98:	06810513          	addi	a0,sp,104
    4d9c:	741030ef          	jal	8cdc <gfx_fill_rect>
    4da0:	0000f7b7          	lui	a5,0xf
    4da4:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    4da8:	00e00713          	li	a4,14
    4dac:	00500693          	li	a3,5
    4db0:	00100613          	li	a2,1
    4db4:	01800593          	li	a1,24
    4db8:	06810513          	addi	a0,sp,104
    4dbc:	721030ef          	jal	8cdc <gfx_fill_rect>
    4dc0:	0000f7b7          	lui	a5,0xf
    4dc4:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    4dc8:	00d00713          	li	a4,13
    4dcc:	00500693          	li	a3,5
    4dd0:	00500613          	li	a2,5
    4dd4:	02700593          	li	a1,39
    4dd8:	06810513          	addi	a0,sp,104
    4ddc:	701030ef          	jal	8cdc <gfx_fill_rect>
    4de0:	0000f7b7          	lui	a5,0xf
    4de4:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    4de8:	00400713          	li	a4,4
    4dec:	02100693          	li	a3,33
    4df0:	00d00613          	li	a2,13
    4df4:	00a00593          	li	a1,10
    4df8:	06810513          	addi	a0,sp,104
    4dfc:	6e1030ef          	jal	8cdc <gfx_fill_rect>
    4e00:	000017b7          	lui	a5,0x1
    4e04:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4e08:	00400713          	li	a4,4
    4e0c:	00b00693          	li	a3,11
    4e10:	01800613          	li	a2,24
    4e14:	00b00593          	li	a1,11
    4e18:	06810513          	addi	a0,sp,104
    4e1c:	6c1030ef          	jal	8cdc <gfx_fill_rect>
    4e20:	000017b7          	lui	a5,0x1
    4e24:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4e28:	00400713          	li	a4,4
    4e2c:	00b00693          	li	a3,11
    4e30:	01800613          	li	a2,24
    4e34:	02000593          	li	a1,32
    4e38:	06810513          	addi	a0,sp,104
    4e3c:	6a1030ef          	jal	8cdc <gfx_fill_rect>
    4e40:	0000f7b7          	lui	a5,0xf
    4e44:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    4e48:	00400713          	li	a4,4
    4e4c:	00500693          	li	a3,5
    4e50:	01c00613          	li	a2,28
    4e54:	01000593          	li	a1,16
    4e58:	06810513          	addi	a0,sp,104
    4e5c:	681030ef          	jal	8cdc <gfx_fill_rect>
    4e60:	0000f7b7          	lui	a5,0xf
    4e64:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    4e68:	00400713          	li	a4,4
    4e6c:	00500693          	li	a3,5
    4e70:	01c00613          	li	a2,28
    4e74:	02000593          	li	a1,32
    4e78:	06810513          	addi	a0,sp,104
    4e7c:	661030ef          	jal	8cdc <gfx_fill_rect>
    4e80:	000017b7          	lui	a5,0x1
    4e84:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4e88:	00400713          	li	a4,4
    4e8c:	00f00693          	li	a3,15
    4e90:	02500613          	li	a2,37
    4e94:	01400593          	li	a1,20
    4e98:	06810513          	addi	a0,sp,104
    4e9c:	641030ef          	jal	8cdc <gfx_fill_rect>
    4ea0:	0000f7b7          	lui	a5,0xf
    4ea4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    4ea8:	00300713          	li	a4,3
    4eac:	00300693          	li	a3,3
    4eb0:	02500613          	li	a2,37
    4eb4:	01700593          	li	a1,23
    4eb8:	06810513          	addi	a0,sp,104
    4ebc:	621030ef          	jal	8cdc <gfx_fill_rect>
    4ec0:	000017b7          	lui	a5,0x1
    4ec4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4ec8:	00500713          	li	a4,5
    4ecc:	00c00693          	li	a3,12
    4ed0:	03100613          	li	a2,49
    4ed4:	00800593          	li	a1,8
    4ed8:	06810513          	addi	a0,sp,104
    4edc:	601030ef          	jal	8cdc <gfx_fill_rect>
    4ee0:	000017b7          	lui	a5,0x1
    4ee4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    4ee8:	00500713          	li	a4,5
    4eec:	00c00693          	li	a3,12
    4ef0:	03100613          	li	a2,49
    4ef4:	02100593          	li	a1,33
    4ef8:	06810513          	addi	a0,sp,104
    4efc:	5e1030ef          	jal	8cdc <gfx_fill_rect>
    4f00:	000155b7          	lui	a1,0x15
    4f04:	01c00693          	li	a3,28
    4f08:	01600613          	li	a2,22
    4f0c:	a3058593          	addi	a1,a1,-1488 # 14a30 <spitter_pixels>
    4f10:	06810513          	addi	a0,sp,104
    4f14:	541030ef          	jal	8c54 <gfx_begin>
    4f18:	000107b7          	lui	a5,0x10
    4f1c:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    4f20:	01c00713          	li	a4,28
    4f24:	01600693          	li	a3,22
    4f28:	00000613          	li	a2,0
    4f2c:	00000593          	li	a1,0
    4f30:	06810513          	addi	a0,sp,104
    4f34:	00002b37          	lui	s6,0x2
    4f38:	00001c37          	lui	s8,0x1
    4f3c:	5a1030ef          	jal	8cdc <gfx_fill_rect>
    4f40:	06400a93          	li	s5,100
    4f44:	900b0b13          	addi	s6,s6,-1792 # 1900 <main+0x4f0>
    4f48:	8c3c0c13          	addi	s8,s8,-1853 # 8c3 <CUSTOM2+0x868>
    4f4c:	01400b93          	li	s7,20
    4f50:	01800a13          	li	s4,24
    4f54:	ff048993          	addi	s3,s1,-16
    4f58:	033989b3          	mul	s3,s3,s3
    4f5c:	00000913          	li	s2,0
    4f60:	035989b3          	mul	s3,s3,s5
    4f64:	ff690713          	addi	a4,s2,-10
    4f68:	00190913          	addi	s2,s2,1
    4f6c:	02e70733          	mul	a4,a4,a4
    4f70:	00671713          	slli	a4,a4,0x6
    4f74:	01370733          	add	a4,a4,s3
    4f78:	6aeb5e63          	bge	s6,a4,5634 <__stack_size+0x1634>
    4f7c:	ff7914e3          	bne	s2,s7,4f64 <__stack_size+0xf64>
    4f80:	00148493          	addi	s1,s1,1
    4f84:	fd4498e3          	bne	s1,s4,4f54 <__stack_size+0xf54>
    4f88:	00001737          	lui	a4,0x1
    4f8c:	000044b7          	lui	s1,0x4
    4f90:	00001b37          	lui	s6,0x1
    4f94:	00001ab7          	lui	s5,0x1
    4f98:	8dc70713          	addi	a4,a4,-1828 # 8dc <CUSTOM2+0x881>
    4f9c:	ffa00a13          	li	s4,-6
    4fa0:	43548493          	addi	s1,s1,1077 # 4435 <__stack_size+0x435>
    4fa4:	870b0b13          	addi	s6,s6,-1936 # 870 <CUSTOM2+0x815>
    4fa8:	7bc00d13          	li	s10,1980
    4fac:	900a8a93          	addi	s5,s5,-1792 # 900 <CUSTOM2+0x8a5>
    4fb0:	3f000c93          	li	s9,1008
    4fb4:	57c00c13          	li	s8,1404
    4fb8:	6c000b93          	li	s7,1728
    4fbc:	00812223          	sw	s0,4(sp)
    4fc0:	00070d93          	mv	s11,a4
    4fc4:	034a07b3          	mul	a5,s4,s4
    4fc8:	00fa0413          	addi	s0,s4,15
    4fcc:	00679913          	slli	s2,a5,0x6
    4fd0:	0a0782e3          	beqz	a5,5874 <__stack_size+0x1874>
    4fd4:	6e490993          	addi	s3,s2,1764
    4fd8:	0b3adee3          	bge	s5,s3,5894 <__stack_size+0x1894>
    4fdc:	0d2cdae3          	bge	s9,s2,58b0 <__stack_size+0x18b0>
    4fe0:	0f2c56e3          	bge	s8,s2,58cc <__stack_size+0x18cc>
    4fe4:	112bd2e3          	bge	s7,s2,58e8 <__stack_size+0x18e8>
    4fe8:	112d5ee3          	bge	s10,s2,5904 <__stack_size+0x1904>
    4fec:	132b5ae3          	bge	s6,s2,5920 <__stack_size+0x1920>
    4ff0:	152dd6e3          	bge	s11,s2,593c <__stack_size+0x193c>
    4ff4:	00048793          	mv	a5,s1
    4ff8:	00100713          	li	a4,1
    4ffc:	00100693          	li	a3,1
    5000:	00040613          	mv	a2,s0
    5004:	00b00593          	li	a1,11
    5008:	06810513          	addi	a0,sp,104
    500c:	4d1030ef          	jal	8cdc <gfx_fill_rect>
    5010:	6b2bd663          	bge	s7,s2,56bc <__stack_size+0x16bc>
    5014:	692c5263          	bge	s8,s2,5698 <__stack_size+0x1698>
    5018:	652cd063          	bge	s9,s2,5658 <__stack_size+0x1658>
    501c:	653ade63          	bge	s5,s3,5678 <__stack_size+0x1678>
    5020:	001a0a13          	addi	s4,s4,1
    5024:	00700793          	li	a5,7
    5028:	f8fa1ee3          	bne	s4,a5,4fc4 <__stack_size+0xfc4>
    502c:	000097b7          	lui	a5,0x9
    5030:	6bb78793          	addi	a5,a5,1723 # 96bb <gfx_text+0x93>
    5034:	00300713          	li	a4,3
    5038:	00800693          	li	a3,8
    503c:	00a00613          	li	a2,10
    5040:	00600593          	li	a1,6
    5044:	06810513          	addi	a0,sp,104
    5048:	00412403          	lw	s0,4(sp)
    504c:	491030ef          	jal	8cdc <gfx_fill_rect>
    5050:	000037b7          	lui	a5,0x3
    5054:	acd78793          	addi	a5,a5,-1331 # 2acd <main+0x16bd>
    5058:	00900713          	li	a4,9
    505c:	00400693          	li	a3,4
    5060:	00100613          	li	a2,1
    5064:	00900593          	li	a1,9
    5068:	06810513          	addi	a0,sp,104
    506c:	471030ef          	jal	8cdc <gfx_fill_rect>
    5070:	0000c7b7          	lui	a5,0xc
    5074:	eac78793          	addi	a5,a5,-340 # beac <hostile_shot+0x238>
    5078:	00400713          	li	a4,4
    507c:	00800693          	li	a3,8
    5080:	00000613          	li	a2,0
    5084:	00700593          	li	a1,7
    5088:	06810513          	addi	a0,sp,104
    508c:	451030ef          	jal	8cdc <gfx_fill_rect>
    5090:	0000f7b7          	lui	a5,0xf
    5094:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    5098:	00200713          	li	a4,2
    509c:	00400693          	li	a3,4
    50a0:	00100613          	li	a2,1
    50a4:	00900593          	li	a1,9
    50a8:	06810513          	addi	a0,sp,104
    50ac:	431030ef          	jal	8cdc <gfx_fill_rect>
    50b0:	000017b7          	lui	a5,0x1
    50b4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    50b8:	00300713          	li	a4,3
    50bc:	00400693          	li	a3,4
    50c0:	00e00613          	li	a2,14
    50c4:	00500593          	li	a1,5
    50c8:	06810513          	addi	a0,sp,104
    50cc:	411030ef          	jal	8cdc <gfx_fill_rect>
    50d0:	000017b7          	lui	a5,0x1
    50d4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    50d8:	00300713          	li	a4,3
    50dc:	00400693          	li	a3,4
    50e0:	00e00613          	li	a2,14
    50e4:	00d00593          	li	a1,13
    50e8:	06810513          	addi	a0,sp,104
    50ec:	3f1030ef          	jal	8cdc <gfx_fill_rect>
    50f0:	0000f7b7          	lui	a5,0xf
    50f4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    50f8:	00200713          	li	a4,2
    50fc:	00200693          	li	a3,2
    5100:	00e00613          	li	a2,14
    5104:	00600593          	li	a1,6
    5108:	06810513          	addi	a0,sp,104
    510c:	3d1030ef          	jal	8cdc <gfx_fill_rect>
    5110:	0000f7b7          	lui	a5,0xf
    5114:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    5118:	00200713          	li	a4,2
    511c:	00200693          	li	a3,2
    5120:	00e00613          	li	a2,14
    5124:	00e00593          	li	a1,14
    5128:	06810513          	addi	a0,sp,104
    512c:	3b1030ef          	jal	8cdc <gfx_fill_rect>
    5130:	000017b7          	lui	a5,0x1
    5134:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    5138:	00400713          	li	a4,4
    513c:	00500693          	li	a3,5
    5140:	01600613          	li	a2,22
    5144:	00400593          	li	a1,4
    5148:	06810513          	addi	a0,sp,104
    514c:	391030ef          	jal	8cdc <gfx_fill_rect>
    5150:	000017b7          	lui	a5,0x1
    5154:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    5158:	00400713          	li	a4,4
    515c:	00500693          	li	a3,5
    5160:	01600613          	li	a2,22
    5164:	00d00593          	li	a1,13
    5168:	06810513          	addi	a0,sp,104
    516c:	371030ef          	jal	8cdc <gfx_fill_rect>
    5170:	000155b7          	lui	a1,0x15
    5174:	00b00693          	li	a3,11
    5178:	00900613          	li	a2,9
    517c:	96858593          	addi	a1,a1,-1688 # 14968 <gem_pixels>
    5180:	06810513          	addi	a0,sp,104
    5184:	2d1030ef          	jal	8c54 <gfx_begin>
    5188:	000107b7          	lui	a5,0x10
    518c:	81f78793          	addi	a5,a5,-2017 # f81f <demo_init+0x19b>
    5190:	00b00713          	li	a4,11
    5194:	00900693          	li	a3,9
    5198:	00000613          	li	a2,0
    519c:	00000593          	li	a1,0
    51a0:	06810513          	addi	a0,sp,104
    51a4:	339030ef          	jal	8cdc <gfx_fill_rect>
    51a8:	000017b7          	lui	a5,0x1
    51ac:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    51b0:	00200713          	li	a4,2
    51b4:	00300693          	li	a3,3
    51b8:	00000613          	li	a2,0
    51bc:	00300593          	li	a1,3
    51c0:	06810513          	addi	a0,sp,104
    51c4:	319030ef          	jal	8cdc <gfx_fill_rect>
    51c8:	000017b7          	lui	a5,0x1
    51cc:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    51d0:	00600713          	li	a4,6
    51d4:	00700693          	li	a3,7
    51d8:	00200613          	li	a2,2
    51dc:	00100593          	li	a1,1
    51e0:	06810513          	addi	a0,sp,104
    51e4:	2f9030ef          	jal	8cdc <gfx_fill_rect>
    51e8:	000017b7          	lui	a5,0x1
    51ec:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    51f0:	00200713          	li	a4,2
    51f4:	00300693          	li	a3,3
    51f8:	00800613          	li	a2,8
    51fc:	00300593          	li	a1,3
    5200:	06810513          	addi	a0,sp,104
    5204:	2d9030ef          	jal	8cdc <gfx_fill_rect>
    5208:	000077b7          	lui	a5,0x7
    520c:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    5210:	00600713          	li	a4,6
    5214:	00300693          	li	a3,3
    5218:	00200613          	li	a2,2
    521c:	00300593          	li	a1,3
    5220:	06810513          	addi	a0,sp,104
    5224:	2b9030ef          	jal	8cdc <gfx_fill_rect>
    5228:	000077b7          	lui	a5,0x7
    522c:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    5230:	00300713          	li	a4,3
    5234:	00500693          	li	a3,5
    5238:	00300613          	li	a2,3
    523c:	00200593          	li	a1,2
    5240:	06810513          	addi	a0,sp,104
    5244:	299030ef          	jal	8cdc <gfx_fill_rect>
    5248:	0000f7b7          	lui	a5,0xf
    524c:	00200693          	li	a3,2
    5250:	00200613          	li	a2,2
    5254:	00300593          	li	a1,3
    5258:	06810513          	addi	a0,sp,104
    525c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    5260:	00200713          	li	a4,2
    5264:	279030ef          	jal	8cdc <gfx_fill_rect>
    5268:	000133b7          	lui	t2,0x13
    526c:	a2838393          	addi	t2,t2,-1496 # 12a28 <patterns.3>
    5270:	000016b7          	lui	a3,0x1
    5274:	00010337          	lui	t1,0x10
    5278:	0000f8b7          	lui	a7,0xf
    527c:	000134b7          	lui	s1,0x13
    5280:	05a38293          	addi	t0,t2,90
    5284:	83818813          	addi	a6,gp,-1992 # 13440 <body.2>
    5288:	83018513          	addi	a0,gp,-2000 # 13438 <light.1>
    528c:	4a048493          	addi	s1,s1,1184 # 134a0 <hostile_pixels>
    5290:	1c238393          	addi	t2,t2,450
    5294:	07800613          	li	a2,120
    5298:	8c368693          	addi	a3,a3,-1853 # 8c3 <CUSTOM2+0x868>
    529c:	06200593          	li	a1,98
    52a0:	06800e93          	li	t4,104
    52a4:	07700e13          	li	t3,119
    52a8:	81f30313          	addi	t1,t1,-2017 # f81f <demo_init+0x19b>
    52ac:	77a88893          	addi	a7,a7,1914 # f77a <demo_init+0xf6>
    52b0:	fa628793          	addi	a5,t0,-90
    52b4:	00048713          	mv	a4,s1
    52b8:	1440006f          	j	53fc <__stack_size+0x13fc>
    52bc:	21df0ae3          	beq	t5,t4,5cd0 <__stack_size+0x1cd0>
    52c0:	00030f93          	mv	t6,t1
    52c4:	01cf1463          	bne	t5,t3,52cc <__stack_size+0x12cc>
    52c8:	00088f93          	mv	t6,a7
    52cc:	0017cf03          	lbu	t5,1(a5)
    52d0:	01f71023          	sh	t6,0(a4)
    52d4:	00068f93          	mv	t6,a3
    52d8:	00cf0c63          	beq	t5,a2,52f0 <__stack_size+0x12f0>
    52dc:	58bf0863          	beq	t5,a1,586c <__stack_size+0x186c>
    52e0:	21df00e3          	beq	t5,t4,5ce0 <__stack_size+0x1ce0>
    52e4:	00030f93          	mv	t6,t1
    52e8:	01cf1463          	bne	t5,t3,52f0 <__stack_size+0x12f0>
    52ec:	00088f93          	mv	t6,a7
    52f0:	0027cf03          	lbu	t5,2(a5)
    52f4:	01f71123          	sh	t6,2(a4)
    52f8:	00068f93          	mv	t6,a3
    52fc:	00cf0c63          	beq	t5,a2,5314 <__stack_size+0x1314>
    5300:	56bf0263          	beq	t5,a1,5864 <__stack_size+0x1864>
    5304:	1ddf0ae3          	beq	t5,t4,5cd8 <__stack_size+0x1cd8>
    5308:	00030f93          	mv	t6,t1
    530c:	01cf1463          	bne	t5,t3,5314 <__stack_size+0x1314>
    5310:	00088f93          	mv	t6,a7
    5314:	0037cf03          	lbu	t5,3(a5)
    5318:	01f71223          	sh	t6,4(a4)
    531c:	00068f93          	mv	t6,a3
    5320:	00cf0c63          	beq	t5,a2,5338 <__stack_size+0x1338>
    5324:	52bf0c63          	beq	t5,a1,585c <__stack_size+0x185c>
    5328:	1ddf04e3          	beq	t5,t4,5cf0 <__stack_size+0x1cf0>
    532c:	00030f93          	mv	t6,t1
    5330:	01cf1463          	bne	t5,t3,5338 <__stack_size+0x1338>
    5334:	00088f93          	mv	t6,a7
    5338:	0047cf03          	lbu	t5,4(a5)
    533c:	01f71323          	sh	t6,6(a4)
    5340:	00068f93          	mv	t6,a3
    5344:	00cf0c63          	beq	t5,a2,535c <__stack_size+0x135c>
    5348:	50bf0663          	beq	t5,a1,5854 <__stack_size+0x1854>
    534c:	19df0ee3          	beq	t5,t4,5ce8 <__stack_size+0x1ce8>
    5350:	00030f93          	mv	t6,t1
    5354:	01cf1463          	bne	t5,t3,535c <__stack_size+0x135c>
    5358:	00088f93          	mv	t6,a7
    535c:	0057cf03          	lbu	t5,5(a5)
    5360:	01f71423          	sh	t6,8(a4)
    5364:	00068f93          	mv	t6,a3
    5368:	00cf0c63          	beq	t5,a2,5380 <__stack_size+0x1380>
    536c:	4ebf0063          	beq	t5,a1,584c <__stack_size+0x184c>
    5370:	19df08e3          	beq	t5,t4,5d00 <__stack_size+0x1d00>
    5374:	00030f93          	mv	t6,t1
    5378:	01cf1463          	bne	t5,t3,5380 <__stack_size+0x1380>
    537c:	00088f93          	mv	t6,a7
    5380:	0067cf03          	lbu	t5,6(a5)
    5384:	01f71523          	sh	t6,10(a4)
    5388:	00068f93          	mv	t6,a3
    538c:	00cf0c63          	beq	t5,a2,53a4 <__stack_size+0x13a4>
    5390:	4abf0a63          	beq	t5,a1,5844 <__stack_size+0x1844>
    5394:	17df02e3          	beq	t5,t4,5cf8 <__stack_size+0x1cf8>
    5398:	00030f93          	mv	t6,t1
    539c:	01cf1463          	bne	t5,t3,53a4 <__stack_size+0x13a4>
    53a0:	00088f93          	mv	t6,a7
    53a4:	0077cf03          	lbu	t5,7(a5)
    53a8:	01f71623          	sh	t6,12(a4)
    53ac:	00068f93          	mv	t6,a3
    53b0:	00cf0c63          	beq	t5,a2,53c8 <__stack_size+0x13c8>
    53b4:	48bf0463          	beq	t5,a1,583c <__stack_size+0x183c>
    53b8:	15df0ce3          	beq	t5,t4,5d10 <__stack_size+0x1d10>
    53bc:	00030f93          	mv	t6,t1
    53c0:	01cf1463          	bne	t5,t3,53c8 <__stack_size+0x13c8>
    53c4:	00088f93          	mv	t6,a7
    53c8:	0087cf03          	lbu	t5,8(a5)
    53cc:	01f71723          	sh	t6,14(a4)
    53d0:	00068f93          	mv	t6,a3
    53d4:	00cf0c63          	beq	t5,a2,53ec <__stack_size+0x13ec>
    53d8:	30bf0463          	beq	t5,a1,56e0 <__stack_size+0x16e0>
    53dc:	13df06e3          	beq	t5,t4,5d08 <__stack_size+0x1d08>
    53e0:	00030f93          	mv	t6,t1
    53e4:	01cf1463          	bne	t5,t3,53ec <__stack_size+0x13ec>
    53e8:	00088f93          	mv	t6,a7
    53ec:	01f71823          	sh	t6,16(a4)
    53f0:	00a78793          	addi	a5,a5,10
    53f4:	01270713          	addi	a4,a4,18
    53f8:	2ef28e63          	beq	t0,a5,56f4 <__stack_size+0x16f4>
    53fc:	0007cf03          	lbu	t5,0(a5)
    5400:	00068f93          	mv	t6,a3
    5404:	eccf04e3          	beq	t5,a2,52cc <__stack_size+0x12cc>
    5408:	eabf1ae3          	bne	t5,a1,52bc <__stack_size+0x12bc>
    540c:	00085f83          	lhu	t6,0(a6)
    5410:	ebdff06f          	j	52cc <__stack_size+0x12cc>
    5414:	00812783          	lw	a5,8(sp)
    5418:	00100693          	li	a3,1
    541c:	001c8613          	addi	a2,s9,1
    5420:	06810513          	addi	a0,sp,104
    5424:	0b9030ef          	jal	8cdc <gfx_fill_rect>
    5428:	00492593          	slti	a1,s2,4
    542c:	0015b593          	seqz	a1,a1
    5430:	00412783          	lw	a5,4(sp)
    5434:	40b005b3          	neg	a1,a1
    5438:	00b9f5b3          	and	a1,s3,a1
    543c:	00100713          	li	a4,1
    5440:	00100693          	li	a3,1
    5444:	000c8613          	mv	a2,s9
    5448:	008585b3          	add	a1,a1,s0
    544c:	06810513          	addi	a0,sp,104
    5450:	08d030ef          	jal	8cdc <gfx_fill_rect>
    5454:	001b0813          	addi	a6,s6,1
    5458:	01784463          	blt	a6,s7,5460 <__stack_size+0x1460>
    545c:	f10fe06f          	j	3b6c <render_init+0x804>
    5460:	00348493          	addi	s1,s1,3
    5464:	00240413          	addi	s0,s0,2
    5468:	00100b13          	li	s6,1
    546c:	e7cfe06f          	j	3ae8 <render_init+0x780>
    5470:	000b8793          	mv	a5,s7
    5474:	00100713          	li	a4,1
    5478:	00100693          	li	a3,1
    547c:	000b0613          	mv	a2,s6
    5480:	00048593          	mv	a1,s1
    5484:	06810513          	addi	a0,sp,104
    5488:	055030ef          	jal	8cdc <gfx_fill_rect>
    548c:	01548463          	beq	s1,s5,5494 <__stack_size+0x1494>
    5490:	ce9fe06f          	j	4178 <__stack_size+0x178>
    5494:	d11fe06f          	j	41a4 <__stack_size+0x1a4>
    5498:	0000e7b7          	lui	a5,0xe
    549c:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    54a0:	00100713          	li	a4,1
    54a4:	00100693          	li	a3,1
    54a8:	00040613          	mv	a2,s0
    54ac:	01200593          	li	a1,18
    54b0:	06810513          	addi	a0,sp,104
    54b4:	029030ef          	jal	8cdc <gfx_fill_rect>
    54b8:	013d5463          	bge	s10,s3,54c0 <__stack_size+0x14c0>
    54bc:	e49fe06f          	j	4304 <__stack_size+0x304>
    54c0:	0000e7b7          	lui	a5,0xe
    54c4:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    54c8:	00100713          	li	a4,1
    54cc:	00100693          	li	a3,1
    54d0:	00040613          	mv	a2,s0
    54d4:	01300593          	li	a1,19
    54d8:	06810513          	addi	a0,sp,104
    54dc:	001030ef          	jal	8cdc <gfx_fill_rect>
    54e0:	e25fe06f          	j	4304 <__stack_size+0x304>
    54e4:	0000e7b7          	lui	a5,0xe
    54e8:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    54ec:	00100713          	li	a4,1
    54f0:	00100693          	li	a3,1
    54f4:	00040613          	mv	a2,s0
    54f8:	01100593          	li	a1,17
    54fc:	06810513          	addi	a0,sp,104
    5500:	7dc030ef          	jal	8cdc <gfx_fill_rect>
    5504:	012cd463          	bge	s9,s2,550c <__stack_size+0x150c>
    5508:	df5fe06f          	j	42fc <__stack_size+0x2fc>
    550c:	f8dff06f          	j	5498 <__stack_size+0x1498>
    5510:	0000e7b7          	lui	a5,0xe
    5514:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5518:	00100713          	li	a4,1
    551c:	00100693          	li	a3,1
    5520:	00040613          	mv	a2,s0
    5524:	01000593          	li	a1,16
    5528:	06810513          	addi	a0,sp,104
    552c:	7b0030ef          	jal	8cdc <gfx_fill_rect>
    5530:	012ad463          	bge	s5,s2,5538 <__stack_size+0x1538>
    5534:	dc1fe06f          	j	42f4 <__stack_size+0x2f4>
    5538:	fadff06f          	j	54e4 <__stack_size+0x14e4>
    553c:	000c0793          	mv	a5,s8
    5540:	00100713          	li	a4,1
    5544:	00100693          	li	a3,1
    5548:	000b8613          	mv	a2,s7
    554c:	00090593          	mv	a1,s2
    5550:	06810513          	addi	a0,sp,104
    5554:	788030ef          	jal	8cdc <gfx_fill_rect>
    5558:	01690463          	beq	s2,s6,5560 <__stack_size+0x1560>
    555c:	f61fe06f          	j	44bc <__stack_size+0x4bc>
    5560:	f89fe06f          	j	44e8 <__stack_size+0x4e8>
    5564:	000c0793          	mv	a5,s8
    5568:	00100713          	li	a4,1
    556c:	00100693          	li	a3,1
    5570:	000b8613          	mv	a2,s7
    5574:	00090593          	mv	a1,s2
    5578:	06810513          	addi	a0,sp,104
    557c:	760030ef          	jal	8cdc <gfx_fill_rect>
    5580:	9d691063          	bne	s2,s6,4740 <__stack_size+0x740>
    5584:	9d8ff06f          	j	475c <__stack_size+0x75c>
    5588:	000d8793          	mv	a5,s11
    558c:	00100713          	li	a4,1
    5590:	00100693          	li	a3,1
    5594:	000a0613          	mv	a2,s4
    5598:	00f00593          	li	a1,15
    559c:	06810513          	addi	a0,sp,104
    55a0:	73c030ef          	jal	8cdc <gfx_fill_rect>
    55a4:	a75c4063          	blt	s8,s5,4804 <__stack_size+0x804>
    55a8:	000d8793          	mv	a5,s11
    55ac:	00100713          	li	a4,1
    55b0:	00100693          	li	a3,1
    55b4:	000a0613          	mv	a2,s4
    55b8:	01000593          	li	a1,16
    55bc:	06810513          	addi	a0,sp,104
    55c0:	71c030ef          	jal	8cdc <gfx_fill_rect>
    55c4:	a40ff06f          	j	4804 <__stack_size+0x804>
    55c8:	000d8793          	mv	a5,s11
    55cc:	00100713          	li	a4,1
    55d0:	00100693          	li	a3,1
    55d4:	000a0613          	mv	a2,s4
    55d8:	00e00593          	li	a1,14
    55dc:	06810513          	addi	a0,sp,104
    55e0:	6fc030ef          	jal	8cdc <gfx_fill_rect>
    55e4:	a12bce63          	blt	s7,s2,4800 <__stack_size+0x800>
    55e8:	fa1ff06f          	j	5588 <__stack_size+0x1588>
    55ec:	000d0793          	mv	a5,s10
    55f0:	00100713          	li	a4,1
    55f4:	00100693          	li	a3,1
    55f8:	000c8613          	mv	a2,s9
    55fc:	00090593          	mv	a1,s2
    5600:	06810513          	addi	a0,sp,104
    5604:	6d8030ef          	jal	8cdc <gfx_fill_rect>
    5608:	b9791e63          	bne	s2,s7,49a4 <__stack_size+0x9a4>
    560c:	bb4ff06f          	j	49c0 <__stack_size+0x9c0>
    5610:	000d8793          	mv	a5,s11
    5614:	00100713          	li	a4,1
    5618:	00100693          	li	a3,1
    561c:	00098613          	mv	a2,s3
    5620:	000a0593          	mv	a1,s4
    5624:	06810513          	addi	a0,sp,104
    5628:	6b4030ef          	jal	8cdc <gfx_fill_rect>
    562c:	e39a1663          	bne	s4,s9,4c58 <__stack_size+0xc58>
    5630:	e44ff06f          	j	4c74 <__stack_size+0xc74>
    5634:	000c0793          	mv	a5,s8
    5638:	00100713          	li	a4,1
    563c:	00100693          	li	a3,1
    5640:	00048613          	mv	a2,s1
    5644:	00090593          	mv	a1,s2
    5648:	06810513          	addi	a0,sp,104
    564c:	690030ef          	jal	8cdc <gfx_fill_rect>
    5650:	91791ae3          	bne	s2,s7,4f64 <__stack_size+0xf64>
    5654:	92dff06f          	j	4f80 <__stack_size+0xf80>
    5658:	00048793          	mv	a5,s1
    565c:	00100713          	li	a4,1
    5660:	00100693          	li	a3,1
    5664:	00040613          	mv	a2,s0
    5668:	01100593          	li	a1,17
    566c:	06810513          	addi	a0,sp,104
    5670:	66c030ef          	jal	8cdc <gfx_fill_rect>
    5674:	9b3ac6e3          	blt	s5,s3,5020 <__stack_size+0x1020>
    5678:	00048793          	mv	a5,s1
    567c:	00100713          	li	a4,1
    5680:	00100693          	li	a3,1
    5684:	00040613          	mv	a2,s0
    5688:	01200593          	li	a1,18
    568c:	06810513          	addi	a0,sp,104
    5690:	64c030ef          	jal	8cdc <gfx_fill_rect>
    5694:	98dff06f          	j	5020 <__stack_size+0x1020>
    5698:	00048793          	mv	a5,s1
    569c:	00100713          	li	a4,1
    56a0:	00100693          	li	a3,1
    56a4:	00040613          	mv	a2,s0
    56a8:	01000593          	li	a1,16
    56ac:	06810513          	addi	a0,sp,104
    56b0:	62c030ef          	jal	8cdc <gfx_fill_rect>
    56b4:	972cc4e3          	blt	s9,s2,501c <__stack_size+0x101c>
    56b8:	fa1ff06f          	j	5658 <__stack_size+0x1658>
    56bc:	00048793          	mv	a5,s1
    56c0:	00100713          	li	a4,1
    56c4:	00100693          	li	a3,1
    56c8:	00040613          	mv	a2,s0
    56cc:	00f00593          	li	a1,15
    56d0:	06810513          	addi	a0,sp,104
    56d4:	608030ef          	jal	8cdc <gfx_fill_rect>
    56d8:	952c40e3          	blt	s8,s2,5018 <__stack_size+0x1018>
    56dc:	fbdff06f          	j	5698 <__stack_size+0x1698>
    56e0:	00085f83          	lhu	t6,0(a6)
    56e4:	00a78793          	addi	a5,a5,10
    56e8:	01270713          	addi	a4,a4,18
    56ec:	fff71f23          	sh	t6,-2(a4)
    56f0:	d0f296e3          	bne	t0,a5,53fc <__stack_size+0x13fc>
    56f4:	05a28293          	addi	t0,t0,90
    56f8:	00280813          	addi	a6,a6,2
    56fc:	00250513          	addi	a0,a0,2
    5700:	0a248493          	addi	s1,s1,162
    5704:	ba5396e3          	bne	t2,t0,52b0 <__stack_size+0x12b0>
    5708:	000147b7          	lui	a5,0x14
    570c:	4f078b13          	addi	s6,a5,1264 # 144f0 <shadow_px>
    5710:	00610737          	lui	a4,0x610
    5714:	4f078793          	addi	a5,a5,1264
    5718:	478b0693          	addi	a3,s6,1144
    571c:	06170713          	addi	a4,a4,97 # 610061 <__freertos_irq_stack_top+0x527ec1>
    5720:	00e7a023          	sw	a4,0(a5)
    5724:	00478793          	addi	a5,a5,4
    5728:	fed79ce3          	bne	a5,a3,5720 <__stack_size+0x1720>
    572c:	00001ab7          	lui	s5,0x1
    5730:	00013a37          	lui	s4,0x13
    5734:	000133b7          	lui	t2,0x13
    5738:	ba0a0a13          	addi	s4,s4,-1120 # 12ba0 <shadow_radius>
    573c:	b2018293          	addi	t0,gp,-1248 # 13728 <shadow_sprites>
    5740:	78838393          	addi	t2,t2,1928 # 13788 <shadow_alpha>
    5744:	00800793          	li	a5,8
    5748:	03400c93          	li	s9,52
    574c:	ffe00c13          	li	s8,-2
    5750:	07900993          	li	s3,121
    5754:	00100913          	li	s2,1
    5758:	0ff00313          	li	t1,255
    575c:	10000893          	li	a7,256
    5760:	12c00f13          	li	t5,300
    5764:	09600e93          	li	t4,150
    5768:	00c00493          	li	s1,12
    576c:	00b00b93          	li	s7,11
    5770:	d68a8a93          	addi	s5,s5,-664 # d68 <CUSTOM2+0xd0d>
    5774:	00378513          	addi	a0,a5,3
    5778:	00151513          	slli	a0,a0,0x1
    577c:	00acd463          	bge	s9,a0,5784 <__stack_size+0x1784>
    5780:	03400513          	li	a0,52
    5784:	02a50e33          	mul	t3,a0,a0
    5788:	0987c463          	blt	a5,s8,5810 <__stack_size+0x1810>
    578c:	00038813          	mv	a6,t2
    5790:	ff600f93          	li	t6,-10
    5794:	40a906b3          	sub	a3,s2,a0
    5798:	00000713          	li	a4,0
    579c:	03ff85b3          	mul	a1,t6,t6
    57a0:	00859593          	slli	a1,a1,0x8
    57a4:	0335c5b3          	div	a1,a1,s3
    57a8:	01c0006f          	j	57c4 <__stack_size+0x17c4>
    57ac:	00000613          	li	a2,0
    57b0:	00e807b3          	add	a5,a6,a4
    57b4:	00c78023          	sb	a2,0(a5)
    57b8:	00170713          	addi	a4,a4,1
    57bc:	00268693          	addi	a3,a3,2
    57c0:	04a75263          	bge	a4,a0,5804 <__stack_size+0x1804>
    57c4:	02d687b3          	mul	a5,a3,a3
    57c8:	09600613          	li	a2,150
    57cc:	00879793          	slli	a5,a5,0x8
    57d0:	03c7c7b3          	div	a5,a5,t3
    57d4:	00b787b3          	add	a5,a5,a1
    57d8:	40f88d33          	sub	s10,a7,a5
    57dc:	fcf348e3          	blt	t1,a5,57ac <__stack_size+0x17ac>
    57e0:	03ed0d33          	mul	s10,s10,t5
    57e4:	408d5d13          	srai	s10,s10,0x8
    57e8:	fdaec4e3          	blt	t4,s10,57b0 <__stack_size+0x17b0>
    57ec:	00e807b3          	add	a5,a6,a4
    57f0:	0ffd7613          	zext.b	a2,s10
    57f4:	00c78023          	sb	a2,0(a5)
    57f8:	00170713          	addi	a4,a4,1
    57fc:	00268693          	addi	a3,a3,2
    5800:	fca742e3          	blt	a4,a0,57c4 <__stack_size+0x17c4>
    5804:	002f8f93          	addi	t6,t6,2
    5808:	00a80833          	add	a6,a6,a0
    580c:	f89f94e3          	bne	t6,s1,5794 <__stack_size+0x1794>
    5810:	0072a623          	sw	t2,12(t0)
    5814:	00a2a023          	sw	a0,0(t0)
    5818:	0172a223          	sw	s7,4(t0)
    581c:	0162a423          	sw	s6,8(t0)
    5820:	23c40413          	addi	s0,s0,572
    5824:	004a0a13          	addi	s4,s4,4
    5828:	01028293          	addi	t0,t0,16
    582c:	23c38393          	addi	t2,t2,572
    5830:	4f540463          	beq	s0,s5,5d18 <__stack_size+0x1d18>
    5834:	000a2783          	lw	a5,0(s4)
    5838:	f3dff06f          	j	5774 <__stack_size+0x1774>
    583c:	00085f83          	lhu	t6,0(a6)
    5840:	b89ff06f          	j	53c8 <__stack_size+0x13c8>
    5844:	00085f83          	lhu	t6,0(a6)
    5848:	b5dff06f          	j	53a4 <__stack_size+0x13a4>
    584c:	00085f83          	lhu	t6,0(a6)
    5850:	b31ff06f          	j	5380 <__stack_size+0x1380>
    5854:	00085f83          	lhu	t6,0(a6)
    5858:	b05ff06f          	j	535c <__stack_size+0x135c>
    585c:	00085f83          	lhu	t6,0(a6)
    5860:	ad9ff06f          	j	5338 <__stack_size+0x1338>
    5864:	00085f83          	lhu	t6,0(a6)
    5868:	aadff06f          	j	5314 <__stack_size+0x1314>
    586c:	00085f83          	lhu	t6,0(a6)
    5870:	a81ff06f          	j	52f0 <__stack_size+0x12f0>
    5874:	00048793          	mv	a5,s1
    5878:	00100713          	li	a4,1
    587c:	00100693          	li	a3,1
    5880:	00040613          	mv	a2,s0
    5884:	00300593          	li	a1,3
    5888:	06810513          	addi	a0,sp,104
    588c:	450030ef          	jal	8cdc <gfx_fill_rect>
    5890:	6e400993          	li	s3,1764
    5894:	00048793          	mv	a5,s1
    5898:	00100713          	li	a4,1
    589c:	00100693          	li	a3,1
    58a0:	00040613          	mv	a2,s0
    58a4:	00400593          	li	a1,4
    58a8:	06810513          	addi	a0,sp,104
    58ac:	430030ef          	jal	8cdc <gfx_fill_rect>
    58b0:	00048793          	mv	a5,s1
    58b4:	00100713          	li	a4,1
    58b8:	00100693          	li	a3,1
    58bc:	00040613          	mv	a2,s0
    58c0:	00500593          	li	a1,5
    58c4:	06810513          	addi	a0,sp,104
    58c8:	414030ef          	jal	8cdc <gfx_fill_rect>
    58cc:	00048793          	mv	a5,s1
    58d0:	00100713          	li	a4,1
    58d4:	00100693          	li	a3,1
    58d8:	00040613          	mv	a2,s0
    58dc:	00600593          	li	a1,6
    58e0:	06810513          	addi	a0,sp,104
    58e4:	3f8030ef          	jal	8cdc <gfx_fill_rect>
    58e8:	00048793          	mv	a5,s1
    58ec:	00100713          	li	a4,1
    58f0:	00100693          	li	a3,1
    58f4:	00040613          	mv	a2,s0
    58f8:	00700593          	li	a1,7
    58fc:	06810513          	addi	a0,sp,104
    5900:	3dc030ef          	jal	8cdc <gfx_fill_rect>
    5904:	00048793          	mv	a5,s1
    5908:	00100713          	li	a4,1
    590c:	00100693          	li	a3,1
    5910:	00040613          	mv	a2,s0
    5914:	00800593          	li	a1,8
    5918:	06810513          	addi	a0,sp,104
    591c:	3c0030ef          	jal	8cdc <gfx_fill_rect>
    5920:	00048793          	mv	a5,s1
    5924:	00100713          	li	a4,1
    5928:	00100693          	li	a3,1
    592c:	00040613          	mv	a2,s0
    5930:	00900593          	li	a1,9
    5934:	06810513          	addi	a0,sp,104
    5938:	3a4030ef          	jal	8cdc <gfx_fill_rect>
    593c:	00048793          	mv	a5,s1
    5940:	00100713          	li	a4,1
    5944:	00100693          	li	a3,1
    5948:	00040613          	mv	a2,s0
    594c:	00a00593          	li	a1,10
    5950:	06810513          	addi	a0,sp,104
    5954:	388030ef          	jal	8cdc <gfx_fill_rect>
    5958:	00048793          	mv	a5,s1
    595c:	00100713          	li	a4,1
    5960:	00100693          	li	a3,1
    5964:	00040613          	mv	a2,s0
    5968:	00b00593          	li	a1,11
    596c:	06810513          	addi	a0,sp,104
    5970:	36c030ef          	jal	8cdc <gfx_fill_rect>
    5974:	00048793          	mv	a5,s1
    5978:	00100713          	li	a4,1
    597c:	00100693          	li	a3,1
    5980:	00040613          	mv	a2,s0
    5984:	00c00593          	li	a1,12
    5988:	06810513          	addi	a0,sp,104
    598c:	350030ef          	jal	8cdc <gfx_fill_rect>
    5990:	032b5663          	bge	s6,s2,59bc <__stack_size+0x19bc>
    5994:	e72d4e63          	blt	s10,s2,5010 <__stack_size+0x1010>
    5998:	00048793          	mv	a5,s1
    599c:	00100713          	li	a4,1
    59a0:	00100693          	li	a3,1
    59a4:	00040613          	mv	a2,s0
    59a8:	00e00593          	li	a1,14
    59ac:	06810513          	addi	a0,sp,104
    59b0:	32c030ef          	jal	8cdc <gfx_fill_rect>
    59b4:	e72bc063          	blt	s7,s2,5014 <__stack_size+0x1014>
    59b8:	d05ff06f          	j	56bc <__stack_size+0x16bc>
    59bc:	00048793          	mv	a5,s1
    59c0:	00100713          	li	a4,1
    59c4:	00100693          	li	a3,1
    59c8:	00040613          	mv	a2,s0
    59cc:	00d00593          	li	a1,13
    59d0:	06810513          	addi	a0,sp,104
    59d4:	308030ef          	jal	8cdc <gfx_fill_rect>
    59d8:	fbdff06f          	j	5994 <__stack_size+0x1994>
    59dc:	000d8793          	mv	a5,s11
    59e0:	00100713          	li	a4,1
    59e4:	00100693          	li	a3,1
    59e8:	000a0613          	mv	a2,s4
    59ec:	00300593          	li	a1,3
    59f0:	06810513          	addi	a0,sp,104
    59f4:	2e8030ef          	jal	8cdc <gfx_fill_rect>
    59f8:	51000a93          	li	s5,1296
    59fc:	000d8793          	mv	a5,s11
    5a00:	00100713          	li	a4,1
    5a04:	00100693          	li	a3,1
    5a08:	000a0613          	mv	a2,s4
    5a0c:	00400593          	li	a1,4
    5a10:	06810513          	addi	a0,sp,104
    5a14:	2c8030ef          	jal	8cdc <gfx_fill_rect>
    5a18:	000d8793          	mv	a5,s11
    5a1c:	00100713          	li	a4,1
    5a20:	00100693          	li	a3,1
    5a24:	000a0613          	mv	a2,s4
    5a28:	00500593          	li	a1,5
    5a2c:	06810513          	addi	a0,sp,104
    5a30:	2ac030ef          	jal	8cdc <gfx_fill_rect>
    5a34:	000d8793          	mv	a5,s11
    5a38:	00100713          	li	a4,1
    5a3c:	00100693          	li	a3,1
    5a40:	000a0613          	mv	a2,s4
    5a44:	00600593          	li	a1,6
    5a48:	06810513          	addi	a0,sp,104
    5a4c:	290030ef          	jal	8cdc <gfx_fill_rect>
    5a50:	000d8793          	mv	a5,s11
    5a54:	00100713          	li	a4,1
    5a58:	00100693          	li	a3,1
    5a5c:	000a0613          	mv	a2,s4
    5a60:	00700593          	li	a1,7
    5a64:	06810513          	addi	a0,sp,104
    5a68:	274030ef          	jal	8cdc <gfx_fill_rect>
    5a6c:	000d8793          	mv	a5,s11
    5a70:	00100713          	li	a4,1
    5a74:	00100693          	li	a3,1
    5a78:	000a0613          	mv	a2,s4
    5a7c:	00800593          	li	a1,8
    5a80:	06810513          	addi	a0,sp,104
    5a84:	258030ef          	jal	8cdc <gfx_fill_rect>
    5a88:	000d8793          	mv	a5,s11
    5a8c:	00100713          	li	a4,1
    5a90:	00100693          	li	a3,1
    5a94:	000a0613          	mv	a2,s4
    5a98:	00900593          	li	a1,9
    5a9c:	06810513          	addi	a0,sp,104
    5aa0:	23c030ef          	jal	8cdc <gfx_fill_rect>
    5aa4:	000d8793          	mv	a5,s11
    5aa8:	00100713          	li	a4,1
    5aac:	00100693          	li	a3,1
    5ab0:	000a0613          	mv	a2,s4
    5ab4:	00a00593          	li	a1,10
    5ab8:	06810513          	addi	a0,sp,104
    5abc:	220030ef          	jal	8cdc <gfx_fill_rect>
    5ac0:	000d8793          	mv	a5,s11
    5ac4:	00100713          	li	a4,1
    5ac8:	00100693          	li	a3,1
    5acc:	000a0613          	mv	a2,s4
    5ad0:	00b00593          	li	a1,11
    5ad4:	06810513          	addi	a0,sp,104
    5ad8:	204030ef          	jal	8cdc <gfx_fill_rect>
    5adc:	032d5a63          	bge	s10,s2,5b10 <__stack_size+0x1b10>
    5ae0:	012cd463          	bge	s9,s2,5ae8 <__stack_size+0x1ae8>
    5ae4:	d15fe06f          	j	47f8 <__stack_size+0x7f8>
    5ae8:	000d8793          	mv	a5,s11
    5aec:	00100713          	li	a4,1
    5af0:	00100693          	li	a3,1
    5af4:	000a0613          	mv	a2,s4
    5af8:	00d00593          	li	a1,13
    5afc:	06810513          	addi	a0,sp,104
    5b00:	1dc030ef          	jal	8cdc <gfx_fill_rect>
    5b04:	012b5463          	bge	s6,s2,5b0c <__stack_size+0x1b0c>
    5b08:	cf5fe06f          	j	47fc <__stack_size+0x7fc>
    5b0c:	abdff06f          	j	55c8 <__stack_size+0x15c8>
    5b10:	000d8793          	mv	a5,s11
    5b14:	00100713          	li	a4,1
    5b18:	00100693          	li	a3,1
    5b1c:	000a0613          	mv	a2,s4
    5b20:	00c00593          	li	a1,12
    5b24:	06810513          	addi	a0,sp,104
    5b28:	1b4030ef          	jal	8cdc <gfx_fill_rect>
    5b2c:	fb5ff06f          	j	5ae0 <__stack_size+0x1ae0>
    5b30:	0000e7b7          	lui	a5,0xe
    5b34:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5b38:	00100713          	li	a4,1
    5b3c:	00100693          	li	a3,1
    5b40:	00040613          	mv	a2,s0
    5b44:	00400593          	li	a1,4
    5b48:	06810513          	addi	a0,sp,104
    5b4c:	190030ef          	jal	8cdc <gfx_fill_rect>
    5b50:	000b0993          	mv	s3,s6
    5b54:	0000e7b7          	lui	a5,0xe
    5b58:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5b5c:	00100713          	li	a4,1
    5b60:	00100693          	li	a3,1
    5b64:	00040613          	mv	a2,s0
    5b68:	00500593          	li	a1,5
    5b6c:	06810513          	addi	a0,sp,104
    5b70:	16c030ef          	jal	8cdc <gfx_fill_rect>
    5b74:	0000e7b7          	lui	a5,0xe
    5b78:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5b7c:	00100713          	li	a4,1
    5b80:	00100693          	li	a3,1
    5b84:	00040613          	mv	a2,s0
    5b88:	00600593          	li	a1,6
    5b8c:	06810513          	addi	a0,sp,104
    5b90:	14c030ef          	jal	8cdc <gfx_fill_rect>
    5b94:	0000e7b7          	lui	a5,0xe
    5b98:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5b9c:	00100713          	li	a4,1
    5ba0:	00100693          	li	a3,1
    5ba4:	00040613          	mv	a2,s0
    5ba8:	00700593          	li	a1,7
    5bac:	06810513          	addi	a0,sp,104
    5bb0:	12c030ef          	jal	8cdc <gfx_fill_rect>
    5bb4:	0000e7b7          	lui	a5,0xe
    5bb8:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5bbc:	00100713          	li	a4,1
    5bc0:	00100693          	li	a3,1
    5bc4:	00040613          	mv	a2,s0
    5bc8:	00800593          	li	a1,8
    5bcc:	06810513          	addi	a0,sp,104
    5bd0:	10c030ef          	jal	8cdc <gfx_fill_rect>
    5bd4:	0000e7b7          	lui	a5,0xe
    5bd8:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5bdc:	00100713          	li	a4,1
    5be0:	00100693          	li	a3,1
    5be4:	00040613          	mv	a2,s0
    5be8:	00900593          	li	a1,9
    5bec:	06810513          	addi	a0,sp,104
    5bf0:	0ec030ef          	jal	8cdc <gfx_fill_rect>
    5bf4:	0000e7b7          	lui	a5,0xe
    5bf8:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5bfc:	00100713          	li	a4,1
    5c00:	00100693          	li	a3,1
    5c04:	00040613          	mv	a2,s0
    5c08:	00a00593          	li	a1,10
    5c0c:	06810513          	addi	a0,sp,104
    5c10:	0cc030ef          	jal	8cdc <gfx_fill_rect>
    5c14:	0000e7b7          	lui	a5,0xe
    5c18:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5c1c:	00100713          	li	a4,1
    5c20:	00100693          	li	a3,1
    5c24:	00040613          	mv	a2,s0
    5c28:	00b00593          	li	a1,11
    5c2c:	06810513          	addi	a0,sp,104
    5c30:	0ac030ef          	jal	8cdc <gfx_fill_rect>
    5c34:	0000e7b7          	lui	a5,0xe
    5c38:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5c3c:	00100713          	li	a4,1
    5c40:	00100693          	li	a3,1
    5c44:	00040613          	mv	a2,s0
    5c48:	00c00593          	li	a1,12
    5c4c:	06810513          	addi	a0,sp,104
    5c50:	08c030ef          	jal	8cdc <gfx_fill_rect>
    5c54:	0000e7b7          	lui	a5,0xe
    5c58:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5c5c:	00100713          	li	a4,1
    5c60:	00100693          	li	a3,1
    5c64:	00040613          	mv	a2,s0
    5c68:	00d00593          	li	a1,13
    5c6c:	06810513          	addi	a0,sp,104
    5c70:	06c030ef          	jal	8cdc <gfx_fill_rect>
    5c74:	032c5c63          	bge	s8,s2,5cac <__stack_size+0x1cac>
    5c78:	012bd463          	bge	s7,s2,5c80 <__stack_size+0x1c80>
    5c7c:	e68fe06f          	j	42e4 <__stack_size+0x2e4>
    5c80:	0000e7b7          	lui	a5,0xe
    5c84:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5c88:	00100713          	li	a4,1
    5c8c:	00100693          	li	a3,1
    5c90:	00040613          	mv	a2,s0
    5c94:	00f00593          	li	a1,15
    5c98:	06810513          	addi	a0,sp,104
    5c9c:	040030ef          	jal	8cdc <gfx_fill_rect>
    5ca0:	012a5463          	bge	s4,s2,5ca8 <__stack_size+0x1ca8>
    5ca4:	e48fe06f          	j	42ec <__stack_size+0x2ec>
    5ca8:	869ff06f          	j	5510 <__stack_size+0x1510>
    5cac:	0000e7b7          	lui	a5,0xe
    5cb0:	58c78793          	addi	a5,a5,1420 # e58c <game_tick+0x1700>
    5cb4:	00100713          	li	a4,1
    5cb8:	00100693          	li	a3,1
    5cbc:	00040613          	mv	a2,s0
    5cc0:	00e00593          	li	a1,14
    5cc4:	06810513          	addi	a0,sp,104
    5cc8:	014030ef          	jal	8cdc <gfx_fill_rect>
    5ccc:	fadff06f          	j	5c78 <__stack_size+0x1c78>
    5cd0:	00055f83          	lhu	t6,0(a0)
    5cd4:	df8ff06f          	j	52cc <__stack_size+0x12cc>
    5cd8:	00055f83          	lhu	t6,0(a0)
    5cdc:	e38ff06f          	j	5314 <__stack_size+0x1314>
    5ce0:	00055f83          	lhu	t6,0(a0)
    5ce4:	e0cff06f          	j	52f0 <__stack_size+0x12f0>
    5ce8:	00055f83          	lhu	t6,0(a0)
    5cec:	e70ff06f          	j	535c <__stack_size+0x135c>
    5cf0:	00055f83          	lhu	t6,0(a0)
    5cf4:	e44ff06f          	j	5338 <__stack_size+0x1338>
    5cf8:	00055f83          	lhu	t6,0(a0)
    5cfc:	ea8ff06f          	j	53a4 <__stack_size+0x13a4>
    5d00:	00055f83          	lhu	t6,0(a0)
    5d04:	e7cff06f          	j	5380 <__stack_size+0x1380>
    5d08:	00055f83          	lhu	t6,0(a0)
    5d0c:	ee0ff06f          	j	53ec <__stack_size+0x13ec>
    5d10:	00055f83          	lhu	t6,0(a0)
    5d14:	eb4ff06f          	j	53c8 <__stack_size+0x13c8>
    5d18:	0cc12083          	lw	ra,204(sp)
    5d1c:	0c812403          	lw	s0,200(sp)
    5d20:	0c412483          	lw	s1,196(sp)
    5d24:	0c012903          	lw	s2,192(sp)
    5d28:	0bc12983          	lw	s3,188(sp)
    5d2c:	0b812a03          	lw	s4,184(sp)
    5d30:	0b412a83          	lw	s5,180(sp)
    5d34:	0b012b03          	lw	s6,176(sp)
    5d38:	0ac12b83          	lw	s7,172(sp)
    5d3c:	0a812c03          	lw	s8,168(sp)
    5d40:	0a412c83          	lw	s9,164(sp)
    5d44:	0a012d03          	lw	s10,160(sp)
    5d48:	09c12d83          	lw	s11,156(sp)
    5d4c:	0d010113          	addi	sp,sp,208
    5d50:	00008067          	ret

00005d54 <render_game>:
    5d54:	bc010113          	addi	sp,sp,-1088
    5d58:	02062783          	lw	a5,32(a2)
    5d5c:	42812c23          	sw	s0,1080(sp)
    5d60:	000136b7          	lui	a3,0x13
    5d64:	00058413          	mv	s0,a1
    5d68:	41a12823          	sw	s10,1040(sp)
    5d6c:	02c12c23          	sw	a2,56(sp)
    5d70:	00050d13          	mv	s10,a0
    5d74:	00000713          	li	a4,0
    5d78:	40468693          	addi	a3,a3,1028 # 13404 <background>
    5d7c:	00000613          	li	a2,0
    5d80:	00000593          	li	a1,0
    5d84:	00040513          	mv	a0,s0
    5d88:	42912a23          	sw	s1,1076(sp)
    5d8c:	43212823          	sw	s2,1072(sp)
    5d90:	43312623          	sw	s3,1068(sp)
    5d94:	43412423          	sw	s4,1064(sp)
    5d98:	42112e23          	sw	ra,1084(sp)
    5d9c:	43512223          	sw	s5,1060(sp)
    5da0:	43612023          	sw	s6,1056(sp)
    5da4:	41712e23          	sw	s7,1052(sp)
    5da8:	41812c23          	sw	s8,1048(sp)
    5dac:	41912a23          	sw	s9,1044(sp)
    5db0:	41b12623          	sw	s11,1036(sp)
    5db4:	01a12e23          	sw	s10,28(sp)
    5db8:	80f1a823          	sw	a5,-2032(gp) # 13418 <fx_alpha>
    5dbc:	731020ef          	jal	8cec <gfx_blit>
    5dc0:	01c12883          	lw	a7,28(sp)
    5dc4:	000234b7          	lui	s1,0x23
    5dc8:	00024a37          	lui	s4,0x24
    5dcc:	5d448493          	addi	s1,s1,1492 # 235d4 <background_pixels+0xb714>
    5dd0:	1d4a0a13          	addi	s4,s4,468 # 241d4 <background_pixels+0xc314>
    5dd4:	009884b3          	add	s1,a7,s1
    5dd8:	01488a33          	add	s4,a7,s4
    5ddc:	000139b7          	lui	s3,0x13
    5de0:	00088913          	mv	s2,a7
    5de4:	00c0006f          	j	5df0 <render_game+0x9c>
    5de8:	01048493          	addi	s1,s1,16
    5dec:	05448a63          	beq	s1,s4,5e40 <render_game+0xec>
    5df0:	0004a783          	lw	a5,0(s1)
    5df4:	fe078ae3          	beqz	a5,5de8 <render_game+0x94>
    5df8:	0084a683          	lw	a3,8(s1)
    5dfc:	0044a783          	lw	a5,4(s1)
    5e00:	00100713          	li	a4,1
    5e04:	41f6d613          	srai	a2,a3,0x1f
    5e08:	41f7d593          	srai	a1,a5,0x1f
    5e0c:	0ff67613          	zext.b	a2,a2
    5e10:	0ff5f593          	zext.b	a1,a1
    5e14:	00d60633          	add	a2,a2,a3
    5e18:	00f585b3          	add	a1,a1,a5
    5e1c:	40865613          	srai	a2,a2,0x8
    5e20:	4085d593          	srai	a1,a1,0x8
    5e24:	39498693          	addi	a3,s3,916 # 13394 <gem>
    5e28:	ffb60613          	addi	a2,a2,-5
    5e2c:	ffc58593          	addi	a1,a1,-4
    5e30:	00040513          	mv	a0,s0
    5e34:	01048493          	addi	s1,s1,16
    5e38:	6b5020ef          	jal	8cec <gfx_blit>
    5e3c:	fb449ae3          	bne	s1,s4,5df0 <render_game+0x9c>
    5e40:	0b890d93          	addi	s11,s2,184
    5e44:	00090893          	mv	a7,s2
    5e48:	000d8713          	mv	a4,s11
    5e4c:	00000813          	li	a6,0
    5e50:	00000793          	li	a5,0
    5e54:	0c000613          	li	a2,192
    5e58:	00072683          	lw	a3,0(a4)
    5e5c:	00068e63          	beqz	a3,5e78 <render_game+0x124>
    5e60:	00281693          	slli	a3,a6,0x2
    5e64:	3c068693          	addi	a3,a3,960
    5e68:	04010593          	addi	a1,sp,64
    5e6c:	00b686b3          	add	a3,a3,a1
    5e70:	d0f6a023          	sw	a5,-768(a3)
    5e74:	00180813          	addi	a6,a6,1
    5e78:	00178793          	addi	a5,a5,1
    5e7c:	02c70713          	addi	a4,a4,44
    5e80:	fcc79ce3          	bne	a5,a2,5e58 <render_game+0x104>
    5e84:	00100793          	li	a5,1
    5e88:	10410e13          	addi	t3,sp,260
    5e8c:	00100313          	li	t1,1
    5e90:	10010913          	addi	s2,sp,256
    5e94:	02c00593          	li	a1,44
    5e98:	0107c463          	blt	a5,a6,5ea0 <render_game+0x14c>
    5e9c:	1d80106f          	j	7074 <render_game+0x1320>
    5ea0:	000e2e83          	lw	t4,0(t3)
    5ea4:	00030613          	mv	a2,t1
    5ea8:	000e0793          	mv	a5,t3
    5eac:	02be8733          	mul	a4,t4,a1
    5eb0:	00e88733          	add	a4,a7,a4
    5eb4:	0c072503          	lw	a0,192(a4)
    5eb8:	0140006f          	j	5ecc <render_game+0x178>
    5ebc:	00d7a023          	sw	a3,0(a5)
    5ec0:	ffc78793          	addi	a5,a5,-4
    5ec4:	fff60613          	addi	a2,a2,-1
    5ec8:	7ef90e63          	beq	s2,a5,66c4 <render_game+0x970>
    5ecc:	ffc7a683          	lw	a3,-4(a5)
    5ed0:	02b68733          	mul	a4,a3,a1
    5ed4:	00e88733          	add	a4,a7,a4
    5ed8:	0c072703          	lw	a4,192(a4)
    5edc:	fee540e3          	blt	a0,a4,5ebc <render_game+0x168>
    5ee0:	00261613          	slli	a2,a2,0x2
    5ee4:	3c060793          	addi	a5,a2,960
    5ee8:	04010713          	addi	a4,sp,64
    5eec:	00e78633          	add	a2,a5,a4
    5ef0:	d1d62023          	sw	t4,-768(a2)
    5ef4:	00130313          	addi	t1,t1,1
    5ef8:	004e0e13          	addi	t3,t3,4
    5efc:	fb0312e3          	bne	t1,a6,5ea0 <render_game+0x14c>
    5f00:	000137b7          	lui	a5,0x13
    5f04:	3e478793          	addi	a5,a5,996 # 133e4 <slime>
    5f08:	0000f2b7          	lui	t0,0xf
    5f0c:	00007fb7          	lui	t6,0x7
    5f10:	00f12e23          	sw	a5,28(sp)
    5f14:	000137b7          	lui	a5,0x13
    5f18:	a0c78793          	addi	a5,a5,-1524 # 12a0c <CSWTCH.84>
    5f1c:	b8e28293          	addi	t0,t0,-1138 # eb8e <game_tick+0x1d02>
    5f20:	a67f8f93          	addi	t6,t6,-1433 # 6a67 <render_game+0xd13>
    5f24:	03b12e23          	sw	s11,60(sp)
    5f28:	00090a93          	mv	s5,s2
    5f2c:	00000a13          	li	s4,0
    5f30:	00000c13          	li	s8,0
    5f34:	02c00b93          	li	s7,44
    5f38:	02f12823          	sw	a5,48(sp)
    5f3c:	03012023          	sw	a6,32(sp)
    5f40:	00088d93          	mv	s11,a7
    5f44:	02512a23          	sw	t0,52(sp)
    5f48:	03f12623          	sw	t6,44(sp)
    5f4c:	0840006f          	j	5fd0 <render_game+0x27c>
    5f50:	00600713          	li	a4,6
    5f54:	02e7c7b3          	div	a5,a5,a4
    5f58:	02c12b03          	lw	s6,44(sp)
    5f5c:	0017f793          	andi	a5,a5,1
    5f60:	00078463          	beqz	a5,5f68 <render_game+0x214>
    5f64:	03412b03          	lw	s6,52(sp)
    5f68:	000b0813          	mv	a6,s6
    5f6c:	00200793          	li	a5,2
    5f70:	01400713          	li	a4,20
    5f74:	01400693          	li	a3,20
    5f78:	ff648613          	addi	a2,s1,-10
    5f7c:	ff698593          	addi	a1,s3,-10
    5f80:	00040513          	mv	a0,s0
    5f84:	5a0030ef          	jal	9524 <gfx_border>
    5f88:	000b0793          	mv	a5,s6
    5f8c:	00200713          	li	a4,2
    5f90:	00900693          	li	a3,9
    5f94:	fff48613          	addi	a2,s1,-1
    5f98:	ffc98593          	addi	a1,s3,-4
    5f9c:	00040513          	mv	a0,s0
    5fa0:	53d020ef          	jal	8cdc <gfx_fill_rect>
    5fa4:	000b0793          	mv	a5,s6
    5fa8:	00a00713          	li	a4,10
    5fac:	00200693          	li	a3,2
    5fb0:	ffb48613          	addi	a2,s1,-5
    5fb4:	00098593          	mv	a1,s3
    5fb8:	00040513          	mv	a0,s0
    5fbc:	521020ef          	jal	8cdc <gfx_fill_rect>
    5fc0:	02012783          	lw	a5,32(sp)
    5fc4:	001a0a13          	addi	s4,s4,1
    5fc8:	004a8a93          	addi	s5,s5,4
    5fcc:	1efa0e63          	beq	s4,a5,61c8 <render_game+0x474>
    5fd0:	000aac83          	lw	s9,0(s5)
    5fd4:	037c84b3          	mul	s1,s9,s7
    5fd8:	009d84b3          	add	s1,s11,s1
    5fdc:	0c04a703          	lw	a4,192(s1)
    5fe0:	000c1663          	bnez	s8,5fec <render_game+0x298>
    5fe4:	014da783          	lw	a5,20(s11)
    5fe8:	7ce7ce63          	blt	a5,a4,67c4 <render_game+0xa70>
    5fec:	037c87b3          	mul	a5,s9,s7
    5ff0:	00300693          	li	a3,3
    5ff4:	008dae83          	lw	t4,8(s11)
    5ff8:	01c12d03          	lw	s10,28(sp)
    5ffc:	00fd87b3          	add	a5,s11,a5
    6000:	0cc7a503          	lw	a0,204(a5)
    6004:	fff50793          	addi	a5,a0,-1
    6008:	00f6ea63          	bltu	a3,a5,601c <render_game+0x2c8>
    600c:	03012683          	lw	a3,48(sp)
    6010:	00279793          	slli	a5,a5,0x2
    6014:	00f687b3          	add	a5,a3,a5
    6018:	0007ad03          	lw	s10,0(a5)
    601c:	037c8b33          	mul	s6,s9,s7
    6020:	41f75493          	srai	s1,a4,0x1f
    6024:	0ff4f493          	zext.b	s1,s1
    6028:	00e484b3          	add	s1,s1,a4
    602c:	4084d493          	srai	s1,s1,0x8
    6030:	016d8b33          	add	s6,s11,s6
    6034:	0bcb2703          	lw	a4,188(s6)
    6038:	0d0b2783          	lw	a5,208(s6)
    603c:	0d8b2f03          	lw	t5,216(s6)
    6040:	41f75993          	srai	s3,a4,0x1f
    6044:	0ff9f993          	zext.b	s3,s3
    6048:	00e989b3          	add	s3,s3,a4
    604c:	4089d993          	srai	s3,s3,0x8
    6050:	f00790e3          	bnez	a5,5f50 <render_game+0x1fc>
    6054:	03e12423          	sw	t5,40(sp)
    6058:	03d12223          	sw	t4,36(sp)
    605c:	0ad060ef          	jal	c908 <game_enemy_radius>
    6060:	00050693          	mv	a3,a0
    6064:	00848613          	addi	a2,s1,8
    6068:	00098593          	mv	a1,s3
    606c:	00040513          	mv	a0,s0
    6070:	c51fc0ef          	jal	2cc0 <shadow>
    6074:	0ccb2783          	lw	a5,204(s6)
    6078:	00300713          	li	a4,3
    607c:	02412e83          	lw	t4,36(sp)
    6080:	02812f03          	lw	t5,40(sp)
    6084:	6ce78a63          	beq	a5,a4,6758 <render_game+0xa04>
    6088:	00400713          	li	a4,4
    608c:	66e78063          	beq	a5,a4,66ec <render_game+0x998>
    6090:	003f1793          	slli	a5,t5,0x3
    6094:	41e787b3          	sub	a5,a5,t5
    6098:	01d787b3          	add	a5,a5,t4
    609c:	00c00713          	li	a4,12
    60a0:	02e7c7b3          	div	a5,a5,a4
    60a4:	000d2683          	lw	a3,0(s10)
    60a8:	004d2603          	lw	a2,4(s10)
    60ac:	00100713          	li	a4,1
    60b0:	01f6d593          	srli	a1,a3,0x1f
    60b4:	00d585b3          	add	a1,a1,a3
    60b8:	40c48633          	sub	a2,s1,a2
    60bc:	00c60613          	addi	a2,a2,12
    60c0:	4015d593          	srai	a1,a1,0x1
    60c4:	000d0693          	mv	a3,s10
    60c8:	40b985b3          	sub	a1,s3,a1
    60cc:	00040513          	mv	a0,s0
    60d0:	01f7d813          	srli	a6,a5,0x1f
    60d4:	010787b3          	add	a5,a5,a6
    60d8:	0017f793          	andi	a5,a5,1
    60dc:	410787b3          	sub	a5,a5,a6
    60e0:	40f60633          	sub	a2,a2,a5
    60e4:	409020ef          	jal	8cec <gfx_blit>
    60e8:	037c87b3          	mul	a5,s9,s7
    60ec:	00fd87b3          	add	a5,s11,a5
    60f0:	0d47a783          	lw	a5,212(a5)
    60f4:	6e079463          	bnez	a5,67dc <render_game+0xa88>
    60f8:	037c87b3          	mul	a5,s9,s7
    60fc:	00100693          	li	a3,1
    6100:	00fd87b3          	add	a5,s11,a5
    6104:	0cc7a703          	lw	a4,204(a5)
    6108:	eae6dce3          	bge	a3,a4,5fc0 <render_game+0x26c>
    610c:	0c47a683          	lw	a3,196(a5)
    6110:	0c87a783          	lw	a5,200(a5)
    6114:	eaf6d6e3          	bge	a3,a5,5fc0 <render_game+0x26c>
    6118:	00300693          	li	a3,3
    611c:	ff300793          	li	a5,-13
    6120:	01a00b13          	li	s6,26
    6124:	00d71663          	bne	a4,a3,6130 <render_game+0x3dc>
    6128:	fea00793          	li	a5,-22
    612c:	02c00b13          	li	s6,44
    6130:	004d2603          	lw	a2,4(s10)
    6134:	00f989b3          	add	s3,s3,a5
    6138:	000017b7          	lui	a5,0x1
    613c:	40c48633          	sub	a2,s1,a2
    6140:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    6144:	00300713          	li	a4,3
    6148:	000b0693          	mv	a3,s6
    614c:	00560613          	addi	a2,a2,5
    6150:	00098593          	mv	a1,s3
    6154:	00040513          	mv	a0,s0
    6158:	385020ef          	jal	8cdc <gfx_fill_rect>
    615c:	037c8733          	mul	a4,s9,s7
    6160:	004d2683          	lw	a3,4(s10)
    6164:	00300593          	li	a1,3
    6168:	03412783          	lw	a5,52(sp)
    616c:	40d48633          	sub	a2,s1,a3
    6170:	00560613          	addi	a2,a2,5
    6174:	00ed8733          	add	a4,s11,a4
    6178:	0c472683          	lw	a3,196(a4)
    617c:	0c872503          	lw	a0,200(a4)
    6180:	0cc72703          	lw	a4,204(a4)
    6184:	02db06b3          	mul	a3,s6,a3
    6188:	02a6c6b3          	div	a3,a3,a0
    618c:	00b70e63          	beq	a4,a1,61a8 <render_game+0x454>
    6190:	000077b7          	lui	a5,0x7
    6194:	00400593          	li	a1,4
    6198:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    619c:	00b70663          	beq	a4,a1,61a8 <render_game+0x454>
    61a0:	0000f7b7          	lui	a5,0xf
    61a4:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    61a8:	00200713          	li	a4,2
    61ac:	00098593          	mv	a1,s3
    61b0:	00040513          	mv	a0,s0
    61b4:	329020ef          	jal	8cdc <gfx_fill_rect>
    61b8:	02012783          	lw	a5,32(sp)
    61bc:	001a0a13          	addi	s4,s4,1
    61c0:	004a8a93          	addi	s5,s5,4
    61c4:	e0fa16e3          	bne	s4,a5,5fd0 <render_game+0x27c>
    61c8:	000d8893          	mv	a7,s11
    61cc:	03c12d83          	lw	s11,60(sp)
    61d0:	6a0c06e3          	beqz	s8,707c <render_game+0x1328>
    61d4:	000024b7          	lui	s1,0x2
    61d8:	00003ab7          	lui	s5,0x3
    61dc:	1b848493          	addi	s1,s1,440 # 21b8 <main+0xda8>
    61e0:	5b8a8a93          	addi	s5,s5,1464 # 35b8 <render_init+0x250>
    61e4:	00008cb7          	lui	s9,0x8
    61e8:	0000fc37          	lui	s8,0xf
    61ec:	0000fbb7          	lui	s7,0xf
    61f0:	009884b3          	add	s1,a7,s1
    61f4:	01588ab3          	add	s5,a7,s5
    61f8:	d0000b13          	li	s6,-768
    61fc:	3a8c8c93          	addi	s9,s9,936 # 83a8 <render_game+0x2654>
    6200:	5ebc0c13          	addi	s8,s8,1515 # f5eb <overlay.isra.0+0x307>
    6204:	77ab8b93          	addi	s7,s7,1914 # f77a <demo_init+0xf6>
    6208:	00088a13          	mv	s4,a7
    620c:	00c0006f          	j	6218 <render_game+0x4c4>
    6210:	02048493          	addi	s1,s1,32
    6214:	0b548463          	beq	s1,s5,62bc <render_game+0x568>
    6218:	0004a783          	lw	a5,0(s1)
    621c:	fe078ae3          	beqz	a5,6210 <render_game+0x4bc>
    6220:	0104a603          	lw	a2,16(s1)
    6224:	00c4a583          	lw	a1,12(s1)
    6228:	0044a703          	lw	a4,4(s1)
    622c:	03664633          	div	a2,a2,s6
    6230:	0084a783          	lw	a5,8(s1)
    6234:	41f75993          	srai	s3,a4,0x1f
    6238:	0ff9f993          	zext.b	s3,s3
    623c:	41f7dd13          	srai	s10,a5,0x1f
    6240:	0ffd7d13          	zext.b	s10,s10
    6244:	00e989b3          	add	s3,s3,a4
    6248:	00fd0d33          	add	s10,s10,a5
    624c:	4089d993          	srai	s3,s3,0x8
    6250:	408d5d13          	srai	s10,s10,0x8
    6254:	000c8793          	mv	a5,s9
    6258:	00300713          	li	a4,3
    625c:	00500693          	li	a3,5
    6260:	00040513          	mv	a0,s0
    6264:	02048493          	addi	s1,s1,32
    6268:	0365c5b3          	div	a1,a1,s6
    626c:	01a60633          	add	a2,a2,s10
    6270:	fff60613          	addi	a2,a2,-1
    6274:	013585b3          	add	a1,a1,s3
    6278:	ffe58593          	addi	a1,a1,-2
    627c:	261020ef          	jal	8cdc <gfx_fill_rect>
    6280:	000c0793          	mv	a5,s8
    6284:	00500713          	li	a4,5
    6288:	00700693          	li	a3,7
    628c:	ffed0613          	addi	a2,s10,-2
    6290:	ffd98593          	addi	a1,s3,-3
    6294:	00040513          	mv	a0,s0
    6298:	245020ef          	jal	8cdc <gfx_fill_rect>
    629c:	000b8793          	mv	a5,s7
    62a0:	00300713          	li	a4,3
    62a4:	00300693          	li	a3,3
    62a8:	fffd0613          	addi	a2,s10,-1
    62ac:	fff98593          	addi	a1,s3,-1
    62b0:	00040513          	mv	a0,s0
    62b4:	229020ef          	jal	8cdc <gfx_fill_rect>
    62b8:	f75490e3          	bne	s1,s5,6218 <render_game+0x4c4>
    62bc:	000a0513          	mv	a0,s4
    62c0:	01412e23          	sw	s4,28(sp)
    62c4:	670060ef          	jal	c934 <game_hostile_slots>
    62c8:	01c12883          	lw	a7,28(sp)
    62cc:	00050b13          	mv	s6,a0
    62d0:	500514e3          	bnez	a0,6fd8 <render_game+0x1284>
    62d4:	000244b7          	lui	s1,0x24
    62d8:	00025ab7          	lui	s5,0x25
    62dc:	1d448493          	addi	s1,s1,468 # 241d4 <background_pixels+0xc314>
    62e0:	1d4a8a93          	addi	s5,s5,468 # 251d4 <background_pixels+0xd314>
    62e4:	0000fb37          	lui	s6,0xf
    62e8:	0000bbb7          	lui	s7,0xb
    62ec:	0000fc37          	lui	s8,0xf
    62f0:	009884b3          	add	s1,a7,s1
    62f4:	01588ab3          	add	s5,a7,s5
    62f8:	00100993          	li	s3,1
    62fc:	b8eb0b13          	addi	s6,s6,-1138 # eb8e <game_tick+0x1d02>
    6300:	00200a13          	li	s4,2
    6304:	70db8b93          	addi	s7,s7,1805 # b70d <check_level+0x61>
    6308:	5ebc0c13          	addi	s8,s8,1515 # f5eb <overlay.isra.0+0x307>
    630c:	00088c93          	mv	s9,a7
    6310:	0004a783          	lw	a5,0(s1)
    6314:	04078a63          	beqz	a5,6368 <render_game+0x614>
    6318:	0184a703          	lw	a4,24(s1)
    631c:	000b0793          	mv	a5,s6
    6320:	01370863          	beq	a4,s3,6330 <render_game+0x5dc>
    6324:	000b8793          	mv	a5,s7
    6328:	01470463          	beq	a4,s4,6330 <render_game+0x5dc>
    632c:	000c0793          	mv	a5,s8
    6330:	0084a703          	lw	a4,8(s1)
    6334:	00c4a683          	lw	a3,12(s1)
    6338:	00040513          	mv	a0,s0
    633c:	41f75593          	srai	a1,a4,0x1f
    6340:	0ff5f593          	zext.b	a1,a1
    6344:	41f6d613          	srai	a2,a3,0x1f
    6348:	00e585b3          	add	a1,a1,a4
    634c:	01c4a703          	lw	a4,28(s1)
    6350:	0ff67613          	zext.b	a2,a2
    6354:	00d60633          	add	a2,a2,a3
    6358:	40865613          	srai	a2,a2,0x8
    635c:	00070693          	mv	a3,a4
    6360:	4085d593          	srai	a1,a1,0x8
    6364:	179020ef          	jal	8cdc <gfx_fill_rect>
    6368:	02048493          	addi	s1,s1,32
    636c:	fb5492e3          	bne	s1,s5,6310 <render_game+0x5bc>
    6370:	06cca783          	lw	a5,108(s9)
    6374:	000c8893          	mv	a7,s9
    6378:	420798e3          	bnez	a5,6fa8 <render_game+0x1254>
    637c:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    6380:	00078863          	beqz	a5,6390 <render_game+0x63c>
    6384:	0008a703          	lw	a4,0(a7)
    6388:	00200793          	li	a5,2
    638c:	48f71a63          	bne	a4,a5,6820 <render_game+0xacc>
    6390:	03812803          	lw	a6,56(sp)
    6394:	0088a983          	lw	s3,8(a7)
    6398:	03112223          	sw	a7,36(sp)
    639c:	00082b03          	lw	s6,0(a6)
    63a0:	03882883          	lw	a7,56(a6)
    63a4:	00482a03          	lw	s4,4(a6)
    63a8:	01082a83          	lw	s5,16(a6)
    63ac:	03c82803          	lw	a6,60(a6)
    63b0:	000017b7          	lui	a5,0x1
    63b4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    63b8:	03900713          	li	a4,57
    63bc:	28000693          	li	a3,640
    63c0:	00000613          	li	a2,0
    63c4:	00000593          	li	a1,0
    63c8:	00040513          	mv	a0,s0
    63cc:	03112023          	sw	a7,32(sp)
    63d0:	01012e23          	sw	a6,28(sp)
    63d4:	109020ef          	jal	8cdc <gfx_fill_rect>
    63d8:	0000f7b7          	lui	a5,0xf
    63dc:	000116b7          	lui	a3,0x11
    63e0:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    63e4:	00200713          	li	a4,2
    63e8:	7b068693          	addi	a3,a3,1968 # 117b0 <_data+0x74>
    63ec:	00a00613          	li	a2,10
    63f0:	01200593          	li	a1,18
    63f4:	00040513          	mv	a0,s0
    63f8:	230030ef          	jal	9628 <gfx_text>
    63fc:	02412883          	lw	a7,36(sp)
    6400:	03c00493          	li	s1,60
    6404:	0299c4b3          	div	s1,s3,s1
    6408:	0888a783          	lw	a5,136(a7)
    640c:	36078ce3          	beqz	a5,6f84 <render_game+0x1230>
    6410:	08c8a783          	lw	a5,140(a7)
    6414:	00100713          	li	a4,1
    6418:	42e78c63          	beq	a5,a4,6850 <render_game+0xafc>
    641c:	00200713          	li	a4,2
    6420:	00e79463          	bne	a5,a4,6428 <render_game+0x6d4>
    6424:	2050106f          	j	7e28 <render_game+0x20d4>
    6428:	00011c37          	lui	s8,0x11
    642c:	744c0c13          	addi	s8,s8,1860 # 11744 <_data+0x8>
    6430:	00088513          	mv	a0,a7
    6434:	0648ac83          	lw	s9,100(a7)
    6438:	03112223          	sw	a7,36(sp)
    643c:	5fc060ef          	jal	ca38 <game_hostile_count>
    6440:	02412883          	lw	a7,36(sp)
    6444:	00050b93          	mv	s7,a0
    6448:	00088513          	mv	a0,a7
    644c:	4e8060ef          	jal	c934 <game_hostile_slots>
    6450:	00011637          	lui	a2,0x11
    6454:	00050813          	mv	a6,a0
    6458:	000b8793          	mv	a5,s7
    645c:	000c8713          	mv	a4,s9
    6460:	000c0693          	mv	a3,s8
    6464:	7bc60613          	addi	a2,a2,1980 # 117bc <_data+0x80>
    6468:	08000593          	li	a1,128
    646c:	00090513          	mv	a0,s2
    6470:	7140a0ef          	jal	10b84 <mini_snprintf>
    6474:	02412883          	lw	a7,36(sp)
    6478:	0888a703          	lw	a4,136(a7)
    647c:	000097b7          	lui	a5,0x9
    6480:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6484:	00070663          	beqz	a4,6490 <render_game+0x73c>
    6488:	0000f7b7          	lui	a5,0xf
    648c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6490:	00100713          	li	a4,1
    6494:	00090693          	mv	a3,s2
    6498:	02000613          	li	a2,32
    649c:	01300593          	li	a1,19
    64a0:	00040513          	mv	a0,s0
    64a4:	03112223          	sw	a7,36(sp)
    64a8:	180030ef          	jal	9628 <gfx_text>
    64ac:	000097b7          	lui	a5,0x9
    64b0:	000116b7          	lui	a3,0x11
    64b4:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    64b8:	00100713          	li	a4,1
    64bc:	7e068693          	addi	a3,a3,2016 # 117e0 <_data+0xa4>
    64c0:	00a00613          	li	a2,10
    64c4:	0ca00593          	li	a1,202
    64c8:	00040513          	mv	a0,s0
    64cc:	15c030ef          	jal	9628 <gfx_text>
    64d0:	02412883          	lw	a7,36(sp)
    64d4:	00011637          	lui	a2,0x11
    64d8:	7c460613          	addi	a2,a2,1988 # 117c4 <_data+0x88>
    64dc:	01c8a703          	lw	a4,28(a7)
    64e0:	0188a683          	lw	a3,24(a7)
    64e4:	08000593          	li	a1,128
    64e8:	00090513          	mv	a0,s2
    64ec:	6980a0ef          	jal	10b84 <mini_snprintf>
    64f0:	0000f7b7          	lui	a5,0xf
    64f4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    64f8:	00100713          	li	a4,1
    64fc:	00090693          	mv	a3,s2
    6500:	00a00613          	li	a2,10
    6504:	11500593          	li	a1,277
    6508:	00040513          	mv	a0,s0
    650c:	11c030ef          	jal	9628 <gfx_text>
    6510:	000037b7          	lui	a5,0x3
    6514:	20778793          	addi	a5,a5,519 # 3207 <vnoise.constprop.0+0x11f>
    6518:	00b00713          	li	a4,11
    651c:	07b00693          	li	a3,123
    6520:	01800613          	li	a2,24
    6524:	0ca00593          	li	a1,202
    6528:	00040513          	mv	a0,s0
    652c:	7b0020ef          	jal	8cdc <gfx_fill_rect>
    6530:	02412883          	lw	a7,36(sp)
    6534:	0000b7b7          	lui	a5,0xb
    6538:	01d00613          	li	a2,29
    653c:	0188a703          	lw	a4,24(a7)
    6540:	01c8a583          	lw	a1,28(a7)
    6544:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    6548:	00471693          	slli	a3,a4,0x4
    654c:	40e686b3          	sub	a3,a3,a4
    6550:	00369693          	slli	a3,a3,0x3
    6554:	00e686b3          	add	a3,a3,a4
    6558:	02b6c6b3          	div	a3,a3,a1
    655c:	00e64663          	blt	a2,a4,6568 <render_game+0x814>
    6560:	0000f7b7          	lui	a5,0xf
    6564:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    6568:	00900713          	li	a4,9
    656c:	01900613          	li	a2,25
    6570:	0cb00593          	li	a1,203
    6574:	00040513          	mv	a0,s0
    6578:	03112223          	sw	a7,36(sp)
    657c:	760020ef          	jal	8cdc <gfx_fill_rect>
    6580:	02412883          	lw	a7,36(sp)
    6584:	00011637          	lui	a2,0x11
    6588:	7e860613          	addi	a2,a2,2024 # 117e8 <_data+0xac>
    658c:	0508a703          	lw	a4,80(a7)
    6590:	04c8a683          	lw	a3,76(a7)
    6594:	08000593          	li	a1,128
    6598:	00090513          	mv	a0,s2
    659c:	5e80a0ef          	jal	10b84 <mini_snprintf>
    65a0:	000077b7          	lui	a5,0x7
    65a4:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    65a8:	00100713          	li	a4,1
    65ac:	00090693          	mv	a3,s2
    65b0:	02800613          	li	a2,40
    65b4:	0ca00593          	li	a1,202
    65b8:	00040513          	mv	a0,s0
    65bc:	06c030ef          	jal	9628 <gfx_text>
    65c0:	02412883          	lw	a7,36(sp)
    65c4:	03c00c13          	li	s8,60
    65c8:	0888a783          	lw	a5,136(a7)
    65cc:	0384ebb3          	rem	s7,s1,s8
    65d0:	28078663          	beqz	a5,685c <render_game+0xb08>
    65d4:	06400513          	li	a0,100
    65d8:	00004637          	lui	a2,0x4
    65dc:	11b60613          	addi	a2,a2,283 # 411b <__stack_size+0x11b>
    65e0:	00000693          	li	a3,0
    65e4:	02aa95b3          	mulh	a1,s5,a0
    65e8:	02aa8533          	mul	a0,s5,a0
    65ec:	7a40a0ef          	jal	10d90 <__divdi3>
    65f0:	000097b7          	lui	a5,0x9
    65f4:	000116b7          	lui	a3,0x11
    65f8:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    65fc:	00100713          	li	a4,1
    6600:	7f468693          	addi	a3,a3,2036 # 117f4 <_data+0xb8>
    6604:	00a00613          	li	a2,10
    6608:	15b00593          	li	a1,347
    660c:	00050993          	mv	s3,a0
    6610:	00040513          	mv	a0,s0
    6614:	014030ef          	jal	9628 <gfx_text>
    6618:	3e800693          	li	a3,1000
    661c:	02dae733          	rem	a4,s5,a3
    6620:	00a00793          	li	a5,10
    6624:	00011637          	lui	a2,0x11
    6628:	7fc60613          	addi	a2,a2,2044 # 117fc <_data+0xc0>
    662c:	08000593          	li	a1,128
    6630:	00090513          	mv	a0,s2
    6634:	02f74733          	div	a4,a4,a5
    6638:	02dac6b3          	div	a3,s5,a3
    663c:	5480a0ef          	jal	10b84 <mini_snprintf>
    6640:	000027b7          	lui	a5,0x2
    6644:	70f78793          	addi	a5,a5,1807 # 270f <main+0x12ff>
    6648:	02412883          	lw	a7,36(sp)
    664c:	0957cee3          	blt	a5,s5,6ee8 <render_game+0x1194>
    6650:	0000f7b7          	lui	a5,0xf
    6654:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    6658:	00200713          	li	a4,2
    665c:	00090693          	mv	a3,s2
    6660:	01800613          	li	a2,24
    6664:	15b00593          	li	a1,347
    6668:	00040513          	mv	a0,s0
    666c:	03112223          	sw	a7,36(sp)
    6670:	7b9020ef          	jal	9628 <gfx_text>
    6674:	000097b7          	lui	a5,0x9
    6678:	000126b7          	lui	a3,0x12
    667c:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6680:	00100713          	li	a4,1
    6684:	80468693          	addi	a3,a3,-2044 # 11804 <_data+0xc8>
    6688:	00a00613          	li	a2,10
    668c:	1b300593          	li	a1,435
    6690:	00040513          	mv	a0,s0
    6694:	795020ef          	jal	9628 <gfx_text>
    6698:	00012637          	lui	a2,0x12
    669c:	00098693          	mv	a3,s3
    66a0:	80c60613          	addi	a2,a2,-2036 # 1180c <_data+0xd0>
    66a4:	08000593          	li	a1,128
    66a8:	00090513          	mv	a0,s2
    66ac:	4d80a0ef          	jal	10b84 <mini_snprintf>
    66b0:	02412883          	lw	a7,36(sp)
    66b4:	0000b7b7          	lui	a5,0xb
    66b8:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    66bc:	03112223          	sw	a7,36(sp)
    66c0:	2580006f          	j	6918 <render_game+0xbc4>
    66c4:	00000613          	li	a2,0
    66c8:	00261613          	slli	a2,a2,0x2
    66cc:	3c060793          	addi	a5,a2,960
    66d0:	04010713          	addi	a4,sp,64
    66d4:	00e78633          	add	a2,a5,a4
    66d8:	d1d62023          	sw	t4,-768(a2)
    66dc:	00130313          	addi	t1,t1,1
    66e0:	004e0e13          	addi	t3,t3,4
    66e4:	fb031e63          	bne	t1,a6,5ea0 <render_game+0x14c>
    66e8:	819ff06f          	j	5f00 <render_game+0x1ac>
    66ec:	037c87b3          	mul	a5,s9,s7
    66f0:	00e00713          	li	a4,14
    66f4:	00fd87b3          	add	a5,s11,a5
    66f8:	0dc7a783          	lw	a5,220(a5)
    66fc:	98f74ae3          	blt	a4,a5,6090 <render_game+0x33c>
    6700:	00007837          	lui	a6,0x7
    6704:	69880813          	addi	a6,a6,1688 # 7698 <render_game+0x1944>
    6708:	00200793          	li	a5,2
    670c:	01c00713          	li	a4,28
    6710:	01c00693          	li	a3,28
    6714:	fee48613          	addi	a2,s1,-18
    6718:	ff298593          	addi	a1,s3,-14
    671c:	00040513          	mv	a0,s0
    6720:	03e12423          	sw	t5,40(sp)
    6724:	03d12223          	sw	t4,36(sp)
    6728:	5fd020ef          	jal	9524 <gfx_border>
    672c:	0000c7b7          	lui	a5,0xc
    6730:	eac78793          	addi	a5,a5,-340 # beac <hostile_shot+0x238>
    6734:	00600713          	li	a4,6
    6738:	00400693          	li	a3,4
    673c:	fec48613          	addi	a2,s1,-20
    6740:	ffe98593          	addi	a1,s3,-2
    6744:	00040513          	mv	a0,s0
    6748:	594020ef          	jal	8cdc <gfx_fill_rect>
    674c:	02812f03          	lw	t5,40(sp)
    6750:	02412e83          	lw	t4,36(sp)
    6754:	93dff06f          	j	6090 <render_game+0x33c>
    6758:	0dcb2683          	lw	a3,220(s6)
    675c:	01800713          	li	a4,24
    6760:	92d748e3          	blt	a4,a3,6090 <render_game+0x33c>
    6764:	40d705b3          	sub	a1,a4,a3
    6768:	02f5c5b3          	div	a1,a1,a5
    676c:	0000f837          	lui	a6,0xf
    6770:	b8e80813          	addi	a6,a6,-1138 # eb8e <game_tick+0x1d02>
    6774:	00200793          	li	a5,2
    6778:	00040513          	mv	a0,s0
    677c:	01c58593          	addi	a1,a1,28
    6780:	00159713          	slli	a4,a1,0x1
    6784:	00070693          	mv	a3,a4
    6788:	40b48633          	sub	a2,s1,a1
    678c:	40b985b3          	sub	a1,s3,a1
    6790:	595020ef          	jal	9524 <gfx_border>
    6794:	0000f7b7          	lui	a5,0xf
    6798:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    679c:	00800713          	li	a4,8
    67a0:	00800693          	li	a3,8
    67a4:	ff848613          	addi	a2,s1,-8
    67a8:	ffc98593          	addi	a1,s3,-4
    67ac:	00040513          	mv	a0,s0
    67b0:	52c020ef          	jal	8cdc <gfx_fill_rect>
    67b4:	0ccb2783          	lw	a5,204(s6)
    67b8:	02812f03          	lw	t5,40(sp)
    67bc:	02412e83          	lw	t4,36(sp)
    67c0:	8c9ff06f          	j	6088 <render_game+0x334>
    67c4:	000d8593          	mv	a1,s11
    67c8:	00040513          	mv	a0,s0
    67cc:	e4cfc0ef          	jal	2e18 <draw_player>
    67d0:	0c04a703          	lw	a4,192(s1)
    67d4:	00100c13          	li	s8,1
    67d8:	815ff06f          	j	5fec <render_game+0x298>
    67dc:	0000f7b7          	lui	a5,0xf
    67e0:	ff348613          	addi	a2,s1,-13
    67e4:	ffc98593          	addi	a1,s3,-4
    67e8:	00040513          	mv	a0,s0
    67ec:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    67f0:	00200713          	li	a4,2
    67f4:	00800693          	li	a3,8
    67f8:	4e4020ef          	jal	8cdc <gfx_fill_rect>
    67fc:	0000f7b7          	lui	a5,0xf
    6800:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    6804:	00800713          	li	a4,8
    6808:	00200693          	li	a3,2
    680c:	ff048613          	addi	a2,s1,-16
    6810:	fff98593          	addi	a1,s3,-1
    6814:	00040513          	mv	a0,s0
    6818:	4c4020ef          	jal	8cdc <gfx_fill_rect>
    681c:	8ddff06f          	j	60f8 <render_game+0x3a4>
    6820:	000017b7          	lui	a5,0x1
    6824:	09600813          	li	a6,150
    6828:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    682c:	1e000713          	li	a4,480
    6830:	28000693          	li	a3,640
    6834:	00000613          	li	a2,0
    6838:	00000593          	li	a1,0
    683c:	00040513          	mv	a0,s0
    6840:	01112e23          	sw	a7,28(sp)
    6844:	670020ef          	jal	8eb4 <gfx_fill_alpha>
    6848:	01c12883          	lw	a7,28(sp)
    684c:	b45ff06f          	j	6390 <render_game+0x63c>
    6850:	00011c37          	lui	s8,0x11
    6854:	73cc0c13          	addi	s8,s8,1852 # 1173c <_data>
    6858:	bd9ff06f          	j	6430 <render_game+0x6dc>
    685c:	000097b7          	lui	a5,0x9
    6860:	000126b7          	lui	a3,0x12
    6864:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6868:	00100713          	li	a4,1
    686c:	81468693          	addi	a3,a3,-2028 # 11814 <_data+0xd8>
    6870:	00a00613          	li	a2,10
    6874:	15b00593          	li	a1,347
    6878:	00040513          	mv	a0,s0
    687c:	03112223          	sw	a7,36(sp)
    6880:	5a9020ef          	jal	9628 <gfx_text>
    6884:	0384c6b3          	div	a3,s1,s8
    6888:	00012637          	lui	a2,0x12
    688c:	000b8713          	mv	a4,s7
    6890:	82060613          	addi	a2,a2,-2016 # 11820 <_data+0xe4>
    6894:	08000593          	li	a1,128
    6898:	00090513          	mv	a0,s2
    689c:	2e80a0ef          	jal	10b84 <mini_snprintf>
    68a0:	000587b7          	lui	a5,0x58
    68a4:	e3f78793          	addi	a5,a5,-449 # 57e3f <background_pixels+0x3ff7f>
    68a8:	0137a733          	slt	a4,a5,s3
    68ac:	00173713          	seqz	a4,a4
    68b0:	0000f7b7          	lui	a5,0xf
    68b4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    68b8:	00170713          	addi	a4,a4,1
    68bc:	00090693          	mv	a3,s2
    68c0:	01800613          	li	a2,24
    68c4:	15b00593          	li	a1,347
    68c8:	00040513          	mv	a0,s0
    68cc:	55d020ef          	jal	9628 <gfx_text>
    68d0:	000097b7          	lui	a5,0x9
    68d4:	000126b7          	lui	a3,0x12
    68d8:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    68dc:	00100713          	li	a4,1
    68e0:	82c68693          	addi	a3,a3,-2004 # 1182c <_data+0xf0>
    68e4:	00a00613          	li	a2,10
    68e8:	1b300593          	li	a1,435
    68ec:	00040513          	mv	a0,s0
    68f0:	539020ef          	jal	9628 <gfx_text>
    68f4:	02412883          	lw	a7,36(sp)
    68f8:	00012637          	lui	a2,0x12
    68fc:	83460613          	addi	a2,a2,-1996 # 11834 <_data+0xf8>
    6900:	0588a683          	lw	a3,88(a7)
    6904:	08000593          	li	a1,128
    6908:	00090513          	mv	a0,s2
    690c:	2780a0ef          	jal	10b84 <mini_snprintf>
    6910:	0000f7b7          	lui	a5,0xf
    6914:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6918:	00200713          	li	a4,2
    691c:	00090693          	mv	a3,s2
    6920:	01800613          	li	a2,24
    6924:	1b300593          	li	a1,435
    6928:	00040513          	mv	a0,s0
    692c:	4fd020ef          	jal	9628 <gfx_text>
    6930:	02412883          	lw	a7,36(sp)
    6934:	00012637          	lui	a2,0x12
    6938:	83c60613          	addi	a2,a2,-1988 # 1183c <_data+0x100>
    693c:	0548a683          	lw	a3,84(a7)
    6940:	08000593          	li	a1,128
    6944:	00090513          	mv	a0,s2
    6948:	03112223          	sw	a7,36(sp)
    694c:	2380a0ef          	jal	10b84 <mini_snprintf>
    6950:	000077b7          	lui	a5,0x7
    6954:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    6958:	00100713          	li	a4,1
    695c:	00090693          	mv	a3,s2
    6960:	00b00613          	li	a2,11
    6964:	20f00593          	li	a1,527
    6968:	00040513          	mv	a0,s0
    696c:	4bd020ef          	jal	9628 <gfx_text>
    6970:	01d00793          	li	a5,29
    6974:	02412883          	lw	a7,36(sp)
    6978:	4f77ca63          	blt	a5,s7,6e6c <render_game+0x1118>
    697c:	000116b7          	lui	a3,0x11
    6980:	74c68693          	addi	a3,a3,1868 # 1174c <_data+0x10>
    6984:	01e00793          	li	a5,30
    6988:	02f4e733          	rem	a4,s1,a5
    698c:	00012637          	lui	a2,0x12
    6990:	84860613          	addi	a2,a2,-1976 # 11848 <_data+0x10c>
    6994:	08000593          	li	a1,128
    6998:	00090513          	mv	a0,s2
    699c:	03112223          	sw	a7,36(sp)
    69a0:	40e78733          	sub	a4,a5,a4
    69a4:	1e00a0ef          	jal	10b84 <mini_snprintf>
    69a8:	000097b7          	lui	a5,0x9
    69ac:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    69b0:	00100713          	li	a4,1
    69b4:	00090693          	mv	a3,s2
    69b8:	01c00613          	li	a2,28
    69bc:	20f00593          	li	a1,527
    69c0:	00040513          	mv	a0,s0
    69c4:	465020ef          	jal	9628 <gfx_text>
    69c8:	000037b7          	lui	a5,0x3
    69cc:	20878793          	addi	a5,a5,520 # 3208 <vnoise.constprop.0+0x120>
    69d0:	00300713          	li	a4,3
    69d4:	25c00693          	li	a3,604
    69d8:	03100613          	li	a2,49
    69dc:	01200593          	li	a1,18
    69e0:	00040513          	mv	a0,s0
    69e4:	2f8020ef          	jal	8cdc <gfx_fill_rect>
    69e8:	02412883          	lw	a7,36(sp)
    69ec:	25c00693          	li	a3,604
    69f0:	04c8a703          	lw	a4,76(a7)
    69f4:	0508a783          	lw	a5,80(a7)
    69f8:	00f75663          	bge	a4,a5,6a04 <render_game+0xcb0>
    69fc:	02d706b3          	mul	a3,a4,a3
    6a00:	02f6c6b3          	div	a3,a3,a5
    6a04:	000077b7          	lui	a5,0x7
    6a08:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    6a0c:	00300713          	li	a4,3
    6a10:	03100613          	li	a2,49
    6a14:	01200593          	li	a1,18
    6a18:	00040513          	mv	a0,s0
    6a1c:	03112223          	sw	a7,36(sp)
    6a20:	2bc020ef          	jal	8cdc <gfx_fill_rect>
    6a24:	000017b7          	lui	a5,0x1
    6a28:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    6a2c:	02000713          	li	a4,32
    6a30:	28000693          	li	a3,640
    6a34:	1c000613          	li	a2,448
    6a38:	00000593          	li	a1,0
    6a3c:	00040513          	mv	a0,s0
    6a40:	29c020ef          	jal	8cdc <gfx_fill_rect>
    6a44:	000097b7          	lui	a5,0x9
    6a48:	000126b7          	lui	a3,0x12
    6a4c:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6a50:	00100713          	li	a4,1
    6a54:	85468693          	addi	a3,a3,-1964 # 11854 <_data+0x118>
    6a58:	1c900613          	li	a2,457
    6a5c:	01200593          	li	a1,18
    6a60:	00040513          	mv	a0,s0
    6a64:	3c5020ef          	jal	9628 <gfx_text>
    6a68:	01c12783          	lw	a5,28(sp)
    6a6c:	02412883          	lw	a7,36(sp)
    6a70:	62078263          	beqz	a5,7094 <render_game+0x1340>
    6a74:	02012783          	lw	a5,32(sp)
    6a78:	4e078c63          	beqz	a5,6f70 <render_game+0x121c>
    6a7c:	0000b7b7          	lui	a5,0xb
    6a80:	000116b7          	lui	a3,0x11
    6a84:	75c68693          	addi	a3,a3,1884 # 1175c <_data+0x20>
    6a88:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    6a8c:	03112223          	sw	a7,36(sp)
    6a90:	00100713          	li	a4,1
    6a94:	1c900613          	li	a2,457
    6a98:	0f800593          	li	a1,248
    6a9c:	00040513          	mv	a0,s0
    6aa0:	389020ef          	jal	9628 <gfx_text>
    6aa4:	02412883          	lw	a7,36(sp)
    6aa8:	00012637          	lui	a2,0x12
    6aac:	000b0693          	mv	a3,s6
    6ab0:	88060613          	addi	a2,a2,-1920 # 11880 <_data+0x144>
    6ab4:	02000593          	li	a1,32
    6ab8:	05010513          	addi	a0,sp,80
    6abc:	03112223          	sw	a7,36(sp)
    6ac0:	0c40a0ef          	jal	10b84 <mini_snprintf>
    6ac4:	02412883          	lw	a7,36(sp)
    6ac8:	3a0a4863          	bltz	s4,6e78 <render_game+0x1124>
    6acc:	00a00693          	li	a3,10
    6ad0:	02da6733          	rem	a4,s4,a3
    6ad4:	08010493          	addi	s1,sp,128
    6ad8:	00012637          	lui	a2,0x12
    6adc:	88860613          	addi	a2,a2,-1912 # 11888 <_data+0x14c>
    6ae0:	02000593          	li	a1,32
    6ae4:	00048513          	mv	a0,s1
    6ae8:	27000993          	li	s3,624
    6aec:	02da46b3          	div	a3,s4,a3
    6af0:	0940a0ef          	jal	10b84 <mini_snprintf>
    6af4:	00200593          	li	a1,2
    6af8:	05010513          	addi	a0,sp,80
    6afc:	5f5020ef          	jal	98f0 <gfx_text_width>
    6b00:	00050b93          	mv	s7,a0
    6b04:	00200593          	li	a1,2
    6b08:	00048513          	mv	a0,s1
    6b0c:	5e5020ef          	jal	98f0 <gfx_text_width>
    6b10:	0000b7b7          	lui	a5,0xb
    6b14:	03100713          	li	a4,49
    6b18:	02412883          	lw	a7,36(sp)
    6b1c:	00050a93          	mv	s5,a0
    6b20:	417989b3          	sub	s3,s3,s7
    6b24:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    6b28:	01674c63          	blt	a4,s6,6b40 <render_game+0xdec>
    6b2c:	0000f7b7          	lui	a5,0xf
    6b30:	01d00713          	li	a4,29
    6b34:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    6b38:	01675463          	bge	a4,s6,6b40 <render_game+0xdec>
    6b3c:	1980106f          	j	7cd4 <render_game+0x1f80>
    6b40:	00200713          	li	a4,2
    6b44:	00098593          	mv	a1,s3
    6b48:	05010693          	addi	a3,sp,80
    6b4c:	1c400613          	li	a2,452
    6b50:	00040513          	mv	a0,s0
    6b54:	03112223          	sw	a7,36(sp)
    6b58:	2d1020ef          	jal	9628 <gfx_text>
    6b5c:	00ca8593          	addi	a1,s5,12
    6b60:	0000f7b7          	lui	a5,0xf
    6b64:	0a000713          	li	a4,160
    6b68:	02412883          	lw	a7,36(sp)
    6b6c:	40b985b3          	sub	a1,s3,a1
    6b70:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6b74:	01475663          	bge	a4,s4,6b80 <render_game+0xe2c>
    6b78:	0000f7b7          	lui	a5,0xf
    6b7c:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    6b80:	00200713          	li	a4,2
    6b84:	00048693          	mv	a3,s1
    6b88:	1c400613          	li	a2,452
    6b8c:	00040513          	mv	a0,s0
    6b90:	03112223          	sw	a7,36(sp)
    6b94:	295020ef          	jal	9628 <gfx_text>
    6b98:	02412883          	lw	a7,36(sp)
    6b9c:	0688a783          	lw	a5,104(a7)
    6ba0:	0008a583          	lw	a1,0(a7)
    6ba4:	00078863          	beqz	a5,6bb4 <render_game+0xe60>
    6ba8:	00200793          	li	a5,2
    6bac:	00f59463          	bne	a1,a5,6bb4 <render_game+0xe60>
    6bb0:	13c0106f          	j	7cec <render_game+0x1f98>
    6bb4:	00000993          	li	s3,0
    6bb8:	fff00a13          	li	s4,-1
    6bbc:	00000793          	li	a5,0
    6bc0:	00300613          	li	a2,3
    6bc4:	0c000693          	li	a3,192
    6bc8:	0100006f          	j	6bd8 <render_game+0xe84>
    6bcc:	00178793          	addi	a5,a5,1
    6bd0:	02cd8d93          	addi	s11,s11,44
    6bd4:	02d78463          	beq	a5,a3,6bfc <render_game+0xea8>
    6bd8:	000da703          	lw	a4,0(s11)
    6bdc:	fe0708e3          	beqz	a4,6bcc <render_game+0xe78>
    6be0:	014da703          	lw	a4,20(s11)
    6be4:	fec714e3          	bne	a4,a2,6bcc <render_game+0xe78>
    6be8:	040a4663          	bltz	s4,6c34 <render_game+0xee0>
    6bec:	00198993          	addi	s3,s3,1
    6bf0:	00178793          	addi	a5,a5,1
    6bf4:	02cd8d93          	addi	s11,s11,44
    6bf8:	fed790e3          	bne	a5,a3,6bd8 <render_game+0xe84>
    6bfc:	00058793          	mv	a5,a1
    6c00:	000a4a63          	bltz	s4,6c14 <render_game+0xec0>
    6c04:	ffe58593          	addi	a1,a1,-2
    6c08:	00100713          	li	a4,1
    6c0c:	00b76463          	bltu	a4,a1,6c14 <render_game+0xec0>
    6c10:	2240106f          	j	7e34 <render_game+0x20e0>
    6c14:	00500713          	li	a4,5
    6c18:	20f76663          	bltu	a4,a5,6e24 <render_game+0x10d0>
    6c1c:	00013737          	lui	a4,0x13
    6c20:	00279793          	slli	a5,a5,0x2
    6c24:	9c070713          	addi	a4,a4,-1600 # 129c0 <_data+0x1284>
    6c28:	00e787b3          	add	a5,a5,a4
    6c2c:	0007a783          	lw	a5,0(a5)
    6c30:	00078067          	jr	a5
    6c34:	00078a13          	mv	s4,a5
    6c38:	00198993          	addi	s3,s3,1
    6c3c:	fb5ff06f          	j	6bf0 <render_game+0xe9c>
    6c40:	000047b7          	lui	a5,0x4
    6c44:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    6c48:	14900713          	li	a4,329
    6c4c:	1a000693          	li	a3,416
    6c50:	05600613          	li	a2,86
    6c54:	07000593          	li	a1,112
    6c58:	00040513          	mv	a0,s0
    6c5c:	03112223          	sw	a7,36(sp)
    6c60:	000124b7          	lui	s1,0x12
    6c64:	da8fc0ef          	jal	320c <panel>
    6c68:	00300593          	li	a1,3
    6c6c:	95048513          	addi	a0,s1,-1712 # 11950 <_data+0x214>
    6c70:	481020ef          	jal	98f0 <gfx_text_width>
    6c74:	01f55793          	srli	a5,a0,0x1f
    6c78:	00a787b3          	add	a5,a5,a0
    6c7c:	4017d593          	srai	a1,a5,0x1
    6c80:	40b005b3          	neg	a1,a1
    6c84:	0000f7b7          	lui	a5,0xf
    6c88:	95048693          	addi	a3,s1,-1712
    6c8c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6c90:	00300713          	li	a4,3
    6c94:	07000613          	li	a2,112
    6c98:	14058593          	addi	a1,a1,320
    6c9c:	00040513          	mv	a0,s0
    6ca0:	189020ef          	jal	9628 <gfx_text>
    6ca4:	000124b7          	lui	s1,0x12
    6ca8:	00100593          	li	a1,1
    6cac:	95c48513          	addi	a0,s1,-1700 # 1195c <_data+0x220>
    6cb0:	441020ef          	jal	98f0 <gfx_text_width>
    6cb4:	01f55793          	srli	a5,a0,0x1f
    6cb8:	00a787b3          	add	a5,a5,a0
    6cbc:	4017d593          	srai	a1,a5,0x1
    6cc0:	40b005b3          	neg	a1,a1
    6cc4:	0000f7b7          	lui	a5,0xf
    6cc8:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    6ccc:	00100713          	li	a4,1
    6cd0:	95c48693          	addi	a3,s1,-1700
    6cd4:	0a900613          	li	a2,169
    6cd8:	14058593          	addi	a1,a1,320
    6cdc:	00040513          	mv	a0,s0
    6ce0:	149020ef          	jal	9628 <gfx_text>
    6ce4:	02412883          	lw	a7,36(sp)
    6ce8:	0000b7b7          	lui	a5,0xb
    6cec:	00001b37          	lui	s6,0x1
    6cf0:	0000fab7          	lui	s5,0xf
    6cf4:	00013737          	lui	a4,0x13
    6cf8:	a1c70b93          	addi	s7,a4,-1508 # 12a1c <labels.0>
    6cfc:	00000993          	li	s3,0
    6d00:	0ce00493          	li	s1,206
    6d04:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    6d08:	8c3b0b13          	addi	s6,s6,-1853 # 8c3 <CUSTOM2+0x868>
    6d0c:	5eba8a93          	addi	s5,s5,1515 # f5eb <overlay.isra.0+0x307>
    6d10:	00088c13          	mv	s8,a7
    6d14:	00048613          	mv	a2,s1
    6d18:	02800713          	li	a4,40
    6d1c:	13600693          	li	a3,310
    6d20:	0a500593          	li	a1,165
    6d24:	00040513          	mv	a0,s0
    6d28:	7b5010ef          	jal	8cdc <gfx_fill_rect>
    6d2c:	000baa03          	lw	s4,0(s7)
    6d30:	00100593          	li	a1,1
    6d34:	004b8b93          	addi	s7,s7,4
    6d38:	000a0513          	mv	a0,s4
    6d3c:	3b5020ef          	jal	98f0 <gfx_text_width>
    6d40:	01f55793          	srli	a5,a0,0x1f
    6d44:	00a787b3          	add	a5,a5,a0
    6d48:	4017d593          	srai	a1,a5,0x1
    6d4c:	40b005b3          	neg	a1,a1
    6d50:	000b0793          	mv	a5,s6
    6d54:	00e48613          	addi	a2,s1,14
    6d58:	00100713          	li	a4,1
    6d5c:	000a0693          	mv	a3,s4
    6d60:	14058593          	addi	a1,a1,320
    6d64:	00040513          	mv	a0,s0
    6d68:	0c1020ef          	jal	9628 <gfx_text>
    6d6c:	03448493          	addi	s1,s1,52
    6d70:	000a8793          	mv	a5,s5
    6d74:	00099463          	bnez	s3,6d7c <render_game+0x1028>
    6d78:	2440106f          	j	7fbc <render_game+0x2268>
    6d7c:	0000f7b7          	lui	a5,0xf
    6d80:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    6d84:	02800713          	li	a4,40
    6d88:	13600693          	li	a3,310
    6d8c:	13600613          	li	a2,310
    6d90:	0a500593          	li	a1,165
    6d94:	00040513          	mv	a0,s0
    6d98:	000124b7          	lui	s1,0x12
    6d9c:	03812223          	sw	s8,36(sp)
    6da0:	73d010ef          	jal	8cdc <gfx_fill_rect>
    6da4:	00100593          	li	a1,1
    6da8:	ba848513          	addi	a0,s1,-1112 # 11ba8 <_data+0x46c>
    6dac:	345020ef          	jal	98f0 <gfx_text_width>
    6db0:	01f55793          	srli	a5,a0,0x1f
    6db4:	00a787b3          	add	a5,a5,a0
    6db8:	4017d593          	srai	a1,a5,0x1
    6dbc:	40b005b3          	neg	a1,a1
    6dc0:	000017b7          	lui	a5,0x1
    6dc4:	ba848693          	addi	a3,s1,-1112
    6dc8:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    6dcc:	00100713          	li	a4,1
    6dd0:	14400613          	li	a2,324
    6dd4:	14058593          	addi	a1,a1,320
    6dd8:	00040513          	mv	a0,s0
    6ddc:	04d020ef          	jal	9628 <gfx_text>
    6de0:	000124b7          	lui	s1,0x12
    6de4:	00100593          	li	a1,1
    6de8:	bc848513          	addi	a0,s1,-1080 # 11bc8 <_data+0x48c>
    6dec:	305020ef          	jal	98f0 <gfx_text_width>
    6df0:	01f55793          	srli	a5,a0,0x1f
    6df4:	00a787b3          	add	a5,a5,a0
    6df8:	4017d593          	srai	a1,a5,0x1
    6dfc:	40b005b3          	neg	a1,a1
    6e00:	000097b7          	lui	a5,0x9
    6e04:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6e08:	00100713          	li	a4,1
    6e0c:	bc848693          	addi	a3,s1,-1080
    6e10:	18100613          	li	a2,385
    6e14:	14058593          	addi	a1,a1,320
    6e18:	00040513          	mv	a0,s0
    6e1c:	00d020ef          	jal	9628 <gfx_text>
    6e20:	02412883          	lw	a7,36(sp)
    6e24:	03812783          	lw	a5,56(sp)
    6e28:	0147a783          	lw	a5,20(a5)
    6e2c:	28079263          	bnez	a5,70b0 <render_game+0x135c>
    6e30:	43c12083          	lw	ra,1084(sp)
    6e34:	43812403          	lw	s0,1080(sp)
    6e38:	43412483          	lw	s1,1076(sp)
    6e3c:	43012903          	lw	s2,1072(sp)
    6e40:	42c12983          	lw	s3,1068(sp)
    6e44:	42812a03          	lw	s4,1064(sp)
    6e48:	42412a83          	lw	s5,1060(sp)
    6e4c:	42012b03          	lw	s6,1056(sp)
    6e50:	41c12b83          	lw	s7,1052(sp)
    6e54:	41812c03          	lw	s8,1048(sp)
    6e58:	41412c83          	lw	s9,1044(sp)
    6e5c:	41012d03          	lw	s10,1040(sp)
    6e60:	40c12d83          	lw	s11,1036(sp)
    6e64:	44010113          	addi	sp,sp,1088
    6e68:	00008067          	ret
    6e6c:	000116b7          	lui	a3,0x11
    6e70:	75468693          	addi	a3,a3,1876 # 11754 <_data+0x18>
    6e74:	b11ff06f          	j	6984 <render_game+0xc30>
    6e78:	08010493          	addi	s1,sp,128
    6e7c:	00012637          	lui	a2,0x12
    6e80:	00000713          	li	a4,0
    6e84:	00000693          	li	a3,0
    6e88:	88860613          	addi	a2,a2,-1912 # 11888 <_data+0x14c>
    6e8c:	02000593          	li	a1,32
    6e90:	00048513          	mv	a0,s1
    6e94:	03112223          	sw	a7,36(sp)
    6e98:	4ed090ef          	jal	10b84 <mini_snprintf>
    6e9c:	00200593          	li	a1,2
    6ea0:	05010513          	addi	a0,sp,80
    6ea4:	24d020ef          	jal	98f0 <gfx_text_width>
    6ea8:	00050b93          	mv	s7,a0
    6eac:	00200593          	li	a1,2
    6eb0:	00048513          	mv	a0,s1
    6eb4:	23d020ef          	jal	98f0 <gfx_text_width>
    6eb8:	27000993          	li	s3,624
    6ebc:	03100793          	li	a5,49
    6ec0:	02412883          	lw	a7,36(sp)
    6ec4:	00050a93          	mv	s5,a0
    6ec8:	417989b3          	sub	s3,s3,s7
    6ecc:	5167ca63          	blt	a5,s6,73e0 <render_game+0x168c>
    6ed0:	01d00793          	li	a5,29
    6ed4:	6167d6e3          	bge	a5,s6,7ce0 <render_game+0x1f8c>
    6ed8:	0000f7b7          	lui	a5,0xf
    6edc:	03112223          	sw	a7,36(sp)
    6ee0:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6ee4:	5080006f          	j	73ec <render_game+0x1698>
    6ee8:	0000f7b7          	lui	a5,0xf
    6eec:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    6ef0:	00100713          	li	a4,1
    6ef4:	00090693          	mv	a3,s2
    6ef8:	01800613          	li	a2,24
    6efc:	15b00593          	li	a1,347
    6f00:	00040513          	mv	a0,s0
    6f04:	724020ef          	jal	9628 <gfx_text>
    6f08:	000097b7          	lui	a5,0x9
    6f0c:	000126b7          	lui	a3,0x12
    6f10:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    6f14:	00100713          	li	a4,1
    6f18:	80468693          	addi	a3,a3,-2044 # 11804 <_data+0xc8>
    6f1c:	00a00613          	li	a2,10
    6f20:	1b300593          	li	a1,435
    6f24:	00040513          	mv	a0,s0
    6f28:	700020ef          	jal	9628 <gfx_text>
    6f2c:	00012637          	lui	a2,0x12
    6f30:	00098693          	mv	a3,s3
    6f34:	80c60613          	addi	a2,a2,-2036 # 1180c <_data+0xd0>
    6f38:	08000593          	li	a1,128
    6f3c:	00090513          	mv	a0,s2
    6f40:	445090ef          	jal	10b84 <mini_snprintf>
    6f44:	0000f7b7          	lui	a5,0xf
    6f48:	06300713          	li	a4,99
    6f4c:	02412883          	lw	a7,36(sp)
    6f50:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    6f54:	f7374463          	blt	a4,s3,66bc <render_game+0x968>
    6f58:	0000f7b7          	lui	a5,0xf
    6f5c:	04500713          	li	a4,69
    6f60:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6f64:	f5375863          	bge	a4,s3,66b4 <render_game+0x960>
    6f68:	03112223          	sw	a7,36(sp)
    6f6c:	9adff06f          	j	6918 <render_game+0xbc4>
    6f70:	0000f7b7          	lui	a5,0xf
    6f74:	000116b7          	lui	a3,0x11
    6f78:	76468693          	addi	a3,a3,1892 # 11764 <_data+0x28>
    6f7c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    6f80:	b0dff06f          	j	6a8c <render_game+0xd38>
    6f84:	0648a683          	lw	a3,100(a7)
    6f88:	00011637          	lui	a2,0x11
    6f8c:	7cc60613          	addi	a2,a2,1996 # 117cc <_data+0x90>
    6f90:	08000593          	li	a1,128
    6f94:	00090513          	mv	a0,s2
    6f98:	03112223          	sw	a7,36(sp)
    6f9c:	3e9090ef          	jal	10b84 <mini_snprintf>
    6fa0:	02412883          	lw	a7,36(sp)
    6fa4:	cd4ff06f          	j	6478 <render_game+0x724>
    6fa8:	0000f837          	lui	a6,0xf
    6fac:	b8e80813          	addi	a6,a6,-1138 # eb8e <game_tick+0x1d02>
    6fb0:	00300793          	li	a5,3
    6fb4:	17f00713          	li	a4,383
    6fb8:	26400693          	li	a3,612
    6fbc:	03c00613          	li	a2,60
    6fc0:	00e00593          	li	a1,14
    6fc4:	00040513          	mv	a0,s0
    6fc8:	01912e23          	sw	s9,28(sp)
    6fcc:	558020ef          	jal	9524 <gfx_border>
    6fd0:	01c12883          	lw	a7,28(sp)
    6fd4:	ba8ff06f          	j	637c <render_game+0x628>
    6fd8:	00088513          	mv	a0,a7
    6fdc:	00000593          	li	a1,0
    6fe0:	01112e23          	sw	a7,28(sp)
    6fe4:	1b5050ef          	jal	c998 <game_hostile_at>
    6fe8:	01c12883          	lw	a7,28(sp)
    6fec:	00050493          	mv	s1,a0
    6ff0:	af605263          	blez	s6,62d4 <render_game+0x580>
    6ff4:	000139b7          	lui	s3,0x13
    6ff8:	00000a93          	li	s5,0
    6ffc:	00300a13          	li	s4,3
    7000:	35498993          	addi	s3,s3,852 # 13354 <hostile_sprites>
    7004:	00088b93          	mv	s7,a7
    7008:	0004a783          	lw	a5,0(s1)
    700c:	04078a63          	beqz	a5,7060 <render_game+0x130c>
    7010:	0044a683          	lw	a3,4(s1)
    7014:	0084a703          	lw	a4,8(s1)
    7018:	01c4a783          	lw	a5,28(s1)
    701c:	41f6d593          	srai	a1,a3,0x1f
    7020:	41f75613          	srai	a2,a4,0x1f
    7024:	0ff5f593          	zext.b	a1,a1
    7028:	0ff67613          	zext.b	a2,a2
    702c:	00d585b3          	add	a1,a1,a3
    7030:	00e60633          	add	a2,a2,a4
    7034:	4085d593          	srai	a1,a1,0x8
    7038:	40865613          	srai	a2,a2,0x8
    703c:	00098693          	mv	a3,s3
    7040:	00fa6663          	bltu	s4,a5,704c <render_game+0x12f8>
    7044:	00479793          	slli	a5,a5,0x4
    7048:	013786b3          	add	a3,a5,s3
    704c:	00100713          	li	a4,1
    7050:	ffc60613          	addi	a2,a2,-4
    7054:	ffc58593          	addi	a1,a1,-4
    7058:	00040513          	mv	a0,s0
    705c:	491010ef          	jal	8cec <gfx_blit>
    7060:	001a8a93          	addi	s5,s5,1
    7064:	02048493          	addi	s1,s1,32
    7068:	fb5b10e3          	bne	s6,s5,7008 <render_game+0x12b4>
    706c:	000b8893          	mv	a7,s7
    7070:	a64ff06f          	j	62d4 <render_game+0x580>
    7074:	00f81463          	bne	a6,a5,707c <render_game+0x1328>
    7078:	e89fe06f          	j	5f00 <render_game+0x1ac>
    707c:	00088593          	mv	a1,a7
    7080:	00040513          	mv	a0,s0
    7084:	01112e23          	sw	a7,28(sp)
    7088:	d91fb0ef          	jal	2e18 <draw_player>
    708c:	01c12883          	lw	a7,28(sp)
    7090:	944ff06f          	j	61d4 <render_game+0x480>
    7094:	0000b7b7          	lui	a5,0xb
    7098:	000126b7          	lui	a3,0x12
    709c:	03112223          	sw	a7,36(sp)
    70a0:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    70a4:	00100713          	li	a4,1
    70a8:	87468693          	addi	a3,a3,-1932 # 11874 <_data+0x138>
    70ac:	9e9ff06f          	j	6a94 <render_game+0xd40>
    70b0:	03812783          	lw	a5,56(sp)
    70b4:	0287a703          	lw	a4,40(a5)
    70b8:	0087a483          	lw	s1,8(a5)
    70bc:	0247ad83          	lw	s11,36(a5)
    70c0:	02e12223          	sw	a4,36(sp)
    70c4:	02c7a703          	lw	a4,44(a5)
    70c8:	0307aa83          	lw	s5,48(a5)
    70cc:	0347ab03          	lw	s6,52(a5)
    70d0:	02e12423          	sw	a4,40(sp)
    70d4:	0507a703          	lw	a4,80(a5)
    70d8:	0407ab83          	lw	s7,64(a5)
    70dc:	0447ac03          	lw	s8,68(a5)
    70e0:	02e12c23          	sw	a4,56(sp)
    70e4:	0547a703          	lw	a4,84(a5)
    70e8:	0487a983          	lw	s3,72(a5)
    70ec:	04c7aa03          	lw	s4,76(a5)
    70f0:	02e12e23          	sw	a4,60(sp)
    70f4:	0587a703          	lw	a4,88(a5)
    70f8:	04e12023          	sw	a4,64(sp)
    70fc:	05c7a703          	lw	a4,92(a5)
    7100:	04e12223          	sw	a4,68(sp)
    7104:	0607a703          	lw	a4,96(a5)
    7108:	0647a783          	lw	a5,100(a5)
    710c:	04e12423          	sw	a4,72(sp)
    7110:	04f12623          	sw	a5,76(sp)
    7114:	01c12783          	lw	a5,28(sp)
    7118:	320790e3          	bnez	a5,7c38 <render_game+0x1ee4>
    711c:	000047b7          	lui	a5,0x4
    7120:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    7124:	04a00713          	li	a4,74
    7128:	11400693          	li	a3,276
    712c:	16f00613          	li	a2,367
    7130:	01600593          	li	a1,22
    7134:	00040513          	mv	a0,s0
    7138:	03112623          	sw	a7,44(sp)
    713c:	8d0fc0ef          	jal	320c <panel>
    7140:	000077b7          	lui	a5,0x7
    7144:	000126b7          	lui	a3,0x12
    7148:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    714c:	00100713          	li	a4,1
    7150:	b9868693          	addi	a3,a3,-1128 # 11b98 <_data+0x45c>
    7154:	17900613          	li	a2,377
    7158:	02000593          	li	a1,32
    715c:	00040513          	mv	a0,s0
    7160:	4c8020ef          	jal	9628 <gfx_text>
    7164:	1a300793          	li	a5,419
    7168:	02c12883          	lw	a7,44(sp)
    716c:	02f12a23          	sw	a5,52(sp)
    7170:	19500793          	li	a5,405
    7174:	02f12823          	sw	a5,48(sp)
    7178:	18700d13          	li	s10,391
    717c:	04a00c93          	li	s9,74
    7180:	00012637          	lui	a2,0x12
    7184:	000b0793          	mv	a5,s6
    7188:	000d8713          	mv	a4,s11
    718c:	00048693          	mv	a3,s1
    7190:	ac460613          	addi	a2,a2,-1340 # 11ac4 <_data+0x388>
    7194:	08000593          	li	a1,128
    7198:	00090513          	mv	a0,s2
    719c:	03112623          	sw	a7,44(sp)
    71a0:	1e5090ef          	jal	10b84 <mini_snprintf>
    71a4:	0000f7b7          	lui	a5,0xf
    71a8:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    71ac:	00100713          	li	a4,1
    71b0:	00090693          	mv	a3,s2
    71b4:	000d0613          	mv	a2,s10
    71b8:	02000593          	li	a1,32
    71bc:	00040513          	mv	a0,s0
    71c0:	468020ef          	jal	9628 <gfx_text>
    71c4:	02812703          	lw	a4,40(sp)
    71c8:	02412683          	lw	a3,36(sp)
    71cc:	00012637          	lui	a2,0x12
    71d0:	000a8793          	mv	a5,s5
    71d4:	ae060613          	addi	a2,a2,-1312 # 11ae0 <_data+0x3a4>
    71d8:	08000593          	li	a1,128
    71dc:	00090513          	mv	a0,s2
    71e0:	1a5090ef          	jal	10b84 <mini_snprintf>
    71e4:	03012603          	lw	a2,48(sp)
    71e8:	0000f7b7          	lui	a5,0xf
    71ec:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    71f0:	00100713          	li	a4,1
    71f4:	00090693          	mv	a3,s2
    71f8:	02000593          	li	a1,32
    71fc:	00040513          	mv	a0,s0
    7200:	428020ef          	jal	9628 <gfx_text>
    7204:	02c12883          	lw	a7,44(sp)
    7208:	0888a783          	lw	a5,136(a7)
    720c:	3e078ce3          	beqz	a5,7e04 <render_game+0x20b0>
    7210:	00088513          	mv	a0,a7
    7214:	03112223          	sw	a7,36(sp)
    7218:	021050ef          	jal	ca38 <game_hostile_count>
    721c:	02412883          	lw	a7,36(sp)
    7220:	00050493          	mv	s1,a0
    7224:	00088513          	mv	a0,a7
    7228:	70c050ef          	jal	c934 <game_hostile_slots>
    722c:	02412883          	lw	a7,36(sp)
    7230:	00050793          	mv	a5,a0
    7234:	00078a93          	mv	s5,a5
    7238:	00088513          	mv	a0,a7
    723c:	1b5050ef          	jal	cbf0 <game_dropped_shots>
    7240:	00012637          	lui	a2,0x12
    7244:	00050793          	mv	a5,a0
    7248:	000a8713          	mv	a4,s5
    724c:	00048693          	mv	a3,s1
    7250:	afc60613          	addi	a2,a2,-1284 # 11afc <_data+0x3c0>
    7254:	08000593          	li	a1,128
    7258:	00090513          	mv	a0,s2
    725c:	129090ef          	jal	10b84 <mini_snprintf>
    7260:	03412603          	lw	a2,52(sp)
    7264:	000097b7          	lui	a5,0x9
    7268:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    726c:	00100713          	li	a4,1
    7270:	00090693          	mv	a3,s2
    7274:	02000593          	li	a1,32
    7278:	00040513          	mv	a0,s0
    727c:	3ac020ef          	jal	9628 <gfx_text>
    7280:	01c12783          	lw	a5,28(sp)
    7284:	ba0786e3          	beqz	a5,6e30 <render_game+0x10dc>
    7288:	02012783          	lw	a5,32(sp)
    728c:	1fb00493          	li	s1,507
    7290:	419484b3          	sub	s1,s1,s9
    7294:	48078ee3          	beqz	a5,7f30 <render_game+0x21dc>
    7298:	04c12783          	lw	a5,76(sp)
    729c:	04812883          	lw	a7,72(sp)
    72a0:	03812803          	lw	a6,56(sp)
    72a4:	00f12023          	sw	a5,0(sp)
    72a8:	04012703          	lw	a4,64(sp)
    72ac:	04412783          	lw	a5,68(sp)
    72b0:	03c12683          	lw	a3,60(sp)
    72b4:	00012637          	lui	a2,0x12
    72b8:	b2c60613          	addi	a2,a2,-1236 # 11b2c <_data+0x3f0>
    72bc:	08000593          	li	a1,128
    72c0:	00090513          	mv	a0,s2
    72c4:	0c1090ef          	jal	10b84 <mini_snprintf>
    72c8:	000077b7          	lui	a5,0x7
    72cc:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    72d0:	00048613          	mv	a2,s1
    72d4:	00100713          	li	a4,1
    72d8:	00090693          	mv	a3,s2
    72dc:	02000593          	li	a1,32
    72e0:	00040513          	mv	a0,s0
    72e4:	20900493          	li	s1,521
    72e8:	340020ef          	jal	9628 <gfx_text>
    72ec:	419484b3          	sub	s1,s1,s9
    72f0:	497050e3          	blez	s7,7f70 <render_game+0x221c>
    72f4:	3e800793          	li	a5,1000
    72f8:	02f9e8b3          	rem	a7,s3,a5
    72fc:	06400813          	li	a6,100
    7300:	000126b7          	lui	a3,0x12
    7304:	00012637          	lui	a2,0x12
    7308:	000b8713          	mv	a4,s7
    730c:	b7068693          	addi	a3,a3,-1168 # 11b70 <_data+0x434>
    7310:	b7460613          	addi	a2,a2,-1164 # 11b74 <_data+0x438>
    7314:	03000593          	li	a1,48
    7318:	05010513          	addi	a0,sp,80
    731c:	0308c833          	div	a6,a7,a6
    7320:	02f9c7b3          	div	a5,s3,a5
    7324:	061090ef          	jal	10b84 <mini_snprintf>
    7328:	02012703          	lw	a4,32(sp)
    732c:	000097b7          	lui	a5,0x9
    7330:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    7334:	00071663          	bnez	a4,7340 <render_game+0x15ec>
    7338:	0000b7b7          	lui	a5,0xb
    733c:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    7340:	00100713          	li	a4,1
    7344:	05010693          	addi	a3,sp,80
    7348:	00048613          	mv	a2,s1
    734c:	02000593          	li	a1,32
    7350:	00040513          	mv	a0,s0
    7354:	2d4020ef          	jal	9628 <gfx_text>
    7358:	00100593          	li	a1,1
    735c:	05010513          	addi	a0,sp,80
    7360:	590020ef          	jal	98f0 <gfx_text_width>
    7364:	03250913          	addi	s2,a0,50
    7368:	3f8054e3          	blez	s8,7f50 <render_game+0x21fc>
    736c:	3e800793          	li	a5,1000
    7370:	02fa68b3          	rem	a7,s4,a5
    7374:	06400813          	li	a6,100
    7378:	000126b7          	lui	a3,0x12
    737c:	00012637          	lui	a2,0x12
    7380:	000c0713          	mv	a4,s8
    7384:	b9468693          	addi	a3,a3,-1132 # 11b94 <_data+0x458>
    7388:	b7460613          	addi	a2,a2,-1164 # 11b74 <_data+0x438>
    738c:	03000593          	li	a1,48
    7390:	05010513          	addi	a0,sp,80
    7394:	0308c833          	div	a6,a7,a6
    7398:	02fa47b3          	div	a5,s4,a5
    739c:	7e8090ef          	jal	10b84 <mini_snprintf>
    73a0:	02012703          	lw	a4,32(sp)
    73a4:	000097b7          	lui	a5,0x9
    73a8:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    73ac:	00070663          	beqz	a4,73b8 <render_game+0x1664>
    73b0:	0000b7b7          	lui	a5,0xb
    73b4:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    73b8:	00090593          	mv	a1,s2
    73bc:	00040513          	mv	a0,s0
    73c0:	00100713          	li	a4,1
    73c4:	05010693          	addi	a3,sp,80
    73c8:	00048613          	mv	a2,s1
    73cc:	25c020ef          	jal	9628 <gfx_text>
    73d0:	00100593          	li	a1,1
    73d4:	05010513          	addi	a0,sp,80
    73d8:	518020ef          	jal	98f0 <gfx_text_width>
    73dc:	a55ff06f          	j	6e30 <render_game+0x10dc>
    73e0:	0000b7b7          	lui	a5,0xb
    73e4:	03112223          	sw	a7,36(sp)
    73e8:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    73ec:	00098593          	mv	a1,s3
    73f0:	00200713          	li	a4,2
    73f4:	05010693          	addi	a3,sp,80
    73f8:	1c400613          	li	a2,452
    73fc:	00040513          	mv	a0,s0
    7400:	228020ef          	jal	9628 <gfx_text>
    7404:	00ca8593          	addi	a1,s5,12
    7408:	0000f7b7          	lui	a5,0xf
    740c:	02412883          	lw	a7,36(sp)
    7410:	40b985b3          	sub	a1,s3,a1
    7414:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7418:	f68ff06f          	j	6b80 <render_game+0xe2c>
    741c:	0000f7b7          	lui	a5,0xf
    7420:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    7424:	12900713          	li	a4,297
    7428:	1a400693          	li	a3,420
    742c:	06b00613          	li	a2,107
    7430:	06e00593          	li	a1,110
    7434:	00040513          	mv	a0,s0
    7438:	03112223          	sw	a7,36(sp)
    743c:	000129b7          	lui	s3,0x12
    7440:	dcdfb0ef          	jal	320c <panel>
    7444:	00200593          	li	a1,2
    7448:	a2c98513          	addi	a0,s3,-1492 # 11a2c <_data+0x2f0>
    744c:	4a4020ef          	jal	98f0 <gfx_text_width>
    7450:	01f55793          	srli	a5,a0,0x1f
    7454:	00a787b3          	add	a5,a5,a0
    7458:	4017d593          	srai	a1,a5,0x1
    745c:	40b005b3          	neg	a1,a1
    7460:	0000f7b7          	lui	a5,0xf
    7464:	a2c98693          	addi	a3,s3,-1492
    7468:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    746c:	00200713          	li	a4,2
    7470:	08500613          	li	a2,133
    7474:	14058593          	addi	a1,a1,320
    7478:	00040513          	mv	a0,s0
    747c:	1ac020ef          	jal	9628 <gfx_text>
    7480:	000129b7          	lui	s3,0x12
    7484:	00100593          	li	a1,1
    7488:	a4098513          	addi	a0,s3,-1472 # 11a40 <_data+0x304>
    748c:	464020ef          	jal	98f0 <gfx_text_width>
    7490:	01f55793          	srli	a5,a0,0x1f
    7494:	00a787b3          	add	a5,a5,a0
    7498:	4017d593          	srai	a1,a5,0x1
    749c:	40b005b3          	neg	a1,a1
    74a0:	0000f7b7          	lui	a5,0xf
    74a4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    74a8:	00100713          	li	a4,1
    74ac:	a4098693          	addi	a3,s3,-1472
    74b0:	0ab00613          	li	a2,171
    74b4:	14058593          	addi	a1,a1,320
    74b8:	00040513          	mv	a0,s0
    74bc:	16c020ef          	jal	9628 <gfx_text>
    74c0:	02412883          	lw	a7,36(sp)
    74c4:	03c00713          	li	a4,60
    74c8:	000017b7          	lui	a5,0x1
    74cc:	0088a683          	lw	a3,8(a7)
    74d0:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    74d4:	00012637          	lui	a2,0x12
    74d8:	02e6c833          	div	a6,a3,a4
    74dc:	a6060613          	addi	a2,a2,-1440 # 11a60 <_data+0x324>
    74e0:	08000593          	li	a1,128
    74e4:	00048513          	mv	a0,s1
    74e8:	02e86733          	rem	a4,a6,a4
    74ec:	02f6c6b3          	div	a3,a3,a5
    74f0:	694090ef          	jal	10b84 <mini_snprintf>
    74f4:	00200593          	li	a1,2
    74f8:	00048513          	mv	a0,s1
    74fc:	3f4020ef          	jal	98f0 <gfx_text_width>
    7500:	01f55793          	srli	a5,a0,0x1f
    7504:	00a787b3          	add	a5,a5,a0
    7508:	4017d593          	srai	a1,a5,0x1
    750c:	40b005b3          	neg	a1,a1
    7510:	0000f7b7          	lui	a5,0xf
    7514:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    7518:	00048693          	mv	a3,s1
    751c:	00200713          	li	a4,2
    7520:	0cf00613          	li	a2,207
    7524:	14058593          	addi	a1,a1,320
    7528:	00040513          	mv	a0,s0
    752c:	0fc020ef          	jal	9628 <gfx_text>
    7530:	02412883          	lw	a7,36(sp)
    7534:	00012637          	lui	a2,0x12
    7538:	a7060613          	addi	a2,a2,-1424 # 11a70 <_data+0x334>
    753c:	0548a703          	lw	a4,84(a7)
    7540:	0588a683          	lw	a3,88(a7)
    7544:	08000593          	li	a1,128
    7548:	00048513          	mv	a0,s1
    754c:	638090ef          	jal	10b84 <mini_snprintf>
    7550:	00200593          	li	a1,2
    7554:	00048513          	mv	a0,s1
    7558:	398020ef          	jal	98f0 <gfx_text_width>
    755c:	01f55793          	srli	a5,a0,0x1f
    7560:	00a787b3          	add	a5,a5,a0
    7564:	4017d593          	srai	a1,a5,0x1
    7568:	40b005b3          	neg	a1,a1
    756c:	0000f7b7          	lui	a5,0xf
    7570:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7574:	00200713          	li	a4,2
    7578:	00048693          	mv	a3,s1
    757c:	0f100613          	li	a2,241
    7580:	14058593          	addi	a1,a1,320
    7584:	00040513          	mv	a0,s0
    7588:	0a0020ef          	jal	9628 <gfx_text>
    758c:	02412883          	lw	a7,36(sp)
    7590:	00012637          	lui	a2,0x12
    7594:	a8460613          	addi	a2,a2,-1404 # 11a84 <_data+0x348>
    7598:	0848a683          	lw	a3,132(a7)
    759c:	08000593          	li	a1,128
    75a0:	00048513          	mv	a0,s1
    75a4:	5e0090ef          	jal	10b84 <mini_snprintf>
    75a8:	00100593          	li	a1,1
    75ac:	00048513          	mv	a0,s1
    75b0:	340020ef          	jal	98f0 <gfx_text_width>
    75b4:	01f55793          	srli	a5,a0,0x1f
    75b8:	00a787b3          	add	a5,a5,a0
    75bc:	4017d593          	srai	a1,a5,0x1
    75c0:	40b005b3          	neg	a1,a1
    75c4:	000097b7          	lui	a5,0x9
    75c8:	00048693          	mv	a3,s1
    75cc:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    75d0:	00100713          	li	a4,1
    75d4:	11700613          	li	a2,279
    75d8:	14058593          	addi	a1,a1,320
    75dc:	00040513          	mv	a0,s0
    75e0:	048020ef          	jal	9628 <gfx_text>
    75e4:	0000f7b7          	lui	a5,0xf
    75e8:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    75ec:	02500713          	li	a4,37
    75f0:	12400693          	li	a3,292
    75f4:	13d00613          	li	a2,317
    75f8:	0ae00593          	li	a1,174
    75fc:	00040513          	mv	a0,s0
    7600:	6dc010ef          	jal	8cdc <gfx_fill_rect>
    7604:	000124b7          	lui	s1,0x12
    7608:	00200593          	li	a1,2
    760c:	a9848513          	addi	a0,s1,-1384 # 11a98 <_data+0x35c>
    7610:	2e0020ef          	jal	98f0 <gfx_text_width>
    7614:	01f55793          	srli	a5,a0,0x1f
    7618:	00a787b3          	add	a5,a5,a0
    761c:	4017d593          	srai	a1,a5,0x1
    7620:	40b005b3          	neg	a1,a1
    7624:	000017b7          	lui	a5,0x1
    7628:	a9848693          	addi	a3,s1,-1384
    762c:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    7630:	00200713          	li	a4,2
    7634:	14900613          	li	a2,329
    7638:	14058593          	addi	a1,a1,320
    763c:	00040513          	mv	a0,s0
    7640:	7e9010ef          	jal	9628 <gfx_text>
    7644:	000124b7          	lui	s1,0x12
    7648:	00100593          	li	a1,1
    764c:	aac48513          	addi	a0,s1,-1364 # 11aac <_data+0x370>
    7650:	2a0020ef          	jal	98f0 <gfx_text_width>
    7654:	01f55793          	srli	a5,a0,0x1f
    7658:	00a787b3          	add	a5,a5,a0
    765c:	4017d593          	srai	a1,a5,0x1
    7660:	40b005b3          	neg	a1,a1
    7664:	000097b7          	lui	a5,0x9
    7668:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    766c:	00100713          	li	a4,1
    7670:	aac48693          	addi	a3,s1,-1364
    7674:	17a00613          	li	a2,378
    7678:	14058593          	addi	a1,a1,320
    767c:	00040513          	mv	a0,s0
    7680:	7a9010ef          	jal	9628 <gfx_text>
    7684:	02412883          	lw	a7,36(sp)
    7688:	f9cff06f          	j	6e24 <render_game+0x10d0>
    768c:	000047b7          	lui	a5,0x4
    7690:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    7694:	12900713          	li	a4,297
    7698:	25800693          	li	a3,600
    769c:	06400613          	li	a2,100
    76a0:	01400593          	li	a1,20
    76a4:	00040513          	mv	a0,s0
    76a8:	03112223          	sw	a7,36(sp)
    76ac:	000129b7          	lui	s3,0x12
    76b0:	b5dfb0ef          	jal	320c <panel>
    76b4:	00300593          	li	a1,3
    76b8:	98098513          	addi	a0,s3,-1664 # 11980 <_data+0x244>
    76bc:	234020ef          	jal	98f0 <gfx_text_width>
    76c0:	01f55793          	srli	a5,a0,0x1f
    76c4:	00a787b3          	add	a5,a5,a0
    76c8:	4017d593          	srai	a1,a5,0x1
    76cc:	40b005b3          	neg	a1,a1
    76d0:	0000b7b7          	lui	a5,0xb
    76d4:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    76d8:	00300713          	li	a4,3
    76dc:	98098693          	addi	a3,s3,-1664
    76e0:	07800613          	li	a2,120
    76e4:	14058593          	addi	a1,a1,320
    76e8:	00040513          	mv	a0,s0
    76ec:	73d010ef          	jal	9628 <gfx_text>
    76f0:	02412883          	lw	a7,36(sp)
    76f4:	00012637          	lui	a2,0x12
    76f8:	99060613          	addi	a2,a2,-1648 # 11990 <_data+0x254>
    76fc:	0548a683          	lw	a3,84(a7)
    7700:	08000593          	li	a1,128
    7704:	00048513          	mv	a0,s1
    7708:	47c090ef          	jal	10b84 <mini_snprintf>
    770c:	00100593          	li	a1,1
    7710:	00048513          	mv	a0,s1
    7714:	1dc020ef          	jal	98f0 <gfx_text_width>
    7718:	01f55793          	srli	a5,a0,0x1f
    771c:	00a787b3          	add	a5,a5,a0
    7720:	4017d593          	srai	a1,a5,0x1
    7724:	40b005b3          	neg	a1,a1
    7728:	0000f7b7          	lui	a5,0xf
    772c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    7730:	00100713          	li	a4,1
    7734:	00048693          	mv	a3,s1
    7738:	09800613          	li	a2,152
    773c:	14058593          	addi	a1,a1,320
    7740:	00040513          	mv	a0,s0
    7744:	6e5010ef          	jal	9628 <gfx_text>
    7748:	03812703          	lw	a4,56(sp)
    774c:	01f00793          	li	a5,31
    7750:	02412883          	lw	a7,36(sp)
    7754:	01872d83          	lw	s11,24(a4)
    7758:	01c72703          	lw	a4,28(a4)
    775c:	02e12a23          	sw	a4,52(sp)
    7760:	07b7cce3          	blt	a5,s11,7fd8 <render_game+0x2284>
    7764:	00012737          	lui	a4,0x12
    7768:	000047b7          	lui	a5,0x4
    776c:	00009637          	lui	a2,0x9
    7770:	0000f337          	lui	t1,0xf
    7774:	0000b837          	lui	a6,0xb
    7778:	0000f6b7          	lui	a3,0xf
    777c:	02e12423          	sw	a4,40(sp)
    7780:	00012737          	lui	a4,0x12
    7784:	02e12623          	sw	a4,44(sp)
    7788:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    778c:	51360613          	addi	a2,a2,1299 # 9513 <gfx_blit_alpha+0x3af>
    7790:	00013737          	lui	a4,0x13
    7794:	77a30313          	addi	t1,t1,1914 # f77a <demo_init+0xf6>
    7798:	70d80813          	addi	a6,a6,1805 # b70d <check_level+0x61>
    779c:	5eb68693          	addi	a3,a3,1515 # f5eb <overlay.isra.0+0x307>
    77a0:	0ac88b93          	addi	s7,a7,172
    77a4:	06e00b13          	li	s6,110
    77a8:	07100c13          	li	s8,113
    77ac:	00000a13          	li	s4,0
    77b0:	00400d13          	li	s10,4
    77b4:	9d870c93          	addi	s9,a4,-1576 # 129d8 <_data+0x129c>
    77b8:	05112223          	sw	a7,68(sp)
    77bc:	02f12823          	sw	a5,48(sp)
    77c0:	00060d93          	mv	s11,a2
    77c4:	02612a23          	sw	t1,52(sp)
    77c8:	03012e23          	sw	a6,60(sp)
    77cc:	04d12023          	sw	a3,64(sp)
    77d0:	03012783          	lw	a5,48(sp)
    77d4:	0b700713          	li	a4,183
    77d8:	0b400693          	li	a3,180
    77dc:	0be00613          	li	a2,190
    77e0:	fafc0593          	addi	a1,s8,-81
    77e4:	00040513          	mv	a0,s0
    77e8:	000ba983          	lw	s3,0(s7)
    77ec:	a21fb0ef          	jal	320c <panel>
    77f0:	02812783          	lw	a5,40(sp)
    77f4:	001a0a13          	addi	s4,s4,1
    77f8:	000a0693          	mv	a3,s4
    77fc:	9b078613          	addi	a2,a5,-1616
    7800:	08000593          	li	a1,128
    7804:	00048513          	mv	a0,s1
    7808:	37c090ef          	jal	10b84 <mini_snprintf>
    780c:	000d8793          	mv	a5,s11
    7810:	00200713          	li	a4,2
    7814:	00048693          	mv	a3,s1
    7818:	0cb00613          	li	a2,203
    781c:	fbbc0593          	addi	a1,s8,-69
    7820:	00040513          	mv	a0,s0
    7824:	605010ef          	jal	9628 <gfx_text>
    7828:	009c0a93          	addi	s5,s8,9
    782c:	013d7463          	bgeu	s10,s3,7834 <render_game+0x1ae0>
    7830:	1b40106f          	j	89e4 <render_game+0x2c90>
    7834:	00299793          	slli	a5,s3,0x2
    7838:	019787b3          	add	a5,a5,s9
    783c:	0007a783          	lw	a5,0(a5)
    7840:	00078067          	jr	a5
    7844:	000047b7          	lui	a5,0x4
    7848:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    784c:	0ae00713          	li	a4,174
    7850:	17200693          	li	a3,370
    7854:	09a00613          	li	a2,154
    7858:	08700593          	li	a1,135
    785c:	00040513          	mv	a0,s0
    7860:	03112223          	sw	a7,36(sp)
    7864:	000124b7          	lui	s1,0x12
    7868:	9a5fb0ef          	jal	320c <panel>
    786c:	00200593          	li	a1,2
    7870:	9dc48513          	addi	a0,s1,-1572 # 119dc <_data+0x2a0>
    7874:	07c020ef          	jal	98f0 <gfx_text_width>
    7878:	01f55793          	srli	a5,a0,0x1f
    787c:	00a787b3          	add	a5,a5,a0
    7880:	4017d593          	srai	a1,a5,0x1
    7884:	40b005b3          	neg	a1,a1
    7888:	0000f7b7          	lui	a5,0xf
    788c:	9dc48693          	addi	a3,s1,-1572
    7890:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    7894:	00200713          	li	a4,2
    7898:	0b100613          	li	a2,177
    789c:	14058593          	addi	a1,a1,320
    78a0:	00040513          	mv	a0,s0
    78a4:	585010ef          	jal	9628 <gfx_text>
    78a8:	000124b7          	lui	s1,0x12
    78ac:	00100593          	li	a1,1
    78b0:	9ec48513          	addi	a0,s1,-1556 # 119ec <_data+0x2b0>
    78b4:	03c020ef          	jal	98f0 <gfx_text_width>
    78b8:	01f55793          	srli	a5,a0,0x1f
    78bc:	00a787b3          	add	a5,a5,a0
    78c0:	4017d593          	srai	a1,a5,0x1
    78c4:	40b005b3          	neg	a1,a1
    78c8:	000097b7          	lui	a5,0x9
    78cc:	9ec48693          	addi	a3,s1,-1556
    78d0:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    78d4:	00100713          	li	a4,1
    78d8:	0d700613          	li	a2,215
    78dc:	14058593          	addi	a1,a1,320
    78e0:	00040513          	mv	a0,s0
    78e4:	545010ef          	jal	9628 <gfx_text>
    78e8:	000124b7          	lui	s1,0x12
    78ec:	00200593          	li	a1,2
    78f0:	a0448513          	addi	a0,s1,-1532 # 11a04 <_data+0x2c8>
    78f4:	7fd010ef          	jal	98f0 <gfx_text_width>
    78f8:	01f55793          	srli	a5,a0,0x1f
    78fc:	00a787b3          	add	a5,a5,a0
    7900:	4017d593          	srai	a1,a5,0x1
    7904:	40b005b3          	neg	a1,a1
    7908:	0000b7b7          	lui	a5,0xb
    790c:	a0448693          	addi	a3,s1,-1532
    7910:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    7914:	00200713          	li	a4,2
    7918:	0fc00613          	li	a2,252
    791c:	14058593          	addi	a1,a1,320
    7920:	00040513          	mv	a0,s0
    7924:	505010ef          	jal	9628 <gfx_text>
    7928:	0000f7b7          	lui	a5,0xf
    792c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7930:	02400713          	li	a4,36
    7934:	10400693          	li	a3,260
    7938:	11800613          	li	a2,280
    793c:	0be00593          	li	a1,190
    7940:	00040513          	mv	a0,s0
    7944:	398010ef          	jal	8cdc <gfx_fill_rect>
    7948:	000124b7          	lui	s1,0x12
    794c:	00100593          	li	a1,1
    7950:	a1848513          	addi	a0,s1,-1512 # 11a18 <_data+0x2dc>
    7954:	79d010ef          	jal	98f0 <gfx_text_width>
    7958:	01f55793          	srli	a5,a0,0x1f
    795c:	00a787b3          	add	a5,a5,a0
    7960:	4017d593          	srai	a1,a5,0x1
    7964:	40b005b3          	neg	a1,a1
    7968:	000017b7          	lui	a5,0x1
    796c:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    7970:	00100713          	li	a4,1
    7974:	a1848693          	addi	a3,s1,-1512
    7978:	12300613          	li	a2,291
    797c:	14058593          	addi	a1,a1,320
    7980:	00040513          	mv	a0,s0
    7984:	4a5010ef          	jal	9628 <gfx_text>
    7988:	02412883          	lw	a7,36(sp)
    798c:	c98ff06f          	j	6e24 <render_game+0x10d0>
    7990:	000047b7          	lui	a5,0x4
    7994:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    7998:	14900713          	li	a4,329
    799c:	1a000693          	li	a3,416
    79a0:	05600613          	li	a2,86
    79a4:	07000593          	li	a1,112
    79a8:	00040513          	mv	a0,s0
    79ac:	03112223          	sw	a7,36(sp)
    79b0:	85dfb0ef          	jal	320c <panel>
    79b4:	0000b7b7          	lui	a5,0xb
    79b8:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    79bc:	00400713          	li	a4,4
    79c0:	1a000693          	li	a3,416
    79c4:	05600613          	li	a2,86
    79c8:	07000593          	li	a1,112
    79cc:	00040513          	mv	a0,s0
    79d0:	30c010ef          	jal	8cdc <gfx_fill_rect>
    79d4:	000124b7          	lui	s1,0x12
    79d8:	00100593          	li	a1,1
    79dc:	8ac48513          	addi	a0,s1,-1876 # 118ac <_data+0x170>
    79e0:	711010ef          	jal	98f0 <gfx_text_width>
    79e4:	01f55793          	srli	a5,a0,0x1f
    79e8:	00a787b3          	add	a5,a5,a0
    79ec:	4017d593          	srai	a1,a5,0x1
    79f0:	40b005b3          	neg	a1,a1
    79f4:	0000b7b7          	lui	a5,0xb
    79f8:	8ac48693          	addi	a3,s1,-1876
    79fc:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    7a00:	00100713          	li	a4,1
    7a04:	06c00613          	li	a2,108
    7a08:	14058593          	addi	a1,a1,320
    7a0c:	00040513          	mv	a0,s0
    7a10:	419010ef          	jal	9628 <gfx_text>
    7a14:	000124b7          	lui	s1,0x12
    7a18:	00600593          	li	a1,6
    7a1c:	8c448513          	addi	a0,s1,-1852 # 118c4 <_data+0x188>
    7a20:	6d1010ef          	jal	98f0 <gfx_text_width>
    7a24:	01f55793          	srli	a5,a0,0x1f
    7a28:	00a787b3          	add	a5,a5,a0
    7a2c:	4017d593          	srai	a1,a5,0x1
    7a30:	40b005b3          	neg	a1,a1
    7a34:	0000f7b7          	lui	a5,0xf
    7a38:	8c448693          	addi	a3,s1,-1852
    7a3c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    7a40:	00600713          	li	a4,6
    7a44:	08900613          	li	a2,137
    7a48:	14058593          	addi	a1,a1,320
    7a4c:	00040513          	mv	a0,s0
    7a50:	3d9010ef          	jal	9628 <gfx_text>
    7a54:	000124b7          	lui	s1,0x12
    7a58:	00600593          	li	a1,6
    7a5c:	8cc48513          	addi	a0,s1,-1844 # 118cc <_data+0x190>
    7a60:	691010ef          	jal	98f0 <gfx_text_width>
    7a64:	01f55793          	srli	a5,a0,0x1f
    7a68:	00a787b3          	add	a5,a5,a0
    7a6c:	4017d593          	srai	a1,a5,0x1
    7a70:	40b005b3          	neg	a1,a1
    7a74:	0000f7b7          	lui	a5,0xf
    7a78:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7a7c:	8cc48693          	addi	a3,s1,-1844
    7a80:	00600713          	li	a4,6
    7a84:	0b900613          	li	a2,185
    7a88:	14058593          	addi	a1,a1,320
    7a8c:	00040513          	mv	a0,s0
    7a90:	399010ef          	jal	9628 <gfx_text>
    7a94:	000136b7          	lui	a3,0x13
    7a98:	00100713          	li	a4,1
    7a9c:	3f468693          	addi	a3,a3,1012 # 133f4 <player>
    7aa0:	0f400613          	li	a2,244
    7aa4:	13400593          	li	a1,308
    7aa8:	00040513          	mv	a0,s0
    7aac:	240010ef          	jal	8cec <gfx_blit>
    7ab0:	000124b7          	lui	s1,0x12
    7ab4:	00100593          	li	a1,1
    7ab8:	8d048513          	addi	a0,s1,-1840 # 118d0 <_data+0x194>
    7abc:	635010ef          	jal	98f0 <gfx_text_width>
    7ac0:	01f55793          	srli	a5,a0,0x1f
    7ac4:	00a787b3          	add	a5,a5,a0
    7ac8:	4017d593          	srai	a1,a5,0x1
    7acc:	40b005b3          	neg	a1,a1
    7ad0:	0000f7b7          	lui	a5,0xf
    7ad4:	8d048693          	addi	a3,s1,-1840
    7ad8:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    7adc:	00100713          	li	a4,1
    7ae0:	11e00613          	li	a2,286
    7ae4:	14058593          	addi	a1,a1,320
    7ae8:	00040513          	mv	a0,s0
    7aec:	33d010ef          	jal	9628 <gfx_text>
    7af0:	000124b7          	lui	s1,0x12
    7af4:	00100593          	li	a1,1
    7af8:	8f048513          	addi	a0,s1,-1808 # 118f0 <_data+0x1b4>
    7afc:	5f5010ef          	jal	98f0 <gfx_text_width>
    7b00:	01f55793          	srli	a5,a0,0x1f
    7b04:	00a787b3          	add	a5,a5,a0
    7b08:	4017d593          	srai	a1,a5,0x1
    7b0c:	40b005b3          	neg	a1,a1
    7b10:	000097b7          	lui	a5,0x9
    7b14:	8f048693          	addi	a3,s1,-1808
    7b18:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    7b1c:	00100713          	li	a4,1
    7b20:	12c00613          	li	a2,300
    7b24:	14058593          	addi	a1,a1,320
    7b28:	00040513          	mv	a0,s0
    7b2c:	2fd010ef          	jal	9628 <gfx_text>
    7b30:	0000b7b7          	lui	a5,0xb
    7b34:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    7b38:	02000713          	li	a4,32
    7b3c:	11600693          	li	a3,278
    7b40:	13400613          	li	a2,308
    7b44:	0b500593          	li	a1,181
    7b48:	00040513          	mv	a0,s0
    7b4c:	190010ef          	jal	8cdc <gfx_fill_rect>
    7b50:	000124b7          	lui	s1,0x12
    7b54:	00200593          	li	a1,2
    7b58:	91448513          	addi	a0,s1,-1772 # 11914 <_data+0x1d8>
    7b5c:	595010ef          	jal	98f0 <gfx_text_width>
    7b60:	01f55793          	srli	a5,a0,0x1f
    7b64:	00a787b3          	add	a5,a5,a0
    7b68:	4017d593          	srai	a1,a5,0x1
    7b6c:	40b005b3          	neg	a1,a1
    7b70:	000017b7          	lui	a5,0x1
    7b74:	91448693          	addi	a3,s1,-1772
    7b78:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    7b7c:	00200713          	li	a4,2
    7b80:	13e00613          	li	a2,318
    7b84:	14058593          	addi	a1,a1,320
    7b88:	00040513          	mv	a0,s0
    7b8c:	29d010ef          	jal	9628 <gfx_text>
    7b90:	0000f7b7          	lui	a5,0xf
    7b94:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7b98:	02000713          	li	a4,32
    7b9c:	11600693          	li	a3,278
    7ba0:	15c00613          	li	a2,348
    7ba4:	0b500593          	li	a1,181
    7ba8:	00040513          	mv	a0,s0
    7bac:	130010ef          	jal	8cdc <gfx_fill_rect>
    7bb0:	000124b7          	lui	s1,0x12
    7bb4:	00200593          	li	a1,2
    7bb8:	92448513          	addi	a0,s1,-1756 # 11924 <_data+0x1e8>
    7bbc:	535010ef          	jal	98f0 <gfx_text_width>
    7bc0:	01f55793          	srli	a5,a0,0x1f
    7bc4:	00a787b3          	add	a5,a5,a0
    7bc8:	4017d593          	srai	a1,a5,0x1
    7bcc:	40b005b3          	neg	a1,a1
    7bd0:	000017b7          	lui	a5,0x1
    7bd4:	92448693          	addi	a3,s1,-1756
    7bd8:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    7bdc:	00200713          	li	a4,2
    7be0:	16600613          	li	a2,358
    7be4:	14058593          	addi	a1,a1,320
    7be8:	00040513          	mv	a0,s0
    7bec:	23d010ef          	jal	9628 <gfx_text>
    7bf0:	000124b7          	lui	s1,0x12
    7bf4:	00100593          	li	a1,1
    7bf8:	93448513          	addi	a0,s1,-1740 # 11934 <_data+0x1f8>
    7bfc:	4f5010ef          	jal	98f0 <gfx_text_width>
    7c00:	01f55793          	srli	a5,a0,0x1f
    7c04:	00a787b3          	add	a5,a5,a0
    7c08:	4017d593          	srai	a1,a5,0x1
    7c0c:	40b005b3          	neg	a1,a1
    7c10:	000097b7          	lui	a5,0x9
    7c14:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    7c18:	00100713          	li	a4,1
    7c1c:	93448693          	addi	a3,s1,-1740
    7c20:	18800613          	li	a2,392
    7c24:	14058593          	addi	a1,a1,320
    7c28:	00040513          	mv	a0,s0
    7c2c:	1fd010ef          	jal	9628 <gfx_text>
    7c30:	02412883          	lw	a7,36(sp)
    7c34:	9f0ff06f          	j	6e24 <render_game+0x10d0>
    7c38:	000047b7          	lui	a5,0x4
    7c3c:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    7c40:	06600713          	li	a4,102
    7c44:	11400693          	li	a3,276
    7c48:	15300613          	li	a2,339
    7c4c:	01600593          	li	a1,22
    7c50:	00040513          	mv	a0,s0
    7c54:	03112623          	sw	a7,44(sp)
    7c58:	db4fb0ef          	jal	320c <panel>
    7c5c:	000077b7          	lui	a5,0x7
    7c60:	000126b7          	lui	a3,0x12
    7c64:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    7c68:	00100713          	li	a4,1
    7c6c:	b9868693          	addi	a3,a3,-1128 # 11b98 <_data+0x45c>
    7c70:	15d00613          	li	a2,349
    7c74:	02000593          	li	a1,32
    7c78:	00040513          	mv	a0,s0
    7c7c:	1ad010ef          	jal	9628 <gfx_text>
    7c80:	02012783          	lw	a5,32(sp)
    7c84:	02c12883          	lw	a7,44(sp)
    7c88:	32079e63          	bnez	a5,7fc4 <render_game+0x2270>
    7c8c:	0000f7b7          	lui	a5,0xf
    7c90:	000116b7          	lui	a3,0x11
    7c94:	76468693          	addi	a3,a3,1892 # 11764 <_data+0x28>
    7c98:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7c9c:	00100713          	li	a4,1
    7ca0:	15d00613          	li	a2,349
    7ca4:	0e800593          	li	a1,232
    7ca8:	00040513          	mv	a0,s0
    7cac:	03112623          	sw	a7,44(sp)
    7cb0:	179010ef          	jal	9628 <gfx_text>
    7cb4:	18700793          	li	a5,391
    7cb8:	02f12a23          	sw	a5,52(sp)
    7cbc:	17900793          	li	a5,377
    7cc0:	02c12883          	lw	a7,44(sp)
    7cc4:	02f12823          	sw	a5,48(sp)
    7cc8:	16b00d13          	li	s10,363
    7ccc:	06600c93          	li	s9,102
    7cd0:	cb0ff06f          	j	7180 <render_game+0x142c>
    7cd4:	0000f7b7          	lui	a5,0xf
    7cd8:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7cdc:	e65fe06f          	j	6b40 <render_game+0xdec>
    7ce0:	0000f7b7          	lui	a5,0xf
    7ce4:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    7ce8:	e59fe06f          	j	6b40 <render_game+0xdec>
    7cec:	0888a703          	lw	a4,136(a7)
    7cf0:	0908a783          	lw	a5,144(a7)
    7cf4:	00070463          	beqz	a4,7cfc <render_game+0x1fa8>
    7cf8:	480782e3          	beqz	a5,897c <render_game+0x2c28>
    7cfc:	fff78793          	addi	a5,a5,-1
    7d00:	00200713          	li	a4,2
    7d04:	46f76c63          	bltu	a4,a5,817c <render_game+0x2428>
    7d08:	00013737          	lui	a4,0x13
    7d0c:	00279793          	slli	a5,a5,0x2
    7d10:	a0070713          	addi	a4,a4,-1536 # 12a00 <CSWTCH.93>
    7d14:	00f707b3          	add	a5,a4,a5
    7d18:	0007aa03          	lw	s4,0(a5)
    7d1c:	00100593          	li	a1,1
    7d20:	000a0513          	mv	a0,s4
    7d24:	03112223          	sw	a7,36(sp)
    7d28:	3c9010ef          	jal	98f0 <gfx_text_width>
    7d2c:	01a50a93          	addi	s5,a0,26
    7d30:	01fad993          	srli	s3,s5,0x1f
    7d34:	015989b3          	add	s3,s3,s5
    7d38:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    7d3c:	4019d993          	srai	s3,s3,0x1
    7d40:	413009b3          	neg	s3,s3
    7d44:	02412883          	lw	a7,36(sp)
    7d48:	14098993          	addi	s3,s3,320
    7d4c:	24078263          	beqz	a5,7f90 <render_game+0x223c>
    7d50:	000017b7          	lui	a5,0x1
    7d54:	0b400813          	li	a6,180
    7d58:	10578793          	addi	a5,a5,261 # 1105 <memset+0x1>
    7d5c:	01500713          	li	a4,21
    7d60:	000a8693          	mv	a3,s5
    7d64:	04400613          	li	a2,68
    7d68:	00098593          	mv	a1,s3
    7d6c:	00040513          	mv	a0,s0
    7d70:	144010ef          	jal	8eb4 <gfx_fill_alpha>
    7d74:	02412883          	lw	a7,36(sp)
    7d78:	0000f7b7          	lui	a5,0xf
    7d7c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7d80:	00100713          	li	a4,1
    7d84:	000a8693          	mv	a3,s5
    7d88:	04400613          	li	a2,68
    7d8c:	00098593          	mv	a1,s3
    7d90:	00040513          	mv	a0,s0
    7d94:	03112223          	sw	a7,36(sp)
    7d98:	745000ef          	jal	8cdc <gfx_fill_rect>
    7d9c:	0000f7b7          	lui	a5,0xf
    7da0:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7da4:	00100713          	li	a4,1
    7da8:	000a8693          	mv	a3,s5
    7dac:	05800613          	li	a2,88
    7db0:	00098593          	mv	a1,s3
    7db4:	00040513          	mv	a0,s0
    7db8:	725000ef          	jal	8cdc <gfx_fill_rect>
    7dbc:	00100593          	li	a1,1
    7dc0:	000a0513          	mv	a0,s4
    7dc4:	32d010ef          	jal	98f0 <gfx_text_width>
    7dc8:	01f55793          	srli	a5,a0,0x1f
    7dcc:	00a787b3          	add	a5,a5,a0
    7dd0:	4017d593          	srai	a1,a5,0x1
    7dd4:	40b005b3          	neg	a1,a1
    7dd8:	0000f7b7          	lui	a5,0xf
    7ddc:	14058593          	addi	a1,a1,320
    7de0:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    7de4:	00100713          	li	a4,1
    7de8:	000a0693          	mv	a3,s4
    7dec:	04b00613          	li	a2,75
    7df0:	00040513          	mv	a0,s0
    7df4:	035010ef          	jal	9628 <gfx_text>
    7df8:	02412883          	lw	a7,36(sp)
    7dfc:	0008a583          	lw	a1,0(a7)
    7e00:	db5fe06f          	j	6bb4 <render_game+0xe60>
    7e04:	02042783          	lw	a5,32(s0)
    7e08:	01842703          	lw	a4,24(s0)
    7e0c:	01442683          	lw	a3,20(s0)
    7e10:	00012637          	lui	a2,0x12
    7e14:	b1060613          	addi	a2,a2,-1264 # 11b10 <_data+0x3d4>
    7e18:	08000593          	li	a1,128
    7e1c:	00090513          	mv	a0,s2
    7e20:	565080ef          	jal	10b84 <mini_snprintf>
    7e24:	c3cff06f          	j	7260 <render_game+0x150c>
    7e28:	00011c37          	lui	s8,0x11
    7e2c:	740c0c13          	addi	s8,s8,1856 # 11740 <_data+0x4>
    7e30:	e00fe06f          	j	6430 <render_game+0x6dc>
    7e34:	8101a783          	lw	a5,-2032(gp) # 13418 <fx_alpha>
    7e38:	03112223          	sw	a7,36(sp)
    7e3c:	28078a63          	beqz	a5,80d0 <render_game+0x237c>
    7e40:	000017b7          	lui	a5,0x1
    7e44:	0aa00813          	li	a6,170
    7e48:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    7e4c:	01a00713          	li	a4,26
    7e50:	0e800693          	li	a3,232
    7e54:	05e00613          	li	a2,94
    7e58:	0cc00593          	li	a1,204
    7e5c:	00040513          	mv	a0,s0
    7e60:	054010ef          	jal	8eb4 <gfx_fill_alpha>
    7e64:	02412883          	lw	a7,36(sp)
    7e68:	02c00793          	li	a5,44
    7e6c:	02fa0733          	mul	a4,s4,a5
    7e70:	00098793          	mv	a5,s3
    7e74:	00012637          	lui	a2,0x12
    7e78:	89460613          	addi	a2,a2,-1900 # 11894 <_data+0x158>
    7e7c:	08000593          	li	a1,128
    7e80:	00090513          	mv	a0,s2
    7e84:	03112223          	sw	a7,36(sp)
    7e88:	00e889b3          	add	s3,a7,a4
    7e8c:	0c89a703          	lw	a4,200(s3)
    7e90:	0c49a683          	lw	a3,196(s3)
    7e94:	4f1080ef          	jal	10b84 <mini_snprintf>
    7e98:	00100593          	li	a1,1
    7e9c:	00090513          	mv	a0,s2
    7ea0:	251010ef          	jal	98f0 <gfx_text_width>
    7ea4:	01f55793          	srli	a5,a0,0x1f
    7ea8:	00a787b3          	add	a5,a5,a0
    7eac:	4017d593          	srai	a1,a5,0x1
    7eb0:	40b005b3          	neg	a1,a1
    7eb4:	0000f7b7          	lui	a5,0xf
    7eb8:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    7ebc:	00100713          	li	a4,1
    7ec0:	00090693          	mv	a3,s2
    7ec4:	06200613          	li	a2,98
    7ec8:	14058593          	addi	a1,a1,320
    7ecc:	00040513          	mv	a0,s0
    7ed0:	758010ef          	jal	9628 <gfx_text>
    7ed4:	000047b7          	lui	a5,0x4
    7ed8:	2aa78793          	addi	a5,a5,682 # 42aa <__stack_size+0x2aa>
    7edc:	00400713          	li	a4,4
    7ee0:	0dc00693          	li	a3,220
    7ee4:	06f00613          	li	a2,111
    7ee8:	0d200593          	li	a1,210
    7eec:	00040513          	mv	a0,s0
    7ef0:	5ed000ef          	jal	8cdc <gfx_fill_rect>
    7ef4:	0c49a703          	lw	a4,196(s3)
    7ef8:	0dc00793          	li	a5,220
    7efc:	0c89a803          	lw	a6,200(s3)
    7f00:	02e786b3          	mul	a3,a5,a4
    7f04:	0000f7b7          	lui	a5,0xf
    7f08:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    7f0c:	00400713          	li	a4,4
    7f10:	06f00613          	li	a2,111
    7f14:	0d200593          	li	a1,210
    7f18:	00040513          	mv	a0,s0
    7f1c:	0306c6b3          	div	a3,a3,a6
    7f20:	5bd000ef          	jal	8cdc <gfx_fill_rect>
    7f24:	02412883          	lw	a7,36(sp)
    7f28:	0008a783          	lw	a5,0(a7)
    7f2c:	ce9fe06f          	j	6c14 <render_game+0xec0>
    7f30:	00012637          	lui	a2,0x12
    7f34:	b5460613          	addi	a2,a2,-1196 # 11b54 <_data+0x418>
    7f38:	08000593          	li	a1,128
    7f3c:	00090513          	mv	a0,s2
    7f40:	445080ef          	jal	10b84 <mini_snprintf>
    7f44:	000097b7          	lui	a5,0x9
    7f48:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    7f4c:	b84ff06f          	j	72d0 <render_game+0x157c>
    7f50:	000126b7          	lui	a3,0x12
    7f54:	00012637          	lui	a2,0x12
    7f58:	b9468693          	addi	a3,a3,-1132 # 11b94 <_data+0x458>
    7f5c:	b8860613          	addi	a2,a2,-1144 # 11b88 <_data+0x44c>
    7f60:	03000593          	li	a1,48
    7f64:	05010513          	addi	a0,sp,80
    7f68:	41d080ef          	jal	10b84 <mini_snprintf>
    7f6c:	c34ff06f          	j	73a0 <render_game+0x164c>
    7f70:	000126b7          	lui	a3,0x12
    7f74:	00012637          	lui	a2,0x12
    7f78:	b7068693          	addi	a3,a3,-1168 # 11b70 <_data+0x434>
    7f7c:	b8860613          	addi	a2,a2,-1144 # 11b88 <_data+0x44c>
    7f80:	03000593          	li	a1,48
    7f84:	05010513          	addi	a0,sp,80
    7f88:	3fd080ef          	jal	10b84 <mini_snprintf>
    7f8c:	b9cff06f          	j	7328 <render_game+0x15d4>
    7f90:	000017b7          	lui	a5,0x1
    7f94:	10578793          	addi	a5,a5,261 # 1105 <memset+0x1>
    7f98:	01500713          	li	a4,21
    7f9c:	000a8693          	mv	a3,s5
    7fa0:	04400613          	li	a2,68
    7fa4:	00098593          	mv	a1,s3
    7fa8:	00040513          	mv	a0,s0
    7fac:	03112223          	sw	a7,36(sp)
    7fb0:	52d000ef          	jal	8cdc <gfx_fill_rect>
    7fb4:	02412883          	lw	a7,36(sp)
    7fb8:	dc1ff06f          	j	7d78 <render_game+0x2024>
    7fbc:	00100993          	li	s3,1
    7fc0:	d55fe06f          	j	6d14 <render_game+0xfc0>
    7fc4:	0000b7b7          	lui	a5,0xb
    7fc8:	000116b7          	lui	a3,0x11
    7fcc:	75c68693          	addi	a3,a3,1884 # 1175c <_data+0x20>
    7fd0:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    7fd4:	cc9ff06f          	j	7c9c <render_game+0x1f48>
    7fd8:	0ac88793          	addi	a5,a7,172
    7fdc:	02f12223          	sw	a5,36(sp)
    7fe0:	000127b7          	lui	a5,0x12
    7fe4:	02f12423          	sw	a5,40(sp)
    7fe8:	000127b7          	lui	a5,0x12
    7fec:	00004337          	lui	t1,0x4
    7ff0:	00009637          	lui	a2,0x9
    7ff4:	0000f837          	lui	a6,0xf
    7ff8:	02f12623          	sw	a5,44(sp)
    7ffc:	000137b7          	lui	a5,0x13
    8000:	2aa30313          	addi	t1,t1,682 # 42aa <__stack_size+0x2aa>
    8004:	51360613          	addi	a2,a2,1299 # 9513 <gfx_blit_alpha+0x3af>
    8008:	9ec78793          	addi	a5,a5,-1556 # 129ec <_data+0x12b0>
    800c:	77a80813          	addi	a6,a6,1914 # f77a <demo_init+0xf6>
    8010:	07100c13          	li	s8,113
    8014:	06e00b93          	li	s7,110
    8018:	00000b13          	li	s6,0
    801c:	04f12023          	sw	a5,64(sp)
    8020:	05112223          	sw	a7,68(sp)
    8024:	04612623          	sw	t1,76(sp)
    8028:	00060d13          	mv	s10,a2
    802c:	05012423          	sw	a6,72(sp)
    8030:	02412783          	lw	a5,36(sp)
    8034:	fb2b8593          	addi	a1,s7,-78
    8038:	0007aa03          	lw	s4,0(a5)
    803c:	0d300793          	li	a5,211
    8040:	0bb7cc63          	blt	a5,s11,80f8 <render_game+0x23a4>
    8044:	03412783          	lw	a5,52(sp)
    8048:	0b600713          	li	a4,182
    804c:	f4278793          	addi	a5,a5,-190
    8050:	12f77ce3          	bgeu	a4,a5,8988 <render_game+0x2c34>
    8054:	fff00993          	li	s3,-1
    8058:	04c12783          	lw	a5,76(sp)
    805c:	0b700713          	li	a4,183
    8060:	0b400693          	li	a3,180
    8064:	0be00613          	li	a2,190
    8068:	00040513          	mv	a0,s0
    806c:	9a0fb0ef          	jal	320c <panel>
    8070:	02812783          	lw	a5,40(sp)
    8074:	001b0c93          	addi	s9,s6,1
    8078:	000c8693          	mv	a3,s9
    807c:	9b078613          	addi	a2,a5,-1616
    8080:	08000593          	li	a1,128
    8084:	00048513          	mv	a0,s1
    8088:	03912823          	sw	s9,48(sp)
    808c:	2f9080ef          	jal	10b84 <mini_snprintf>
    8090:	fbeb8a93          	addi	s5,s7,-66
    8094:	000d0793          	mv	a5,s10
    8098:	000a8593          	mv	a1,s5
    809c:	00200713          	li	a4,2
    80a0:	00048693          	mv	a3,s1
    80a4:	0cb00613          	li	a2,203
    80a8:	00040513          	mv	a0,s0
    80ac:	57c010ef          	jal	9628 <gfx_text>
    80b0:	00400793          	li	a5,4
    80b4:	00cb8a93          	addi	s5,s7,12
    80b8:	1947e0e3          	bltu	a5,s4,8a38 <render_game+0x2ce4>
    80bc:	04012703          	lw	a4,64(sp)
    80c0:	002a1793          	slli	a5,s4,0x2
    80c4:	00e787b3          	add	a5,a5,a4
    80c8:	0007a783          	lw	a5,0(a5)
    80cc:	00078067          	jr	a5
    80d0:	000017b7          	lui	a5,0x1
    80d4:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    80d8:	01a00713          	li	a4,26
    80dc:	0e800693          	li	a3,232
    80e0:	05e00613          	li	a2,94
    80e4:	0cc00593          	li	a1,204
    80e8:	00040513          	mv	a0,s0
    80ec:	3f1000ef          	jal	8cdc <gfx_fill_rect>
    80f0:	02412883          	lw	a7,36(sp)
    80f4:	d75ff06f          	j	7e68 <render_game+0x2114>
    80f8:	0e500793          	li	a5,229
    80fc:	f5b7dce3          	bge	a5,s11,8054 <render_game+0x2300>
    8100:	19900793          	li	a5,409
    8104:	09b7d263          	bge	a5,s11,8188 <render_game+0x2434>
    8108:	e54d8793          	addi	a5,s11,-428
    810c:	0b300713          	li	a4,179
    8110:	f4f762e3          	bltu	a4,a5,8054 <render_game+0x2300>
    8114:	03412783          	lw	a5,52(sp)
    8118:	0b600713          	li	a4,182
    811c:	00200993          	li	s3,2
    8120:	f4278793          	addi	a5,a5,-190
    8124:	f2f768e3          	bltu	a4,a5,8054 <render_game+0x2300>
    8128:	f36998e3          	bne	s3,s6,8058 <render_game+0x2304>
    812c:	0000b7b7          	lui	a5,0xb
    8130:	00198813          	addi	a6,s3,1
    8134:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    8138:	0b700713          	li	a4,183
    813c:	0b400693          	li	a3,180
    8140:	0be00613          	li	a2,190
    8144:	00040513          	mv	a0,s0
    8148:	00080c93          	mv	s9,a6
    814c:	03012823          	sw	a6,48(sp)
    8150:	8bcfb0ef          	jal	320c <panel>
    8154:	02812783          	lw	a5,40(sp)
    8158:	000c8693          	mv	a3,s9
    815c:	08000593          	li	a1,128
    8160:	9b078613          	addi	a2,a5,-1616
    8164:	00048513          	mv	a0,s1
    8168:	21d080ef          	jal	10b84 <mini_snprintf>
    816c:	0000b7b7          	lui	a5,0xb
    8170:	fbeb8a93          	addi	s5,s7,-66
    8174:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    8178:	f21ff06f          	j	8098 <render_game+0x2344>
    817c:	00011a37          	lui	s4,0x11
    8180:	788a0a13          	addi	s4,s4,1928 # 11788 <_data+0x4c>
    8184:	b99ff06f          	j	7d1c <render_game+0x1fc8>
    8188:	03412783          	lw	a5,52(sp)
    818c:	0b600713          	li	a4,182
    8190:	00100993          	li	s3,1
    8194:	f4278793          	addi	a5,a5,-190
    8198:	f8f778e3          	bgeu	a4,a5,8128 <render_game+0x23d4>
    819c:	eb9ff06f          	j	8054 <render_game+0x2300>
    81a0:	000077b7          	lui	a5,0x7
    81a4:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    81a8:	01100713          	li	a4,17
    81ac:	00700693          	li	a3,7
    81b0:	0ec00613          	li	a2,236
    81b4:	000b0593          	mv	a1,s6
    81b8:	00040513          	mv	a0,s0
    81bc:	321000ef          	jal	8cdc <gfx_fill_rect>
    81c0:	00ec0813          	addi	a6,s8,14
    81c4:	000077b7          	lui	a5,0x7
    81c8:	00080593          	mv	a1,a6
    81cc:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    81d0:	01100713          	li	a4,17
    81d4:	00700693          	li	a3,7
    81d8:	0ec00613          	li	a2,236
    81dc:	00040513          	mv	a0,s0
    81e0:	03012223          	sw	a6,36(sp)
    81e4:	2f9000ef          	jal	8cdc <gfx_fill_rect>
    81e8:	000077b7          	lui	a5,0x7
    81ec:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    81f0:	00700713          	li	a4,7
    81f4:	01200693          	li	a3,18
    81f8:	0fb00613          	li	a2,251
    81fc:	000c0593          	mv	a1,s8
    8200:	00040513          	mv	a0,s0
    8204:	2d9000ef          	jal	8cdc <gfx_fill_rect>
    8208:	0000f7b7          	lui	a5,0xf
    820c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    8210:	00500713          	li	a4,5
    8214:	00700693          	li	a3,7
    8218:	0ec00613          	li	a2,236
    821c:	000b0593          	mv	a1,s6
    8220:	00040513          	mv	a0,s0
    8224:	2b9000ef          	jal	8cdc <gfx_fill_rect>
    8228:	02412803          	lw	a6,36(sp)
    822c:	0000f7b7          	lui	a5,0xf
    8230:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    8234:	00500713          	li	a4,5
    8238:	00700693          	li	a3,7
    823c:	0ec00613          	li	a2,236
    8240:	00080593          	mv	a1,a6
    8244:	00040513          	mv	a0,s0
    8248:	295000ef          	jal	8cdc <gfx_fill_rect>
    824c:	00098513          	mv	a0,s3
    8250:	028070ef          	jal	f278 <game_upgrade_name>
    8254:	00100593          	li	a1,1
    8258:	02a12223          	sw	a0,36(sp)
    825c:	694010ef          	jal	98f0 <gfx_text_width>
    8260:	01f55793          	srli	a5,a0,0x1f
    8264:	00a787b3          	add	a5,a5,a0
    8268:	02412683          	lw	a3,36(sp)
    826c:	4017d593          	srai	a1,a5,0x1
    8270:	03412783          	lw	a5,52(sp)
    8274:	00100713          	li	a4,1
    8278:	11700613          	li	a2,279
    827c:	40ba85b3          	sub	a1,s5,a1
    8280:	00040513          	mv	a0,s0
    8284:	3a4010ef          	jal	9628 <gfx_text>
    8288:	00000593          	li	a1,0
    828c:	00098513          	mv	a0,s3
    8290:	014070ef          	jal	f2a4 <game_upgrade_line>
    8294:	00100593          	li	a1,1
    8298:	02a12223          	sw	a0,36(sp)
    829c:	654010ef          	jal	98f0 <gfx_text_width>
    82a0:	01f55793          	srli	a5,a0,0x1f
    82a4:	00a787b3          	add	a5,a5,a0
    82a8:	02412683          	lw	a3,36(sp)
    82ac:	4017d593          	srai	a1,a5,0x1
    82b0:	03c12783          	lw	a5,60(sp)
    82b4:	00100713          	li	a4,1
    82b8:	12d00613          	li	a2,301
    82bc:	40ba85b3          	sub	a1,s5,a1
    82c0:	00040513          	mv	a0,s0
    82c4:	364010ef          	jal	9628 <gfx_text>
    82c8:	00100593          	li	a1,1
    82cc:	00098513          	mv	a0,s3
    82d0:	7d5060ef          	jal	f2a4 <game_upgrade_line>
    82d4:	00100593          	li	a1,1
    82d8:	00050993          	mv	s3,a0
    82dc:	614010ef          	jal	98f0 <gfx_text_width>
    82e0:	01f55793          	srli	a5,a0,0x1f
    82e4:	00a787b3          	add	a5,a5,a0
    82e8:	4017d593          	srai	a1,a5,0x1
    82ec:	00100713          	li	a4,1
    82f0:	000d8793          	mv	a5,s11
    82f4:	00098693          	mv	a3,s3
    82f8:	13e00613          	li	a2,318
    82fc:	40ba85b3          	sub	a1,s5,a1
    8300:	00040513          	mv	a0,s0
    8304:	324010ef          	jal	9628 <gfx_text>
    8308:	02c12783          	lw	a5,44(sp)
    830c:	000a0693          	mv	a3,s4
    8310:	08000593          	li	a1,128
    8314:	9b478613          	addi	a2,a5,-1612
    8318:	00048513          	mv	a0,s1
    831c:	069080ef          	jal	10b84 <mini_snprintf>
    8320:	00100593          	li	a1,1
    8324:	00048513          	mv	a0,s1
    8328:	5c8010ef          	jal	98f0 <gfx_text_width>
    832c:	01f55793          	srli	a5,a0,0x1f
    8330:	00a787b3          	add	a5,a5,a0
    8334:	4017d593          	srai	a1,a5,0x1
    8338:	04012783          	lw	a5,64(sp)
    833c:	00100713          	li	a4,1
    8340:	00048693          	mv	a3,s1
    8344:	15c00613          	li	a2,348
    8348:	40ba85b3          	sub	a1,s5,a1
    834c:	00040513          	mv	a0,s0
    8350:	2d8010ef          	jal	9628 <gfx_text>
    8354:	00300793          	li	a5,3
    8358:	004b8b93          	addi	s7,s7,4
    835c:	0c6c0c13          	addi	s8,s8,198
    8360:	0c6b0b13          	addi	s6,s6,198
    8364:	c6fa1663          	bne	s4,a5,77d0 <render_game+0x1a7c>
    8368:	04412883          	lw	a7,68(sp)
    836c:	000124b7          	lui	s1,0x12
    8370:	00100593          	li	a1,1
    8374:	9c048513          	addi	a0,s1,-1600 # 119c0 <_data+0x284>
    8378:	03112223          	sw	a7,36(sp)
    837c:	574010ef          	jal	98f0 <gfx_text_width>
    8380:	01f55793          	srli	a5,a0,0x1f
    8384:	00a787b3          	add	a5,a5,a0
    8388:	4017d593          	srai	a1,a5,0x1
    838c:	40b005b3          	neg	a1,a1
    8390:	000097b7          	lui	a5,0x9
    8394:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    8398:	00100713          	li	a4,1
    839c:	9c048693          	addi	a3,s1,-1600
    83a0:	17c00613          	li	a2,380
    83a4:	14058593          	addi	a1,a1,320
    83a8:	00040513          	mv	a0,s0
    83ac:	27c010ef          	jal	9628 <gfx_text>
    83b0:	02412883          	lw	a7,36(sp)
    83b4:	a71fe06f          	j	6e24 <render_game+0x10d0>
    83b8:	0000f7b7          	lui	a5,0xf
    83bc:	ffec0593          	addi	a1,s8,-2
    83c0:	00040513          	mv	a0,s0
    83c4:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    83c8:	00a00713          	li	a4,10
    83cc:	00900693          	li	a3,9
    83d0:	0ed00613          	li	a2,237
    83d4:	109000ef          	jal	8cdc <gfx_fill_rect>
    83d8:	0000f7b7          	lui	a5,0xf
    83dc:	00bc0593          	addi	a1,s8,11
    83e0:	00040513          	mv	a0,s0
    83e4:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    83e8:	00a00713          	li	a4,10
    83ec:	00900693          	li	a3,9
    83f0:	0ed00613          	li	a2,237
    83f4:	0e9000ef          	jal	8cdc <gfx_fill_rect>
    83f8:	0000f7b7          	lui	a5,0xf
    83fc:	fffc0593          	addi	a1,s8,-1
    8400:	00040513          	mv	a0,s0
    8404:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    8408:	00700713          	li	a4,7
    840c:	01400693          	li	a3,20
    8410:	0f500613          	li	a2,245
    8414:	0c9000ef          	jal	8cdc <gfx_fill_rect>
    8418:	0000f7b7          	lui	a5,0xf
    841c:	003c0593          	addi	a1,s8,3
    8420:	00040513          	mv	a0,s0
    8424:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    8428:	00400713          	li	a4,4
    842c:	00c00693          	li	a3,12
    8430:	0fc00613          	li	a2,252
    8434:	0a9000ef          	jal	8cdc <gfx_fill_rect>
    8438:	0000f7b7          	lui	a5,0xf
    843c:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    8440:	00300713          	li	a4,3
    8444:	00400693          	li	a3,4
    8448:	10000613          	li	a2,256
    844c:	007c0593          	addi	a1,s8,7
    8450:	00040513          	mv	a0,s0
    8454:	089000ef          	jal	8cdc <gfx_fill_rect>
    8458:	df5ff06f          	j	824c <render_game+0x24f8>
    845c:	000077b7          	lui	a5,0x7
    8460:	000c0593          	mv	a1,s8
    8464:	00040513          	mv	a0,s0
    8468:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    846c:	00e00713          	li	a4,14
    8470:	00900693          	li	a3,9
    8474:	0eb00613          	li	a2,235
    8478:	065000ef          	jal	8cdc <gfx_fill_rect>
    847c:	000077b7          	lui	a5,0x7
    8480:	000c0593          	mv	a1,s8
    8484:	00040513          	mv	a0,s0
    8488:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    848c:	00900713          	li	a4,9
    8490:	01500693          	li	a3,21
    8494:	0f700613          	li	a2,247
    8498:	045000ef          	jal	8cdc <gfx_fill_rect>
    849c:	0000f7b7          	lui	a5,0xf
    84a0:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    84a4:	00300713          	li	a4,3
    84a8:	01800693          	li	a3,24
    84ac:	0ff00613          	li	a2,255
    84b0:	000c0593          	mv	a1,s8
    84b4:	00040513          	mv	a0,s0
    84b8:	025000ef          	jal	8cdc <gfx_fill_rect>
    84bc:	d91ff06f          	j	824c <render_game+0x24f8>
    84c0:	0000f7b7          	lui	a5,0xf
    84c4:	001c0593          	addi	a1,s8,1
    84c8:	00040513          	mv	a0,s0
    84cc:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    84d0:	01400713          	li	a4,20
    84d4:	01000693          	li	a3,16
    84d8:	0eb00613          	li	a2,235
    84dc:	001000ef          	jal	8cdc <gfx_fill_rect>
    84e0:	0000f7b7          	lui	a5,0xf
    84e4:	000b0593          	mv	a1,s6
    84e8:	00040513          	mv	a0,s0
    84ec:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    84f0:	00a00713          	li	a4,10
    84f4:	01800693          	li	a3,24
    84f8:	0f000613          	li	a2,240
    84fc:	7e0000ef          	jal	8cdc <gfx_fill_rect>
    8500:	0000f7b7          	lui	a5,0xf
    8504:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    8508:	00e00713          	li	a4,14
    850c:	00600693          	li	a3,6
    8510:	0ee00613          	li	a2,238
    8514:	006c0593          	addi	a1,s8,6
    8518:	00040513          	mv	a0,s0
    851c:	7c0000ef          	jal	8cdc <gfx_fill_rect>
    8520:	d2dff06f          	j	824c <render_game+0x24f8>
    8524:	0000f7b7          	lui	a5,0xf
    8528:	000b0593          	mv	a1,s6
    852c:	00040513          	mv	a0,s0
    8530:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8534:	00800713          	li	a4,8
    8538:	00800693          	li	a3,8
    853c:	0eb00613          	li	a2,235
    8540:	79c000ef          	jal	8cdc <gfx_fill_rect>
    8544:	0000f7b7          	lui	a5,0xf
    8548:	005c0593          	addi	a1,s8,5
    854c:	00040513          	mv	a0,s0
    8550:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8554:	00800713          	li	a4,8
    8558:	00a00693          	li	a3,10
    855c:	0f100613          	li	a2,241
    8560:	77c000ef          	jal	8cdc <gfx_fill_rect>
    8564:	0000f7b7          	lui	a5,0xf
    8568:	00bc0593          	addi	a1,s8,11
    856c:	00040513          	mv	a0,s0
    8570:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8574:	00800713          	li	a4,8
    8578:	00800693          	li	a3,8
    857c:	0f800613          	li	a2,248
    8580:	75c000ef          	jal	8cdc <gfx_fill_rect>
    8584:	0000f7b7          	lui	a5,0xf
    8588:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    858c:	00500713          	li	a4,5
    8590:	00700693          	li	a3,7
    8594:	0fc00613          	li	a2,252
    8598:	010c0593          	addi	a1,s8,16
    859c:	00040513          	mv	a0,s0
    85a0:	73c000ef          	jal	8cdc <gfx_fill_rect>
    85a4:	ca9ff06f          	j	824c <render_game+0x24f8>
    85a8:	0000f7b7          	lui	a5,0xf
    85ac:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    85b0:	00a00713          	li	a4,10
    85b4:	00900693          	li	a3,9
    85b8:	0ed00613          	li	a2,237
    85bc:	001b8593          	addi	a1,s7,1
    85c0:	00040513          	mv	a0,s0
    85c4:	718000ef          	jal	8cdc <gfx_fill_rect>
    85c8:	0000f7b7          	lui	a5,0xf
    85cc:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    85d0:	00a00713          	li	a4,10
    85d4:	00900693          	li	a3,9
    85d8:	0ed00613          	li	a2,237
    85dc:	00eb8593          	addi	a1,s7,14
    85e0:	00040513          	mv	a0,s0
    85e4:	6f8000ef          	jal	8cdc <gfx_fill_rect>
    85e8:	0000f7b7          	lui	a5,0xf
    85ec:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    85f0:	00700713          	li	a4,7
    85f4:	01400693          	li	a3,20
    85f8:	0f500613          	li	a2,245
    85fc:	002b8593          	addi	a1,s7,2
    8600:	00040513          	mv	a0,s0
    8604:	6d8000ef          	jal	8cdc <gfx_fill_rect>
    8608:	0000f7b7          	lui	a5,0xf
    860c:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    8610:	00400713          	li	a4,4
    8614:	00c00693          	li	a3,12
    8618:	0fc00613          	li	a2,252
    861c:	006b8593          	addi	a1,s7,6
    8620:	00040513          	mv	a0,s0
    8624:	6b8000ef          	jal	8cdc <gfx_fill_rect>
    8628:	0000f7b7          	lui	a5,0xf
    862c:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    8630:	00300713          	li	a4,3
    8634:	00400693          	li	a3,4
    8638:	10000613          	li	a2,256
    863c:	00ab8593          	addi	a1,s7,10
    8640:	00040513          	mv	a0,s0
    8644:	698000ef          	jal	8cdc <gfx_fill_rect>
    8648:	000a0513          	mv	a0,s4
    864c:	42d060ef          	jal	f278 <game_upgrade_name>
    8650:	00100593          	li	a1,1
    8654:	02a12e23          	sw	a0,60(sp)
    8658:	298010ef          	jal	98f0 <gfx_text_width>
    865c:	01f55793          	srli	a5,a0,0x1f
    8660:	00a787b3          	add	a5,a5,a0
    8664:	03c12683          	lw	a3,60(sp)
    8668:	4017d593          	srai	a1,a5,0x1
    866c:	04812783          	lw	a5,72(sp)
    8670:	00100713          	li	a4,1
    8674:	11700613          	li	a2,279
    8678:	40ba85b3          	sub	a1,s5,a1
    867c:	00040513          	mv	a0,s0
    8680:	7a9000ef          	jal	9628 <gfx_text>
    8684:	00000593          	li	a1,0
    8688:	000a0513          	mv	a0,s4
    868c:	419060ef          	jal	f2a4 <game_upgrade_line>
    8690:	00100593          	li	a1,1
    8694:	02a12e23          	sw	a0,60(sp)
    8698:	258010ef          	jal	98f0 <gfx_text_width>
    869c:	01f55793          	srli	a5,a0,0x1f
    86a0:	03c12683          	lw	a3,60(sp)
    86a4:	00a787b3          	add	a5,a5,a0
    86a8:	4017d593          	srai	a1,a5,0x1
    86ac:	0000b7b7          	lui	a5,0xb
    86b0:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    86b4:	00100713          	li	a4,1
    86b8:	12d00613          	li	a2,301
    86bc:	40ba85b3          	sub	a1,s5,a1
    86c0:	00040513          	mv	a0,s0
    86c4:	765000ef          	jal	9628 <gfx_text>
    86c8:	00100593          	li	a1,1
    86cc:	000a0513          	mv	a0,s4
    86d0:	3d5060ef          	jal	f2a4 <game_upgrade_line>
    86d4:	00100593          	li	a1,1
    86d8:	00050a13          	mv	s4,a0
    86dc:	214010ef          	jal	98f0 <gfx_text_width>
    86e0:	01f55793          	srli	a5,a0,0x1f
    86e4:	00a787b3          	add	a5,a5,a0
    86e8:	4017d593          	srai	a1,a5,0x1
    86ec:	00100713          	li	a4,1
    86f0:	000d0793          	mv	a5,s10
    86f4:	000a0693          	mv	a3,s4
    86f8:	13e00613          	li	a2,318
    86fc:	40ba85b3          	sub	a1,s5,a1
    8700:	00040513          	mv	a0,s0
    8704:	725000ef          	jal	9628 <gfx_text>
    8708:	02c12783          	lw	a5,44(sp)
    870c:	03012683          	lw	a3,48(sp)
    8710:	08000593          	li	a1,128
    8714:	9b478613          	addi	a2,a5,-1612
    8718:	00048513          	mv	a0,s1
    871c:	468080ef          	jal	10b84 <mini_snprintf>
    8720:	00100593          	li	a1,1
    8724:	00048513          	mv	a0,s1
    8728:	27698463          	beq	s3,s6,8990 <render_game+0x2c3c>
    872c:	1c4010ef          	jal	98f0 <gfx_text_width>
    8730:	01f55793          	srli	a5,a0,0x1f
    8734:	00a787b3          	add	a5,a5,a0
    8738:	4017d593          	srai	a1,a5,0x1
    873c:	0000f7b7          	lui	a5,0xf
    8740:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8744:	00100713          	li	a4,1
    8748:	00048693          	mv	a3,s1
    874c:	15c00613          	li	a2,348
    8750:	40ba85b3          	sub	a1,s5,a1
    8754:	00040513          	mv	a0,s0
    8758:	6d1000ef          	jal	9628 <gfx_text>
    875c:	00300793          	li	a5,3
    8760:	000c8b13          	mv	s6,s9
    8764:	c0fc82e3          	beq	s9,a5,8368 <render_game+0x2614>
    8768:	02412783          	lw	a5,36(sp)
    876c:	0c6b8b93          	addi	s7,s7,198
    8770:	0c6c0c13          	addi	s8,s8,198
    8774:	00478793          	addi	a5,a5,4
    8778:	02f12223          	sw	a5,36(sp)
    877c:	8b5ff06f          	j	8030 <render_game+0x22dc>
    8780:	000077b7          	lui	a5,0x7
    8784:	000c0593          	mv	a1,s8
    8788:	00040513          	mv	a0,s0
    878c:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    8790:	00e00713          	li	a4,14
    8794:	00900693          	li	a3,9
    8798:	0eb00613          	li	a2,235
    879c:	540000ef          	jal	8cdc <gfx_fill_rect>
    87a0:	000077b7          	lui	a5,0x7
    87a4:	000c0593          	mv	a1,s8
    87a8:	00040513          	mv	a0,s0
    87ac:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    87b0:	00900713          	li	a4,9
    87b4:	01500693          	li	a3,21
    87b8:	0f700613          	li	a2,247
    87bc:	520000ef          	jal	8cdc <gfx_fill_rect>
    87c0:	0000f7b7          	lui	a5,0xf
    87c4:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    87c8:	00300713          	li	a4,3
    87cc:	01800693          	li	a3,24
    87d0:	0ff00613          	li	a2,255
    87d4:	000c0593          	mv	a1,s8
    87d8:	00040513          	mv	a0,s0
    87dc:	500000ef          	jal	8cdc <gfx_fill_rect>
    87e0:	e69ff06f          	j	8648 <render_game+0x28f4>
    87e4:	0000f7b7          	lui	a5,0xf
    87e8:	004b8593          	addi	a1,s7,4
    87ec:	00040513          	mv	a0,s0
    87f0:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    87f4:	01400713          	li	a4,20
    87f8:	01000693          	li	a3,16
    87fc:	0eb00613          	li	a2,235
    8800:	4dc000ef          	jal	8cdc <gfx_fill_rect>
    8804:	0000f7b7          	lui	a5,0xf
    8808:	000b8593          	mv	a1,s7
    880c:	00040513          	mv	a0,s0
    8810:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8814:	00a00713          	li	a4,10
    8818:	01800693          	li	a3,24
    881c:	0f000613          	li	a2,240
    8820:	4bc000ef          	jal	8cdc <gfx_fill_rect>
    8824:	0000f7b7          	lui	a5,0xf
    8828:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    882c:	00e00713          	li	a4,14
    8830:	00600693          	li	a3,6
    8834:	0ee00613          	li	a2,238
    8838:	009b8593          	addi	a1,s7,9
    883c:	00040513          	mv	a0,s0
    8840:	49c000ef          	jal	8cdc <gfx_fill_rect>
    8844:	e05ff06f          	j	8648 <render_game+0x28f4>
    8848:	0000f7b7          	lui	a5,0xf
    884c:	000b8593          	mv	a1,s7
    8850:	00040513          	mv	a0,s0
    8854:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8858:	00800713          	li	a4,8
    885c:	00800693          	li	a3,8
    8860:	0eb00613          	li	a2,235
    8864:	478000ef          	jal	8cdc <gfx_fill_rect>
    8868:	0000f7b7          	lui	a5,0xf
    886c:	008b8593          	addi	a1,s7,8
    8870:	00040513          	mv	a0,s0
    8874:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8878:	00800713          	li	a4,8
    887c:	00a00693          	li	a3,10
    8880:	0f100613          	li	a2,241
    8884:	458000ef          	jal	8cdc <gfx_fill_rect>
    8888:	0000f7b7          	lui	a5,0xf
    888c:	00eb8593          	addi	a1,s7,14
    8890:	00040513          	mv	a0,s0
    8894:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    8898:	00800713          	li	a4,8
    889c:	00800693          	li	a3,8
    88a0:	0f800613          	li	a2,248
    88a4:	438000ef          	jal	8cdc <gfx_fill_rect>
    88a8:	0000f7b7          	lui	a5,0xf
    88ac:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    88b0:	00500713          	li	a4,5
    88b4:	00700693          	li	a3,7
    88b8:	0fc00613          	li	a2,252
    88bc:	013b8593          	addi	a1,s7,19
    88c0:	00040513          	mv	a0,s0
    88c4:	418000ef          	jal	8cdc <gfx_fill_rect>
    88c8:	d81ff06f          	j	8648 <render_game+0x28f4>
    88cc:	000077b7          	lui	a5,0x7
    88d0:	000b8593          	mv	a1,s7
    88d4:	00040513          	mv	a0,s0
    88d8:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    88dc:	01100713          	li	a4,17
    88e0:	00700693          	li	a3,7
    88e4:	0ec00613          	li	a2,236
    88e8:	3f4000ef          	jal	8cdc <gfx_fill_rect>
    88ec:	011b8813          	addi	a6,s7,17
    88f0:	000077b7          	lui	a5,0x7
    88f4:	00080593          	mv	a1,a6
    88f8:	00040513          	mv	a0,s0
    88fc:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    8900:	01100713          	li	a4,17
    8904:	00700693          	li	a3,7
    8908:	0ec00613          	li	a2,236
    890c:	03012e23          	sw	a6,60(sp)
    8910:	3cc000ef          	jal	8cdc <gfx_fill_rect>
    8914:	000077b7          	lui	a5,0x7
    8918:	000c0593          	mv	a1,s8
    891c:	00040513          	mv	a0,s0
    8920:	69878793          	addi	a5,a5,1688 # 7698 <render_game+0x1944>
    8924:	00700713          	li	a4,7
    8928:	01200693          	li	a3,18
    892c:	0fb00613          	li	a2,251
    8930:	3ac000ef          	jal	8cdc <gfx_fill_rect>
    8934:	0000f7b7          	lui	a5,0xf
    8938:	000b8593          	mv	a1,s7
    893c:	00040513          	mv	a0,s0
    8940:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    8944:	00500713          	li	a4,5
    8948:	00700693          	li	a3,7
    894c:	0ec00613          	li	a2,236
    8950:	38c000ef          	jal	8cdc <gfx_fill_rect>
    8954:	03c12803          	lw	a6,60(sp)
    8958:	0000f7b7          	lui	a5,0xf
    895c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    8960:	00500713          	li	a4,5
    8964:	00700693          	li	a3,7
    8968:	0ec00613          	li	a2,236
    896c:	00080593          	mv	a1,a6
    8970:	00040513          	mv	a0,s0
    8974:	368000ef          	jal	8cdc <gfx_fill_rect>
    8978:	cd1ff06f          	j	8648 <render_game+0x28f4>
    897c:	00011a37          	lui	s4,0x11
    8980:	770a0a13          	addi	s4,s4,1904 # 11770 <_data+0x34>
    8984:	b98ff06f          	j	7d1c <render_game+0x1fc8>
    8988:	00000993          	li	s3,0
    898c:	f9cff06f          	j	8128 <render_game+0x23d4>
    8990:	761000ef          	jal	98f0 <gfx_text_width>
    8994:	01f55793          	srli	a5,a0,0x1f
    8998:	00a787b3          	add	a5,a5,a0
    899c:	4017d593          	srai	a1,a5,0x1
    89a0:	0000b7b7          	lui	a5,0xb
    89a4:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    89a8:	00100713          	li	a4,1
    89ac:	00048693          	mv	a3,s1
    89b0:	15c00613          	li	a2,348
    89b4:	40ba85b3          	sub	a1,s5,a1
    89b8:	00040513          	mv	a0,s0
    89bc:	46d000ef          	jal	9628 <gfx_text>
    89c0:	02412703          	lw	a4,36(sp)
    89c4:	00300793          	li	a5,3
    89c8:	000c8b13          	mv	s6,s9
    89cc:	00470713          	addi	a4,a4,4
    89d0:	02e12223          	sw	a4,36(sp)
    89d4:	0c6b8b93          	addi	s7,s7,198
    89d8:	0c6c0c13          	addi	s8,s8,198
    89dc:	e4fc9a63          	bne	s9,a5,8030 <render_game+0x22dc>
    89e0:	989ff06f          	j	8368 <render_game+0x2614>
    89e4:	0000b6b7          	lui	a3,0xb
    89e8:	000c0593          	mv	a1,s8
    89ec:	00040513          	mv	a0,s0
    89f0:	70d68693          	addi	a3,a3,1805 # b70d <check_level+0x61>
    89f4:	0f200613          	li	a2,242
    89f8:	a08fa0ef          	jal	2c00 <sprout>
    89fc:	0000b6b7          	lui	a3,0xb
    8a00:	011c0593          	addi	a1,s8,17
    8a04:	00040513          	mv	a0,s0
    8a08:	70d68693          	addi	a3,a3,1805 # b70d <check_level+0x61>
    8a0c:	0f200613          	li	a2,242
    8a10:	9f0fa0ef          	jal	2c00 <sprout>
    8a14:	000087b7          	lui	a5,0x8
    8a18:	36978793          	addi	a5,a5,873 # 8369 <render_game+0x2615>
    8a1c:	00400713          	li	a4,4
    8a20:	01e00693          	li	a3,30
    8a24:	0fd00613          	li	a2,253
    8a28:	ffac0593          	addi	a1,s8,-6
    8a2c:	00040513          	mv	a0,s0
    8a30:	2ac000ef          	jal	8cdc <gfx_fill_rect>
    8a34:	819ff06f          	j	824c <render_game+0x24f8>
    8a38:	0000b6b7          	lui	a3,0xb
    8a3c:	000c0593          	mv	a1,s8
    8a40:	00040513          	mv	a0,s0
    8a44:	70d68693          	addi	a3,a3,1805 # b70d <check_level+0x61>
    8a48:	0f200613          	li	a2,242
    8a4c:	9b4fa0ef          	jal	2c00 <sprout>
    8a50:	0000b6b7          	lui	a3,0xb
    8a54:	014b8593          	addi	a1,s7,20
    8a58:	00040513          	mv	a0,s0
    8a5c:	70d68693          	addi	a3,a3,1805 # b70d <check_level+0x61>
    8a60:	0f200613          	li	a2,242
    8a64:	99cfa0ef          	jal	2c00 <sprout>
    8a68:	000087b7          	lui	a5,0x8
    8a6c:	36978793          	addi	a5,a5,873 # 8369 <render_game+0x2615>
    8a70:	00400713          	li	a4,4
    8a74:	01e00693          	li	a3,30
    8a78:	0fd00613          	li	a2,253
    8a7c:	ffdb8593          	addi	a1,s7,-3
    8a80:	00040513          	mv	a0,s0
    8a84:	258000ef          	jal	8cdc <gfx_fill_rect>
    8a88:	bc1ff06f          	j	8648 <render_game+0x28f4>

00008a8c <gfx_fill_rect.part.0>:
    8a8c:	ff010113          	addi	sp,sp,-16
    8a90:	00052883          	lw	a7,0(a0)
    8a94:	00812423          	sw	s0,8(sp)
    8a98:	00050413          	mv	s0,a0
    8a9c:	fff5c513          	not	a0,a1
    8aa0:	41f55513          	srai	a0,a0,0x1f
    8aa4:	00b686b3          	add	a3,a3,a1
    8aa8:	00112623          	sw	ra,12(sp)
    8aac:	00a5f5b3          	and	a1,a1,a0
    8ab0:	00d8d463          	bge	a7,a3,8ab8 <gfx_fill_rect.part.0+0x2c>
    8ab4:	00088693          	mv	a3,a7
    8ab8:	06d5da63          	bge	a1,a3,8b2c <gfx_fill_rect.part.0+0xa0>
    8abc:	00442803          	lw	a6,4(s0)
    8ac0:	fff64513          	not	a0,a2
    8ac4:	41f55513          	srai	a0,a0,0x1f
    8ac8:	00c70f33          	add	t5,a4,a2
    8acc:	00a67633          	and	a2,a2,a0
    8ad0:	07e84663          	blt	a6,t5,8b3c <gfx_fill_rect.part.0+0xb0>
    8ad4:	05e65c63          	bge	a2,t5,8b2c <gfx_fill_rect.part.0+0xa0>
    8ad8:	01442703          	lw	a4,20(s0)
    8adc:	00c42503          	lw	a0,12(s0)
    8ae0:	00912223          	sw	s1,4(sp)
    8ae4:	01212023          	sw	s2,0(sp)
    8ae8:	00170713          	addi	a4,a4,1
    8aec:	00e42a23          	sw	a4,20(s0)
    8af0:	40b684b3          	sub	s1,a3,a1
    8af4:	40cf0933          	sub	s2,t5,a2
    8af8:	04050863          	beqz	a0,8b48 <gfx_fill_rect.part.0+0xbc>
    8afc:	00452803          	lw	a6,4(a0)
    8b00:	00080a63          	beqz	a6,8b14 <gfx_fill_rect.part.0+0x88>
    8b04:	01042503          	lw	a0,16(s0)
    8b08:	00090713          	mv	a4,s2
    8b0c:	00048693          	mv	a3,s1
    8b10:	000800e7          	jalr	a6
    8b14:	03248933          	mul	s2,s1,s2
    8b18:	01c42783          	lw	a5,28(s0)
    8b1c:	00412483          	lw	s1,4(sp)
    8b20:	012787b3          	add	a5,a5,s2
    8b24:	00012903          	lw	s2,0(sp)
    8b28:	00f42e23          	sw	a5,28(s0)
    8b2c:	00c12083          	lw	ra,12(sp)
    8b30:	00812403          	lw	s0,8(sp)
    8b34:	01010113          	addi	sp,sp,16
    8b38:	00008067          	ret
    8b3c:	00080f13          	mv	t5,a6
    8b40:	f9e64ce3          	blt	a2,t5,8ad8 <gfx_fill_rect.part.0+0x4c>
    8b44:	fe9ff06f          	j	8b2c <gfx_fill_rect.part.0+0xa0>
    8b48:	031606b3          	mul	a3,a2,a7
    8b4c:	00842e83          	lw	t4,8(s0)
    8b50:	01079813          	slli	a6,a5,0x10
    8b54:	00f80833          	add	a6,a6,a5
    8b58:	00189713          	slli	a4,a7,0x1
    8b5c:	00100513          	li	a0,1
    8b60:	00500393          	li	t2,5
    8b64:	fff48293          	addi	t0,s1,-1
    8b68:	00700f93          	li	t6,7
    8b6c:	00b686b3          	add	a3,a3,a1
    8b70:	00169693          	slli	a3,a3,0x1
    8b74:	00de8eb3          	add	t4,t4,a3
    8b78:	002ef893          	andi	a7,t4,2
    8b7c:	000e8593          	mv	a1,t4
    8b80:	0a088c63          	beqz	a7,8c38 <gfx_fill_rect.part.0+0x1ac>
    8b84:	00fe9023          	sh	a5,0(t4)
    8b88:	002e8893          	addi	a7,t4,2
    8b8c:	00028313          	mv	t1,t0
    8b90:	025fde63          	bge	t6,t0,8bcc <gfx_fill_rect.part.0+0x140>
    8b94:	00088593          	mv	a1,a7
    8b98:	ff830313          	addi	t1,t1,-8
    8b9c:	00335e13          	srli	t3,t1,0x3
    8ba0:	001e0893          	addi	a7,t3,1
    8ba4:	00489893          	slli	a7,a7,0x4
    8ba8:	011588b3          	add	a7,a1,a7
    8bac:	0105a023          	sw	a6,0(a1)
    8bb0:	0105a223          	sw	a6,4(a1)
    8bb4:	0105a423          	sw	a6,8(a1)
    8bb8:	0105a623          	sw	a6,12(a1)
    8bbc:	01058593          	addi	a1,a1,16
    8bc0:	ff1596e3          	bne	a1,a7,8bac <gfx_fill_rect.part.0+0x120>
    8bc4:	003e1e13          	slli	t3,t3,0x3
    8bc8:	41c30333          	sub	t1,t1,t3
    8bcc:	02655a63          	bge	a0,t1,8c00 <gfx_fill_rect.part.0+0x174>
    8bd0:	0108a023          	sw	a6,0(a7)
    8bd4:	ffe30693          	addi	a3,t1,-2
    8bd8:	00d55863          	bge	a0,a3,8be8 <gfx_fill_rect.part.0+0x15c>
    8bdc:	0108a223          	sw	a6,4(a7)
    8be0:	0063d463          	bge	t2,t1,8be8 <gfx_fill_rect.part.0+0x15c>
    8be4:	0108a423          	sw	a6,8(a7)
    8be8:	0016d313          	srli	t1,a3,0x1
    8bec:	00130593          	addi	a1,t1,1
    8bf0:	00259593          	slli	a1,a1,0x2
    8bf4:	00131313          	slli	t1,t1,0x1
    8bf8:	00b888b3          	add	a7,a7,a1
    8bfc:	40668333          	sub	t1,a3,t1
    8c00:	04031663          	bnez	t1,8c4c <gfx_fill_rect.part.0+0x1c0>
    8c04:	00160613          	addi	a2,a2,1
    8c08:	00ee8eb3          	add	t4,t4,a4
    8c0c:	f6cf16e3          	bne	t5,a2,8b78 <gfx_fill_rect.part.0+0xec>
    8c10:	032486b3          	mul	a3,s1,s2
    8c14:	01c42783          	lw	a5,28(s0)
    8c18:	00c12083          	lw	ra,12(sp)
    8c1c:	00412483          	lw	s1,4(sp)
    8c20:	00012903          	lw	s2,0(sp)
    8c24:	00d787b3          	add	a5,a5,a3
    8c28:	00f42e23          	sw	a5,28(s0)
    8c2c:	00812403          	lw	s0,8(sp)
    8c30:	01010113          	addi	sp,sp,16
    8c34:	00008067          	ret
    8c38:	00048313          	mv	t1,s1
    8c3c:	f49fcee3          	blt	t6,s1,8b98 <gfx_fill_rect.part.0+0x10c>
    8c40:	00100693          	li	a3,1
    8c44:	000e8893          	mv	a7,t4
    8c48:	f8d494e3          	bne	s1,a3,8bd0 <gfx_fill_rect.part.0+0x144>
    8c4c:	00f89023          	sh	a5,0(a7)
    8c50:	fb5ff06f          	j	8c04 <gfx_fill_rect.part.0+0x178>

00008c54 <gfx_begin>:
    8c54:	00b52423          	sw	a1,8(a0)
    8c58:	00c52023          	sw	a2,0(a0)
    8c5c:	00d52223          	sw	a3,4(a0)
    8c60:	00052623          	sw	zero,12(a0)
    8c64:	00052823          	sw	zero,16(a0)
    8c68:	00052a23          	sw	zero,20(a0)
    8c6c:	00052c23          	sw	zero,24(a0)
    8c70:	00052e23          	sw	zero,28(a0)
    8c74:	02052023          	sw	zero,32(a0)
    8c78:	02052223          	sw	zero,36(a0)
    8c7c:	00008067          	ret

00008c80 <gfx_begin_backend>:
    8c80:	00052423          	sw	zero,8(a0)
    8c84:	00d52023          	sw	a3,0(a0)
    8c88:	00e52223          	sw	a4,4(a0)
    8c8c:	00b52623          	sw	a1,12(a0)
    8c90:	00c52823          	sw	a2,16(a0)
    8c94:	00052a23          	sw	zero,20(a0)
    8c98:	00052c23          	sw	zero,24(a0)
    8c9c:	00052e23          	sw	zero,28(a0)
    8ca0:	02052023          	sw	zero,32(a0)
    8ca4:	02052223          	sw	zero,36(a0)
    8ca8:	00058a63          	beqz	a1,8cbc <gfx_begin_backend+0x3c>
    8cac:	0005a783          	lw	a5,0(a1)
    8cb0:	00078663          	beqz	a5,8cbc <gfx_begin_backend+0x3c>
    8cb4:	00060513          	mv	a0,a2
    8cb8:	00078067          	jr	a5
    8cbc:	00008067          	ret

00008cc0 <gfx_end>:
    8cc0:	00c52783          	lw	a5,12(a0)
    8cc4:	00078a63          	beqz	a5,8cd8 <gfx_end+0x18>
    8cc8:	00c7a783          	lw	a5,12(a5)
    8ccc:	00078663          	beqz	a5,8cd8 <gfx_end+0x18>
    8cd0:	01052503          	lw	a0,16(a0)
    8cd4:	00078067          	jr	a5
    8cd8:	00008067          	ret

00008cdc <gfx_fill_rect>:
    8cdc:	00d05663          	blez	a3,8ce8 <gfx_fill_rect+0xc>
    8ce0:	00e05463          	blez	a4,8ce8 <gfx_fill_rect+0xc>
    8ce4:	da9ff06f          	j	8a8c <gfx_fill_rect.part.0>
    8ce8:	00008067          	ret

00008cec <gfx_blit>:
    8cec:	fc010113          	addi	sp,sp,-64
    8cf0:	03412423          	sw	s4,40(sp)
    8cf4:	00068a13          	mv	s4,a3
    8cf8:	0006a683          	lw	a3,0(a3)
    8cfc:	02812c23          	sw	s0,56(sp)
    8d00:	02912a23          	sw	s1,52(sp)
    8d04:	03512223          	sw	s5,36(sp)
    8d08:	03612023          	sw	s6,32(sp)
    8d0c:	01712e23          	sw	s7,28(sp)
    8d10:	01a12823          	sw	s10,16(sp)
    8d14:	00058a93          	mv	s5,a1
    8d18:	02112e23          	sw	ra,60(sp)
    8d1c:	004a2483          	lw	s1,4(s4)
    8d20:	00050413          	mv	s0,a0
    8d24:	00070d13          	mv	s10,a4
    8d28:	00b685b3          	add	a1,a3,a1
    8d2c:	00068b13          	mv	s6,a3
    8d30:	00000b93          	li	s7,0
    8d34:	000ad863          	bgez	s5,8d44 <gfx_blit+0x58>
    8d38:	41500bb3          	neg	s7,s5
    8d3c:	00058b13          	mv	s6,a1
    8d40:	00000a93          	li	s5,0
    8d44:	00c48533          	add	a0,s1,a2
    8d48:	00000793          	li	a5,0
    8d4c:	00065863          	bgez	a2,8d5c <gfx_blit+0x70>
    8d50:	40c007b3          	neg	a5,a2
    8d54:	00050493          	mv	s1,a0
    8d58:	00000613          	li	a2,0
    8d5c:	00042703          	lw	a4,0(s0)
    8d60:	00b75463          	bge	a4,a1,8d68 <gfx_blit+0x7c>
    8d64:	41570b33          	sub	s6,a4,s5
    8d68:	00442583          	lw	a1,4(s0)
    8d6c:	00a5d463          	bge	a1,a0,8d74 <gfx_blit+0x88>
    8d70:	40c584b3          	sub	s1,a1,a2
    8d74:	05605a63          	blez	s6,8dc8 <gfx_blit+0xdc>
    8d78:	04905863          	blez	s1,8dc8 <gfx_blit+0xdc>
    8d7c:	01842583          	lw	a1,24(s0)
    8d80:	00c42503          	lw	a0,12(s0)
    8d84:	00158593          	addi	a1,a1,1
    8d88:	00b42c23          	sw	a1,24(s0)
    8d8c:	06050263          	beqz	a0,8df0 <gfx_blit+0x104>
    8d90:	00852303          	lw	t1,8(a0)
    8d94:	02030263          	beqz	t1,8db8 <gfx_blit+0xcc>
    8d98:	01042503          	lw	a0,16(s0)
    8d9c:	00048893          	mv	a7,s1
    8da0:	01a12023          	sw	s10,0(sp)
    8da4:	000b0813          	mv	a6,s6
    8da8:	000b8713          	mv	a4,s7
    8dac:	000a0693          	mv	a3,s4
    8db0:	000a8593          	mv	a1,s5
    8db4:	000300e7          	jalr	t1
    8db8:	029b04b3          	mul	s1,s6,s1
    8dbc:	01c42783          	lw	a5,28(s0)
    8dc0:	009787b3          	add	a5,a5,s1
    8dc4:	00f42e23          	sw	a5,28(s0)
    8dc8:	03c12083          	lw	ra,60(sp)
    8dcc:	03812403          	lw	s0,56(sp)
    8dd0:	03412483          	lw	s1,52(sp)
    8dd4:	02812a03          	lw	s4,40(sp)
    8dd8:	02412a83          	lw	s5,36(sp)
    8ddc:	02012b03          	lw	s6,32(sp)
    8de0:	01c12b83          	lw	s7,28(sp)
    8de4:	01012d03          	lw	s10,16(sp)
    8de8:	04010113          	addi	sp,sp,64
    8dec:	00008067          	ret
    8df0:	01912a23          	sw	s9,20(sp)
    8df4:	00010cb7          	lui	s9,0x10
    8df8:	03212823          	sw	s2,48(sp)
    8dfc:	03312623          	sw	s3,44(sp)
    8e00:	01812c23          	sw	s8,24(sp)
    8e04:	001b1993          	slli	s3,s6,0x1
    8e08:	00060c13          	mv	s8,a2
    8e0c:	00c484b3          	add	s1,s1,a2
    8e10:	40c78933          	sub	s2,a5,a2
    8e14:	81fc8c93          	addi	s9,s9,-2017 # f81f <demo_init+0x19b>
    8e18:	0280006f          	j	8e40 <gfx_blit+0x154>
    8e1c:	00098613          	mv	a2,s3
    8e20:	bc0f80ef          	jal	11e0 <memcpy>
    8e24:	01c42783          	lw	a5,28(s0)
    8e28:	001c0c13          	addi	s8,s8,1
    8e2c:	016787b3          	add	a5,a5,s6
    8e30:	00f42e23          	sw	a5,28(s0)
    8e34:	07848663          	beq	s1,s8,8ea0 <gfx_blit+0x1b4>
    8e38:	00042703          	lw	a4,0(s0)
    8e3c:	000a2683          	lw	a3,0(s4)
    8e40:	018907b3          	add	a5,s2,s8
    8e44:	02ec0733          	mul	a4,s8,a4
    8e48:	00842503          	lw	a0,8(s0)
    8e4c:	008a2583          	lw	a1,8(s4)
    8e50:	02d787b3          	mul	a5,a5,a3
    8e54:	01570733          	add	a4,a4,s5
    8e58:	00171713          	slli	a4,a4,0x1
    8e5c:	00e50533          	add	a0,a0,a4
    8e60:	017787b3          	add	a5,a5,s7
    8e64:	00179793          	slli	a5,a5,0x1
    8e68:	00f585b3          	add	a1,a1,a5
    8e6c:	fa0d08e3          	beqz	s10,8e1c <gfx_blit+0x130>
    8e70:	013586b3          	add	a3,a1,s3
    8e74:	0005d783          	lhu	a5,0(a1)
    8e78:	00258593          	addi	a1,a1,2
    8e7c:	01978a63          	beq	a5,s9,8e90 <gfx_blit+0x1a4>
    8e80:	01c42703          	lw	a4,28(s0)
    8e84:	00f51023          	sh	a5,0(a0)
    8e88:	00170793          	addi	a5,a4,1
    8e8c:	00f42e23          	sw	a5,28(s0)
    8e90:	00250513          	addi	a0,a0,2
    8e94:	fed590e3          	bne	a1,a3,8e74 <gfx_blit+0x188>
    8e98:	001c0c13          	addi	s8,s8,1
    8e9c:	f9849ee3          	bne	s1,s8,8e38 <gfx_blit+0x14c>
    8ea0:	03012903          	lw	s2,48(sp)
    8ea4:	02c12983          	lw	s3,44(sp)
    8ea8:	01812c03          	lw	s8,24(sp)
    8eac:	01412c83          	lw	s9,20(sp)
    8eb0:	f19ff06f          	j	8dc8 <gfx_blit+0xdc>

00008eb4 <gfx_fill_alpha>:
    8eb4:	2b005063          	blez	a6,9154 <gfx_fill_alpha+0x2a0>
    8eb8:	0fe00893          	li	a7,254
    8ebc:	0b08ca63          	blt	a7,a6,8f70 <gfx_fill_alpha+0xbc>
    8ec0:	28d05a63          	blez	a3,9154 <gfx_fill_alpha+0x2a0>
    8ec4:	28e05863          	blez	a4,9154 <gfx_fill_alpha+0x2a0>
    8ec8:	00581893          	slli	a7,a6,0x5
    8ecc:	010888b3          	add	a7,a7,a6
    8ed0:	0088d893          	srli	a7,a7,0x8
    8ed4:	28088063          	beqz	a7,9154 <gfx_fill_alpha+0x2a0>
    8ed8:	02000313          	li	t1,32
    8edc:	08688e63          	beq	a7,t1,8f78 <gfx_fill_alpha+0xc4>
    8ee0:	fe010113          	addi	sp,sp,-32
    8ee4:	00812e23          	sw	s0,28(sp)
    8ee8:	00052403          	lw	s0,0(a0)
    8eec:	fff5c313          	not	t1,a1
    8ef0:	41f35313          	srai	t1,t1,0x1f
    8ef4:	00b686b3          	add	a3,a3,a1
    8ef8:	0065f5b3          	and	a1,a1,t1
    8efc:	1ed44e63          	blt	s0,a3,90f8 <gfx_fill_alpha+0x244>
    8f00:	00452e03          	lw	t3,4(a0)
    8f04:	1ed5d463          	bge	a1,a3,90ec <gfx_fill_alpha+0x238>
    8f08:	fff64313          	not	t1,a2
    8f0c:	41f35313          	srai	t1,t1,0x1f
    8f10:	00c70f33          	add	t5,a4,a2
    8f14:	00667633          	and	a2,a2,t1
    8f18:	1fee4463          	blt	t3,t5,9100 <gfx_fill_alpha+0x24c>
    8f1c:	1de65863          	bge	a2,t5,90ec <gfx_fill_alpha+0x238>
    8f20:	40b686b3          	sub	a3,a3,a1
    8f24:	40cf0733          	sub	a4,t5,a2
    8f28:	02d70fb3          	mul	t6,a4,a3
    8f2c:	02052e83          	lw	t4,32(a0)
    8f30:	02452e03          	lw	t3,36(a0)
    8f34:	01c52303          	lw	t1,28(a0)
    8f38:	00c52283          	lw	t0,12(a0)
    8f3c:	001e8e93          	addi	t4,t4,1
    8f40:	03d52023          	sw	t4,32(a0)
    8f44:	01fe0e33          	add	t3,t3,t6
    8f48:	01f30333          	add	t1,t1,t6
    8f4c:	03c52223          	sw	t3,36(a0)
    8f50:	00652e23          	sw	t1,28(a0)
    8f54:	02028463          	beqz	t0,8f7c <gfx_fill_alpha+0xc8>
    8f58:	0102a883          	lw	a7,16(t0)
    8f5c:	18088863          	beqz	a7,90ec <gfx_fill_alpha+0x238>
    8f60:	01c12403          	lw	s0,28(sp)
    8f64:	01052503          	lw	a0,16(a0)
    8f68:	02010113          	addi	sp,sp,32
    8f6c:	00088067          	jr	a7
    8f70:	1ed05263          	blez	a3,9154 <gfx_fill_alpha+0x2a0>
    8f74:	1ee05263          	blez	a4,9158 <gfx_fill_alpha+0x2a4>
    8f78:	b15ff06f          	j	8a8c <gfx_fill_rect.part.0>
    8f7c:	02c40733          	mul	a4,s0,a2
    8f80:	00852383          	lw	t2,8(a0)
    8f84:	01079813          	slli	a6,a5,0x10
    8f88:	00f80833          	add	a6,a6,a5
    8f8c:	07e10537          	lui	a0,0x7e10
    8f90:	81f50513          	addi	a0,a0,-2017 # 7e0f81f <__freertos_irq_stack_top+0x7d2767f>
    8f94:	02008337          	lui	t1,0x2008
    8f98:	00010e37          	lui	t3,0x10
    8f9c:	01212a23          	sw	s2,20(sp)
    8fa0:	01312823          	sw	s3,16(sp)
    8fa4:	00b707b3          	add	a5,a4,a1
    8fa8:	00179793          	slli	a5,a5,0x1
    8fac:	00912c23          	sw	s1,24(sp)
    8fb0:	01412623          	sw	s4,12(sp)
    8fb4:	01512423          	sw	s5,8(sp)
    8fb8:	00a87833          	and	a6,a6,a0
    8fbc:	00141413          	slli	s0,s0,0x1
    8fc0:	00f383b3          	add	t2,t2,a5
    8fc4:	00100993          	li	s3,1
    8fc8:	01030313          	addi	t1,t1,16 # 2008010 <__freertos_irq_stack_top+0x1f1fe70>
    8fcc:	fffe0e13          	addi	t3,t3,-1 # ffff <mini_vsnprintf+0x113>
    8fd0:	ffff0fb7          	lui	t6,0xffff0
    8fd4:	fff68913          	addi	s2,a3,-1
    8fd8:	0023f793          	andi	a5,t2,2
    8fdc:	00038593          	mv	a1,t2
    8fe0:	12078663          	beqz	a5,910c <gfx_fill_alpha+0x258>
    8fe4:	0003d703          	lhu	a4,0(t2)
    8fe8:	00238593          	addi	a1,t2,2
    8fec:	00090293          	mv	t0,s2
    8ff0:	01071793          	slli	a5,a4,0x10
    8ff4:	00e787b3          	add	a5,a5,a4
    8ff8:	00a7f7b3          	and	a5,a5,a0
    8ffc:	40f80733          	sub	a4,a6,a5
    9000:	03170733          	mul	a4,a4,a7
    9004:	00579793          	slli	a5,a5,0x5
    9008:	006787b3          	add	a5,a5,t1
    900c:	00e787b3          	add	a5,a5,a4
    9010:	0057d793          	srli	a5,a5,0x5
    9014:	00a7f7b3          	and	a5,a5,a0
    9018:	0107d713          	srli	a4,a5,0x10
    901c:	00f767b3          	or	a5,a4,a5
    9020:	00f39023          	sh	a5,0(t2)
    9024:	1329dc63          	bge	s3,s2,915c <gfx_fill_alpha+0x2a8>
    9028:	ffe28493          	addi	s1,t0,-2
    902c:	0014d293          	srli	t0,s1,0x1
    9030:	00128e93          	addi	t4,t0,1
    9034:	002e9e93          	slli	t4,t4,0x2
    9038:	01d58eb3          	add	t4,a1,t4
    903c:	0005a783          	lw	a5,0(a1)
    9040:	00458593          	addi	a1,a1,4
    9044:	01c7f733          	and	a4,a5,t3
    9048:	01071a13          	slli	s4,a4,0x10
    904c:	0107da93          	srli	s5,a5,0x10
    9050:	00ea0a33          	add	s4,s4,a4
    9054:	01f7f7b3          	and	a5,a5,t6
    9058:	00faeab3          	or	s5,s5,a5
    905c:	00aa7a33          	and	s4,s4,a0
    9060:	00aafab3          	and	s5,s5,a0
    9064:	41480733          	sub	a4,a6,s4
    9068:	415807b3          	sub	a5,a6,s5
    906c:	03170733          	mul	a4,a4,a7
    9070:	005a1a13          	slli	s4,s4,0x5
    9074:	006a0a33          	add	s4,s4,t1
    9078:	005a9a93          	slli	s5,s5,0x5
    907c:	006a8ab3          	add	s5,s5,t1
    9080:	031787b3          	mul	a5,a5,a7
    9084:	01470733          	add	a4,a4,s4
    9088:	00575713          	srli	a4,a4,0x5
    908c:	00a77733          	and	a4,a4,a0
    9090:	01075a13          	srli	s4,a4,0x10
    9094:	00ea6733          	or	a4,s4,a4
    9098:	01c77733          	and	a4,a4,t3
    909c:	015787b3          	add	a5,a5,s5
    90a0:	0057d793          	srli	a5,a5,0x5
    90a4:	00a7f7b3          	and	a5,a5,a0
    90a8:	0107da13          	srli	s4,a5,0x10
    90ac:	00fa67b3          	or	a5,s4,a5
    90b0:	01079793          	slli	a5,a5,0x10
    90b4:	00f767b3          	or	a5,a4,a5
    90b8:	fef5ae23          	sw	a5,-4(a1)
    90bc:	f9d590e3          	bne	a1,t4,903c <gfx_fill_alpha+0x188>
    90c0:	00129293          	slli	t0,t0,0x1
    90c4:	405482b3          	sub	t0,s1,t0
    90c8:	04029863          	bnez	t0,9118 <gfx_fill_alpha+0x264>
    90cc:	00160613          	addi	a2,a2,1
    90d0:	008383b3          	add	t2,t2,s0
    90d4:	f1e612e3          	bne	a2,t5,8fd8 <gfx_fill_alpha+0x124>
    90d8:	01812483          	lw	s1,24(sp)
    90dc:	01412903          	lw	s2,20(sp)
    90e0:	01012983          	lw	s3,16(sp)
    90e4:	00c12a03          	lw	s4,12(sp)
    90e8:	00812a83          	lw	s5,8(sp)
    90ec:	01c12403          	lw	s0,28(sp)
    90f0:	02010113          	addi	sp,sp,32
    90f4:	00008067          	ret
    90f8:	00040693          	mv	a3,s0
    90fc:	e05ff06f          	j	8f00 <gfx_fill_alpha+0x4c>
    9100:	000e0f13          	mv	t5,t3
    9104:	e1e64ee3          	blt	a2,t5,8f20 <gfx_fill_alpha+0x6c>
    9108:	fe5ff06f          	j	90ec <gfx_fill_alpha+0x238>
    910c:	00068293          	mv	t0,a3
    9110:	00038e93          	mv	t4,t2
    9114:	f1369ae3          	bne	a3,s3,9028 <gfx_fill_alpha+0x174>
    9118:	000ed703          	lhu	a4,0(t4)
    911c:	01071793          	slli	a5,a4,0x10
    9120:	00e787b3          	add	a5,a5,a4
    9124:	00a7f7b3          	and	a5,a5,a0
    9128:	40f80733          	sub	a4,a6,a5
    912c:	03170733          	mul	a4,a4,a7
    9130:	00579793          	slli	a5,a5,0x5
    9134:	006787b3          	add	a5,a5,t1
    9138:	00e787b3          	add	a5,a5,a4
    913c:	0057d793          	srli	a5,a5,0x5
    9140:	00a7f7b3          	and	a5,a5,a0
    9144:	0107d713          	srli	a4,a5,0x10
    9148:	00f767b3          	or	a5,a4,a5
    914c:	00fe9023          	sh	a5,0(t4)
    9150:	f7dff06f          	j	90cc <gfx_fill_alpha+0x218>
    9154:	00008067          	ret
    9158:	00008067          	ret
    915c:	00058e93          	mv	t4,a1
    9160:	f69ff06f          	j	90c8 <gfx_fill_alpha+0x214>

00009164 <gfx_blit_alpha>:
    9164:	0006a383          	lw	t2,0(a3)
    9168:	0046a883          	lw	a7,4(a3)
    916c:	00f05e63          	blez	a5,9188 <gfx_blit_alpha+0x24>
    9170:	0fe00813          	li	a6,254
    9174:	02f84063          	blt	a6,a5,9194 <gfx_blit_alpha+0x30>
    9178:	00579e93          	slli	t4,a5,0x5
    917c:	00fe8eb3          	add	t4,t4,a5
    9180:	008ede93          	srli	t4,t4,0x8
    9184:	000e9463          	bnez	t4,918c <gfx_blit_alpha+0x28>
    9188:	00008067          	ret
    918c:	02000813          	li	a6,32
    9190:	010e9863          	bne	t4,a6,91a0 <gfx_blit_alpha+0x3c>
    9194:	00c6a803          	lw	a6,12(a3)
    9198:	02000e93          	li	t4,32
    919c:	38080263          	beqz	a6,9520 <gfx_blit_alpha+0x3bc>
    91a0:	fc010113          	addi	sp,sp,-64
    91a4:	03612023          	sw	s6,32(sp)
    91a8:	02112e23          	sw	ra,60(sp)
    91ac:	03312623          	sw	s3,44(sp)
    91b0:	00b38333          	add	t1,t2,a1
    91b4:	00038813          	mv	a6,t2
    91b8:	00000b13          	li	s6,0
    91bc:	0005d863          	bgez	a1,91cc <gfx_blit_alpha+0x68>
    91c0:	40b00b33          	neg	s6,a1
    91c4:	00030813          	mv	a6,t1
    91c8:	00000593          	li	a1,0
    91cc:	00c88e33          	add	t3,a7,a2
    91d0:	00000993          	li	s3,0
    91d4:	00065863          	bgez	a2,91e4 <gfx_blit_alpha+0x80>
    91d8:	40c009b3          	neg	s3,a2
    91dc:	000e0893          	mv	a7,t3
    91e0:	00000613          	li	a2,0
    91e4:	00052283          	lw	t0,0(a0)
    91e8:	0062d463          	bge	t0,t1,91f0 <gfx_blit_alpha+0x8c>
    91ec:	40b28833          	sub	a6,t0,a1
    91f0:	00452303          	lw	t1,4(a0)
    91f4:	01c35463          	bge	t1,t3,91fc <gfx_blit_alpha+0x98>
    91f8:	40c308b3          	sub	a7,t1,a2
    91fc:	07005063          	blez	a6,925c <gfx_blit_alpha+0xf8>
    9200:	05105e63          	blez	a7,925c <gfx_blit_alpha+0xf8>
    9204:	03180333          	mul	t1,a6,a7
    9208:	02052f83          	lw	t6,32(a0)
    920c:	02452f03          	lw	t5,36(a0)
    9210:	01c52e03          	lw	t3,28(a0)
    9214:	02812c23          	sw	s0,56(sp)
    9218:	00c52403          	lw	s0,12(a0)
    921c:	001f8f93          	addi	t6,t6,1 # ffff0001 <__freertos_irq_stack_top+0xfff07e61>
    9220:	03f52023          	sw	t6,32(a0)
    9224:	006f0f33          	add	t5,t5,t1
    9228:	006e0333          	add	t1,t3,t1
    922c:	03e52223          	sw	t5,36(a0)
    9230:	00652e23          	sw	t1,28(a0)
    9234:	02040e63          	beqz	s0,9270 <gfx_blit_alpha+0x10c>
    9238:	01442303          	lw	t1,20(s0)
    923c:	2c030e63          	beqz	t1,9518 <gfx_blit_alpha+0x3b4>
    9240:	01052503          	lw	a0,16(a0)
    9244:	00f12223          	sw	a5,4(sp)
    9248:	00e12023          	sw	a4,0(sp)
    924c:	00098793          	mv	a5,s3
    9250:	000b0713          	mv	a4,s6
    9254:	000300e7          	jalr	t1
    9258:	03812403          	lw	s0,56(sp)
    925c:	03c12083          	lw	ra,60(sp)
    9260:	02c12983          	lw	s3,44(sp)
    9264:	02012b03          	lw	s6,32(sp)
    9268:	04010113          	addi	sp,sp,64
    926c:	00008067          	ret
    9270:	00078313          	mv	t1,a5
    9274:	02912a23          	sw	s1,52(sp)
    9278:	03212823          	sw	s2,48(sp)
    927c:	03412423          	sw	s4,40(sp)
    9280:	03512223          	sw	s5,36(sp)
    9284:	01712e23          	sw	s7,28(sp)
    9288:	01812c23          	sw	s8,24(sp)
    928c:	0ff00793          	li	a5,255
    9290:	2867c063          	blt	a5,t1,9510 <gfx_blit_alpha+0x3ac>
    9294:	02c28fb3          	mul	t6,t0,a2
    9298:	00852403          	lw	s0,8(a0)
    929c:	0086a903          	lw	s2,8(a3)
    92a0:	00c6aa03          	lw	s4,12(a3)
    92a4:	00735e13          	srli	t3,t1,0x7
    92a8:	00181813          	slli	a6,a6,0x1
    92ac:	000104b7          	lui	s1,0x10
    92b0:	07e10637          	lui	a2,0x7e10
    92b4:	02008f37          	lui	t5,0x2008
    92b8:	006e0e33          	add	t3,t3,t1
    92bc:	027987b3          	mul	a5,s3,t2
    92c0:	00bf8fb3          	add	t6,t6,a1
    92c4:	001f9f93          	slli	t6,t6,0x1
    92c8:	01f40433          	add	s0,s0,t6
    92cc:	00139a93          	slli	s5,t2,0x1
    92d0:	00129293          	slli	t0,t0,0x1
    92d4:	008805b3          	add	a1,a6,s0
    92d8:	00000993          	li	s3,0
    92dc:	81f48493          	addi	s1,s1,-2017 # f81f <demo_init+0x19b>
    92e0:	81f60613          	addi	a2,a2,-2017 # 7e0f81f <__freertos_irq_stack_top+0x7d2767f>
    92e4:	016787b3          	add	a5,a5,s6
    92e8:	00179693          	slli	a3,a5,0x1
    92ec:	00d90933          	add	s2,s2,a3
    92f0:	010f0f13          	addi	t5,t5,16 # 2008010 <__freertos_irq_stack_top+0x1f1fe70>
    92f4:	02000f93          	li	t6,32
    92f8:	140a0c63          	beqz	s4,9450 <gfx_blit_alpha+0x2ec>
    92fc:	0c071263          	bnez	a4,93c0 <gfx_blit_alpha+0x25c>
    9300:	00fa0333          	add	t1,s4,a5
    9304:	00090b13          	mv	s6,s2
    9308:	00040813          	mv	a6,s0
    930c:	00034503          	lbu	a0,0(t1)
    9310:	03c50533          	mul	a0,a0,t3
    9314:	00551693          	slli	a3,a0,0x5
    9318:	00a686b3          	add	a3,a3,a0
    931c:	0106d693          	srli	a3,a3,0x10
    9320:	04068c63          	beqz	a3,9378 <gfx_blit_alpha+0x214>
    9324:	000b5b83          	lhu	s7,0(s6)
    9328:	05f68663          	beq	a3,t6,9374 <gfx_blit_alpha+0x210>
    932c:	00085c03          	lhu	s8,0(a6)
    9330:	010b9513          	slli	a0,s7,0x10
    9334:	01750533          	add	a0,a0,s7
    9338:	010c1b93          	slli	s7,s8,0x10
    933c:	018b8bb3          	add	s7,s7,s8
    9340:	00cbfbb3          	and	s7,s7,a2
    9344:	00c57533          	and	a0,a0,a2
    9348:	41750533          	sub	a0,a0,s7
    934c:	02d506b3          	mul	a3,a0,a3
    9350:	005b9b93          	slli	s7,s7,0x5
    9354:	01eb8bb3          	add	s7,s7,t5
    9358:	017686b3          	add	a3,a3,s7
    935c:	0056d693          	srli	a3,a3,0x5
    9360:	00c6f6b3          	and	a3,a3,a2
    9364:	0106db93          	srli	s7,a3,0x10
    9368:	00dbebb3          	or	s7,s7,a3
    936c:	010b9b93          	slli	s7,s7,0x10
    9370:	010bdb93          	srli	s7,s7,0x10
    9374:	01781023          	sh	s7,0(a6)
    9378:	00280813          	addi	a6,a6,2
    937c:	00130313          	addi	t1,t1,1
    9380:	002b0b13          	addi	s6,s6,2
    9384:	f8b814e3          	bne	a6,a1,930c <gfx_blit_alpha+0x1a8>
    9388:	00198993          	addi	s3,s3,1
    938c:	01590933          	add	s2,s2,s5
    9390:	00540433          	add	s0,s0,t0
    9394:	005585b3          	add	a1,a1,t0
    9398:	007787b3          	add	a5,a5,t2
    939c:	f5389ee3          	bne	a7,s3,92f8 <gfx_blit_alpha+0x194>
    93a0:	03812403          	lw	s0,56(sp)
    93a4:	03412483          	lw	s1,52(sp)
    93a8:	03012903          	lw	s2,48(sp)
    93ac:	02812a03          	lw	s4,40(sp)
    93b0:	02412a83          	lw	s5,36(sp)
    93b4:	01c12b83          	lw	s7,28(sp)
    93b8:	01812c03          	lw	s8,24(sp)
    93bc:	ea1ff06f          	j	925c <gfx_blit_alpha+0xf8>
    93c0:	00fa0833          	add	a6,s4,a5
    93c4:	00090313          	mv	t1,s2
    93c8:	00040513          	mv	a0,s0
    93cc:	00084683          	lbu	a3,0(a6)
    93d0:	00180813          	addi	a6,a6,1
    93d4:	03c686b3          	mul	a3,a3,t3
    93d8:	00569b13          	slli	s6,a3,0x5
    93dc:	00db0b33          	add	s6,s6,a3
    93e0:	010b5b13          	srli	s6,s6,0x10
    93e4:	040b0e63          	beqz	s6,9440 <gfx_blit_alpha+0x2dc>
    93e8:	00035b83          	lhu	s7,0(t1)
    93ec:	010b9693          	slli	a3,s7,0x10
    93f0:	017686b3          	add	a3,a3,s7
    93f4:	00c6f6b3          	and	a3,a3,a2
    93f8:	049b8463          	beq	s7,s1,9440 <gfx_blit_alpha+0x2dc>
    93fc:	05fb0063          	beq	s6,t6,943c <gfx_blit_alpha+0x2d8>
    9400:	00055c03          	lhu	s8,0(a0)
    9404:	010c1b93          	slli	s7,s8,0x10
    9408:	018b8bb3          	add	s7,s7,s8
    940c:	00cbfbb3          	and	s7,s7,a2
    9410:	417686b3          	sub	a3,a3,s7
    9414:	036686b3          	mul	a3,a3,s6
    9418:	005b9b93          	slli	s7,s7,0x5
    941c:	01eb8bb3          	add	s7,s7,t5
    9420:	017686b3          	add	a3,a3,s7
    9424:	0056d693          	srli	a3,a3,0x5
    9428:	00c6f6b3          	and	a3,a3,a2
    942c:	0106db93          	srli	s7,a3,0x10
    9430:	00dbebb3          	or	s7,s7,a3
    9434:	010b9b93          	slli	s7,s7,0x10
    9438:	010bdb93          	srli	s7,s7,0x10
    943c:	01751023          	sh	s7,0(a0)
    9440:	00250513          	addi	a0,a0,2
    9444:	00230313          	addi	t1,t1,2
    9448:	f8b512e3          	bne	a0,a1,93cc <gfx_blit_alpha+0x268>
    944c:	f3dff06f          	j	9388 <gfx_blit_alpha+0x224>
    9450:	00040513          	mv	a0,s0
    9454:	00090813          	mv	a6,s2
    9458:	04071e63          	bnez	a4,94b4 <gfx_blit_alpha+0x350>
    945c:	00055b83          	lhu	s7,0(a0)
    9460:	00085b03          	lhu	s6,0(a6)
    9464:	00250513          	addi	a0,a0,2
    9468:	010b9313          	slli	t1,s7,0x10
    946c:	010b1693          	slli	a3,s6,0x10
    9470:	01730333          	add	t1,t1,s7
    9474:	016686b3          	add	a3,a3,s6
    9478:	00c37333          	and	t1,t1,a2
    947c:	00c6f6b3          	and	a3,a3,a2
    9480:	406686b3          	sub	a3,a3,t1
    9484:	03d686b3          	mul	a3,a3,t4
    9488:	00531313          	slli	t1,t1,0x5
    948c:	01e30333          	add	t1,t1,t5
    9490:	00280813          	addi	a6,a6,2
    9494:	006686b3          	add	a3,a3,t1
    9498:	0056d693          	srli	a3,a3,0x5
    949c:	00c6f6b3          	and	a3,a3,a2
    94a0:	0106d313          	srli	t1,a3,0x10
    94a4:	00d366b3          	or	a3,t1,a3
    94a8:	fed51f23          	sh	a3,-2(a0)
    94ac:	fab518e3          	bne	a0,a1,945c <gfx_blit_alpha+0x2f8>
    94b0:	ed9ff06f          	j	9388 <gfx_blit_alpha+0x224>
    94b4:	00085303          	lhu	t1,0(a6)
    94b8:	00280813          	addi	a6,a6,2
    94bc:	01031693          	slli	a3,t1,0x10
    94c0:	006686b3          	add	a3,a3,t1
    94c4:	00c6f6b3          	and	a3,a3,a2
    94c8:	02930e63          	beq	t1,s1,9504 <gfx_blit_alpha+0x3a0>
    94cc:	00055b03          	lhu	s6,0(a0)
    94d0:	010b1313          	slli	t1,s6,0x10
    94d4:	01630333          	add	t1,t1,s6
    94d8:	00c37333          	and	t1,t1,a2
    94dc:	406686b3          	sub	a3,a3,t1
    94e0:	03d686b3          	mul	a3,a3,t4
    94e4:	00531313          	slli	t1,t1,0x5
    94e8:	01e30333          	add	t1,t1,t5
    94ec:	006686b3          	add	a3,a3,t1
    94f0:	0056d693          	srli	a3,a3,0x5
    94f4:	00c6f6b3          	and	a3,a3,a2
    94f8:	0106d313          	srli	t1,a3,0x10
    94fc:	00d366b3          	or	a3,t1,a3
    9500:	00d51023          	sh	a3,0(a0)
    9504:	00250513          	addi	a0,a0,2
    9508:	fab516e3          	bne	a0,a1,94b4 <gfx_blit_alpha+0x350>
    950c:	e7dff06f          	j	9388 <gfx_blit_alpha+0x224>
    9510:	0ff00313          	li	t1,255
    9514:	d81ff06f          	j	9294 <gfx_blit_alpha+0x130>
    9518:	03812403          	lw	s0,56(sp)
    951c:	d41ff06f          	j	925c <gfx_blit_alpha+0xf8>
    9520:	fccff06f          	j	8cec <gfx_blit>

00009524 <gfx_border>:
    9524:	fe010113          	addi	sp,sp,-32
    9528:	00812c23          	sw	s0,24(sp)
    952c:	00912a23          	sw	s1,20(sp)
    9530:	01212823          	sw	s2,16(sp)
    9534:	01312623          	sw	s3,12(sp)
    9538:	01412423          	sw	s4,8(sp)
    953c:	01512223          	sw	s5,4(sp)
    9540:	01612023          	sw	s6,0(sp)
    9544:	00112e23          	sw	ra,28(sp)
    9548:	00068a13          	mv	s4,a3
    954c:	00050a93          	mv	s5,a0
    9550:	00058493          	mv	s1,a1
    9554:	00060b13          	mv	s6,a2
    9558:	00070913          	mv	s2,a4
    955c:	00078413          	mv	s0,a5
    9560:	00080993          	mv	s3,a6
    9564:	08d05c63          	blez	a3,95fc <gfx_border+0xd8>
    9568:	08f05c63          	blez	a5,9600 <gfx_border+0xdc>
    956c:	00080793          	mv	a5,a6
    9570:	00040713          	mv	a4,s0
    9574:	d18ff0ef          	jal	8a8c <gfx_fill_rect.part.0>
    9578:	012b0633          	add	a2,s6,s2
    957c:	00098793          	mv	a5,s3
    9580:	00040713          	mv	a4,s0
    9584:	000a0693          	mv	a3,s4
    9588:	40860633          	sub	a2,a2,s0
    958c:	00048593          	mv	a1,s1
    9590:	000a8513          	mv	a0,s5
    9594:	cf8ff0ef          	jal	8a8c <gfx_fill_rect.part.0>
    9598:	07205463          	blez	s2,9600 <gfx_border+0xdc>
    959c:	00098793          	mv	a5,s3
    95a0:	00090713          	mv	a4,s2
    95a4:	00040693          	mv	a3,s0
    95a8:	000b0613          	mv	a2,s6
    95ac:	00048593          	mv	a1,s1
    95b0:	000a8513          	mv	a0,s5
    95b4:	cd8ff0ef          	jal	8a8c <gfx_fill_rect.part.0>
    95b8:	014485b3          	add	a1,s1,s4
    95bc:	00040693          	mv	a3,s0
    95c0:	408585b3          	sub	a1,a1,s0
    95c4:	01812403          	lw	s0,24(sp)
    95c8:	01c12083          	lw	ra,28(sp)
    95cc:	01412483          	lw	s1,20(sp)
    95d0:	00812a03          	lw	s4,8(sp)
    95d4:	00098793          	mv	a5,s3
    95d8:	00090713          	mv	a4,s2
    95dc:	00c12983          	lw	s3,12(sp)
    95e0:	01012903          	lw	s2,16(sp)
    95e4:	000b0613          	mv	a2,s6
    95e8:	000a8513          	mv	a0,s5
    95ec:	00012b03          	lw	s6,0(sp)
    95f0:	00412a83          	lw	s5,4(sp)
    95f4:	02010113          	addi	sp,sp,32
    95f8:	c94ff06f          	j	8a8c <gfx_fill_rect.part.0>
    95fc:	f8f04ee3          	bgtz	a5,9598 <gfx_border+0x74>
    9600:	01c12083          	lw	ra,28(sp)
    9604:	01812403          	lw	s0,24(sp)
    9608:	01412483          	lw	s1,20(sp)
    960c:	01012903          	lw	s2,16(sp)
    9610:	00c12983          	lw	s3,12(sp)
    9614:	00812a03          	lw	s4,8(sp)
    9618:	00412a83          	lw	s5,4(sp)
    961c:	00012b03          	lw	s6,0(sp)
    9620:	02010113          	addi	sp,sp,32
    9624:	00008067          	ret

00009628 <gfx_text>:
    9628:	0006c803          	lbu	a6,0(a3)
    962c:	2c080063          	beqz	a6,98ec <gfx_text+0x2c4>
    9630:	fa010113          	addi	sp,sp,-96
    9634:	03712e23          	sw	s7,60(sp)
    9638:	03b12623          	sw	s11,44(sp)
    963c:	04112e23          	sw	ra,92(sp)
    9640:	00068b93          	mv	s7,a3
    9644:	00070d93          	mv	s11,a4
    9648:	14e05663          	blez	a4,9794 <gfx_text+0x16c>
    964c:	05612023          	sw	s6,64(sp)
    9650:	00078b13          	mv	s6,a5
    9654:	00371793          	slli	a5,a4,0x3
    9658:	00e787b3          	add	a5,a5,a4
    965c:	05412423          	sw	s4,72(sp)
    9660:	00013a37          	lui	s4,0x13
    9664:	00f12a23          	sw	a5,20(sp)
    9668:	bb8a0793          	addi	a5,s4,-1096 # 12bb8 <shadow_radius+0x18>
    966c:	00f12223          	sw	a5,4(sp)
    9670:	000137b7          	lui	a5,0x13
    9674:	ca078793          	addi	a5,a5,-864 # 12ca0 <letters>
    9678:	05512223          	sw	s5,68(sp)
    967c:	00171a93          	slli	s5,a4,0x1
    9680:	00ea8ab3          	add	s5,s5,a4
    9684:	00f12623          	sw	a5,12(sp)
    9688:	001a9a93          	slli	s5,s5,0x1
    968c:	84818793          	addi	a5,gp,-1976 # 13450 <p.0>
    9690:	04912a23          	sw	s1,84(sp)
    9694:	05212823          	sw	s2,80(sp)
    9698:	03812c23          	sw	s8,56(sp)
    969c:	03a12823          	sw	s10,48(sp)
    96a0:	04812c23          	sw	s0,88(sp)
    96a4:	05312623          	sw	s3,76(sp)
    96a8:	03912a23          	sw	s9,52(sp)
    96ac:	00050c13          	mv	s8,a0
    96b0:	00058d13          	mv	s10,a1
    96b4:	00f12e23          	sw	a5,28(sp)
    96b8:	00b12c23          	sw	a1,24(sp)
    96bc:	00060493          	mv	s1,a2
    96c0:	00068913          	mv	s2,a3
    96c4:	01512823          	sw	s5,16(sp)
    96c8:	00a00793          	li	a5,10
    96cc:	20f80863          	beq	a6,a5,98dc <gfx_text+0x2b4>
    96d0:	f9f80793          	addi	a5,a6,-97
    96d4:	00000a93          	li	s5,0
    96d8:	01900a13          	li	s4,25
    96dc:	00912423          	sw	s1,8(sp)
    96e0:	0ff7f793          	zext.b	a5,a5
    96e4:	fff00c93          	li	s9,-1
    96e8:	000a8993          	mv	s3,s5
    96ec:	08fa6263          	bltu	s4,a5,9770 <gfx_text+0x148>
    96f0:	fe080813          	addi	a6,a6,-32
    96f4:	0ff87813          	zext.b	a6,a6
    96f8:	fbf80813          	addi	a6,a6,-65
    96fc:	00c12703          	lw	a4,12(sp)
    9700:	00381793          	slli	a5,a6,0x3
    9704:	410787b3          	sub	a5,a5,a6
    9708:	00f707b3          	add	a5,a4,a5
    970c:	013787b3          	add	a5,a5,s3
    9710:	0007ca83          	lbu	s5,0(a5)
    9714:	000d0413          	mv	s0,s10
    9718:	00400b93          	li	s7,4
    971c:	017ad7b3          	srl	a5,s5,s7
    9720:	0017f793          	andi	a5,a5,1
    9724:	02078063          	beqz	a5,9744 <gfx_text+0x11c>
    9728:	000b0793          	mv	a5,s6
    972c:	000d8713          	mv	a4,s11
    9730:	000d8693          	mv	a3,s11
    9734:	00048613          	mv	a2,s1
    9738:	00040593          	mv	a1,s0
    973c:	000c0513          	mv	a0,s8
    9740:	b4cff0ef          	jal	8a8c <gfx_fill_rect.part.0>
    9744:	fffb8b93          	addi	s7,s7,-1
    9748:	01b40433          	add	s0,s0,s11
    974c:	fd9b98e3          	bne	s7,s9,971c <gfx_text+0xf4>
    9750:	00198993          	addi	s3,s3,1
    9754:	00700793          	li	a5,7
    9758:	01b484b3          	add	s1,s1,s11
    975c:	12f98663          	beq	s3,a5,9888 <gfx_text+0x260>
    9760:	00094803          	lbu	a6,0(s2)
    9764:	f9f80793          	addi	a5,a6,-97
    9768:	0ff7f793          	zext.b	a5,a5
    976c:	f8fa72e3          	bgeu	s4,a5,96f0 <gfx_text+0xc8>
    9770:	fdf80793          	addi	a5,a6,-33
    9774:	0ff7f793          	zext.b	a5,a5
    9778:	03900713          	li	a4,57
    977c:	04f76063          	bltu	a4,a5,97bc <gfx_text+0x194>
    9780:	00412703          	lw	a4,4(sp)
    9784:	00279793          	slli	a5,a5,0x2
    9788:	00e787b3          	add	a5,a5,a4
    978c:	0007a783          	lw	a5,0(a5)
    9790:	00078067          	jr	a5
    9794:	001bc783          	lbu	a5,1(s7)
    9798:	001b8b93          	addi	s7,s7,1
    979c:	fe079ce3          	bnez	a5,9794 <gfx_text+0x16c>
    97a0:	05c12083          	lw	ra,92(sp)
    97a4:	03c12b83          	lw	s7,60(sp)
    97a8:	02c12d83          	lw	s11,44(sp)
    97ac:	06010113          	addi	sp,sp,96
    97b0:	00008067          	ret
    97b4:	fea80813          	addi	a6,a6,-22
    97b8:	f45ff06f          	j	96fc <gfx_text+0xd4>
    97bc:	00000a93          	li	s5,0
    97c0:	f55ff06f          	j	9714 <gfx_text+0xec>
    97c4:	00300793          	li	a5,3
    97c8:	00400a93          	li	s5,4
    97cc:	f537d4e3          	bge	a5,s3,9714 <gfx_text+0xec>
    97d0:	ffa98a93          	addi	s5,s3,-6
    97d4:	001aba93          	seqz	s5,s5
    97d8:	002a9a93          	slli	s5,s5,0x2
    97dc:	f39ff06f          	j	9714 <gfx_text+0xec>
    97e0:	ffd98a93          	addi	s5,s3,-3
    97e4:	001aba93          	seqz	s5,s5
    97e8:	41500ab3          	neg	s5,s5
    97ec:	00eafa93          	andi	s5,s5,14
    97f0:	f25ff06f          	j	9714 <gfx_text+0xec>
    97f4:	00100a93          	li	s5,1
    97f8:	f0098ee3          	beqz	s3,9714 <gfx_text+0xec>
    97fc:	00200a93          	li	s5,2
    9800:	f13adae3          	bge	s5,s3,9714 <gfx_text+0xec>
    9804:	00400a93          	li	s5,4
    9808:	f13ad6e3          	bge	s5,s3,9714 <gfx_text+0xec>
    980c:	00800a93          	li	s5,8
    9810:	f05ff06f          	j	9714 <gfx_text+0xec>
    9814:	00300793          	li	a5,3
    9818:	00e00a93          	li	s5,14
    981c:	eef98ce3          	beq	s3,a5,9714 <gfx_text+0xec>
    9820:	00200793          	li	a5,2
    9824:	00400a93          	li	s5,4
    9828:	eef986e3          	beq	s3,a5,9714 <gfx_text+0xec>
    982c:	ffc98a93          	addi	s5,s3,-4
    9830:	001aba93          	seqz	s5,s5
    9834:	002a9a93          	slli	s5,s5,0x2
    9838:	eddff06f          	j	9714 <gfx_text+0xec>
    983c:	01c12783          	lw	a5,28(sp)
    9840:	00f987b3          	add	a5,s3,a5
    9844:	0007ca83          	lbu	s5,0(a5)
    9848:	ecdff06f          	j	9714 <gfx_text+0xec>
    984c:	fff98793          	addi	a5,s3,-1
    9850:	00400613          	li	a2,4
    9854:	00000a93          	li	s5,0
    9858:	eaf66ee3          	bltu	a2,a5,9714 <gfx_text+0xec>
    985c:	84018793          	addi	a5,gp,-1984 # 13448 <CSWTCH.42>
    9860:	00f987b3          	add	a5,s3,a5
    9864:	fff7ca83          	lbu	s5,-1(a5)
    9868:	eadff06f          	j	9714 <gfx_text+0xec>
    986c:	00200793          	li	a5,2
    9870:	00400a93          	li	s5,4
    9874:	eaf980e3          	beq	s3,a5,9714 <gfx_text+0xec>
    9878:	ffb98a93          	addi	s5,s3,-5
    987c:	001aba93          	seqz	s5,s5
    9880:	002a9a93          	slli	s5,s5,0x2
    9884:	e91ff06f          	j	9714 <gfx_text+0xec>
    9888:	01012783          	lw	a5,16(sp)
    988c:	00812483          	lw	s1,8(sp)
    9890:	00fd0d33          	add	s10,s10,a5
    9894:	00194803          	lbu	a6,1(s2)
    9898:	00190913          	addi	s2,s2,1
    989c:	e20816e3          	bnez	a6,96c8 <gfx_text+0xa0>
    98a0:	05812403          	lw	s0,88(sp)
    98a4:	05c12083          	lw	ra,92(sp)
    98a8:	05412483          	lw	s1,84(sp)
    98ac:	05012903          	lw	s2,80(sp)
    98b0:	04c12983          	lw	s3,76(sp)
    98b4:	04812a03          	lw	s4,72(sp)
    98b8:	04412a83          	lw	s5,68(sp)
    98bc:	04012b03          	lw	s6,64(sp)
    98c0:	03812c03          	lw	s8,56(sp)
    98c4:	03412c83          	lw	s9,52(sp)
    98c8:	03012d03          	lw	s10,48(sp)
    98cc:	03c12b83          	lw	s7,60(sp)
    98d0:	02c12d83          	lw	s11,44(sp)
    98d4:	06010113          	addi	sp,sp,96
    98d8:	00008067          	ret
    98dc:	01412783          	lw	a5,20(sp)
    98e0:	01812d03          	lw	s10,24(sp)
    98e4:	00f484b3          	add	s1,s1,a5
    98e8:	fadff06f          	j	9894 <gfx_text+0x26c>
    98ec:	00008067          	ret

000098f0 <gfx_text_width>:
    98f0:	ff010113          	addi	sp,sp,-16
    98f4:	00812423          	sw	s0,8(sp)
    98f8:	00112623          	sw	ra,12(sp)
    98fc:	00058413          	mv	s0,a1
    9900:	a85f70ef          	jal	1384 <strlen>
    9904:	028507b3          	mul	a5,a0,s0
    9908:	00c12083          	lw	ra,12(sp)
    990c:	00179513          	slli	a0,a5,0x1
    9910:	00f50533          	add	a0,a0,a5
    9914:	00151513          	slli	a0,a0,0x1
    9918:	40850533          	sub	a0,a0,s0
    991c:	00812403          	lw	s0,8(sp)
    9920:	01010113          	addi	sp,sp,16
    9924:	00008067          	ret

00009928 <hw_begin>:
    9928:	0ff0000f          	fence
    992c:	00008067          	ret

00009930 <hw_end>:
    9930:	00008067          	ret

00009934 <hw_copy_alpha>:
    9934:	ff010113          	addi	sp,sp,-16
    9938:	01412303          	lw	t1,20(sp)
    993c:	00112623          	sw	ra,12(sp)
    9940:	00812423          	sw	s0,8(sp)
    9944:	00912223          	sw	s1,4(sp)
    9948:	00080e93          	mv	t4,a6
    994c:	00000e13          	li	t3,0
    9950:	00605c63          	blez	t1,9968 <hw_copy_alpha+0x34>
    9954:	00010e37          	lui	t3,0x10
    9958:	0fe00813          	li	a6,254
    995c:	f00e0e13          	addi	t3,t3,-256 # ff00 <mini_vsnprintf+0x14>
    9960:	00684463          	blt	a6,t1,9968 <hw_copy_alpha+0x34>
    9964:	00831e13          	slli	t3,t1,0x8
    9968:	0006a303          	lw	t1,0(a3)
    996c:	00c6a803          	lw	a6,12(a3)
    9970:	00052f83          	lw	t6,0(a0)
    9974:	026787b3          	mul	a5,a5,t1
    9978:	00452503          	lw	a0,4(a0)
    997c:	00000f13          	li	t5,0
    9980:	00e78733          	add	a4,a5,a4
    9984:	00080463          	beqz	a6,998c <hw_copy_alpha+0x58>
    9988:	00e80f33          	add	t5,a6,a4
    998c:	02a60633          	mul	a2,a2,a0
    9990:	000e4437          	lui	s0,0xe4
    9994:	01012783          	lw	a5,16(sp)
    9998:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    999c:	0086a683          	lw	a3,8(a3)
    99a0:	00159593          	slli	a1,a1,0x1
    99a4:	00042803          	lw	a6,0(s0)
    99a8:	01f585b3          	add	a1,a1,t6
    99ac:	00171713          	slli	a4,a4,0x1
    99b0:	00f037b3          	snez	a5,a5
    99b4:	00c585b3          	add	a1,a1,a2
    99b8:	00e686b3          	add	a3,a3,a4
    99bc:	00179793          	slli	a5,a5,0x1
    99c0:	00131713          	slli	a4,t1,0x1
    99c4:	01058863          	beq	a1,a6,99d4 <hw_copy_alpha+0xa0>
    99c8:	f8101637          	lui	a2,0xf8101
    99cc:	00b42023          	sw	a1,0(s0)
    99d0:	00b62823          	sw	a1,16(a2) # f8101010 <__freertos_irq_stack_top+0xf8018e70>
    99d4:	00442603          	lw	a2,4(s0)
    99d8:	00c50863          	beq	a0,a2,99e8 <hw_copy_alpha+0xb4>
    99dc:	f8101637          	lui	a2,0xf8101
    99e0:	00a42223          	sw	a0,4(s0)
    99e4:	00a62a23          	sw	a0,20(a2) # f8101014 <__freertos_irq_stack_top+0xf8018e74>
    99e8:	00842603          	lw	a2,8(s0)
    99ec:	00c68863          	beq	a3,a2,99fc <hw_copy_alpha+0xc8>
    99f0:	f8101637          	lui	a2,0xf8101
    99f4:	00d42423          	sw	a3,8(s0)
    99f8:	00d62c23          	sw	a3,24(a2) # f8101018 <__freertos_irq_stack_top+0xf8018e78>
    99fc:	00c42683          	lw	a3,12(s0)
    9a00:	00d70863          	beq	a4,a3,9a10 <hw_copy_alpha+0xdc>
    9a04:	f81016b7          	lui	a3,0xf8101
    9a08:	00e42623          	sw	a4,12(s0)
    9a0c:	00e6ae23          	sw	a4,28(a3) # f810101c <__freertos_irq_stack_top+0xf8018e7c>
    9a10:	01842683          	lw	a3,24(s0)
    9a14:	01089713          	slli	a4,a7,0x10
    9a18:	01d76833          	or	a6,a4,t4
    9a1c:	00d80863          	beq	a6,a3,9a2c <hw_copy_alpha+0xf8>
    9a20:	f8101737          	lui	a4,0xf8101
    9a24:	01042c23          	sw	a6,24(s0)
    9a28:	03072423          	sw	a6,40(a4) # f8101028 <__freertos_irq_stack_top+0xf8018e88>
    9a2c:	020f0863          	beqz	t5,9a5c <hw_copy_alpha+0x128>
    9a30:	01042703          	lw	a4,16(s0)
    9a34:	00ef0863          	beq	t5,a4,9a44 <hw_copy_alpha+0x110>
    9a38:	f8101737          	lui	a4,0xf8101
    9a3c:	01e42823          	sw	t5,16(s0)
    9a40:	03e72023          	sw	t5,32(a4) # f8101020 <__freertos_irq_stack_top+0xf8018e80>
    9a44:	01442703          	lw	a4,20(s0)
    9a48:	00e30863          	beq	t1,a4,9a58 <hw_copy_alpha+0x124>
    9a4c:	f8101737          	lui	a4,0xf8101
    9a50:	00642a23          	sw	t1,20(s0)
    9a54:	02672223          	sw	t1,36(a4) # f8101024 <__freertos_irq_stack_top+0xf8018e84>
    9a58:	0047e793          	ori	a5,a5,4
    9a5c:	02442703          	lw	a4,36(s0)
    9a60:	01c7e7b3          	or	a5,a5,t3
    9a64:	0017e493          	ori	s1,a5,1
    9a68:	02070a63          	beqz	a4,9a9c <hw_copy_alpha+0x168>
    9a6c:	02842683          	lw	a3,40(s0)
    9a70:	fff70713          	addi	a4,a4,-1
    9a74:	02e42223          	sw	a4,36(s0)
    9a78:	00168693          	addi	a3,a3,1
    9a7c:	02d42423          	sw	a3,40(s0)
    9a80:	00c12083          	lw	ra,12(sp)
    9a84:	00812403          	lw	s0,8(sp)
    9a88:	f81017b7          	lui	a5,0xf8101
    9a8c:	0297aa23          	sw	s1,52(a5) # f8101034 <__freertos_irq_stack_top+0xf8018e94>
    9a90:	00412483          	lw	s1,4(sp)
    9a94:	01010113          	addi	sp,sp,16
    9a98:	00008067          	ret
    9a9c:	120070ef          	jal	10bbc <blit_refill>
    9aa0:	02442703          	lw	a4,36(s0)
    9aa4:	fc9ff06f          	j	9a6c <hw_copy_alpha+0x138>

00009aa8 <hw_fill_alpha>:
    9aa8:	00452883          	lw	a7,4(a0)
    9aac:	00052503          	lw	a0,0(a0)
    9ab0:	ff010113          	addi	sp,sp,-16
    9ab4:	03160633          	mul	a2,a2,a7
    9ab8:	00159593          	slli	a1,a1,0x1
    9abc:	00912223          	sw	s1,4(sp)
    9ac0:	00a585b3          	add	a1,a1,a0
    9ac4:	00112623          	sw	ra,12(sp)
    9ac8:	00812423          	sw	s0,8(sp)
    9acc:	00000493          	li	s1,0
    9ad0:	00c585b3          	add	a1,a1,a2
    9ad4:	01005c63          	blez	a6,9aec <hw_fill_alpha+0x44>
    9ad8:	000104b7          	lui	s1,0x10
    9adc:	0fe00613          	li	a2,254
    9ae0:	f0048493          	addi	s1,s1,-256 # ff00 <mini_vsnprintf+0x14>
    9ae4:	01064463          	blt	a2,a6,9aec <hw_fill_alpha+0x44>
    9ae8:	00881493          	slli	s1,a6,0x8
    9aec:	000e4437          	lui	s0,0xe4
    9af0:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    9af4:	00042603          	lw	a2,0(s0)
    9af8:	00c58863          	beq	a1,a2,9b08 <hw_fill_alpha+0x60>
    9afc:	f8101637          	lui	a2,0xf8101
    9b00:	00b42023          	sw	a1,0(s0)
    9b04:	00b62823          	sw	a1,16(a2) # f8101010 <__freertos_irq_stack_top+0xf8018e70>
    9b08:	00442603          	lw	a2,4(s0)
    9b0c:	00c88863          	beq	a7,a2,9b1c <hw_fill_alpha+0x74>
    9b10:	f8101637          	lui	a2,0xf8101
    9b14:	01142223          	sw	a7,4(s0)
    9b18:	01162a23          	sw	a7,20(a2) # f8101014 <__freertos_irq_stack_top+0xf8018e74>
    9b1c:	01842603          	lw	a2,24(s0)
    9b20:	01071713          	slli	a4,a4,0x10
    9b24:	00d766b3          	or	a3,a4,a3
    9b28:	00c68863          	beq	a3,a2,9b38 <hw_fill_alpha+0x90>
    9b2c:	f8101737          	lui	a4,0xf8101
    9b30:	00d42c23          	sw	a3,24(s0)
    9b34:	02d72423          	sw	a3,40(a4) # f8101028 <__freertos_irq_stack_top+0xf8018e88>
    9b38:	01c42703          	lw	a4,28(s0)
    9b3c:	00e78863          	beq	a5,a4,9b4c <hw_fill_alpha+0xa4>
    9b40:	f8101737          	lui	a4,0xf8101
    9b44:	00f42e23          	sw	a5,28(s0)
    9b48:	02f72623          	sw	a5,44(a4) # f810102c <__freertos_irq_stack_top+0xf8018e8c>
    9b4c:	02442783          	lw	a5,36(s0)
    9b50:	02078a63          	beqz	a5,9b84 <hw_fill_alpha+0xdc>
    9b54:	02842683          	lw	a3,40(s0)
    9b58:	fff78793          	addi	a5,a5,-1
    9b5c:	02f42223          	sw	a5,36(s0)
    9b60:	00168693          	addi	a3,a3,1
    9b64:	02d42423          	sw	a3,40(s0)
    9b68:	00c12083          	lw	ra,12(sp)
    9b6c:	00812403          	lw	s0,8(sp)
    9b70:	f8101737          	lui	a4,0xf8101
    9b74:	02972a23          	sw	s1,52(a4) # f8101034 <__freertos_irq_stack_top+0xf8018e94>
    9b78:	00412483          	lw	s1,4(sp)
    9b7c:	01010113          	addi	sp,sp,16
    9b80:	00008067          	ret
    9b84:	038070ef          	jal	10bbc <blit_refill>
    9b88:	02442783          	lw	a5,36(s0)
    9b8c:	fc9ff06f          	j	9b54 <hw_fill_alpha+0xac>

00009b90 <hw_copy>:
    9b90:	0006a303          	lw	t1,0(a3)
    9b94:	00452e03          	lw	t3,4(a0)
    9b98:	ff010113          	addi	sp,sp,-16
    9b9c:	026787b3          	mul	a5,a5,t1
    9ba0:	00052e83          	lw	t4,0(a0)
    9ba4:	00812423          	sw	s0,8(sp)
    9ba8:	000e4437          	lui	s0,0xe4
    9bac:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    9bb0:	0086a683          	lw	a3,8(a3)
    9bb4:	00159593          	slli	a1,a1,0x1
    9bb8:	00042503          	lw	a0,0(s0)
    9bbc:	01d585b3          	add	a1,a1,t4
    9bc0:	00112623          	sw	ra,12(sp)
    9bc4:	03c60633          	mul	a2,a2,t3
    9bc8:	00e787b3          	add	a5,a5,a4
    9bcc:	01012703          	lw	a4,16(sp)
    9bd0:	00179793          	slli	a5,a5,0x1
    9bd4:	00912223          	sw	s1,4(sp)
    9bd8:	00e03733          	snez	a4,a4
    9bdc:	00f686b3          	add	a3,a3,a5
    9be0:	00171713          	slli	a4,a4,0x1
    9be4:	00131793          	slli	a5,t1,0x1
    9be8:	00c585b3          	add	a1,a1,a2
    9bec:	00a58863          	beq	a1,a0,9bfc <hw_copy+0x6c>
    9bf0:	f8101637          	lui	a2,0xf8101
    9bf4:	00b42023          	sw	a1,0(s0)
    9bf8:	00b62823          	sw	a1,16(a2) # f8101010 <__freertos_irq_stack_top+0xf8018e70>
    9bfc:	00442603          	lw	a2,4(s0)
    9c00:	00ce0863          	beq	t3,a2,9c10 <hw_copy+0x80>
    9c04:	f8101637          	lui	a2,0xf8101
    9c08:	01c42223          	sw	t3,4(s0)
    9c0c:	01c62a23          	sw	t3,20(a2) # f8101014 <__freertos_irq_stack_top+0xf8018e74>
    9c10:	00842603          	lw	a2,8(s0)
    9c14:	00c68863          	beq	a3,a2,9c24 <hw_copy+0x94>
    9c18:	f8101637          	lui	a2,0xf8101
    9c1c:	00d42423          	sw	a3,8(s0)
    9c20:	00d62c23          	sw	a3,24(a2) # f8101018 <__freertos_irq_stack_top+0xf8018e78>
    9c24:	00c42683          	lw	a3,12(s0)
    9c28:	00d78863          	beq	a5,a3,9c38 <hw_copy+0xa8>
    9c2c:	f81016b7          	lui	a3,0xf8101
    9c30:	00f42623          	sw	a5,12(s0)
    9c34:	00f6ae23          	sw	a5,28(a3) # f810101c <__freertos_irq_stack_top+0xf8018e7c>
    9c38:	01842783          	lw	a5,24(s0)
    9c3c:	01089893          	slli	a7,a7,0x10
    9c40:	0108e833          	or	a6,a7,a6
    9c44:	00f80863          	beq	a6,a5,9c54 <hw_copy+0xc4>
    9c48:	f81017b7          	lui	a5,0xf8101
    9c4c:	01042c23          	sw	a6,24(s0)
    9c50:	0307a423          	sw	a6,40(a5) # f8101028 <__freertos_irq_stack_top+0xf8018e88>
    9c54:	02442783          	lw	a5,36(s0)
    9c58:	000106b7          	lui	a3,0x10
    9c5c:	f0168693          	addi	a3,a3,-255 # ff01 <mini_vsnprintf+0x15>
    9c60:	00d764b3          	or	s1,a4,a3
    9c64:	02078a63          	beqz	a5,9c98 <hw_copy+0x108>
    9c68:	02842683          	lw	a3,40(s0)
    9c6c:	fff78793          	addi	a5,a5,-1
    9c70:	02f42223          	sw	a5,36(s0)
    9c74:	00168693          	addi	a3,a3,1
    9c78:	02d42423          	sw	a3,40(s0)
    9c7c:	00c12083          	lw	ra,12(sp)
    9c80:	00812403          	lw	s0,8(sp)
    9c84:	f8101737          	lui	a4,0xf8101
    9c88:	02972a23          	sw	s1,52(a4) # f8101034 <__freertos_irq_stack_top+0xf8018e94>
    9c8c:	00412483          	lw	s1,4(sp)
    9c90:	01010113          	addi	sp,sp,16
    9c94:	00008067          	ret
    9c98:	725060ef          	jal	10bbc <blit_refill>
    9c9c:	02442783          	lw	a5,36(s0)
    9ca0:	fc9ff06f          	j	9c68 <hw_copy+0xd8>

00009ca4 <hw_fill>:
    9ca4:	00452803          	lw	a6,4(a0)
    9ca8:	ff010113          	addi	sp,sp,-16
    9cac:	00052883          	lw	a7,0(a0)
    9cb0:	03060633          	mul	a2,a2,a6
    9cb4:	00812423          	sw	s0,8(sp)
    9cb8:	000e4437          	lui	s0,0xe4
    9cbc:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    9cc0:	00159593          	slli	a1,a1,0x1
    9cc4:	00042503          	lw	a0,0(s0)
    9cc8:	011585b3          	add	a1,a1,a7
    9ccc:	00112623          	sw	ra,12(sp)
    9cd0:	00c585b3          	add	a1,a1,a2
    9cd4:	00a58863          	beq	a1,a0,9ce4 <hw_fill+0x40>
    9cd8:	f8101637          	lui	a2,0xf8101
    9cdc:	00b42023          	sw	a1,0(s0)
    9ce0:	00b62823          	sw	a1,16(a2) # f8101010 <__freertos_irq_stack_top+0xf8018e70>
    9ce4:	00442603          	lw	a2,4(s0)
    9ce8:	00c80863          	beq	a6,a2,9cf8 <hw_fill+0x54>
    9cec:	f8101637          	lui	a2,0xf8101
    9cf0:	01042223          	sw	a6,4(s0)
    9cf4:	01062a23          	sw	a6,20(a2) # f8101014 <__freertos_irq_stack_top+0xf8018e74>
    9cf8:	01842603          	lw	a2,24(s0)
    9cfc:	01071713          	slli	a4,a4,0x10
    9d00:	00d766b3          	or	a3,a4,a3
    9d04:	00c68863          	beq	a3,a2,9d14 <hw_fill+0x70>
    9d08:	f8101737          	lui	a4,0xf8101
    9d0c:	00d42c23          	sw	a3,24(s0)
    9d10:	02d72423          	sw	a3,40(a4) # f8101028 <__freertos_irq_stack_top+0xf8018e88>
    9d14:	01c42703          	lw	a4,28(s0)
    9d18:	00e78863          	beq	a5,a4,9d28 <hw_fill+0x84>
    9d1c:	f8101737          	lui	a4,0xf8101
    9d20:	00f42e23          	sw	a5,28(s0)
    9d24:	02f72623          	sw	a5,44(a4) # f810102c <__freertos_irq_stack_top+0xf8018e8c>
    9d28:	02442783          	lw	a5,36(s0)
    9d2c:	02078c63          	beqz	a5,9d64 <hw_fill+0xc0>
    9d30:	02842603          	lw	a2,40(s0)
    9d34:	fff78793          	addi	a5,a5,-1
    9d38:	02f42223          	sw	a5,36(s0)
    9d3c:	00160613          	addi	a2,a2,1
    9d40:	02c42423          	sw	a2,40(s0)
    9d44:	00c12083          	lw	ra,12(sp)
    9d48:	00812403          	lw	s0,8(sp)
    9d4c:	000106b7          	lui	a3,0x10
    9d50:	f8101737          	lui	a4,0xf8101
    9d54:	f0068693          	addi	a3,a3,-256 # ff00 <mini_vsnprintf+0x14>
    9d58:	02d72a23          	sw	a3,52(a4) # f8101034 <__freertos_irq_stack_top+0xf8018e94>
    9d5c:	01010113          	addi	sp,sp,16
    9d60:	00008067          	ret
    9d64:	659060ef          	jal	10bbc <blit_refill>
    9d68:	02442783          	lw	a5,36(s0)
    9d6c:	fc5ff06f          	j	9d30 <hw_fill+0x8c>

00009d70 <gfx_begin_hw>:
    9d70:	00169793          	slli	a5,a3,0x1
    9d74:	00c5a023          	sw	a2,0(a1)
    9d78:	00f5a223          	sw	a5,4(a1)
    9d7c:	00058613          	mv	a2,a1
    9d80:	000135b7          	lui	a1,0x13
    9d84:	d9c58593          	addi	a1,a1,-612 # 12d9c <gfx_hw_ops>
    9d88:	ef9fe06f          	j	8c80 <gfx_begin_backend>

00009d8c <win_end>:
    9d8c:	00052783          	lw	a5,0(a0)
    9d90:	fb010113          	addi	sp,sp,-80
    9d94:	04812423          	sw	s0,72(sp)
    9d98:	05212023          	sw	s2,64(sp)
    9d9c:	00060413          	mv	s0,a2
    9da0:	00058913          	mv	s2,a1
    9da4:	04912223          	sw	s1,68(sp)
    9da8:	40f90933          	sub	s2,s2,a5
    9dac:	00050493          	mv	s1,a0
    9db0:	06c00613          	li	a2,108
    9db4:	00000593          	li	a1,0
    9db8:	00c40513          	addi	a0,s0,12
    9dbc:	03412c23          	sw	s4,56(sp)
    9dc0:	04112623          	sw	ra,76(sp)
    9dc4:	03312e23          	sw	s3,60(sp)
    9dc8:	03512a23          	sw	s5,52(sp)
    9dcc:	03612823          	sw	s6,48(sp)
    9dd0:	03712623          	sw	s7,44(sp)
    9dd4:	03812423          	sw	s8,40(sp)
    9dd8:	03912223          	sw	s9,36(sp)
    9ddc:	03a12023          	sw	s10,32(sp)
    9de0:	01b12e23          	sw	s11,28(sp)
    9de4:	0044aa03          	lw	s4,4(s1)
    9de8:	b1cf70ef          	jal	1104 <memset>
    9dec:	02090a63          	beqz	s2,9e20 <win_end+0x94>
    9df0:	06300793          	li	a5,99
    9df4:	00195993          	srli	s3,s2,0x1
    9df8:	00090a93          	mv	s5,s2
    9dfc:	4127f263          	bgeu	a5,s2,a200 <win_end+0x474>
    9e00:	06400c13          	li	s8,100
    9e04:	03895733          	divu	a4,s2,s8
    9e08:	038737b3          	mulhu	a5,a4,s8
    9e0c:	00e12223          	sw	a4,4(sp)
    9e10:	00f12623          	sw	a5,12(sp)
    9e14:	038707b3          	mul	a5,a4,s8
    9e18:	00f12423          	sw	a5,8(sp)
    9e1c:	0280006f          	j	9e44 <win_end+0xb8>
    9e20:	06400793          	li	a5,100
    9e24:	00f12423          	sw	a5,8(sp)
    9e28:	00100793          	li	a5,1
    9e2c:	00012623          	sw	zero,12(sp)
    9e30:	00f12223          	sw	a5,4(sp)
    9e34:	00100a93          	li	s5,1
    9e38:	00000993          	li	s3,0
    9e3c:	00100913          	li	s2,1
    9e40:	00100713          	li	a4,1
    9e44:	3b9ad7b7          	lui	a5,0x3b9ad
    9e48:	a0078793          	addi	a5,a5,-1536 # 3b9aca00 <__freertos_irq_stack_top+0x3b8c4860>
    9e4c:	02fa05b3          	mul	a1,s4,a5
    9e50:	00e42023          	sw	a4,0(s0)
    9e54:	000a8613          	mv	a2,s5
    9e58:	00000693          	li	a3,0
    9e5c:	01442223          	sw	s4,4(s0)
    9e60:	02fa37b3          	mulhu	a5,s4,a5
    9e64:	01358533          	add	a0,a1,s3
    9e68:	00b535b3          	sltu	a1,a0,a1
    9e6c:	00f585b3          	add	a1,a1,a5
    9e70:	41c070ef          	jal	1128c <__udivdi3>
    9e74:	00a42423          	sw	a0,8(s0)
    9e78:	f8100837          	lui	a6,0xf8100
    9e7c:	0304a683          	lw	a3,48(s1)
    9e80:	01082783          	lw	a5,16(a6) # f8100010 <__freertos_irq_stack_top+0xf8017e70>
    9e84:	05f5e737          	lui	a4,0x5f5e
    9e88:	10070713          	addi	a4,a4,256 # 5f5e100 <__freertos_irq_stack_top+0x5e75f60>
    9e8c:	40d787b3          	sub	a5,a5,a3
    9e90:	02e785b3          	mul	a1,a5,a4
    9e94:	000a8613          	mv	a2,s5
    9e98:	00000693          	li	a3,0
    9e9c:	02e7b7b3          	mulhu	a5,a5,a4
    9ea0:	01358533          	add	a0,a1,s3
    9ea4:	00b535b3          	sltu	a1,a0,a1
    9ea8:	00f585b3          	add	a1,a1,a5
    9eac:	3e0070ef          	jal	1128c <__udivdi3>
    9eb0:	0144a783          	lw	a5,20(s1)
    9eb4:	06400693          	li	a3,100
    9eb8:	00a42623          	sw	a0,12(s0)
    9ebc:	02d7d633          	divu	a2,a5,a3
    9ec0:	00c4ad03          	lw	s10,12(s1)
    9ec4:	0104a703          	lw	a4,16(s1)
    9ec8:	0184ad83          	lw	s11,24(s1)
    9ecc:	0084a783          	lw	a5,8(s1)
    9ed0:	340a0663          	beqz	s4,a21c <win_end+0x490>
    9ed4:	02da05b3          	mul	a1,s4,a3
    9ed8:	02bd5533          	divu	a0,s10,a1
    9edc:	00a42823          	sw	a0,16(s0)
    9ee0:	30079863          	bnez	a5,a1f0 <win_end+0x464>
    9ee4:	02bdd6b3          	divu	a3,s11,a1
    9ee8:	01c4ac83          	lw	s9,28(s1)
    9eec:	0204ac03          	lw	s8,32(s1)
    9ef0:	00c42c23          	sw	a2,24(s0)
    9ef4:	019c0533          	add	a0,s8,s9
    9ef8:	01b50633          	add	a2,a0,s11
    9efc:	02bcde33          	divu	t3,s9,a1
    9f00:	00d42e23          	sw	a3,28(s0)
    9f04:	02b65633          	divu	a2,a2,a1
    9f08:	03c42023          	sw	t3,32(s0)
    9f0c:	02bc56b3          	divu	a3,s8,a1
    9f10:	00c787b3          	add	a5,a5,a2
    9f14:	02d42223          	sw	a3,36(s0)
    9f18:	0244a683          	lw	a3,36(s1)
    9f1c:	02c42623          	sw	a2,44(s0)
    9f20:	06400613          	li	a2,100
    9f24:	02c6d6b3          	divu	a3,a3,a2
    9f28:	02f42a23          	sw	a5,52(s0)
    9f2c:	02d42823          	sw	a3,48(s0)
    9f30:	00078863          	beqz	a5,9f40 <win_end+0x1b4>
    9f34:	009896b7          	lui	a3,0x989
    9f38:	68068693          	addi	a3,a3,1664 # 989680 <__freertos_irq_stack_top+0x8a14e0>
    9f3c:	02f6d7b3          	divu	a5,a3,a5
    9f40:	00ed05b3          	add	a1,s10,a4
    9f44:	01b585b3          	add	a1,a1,s11
    9f48:	02f42c23          	sw	a5,56(s0)
    9f4c:	00a585b3          	add	a1,a1,a0
    9f50:	2725f463          	bgeu	a1,s2,a1b8 <win_end+0x42c>
    9f54:	40b905b3          	sub	a1,s2,a1
    9f58:	06400793          	li	a5,100
    9f5c:	02f58b33          	mul	s6,a1,a5
    9f60:	02f5b7b3          	mulhu	a5,a1,a5
    9f64:	013b0bb3          	add	s7,s6,s3
    9f68:	016bbb33          	sltu	s6,s7,s6
    9f6c:	00fb0b33          	add	s6,s6,a5
    9f70:	00000793          	li	a5,0
    9f74:	000a0863          	beqz	s4,9f84 <win_end+0x1f8>
    9f78:	06400793          	li	a5,100
    9f7c:	02fa07b3          	mul	a5,s4,a5
    9f80:	02f5d7b3          	divu	a5,a1,a5
    9f84:	06400913          	li	s2,100
    9f88:	02f42423          	sw	a5,40(s0)
    9f8c:	000a8613          	mv	a2,s5
    9f90:	00000693          	li	a3,0
    9f94:	032707b3          	mul	a5,a4,s2
    9f98:	03273733          	mulhu	a4,a4,s2
    9f9c:	01378533          	add	a0,a5,s3
    9fa0:	00f537b3          	sltu	a5,a0,a5
    9fa4:	00e785b3          	add	a1,a5,a4
    9fa8:	2e4070ef          	jal	1128c <__udivdi3>
    9fac:	032d87b3          	mul	a5,s11,s2
    9fb0:	02a42e23          	sw	a0,60(s0)
    9fb4:	000a8613          	mv	a2,s5
    9fb8:	00000693          	li	a3,0
    9fbc:	032db5b3          	mulhu	a1,s11,s2
    9fc0:	01378533          	add	a0,a5,s3
    9fc4:	00f537b3          	sltu	a5,a0,a5
    9fc8:	00b785b3          	add	a1,a5,a1
    9fcc:	2c0070ef          	jal	1128c <__udivdi3>
    9fd0:	032c85b3          	mul	a1,s9,s2
    9fd4:	04a42023          	sw	a0,64(s0)
    9fd8:	000a8613          	mv	a2,s5
    9fdc:	00000693          	li	a3,0
    9fe0:	032cb8b3          	mulhu	a7,s9,s2
    9fe4:	01358533          	add	a0,a1,s3
    9fe8:	00b535b3          	sltu	a1,a0,a1
    9fec:	011585b3          	add	a1,a1,a7
    9ff0:	29c070ef          	jal	1128c <__udivdi3>
    9ff4:	032c05b3          	mul	a1,s8,s2
    9ff8:	04a42223          	sw	a0,68(s0)
    9ffc:	000a8613          	mv	a2,s5
    a000:	00000693          	li	a3,0
    a004:	032c3833          	mulhu	a6,s8,s2
    a008:	01358533          	add	a0,a1,s3
    a00c:	00b535b3          	sltu	a1,a0,a1
    a010:	010585b3          	add	a1,a1,a6
    a014:	278070ef          	jal	1128c <__udivdi3>
    a018:	032d05b3          	mul	a1,s10,s2
    a01c:	04a42423          	sw	a0,72(s0)
    a020:	000a8613          	mv	a2,s5
    a024:	00000693          	li	a3,0
    a028:	032d3933          	mulhu	s2,s10,s2
    a02c:	01358533          	add	a0,a1,s3
    a030:	00b535b3          	sltu	a1,a0,a1
    a034:	012585b3          	add	a1,a1,s2
    a038:	254070ef          	jal	1128c <__udivdi3>
    a03c:	04a42623          	sw	a0,76(s0)
    a040:	000a8613          	mv	a2,s5
    a044:	00000693          	li	a3,0
    a048:	000b8513          	mv	a0,s7
    a04c:	000b0593          	mv	a1,s6
    a050:	23c070ef          	jal	1128c <__udivdi3>
    a054:	000e47b7          	lui	a5,0xe4
    a058:	15478793          	addi	a5,a5,340 # e4154 <blit_st>
    a05c:	0444a683          	lw	a3,68(s1)
    a060:	0347a703          	lw	a4,52(a5)
    a064:	04a42823          	sw	a0,80(s0)
    a068:	02c4a603          	lw	a2,44(s1)
    a06c:	40d70733          	sub	a4,a4,a3
    a070:	160a0863          	beqz	s4,a1e0 <win_end+0x454>
    a074:	0284a683          	lw	a3,40(s1)
    a078:	0404a503          	lw	a0,64(s1)
    a07c:	0287a583          	lw	a1,40(a5)
    a080:	001a5793          	srli	a5,s4,0x1
    a084:	00d786b3          	add	a3,a5,a3
    a088:	40a787b3          	sub	a5,a5,a0
    a08c:	0346d6b3          	divu	a3,a3,s4
    a090:	00b787b3          	add	a5,a5,a1
    a094:	04c42c23          	sw	a2,88(s0)
    a098:	04e42e23          	sw	a4,92(s0)
    a09c:	0347da33          	divu	s4,a5,s4
    a0a0:	04d42a23          	sw	a3,84(s0)
    a0a4:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    a0a8:	07442023          	sw	s4,96(s0)
    a0ac:	10078e63          	beqz	a5,a1c8 <win_end+0x43c>
    a0b0:	f8101737          	lui	a4,0xf8101
    a0b4:	04072903          	lw	s2,64(a4) # f8101040 <__freertos_irq_stack_top+0xf8018ea0>
    a0b8:	0344a683          	lw	a3,52(s1)
    a0bc:	f81017b7          	lui	a5,0xf8101
    a0c0:	0487a983          	lw	s3,72(a5) # f8101048 <__freertos_irq_stack_top+0xf8018ea8>
    a0c4:	40d90933          	sub	s2,s2,a3
    a0c8:	f81016b7          	lui	a3,0xf8101
    a0cc:	04c6a803          	lw	a6,76(a3) # f810104c <__freertos_irq_stack_top+0xf8018eac>
    a0d0:	03c4a583          	lw	a1,60(s1)
    a0d4:	06400513          	li	a0,100
    a0d8:	0384a883          	lw	a7,56(s1)
    a0dc:	40b804b3          	sub	s1,a6,a1
    a0e0:	00812603          	lw	a2,8(sp)
    a0e4:	00c12683          	lw	a3,12(sp)
    a0e8:	411989b3          	sub	s3,s3,a7
    a0ec:	02a935b3          	mulhu	a1,s2,a0
    a0f0:	02a90533          	mul	a0,s2,a0
    a0f4:	198070ef          	jal	1128c <__udivdi3>
    a0f8:	00412a03          	lw	s4,4(sp)
    a0fc:	00000693          	li	a3,0
    a100:	00050b13          	mv	s6,a0
    a104:	000a0613          	mv	a2,s4
    a108:	00299513          	slli	a0,s3,0x2
    a10c:	01e9d593          	srli	a1,s3,0x1e
    a110:	17c070ef          	jal	1128c <__udivdi3>
    a114:	000a0613          	mv	a2,s4
    a118:	00050a93          	mv	s5,a0
    a11c:	00000693          	li	a3,0
    a120:	00249513          	slli	a0,s1,0x2
    a124:	01e4d593          	srli	a1,s1,0x1e
    a128:	164070ef          	jal	1128c <__udivdi3>
    a12c:	00050a13          	mv	s4,a0
    a130:	10090a63          	beqz	s2,a244 <win_end+0x4b8>
    a134:	19000b93          	li	s7,400
    a138:	0379b5b3          	mulhu	a1,s3,s7
    a13c:	00090613          	mv	a2,s2
    a140:	00000693          	li	a3,0
    a144:	03798533          	mul	a0,s3,s7
    a148:	144070ef          	jal	1128c <__udivdi3>
    a14c:	0374b5b3          	mulhu	a1,s1,s7
    a150:	00050993          	mv	s3,a0
    a154:	00090613          	mv	a2,s2
    a158:	00000693          	li	a3,0
    a15c:	03748533          	mul	a0,s1,s7
    a160:	12c070ef          	jal	1128c <__udivdi3>
    a164:	00050913          	mv	s2,a0
    a168:	07642223          	sw	s6,100(s0)
    a16c:	07542423          	sw	s5,104(s0)
    a170:	07442623          	sw	s4,108(s0)
    a174:	07342823          	sw	s3,112(s0)
    a178:	07242a23          	sw	s2,116(s0)
    a17c:	04c12083          	lw	ra,76(sp)
    a180:	04812403          	lw	s0,72(sp)
    a184:	04412483          	lw	s1,68(sp)
    a188:	04012903          	lw	s2,64(sp)
    a18c:	03c12983          	lw	s3,60(sp)
    a190:	03812a03          	lw	s4,56(sp)
    a194:	03412a83          	lw	s5,52(sp)
    a198:	03012b03          	lw	s6,48(sp)
    a19c:	02c12b83          	lw	s7,44(sp)
    a1a0:	02812c03          	lw	s8,40(sp)
    a1a4:	02412c83          	lw	s9,36(sp)
    a1a8:	02012d03          	lw	s10,32(sp)
    a1ac:	01c12d83          	lw	s11,28(sp)
    a1b0:	05010113          	addi	sp,sp,80
    a1b4:	00008067          	ret
    a1b8:	00098b93          	mv	s7,s3
    a1bc:	00000b13          	li	s6,0
    a1c0:	00000593          	li	a1,0
    a1c4:	dadff06f          	j	9f70 <win_end+0x1e4>
    a1c8:	00000b13          	li	s6,0
    a1cc:	00000a93          	li	s5,0
    a1d0:	00000a13          	li	s4,0
    a1d4:	00000993          	li	s3,0
    a1d8:	00000913          	li	s2,0
    a1dc:	f8dff06f          	j	a168 <win_end+0x3dc>
    a1e0:	04042a23          	sw	zero,84(s0)
    a1e4:	04c42c23          	sw	a2,88(s0)
    a1e8:	04e42e23          	sw	a4,92(s0)
    a1ec:	eb9ff06f          	j	a0a4 <win_end+0x318>
    a1f0:	02d786b3          	mul	a3,a5,a3
    a1f4:	02d757b3          	divu	a5,a4,a3
    a1f8:	00f42a23          	sw	a5,20(s0)
    a1fc:	ce9ff06f          	j	9ee4 <win_end+0x158>
    a200:	06400793          	li	a5,100
    a204:	00f12423          	sw	a5,8(sp)
    a208:	00100793          	li	a5,1
    a20c:	00012623          	sw	zero,12(sp)
    a210:	00f12223          	sw	a5,4(sp)
    a214:	00100713          	li	a4,1
    a218:	c2dff06f          	j	9e44 <win_end+0xb8>
    a21c:	00078863          	beqz	a5,a22c <win_end+0x4a0>
    a220:	02d786b3          	mul	a3,a5,a3
    a224:	02d757b3          	divu	a5,a4,a3
    a228:	00f42a23          	sw	a5,20(s0)
    a22c:	01c4ac83          	lw	s9,28(s1)
    a230:	0204ac03          	lw	s8,32(s1)
    a234:	00c42c23          	sw	a2,24(s0)
    a238:	00000613          	li	a2,0
    a23c:	019c0533          	add	a0,s8,s9
    a240:	cd9ff06f          	j	9f18 <win_end+0x18c>
    a244:	00000993          	li	s3,0
    a248:	f21ff06f          	j	a168 <win_end+0x3dc>

0000a24c <bsp_printf>:
    a24c:	fa010113          	addi	sp,sp,-96
    a250:	00054303          	lbu	t1,0(a0)
    a254:	04f12a23          	sw	a5,84(sp)
    a258:	04410793          	addi	a5,sp,68
    a25c:	04b12223          	sw	a1,68(sp)
    a260:	04c12423          	sw	a2,72(sp)
    a264:	04d12623          	sw	a3,76(sp)
    a268:	04e12823          	sw	a4,80(sp)
    a26c:	05012c23          	sw	a6,88(sp)
    a270:	05112e23          	sw	a7,92(sp)
    a274:	00f12623          	sw	a5,12(sp)
    a278:	04030a63          	beqz	t1,a2cc <bsp_printf+0x80>
    a27c:	f8010737          	lui	a4,0xf8010
    a280:	00013eb7          	lui	t4,0x13
    a284:	02812e23          	sw	s0,60(sp)
    a288:	00000613          	li	a2,0
    a28c:	02500893          	li	a7,37
    a290:	02000f13          	li	t5,32
    a294:	db4e8e93          	addi	t4,t4,-588 # 12db4 <gfx_hw_ops+0x18>
    a298:	f8010e37          	lui	t3,0xf8010
    a29c:	00470713          	addi	a4,a4,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a2a0:	03130a63          	beq	t1,a7,a2d4 <bsp_printf+0x88>
    a2a4:	00072783          	lw	a5,0(a4)
    a2a8:	0107d693          	srli	a3,a5,0x10
    a2ac:	0ff6f693          	zext.b	a3,a3
    a2b0:	fe068ae3          	beqz	a3,a2a4 <bsp_printf+0x58>
    a2b4:	006e2023          	sw	t1,0(t3) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a2b8:	00160613          	addi	a2,a2,1
    a2bc:	00c507b3          	add	a5,a0,a2
    a2c0:	0007c303          	lbu	t1,0(a5)
    a2c4:	fc031ee3          	bnez	t1,a2a0 <bsp_printf+0x54>
    a2c8:	03c12403          	lw	s0,60(sp)
    a2cc:	06010113          	addi	sp,sp,96
    a2d0:	00008067          	ret
    a2d4:	00160613          	addi	a2,a2,1
    a2d8:	00c507b3          	add	a5,a0,a2
    a2dc:	0007c783          	lbu	a5,0(a5)
    a2e0:	fc078ce3          	beqz	a5,a2b8 <bsp_printf+0x6c>
    a2e4:	fa878793          	addi	a5,a5,-88
    a2e8:	0ff7f793          	zext.b	a5,a5
    a2ec:	feff64e3          	bltu	t5,a5,a2d4 <bsp_printf+0x88>
    a2f0:	00279793          	slli	a5,a5,0x2
    a2f4:	01d787b3          	add	a5,a5,t4
    a2f8:	0007a783          	lw	a5,0(a5)
    a2fc:	00078067          	jr	a5
    a300:	00c12783          	lw	a5,12(sp)
    a304:	f80106b7          	lui	a3,0xf8010
    a308:	00012fb7          	lui	t6,0x12
    a30c:	0007a403          	lw	s0,0(a5)
    a310:	00478793          	addi	a5,a5,4
    a314:	00f12623          	sw	a5,12(sp)
    a318:	01c00813          	li	a6,28
    a31c:	cd0f8f93          	addi	t6,t6,-816 # 11cd0 <_data+0x594>
    a320:	f80103b7          	lui	t2,0xf8010
    a324:	00468693          	addi	a3,a3,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a328:	ffc00293          	li	t0,-4
    a32c:	010457b3          	srl	a5,s0,a6
    a330:	00f7f793          	andi	a5,a5,15
    a334:	00ff87b3          	add	a5,t6,a5
    a338:	0007c303          	lbu	t1,0(a5)
    a33c:	0006a783          	lw	a5,0(a3)
    a340:	0107d593          	srli	a1,a5,0x10
    a344:	0ff5f593          	zext.b	a1,a1
    a348:	fe058ae3          	beqz	a1,a33c <bsp_printf+0xf0>
    a34c:	0063a023          	sw	t1,0(t2) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a350:	ffc80813          	addi	a6,a6,-4
    a354:	fc581ce3          	bne	a6,t0,a32c <bsp_printf+0xe0>
    a358:	f61ff06f          	j	a2b8 <bsp_printf+0x6c>
    a35c:	00c12783          	lw	a5,12(sp)
    a360:	0007a803          	lw	a6,0(a5)
    a364:	00478793          	addi	a5,a5,4
    a368:	00f12623          	sw	a5,12(sp)
    a36c:	00084303          	lbu	t1,0(a6)
    a370:	f40304e3          	beqz	t1,a2b8 <bsp_printf+0x6c>
    a374:	f80106b7          	lui	a3,0xf8010
    a378:	f8010fb7          	lui	t6,0xf8010
    a37c:	00468693          	addi	a3,a3,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a380:	00180813          	addi	a6,a6,1
    a384:	0006a783          	lw	a5,0(a3)
    a388:	0107d593          	srli	a1,a5,0x10
    a38c:	0ff5f593          	zext.b	a1,a1
    a390:	fe058ae3          	beqz	a1,a384 <bsp_printf+0x138>
    a394:	006fa023          	sw	t1,0(t6) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a398:	00084303          	lbu	t1,0(a6)
    a39c:	fe0312e3          	bnez	t1,a380 <bsp_printf+0x134>
    a3a0:	f19ff06f          	j	a2b8 <bsp_printf+0x6c>
    a3a4:	00c12683          	lw	a3,12(sp)
    a3a8:	0006a783          	lw	a5,0(a3)
    a3ac:	00468693          	addi	a3,a3,4
    a3b0:	00d12623          	sw	a3,12(sp)
    a3b4:	1207c863          	bltz	a5,a4e4 <bsp_printf+0x298>
    a3b8:	01010313          	addi	t1,sp,16
    a3bc:	00030693          	mv	a3,t1
    a3c0:	00a00813          	li	a6,10
    a3c4:	00079463          	bnez	a5,a3cc <bsp_printf+0x180>
    a3c8:	0a669863          	bne	a3,t1,a478 <bsp_printf+0x22c>
    a3cc:	0307e5b3          	rem	a1,a5,a6
    a3d0:	00168693          	addi	a3,a3,1
    a3d4:	03058593          	addi	a1,a1,48
    a3d8:	0307c7b3          	div	a5,a5,a6
    a3dc:	feb68fa3          	sb	a1,-1(a3)
    a3e0:	fe5ff06f          	j	a3c4 <bsp_printf+0x178>
    a3e4:	00c12783          	lw	a5,12(sp)
    a3e8:	f80106b7          	lui	a3,0xf8010
    a3ec:	00468693          	addi	a3,a3,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a3f0:	0007a803          	lw	a6,0(a5)
    a3f4:	00478793          	addi	a5,a5,4
    a3f8:	00f12623          	sw	a5,12(sp)
    a3fc:	0006a783          	lw	a5,0(a3)
    a400:	0107d593          	srli	a1,a5,0x10
    a404:	0ff5f593          	zext.b	a1,a1
    a408:	fe058ae3          	beqz	a1,a3fc <bsp_printf+0x1b0>
    a40c:	0ff87793          	zext.b	a5,a6
    a410:	f80106b7          	lui	a3,0xf8010
    a414:	00f6a023          	sw	a5,0(a3) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a418:	ea1ff06f          	j	a2b8 <bsp_printf+0x6c>
    a41c:	00c12783          	lw	a5,12(sp)
    a420:	f80106b7          	lui	a3,0xf8010
    a424:	00012fb7          	lui	t6,0x12
    a428:	0007a403          	lw	s0,0(a5)
    a42c:	00478793          	addi	a5,a5,4
    a430:	00f12623          	sw	a5,12(sp)
    a434:	01c00813          	li	a6,28
    a438:	ce4f8f93          	addi	t6,t6,-796 # 11ce4 <_data+0x5a8>
    a43c:	f80103b7          	lui	t2,0xf8010
    a440:	00468693          	addi	a3,a3,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a444:	ffc00293          	li	t0,-4
    a448:	010457b3          	srl	a5,s0,a6
    a44c:	00f7f793          	andi	a5,a5,15
    a450:	00ff87b3          	add	a5,t6,a5
    a454:	0007c303          	lbu	t1,0(a5)
    a458:	0006a783          	lw	a5,0(a3)
    a45c:	0107d593          	srli	a1,a5,0x10
    a460:	0ff5f593          	zext.b	a1,a1
    a464:	fe058ae3          	beqz	a1,a458 <bsp_printf+0x20c>
    a468:	0063a023          	sw	t1,0(t2) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a46c:	ffc80813          	addi	a6,a6,-4
    a470:	fc581ce3          	bne	a6,t0,a448 <bsp_printf+0x1fc>
    a474:	e45ff06f          	j	a2b8 <bsp_printf+0x6c>
    a478:	f80105b7          	lui	a1,0xf8010
    a47c:	f80102b7          	lui	t0,0xf8010
    a480:	00458593          	addi	a1,a1,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a484:	fff6cf83          	lbu	t6,-1(a3)
    a488:	fff68693          	addi	a3,a3,-1
    a48c:	0005a783          	lw	a5,0(a1)
    a490:	0107d813          	srli	a6,a5,0x10
    a494:	0ff87813          	zext.b	a6,a6
    a498:	fe080ae3          	beqz	a6,a48c <bsp_printf+0x240>
    a49c:	01f2a023          	sw	t6,0(t0) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a4a0:	fe6692e3          	bne	a3,t1,a484 <bsp_printf+0x238>
    a4a4:	e15ff06f          	j	a2b8 <bsp_printf+0x6c>
    a4a8:	f80106b7          	lui	a3,0xf8010
    a4ac:	00012837          	lui	a6,0x12
    a4b0:	03c00313          	li	t1,60
    a4b4:	c8480813          	addi	a6,a6,-892 # 11c84 <_data+0x548>
    a4b8:	f8010fb7          	lui	t6,0xf8010
    a4bc:	00468693          	addi	a3,a3,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a4c0:	00180813          	addi	a6,a6,1
    a4c4:	0006a783          	lw	a5,0(a3)
    a4c8:	0107d593          	srli	a1,a5,0x10
    a4cc:	0ff5f593          	zext.b	a1,a1
    a4d0:	fe058ae3          	beqz	a1,a4c4 <bsp_printf+0x278>
    a4d4:	006fa023          	sw	t1,0(t6) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a4d8:	00084303          	lbu	t1,0(a6)
    a4dc:	fe0312e3          	bnez	t1,a4c0 <bsp_printf+0x274>
    a4e0:	dd9ff06f          	j	a2b8 <bsp_printf+0x6c>
    a4e4:	f80105b7          	lui	a1,0xf8010
    a4e8:	00458593          	addi	a1,a1,4 # f8010004 <__freertos_irq_stack_top+0xf7f27e64>
    a4ec:	0005a683          	lw	a3,0(a1)
    a4f0:	0106d813          	srli	a6,a3,0x10
    a4f4:	0ff87813          	zext.b	a6,a6
    a4f8:	fe080ae3          	beqz	a6,a4ec <bsp_printf+0x2a0>
    a4fc:	f80106b7          	lui	a3,0xf8010
    a500:	02d00593          	li	a1,45
    a504:	00b6a023          	sw	a1,0(a3) # f8010000 <__freertos_irq_stack_top+0xf7f27e60>
    a508:	40f007b3          	neg	a5,a5
    a50c:	eadff06f          	j	a3b8 <bsp_printf+0x16c>

0000a510 <tx_puts>:
    a510:	ff010113          	addi	sp,sp,-16
    a514:	00812423          	sw	s0,8(sp)
    a518:	00112623          	sw	ra,12(sp)
    a51c:	00050413          	mv	s0,a0
    a520:	e65f60ef          	jal	1384 <strlen>
    a524:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    a528:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    a52c:	000015b7          	lui	a1,0x1
    a530:	fff58593          	addi	a1,a1,-1 # fff <CUSTOM2+0xfa4>
    a534:	fff78793          	addi	a5,a5,-1
    a538:	40e787b3          	sub	a5,a5,a4
    a53c:	00b7f7b3          	and	a5,a5,a1
    a540:	04f57663          	bgeu	a0,a5,a58c <tx_puts+0x7c>
    a544:	00044603          	lbu	a2,0(s0)
    a548:	000ae537          	lui	a0,0xae
    a54c:	f5050513          	addi	a0,a0,-176 # adf50 <tx_buf>
    a550:	02060663          	beqz	a2,a57c <tx_puts+0x6c>
    a554:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    a558:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    a55c:	00140413          	addi	s0,s0,1
    a560:	00e50733          	add	a4,a0,a4
    a564:	00c70023          	sb	a2,0(a4)
    a568:	00178793          	addi	a5,a5,1
    a56c:	00044603          	lbu	a2,0(s0)
    a570:	00b7f7b3          	and	a5,a5,a1
    a574:	88f1aa23          	sw	a5,-1900(gp) # 1349c <tx_head>
    a578:	fc061ee3          	bnez	a2,a554 <tx_puts+0x44>
    a57c:	00c12083          	lw	ra,12(sp)
    a580:	00812403          	lw	s0,8(sp)
    a584:	01010113          	addi	sp,sp,16
    a588:	00008067          	ret
    a58c:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    a590:	00c12083          	lw	ra,12(sp)
    a594:	00812403          	lw	s0,8(sp)
    a598:	00178793          	addi	a5,a5,1
    a59c:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    a5a0:	01010113          	addi	sp,sp,16
    a5a4:	00008067          	ret

0000a5a8 <bandwidth_test.constprop.0>:
    a5a8:	f9010113          	addi	sp,sp,-112
    a5ac:	28000793          	li	a5,640
    a5b0:	00f12423          	sw	a5,8(sp)
    a5b4:	1e000793          	li	a5,480
    a5b8:	00f12623          	sw	a5,12(sp)
    a5bc:	014007b7          	lui	a5,0x1400
    a5c0:	05312e23          	sw	s3,92(sp)
    a5c4:	05812423          	sw	s8,72(sp)
    a5c8:	05a12023          	sw	s10,64(sp)
    a5cc:	06112623          	sw	ra,108(sp)
    a5d0:	06812423          	sw	s0,104(sp)
    a5d4:	06912223          	sw	s1,100(sp)
    a5d8:	07212023          	sw	s2,96(sp)
    a5dc:	05412c23          	sw	s4,88(sp)
    a5e0:	05512a23          	sw	s5,84(sp)
    a5e4:	05612823          	sw	s6,80(sp)
    a5e8:	05712623          	sw	s7,76(sp)
    a5ec:	05912223          	sw	s9,68(sp)
    a5f0:	00f12823          	sw	a5,16(sp)
    a5f4:	00012a23          	sw	zero,20(sp)
    a5f8:	00050d13          	mv	s10,a0
    a5fc:	00058c13          	mv	s8,a1
    a600:	01810993          	addi	s3,sp,24
    a604:	12050863          	beqz	a0,a734 <bandwidth_test.constprop.0+0x18c>
    a608:	1e000713          	li	a4,480
    a60c:	28000693          	li	a3,640
    a610:	01200637          	lui	a2,0x1200
    a614:	00010593          	mv	a1,sp
    a618:	00098513          	mv	a0,s3
    a61c:	f54ff0ef          	jal	9d70 <gfx_begin_hw>
    a620:	000017b7          	lui	a5,0x1
    a624:	f8b0c837          	lui	a6,0xf8b0c
    a628:	1e000713          	li	a4,480
    a62c:	0e378793          	addi	a5,a5,227 # 10e3 <__libc_init_array+0x73>
    a630:	28000693          	li	a3,640
    a634:	00000613          	li	a2,0
    a638:	00000593          	li	a1,0
    a63c:	00098513          	mv	a0,s3
    a640:	ff882483          	lw	s1,-8(a6) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a644:	e98fe0ef          	jal	8cdc <gfx_fill_rect>
    a648:	f8101737          	lui	a4,0xf8101
    a64c:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
    a650:	00072783          	lw	a5,0(a4)
    a654:	0017f793          	andi	a5,a5,1
    a658:	fe079ce3          	bnez	a5,a650 <bandwidth_test.constprop.0+0xa8>
    a65c:	000e4437          	lui	s0,0xe4
    a660:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    a664:	f8b0c837          	lui	a6,0xf8b0c
    a668:	02042223          	sw	zero,36(s0)
    a66c:	ff882783          	lw	a5,-8(a6) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a670:	06300713          	li	a4,99
    a674:	ff880813          	addi	a6,a6,-8
    a678:	409787b3          	sub	a5,a5,s1
    a67c:	2af77263          	bgeu	a4,a5,a920 <bandwidth_test.constprop.0+0x378>
    a680:	06400713          	li	a4,100
    a684:	02e7d7b3          	divu	a5,a5,a4
    a688:	f8b0c837          	lui	a6,0xf8b0c
    a68c:	00000713          	li	a4,0
    a690:	00810693          	addi	a3,sp,8
    a694:	00000613          	li	a2,0
    a698:	00000593          	li	a1,0
    a69c:	00098513          	mv	a0,s3
    a6a0:	00fc2023          	sw	a5,0(s8)
    a6a4:	ff882483          	lw	s1,-8(a6) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a6a8:	e44fe0ef          	jal	8cec <gfx_blit>
    a6ac:	0e0d0e63          	beqz	s10,a7a8 <bandwidth_test.constprop.0+0x200>
    a6b0:	000e4437          	lui	s0,0xe4
    a6b4:	15440413          	addi	s0,s0,340 # e4154 <blit_st>
    a6b8:	f8101737          	lui	a4,0xf8101
    a6bc:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
    a6c0:	00072783          	lw	a5,0(a4)
    a6c4:	0017f793          	andi	a5,a5,1
    a6c8:	fe079ce3          	bnez	a5,a6c0 <bandwidth_test.constprop.0+0x118>
    a6cc:	f8b0c337          	lui	t1,0xf8b0c
    a6d0:	02042223          	sw	zero,36(s0)
    a6d4:	ff832783          	lw	a5,-8(t1) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a6d8:	06300713          	li	a4,99
    a6dc:	ff830313          	addi	t1,t1,-8
    a6e0:	409787b3          	sub	a5,a5,s1
    a6e4:	28f77863          	bgeu	a4,a5,a974 <bandwidth_test.constprop.0+0x3cc>
    a6e8:	06400713          	li	a4,100
    a6ec:	02e7d8b3          	divu	a7,a5,a4
    a6f0:	09600813          	li	a6,150
    a6f4:	00000793          	li	a5,0
    a6f8:	1e000713          	li	a4,480
    a6fc:	28000693          	li	a3,640
    a700:	00000613          	li	a2,0
    a704:	00000593          	li	a1,0
    a708:	00098513          	mv	a0,s3
    a70c:	011c2223          	sw	a7,4(s8)
    a710:	00032483          	lw	s1,0(t1)
    a714:	fa0fe0ef          	jal	8eb4 <gfx_fill_alpha>
    a718:	f8101737          	lui	a4,0xf8101
    a71c:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
    a720:	00072783          	lw	a5,0(a4)
    a724:	0017f793          	andi	a5,a5,1
    a728:	fe079ce3          	bnez	a5,a720 <bandwidth_test.constprop.0+0x178>
    a72c:	02042223          	sw	zero,36(s0)
    a730:	0bc0006f          	j	a7ec <bandwidth_test.constprop.0+0x244>
    a734:	1e000693          	li	a3,480
    a738:	28000613          	li	a2,640
    a73c:	012005b7          	lui	a1,0x1200
    a740:	00098513          	mv	a0,s3
    a744:	d10fe0ef          	jal	8c54 <gfx_begin>
    a748:	f8b0c437          	lui	s0,0xf8b0c
    a74c:	000017b7          	lui	a5,0x1
    a750:	0e378793          	addi	a5,a5,227 # 10e3 <__libc_init_array+0x73>
    a754:	1e000713          	li	a4,480
    a758:	28000693          	li	a3,640
    a75c:	00000613          	li	a2,0
    a760:	00000593          	li	a1,0
    a764:	00098513          	mv	a0,s3
    a768:	ff842483          	lw	s1,-8(s0) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a76c:	d70fe0ef          	jal	8cdc <gfx_fill_rect>
    a770:	ff842783          	lw	a5,-8(s0)
    a774:	06300713          	li	a4,99
    a778:	ff840413          	addi	s0,s0,-8
    a77c:	409787b3          	sub	a5,a5,s1
    a780:	f0f760e3          	bltu	a4,a5,a680 <bandwidth_test.constprop.0+0xd8>
    a784:	00100793          	li	a5,1
    a788:	00fc2023          	sw	a5,0(s8)
    a78c:	00000713          	li	a4,0
    a790:	00810693          	addi	a3,sp,8
    a794:	00000613          	li	a2,0
    a798:	00000593          	li	a1,0
    a79c:	00098513          	mv	a0,s3
    a7a0:	00042483          	lw	s1,0(s0)
    a7a4:	d48fe0ef          	jal	8cec <gfx_blit>
    a7a8:	f8b0c337          	lui	t1,0xf8b0c
    a7ac:	ff832783          	lw	a5,-8(t1) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a7b0:	06300713          	li	a4,99
    a7b4:	ff830313          	addi	t1,t1,-8
    a7b8:	409787b3          	sub	a5,a5,s1
    a7bc:	12f76863          	bltu	a4,a5,a8ec <bandwidth_test.constprop.0+0x344>
    a7c0:	00100793          	li	a5,1
    a7c4:	00fc2223          	sw	a5,4(s8)
    a7c8:	09600813          	li	a6,150
    a7cc:	00000793          	li	a5,0
    a7d0:	1e000713          	li	a4,480
    a7d4:	28000693          	li	a3,640
    a7d8:	00000613          	li	a2,0
    a7dc:	00000593          	li	a1,0
    a7e0:	00098513          	mv	a0,s3
    a7e4:	00032483          	lw	s1,0(t1)
    a7e8:	eccfe0ef          	jal	8eb4 <gfx_fill_alpha>
    a7ec:	f8b0c737          	lui	a4,0xf8b0c
    a7f0:	ff872783          	lw	a5,-8(a4) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a7f4:	06300693          	li	a3,99
    a7f8:	00100713          	li	a4,1
    a7fc:	409787b3          	sub	a5,a5,s1
    a800:	00f6f663          	bgeu	a3,a5,a80c <bandwidth_test.constprop.0+0x264>
    a804:	06400713          	li	a4,100
    a808:	02e7d733          	divu	a4,a5,a4
    a80c:	f8b0c7b7          	lui	a5,0xf8b0c
    a810:	00ec2423          	sw	a4,8(s8)
    a814:	ff87ac83          	lw	s9,-8(a5) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a818:	9e378ab7          	lui	s5,0x9e378
    a81c:	00012a37          	lui	s4,0x12
    a820:	00000913          	li	s2,0
    a824:	00000493          	li	s1,0
    a828:	00000413          	li	s0,0
    a82c:	1d800b93          	li	s7,472
    a830:	27800b13          	li	s6,632
    a834:	9b1a8a93          	addi	s5,s5,-1615 # 9e3779b1 <__freertos_irq_stack_top+0x9e28f811>
    a838:	110a0a13          	addi	s4,s4,272 # 12110 <_data+0x9d4>
    a83c:	01095793          	srli	a5,s2,0x10
    a840:	00800713          	li	a4,8
    a844:	00800693          	li	a3,8
    a848:	00098513          	mv	a0,s3
    a84c:	01590933          	add	s2,s2,s5
    a850:	0374e633          	rem	a2,s1,s7
    a854:	03548493          	addi	s1,s1,53
    a858:	036465b3          	rem	a1,s0,s6
    a85c:	02540413          	addi	s0,s0,37
    a860:	c7cfe0ef          	jal	8cdc <gfx_fill_rect>
    a864:	fd441ce3          	bne	s0,s4,a83c <bandwidth_test.constprop.0+0x294>
    a868:	0e0d0063          	beqz	s10,a948 <bandwidth_test.constprop.0+0x3a0>
    a86c:	f8101737          	lui	a4,0xf8101
    a870:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
    a874:	00072783          	lw	a5,0(a4)
    a878:	0017f793          	andi	a5,a5,1
    a87c:	fe079ce3          	bnez	a5,a874 <bandwidth_test.constprop.0+0x2cc>
    a880:	000e47b7          	lui	a5,0xe4
    a884:	f8b0c737          	lui	a4,0xf8b0c
    a888:	1607ac23          	sw	zero,376(a5) # e4178 <blit_st+0x24>
    a88c:	ff872783          	lw	a5,-8(a4) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a890:	06300693          	li	a3,99
    a894:	419787b3          	sub	a5,a5,s9
    a898:	10f6f663          	bgeu	a3,a5,a9a4 <bandwidth_test.constprop.0+0x3fc>
    a89c:	06400713          	li	a4,100
    a8a0:	02e7d7b3          	divu	a5,a5,a4
    a8a4:	00098513          	mv	a0,s3
    a8a8:	00fc2623          	sw	a5,12(s8)
    a8ac:	c14fe0ef          	jal	8cc0 <gfx_end>
    a8b0:	0000500f          	.word	0x0000500f
    a8b4:	06c12083          	lw	ra,108(sp)
    a8b8:	06812403          	lw	s0,104(sp)
    a8bc:	06412483          	lw	s1,100(sp)
    a8c0:	06012903          	lw	s2,96(sp)
    a8c4:	05c12983          	lw	s3,92(sp)
    a8c8:	05812a03          	lw	s4,88(sp)
    a8cc:	05412a83          	lw	s5,84(sp)
    a8d0:	05012b03          	lw	s6,80(sp)
    a8d4:	04c12b83          	lw	s7,76(sp)
    a8d8:	04812c03          	lw	s8,72(sp)
    a8dc:	04412c83          	lw	s9,68(sp)
    a8e0:	04012d03          	lw	s10,64(sp)
    a8e4:	07010113          	addi	sp,sp,112
    a8e8:	00008067          	ret
    a8ec:	06400713          	li	a4,100
    a8f0:	02e7d8b3          	divu	a7,a5,a4
    a8f4:	09600813          	li	a6,150
    a8f8:	00000793          	li	a5,0
    a8fc:	1e000713          	li	a4,480
    a900:	28000693          	li	a3,640
    a904:	00000613          	li	a2,0
    a908:	00000593          	li	a1,0
    a90c:	00098513          	mv	a0,s3
    a910:	011c2223          	sw	a7,4(s8)
    a914:	00032483          	lw	s1,0(t1)
    a918:	d9cfe0ef          	jal	8eb4 <gfx_fill_alpha>
    a91c:	ed1ff06f          	j	a7ec <bandwidth_test.constprop.0+0x244>
    a920:	00100793          	li	a5,1
    a924:	00fc2023          	sw	a5,0(s8)
    a928:	00000713          	li	a4,0
    a92c:	00810693          	addi	a3,sp,8
    a930:	00000613          	li	a2,0
    a934:	00000593          	li	a1,0
    a938:	00098513          	mv	a0,s3
    a93c:	00082483          	lw	s1,0(a6)
    a940:	bacfe0ef          	jal	8cec <gfx_blit>
    a944:	d75ff06f          	j	a6b8 <bandwidth_test.constprop.0+0x110>
    a948:	f8b0c737          	lui	a4,0xf8b0c
    a94c:	ff872783          	lw	a5,-8(a4) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
    a950:	06300693          	li	a3,99
    a954:	419787b3          	sub	a5,a5,s9
    a958:	06f6f063          	bgeu	a3,a5,a9b8 <bandwidth_test.constprop.0+0x410>
    a95c:	06400713          	li	a4,100
    a960:	02e7d7b3          	divu	a5,a5,a4
    a964:	00098513          	mv	a0,s3
    a968:	00fc2623          	sw	a5,12(s8)
    a96c:	b54fe0ef          	jal	8cc0 <gfx_end>
    a970:	f45ff06f          	j	a8b4 <bandwidth_test.constprop.0+0x30c>
    a974:	00100793          	li	a5,1
    a978:	00fc2223          	sw	a5,4(s8)
    a97c:	09600813          	li	a6,150
    a980:	00000793          	li	a5,0
    a984:	1e000713          	li	a4,480
    a988:	28000693          	li	a3,640
    a98c:	00000613          	li	a2,0
    a990:	00000593          	li	a1,0
    a994:	00098513          	mv	a0,s3
    a998:	00032483          	lw	s1,0(t1)
    a99c:	d18fe0ef          	jal	8eb4 <gfx_fill_alpha>
    a9a0:	d79ff06f          	j	a718 <bandwidth_test.constprop.0+0x170>
    a9a4:	00100793          	li	a5,1
    a9a8:	00fc2623          	sw	a5,12(s8)
    a9ac:	00098513          	mv	a0,s3
    a9b0:	b10fe0ef          	jal	8cc0 <gfx_end>
    a9b4:	efdff06f          	j	a8b0 <bandwidth_test.constprop.0+0x308>
    a9b8:	00100793          	li	a5,1
    a9bc:	00fc2623          	sw	a5,12(s8)
    a9c0:	00098513          	mv	a0,s3
    a9c4:	afcfe0ef          	jal	8cc0 <gfx_end>
    a9c8:	eedff06f          	j	a8b4 <bandwidth_test.constprop.0+0x30c>

0000a9cc <tx_printf>:
    a9cc:	ec010113          	addi	sp,sp,-320
    a9d0:	12410313          	addi	t1,sp,292
    a9d4:	12b12223          	sw	a1,292(sp)
    a9d8:	12c12423          	sw	a2,296(sp)
    a9dc:	12d12623          	sw	a3,300(sp)
    a9e0:	00050613          	mv	a2,a0
    a9e4:	00030693          	mv	a3,t1
    a9e8:	10000593          	li	a1,256
    a9ec:	01010513          	addi	a0,sp,16
    a9f0:	12e12823          	sw	a4,304(sp)
    a9f4:	12f12a23          	sw	a5,308(sp)
    a9f8:	13012c23          	sw	a6,312(sp)
    a9fc:	10112e23          	sw	ra,284(sp)
    aa00:	13112e23          	sw	a7,316(sp)
    aa04:	00612623          	sw	t1,12(sp)
    aa08:	4e4050ef          	jal	feec <mini_vsnprintf>
    aa0c:	01010513          	addi	a0,sp,16
    aa10:	975f60ef          	jal	1384 <strlen>
    aa14:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    aa18:	8901a783          	lw	a5,-1904(gp) # 13498 <tx_tail>
    aa1c:	00001837          	lui	a6,0x1
    aa20:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    aa24:	fff78793          	addi	a5,a5,-1
    aa28:	40e787b3          	sub	a5,a5,a4
    aa2c:	0107f7b3          	and	a5,a5,a6
    aa30:	04f57663          	bgeu	a0,a5,aa7c <tx_printf+0xb0>
    aa34:	01014583          	lbu	a1,16(sp)
    aa38:	02058c63          	beqz	a1,aa70 <tx_printf+0xa4>
    aa3c:	000ae537          	lui	a0,0xae
    aa40:	01010693          	addi	a3,sp,16
    aa44:	f5050513          	addi	a0,a0,-176 # adf50 <tx_buf>
    aa48:	8941a703          	lw	a4,-1900(gp) # 1349c <tx_head>
    aa4c:	8941a783          	lw	a5,-1900(gp) # 1349c <tx_head>
    aa50:	00168693          	addi	a3,a3,1
    aa54:	00e50733          	add	a4,a0,a4
    aa58:	00b70023          	sb	a1,0(a4)
    aa5c:	00178793          	addi	a5,a5,1
    aa60:	0006c583          	lbu	a1,0(a3)
    aa64:	0107f7b3          	and	a5,a5,a6
    aa68:	88f1aa23          	sw	a5,-1900(gp) # 1349c <tx_head>
    aa6c:	fc059ee3          	bnez	a1,aa48 <tx_printf+0x7c>
    aa70:	11c12083          	lw	ra,284(sp)
    aa74:	14010113          	addi	sp,sp,320
    aa78:	00008067          	ret
    aa7c:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    aa80:	11c12083          	lw	ra,284(sp)
    aa84:	00178793          	addi	a5,a5,1
    aa88:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    aa8c:	14010113          	addi	sp,sp,320
    aa90:	00008067          	ret

0000aa94 <change_load>:
    aa94:	8681a783          	lw	a5,-1944(gp) # 13470 <scene>
    aa98:	ff010113          	addi	sp,sp,-16
    aa9c:	00112623          	sw	ra,12(sp)
    aaa0:	00100713          	li	a4,1
    aaa4:	06e78263          	beq	a5,a4,ab08 <change_load+0x74>
    aaa8:	00200713          	li	a4,2
    aaac:	00e78c63          	beq	a5,a4,aac4 <change_load+0x30>
    aab0:	00c12083          	lw	ra,12(sp)
    aab4:	00100713          	li	a4,1
    aab8:	86e1a223          	sw	a4,-1948(gp) # 1346c <stats_reset>
    aabc:	01010113          	addi	sp,sp,16
    aac0:	00008067          	ret
    aac4:	8141a583          	lw	a1,-2028(gp) # 1341c <img_period>
    aac8:	0a051063          	bnez	a0,ab68 <change_load+0xd4>
    aacc:	00500713          	li	a4,5
    aad0:	0ab75463          	bge	a4,a1,ab78 <change_load+0xe4>
    aad4:	ffb58593          	addi	a1,a1,-5 # 11ffffb <__freertos_irq_stack_top+0x1117e5b>
    aad8:	03c00713          	li	a4,60
    aadc:	0cb75263          	bge	a4,a1,aba0 <change_load+0x10c>
    aae0:	80e1aa23          	sw	a4,-2028(gp) # 1341c <img_period>
    aae4:	03c00593          	li	a1,60
    aae8:	00012537          	lui	a0,0x12
    aaec:	d0850513          	addi	a0,a0,-760 # 11d08 <_data+0x5cc>
    aaf0:	eddff0ef          	jal	a9cc <tx_printf>
    aaf4:	00c12083          	lw	ra,12(sp)
    aaf8:	00100713          	li	a4,1
    aafc:	86e1a223          	sw	a4,-1948(gp) # 1346c <stats_reset>
    ab00:	01010113          	addi	sp,sp,16
    ab04:	00008067          	ret
    ab08:	8181a703          	lw	a4,-2024(gp) # 13420 <sq_count>
    ab0c:	00171593          	slli	a1,a4,0x1
    ab10:	00051863          	bnez	a0,ab20 <change_load+0x8c>
    ab14:	01f75593          	srli	a1,a4,0x1f
    ab18:	00e585b3          	add	a1,a1,a4
    ab1c:	4015d593          	srai	a1,a1,0x1
    ab20:	00f00713          	li	a4,15
    ab24:	02b75c63          	bge	a4,a1,ab5c <change_load+0xc8>
    ab28:	00001737          	lui	a4,0x1
    ab2c:	06b75e63          	bge	a4,a1,aba8 <change_load+0x114>
    ab30:	00001737          	lui	a4,0x1
    ab34:	000015b7          	lui	a1,0x1
    ab38:	00012537          	lui	a0,0x12
    ab3c:	cf850513          	addi	a0,a0,-776 # 11cf8 <_data+0x5bc>
    ab40:	80e1ac23          	sw	a4,-2024(gp) # 13420 <sq_count>
    ab44:	e89ff0ef          	jal	a9cc <tx_printf>
    ab48:	00c12083          	lw	ra,12(sp)
    ab4c:	00100713          	li	a4,1
    ab50:	86e1a223          	sw	a4,-1948(gp) # 1346c <stats_reset>
    ab54:	01010113          	addi	sp,sp,16
    ab58:	00008067          	ret
    ab5c:	01000713          	li	a4,16
    ab60:	01000593          	li	a1,16
    ab64:	fd5ff06f          	j	ab38 <change_load+0xa4>
    ab68:	00400713          	li	a4,4
    ab6c:	02b75263          	bge	a4,a1,ab90 <change_load+0xfc>
    ab70:	00558593          	addi	a1,a1,5 # 1005 <_start+0x5>
    ab74:	f65ff06f          	j	aad8 <change_load+0x44>
    ab78:	fff58593          	addi	a1,a1,-1
    ab7c:	00b04e63          	bgtz	a1,ab98 <change_load+0x104>
    ab80:	00100713          	li	a4,1
    ab84:	00100593          	li	a1,1
    ab88:	80e1aa23          	sw	a4,-2028(gp) # 1341c <img_period>
    ab8c:	f5dff06f          	j	aae8 <change_load+0x54>
    ab90:	00158593          	addi	a1,a1,1
    ab94:	fe9ff06f          	j	ab7c <change_load+0xe8>
    ab98:	00058713          	mv	a4,a1
    ab9c:	fedff06f          	j	ab88 <change_load+0xf4>
    aba0:	80b1aa23          	sw	a1,-2028(gp) # 1341c <img_period>
    aba4:	f45ff06f          	j	aae8 <change_load+0x54>
    aba8:	00058713          	mv	a4,a1
    abac:	f8dff06f          	j	ab38 <change_load+0xa4>

0000abb0 <handle_char>:
    abb0:	8781a783          	lw	a5,-1928(gp) # 13480 <esc_state>
    abb4:	00100693          	li	a3,1
    abb8:	04d78e63          	beq	a5,a3,ac14 <handle_char+0x64>
    abbc:	00200613          	li	a2,2
    abc0:	02c78663          	beq	a5,a2,abec <handle_char+0x3c>
    abc4:	ff650513          	addi	a0,a0,-10
    abc8:	0ff57513          	zext.b	a0,a0
    abcc:	06e00793          	li	a5,110
    abd0:	04a7e063          	bltu	a5,a0,ac10 <handle_char+0x60>
    abd4:	000137b7          	lui	a5,0x13
    abd8:	e3878793          	addi	a5,a5,-456 # 12e38 <gfx_hw_ops+0x9c>
    abdc:	00251513          	slli	a0,a0,0x2
    abe0:	00f50533          	add	a0,a0,a5
    abe4:	00052783          	lw	a5,0(a0)
    abe8:	00078067          	jr	a5
    abec:	8601ac23          	sw	zero,-1928(gp) # 13480 <esc_state>
    abf0:	04100793          	li	a5,65
    abf4:	04f50663          	beq	a0,a5,ac40 <handle_char+0x90>
    abf8:	04200793          	li	a5,66
    abfc:	7cf50663          	beq	a0,a5,b3c8 <handle_char+0x818>
    ac00:	04300793          	li	a5,67
    ac04:	02f50ce3          	beq	a0,a5,b43c <handle_char+0x88c>
    ac08:	04400793          	li	a5,68
    ac0c:	0af50663          	beq	a0,a5,acb8 <handle_char+0x108>
    ac10:	00008067          	ret
    ac14:	05b00793          	li	a5,91
    ac18:	00f50e63          	beq	a0,a5,ac34 <handle_char+0x84>
    ac1c:	ff650513          	addi	a0,a0,-10
    ac20:	8601ac23          	sw	zero,-1928(gp) # 13480 <esc_state>
    ac24:	0ff57513          	zext.b	a0,a0
    ac28:	06e00793          	li	a5,110
    ac2c:	faa7f4e3          	bgeu	a5,a0,abd4 <handle_char+0x24>
    ac30:	00008067          	ret
    ac34:	00200793          	li	a5,2
    ac38:	86f1ac23          	sw	a5,-1928(gp) # 13480 <esc_state>
    ac3c:	00008067          	ret
    ac40:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    ac44:	fff00613          	li	a2,-1
    ac48:	88c1a223          	sw	a2,-1916(gp) # 1348c <dir_y>
    ac4c:	fc0782e3          	beqz	a5,ac10 <handle_char+0x60>
    ac50:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ac54:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    ac58:	00001837          	lui	a6,0x1
    ac5c:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    ac60:	fff70713          	addi	a4,a4,-1 # fff <CUSTOM2+0xfa4>
    ac64:	40c70733          	sub	a4,a4,a2
    ac68:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    ac6c:	01077733          	and	a4,a4,a6
    ac70:	01000693          	li	a3,16
    ac74:	74e6f263          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    ac78:	000ae737          	lui	a4,0xae
    ac7c:	000125b7          	lui	a1,0x12
    ac80:	05b00513          	li	a0,91
    ac84:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    ac88:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    ac8c:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ac90:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ac94:	00158593          	addi	a1,a1,1
    ac98:	00c70633          	add	a2,a4,a2
    ac9c:	00a60023          	sb	a0,0(a2) # 1200000 <__freertos_irq_stack_top+0x1117e60>
    aca0:	00168693          	addi	a3,a3,1
    aca4:	0005c503          	lbu	a0,0(a1)
    aca8:	0106f6b3          	and	a3,a3,a6
    acac:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    acb0:	fc051ee3          	bnez	a0,ac8c <handle_char+0xdc>
    acb4:	00008067          	ret
    acb8:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    acbc:	fff00613          	li	a2,-1
    acc0:	88c1a423          	sw	a2,-1912(gp) # 13490 <dir_x>
    acc4:	f40786e3          	beqz	a5,ac10 <handle_char+0x60>
    acc8:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    accc:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    acd0:	00001837          	lui	a6,0x1
    acd4:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    acd8:	fff70713          	addi	a4,a4,-1
    acdc:	40c70733          	sub	a4,a4,a2
    ace0:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    ace4:	01077733          	and	a4,a4,a6
    ace8:	01000693          	li	a3,16
    acec:	6ce6f663          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    acf0:	000ae737          	lui	a4,0xae
    acf4:	000125b7          	lui	a1,0x12
    acf8:	05b00513          	li	a0,91
    acfc:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    ad00:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    ad04:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ad08:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ad0c:	00158593          	addi	a1,a1,1
    ad10:	00c70633          	add	a2,a4,a2
    ad14:	00a60023          	sb	a0,0(a2)
    ad18:	00168693          	addi	a3,a3,1
    ad1c:	0005c503          	lbu	a0,0(a1)
    ad20:	0106f6b3          	and	a3,a3,a6
    ad24:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    ad28:	fc051ee3          	bnez	a0,ad04 <handle_char+0x154>
    ad2c:	00008067          	ret
    ad30:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    ad34:	0107e793          	ori	a5,a5,16
    ad38:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    ad3c:	00008067          	ret
    ad40:	00000513          	li	a0,0
    ad44:	d51ff06f          	j	aa94 <change_load>
    ad48:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ad4c:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    ad50:	00001837          	lui	a6,0x1
    ad54:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    ad58:	fff70713          	addi	a4,a4,-1
    ad5c:	40d70733          	sub	a4,a4,a3
    ad60:	01077733          	and	a4,a4,a6
    ad64:	12600693          	li	a3,294
    ad68:	64e6f863          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    ad6c:	000ae737          	lui	a4,0xae
    ad70:	000125b7          	lui	a1,0x12
    ad74:	00d00513          	li	a0,13
    ad78:	f2c58593          	addi	a1,a1,-212 # 11f2c <_data+0x7f0>
    ad7c:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    ad80:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ad84:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ad88:	00158593          	addi	a1,a1,1
    ad8c:	00c70633          	add	a2,a4,a2
    ad90:	00a60023          	sb	a0,0(a2)
    ad94:	00168693          	addi	a3,a3,1
    ad98:	0005c503          	lbu	a0,0(a1)
    ad9c:	0106f6b3          	and	a3,a3,a6
    ada0:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    ada4:	fc051ee3          	bnez	a0,ad80 <handle_char+0x1d0>
    ada8:	00008067          	ret
    adac:	00100513          	li	a0,1
    adb0:	ce5ff06f          	j	aa94 <change_load>
    adb4:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    adb8:	00100613          	li	a2,1
    adbc:	88c1a423          	sw	a2,-1912(gp) # 13490 <dir_x>
    adc0:	e40788e3          	beqz	a5,ac10 <handle_char+0x60>
    adc4:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    adc8:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    adcc:	00001837          	lui	a6,0x1
    add0:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    add4:	fff70713          	addi	a4,a4,-1
    add8:	40c70733          	sub	a4,a4,a2
    addc:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    ade0:	01077733          	and	a4,a4,a6
    ade4:	01000693          	li	a3,16
    ade8:	5ce6f863          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    adec:	000ae737          	lui	a4,0xae
    adf0:	000125b7          	lui	a1,0x12
    adf4:	05b00513          	li	a0,91
    adf8:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    adfc:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    ae00:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ae04:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ae08:	00158593          	addi	a1,a1,1
    ae0c:	00c70633          	add	a2,a4,a2
    ae10:	00a60023          	sb	a0,0(a2)
    ae14:	00168693          	addi	a3,a3,1
    ae18:	0005c503          	lbu	a0,0(a1)
    ae1c:	0106f6b3          	and	a3,a3,a6
    ae20:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    ae24:	fc051ee3          	bnez	a0,ae00 <handle_char+0x250>
    ae28:	00008067          	ret
    ae2c:	81c1a783          	lw	a5,-2020(gp) # 13424 <alpha_fx>
    ae30:	0017b693          	seqz	a3,a5
    ae34:	80d1ae23          	sw	a3,-2020(gp) # 13424 <alpha_fx>
    ae38:	66079c63          	bnez	a5,b4b0 <handle_char+0x900>
    ae3c:	000125b7          	lui	a1,0x12
    ae40:	03100613          	li	a2,49
    ae44:	dd858593          	addi	a1,a1,-552 # 11dd8 <_data+0x69c>
    ae48:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ae4c:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    ae50:	00001837          	lui	a6,0x1
    ae54:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    ae58:	fff70713          	addi	a4,a4,-1
    ae5c:	40d70733          	sub	a4,a4,a3
    ae60:	01077733          	and	a4,a4,a6
    ae64:	6ae67c63          	bgeu	a2,a4,b51c <handle_char+0x96c>
    ae68:	000ae737          	lui	a4,0xae
    ae6c:	05b00513          	li	a0,91
    ae70:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    ae74:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    ae78:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    ae7c:	00158593          	addi	a1,a1,1
    ae80:	00c70633          	add	a2,a4,a2
    ae84:	00a60023          	sb	a0,0(a2)
    ae88:	00168693          	addi	a3,a3,1
    ae8c:	0005c503          	lbu	a0,0(a1)
    ae90:	0106f6b3          	and	a3,a3,a6
    ae94:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    ae98:	fc051ee3          	bnez	a0,ae74 <handle_char+0x2c4>
    ae9c:	00100713          	li	a4,1
    aea0:	86e1a223          	sw	a4,-1948(gp) # 1346c <stats_reset>
    aea4:	00008067          	ret
    aea8:	8701a783          	lw	a5,-1936(gp) # 13478 <use_hw>
    aeac:	64079e63          	bnez	a5,b508 <handle_char+0x958>
    aeb0:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    aeb4:	6a078263          	beqz	a5,b558 <handle_char+0x9a8>
    aeb8:	000125b7          	lui	a1,0x12
    aebc:	00100513          	li	a0,1
    aec0:	01a00613          	li	a2,26
    aec4:	e7058593          	addi	a1,a1,-400 # 11e70 <_data+0x734>
    aec8:	8941a803          	lw	a6,-1900(gp) # 1349c <tx_head>
    aecc:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    aed0:	86a1a823          	sw	a0,-1936(gp) # 13478 <use_hw>
    aed4:	fff70713          	addi	a4,a4,-1
    aed8:	41070733          	sub	a4,a4,a6
    aedc:	00001837          	lui	a6,0x1
    aee0:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    aee4:	00100513          	li	a0,1
    aee8:	86a1a223          	sw	a0,-1948(gp) # 1346c <stats_reset>
    aeec:	01077733          	and	a4,a4,a6
    aef0:	4ce67463          	bgeu	a2,a4,b3b8 <handle_char+0x808>
    aef4:	000ae737          	lui	a4,0xae
    aef8:	05b00513          	li	a0,91
    aefc:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    af00:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    af04:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    af08:	00158593          	addi	a1,a1,1
    af0c:	00c70633          	add	a2,a4,a2
    af10:	00a60023          	sb	a0,0(a2)
    af14:	00168693          	addi	a3,a3,1
    af18:	0005c503          	lbu	a0,0(a1)
    af1c:	0106f6b3          	and	a3,a3,a6
    af20:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    af24:	fc051ee3          	bnez	a0,af00 <handle_char+0x350>
    af28:	00008067          	ret
    af2c:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    af30:	0017b693          	seqz	a3,a5
    af34:	82d1a423          	sw	a3,-2008(gp) # 13430 <autoplay>
    af38:	8801a223          	sw	zero,-1916(gp) # 1348c <dir_y>
    af3c:	8801a423          	sw	zero,-1912(gp) # 13490 <dir_x>
    af40:	5a079463          	bnez	a5,b4e8 <handle_char+0x938>
    af44:	000125b7          	lui	a1,0x12
    af48:	00f00613          	li	a2,15
    af4c:	d4058593          	addi	a1,a1,-704 # 11d40 <_data+0x604>
    af50:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    af54:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    af58:	00001837          	lui	a6,0x1
    af5c:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    af60:	fff70713          	addi	a4,a4,-1
    af64:	40d70733          	sub	a4,a4,a3
    af68:	01077733          	and	a4,a4,a6
    af6c:	44e67663          	bgeu	a2,a4,b3b8 <handle_char+0x808>
    af70:	000ae737          	lui	a4,0xae
    af74:	05b00513          	li	a0,91
    af78:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    af7c:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    af80:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    af84:	00158593          	addi	a1,a1,1
    af88:	00c70633          	add	a2,a4,a2
    af8c:	00a60023          	sb	a0,0(a2)
    af90:	00168693          	addi	a3,a3,1
    af94:	0005c503          	lbu	a0,0(a1)
    af98:	0106f6b3          	and	a3,a3,a6
    af9c:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    afa0:	fc051ee3          	bnez	a0,af7c <handle_char+0x3cc>
    afa4:	00008067          	ret
    afa8:	8201a783          	lw	a5,-2016(gp) # 13428 <show_stats>
    afac:	0017b793          	seqz	a5,a5
    afb0:	82f1a023          	sw	a5,-2016(gp) # 13428 <show_stats>
    afb4:	00100713          	li	a4,1
    afb8:	86e1a223          	sw	a4,-1948(gp) # 1346c <stats_reset>
    afbc:	00008067          	ret
    afc0:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    afc4:	00100613          	li	a2,1
    afc8:	88c1a223          	sw	a2,-1916(gp) # 1348c <dir_y>
    afcc:	c40782e3          	beqz	a5,ac10 <handle_char+0x60>
    afd0:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    afd4:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    afd8:	00001837          	lui	a6,0x1
    afdc:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    afe0:	fff70713          	addi	a4,a4,-1
    afe4:	40c70733          	sub	a4,a4,a2
    afe8:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    afec:	01077733          	and	a4,a4,a6
    aff0:	01000693          	li	a3,16
    aff4:	3ce6f263          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    aff8:	000ae737          	lui	a4,0xae
    affc:	000125b7          	lui	a1,0x12
    b000:	05b00513          	li	a0,91
    b004:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b008:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b00c:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b010:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b014:	00158593          	addi	a1,a1,1
    b018:	00c70633          	add	a2,a4,a2
    b01c:	00a60023          	sb	a0,0(a2)
    b020:	00168693          	addi	a3,a3,1
    b024:	0005c503          	lbu	a0,0(a1)
    b028:	0106f6b3          	and	a3,a3,a6
    b02c:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b030:	fc051ee3          	bnez	a0,b00c <handle_char+0x45c>
    b034:	00008067          	ret
    b038:	87c1a783          	lw	a5,-1924(gp) # 13484 <test_bars>
    b03c:	0017b693          	seqz	a3,a5
    b040:	86d1ae23          	sw	a3,-1924(gp) # 13484 <test_bars>
    b044:	48079663          	bnez	a5,b4d0 <handle_char+0x920>
    b048:	f81007b7          	lui	a5,0xf8100
    b04c:	00300713          	li	a4,3
    b050:	000125b7          	lui	a1,0x12
    b054:	00e7a023          	sw	a4,0(a5) # f8100000 <__freertos_irq_stack_top+0xf8017e60>
    b058:	da858593          	addi	a1,a1,-600 # 11da8 <_data+0x66c>
    b05c:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b060:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b064:	00001837          	lui	a6,0x1
    b068:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b06c:	fff70713          	addi	a4,a4,-1
    b070:	40d70733          	sub	a4,a4,a3
    b074:	01077733          	and	a4,a4,a6
    b078:	01400693          	li	a3,20
    b07c:	32e6fe63          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b080:	000ae737          	lui	a4,0xae
    b084:	05b00513          	li	a0,91
    b088:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b08c:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b090:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b094:	00158593          	addi	a1,a1,1
    b098:	00c70633          	add	a2,a4,a2
    b09c:	00a60023          	sb	a0,0(a2)
    b0a0:	00168693          	addi	a3,a3,1
    b0a4:	0005c503          	lbu	a0,0(a1)
    b0a8:	0106f6b3          	and	a3,a3,a6
    b0ac:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b0b0:	fc051ee3          	bnez	a0,b08c <handle_char+0x4dc>
    b0b4:	00008067          	ret
    b0b8:	8241a783          	lw	a5,-2012(gp) # 1342c <vsync>
    b0bc:	0017b693          	seqz	a3,a5
    b0c0:	82d1a223          	sw	a3,-2012(gp) # 1342c <vsync>
    b0c4:	00100693          	li	a3,1
    b0c8:	86d1a223          	sw	a3,-1948(gp) # 1346c <stats_reset>
    b0cc:	3e079a63          	bnez	a5,b4c0 <handle_char+0x910>
    b0d0:	000125b7          	lui	a1,0x12
    b0d4:	01e00613          	li	a2,30
    b0d8:	d5058593          	addi	a1,a1,-688 # 11d50 <_data+0x614>
    b0dc:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b0e0:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b0e4:	00001837          	lui	a6,0x1
    b0e8:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b0ec:	fff70713          	addi	a4,a4,-1
    b0f0:	40d70733          	sub	a4,a4,a3
    b0f4:	01077733          	and	a4,a4,a6
    b0f8:	2ce67063          	bgeu	a2,a4,b3b8 <handle_char+0x808>
    b0fc:	000ae737          	lui	a4,0xae
    b100:	05b00513          	li	a0,91
    b104:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b108:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b10c:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b110:	00158593          	addi	a1,a1,1
    b114:	00c70633          	add	a2,a4,a2
    b118:	00a60023          	sb	a0,0(a2)
    b11c:	00168693          	addi	a3,a3,1
    b120:	0005c503          	lbu	a0,0(a1)
    b124:	0106f6b3          	and	a3,a3,a6
    b128:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b12c:	fc051ee3          	bnez	a0,b108 <handle_char+0x558>
    b130:	00008067          	ret
    b134:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    b138:	fff00613          	li	a2,-1
    b13c:	88c1a423          	sw	a2,-1912(gp) # 13490 <dir_x>
    b140:	ac0788e3          	beqz	a5,ac10 <handle_char+0x60>
    b144:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b148:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b14c:	00001837          	lui	a6,0x1
    b150:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b154:	fff70713          	addi	a4,a4,-1
    b158:	40c70733          	sub	a4,a4,a2
    b15c:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    b160:	01077733          	and	a4,a4,a6
    b164:	01000693          	li	a3,16
    b168:	24e6f863          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b16c:	000ae737          	lui	a4,0xae
    b170:	000125b7          	lui	a1,0x12
    b174:	05b00513          	li	a0,91
    b178:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b17c:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b180:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b184:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b188:	00158593          	addi	a1,a1,1
    b18c:	00c70633          	add	a2,a4,a2
    b190:	00a60023          	sb	a0,0(a2)
    b194:	00168693          	addi	a3,a3,1
    b198:	0005c503          	lbu	a0,0(a1)
    b19c:	0106f6b3          	and	a3,a3,a6
    b1a0:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b1a4:	fc051ee3          	bnez	a0,b180 <handle_char+0x5d0>
    b1a8:	00008067          	ret
    b1ac:	8741a783          	lw	a5,-1932(gp) # 1347c <hw_avail>
    b1b0:	38078663          	beqz	a5,b53c <handle_char+0x98c>
    b1b4:	86c1a783          	lw	a5,-1940(gp) # 13474 <auto_compare>
    b1b8:	0017b693          	seqz	a3,a5
    b1bc:	86d1a623          	sw	a3,-1940(gp) # 13474 <auto_compare>
    b1c0:	36079663          	bnez	a5,b52c <handle_char+0x97c>
    b1c4:	000125b7          	lui	a1,0x12
    b1c8:	02b00613          	li	a2,43
    b1cc:	ea058593          	addi	a1,a1,-352 # 11ea0 <_data+0x764>
    b1d0:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b1d4:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b1d8:	00001837          	lui	a6,0x1
    b1dc:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b1e0:	fff70713          	addi	a4,a4,-1
    b1e4:	40d70733          	sub	a4,a4,a3
    b1e8:	01077733          	and	a4,a4,a6
    b1ec:	1ce67663          	bgeu	a2,a4,b3b8 <handle_char+0x808>
    b1f0:	000ae737          	lui	a4,0xae
    b1f4:	05b00513          	li	a0,91
    b1f8:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b1fc:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b200:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b204:	00158593          	addi	a1,a1,1
    b208:	00c70633          	add	a2,a4,a2
    b20c:	00a60023          	sb	a0,0(a2)
    b210:	00168693          	addi	a3,a3,1
    b214:	0005c503          	lbu	a0,0(a1)
    b218:	0106f6b3          	and	a3,a3,a6
    b21c:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b220:	fc051ee3          	bnez	a0,b1fc <handle_char+0x64c>
    b224:	00008067          	ret
    b228:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b22c:	00001737          	lui	a4,0x1
    b230:	80070713          	addi	a4,a4,-2048 # 800 <CUSTOM2+0x7a5>
    b234:	00e7e7b3          	or	a5,a5,a4
    b238:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b23c:	00008067          	ret
    b240:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b244:	0207e793          	ori	a5,a5,32
    b248:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b24c:	00008067          	ret
    b250:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b254:	0407e793          	ori	a5,a5,64
    b258:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b25c:	00008067          	ret
    b260:	8801a423          	sw	zero,-1912(gp) # 13490 <dir_x>
    b264:	8801a223          	sw	zero,-1916(gp) # 1348c <dir_y>
    b268:	00008067          	ret
    b26c:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b270:	4007e793          	ori	a5,a5,1024
    b274:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b278:	00008067          	ret
    b27c:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    b280:	fff00613          	li	a2,-1
    b284:	88c1a223          	sw	a2,-1916(gp) # 1348c <dir_y>
    b288:	980784e3          	beqz	a5,ac10 <handle_char+0x60>
    b28c:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b290:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b294:	00001837          	lui	a6,0x1
    b298:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b29c:	fff70713          	addi	a4,a4,-1
    b2a0:	40c70733          	sub	a4,a4,a2
    b2a4:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    b2a8:	01077733          	and	a4,a4,a6
    b2ac:	01000693          	li	a3,16
    b2b0:	10e6f463          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b2b4:	000ae737          	lui	a4,0xae
    b2b8:	000125b7          	lui	a1,0x12
    b2bc:	05b00513          	li	a0,91
    b2c0:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b2c4:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b2c8:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b2cc:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b2d0:	00158593          	addi	a1,a1,1
    b2d4:	00c70633          	add	a2,a4,a2
    b2d8:	00a60023          	sb	a0,0(a2)
    b2dc:	00168693          	addi	a3,a3,1
    b2e0:	0005c503          	lbu	a0,0(a1)
    b2e4:	0106f6b3          	and	a3,a3,a6
    b2e8:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b2ec:	fc051ee3          	bnez	a0,b2c8 <handle_char+0x718>
    b2f0:	00008067          	ret
    b2f4:	8681a783          	lw	a5,-1944(gp) # 13470 <scene>
    b2f8:	00300693          	li	a3,3
    b2fc:	00178793          	addi	a5,a5,1
    b300:	02d7e7b3          	rem	a5,a5,a3
    b304:	00100693          	li	a3,1
    b308:	86d1a223          	sw	a3,-1948(gp) # 1346c <stats_reset>
    b30c:	86f1a423          	sw	a5,-1944(gp) # 13470 <scene>
    b310:	1e078463          	beqz	a5,b4f8 <handle_char+0x948>
    b314:	22d78a63          	beq	a5,a3,b548 <handle_char+0x998>
    b318:	000125b7          	lui	a1,0x12
    b31c:	01a00613          	li	a2,26
    b320:	ee458593          	addi	a1,a1,-284 # 11ee4 <_data+0x7a8>
    b324:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b328:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b32c:	00001837          	lui	a6,0x1
    b330:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b334:	fff70713          	addi	a4,a4,-1
    b338:	40d70733          	sub	a4,a4,a3
    b33c:	01077733          	and	a4,a4,a6
    b340:	06e67c63          	bgeu	a2,a4,b3b8 <handle_char+0x808>
    b344:	000ae737          	lui	a4,0xae
    b348:	05b00513          	li	a0,91
    b34c:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b350:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b354:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b358:	00158593          	addi	a1,a1,1
    b35c:	00c70633          	add	a2,a4,a2
    b360:	00a60023          	sb	a0,0(a2)
    b364:	00168693          	addi	a3,a3,1
    b368:	0005c503          	lbu	a0,0(a1)
    b36c:	0106f6b3          	and	a3,a3,a6
    b370:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b374:	fc051ee3          	bnez	a0,b350 <handle_char+0x7a0>
    b378:	00008067          	ret
    b37c:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b380:	1007e793          	ori	a5,a5,256
    b384:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b388:	00008067          	ret
    b38c:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b390:	0807e793          	ori	a5,a5,128
    b394:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b398:	00008067          	ret
    b39c:	8801a783          	lw	a5,-1920(gp) # 13488 <key_pressed>
    b3a0:	2007e793          	ori	a5,a5,512
    b3a4:	88f1a023          	sw	a5,-1920(gp) # 13488 <key_pressed>
    b3a8:	00008067          	ret
    b3ac:	00100793          	li	a5,1
    b3b0:	86f1ac23          	sw	a5,-1928(gp) # 13480 <esc_state>
    b3b4:	00008067          	ret
    b3b8:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    b3bc:	00178793          	addi	a5,a5,1
    b3c0:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    b3c4:	00008067          	ret
    b3c8:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    b3cc:	88d1a223          	sw	a3,-1916(gp) # 1348c <dir_y>
    b3d0:	840780e3          	beqz	a5,ac10 <handle_char+0x60>
    b3d4:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b3d8:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b3dc:	00001837          	lui	a6,0x1
    b3e0:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b3e4:	fff70713          	addi	a4,a4,-1
    b3e8:	40d70733          	sub	a4,a4,a3
    b3ec:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    b3f0:	01077733          	and	a4,a4,a6
    b3f4:	01000693          	li	a3,16
    b3f8:	fce6f0e3          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b3fc:	000ae737          	lui	a4,0xae
    b400:	000125b7          	lui	a1,0x12
    b404:	05b00513          	li	a0,91
    b408:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b40c:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b410:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b414:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b418:	00158593          	addi	a1,a1,1
    b41c:	00c70633          	add	a2,a4,a2
    b420:	00a60023          	sb	a0,0(a2)
    b424:	00168693          	addi	a3,a3,1
    b428:	0005c503          	lbu	a0,0(a1)
    b42c:	0106f6b3          	and	a3,a3,a6
    b430:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b434:	fc051ee3          	bnez	a0,b410 <handle_char+0x860>
    b438:	00008067          	ret
    b43c:	8281a783          	lw	a5,-2008(gp) # 13430 <autoplay>
    b440:	88d1a423          	sw	a3,-1912(gp) # 13490 <dir_x>
    b444:	fc078663          	beqz	a5,ac10 <handle_char+0x60>
    b448:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b44c:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b450:	00001837          	lui	a6,0x1
    b454:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b458:	fff70713          	addi	a4,a4,-1
    b45c:	40d70733          	sub	a4,a4,a3
    b460:	8201a423          	sw	zero,-2008(gp) # 13430 <autoplay>
    b464:	01077733          	and	a4,a4,a6
    b468:	01000693          	li	a3,16
    b46c:	f4e6f6e3          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b470:	000ae737          	lui	a4,0xae
    b474:	000125b7          	lui	a1,0x12
    b478:	05b00513          	li	a0,91
    b47c:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b480:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b484:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b488:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b48c:	00158593          	addi	a1,a1,1
    b490:	00c70633          	add	a2,a4,a2
    b494:	00a60023          	sb	a0,0(a2)
    b498:	00168693          	addi	a3,a3,1
    b49c:	0005c503          	lbu	a0,0(a1)
    b4a0:	0106f6b3          	and	a3,a3,a6
    b4a4:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b4a8:	fc051ee3          	bnez	a0,b484 <handle_char+0x8d4>
    b4ac:	00008067          	ret
    b4b0:	000125b7          	lui	a1,0x12
    b4b4:	02e00613          	li	a2,46
    b4b8:	e0c58593          	addi	a1,a1,-500 # 11e0c <_data+0x6d0>
    b4bc:	98dff06f          	j	ae48 <handle_char+0x298>
    b4c0:	000125b7          	lui	a1,0x12
    b4c4:	03500613          	li	a2,53
    b4c8:	d7058593          	addi	a1,a1,-656 # 11d70 <_data+0x634>
    b4cc:	c11ff06f          	j	b0dc <handle_char+0x52c>
    b4d0:	f81007b7          	lui	a5,0xf8100
    b4d4:	00100713          	li	a4,1
    b4d8:	000125b7          	lui	a1,0x12
    b4dc:	00e7a023          	sw	a4,0(a5) # f8100000 <__freertos_irq_stack_top+0xf8017e60>
    b4e0:	dc058593          	addi	a1,a1,-576 # 11dc0 <_data+0x684>
    b4e4:	b79ff06f          	j	b05c <handle_char+0x4ac>
    b4e8:	000125b7          	lui	a1,0x12
    b4ec:	01000613          	li	a2,16
    b4f0:	d2c58593          	addi	a1,a1,-724 # 11d2c <_data+0x5f0>
    b4f4:	a5dff06f          	j	af50 <handle_char+0x3a0>
    b4f8:	000125b7          	lui	a1,0x12
    b4fc:	00f00613          	li	a2,15
    b500:	f0058593          	addi	a1,a1,-256 # 11f00 <_data+0x7c4>
    b504:	e21ff06f          	j	b324 <handle_char+0x774>
    b508:	000125b7          	lui	a1,0x12
    b50c:	00000513          	li	a0,0
    b510:	01000613          	li	a2,16
    b514:	e8c58593          	addi	a1,a1,-372 # 11e8c <_data+0x750>
    b518:	9b1ff06f          	j	aec8 <handle_char+0x318>
    b51c:	88c1a783          	lw	a5,-1908(gp) # 13494 <tx_dropped>
    b520:	00178793          	addi	a5,a5,1
    b524:	88f1a623          	sw	a5,-1908(gp) # 13494 <tx_dropped>
    b528:	975ff06f          	j	ae9c <handle_char+0x2ec>
    b52c:	000125b7          	lui	a1,0x12
    b530:	01400613          	li	a2,20
    b534:	ecc58593          	addi	a1,a1,-308 # 11ecc <_data+0x790>
    b538:	c99ff06f          	j	b1d0 <handle_char+0x620>
    b53c:	00012537          	lui	a0,0x12
    b540:	e3c50513          	addi	a0,a0,-452 # 11e3c <_data+0x700>
    b544:	fcdfe06f          	j	a510 <tx_puts>
    b548:	000125b7          	lui	a1,0x12
    b54c:	01900613          	li	a2,25
    b550:	f1058593          	addi	a1,a1,-240 # 11f10 <_data+0x7d4>
    b554:	dd1ff06f          	j	b324 <handle_char+0x774>
    b558:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b55c:	8901a703          	lw	a4,-1904(gp) # 13498 <tx_tail>
    b560:	00001837          	lui	a6,0x1
    b564:	fff80813          	addi	a6,a6,-1 # fff <CUSTOM2+0xfa4>
    b568:	fff70713          	addi	a4,a4,-1
    b56c:	40d70733          	sub	a4,a4,a3
    b570:	01077733          	and	a4,a4,a6
    b574:	03000693          	li	a3,48
    b578:	e4e6f0e3          	bgeu	a3,a4,b3b8 <handle_char+0x808>
    b57c:	000ae737          	lui	a4,0xae
    b580:	000125b7          	lui	a1,0x12
    b584:	05b00513          	li	a0,91
    b588:	e3c58593          	addi	a1,a1,-452 # 11e3c <_data+0x700>
    b58c:	f5070713          	addi	a4,a4,-176 # adf50 <tx_buf>
    b590:	8941a603          	lw	a2,-1900(gp) # 1349c <tx_head>
    b594:	8941a683          	lw	a3,-1900(gp) # 1349c <tx_head>
    b598:	00158593          	addi	a1,a1,1
    b59c:	00c70633          	add	a2,a4,a2
    b5a0:	00a60023          	sb	a0,0(a2)
    b5a4:	00168693          	addi	a3,a3,1
    b5a8:	0005c503          	lbu	a0,0(a1)
    b5ac:	0106f6b3          	and	a3,a3,a6
    b5b0:	88d1aa23          	sw	a3,-1900(gp) # 1349c <tx_head>
    b5b4:	fc051ee3          	bnez	a0,b590 <handle_char+0x9e0>
    b5b8:	00008067          	ret

0000b5bc <particles>:
    b5bc:	000248b7          	lui	a7,0x24
    b5c0:	00025f37          	lui	t5,0x25
    b5c4:	1d488893          	addi	a7,a7,468 # 241d4 <background_pixels+0xc314>
    b5c8:	1b4f0f13          	addi	t5,t5,436 # 251b4 <background_pixels+0xd2f4>
    b5cc:	011508b3          	add	a7,a0,a7
    b5d0:	01e50f33          	add	t5,a0,t5
    b5d4:	2bc00e93          	li	t4,700
    b5d8:	0008a783          	lw	a5,0(a7)
    b5dc:	0c079063          	bnez	a5,b69c <particles+0xe0>
    b5e0:	00452783          	lw	a5,4(a0)
    b5e4:	00b8a423          	sw	a1,8(a7)
    b5e8:	00c8a623          	sw	a2,12(a7)
    b5ec:	00d79813          	slli	a6,a5,0xd
    b5f0:	0107c7b3          	xor	a5,a5,a6
    b5f4:	0117d813          	srli	a6,a5,0x11
    b5f8:	0107c7b3          	xor	a5,a5,a6
    b5fc:	00579813          	slli	a6,a5,0x5
    b600:	0107c7b3          	xor	a5,a5,a6
    b604:	00d79813          	slli	a6,a5,0xd
    b608:	00f84833          	xor	a6,a6,a5
    b60c:	01185313          	srli	t1,a6,0x11
    b610:	00684833          	xor	a6,a6,t1
    b614:	00581313          	slli	t1,a6,0x5
    b618:	00684833          	xor	a6,a6,t1
    b61c:	00d81313          	slli	t1,a6,0xd
    b620:	01034333          	xor	t1,t1,a6
    b624:	03d7f7b3          	remu	a5,a5,t4
    b628:	01135e13          	srli	t3,t1,0x11
    b62c:	01c34333          	xor	t1,t1,t3
    b630:	00531f93          	slli	t6,t1,0x5
    b634:	01f34fb3          	xor	t6,t1,t6
    b638:	00df9e13          	slli	t3,t6,0xd
    b63c:	01fe4e33          	xor	t3,t3,t6
    b640:	011e5f93          	srli	t6,t3,0x11
    b644:	01fe4e33          	xor	t3,t3,t6
    b648:	00f37313          	andi	t1,t1,15
    b64c:	005e1f93          	slli	t6,t3,0x5
    b650:	00c30313          	addi	t1,t1,12
    b654:	01fe4e33          	xor	t3,t3,t6
    b658:	0068a023          	sw	t1,0(a7)
    b65c:	0068a223          	sw	t1,4(a7)
    b660:	00d8ac23          	sw	a3,24(a7)
    b664:	001e7313          	andi	t1,t3,1
    b668:	00230313          	addi	t1,t1,2
    b66c:	fff70713          	addi	a4,a4,-1
    b670:	03d87833          	remu	a6,a6,t4
    b674:	ea278793          	addi	a5,a5,-350
    b678:	00f8a823          	sw	a5,16(a7)
    b67c:	e7080813          	addi	a6,a6,-400
    b680:	0108aa23          	sw	a6,20(a7)
    b684:	01c52223          	sw	t3,4(a0)
    b688:	0068ae23          	sw	t1,28(a7)
    b68c:	011f0e63          	beq	t5,a7,b6a8 <particles+0xec>
    b690:	02088893          	addi	a7,a7,32
    b694:	f40712e3          	bnez	a4,b5d8 <particles+0x1c>
    b698:	00008067          	ret
    b69c:	011f0663          	beq	t5,a7,b6a8 <particles+0xec>
    b6a0:	02088893          	addi	a7,a7,32
    b6a4:	f35ff06f          	j	b5d8 <particles+0x1c>
    b6a8:	00008067          	ret

0000b6ac <check_level>:
    b6ac:	04c52683          	lw	a3,76(a0)
    b6b0:	05052583          	lw	a1,80(a0)
    b6b4:	2ab6cc63          	blt	a3,a1,b96c <check_level+0x2c0>
    b6b8:	00852603          	lw	a2,8(a0)
    b6bc:	70800713          	li	a4,1800
    b6c0:	05452783          	lw	a5,84(a0)
    b6c4:	02e64633          	div	a2,a2,a4
    b6c8:	40b686b3          	sub	a3,a3,a1
    b6cc:	00279713          	slli	a4,a5,0x2
    b6d0:	00178593          	addi	a1,a5,1
    b6d4:	01852803          	lw	a6,24(a0)
    b6d8:	00670713          	addi	a4,a4,6
    b6dc:	fe010113          	addi	sp,sp,-32
    b6e0:	04d52623          	sw	a3,76(a0)
    b6e4:	04b52a23          	sw	a1,84(a0)
    b6e8:	00780593          	addi	a1,a6,7
    b6ec:	01c52883          	lw	a7,28(a0)
    b6f0:	00000813          	li	a6,0
    b6f4:	02f787b3          	mul	a5,a5,a5
    b6f8:	00161693          	slli	a3,a2,0x1
    b6fc:	4017d793          	srai	a5,a5,0x1
    b700:	00f707b3          	add	a5,a4,a5
    b704:	00d787b3          	add	a5,a5,a3
    b708:	04f52823          	sw	a5,80(a0)
    b70c:	0005c663          	bltz	a1,b718 <check_level+0x6c>
    b710:	00058813          	mv	a6,a1
    b714:	28b8c263          	blt	a7,a1,b998 <check_level+0x2ec>
    b718:	03852683          	lw	a3,56(a0)
    b71c:	01052c23          	sw	a6,24(a0)
    b720:	00700793          	li	a5,7
    b724:	00000713          	li	a4,0
    b728:	24d7c463          	blt	a5,a3,b970 <check_level+0x2c4>
    b72c:	00271793          	slli	a5,a4,0x2
    b730:	02078793          	addi	a5,a5,32
    b734:	04052603          	lw	a2,64(a0)
    b738:	002787b3          	add	a5,a5,sp
    b73c:	00100693          	li	a3,1
    b740:	fed7a423          	sw	a3,-24(a5)
    b744:	3b500693          	li	a3,949
    b748:	00170793          	addi	a5,a4,1
    b74c:	00c6ce63          	blt	a3,a2,b768 <check_level+0xbc>
    b750:	00279793          	slli	a5,a5,0x2
    b754:	02078793          	addi	a5,a5,32
    b758:	002787b3          	add	a5,a5,sp
    b75c:	00200693          	li	a3,2
    b760:	fed7a423          	sw	a3,-24(a5)
    b764:	00270793          	addi	a5,a4,2
    b768:	00279713          	slli	a4,a5,0x2
    b76c:	02070713          	addi	a4,a4,32
    b770:	04852603          	lw	a2,72(a0)
    b774:	00270733          	add	a4,a4,sp
    b778:	00300693          	li	a3,3
    b77c:	fed72423          	sw	a3,-24(a4)
    b780:	0bd00713          	li	a4,189
    b784:	00178693          	addi	a3,a5,1
    b788:	1ec75a63          	bge	a4,a2,b97c <check_level+0x2d0>
    b78c:	04452703          	lw	a4,68(a0)
    b790:	00300793          	li	a5,3
    b794:	00e7ce63          	blt	a5,a4,b7b0 <check_level+0x104>
    b798:	00269793          	slli	a5,a3,0x2
    b79c:	02078793          	addi	a5,a5,32
    b7a0:	002787b3          	add	a5,a5,sp
    b7a4:	00500713          	li	a4,5
    b7a8:	fee7a423          	sw	a4,-24(a5)
    b7ac:	00168693          	addi	a3,a3,1
    b7b0:	00452703          	lw	a4,4(a0)
    b7b4:	fff68e13          	addi	t3,a3,-1
    b7b8:	002e1613          	slli	a2,t3,0x2
    b7bc:	00d71793          	slli	a5,a4,0xd
    b7c0:	00e7c7b3          	xor	a5,a5,a4
    b7c4:	0117d713          	srli	a4,a5,0x11
    b7c8:	00f74733          	xor	a4,a4,a5
    b7cc:	00571793          	slli	a5,a4,0x5
    b7d0:	00e7c7b3          	xor	a5,a5,a4
    b7d4:	02d7f733          	remu	a4,a5,a3
    b7d8:	02060613          	addi	a2,a2,32
    b7dc:	00260633          	add	a2,a2,sp
    b7e0:	fe862583          	lw	a1,-24(a2)
    b7e4:	ffe68893          	addi	a7,a3,-2
    b7e8:	00271713          	slli	a4,a4,0x2
    b7ec:	02070813          	addi	a6,a4,32
    b7f0:	00280333          	add	t1,a6,sp
    b7f4:	fe832803          	lw	a6,-24(t1)
    b7f8:	ff062423          	sw	a6,-24(a2)
    b7fc:	feb32423          	sw	a1,-24(t1)
    b800:	10088e63          	beqz	a7,b91c <check_level+0x270>
    b804:	00d79713          	slli	a4,a5,0xd
    b808:	00f747b3          	xor	a5,a4,a5
    b80c:	0117d713          	srli	a4,a5,0x11
    b810:	00f74733          	xor	a4,a4,a5
    b814:	00571793          	slli	a5,a4,0x5
    b818:	00e7c7b3          	xor	a5,a5,a4
    b81c:	03c7f733          	remu	a4,a5,t3
    b820:	00289613          	slli	a2,a7,0x2
    b824:	02060613          	addi	a2,a2,32
    b828:	00260633          	add	a2,a2,sp
    b82c:	fe862583          	lw	a1,-24(a2)
    b830:	ffd68313          	addi	t1,a3,-3
    b834:	00271713          	slli	a4,a4,0x2
    b838:	02070813          	addi	a6,a4,32
    b83c:	00280e33          	add	t3,a6,sp
    b840:	fe8e2803          	lw	a6,-24(t3)
    b844:	ff062423          	sw	a6,-24(a2)
    b848:	febe2423          	sw	a1,-24(t3)
    b84c:	0c030863          	beqz	t1,b91c <check_level+0x270>
    b850:	00d79713          	slli	a4,a5,0xd
    b854:	00f747b3          	xor	a5,a4,a5
    b858:	0117d713          	srli	a4,a5,0x11
    b85c:	00f74733          	xor	a4,a4,a5
    b860:	00571793          	slli	a5,a4,0x5
    b864:	00e7c7b3          	xor	a5,a5,a4
    b868:	0317f733          	remu	a4,a5,a7
    b86c:	00231893          	slli	a7,t1,0x2
    b870:	02088613          	addi	a2,a7,32
    b874:	002608b3          	add	a7,a2,sp
    b878:	fe88a583          	lw	a1,-24(a7)
    b87c:	ffc68613          	addi	a2,a3,-4
    b880:	00271713          	slli	a4,a4,0x2
    b884:	02070813          	addi	a6,a4,32
    b888:	00280e33          	add	t3,a6,sp
    b88c:	fe8e2803          	lw	a6,-24(t3)
    b890:	ff08a423          	sw	a6,-24(a7)
    b894:	febe2423          	sw	a1,-24(t3)
    b898:	08060263          	beqz	a2,b91c <check_level+0x270>
    b89c:	00d79713          	slli	a4,a5,0xd
    b8a0:	00e7c733          	xor	a4,a5,a4
    b8a4:	01175793          	srli	a5,a4,0x11
    b8a8:	00f74733          	xor	a4,a4,a5
    b8ac:	00571793          	slli	a5,a4,0x5
    b8b0:	00f747b3          	xor	a5,a4,a5
    b8b4:	0267f733          	remu	a4,a5,t1
    b8b8:	00261613          	slli	a2,a2,0x2
    b8bc:	02060613          	addi	a2,a2,32
    b8c0:	00260633          	add	a2,a2,sp
    b8c4:	fe862583          	lw	a1,-24(a2)
    b8c8:	00500313          	li	t1,5
    b8cc:	00271713          	slli	a4,a4,0x2
    b8d0:	02070813          	addi	a6,a4,32
    b8d4:	002808b3          	add	a7,a6,sp
    b8d8:	fe88a803          	lw	a6,-24(a7)
    b8dc:	ff062423          	sw	a6,-24(a2)
    b8e0:	feb8a423          	sw	a1,-24(a7)
    b8e4:	02668c63          	beq	a3,t1,b91c <check_level+0x270>
    b8e8:	00d79613          	slli	a2,a5,0xd
    b8ec:	00c7c633          	xor	a2,a5,a2
    b8f0:	01165793          	srli	a5,a2,0x11
    b8f4:	00f64633          	xor	a2,a2,a5
    b8f8:	00167713          	andi	a4,a2,1
    b8fc:	00c12583          	lw	a1,12(sp)
    b900:	00271713          	slli	a4,a4,0x2
    b904:	02070793          	addi	a5,a4,32
    b908:	002788b3          	add	a7,a5,sp
    b90c:	00561793          	slli	a5,a2,0x5
    b910:	fe88a803          	lw	a6,-24(a7)
    b914:	00f647b3          	xor	a5,a2,a5
    b918:	feb8a423          	sw	a1,-24(a7)
    b91c:	00200613          	li	a2,2
    b920:	02d666b3          	rem	a3,a2,a3
    b924:	02070713          	addi	a4,a4,32
    b928:	01012623          	sw	a6,12(sp)
    b92c:	00270733          	add	a4,a4,sp
    b930:	feb72423          	sw	a1,-24(a4)
    b934:	00f52223          	sw	a5,4(a0)
    b938:	00812783          	lw	a5,8(sp)
    b93c:	0af52623          	sw	a5,172(a0)
    b940:	00c12783          	lw	a5,12(sp)
    b944:	0af52823          	sw	a5,176(a0)
    b948:	00400793          	li	a5,4
    b94c:	00f52023          	sw	a5,0(a0)
    b950:	00269793          	slli	a5,a3,0x2
    b954:	02078793          	addi	a5,a5,32
    b958:	002787b3          	add	a5,a5,sp
    b95c:	fe87a783          	lw	a5,-24(a5)
    b960:	0af52a23          	sw	a5,180(a0)
    b964:	02010113          	addi	sp,sp,32
    b968:	00008067          	ret
    b96c:	00008067          	ret
    b970:	00012423          	sw	zero,8(sp)
    b974:	00100713          	li	a4,1
    b978:	db5ff06f          	j	b72c <check_level+0x80>
    b97c:	00269693          	slli	a3,a3,0x2
    b980:	02068713          	addi	a4,a3,32
    b984:	002706b3          	add	a3,a4,sp
    b988:	00400713          	li	a4,4
    b98c:	fee6a423          	sw	a4,-24(a3)
    b990:	00278693          	addi	a3,a5,2
    b994:	df9ff06f          	j	b78c <check_level+0xe0>
    b998:	03852683          	lw	a3,56(a0)
    b99c:	00088813          	mv	a6,a7
    b9a0:	01052c23          	sw	a6,24(a0)
    b9a4:	00700793          	li	a5,7
    b9a8:	00000713          	li	a4,0
    b9ac:	d8d7d0e3          	bge	a5,a3,b72c <check_level+0x80>
    b9b0:	fc1ff06f          	j	b970 <check_level+0x2c4>

0000b9b4 <spawn_enemy>:
    b9b4:	fe010113          	addi	sp,sp,-32
    b9b8:	01412423          	sw	s4,8(sp)
    b9bc:	00852a03          	lw	s4,8(a0)
    b9c0:	000017b7          	lui	a5,0x1
    b9c4:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    b9c8:	02fa4a33          	div	s4,s4,a5
    b9cc:	00812c23          	sw	s0,24(sp)
    b9d0:	01312623          	sw	s3,12(sp)
    b9d4:	01512223          	sw	s5,4(sp)
    b9d8:	00112e23          	sw	ra,28(sp)
    b9dc:	00050993          	mv	s3,a0
    b9e0:	00058a93          	mv	s5,a1
    b9e4:	0b850793          	addi	a5,a0,184
    b9e8:	00000413          	li	s0,0
    b9ec:	0c000693          	li	a3,192
    b9f0:	0100006f          	j	ba00 <spawn_enemy+0x4c>
    b9f4:	00140413          	addi	s0,s0,1
    b9f8:	02c78793          	addi	a5,a5,44
    b9fc:	20d40863          	beq	s0,a3,bc0c <spawn_enemy+0x258>
    ba00:	0007a703          	lw	a4,0(a5)
    ba04:	fe0718e3          	bnez	a4,b9f4 <spawn_enemy+0x40>
    ba08:	0049a683          	lw	a3,4(s3)
    ba0c:	00912a23          	sw	s1,20(sp)
    ba10:	01212823          	sw	s2,16(sp)
    ba14:	00d69793          	slli	a5,a3,0xd
    ba18:	00f6c6b3          	xor	a3,a3,a5
    ba1c:	0116d793          	srli	a5,a3,0x11
    ba20:	00f6c6b3          	xor	a3,a3,a5
    ba24:	00569713          	slli	a4,a3,0x5
    ba28:	00e6c733          	xor	a4,a3,a4
    ba2c:	00d71793          	slli	a5,a4,0xd
    ba30:	00e7c7b3          	xor	a5,a5,a4
    ba34:	0117d713          	srli	a4,a5,0x11
    ba38:	00e7c7b3          	xor	a5,a5,a4
    ba3c:	00579713          	slli	a4,a5,0x5
    ba40:	00e7c7b3          	xor	a5,a5,a4
    ba44:	00d79713          	slli	a4,a5,0xd
    ba48:	00f74733          	xor	a4,a4,a5
    ba4c:	01175613          	srli	a2,a4,0x11
    ba50:	00c74733          	xor	a4,a4,a2
    ba54:	00571613          	slli	a2,a4,0x5
    ba58:	00c74733          	xor	a4,a4,a2
    ba5c:	01612023          	sw	s6,0(sp)
    ba60:	24400613          	li	a2,580
    ba64:	15800593          	li	a1,344
    ba68:	02c7f7b3          	remu	a5,a5,a2
    ba6c:	00e9a223          	sw	a4,4(s3)
    ba70:	0036f693          	andi	a3,a3,3
    ba74:	00100613          	li	a2,1
    ba78:	02b77733          	remu	a4,a4,a1
    ba7c:	01e78793          	addi	a5,a5,30
    ba80:	05070713          	addi	a4,a4,80
    ba84:	16c68c63          	beq	a3,a2,bbfc <spawn_enemy+0x248>
    ba88:	00200613          	li	a2,2
    ba8c:	10c68263          	beq	a3,a2,bb90 <spawn_enemy+0x1dc>
    ba90:	0e069a63          	bnez	a3,bb84 <spawn_enemy+0x1d0>
    ba94:	000014b7          	lui	s1,0x1
    ba98:	00871913          	slli	s2,a4,0x8
    ba9c:	40048493          	addi	s1,s1,1024 # 1400 <strlen+0x7c>
    baa0:	02c00793          	li	a5,44
    baa4:	02f40b33          	mul	s6,s0,a5
    baa8:	02c00613          	li	a2,44
    baac:	00000593          	li	a1,0
    bab0:	0b8b0513          	addi	a0,s6,184
    bab4:	00a98533          	add	a0,s3,a0
    bab8:	e4cf50ef          	jal	1104 <memset>
    babc:	016987b3          	add	a5,s3,s6
    bac0:	00100713          	li	a4,1
    bac4:	0ae7ac23          	sw	a4,184(a5)
    bac8:	0a97ae23          	sw	s1,188(a5)
    bacc:	0d27a023          	sw	s2,192(a5)
    bad0:	0d57a623          	sw	s5,204(a5)
    bad4:	00300713          	li	a4,3
    bad8:	0cea8463          	beq	s5,a4,bba0 <spawn_enemy+0x1ec>
    badc:	0609a703          	lw	a4,96(s3)
    bae0:	0889a683          	lw	a3,136(s3)
    bae4:	00170713          	addi	a4,a4,1
    bae8:	0e068263          	beqz	a3,bbcc <spawn_enemy+0x218>
    baec:	00800693          	li	a3,8
    baf0:	0cd7a823          	sw	a3,208(a5)
    baf4:	06e9a023          	sw	a4,96(s3)
    baf8:	0ce7ac23          	sw	a4,216(a5)
    bafc:	00400713          	li	a4,4
    bb00:	001a1a13          	slli	s4,s4,0x1
    bb04:	12ea8463          	beq	s5,a4,bc2c <spawn_enemy+0x278>
    bb08:	00200713          	li	a4,2
    bb0c:	00ba0793          	addi	a5,s4,11
    bb10:	00ea8a63          	beq	s5,a4,bb24 <spawn_enemy+0x170>
    bb14:	fffa8793          	addi	a5,s5,-1
    bb18:	00f037b3          	snez	a5,a5
    bb1c:	014787b3          	add	a5,a5,s4
    bb20:	00378793          	addi	a5,a5,3
    bb24:	02c00693          	li	a3,44
    bb28:	02d406b3          	mul	a3,s0,a3
    bb2c:	06900713          	li	a4,105
    bb30:	00d986b3          	add	a3,s3,a3
    bb34:	0cf6a223          	sw	a5,196(a3)
    bb38:	0cf6a423          	sw	a5,200(a3)
    bb3c:	02c00793          	li	a5,44
    bb40:	02f407b3          	mul	a5,s0,a5
    bb44:	01c12083          	lw	ra,28(sp)
    bb48:	01812403          	lw	s0,24(sp)
    bb4c:	01412483          	lw	s1,20(sp)
    bb50:	01012903          	lw	s2,16(sp)
    bb54:	00012b03          	lw	s6,0(sp)
    bb58:	00812a03          	lw	s4,8(sp)
    bb5c:	00412a83          	lw	s5,4(sp)
    bb60:	00100513          	li	a0,1
    bb64:	00f987b3          	add	a5,s3,a5
    bb68:	0ce7ae23          	sw	a4,220(a5)
    bb6c:	0649a783          	lw	a5,100(s3)
    bb70:	00178793          	addi	a5,a5,1
    bb74:	06f9a223          	sw	a5,100(s3)
    bb78:	00c12983          	lw	s3,12(sp)
    bb7c:	02010113          	addi	sp,sp,32
    bb80:	00008067          	ret
    bb84:	00879493          	slli	s1,a5,0x8
    bb88:	0001b937          	lui	s2,0x1b
    bb8c:	f15ff06f          	j	baa0 <spawn_enemy+0xec>
    bb90:	00004937          	lui	s2,0x4
    bb94:	00879493          	slli	s1,a5,0x8
    bb98:	40090913          	addi	s2,s2,1024 # 4400 <__stack_size+0x400>
    bb9c:	f05ff06f          	j	baa0 <spawn_enemy+0xec>
    bba0:	05a00713          	li	a4,90
    bba4:	0ce7a823          	sw	a4,208(a5)
    bba8:	0609a683          	lw	a3,96(s3)
    bbac:	004a1713          	slli	a4,s4,0x4
    bbb0:	41470733          	sub	a4,a4,s4
    bbb4:	00168693          	addi	a3,a3,1
    bbb8:	06d9a023          	sw	a3,96(s3)
    bbbc:	00271713          	slli	a4,a4,0x2
    bbc0:	0cd7ac23          	sw	a3,216(a5)
    bbc4:	0a070793          	addi	a5,a4,160
    bbc8:	f5dff06f          	j	bb24 <spawn_enemy+0x170>
    bbcc:	02d00693          	li	a3,45
    bbd0:	0cd7a823          	sw	a3,208(a5)
    bbd4:	06e9a023          	sw	a4,96(s3)
    bbd8:	0ce7ac23          	sw	a4,216(a5)
    bbdc:	00400713          	li	a4,4
    bbe0:	001a1a13          	slli	s4,s4,0x1
    bbe4:	f2ea92e3          	bne	s5,a4,bb08 <spawn_enemy+0x154>
    bbe8:	006a0a13          	addi	s4,s4,6
    bbec:	0d47a223          	sw	s4,196(a5)
    bbf0:	0d47a423          	sw	s4,200(a5)
    bbf4:	02400713          	li	a4,36
    bbf8:	f45ff06f          	j	bb3c <spawn_enemy+0x188>
    bbfc:	000274b7          	lui	s1,0x27
    bc00:	00871913          	slli	s2,a4,0x8
    bc04:	c0048493          	addi	s1,s1,-1024 # 26c00 <background_pixels+0xed40>
    bc08:	e99ff06f          	j	baa0 <spawn_enemy+0xec>
    bc0c:	01c12083          	lw	ra,28(sp)
    bc10:	01812403          	lw	s0,24(sp)
    bc14:	00c12983          	lw	s3,12(sp)
    bc18:	00812a03          	lw	s4,8(sp)
    bc1c:	00412a83          	lw	s5,4(sp)
    bc20:	00000513          	li	a0,0
    bc24:	02010113          	addi	sp,sp,32
    bc28:	00008067          	ret
    bc2c:	00018737          	lui	a4,0x18
    bc30:	6a070713          	addi	a4,a4,1696 # 186a0 <background_pixels+0x7e0>
    bc34:	0ce7a223          	sw	a4,196(a5)
    bc38:	0ce7a423          	sw	a4,200(a5)
    bc3c:	08c9a783          	lw	a5,140(s3)
    bc40:	00200713          	li	a4,2
    bc44:	fff78693          	addi	a3,a5,-1
    bc48:	02d76263          	bltu	a4,a3,bc6c <spawn_enemy+0x2b8>
    bc4c:	00379713          	slli	a4,a5,0x3
    bc50:	40f70733          	sub	a4,a4,a5
    bc54:	000137b7          	lui	a5,0x13
    bc58:	00271713          	slli	a4,a4,0x2
    bc5c:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    bc60:	00e787b3          	add	a5,a5,a4
    bc64:	0147a703          	lw	a4,20(a5)
    bc68:	ed5ff06f          	j	bb3c <spawn_enemy+0x188>
    bc6c:	01402783          	lw	a5,20(zero) # 14 <regnum_s4>
    bc70:	00100073          	ebreak

0000bc74 <hostile_shot>:
    bc74:	08852883          	lw	a7,136(a0)
    bc78:	fd010113          	addi	sp,sp,-48
    bc7c:	00003837          	lui	a6,0x3
    bc80:	02812423          	sw	s0,40(sp)
    bc84:	03212023          	sw	s2,32(sp)
    bc88:	01312e23          	sw	s3,28(sp)
    bc8c:	01412c23          	sw	s4,24(sp)
    bc90:	01512a23          	sw	s5,20(sp)
    bc94:	01612823          	sw	s6,16(sp)
    bc98:	5b880813          	addi	a6,a6,1464 # 35b8 <render_init+0x250>
    bc9c:	02112623          	sw	ra,44(sp)
    bca0:	00050413          	mv	s0,a0
    bca4:	00058b13          	mv	s6,a1
    bca8:	00060993          	mv	s3,a2
    bcac:	00068913          	mv	s2,a3
    bcb0:	00070a93          	mv	s5,a4
    bcb4:	00078a13          	mv	s4,a5
    bcb8:	01050833          	add	a6,a0,a6
    bcbc:	40000e13          	li	t3,1024
    bcc0:	00088a63          	beqz	a7,bcd4 <hostile_shot+0x60>
    bcc4:	0800006f          	j	bd44 <hostile_shot+0xd0>
    bcc8:	00188893          	addi	a7,a7,1
    bccc:	02080813          	addi	a6,a6,32
    bcd0:	05c88863          	beq	a7,t3,bd20 <hostile_shot+0xac>
    bcd4:	00082303          	lw	t1,0(a6)
    bcd8:	fe0318e3          	bnez	t1,bcc8 <hostile_shot+0x54>
    bcdc:	000037b7          	lui	a5,0x3
    bce0:	004b2683          	lw	a3,4(s6)
    bce4:	008b2703          	lw	a4,8(s6)
    bce8:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    bcec:	00589893          	slli	a7,a7,0x5
    bcf0:	00f888b3          	add	a7,a7,a5
    bcf4:	011408b3          	add	a7,s0,a7
    bcf8:	00100793          	li	a5,1
    bcfc:	00f8a023          	sw	a5,0(a7)
    bd00:	00d8a223          	sw	a3,4(a7)
    bd04:	00e8a423          	sw	a4,8(a7)
    bd08:	0138a623          	sw	s3,12(a7)
    bd0c:	0128a823          	sw	s2,16(a7)
    bd10:	12c00793          	li	a5,300
    bd14:	00f8aa23          	sw	a5,20(a7)
    bd18:	0158ac23          	sw	s5,24(a7)
    bd1c:	0148ae23          	sw	s4,28(a7)
    bd20:	02c12083          	lw	ra,44(sp)
    bd24:	02812403          	lw	s0,40(sp)
    bd28:	02012903          	lw	s2,32(sp)
    bd2c:	01c12983          	lw	s3,28(sp)
    bd30:	01812a03          	lw	s4,24(sp)
    bd34:	01412a83          	lw	s5,20(sp)
    bd38:	01012b03          	lw	s6,16(sp)
    bd3c:	03010113          	addi	sp,sp,48
    bd40:	00008067          	ret
    bd44:	000237b7          	lui	a5,0x23
    bd48:	00f507b3          	add	a5,a0,a5
    bd4c:	5b87a603          	lw	a2,1464(a5) # 235b8 <background_pixels+0xb6f8>
    bd50:	02912223          	sw	s1,36(sp)
    bd54:	000014b7          	lui	s1,0x1
    bd58:	00060663          	beqz	a2,bd64 <hostile_shot+0xf0>
    bd5c:	5bc7a483          	lw	s1,1468(a5)
    bd60:	00060813          	mv	a6,a2
    bd64:	08c42703          	lw	a4,140(s0)
    bd68:	00200793          	li	a5,2
    bd6c:	fff70593          	addi	a1,a4,-1
    bd70:	02b7e263          	bltu	a5,a1,bd94 <hostile_shot+0x120>
    bd74:	00371693          	slli	a3,a4,0x3
    bd78:	40e686b3          	sub	a3,a3,a4
    bd7c:	000137b7          	lui	a5,0x13
    bd80:	00269693          	slli	a3,a3,0x2
    bd84:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    bd88:	00d787b3          	add	a5,a5,a3
    bd8c:	0047a783          	lw	a5,4(a5)
    bd90:	0c97c463          	blt	a5,s1,be58 <hostile_shot+0x1e4>
    bd94:	000237b7          	lui	a5,0x23
    bd98:	00f407b3          	add	a5,s0,a5
    bd9c:	5c07a303          	lw	t1,1472(a5) # 235c0 <background_pixels+0xb700>
    bda0:	00034463          	bltz	t1,bda8 <hostile_shot+0x134>
    bda4:	00934c63          	blt	t1,s1,bdbc <hostile_shot+0x148>
    bda8:	000237b7          	lui	a5,0x23
    bdac:	00f407b3          	add	a5,s0,a5
    bdb0:	5c07a023          	sw	zero,1472(a5) # 235c0 <background_pixels+0xb700>
    bdb4:	0a905e63          	blez	s1,be70 <hostile_shot+0x1fc>
    bdb8:	00000313          	li	t1,0
    bdbc:	40930eb3          	sub	t4,t1,s1
    bdc0:	006487b3          	add	a5,s1,t1
    bdc4:	0100006f          	j	bdd4 <hostile_shot+0x160>
    bdc8:	00130313          	addi	t1,t1,1
    bdcc:	001e8e93          	addi	t4,t4,1
    bdd0:	0a678063          	beq	a5,t1,be70 <hostile_shot+0x1fc>
    bdd4:	00030e13          	mv	t3,t1
    bdd8:	00934463          	blt	t1,s1,bde0 <hostile_shot+0x16c>
    bddc:	000e8e13          	mv	t3,t4
    bde0:	005e1893          	slli	a7,t3,0x5
    bde4:	011808b3          	add	a7,a6,a7
    bde8:	0008af03          	lw	t5,0(a7)
    bdec:	fc0f1ee3          	bnez	t5,bdc8 <hostile_shot+0x154>
    bdf0:	001e0e13          	addi	t3,t3,1
    bdf4:	009e5463          	bge	t3,s1,bdfc <hostile_shot+0x188>
    bdf8:	000e0f13          	mv	t5,t3
    bdfc:	004b2603          	lw	a2,4(s6)
    be00:	008b2683          	lw	a3,8(s6)
    be04:	000237b7          	lui	a5,0x23
    be08:	00f407b3          	add	a5,s0,a5
    be0c:	5de7a023          	sw	t5,1472(a5) # 235c0 <background_pixels+0xb700>
    be10:	00100793          	li	a5,1
    be14:	00f8a023          	sw	a5,0(a7)
    be18:	00c8a223          	sw	a2,4(a7)
    be1c:	00d8a423          	sw	a3,8(a7)
    be20:	0138a623          	sw	s3,12(a7)
    be24:	0128a823          	sw	s2,16(a7)
    be28:	fff70793          	addi	a5,a4,-1
    be2c:	00200693          	li	a3,2
    be30:	1ef6e863          	bltu	a3,a5,c020 <hostile_shot+0x3ac>
    be34:	00371793          	slli	a5,a4,0x3
    be38:	40e78733          	sub	a4,a5,a4
    be3c:	000137b7          	lui	a5,0x13
    be40:	00271713          	slli	a4,a4,0x2
    be44:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    be48:	00e787b3          	add	a5,a5,a4
    be4c:	0187a783          	lw	a5,24(a5)
    be50:	02412483          	lw	s1,36(sp)
    be54:	ec1ff06f          	j	bd14 <hostile_shot+0xa0>
    be58:	00078493          	mv	s1,a5
    be5c:	000237b7          	lui	a5,0x23
    be60:	00f407b3          	add	a5,s0,a5
    be64:	5c07a303          	lw	t1,1472(a5) # 235c0 <background_pixels+0xb700>
    be68:	f2035ee3          	bgez	t1,bda4 <hostile_shot+0x130>
    be6c:	f3dff06f          	j	bda8 <hostile_shot+0x134>
    be70:	000237b7          	lui	a5,0x23
    be74:	01712623          	sw	s7,12(sp)
    be78:	00f407b3          	add	a5,s0,a5
    be7c:	01a12023          	sw	s10,0(sp)
    be80:	00200693          	li	a3,2
    be84:	5bc7ab83          	lw	s7,1468(a5) # 235bc <background_pixels+0xb6fc>
    be88:	12b6e863          	bltu	a3,a1,bfb8 <hostile_shot+0x344>
    be8c:	00371793          	slli	a5,a4,0x3
    be90:	40e787b3          	sub	a5,a5,a4
    be94:	00013737          	lui	a4,0x13
    be98:	00279793          	slli	a5,a5,0x2
    be9c:	14470713          	addi	a4,a4,324 # 13144 <stress_settings>
    bea0:	0ff00693          	li	a3,255
    bea4:	00e787b3          	add	a5,a5,a4
    bea8:	0f76c663          	blt	a3,s7,bf94 <hostile_shot+0x320>
    beac:	10000713          	li	a4,256
    beb0:	0e060e63          	beqz	a2,bfac <hostile_shot+0x338>
    beb4:	0047ad03          	lw	s10,4(a5)
    beb8:	01a75463          	bge	a4,s10,bec0 <hostile_shot+0x24c>
    bebc:	00070d13          	mv	s10,a4
    bec0:	0babd863          	bge	s7,s10,bf70 <hostile_shot+0x2fc>
    bec4:	01812423          	sw	s8,8(sp)
    bec8:	00023c37          	lui	s8,0x23
    becc:	01840c33          	add	s8,s0,s8
    bed0:	5c8c2783          	lw	a5,1480(s8) # 235c8 <background_pixels+0xb708>
    bed4:	10078a63          	beqz	a5,bfe8 <hostile_shot+0x374>
    bed8:	5d0c2503          	lw	a0,1488(s8)
    bedc:	01912223          	sw	s9,4(sp)
    bee0:	000d0693          	mv	a3,s10
    bee4:	000b8613          	mv	a2,s7
    bee8:	00080593          	mv	a1,a6
    beec:	000780e7          	jalr	a5
    bef0:	00050c93          	mv	s9,a0
    bef4:	06050a63          	beqz	a0,bf68 <hostile_shot+0x2f4>
    bef8:	417d0633          	sub	a2,s10,s7
    befc:	005b9b93          	slli	s7,s7,0x5
    bf00:	00561613          	slli	a2,a2,0x5
    bf04:	00000593          	li	a1,0
    bf08:	01750533          	add	a0,a0,s7
    bf0c:	9f8f50ef          	jal	1104 <memset>
    bf10:	004b2683          	lw	a3,4(s6)
    bf14:	008b2703          	lw	a4,8(s6)
    bf18:	08842783          	lw	a5,136(s0)
    bf1c:	00148613          	addi	a2,s1,1 # 1001 <_start+0x1>
    bf20:	00549593          	slli	a1,s1,0x5
    bf24:	5ccc2023          	sw	a2,1472(s8)
    bf28:	5b9c2c23          	sw	s9,1464(s8)
    bf2c:	5bac2e23          	sw	s10,1468(s8)
    bf30:	00bc88b3          	add	a7,s9,a1
    bf34:	00100613          	li	a2,1
    bf38:	00c8a023          	sw	a2,0(a7)
    bf3c:	00d8a223          	sw	a3,4(a7)
    bf40:	00e8a423          	sw	a4,8(a7)
    bf44:	0138a623          	sw	s3,12(a7)
    bf48:	0128a823          	sw	s2,16(a7)
    bf4c:	0a078c63          	beqz	a5,c004 <hostile_shot+0x390>
    bf50:	08c42703          	lw	a4,140(s0)
    bf54:	00c12b83          	lw	s7,12(sp)
    bf58:	00812c03          	lw	s8,8(sp)
    bf5c:	00412c83          	lw	s9,4(sp)
    bf60:	00012d03          	lw	s10,0(sp)
    bf64:	ec5ff06f          	j	be28 <hostile_shot+0x1b4>
    bf68:	00812c03          	lw	s8,8(sp)
    bf6c:	00412c83          	lw	s9,4(sp)
    bf70:	000237b7          	lui	a5,0x23
    bf74:	00f407b3          	add	a5,s0,a5
    bf78:	5c47a703          	lw	a4,1476(a5) # 235c4 <background_pixels+0xb704>
    bf7c:	02412483          	lw	s1,36(sp)
    bf80:	00c12b83          	lw	s7,12(sp)
    bf84:	00170713          	addi	a4,a4,1
    bf88:	00012d03          	lw	s10,0(sp)
    bf8c:	5ce7a223          	sw	a4,1476(a5)
    bf90:	d91ff06f          	j	bd20 <hostile_shot+0xac>
    bf94:	001b9713          	slli	a4,s7,0x1
    bf98:	f0061ee3          	bnez	a2,beb4 <hostile_shot+0x240>
    bf9c:	000016b7          	lui	a3,0x1
    bfa0:	00e6d663          	bge	a3,a4,bfac <hostile_shot+0x338>
    bfa4:	00001bb7          	lui	s7,0x1
    bfa8:	f0dff06f          	j	beb4 <hostile_shot+0x240>
    bfac:	00002737          	lui	a4,0x2
    bfb0:	00001bb7          	lui	s7,0x1
    bfb4:	f01ff06f          	j	beb4 <hostile_shot+0x240>
    bfb8:	0ff00793          	li	a5,255
    bfbc:	0177ce63          	blt	a5,s7,bfd8 <hostile_shot+0x364>
    bfc0:	01812423          	sw	s8,8(sp)
    bfc4:	10000d13          	li	s10,256
    bfc8:	f00610e3          	bnez	a2,bec8 <hostile_shot+0x254>
    bfcc:	00002d37          	lui	s10,0x2
    bfd0:	00001bb7          	lui	s7,0x1
    bfd4:	ef5ff06f          	j	bec8 <hostile_shot+0x254>
    bfd8:	001b9d13          	slli	s10,s7,0x1
    bfdc:	00060a63          	beqz	a2,bff0 <hostile_shot+0x37c>
    bfe0:	f9abd8e3          	bge	s7,s10,bf70 <hostile_shot+0x2fc>
    bfe4:	ee1ff06f          	j	bec4 <hostile_shot+0x250>
    bfe8:	00812c03          	lw	s8,8(sp)
    bfec:	f85ff06f          	j	bf70 <hostile_shot+0x2fc>
    bff0:	01812423          	sw	s8,8(sp)
    bff4:	000017b7          	lui	a5,0x1
    bff8:	fda7dae3          	bge	a5,s10,bfcc <hostile_shot+0x358>
    bffc:	00001bb7          	lui	s7,0x1
    c000:	ec9ff06f          	j	bec8 <hostile_shot+0x254>
    c004:	02412483          	lw	s1,36(sp)
    c008:	00c12b83          	lw	s7,12(sp)
    c00c:	00812c03          	lw	s8,8(sp)
    c010:	00412c83          	lw	s9,4(sp)
    c014:	00012d03          	lw	s10,0(sp)
    c018:	12c00793          	li	a5,300
    c01c:	cf9ff06f          	j	bd14 <hostile_shot+0xa0>
    c020:	01802783          	lw	a5,24(zero) # 18 <regnum_s8>
    c024:	01712623          	sw	s7,12(sp)
    c028:	01812423          	sw	s8,8(sp)
    c02c:	01912223          	sw	s9,4(sp)
    c030:	01a12023          	sw	s10,0(sp)
    c034:	00100073          	ebreak

0000c038 <game_choose_upgrade.part.0>:
    c038:	02858593          	addi	a1,a1,40
    c03c:	00259593          	slli	a1,a1,0x2
    c040:	00b505b3          	add	a1,a0,a1
    c044:	00c5a683          	lw	a3,12(a1)
    c048:	00500593          	li	a1,5
    c04c:	00269793          	slli	a5,a3,0x2
    c050:	00f50633          	add	a2,a0,a5
    c054:	09462703          	lw	a4,148(a2)
    c058:	00170713          	addi	a4,a4,1 # 2001 <main+0xbf1>
    c05c:	08e62a23          	sw	a4,148(a2)
    c060:	02d5e863          	bltu	a1,a3,c090 <game_choose_upgrade.part.0+0x58>
    c064:	00013737          	lui	a4,0x13
    c068:	05c70713          	addi	a4,a4,92 # 1305c <fb_addr+0xc>
    c06c:	00e787b3          	add	a5,a5,a4
    c070:	0007a783          	lw	a5,0(a5) # 1000 <_start>
    c074:	00078067          	jr	a5
    c078:	04452783          	lw	a5,68(a0)
    c07c:	00400713          	li	a4,4
    c080:	00178793          	addi	a5,a5,1
    c084:	0ef75063          	bge	a4,a5,c164 <game_choose_upgrade.part.0+0x12c>
    c088:	00400793          	li	a5,4
    c08c:	04f52223          	sw	a5,68(a0)
    c090:	00200793          	li	a5,2
    c094:	00f52023          	sw	a5,0(a0)
    c098:	03c00793          	li	a5,60
    c09c:	02f52023          	sw	a5,32(a0)
    c0a0:	05a00793          	li	a5,90
    c0a4:	06f52423          	sw	a5,104(a0)
    c0a8:	00300793          	li	a5,3
    c0ac:	08f52823          	sw	a5,144(a0)
    c0b0:	dfcff06f          	j	b6ac <check_level>
    c0b4:	04852783          	lw	a5,72(a0)
    c0b8:	0be00713          	li	a4,190
    c0bc:	02878793          	addi	a5,a5,40
    c0c0:	08f75863          	bge	a4,a5,c150 <game_choose_upgrade.part.0+0x118>
    c0c4:	0be00793          	li	a5,190
    c0c8:	04f52423          	sw	a5,72(a0)
    c0cc:	fc5ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c0d0:	03852783          	lw	a5,56(a0)
    c0d4:	03c00713          	li	a4,60
    c0d8:	ffd78793          	addi	a5,a5,-3
    c0dc:	00f75463          	bge	a4,a5,c0e4 <game_choose_upgrade.part.0+0xac>
    c0e0:	03c00793          	li	a5,60
    c0e4:	00700713          	li	a4,7
    c0e8:	00e7d463          	bge	a5,a4,c0f0 <game_choose_upgrade.part.0+0xb8>
    c0ec:	00700793          	li	a5,7
    c0f0:	02f52c23          	sw	a5,56(a0)
    c0f4:	f9dff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c0f8:	03c52783          	lw	a5,60(a0)
    c0fc:	00178793          	addi	a5,a5,1
    c100:	02f52e23          	sw	a5,60(a0)
    c104:	f8dff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c108:	04052783          	lw	a5,64(a0)
    c10c:	3b600713          	li	a4,950
    c110:	05078793          	addi	a5,a5,80
    c114:	06f75063          	bge	a4,a5,c174 <game_choose_upgrade.part.0+0x13c>
    c118:	3b600793          	li	a5,950
    c11c:	04f52023          	sw	a5,64(a0)
    c120:	f71ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c124:	01c52703          	lw	a4,28(a0)
    c128:	01852783          	lw	a5,24(a0)
    c12c:	00000693          	li	a3,0
    c130:	01470713          	addi	a4,a4,20
    c134:	00e52e23          	sw	a4,28(a0)
    c138:	02878793          	addi	a5,a5,40
    c13c:	0007c663          	bltz	a5,c148 <game_choose_upgrade.part.0+0x110>
    c140:	00070693          	mv	a3,a4
    c144:	04e7c263          	blt	a5,a4,c188 <game_choose_upgrade.part.0+0x150>
    c148:	00d52c23          	sw	a3,24(a0)
    c14c:	f45ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c150:	fff7c713          	not	a4,a5
    c154:	41f75713          	srai	a4,a4,0x1f
    c158:	00e7f7b3          	and	a5,a5,a4
    c15c:	04f52423          	sw	a5,72(a0)
    c160:	f31ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c164:	f2f044e3          	bgtz	a5,c08c <game_choose_upgrade.part.0+0x54>
    c168:	00100793          	li	a5,1
    c16c:	04f52223          	sw	a5,68(a0)
    c170:	f21ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c174:	fff7c713          	not	a4,a5
    c178:	41f75713          	srai	a4,a4,0x1f
    c17c:	00e7f7b3          	and	a5,a5,a4
    c180:	04f52023          	sw	a5,64(a0)
    c184:	f0dff06f          	j	c090 <game_choose_upgrade.part.0+0x58>
    c188:	00078693          	mv	a3,a5
    c18c:	00d52c23          	sw	a3,24(a0)
    c190:	f01ff06f          	j	c090 <game_choose_upgrade.part.0+0x58>

0000c194 <begin_run>:
    c194:	fa010113          	addi	sp,sp,-96
    c198:	04812c23          	sw	s0,88(sp)
    c19c:	00023437          	lui	s0,0x23
    c1a0:	00850433          	add	s0,a0,s0
    c1a4:	05312623          	sw	s3,76(sp)
    c1a8:	05412423          	sw	s4,72(sp)
    c1ac:	5b842983          	lw	s3,1464(s0) # 235b8 <background_pixels+0xb6f8>
    c1b0:	5bc42a03          	lw	s4,1468(s0)
    c1b4:	05512223          	sw	s5,68(sp)
    c1b8:	05612023          	sw	s6,64(sp)
    c1bc:	5d042a83          	lw	s5,1488(s0)
    c1c0:	5cc42b03          	lw	s6,1484(s0)
    c1c4:	03712e23          	sw	s7,60(sp)
    c1c8:	5c842b83          	lw	s7,1480(s0)
    c1cc:	04912a23          	sw	s1,84(sp)
    c1d0:	00060493          	mv	s1,a2
    c1d4:	00025637          	lui	a2,0x25
    c1d8:	05212823          	sw	s2,80(sp)
    c1dc:	1d460613          	addi	a2,a2,468 # 251d4 <background_pixels+0xd314>
    c1e0:	00058913          	mv	s2,a1
    c1e4:	00000593          	li	a1,0
    c1e8:	03912a23          	sw	s9,52(sp)
    c1ec:	04112e23          	sw	ra,92(sp)
    c1f0:	00050c93          	mv	s9,a0
    c1f4:	f11f40ef          	jal	1104 <memset>
    c1f8:	5b342c23          	sw	s3,1464(s0)
    c1fc:	5b442e23          	sw	s4,1468(s0)
    c200:	5d742423          	sw	s7,1480(s0)
    c204:	5d642623          	sw	s6,1484(s0)
    c208:	5d542823          	sw	s5,1488(s0)
    c20c:	00098463          	beqz	s3,c214 <begin_run+0x80>
    c210:	2d404663          	bgtz	s4,c4dc <begin_run+0x348>
    c214:	00091663          	bnez	s2,c220 <begin_run+0x8c>
    c218:	12345937          	lui	s2,0x12345
    c21c:	67890913          	addi	s2,s2,1656 # 12345678 <__freertos_irq_stack_top+0x1225d4d8>
    c220:	00903733          	snez	a4,s1
    c224:	000107b7          	lui	a5,0x10
    c228:	00200693          	li	a3,2
    c22c:	c0078793          	addi	a5,a5,-1024 # fc00 <demo_squares_render+0x3c>
    c230:	08eca423          	sw	a4,136(s9)
    c234:	00014737          	lui	a4,0x14
    c238:	012ca223          	sw	s2,4(s9)
    c23c:	00dca023          	sw	a3,0(s9)
    c240:	089ca623          	sw	s1,140(s9)
    c244:	00eca823          	sw	a4,16(s9)
    c248:	00fcaa23          	sw	a5,20(s9)
    c24c:	30048a63          	beqz	s1,c560 <begin_run+0x3cc>
    c250:	00349793          	slli	a5,s1,0x3
    c254:	40978733          	sub	a4,a5,s1
    c258:	00013bb7          	lui	s7,0x13
    c25c:	03b12623          	sw	s11,44(sp)
    c260:	144b8613          	addi	a2,s7,324 # 13144 <stress_settings>
    c264:	00271713          	slli	a4,a4,0x2
    c268:	00e60733          	add	a4,a2,a4
    c26c:	02dcae23          	sw	a3,60(s9)
    c270:	01100693          	li	a3,17
    c274:	3e700593          	li	a1,999
    c278:	00c12823          	sw	a2,16(sp)
    c27c:	02dcac23          	sw	a3,56(s9)
    c280:	00072603          	lw	a2,0(a4) # 14000 <__global_pointer$+0x3f8>
    c284:	03e00693          	li	a3,62
    c288:	00100713          	li	a4,1
    c28c:	00bcac23          	sw	a1,24(s9)
    c290:	00bcae23          	sw	a1,28(s9)
    c294:	04eca223          	sw	a4,68(s9)
    c298:	23000593          	li	a1,560
    c29c:	04dca423          	sw	a3,72(s9)
    c2a0:	04ecaa23          	sw	a4,84(s9)
    c2a4:	00600693          	li	a3,6
    c2a8:	02eca223          	sw	a4,36(s9)
    c2ac:	02eca623          	sw	a4,44(s9)
    c2b0:	0b400713          	li	a4,180
    c2b4:	06eca423          	sw	a4,104(s9)
    c2b8:	04bca023          	sw	a1,64(s9)
    c2bc:	04dca823          	sw	a3,80(s9)
    c2c0:	0c000713          	li	a4,192
    c2c4:	00060993          	mv	s3,a2
    c2c8:	22c74463          	blt	a4,a2,c4f0 <begin_run+0x35c>
    c2cc:	00000d93          	li	s11,0
    c2d0:	0b8c8a13          	addi	s4,s9,184
    c2d4:	1ac05e63          	blez	a2,c490 <begin_run+0x2fc>
    c2d8:	000017b7          	lui	a5,0x1
    c2dc:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    c2e0:	00027737          	lui	a4,0x27
    c2e4:	000046b7          	lui	a3,0x4
    c2e8:	00f12623          	sw	a5,12(sp)
    c2ec:	c0070793          	addi	a5,a4,-1024 # 26c00 <background_pixels+0xed40>
    c2f0:	00001637          	lui	a2,0x1
    c2f4:	00f12a23          	sw	a5,20(sp)
    c2f8:	40068793          	addi	a5,a3,1024 # 4400 <__stack_size+0x400>
    c2fc:	00018ab7          	lui	s5,0x18
    c300:	00f12c23          	sw	a5,24(sp)
    c304:	40060793          	addi	a5,a2,1024 # 1400 <strlen+0x7c>
    c308:	03a12823          	sw	s10,48(sp)
    c30c:	03812c23          	sw	s8,56(sp)
    c310:	0c000d13          	li	s10,192
    c314:	15800b13          	li	s6,344
    c318:	6a0a8a93          	addi	s5,s5,1696 # 186a0 <background_pixels+0x7e0>
    c31c:	00f12e23          	sw	a5,28(sp)
    c320:	000a0713          	mv	a4,s4
    c324:	00000b93          	li	s7,0
    c328:	0100006f          	j	c338 <begin_run+0x1a4>
    c32c:	001b8b93          	addi	s7,s7,1
    c330:	02c70713          	addi	a4,a4,44
    c334:	13ab8c63          	beq	s7,s10,c46c <begin_run+0x2d8>
    c338:	00072683          	lw	a3,0(a4)
    c33c:	fe0698e3          	bnez	a3,c32c <begin_run+0x198>
    c340:	004ca603          	lw	a2,4(s9)
    c344:	00100593          	li	a1,1
    c348:	008ca903          	lw	s2,8(s9)
    c34c:	00d61713          	slli	a4,a2,0xd
    c350:	00e64633          	xor	a2,a2,a4
    c354:	01165713          	srli	a4,a2,0x11
    c358:	00e64633          	xor	a2,a2,a4
    c35c:	00561693          	slli	a3,a2,0x5
    c360:	00d646b3          	xor	a3,a2,a3
    c364:	00d69713          	slli	a4,a3,0xd
    c368:	00d74733          	xor	a4,a4,a3
    c36c:	01175693          	srli	a3,a4,0x11
    c370:	00d74733          	xor	a4,a4,a3
    c374:	00571693          	slli	a3,a4,0x5
    c378:	00d74733          	xor	a4,a4,a3
    c37c:	00d71693          	slli	a3,a4,0xd
    c380:	00e6c6b3          	xor	a3,a3,a4
    c384:	0116d513          	srli	a0,a3,0x11
    c388:	00a6c6b3          	xor	a3,a3,a0
    c38c:	00569513          	slli	a0,a3,0x5
    c390:	00a6c6b3          	xor	a3,a3,a0
    c394:	0366f4b3          	remu	s1,a3,s6
    c398:	00367613          	andi	a2,a2,3
    c39c:	00dca223          	sw	a3,4(s9)
    c3a0:	05048493          	addi	s1,s1,80
    c3a4:	1ab60863          	beq	a2,a1,c554 <begin_run+0x3c0>
    c3a8:	24400693          	li	a3,580
    c3ac:	02d77733          	remu	a4,a4,a3
    c3b0:	00200693          	li	a3,2
    c3b4:	01e70713          	addi	a4,a4,30
    c3b8:	18d60863          	beq	a2,a3,c548 <begin_run+0x3b4>
    c3bc:	18060063          	beqz	a2,c53c <begin_run+0x3a8>
    c3c0:	00871413          	slli	s0,a4,0x8
    c3c4:	0001b4b7          	lui	s1,0x1b
    c3c8:	02c00713          	li	a4,44
    c3cc:	02eb8c33          	mul	s8,s7,a4
    c3d0:	02c00613          	li	a2,44
    c3d4:	00000593          	li	a1,0
    c3d8:	0b8c0513          	addi	a0,s8,184
    c3dc:	00ac8533          	add	a0,s9,a0
    c3e0:	d25f40ef          	jal	1104 <memset>
    c3e4:	018c8733          	add	a4,s9,s8
    c3e8:	00100693          	li	a3,1
    c3ec:	0ad72c23          	sw	a3,184(a4)
    c3f0:	00400693          	li	a3,4
    c3f4:	0a872e23          	sw	s0,188(a4)
    c3f8:	0c972023          	sw	s1,192(a4)
    c3fc:	0cd72623          	sw	a3,204(a4)
    c400:	088ca683          	lw	a3,136(s9)
    c404:	10068063          	beqz	a3,c504 <begin_run+0x370>
    c408:	00800693          	li	a3,8
    c40c:	0cd72823          	sw	a3,208(a4)
    c410:	060ca683          	lw	a3,96(s9)
    c414:	00200613          	li	a2,2
    c418:	00168693          	addi	a3,a3,1
    c41c:	06dca023          	sw	a3,96(s9)
    c420:	0cd72c23          	sw	a3,216(a4)
    c424:	0d572223          	sw	s5,196(a4)
    c428:	0d572423          	sw	s5,200(a4)
    c42c:	08cca683          	lw	a3,140(s9)
    c430:	fff68713          	addi	a4,a3,-1
    c434:	18e66c63          	bltu	a2,a4,c5cc <begin_run+0x438>
    c438:	01012783          	lw	a5,16(sp)
    c43c:	00369713          	slli	a4,a3,0x3
    c440:	40d70733          	sub	a4,a4,a3
    c444:	00271713          	slli	a4,a4,0x2
    c448:	00e78733          	add	a4,a5,a4
    c44c:	01472583          	lw	a1,20(a4)
    c450:	02c00713          	li	a4,44
    c454:	02eb87b3          	mul	a5,s7,a4
    c458:	00fc87b3          	add	a5,s9,a5
    c45c:	0cb7ae23          	sw	a1,220(a5)
    c460:	064ca783          	lw	a5,100(s9)
    c464:	00178793          	addi	a5,a5,1
    c468:	06fca223          	sw	a5,100(s9)
    c46c:	001d8d93          	addi	s11,s11,1
    c470:	eb3dc8e3          	blt	s11,s3,c320 <begin_run+0x18c>
    c474:	08cca483          	lw	s1,140(s9)
    c478:	00200793          	li	a5,2
    c47c:	fff48713          	addi	a4,s1,-1 # 1afff <background_pixels+0x313f>
    c480:	14e7ea63          	bltu	a5,a4,c5d4 <begin_run+0x440>
    c484:	03812c03          	lw	s8,56(sp)
    c488:	03012d03          	lw	s10,48(sp)
    c48c:	00349793          	slli	a5,s1,0x3
    c490:	01012703          	lw	a4,16(sp)
    c494:	409787b3          	sub	a5,a5,s1
    c498:	00279793          	slli	a5,a5,0x2
    c49c:	00f707b3          	add	a5,a4,a5
    c4a0:	00c7a783          	lw	a5,12(a5)
    c4a4:	02c12d83          	lw	s11,44(sp)
    c4a8:	05c12083          	lw	ra,92(sp)
    c4ac:	05812403          	lw	s0,88(sp)
    c4b0:	04fcae23          	sw	a5,92(s9)
    c4b4:	05412483          	lw	s1,84(sp)
    c4b8:	05012903          	lw	s2,80(sp)
    c4bc:	04c12983          	lw	s3,76(sp)
    c4c0:	04812a03          	lw	s4,72(sp)
    c4c4:	04412a83          	lw	s5,68(sp)
    c4c8:	04012b03          	lw	s6,64(sp)
    c4cc:	03c12b83          	lw	s7,60(sp)
    c4d0:	03412c83          	lw	s9,52(sp)
    c4d4:	06010113          	addi	sp,sp,96
    c4d8:	00008067          	ret
    c4dc:	005a1613          	slli	a2,s4,0x5
    c4e0:	00000593          	li	a1,0
    c4e4:	00098513          	mv	a0,s3
    c4e8:	c1df40ef          	jal	1104 <memset>
    c4ec:	d29ff06f          	j	c214 <begin_run+0x80>
    c4f0:	0c000993          	li	s3,192
    c4f4:	00000d93          	li	s11,0
    c4f8:	0b8c8a13          	addi	s4,s9,184
    c4fc:	dcc04ee3          	bgtz	a2,c2d8 <begin_run+0x144>
    c500:	f91ff06f          	j	c490 <begin_run+0x2fc>
    c504:	00c12783          	lw	a5,12(sp)
    c508:	02d00613          	li	a2,45
    c50c:	0cc72823          	sw	a2,208(a4)
    c510:	02f946b3          	div	a3,s2,a5
    c514:	060ca603          	lw	a2,96(s9)
    c518:	02400593          	li	a1,36
    c51c:	00160613          	addi	a2,a2,1
    c520:	06cca023          	sw	a2,96(s9)
    c524:	0cc72c23          	sw	a2,216(a4)
    c528:	00169693          	slli	a3,a3,0x1
    c52c:	00668693          	addi	a3,a3,6
    c530:	0cd72223          	sw	a3,196(a4)
    c534:	0cd72423          	sw	a3,200(a4)
    c538:	f19ff06f          	j	c450 <begin_run+0x2bc>
    c53c:	01c12403          	lw	s0,28(sp)
    c540:	00849493          	slli	s1,s1,0x8
    c544:	e85ff06f          	j	c3c8 <begin_run+0x234>
    c548:	01812483          	lw	s1,24(sp)
    c54c:	00871413          	slli	s0,a4,0x8
    c550:	e79ff06f          	j	c3c8 <begin_run+0x234>
    c554:	01412403          	lw	s0,20(sp)
    c558:	00849493          	slli	s1,s1,0x8
    c55c:	e6dff06f          	j	c3c8 <begin_run+0x234>
    c560:	06400713          	li	a4,100
    c564:	00ecac23          	sw	a4,24(s9)
    c568:	00ecae23          	sw	a4,28(s9)
    c56c:	23000713          	li	a4,560
    c570:	04eca023          	sw	a4,64(s9)
    c574:	01100713          	li	a4,17
    c578:	00100793          	li	a5,1
    c57c:	02ecac23          	sw	a4,56(s9)
    c580:	03e00713          	li	a4,62
    c584:	04fca223          	sw	a5,68(s9)
    c588:	04eca423          	sw	a4,72(s9)
    c58c:	04fcaa23          	sw	a5,84(s9)
    c590:	00600713          	li	a4,6
    c594:	02fca223          	sw	a5,36(s9)
    c598:	02fca623          	sw	a5,44(s9)
    c59c:	09600793          	li	a5,150
    c5a0:	02dcae23          	sw	a3,60(s9)
    c5a4:	04eca823          	sw	a4,80(s9)
    c5a8:	06fca423          	sw	a5,104(s9)
    c5ac:	00500413          	li	s0,5
    c5b0:	00000593          	li	a1,0
    c5b4:	000c8513          	mv	a0,s9
    c5b8:	fff40413          	addi	s0,s0,-1
    c5bc:	bf8ff0ef          	jal	b9b4 <spawn_enemy>
    c5c0:	fe0418e3          	bnez	s0,c5b0 <begin_run+0x41c>
    c5c4:	01e00793          	li	a5,30
    c5c8:	ee1ff06f          	j	c4a8 <begin_run+0x314>
    c5cc:	01402783          	lw	a5,20(zero) # 14 <regnum_s4>
    c5d0:	00100073          	ebreak
    c5d4:	00c02783          	lw	a5,12(zero) # c <regnum_a2>
    c5d8:	00100073          	ebreak

0000c5dc <begin_run.constprop.0>:
    c5dc:	fc010113          	addi	sp,sp,-64
    c5e0:	02912a23          	sw	s1,52(sp)
    c5e4:	000234b7          	lui	s1,0x23
    c5e8:	009504b3          	add	s1,a0,s1
    c5ec:	03312623          	sw	s3,44(sp)
    c5f0:	03412423          	sw	s4,40(sp)
    c5f4:	5b84a983          	lw	s3,1464(s1) # 235b8 <background_pixels+0xb6f8>
    c5f8:	5bc4aa03          	lw	s4,1468(s1)
    c5fc:	03512223          	sw	s5,36(sp)
    c600:	03612023          	sw	s6,32(sp)
    c604:	5d04aa83          	lw	s5,1488(s1)
    c608:	5cc4ab03          	lw	s6,1484(s1)
    c60c:	01712e23          	sw	s7,28(sp)
    c610:	5c84ab83          	lw	s7,1480(s1)
    c614:	00025637          	lui	a2,0x25
    c618:	03212823          	sw	s2,48(sp)
    c61c:	1d460613          	addi	a2,a2,468 # 251d4 <background_pixels+0xd314>
    c620:	00058913          	mv	s2,a1
    c624:	00000593          	li	a1,0
    c628:	02812c23          	sw	s0,56(sp)
    c62c:	01812c23          	sw	s8,24(sp)
    c630:	02112e23          	sw	ra,60(sp)
    c634:	01912a23          	sw	s9,20(sp)
    c638:	01a12823          	sw	s10,16(sp)
    c63c:	01b12623          	sw	s11,12(sp)
    c640:	00050413          	mv	s0,a0
    c644:	ac1f40ef          	jal	1104 <memset>
    c648:	5b34ac23          	sw	s3,1464(s1)
    c64c:	5b44ae23          	sw	s4,1468(s1)
    c650:	5d74a423          	sw	s7,1480(s1)
    c654:	5d64a623          	sw	s6,1484(s1)
    c658:	5d54a823          	sw	s5,1488(s1)
    c65c:	00000c13          	li	s8,0
    c660:	00098463          	beqz	s3,c668 <begin_run.constprop.0+0x8c>
    c664:	23404063          	bgtz	s4,c884 <begin_run.constprop.0+0x2a8>
    c668:	00091663          	bnez	s2,c674 <begin_run.constprop.0+0x98>
    c66c:	12345937          	lui	s2,0x12345
    c670:	67890913          	addi	s2,s2,1656 # 12345678 <__freertos_irq_stack_top+0x1225d4d8>
    c674:	00010737          	lui	a4,0x10
    c678:	c0070713          	addi	a4,a4,-1024 # fc00 <demo_squares_render+0x3c>
    c67c:	00e42a23          	sw	a4,20(s0)
    c680:	23000713          	li	a4,560
    c684:	04e42023          	sw	a4,64(s0)
    c688:	01100713          	li	a4,17
    c68c:	00100793          	li	a5,1
    c690:	02e42c23          	sw	a4,56(s0)
    c694:	03e00713          	li	a4,62
    c698:	00200693          	li	a3,2
    c69c:	06400613          	li	a2,100
    c6a0:	00001d37          	lui	s10,0x1
    c6a4:	000145b7          	lui	a1,0x14
    c6a8:	04f42223          	sw	a5,68(s0)
    c6ac:	04e42423          	sw	a4,72(s0)
    c6b0:	04f42a23          	sw	a5,84(s0)
    c6b4:	00600713          	li	a4,6
    c6b8:	02f42223          	sw	a5,36(s0)
    c6bc:	02f42623          	sw	a5,44(s0)
    c6c0:	09600793          	li	a5,150
    c6c4:	01242223          	sw	s2,4(s0)
    c6c8:	00d42023          	sw	a3,0(s0)
    c6cc:	08042623          	sw	zero,140(s0)
    c6d0:	08042423          	sw	zero,136(s0)
    c6d4:	00b42823          	sw	a1,16(s0)
    c6d8:	00c42c23          	sw	a2,24(s0)
    c6dc:	00c42e23          	sw	a2,28(s0)
    c6e0:	02d42e23          	sw	a3,60(s0)
    c6e4:	04e42823          	sw	a4,80(s0)
    c6e8:	06f42423          	sw	a5,104(s0)
    c6ec:	00500c93          	li	s9,5
    c6f0:	0b840913          	addi	s2,s0,184
    c6f4:	e10d0d13          	addi	s10,s10,-496 # e10 <CUSTOM2+0xdb5>
    c6f8:	0c000493          	li	s1,192
    c6fc:	24400b13          	li	s6,580
    c700:	15800a93          	li	s5,344
    c704:	00100d93          	li	s11,1
    c708:	03ac4c33          	div	s8,s8,s10
    c70c:	00090713          	mv	a4,s2
    c710:	00000793          	li	a5,0
    c714:	00c0006f          	j	c720 <begin_run.constprop.0+0x144>
    c718:	00178793          	addi	a5,a5,1
    c71c:	10978463          	beq	a5,s1,c824 <begin_run.constprop.0+0x248>
    c720:	00072683          	lw	a3,0(a4)
    c724:	02c70713          	addi	a4,a4,44
    c728:	fe0698e3          	bnez	a3,c718 <begin_run.constprop.0+0x13c>
    c72c:	00442603          	lw	a2,4(s0)
    c730:	00d61713          	slli	a4,a2,0xd
    c734:	00e64633          	xor	a2,a2,a4
    c738:	01165713          	srli	a4,a2,0x11
    c73c:	00e64633          	xor	a2,a2,a4
    c740:	00561693          	slli	a3,a2,0x5
    c744:	00d646b3          	xor	a3,a2,a3
    c748:	00d69713          	slli	a4,a3,0xd
    c74c:	00d74733          	xor	a4,a4,a3
    c750:	01175693          	srli	a3,a4,0x11
    c754:	00d74733          	xor	a4,a4,a3
    c758:	00571693          	slli	a3,a4,0x5
    c75c:	00d74733          	xor	a4,a4,a3
    c760:	00d71693          	slli	a3,a4,0xd
    c764:	00e6c6b3          	xor	a3,a3,a4
    c768:	0116d593          	srli	a1,a3,0x11
    c76c:	00b6c6b3          	xor	a3,a3,a1
    c770:	00569593          	slli	a1,a3,0x5
    c774:	00b6c6b3          	xor	a3,a3,a1
    c778:	03677733          	remu	a4,a4,s6
    c77c:	00d42223          	sw	a3,4(s0)
    c780:	00367613          	andi	a2,a2,3
    c784:	0356f6b3          	remu	a3,a3,s5
    c788:	01e70713          	addi	a4,a4,30
    c78c:	05068693          	addi	a3,a3,80
    c790:	11b60e63          	beq	a2,s11,c8ac <begin_run.constprop.0+0x2d0>
    c794:	00200593          	li	a1,2
    c798:	10b60263          	beq	a2,a1,c89c <begin_run.constprop.0+0x2c0>
    c79c:	0c061e63          	bnez	a2,c878 <begin_run.constprop.0+0x29c>
    c7a0:	00001bb7          	lui	s7,0x1
    c7a4:	00869993          	slli	s3,a3,0x8
    c7a8:	400b8b93          	addi	s7,s7,1024 # 1400 <strlen+0x7c>
    c7ac:	02c00713          	li	a4,44
    c7b0:	02e78a33          	mul	s4,a5,a4
    c7b4:	02c00613          	li	a2,44
    c7b8:	00000593          	li	a1,0
    c7bc:	001c1c13          	slli	s8,s8,0x1
    c7c0:	004c0c13          	addi	s8,s8,4
    c7c4:	0b8a0513          	addi	a0,s4,184
    c7c8:	00a40533          	add	a0,s0,a0
    c7cc:	939f40ef          	jal	1104 <memset>
    c7d0:	014407b3          	add	a5,s0,s4
    c7d4:	0bb7ac23          	sw	s11,184(a5)
    c7d8:	0b77ae23          	sw	s7,188(a5)
    c7dc:	0d37a023          	sw	s3,192(a5)
    c7e0:	08842703          	lw	a4,136(s0)
    c7e4:	00173713          	seqz	a4,a4
    c7e8:	40e00733          	neg	a4,a4
    c7ec:	02577713          	andi	a4,a4,37
    c7f0:	00870713          	addi	a4,a4,8
    c7f4:	0ce7a823          	sw	a4,208(a5)
    c7f8:	06042703          	lw	a4,96(s0)
    c7fc:	00170713          	addi	a4,a4,1
    c800:	06e42023          	sw	a4,96(s0)
    c804:	0ce7ac23          	sw	a4,216(a5)
    c808:	06900713          	li	a4,105
    c80c:	0d87a223          	sw	s8,196(a5)
    c810:	0d87a423          	sw	s8,200(a5)
    c814:	0ce7ae23          	sw	a4,220(a5)
    c818:	06442783          	lw	a5,100(s0)
    c81c:	00178793          	addi	a5,a5,1
    c820:	06f42223          	sw	a5,100(s0)
    c824:	fffc8c93          	addi	s9,s9,-1
    c828:	000c8663          	beqz	s9,c834 <begin_run.constprop.0+0x258>
    c82c:	00842c03          	lw	s8,8(s0)
    c830:	ed9ff06f          	j	c708 <begin_run.constprop.0+0x12c>
    c834:	01e00793          	li	a5,30
    c838:	03c12083          	lw	ra,60(sp)
    c83c:	04f42e23          	sw	a5,92(s0)
    c840:	03812403          	lw	s0,56(sp)
    c844:	03412483          	lw	s1,52(sp)
    c848:	03012903          	lw	s2,48(sp)
    c84c:	02c12983          	lw	s3,44(sp)
    c850:	02812a03          	lw	s4,40(sp)
    c854:	02412a83          	lw	s5,36(sp)
    c858:	02012b03          	lw	s6,32(sp)
    c85c:	01c12b83          	lw	s7,28(sp)
    c860:	01812c03          	lw	s8,24(sp)
    c864:	01412c83          	lw	s9,20(sp)
    c868:	01012d03          	lw	s10,16(sp)
    c86c:	00c12d83          	lw	s11,12(sp)
    c870:	04010113          	addi	sp,sp,64
    c874:	00008067          	ret
    c878:	00871b93          	slli	s7,a4,0x8
    c87c:	0001b9b7          	lui	s3,0x1b
    c880:	f2dff06f          	j	c7ac <begin_run.constprop.0+0x1d0>
    c884:	005a1613          	slli	a2,s4,0x5
    c888:	00000593          	li	a1,0
    c88c:	00098513          	mv	a0,s3
    c890:	875f40ef          	jal	1104 <memset>
    c894:	00842c03          	lw	s8,8(s0)
    c898:	dd1ff06f          	j	c668 <begin_run.constprop.0+0x8c>
    c89c:	000049b7          	lui	s3,0x4
    c8a0:	00871b93          	slli	s7,a4,0x8
    c8a4:	40098993          	addi	s3,s3,1024 # 4400 <__stack_size+0x400>
    c8a8:	f05ff06f          	j	c7ac <begin_run.constprop.0+0x1d0>
    c8ac:	00027bb7          	lui	s7,0x27
    c8b0:	00869993          	slli	s3,a3,0x8
    c8b4:	c00b8b93          	addi	s7,s7,-1024 # 26c00 <background_pixels+0xed40>
    c8b8:	ef5ff06f          	j	c7ac <begin_run.constprop.0+0x1d0>

0000c8bc <game_isqrt>:
    c8bc:	400007b7          	lui	a5,0x40000
    c8c0:	04f57063          	bgeu	a0,a5,c900 <game_isqrt+0x44>
    c8c4:	0027d793          	srli	a5,a5,0x2
    c8c8:	fef56ee3          	bltu	a0,a5,c8c4 <game_isqrt+0x8>
    c8cc:	02078663          	beqz	a5,c8f8 <game_isqrt+0x3c>
    c8d0:	00000713          	li	a4,0
    c8d4:	00f706b3          	add	a3,a4,a5
    c8d8:	00175713          	srli	a4,a4,0x1
    c8dc:	00d56663          	bltu	a0,a3,c8e8 <game_isqrt+0x2c>
    c8e0:	00f70733          	add	a4,a4,a5
    c8e4:	40d50533          	sub	a0,a0,a3
    c8e8:	0027d793          	srli	a5,a5,0x2
    c8ec:	fe0794e3          	bnez	a5,c8d4 <game_isqrt+0x18>
    c8f0:	00070513          	mv	a0,a4
    c8f4:	00008067          	ret
    c8f8:	00000513          	li	a0,0
    c8fc:	00008067          	ret
    c900:	400007b7          	lui	a5,0x40000
    c904:	fcdff06f          	j	c8d0 <game_isqrt+0x14>

0000c908 <game_enemy_radius>:
    c908:	fff50513          	addi	a0,a0,-1
    c90c:	00300793          	li	a5,3
    c910:	00a7ee63          	bltu	a5,a0,c92c <game_enemy_radius+0x24>
    c914:	000137b7          	lui	a5,0x13
    c918:	00251513          	slli	a0,a0,0x2
    c91c:	07478793          	addi	a5,a5,116 # 13074 <CSWTCH.122>
    c920:	00a787b3          	add	a5,a5,a0
    c924:	0007a503          	lw	a0,0(a5)
    c928:	00008067          	ret
    c92c:	00a00513          	li	a0,10
    c930:	00008067          	ret

0000c934 <game_hostile_slots>:
    c934:	08852703          	lw	a4,136(a0)
    c938:	00050793          	mv	a5,a0
    c93c:	40000513          	li	a0,1024
    c940:	00070e63          	beqz	a4,c95c <game_hostile_slots+0x28>
    c944:	00023737          	lui	a4,0x23
    c948:	00e78733          	add	a4,a5,a4
    c94c:	5b872683          	lw	a3,1464(a4) # 235b8 <background_pixels+0xb6f8>
    c950:	00001537          	lui	a0,0x1
    c954:	00068463          	beqz	a3,c95c <game_hostile_slots+0x28>
    c958:	5bc72503          	lw	a0,1468(a4)
    c95c:	08c7a783          	lw	a5,140(a5)
    c960:	00200713          	li	a4,2
    c964:	fff78693          	addi	a3,a5,-1
    c968:	02d76263          	bltu	a4,a3,c98c <game_hostile_slots+0x58>
    c96c:	00379713          	slli	a4,a5,0x3
    c970:	40f70733          	sub	a4,a4,a5
    c974:	000137b7          	lui	a5,0x13
    c978:	00271713          	slli	a4,a4,0x2
    c97c:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    c980:	00e787b3          	add	a5,a5,a4
    c984:	0047a783          	lw	a5,4(a5)
    c988:	00a7c463          	blt	a5,a0,c990 <game_hostile_slots+0x5c>
    c98c:	00008067          	ret
    c990:	00078513          	mv	a0,a5
    c994:	00008067          	ret

0000c998 <game_hostile_at>:
    c998:	0805cc63          	bltz	a1,ca30 <game_hostile_at+0x98>
    c99c:	08852603          	lw	a2,136(a0) # 1088 <__libc_init_array+0x18>
    c9a0:	40000693          	li	a3,1024
    c9a4:	00060e63          	beqz	a2,c9c0 <game_hostile_at+0x28>
    c9a8:	000237b7          	lui	a5,0x23
    c9ac:	00f507b3          	add	a5,a0,a5
    c9b0:	5b87a703          	lw	a4,1464(a5) # 235b8 <background_pixels+0xb6f8>
    c9b4:	000016b7          	lui	a3,0x1
    c9b8:	00070463          	beqz	a4,c9c0 <game_hostile_at+0x28>
    c9bc:	5bc7a683          	lw	a3,1468(a5)
    c9c0:	08c52783          	lw	a5,140(a0)
    c9c4:	00200713          	li	a4,2
    c9c8:	fff78813          	addi	a6,a5,-1
    c9cc:	03076263          	bltu	a4,a6,c9f0 <game_hostile_at+0x58>
    c9d0:	00379713          	slli	a4,a5,0x3
    c9d4:	40f70733          	sub	a4,a4,a5
    c9d8:	000137b7          	lui	a5,0x13
    c9dc:	00271713          	slli	a4,a4,0x2
    c9e0:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    c9e4:	00e787b3          	add	a5,a5,a4
    c9e8:	0047a783          	lw	a5,4(a5)
    c9ec:	02d7ce63          	blt	a5,a3,ca28 <game_hostile_at+0x90>
    c9f0:	04d5d063          	bge	a1,a3,ca30 <game_hostile_at+0x98>
    c9f4:	00559593          	slli	a1,a1,0x5
    c9f8:	00060e63          	beqz	a2,ca14 <game_hostile_at+0x7c>
    c9fc:	000237b7          	lui	a5,0x23
    ca00:	00f507b3          	add	a5,a0,a5
    ca04:	5b87a783          	lw	a5,1464(a5) # 235b8 <background_pixels+0xb6f8>
    ca08:	00078663          	beqz	a5,ca14 <game_hostile_at+0x7c>
    ca0c:	00b78533          	add	a0,a5,a1
    ca10:	00008067          	ret
    ca14:	000037b7          	lui	a5,0x3
    ca18:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    ca1c:	00f585b3          	add	a1,a1,a5
    ca20:	00b50533          	add	a0,a0,a1
    ca24:	00008067          	ret
    ca28:	00078693          	mv	a3,a5
    ca2c:	fcd5c4e3          	blt	a1,a3,c9f4 <game_hostile_at+0x5c>
    ca30:	00000513          	li	a0,0
    ca34:	00008067          	ret

0000ca38 <game_hostile_count>:
    ca38:	08c52683          	lw	a3,140(a0)
    ca3c:	08852783          	lw	a5,136(a0)
    ca40:	00050713          	mv	a4,a0
    ca44:	fff68593          	addi	a1,a3,-1 # fff <CUSTOM2+0xfa4>
    ca48:	02079863          	bnez	a5,ca78 <game_hostile_count+0x40>
    ca4c:	00200793          	li	a5,2
    ca50:	10b7e463          	bltu	a5,a1,cb58 <game_hostile_count+0x120>
    ca54:	00369793          	slli	a5,a3,0x3
    ca58:	40d786b3          	sub	a3,a5,a3
    ca5c:	000137b7          	lui	a5,0x13
    ca60:	00269693          	slli	a3,a3,0x2
    ca64:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    ca68:	00d787b3          	add	a5,a5,a3
    ca6c:	0047a503          	lw	a0,4(a5)
    ca70:	14051063          	bnez	a0,cbb0 <game_hostile_count+0x178>
    ca74:	00008067          	ret
    ca78:	00023637          	lui	a2,0x23
    ca7c:	00c50633          	add	a2,a0,a2
    ca80:	5b862783          	lw	a5,1464(a2) # 235b8 <background_pixels+0xb6f8>
    ca84:	06078863          	beqz	a5,caf4 <game_hostile_count+0xbc>
    ca88:	00200813          	li	a6,2
    ca8c:	5bc62503          	lw	a0,1468(a2)
    ca90:	10b86463          	bltu	a6,a1,cb98 <game_hostile_count+0x160>
    ca94:	00369593          	slli	a1,a3,0x3
    ca98:	00013637          	lui	a2,0x13
    ca9c:	40d58833          	sub	a6,a1,a3
    caa0:	14460613          	addi	a2,a2,324 # 13144 <stress_settings>
    caa4:	00281813          	slli	a6,a6,0x2
    caa8:	01060833          	add	a6,a2,a6
    caac:	00482803          	lw	a6,4(a6)
    cab0:	0ca84e63          	blt	a6,a0,cb8c <game_hostile_count+0x154>
    cab4:	fc0500e3          	beqz	a0,ca74 <game_hostile_count+0x3c>
    cab8:	40d586b3          	sub	a3,a1,a3
    cabc:	00269693          	slli	a3,a3,0x2
    cac0:	00d60633          	add	a2,a2,a3
    cac4:	000236b7          	lui	a3,0x23
    cac8:	00d70733          	add	a4,a4,a3
    cacc:	5bc72683          	lw	a3,1468(a4)
    cad0:	00462703          	lw	a4,4(a2)
    cad4:	00e6d463          	bge	a3,a4,cadc <game_hostile_count+0xa4>
    cad8:	00068713          	mv	a4,a3
    cadc:	00e02733          	sgtz	a4,a4
    cae0:	40e00733          	neg	a4,a4
    cae4:	00e7f7b3          	and	a5,a5,a4
    cae8:	08a04063          	bgtz	a0,cb68 <game_hostile_count+0x130>
    caec:	00000513          	li	a0,0
    caf0:	00008067          	ret
    caf4:	00200613          	li	a2,2
    caf8:	0eb66263          	bltu	a2,a1,cbdc <game_hostile_count+0x1a4>
    cafc:	00369593          	slli	a1,a3,0x3
    cb00:	00013637          	lui	a2,0x13
    cb04:	40d58533          	sub	a0,a1,a3
    cb08:	14460613          	addi	a2,a2,324 # 13144 <stress_settings>
    cb0c:	00251513          	slli	a0,a0,0x2
    cb10:	00a60533          	add	a0,a2,a0
    cb14:	00452503          	lw	a0,4(a0)
    cb18:	000018b7          	lui	a7,0x1
    cb1c:	00050813          	mv	a6,a0
    cb20:	00a8d463          	bge	a7,a0,cb28 <game_hostile_count+0xf0>
    cb24:	00001837          	lui	a6,0x1
    cb28:	f40506e3          	beqz	a0,ca74 <game_hostile_count+0x3c>
    cb2c:	40d586b3          	sub	a3,a1,a3
    cb30:	00269693          	slli	a3,a3,0x2
    cb34:	00d60633          	add	a2,a2,a3
    cb38:	00462683          	lw	a3,4(a2)
    cb3c:	08d05a63          	blez	a3,cbd0 <game_hostile_count+0x198>
    cb40:	000037b7          	lui	a5,0x3
    cb44:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    cb48:	00080513          	mv	a0,a6
    cb4c:	00f707b3          	add	a5,a4,a5
    cb50:	00a04c63          	bgtz	a0,cb68 <game_hostile_count+0x130>
    cb54:	f99ff06f          	j	caec <game_hostile_count+0xb4>
    cb58:	000037b7          	lui	a5,0x3
    cb5c:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    cb60:	00f507b3          	add	a5,a0,a5
    cb64:	40000513          	li	a0,1024
    cb68:	00551693          	slli	a3,a0,0x5
    cb6c:	00d786b3          	add	a3,a5,a3
    cb70:	00000513          	li	a0,0
    cb74:	0007a703          	lw	a4,0(a5)
    cb78:	02078793          	addi	a5,a5,32
    cb7c:	00e03733          	snez	a4,a4
    cb80:	00e50533          	add	a0,a0,a4
    cb84:	fef698e3          	bne	a3,a5,cb74 <game_hostile_count+0x13c>
    cb88:	00008067          	ret
    cb8c:	00080513          	mv	a0,a6
    cb90:	ee0502e3          	beqz	a0,ca74 <game_hostile_count+0x3c>
    cb94:	f25ff06f          	j	cab8 <game_hostile_count+0x80>
    cb98:	ec050ee3          	beqz	a0,ca74 <game_hostile_count+0x3c>
    cb9c:	00a02733          	sgtz	a4,a0
    cba0:	40e00733          	neg	a4,a4
    cba4:	00e7f7b3          	and	a5,a5,a4
    cba8:	fca040e3          	bgtz	a0,cb68 <game_hostile_count+0x130>
    cbac:	f41ff06f          	j	caec <game_hostile_count+0xb4>
    cbb0:	f2a05ee3          	blez	a0,caec <game_hostile_count+0xb4>
    cbb4:	40000793          	li	a5,1024
    cbb8:	00a7d463          	bge	a5,a0,cbc0 <game_hostile_count+0x188>
    cbbc:	40000513          	li	a0,1024
    cbc0:	000037b7          	lui	a5,0x3
    cbc4:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    cbc8:	00f707b3          	add	a5,a4,a5
    cbcc:	f9dff06f          	j	cb68 <game_hostile_count+0x130>
    cbd0:	00080513          	mv	a0,a6
    cbd4:	f8a04ae3          	bgtz	a0,cb68 <game_hostile_count+0x130>
    cbd8:	f15ff06f          	j	caec <game_hostile_count+0xb4>
    cbdc:	000037b7          	lui	a5,0x3
    cbe0:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    cbe4:	00f507b3          	add	a5,a0,a5
    cbe8:	00001537          	lui	a0,0x1
    cbec:	f7dff06f          	j	cb68 <game_hostile_count+0x130>

0000cbf0 <game_dropped_shots>:
    cbf0:	000237b7          	lui	a5,0x23
    cbf4:	00f50533          	add	a0,a0,a5
    cbf8:	5c452503          	lw	a0,1476(a0) # 15c4 <main+0x1b4>
    cbfc:	00008067          	ret

0000cc00 <game_init>:
    cc00:	fb010113          	addi	sp,sp,-80
    cc04:	00025637          	lui	a2,0x25
    cc08:	04912223          	sw	s1,68(sp)
    cc0c:	1d460613          	addi	a2,a2,468 # 251d4 <background_pixels+0xd314>
    cc10:	00058493          	mv	s1,a1
    cc14:	00000593          	li	a1,0
    cc18:	04812423          	sw	s0,72(sp)
    cc1c:	04112623          	sw	ra,76(sp)
    cc20:	05212023          	sw	s2,64(sp)
    cc24:	03312e23          	sw	s3,60(sp)
    cc28:	03412c23          	sw	s4,56(sp)
    cc2c:	03512a23          	sw	s5,52(sp)
    cc30:	03612823          	sw	s6,48(sp)
    cc34:	03712623          	sw	s7,44(sp)
    cc38:	03812423          	sw	s8,40(sp)
    cc3c:	03912223          	sw	s9,36(sp)
    cc40:	03a12023          	sw	s10,32(sp)
    cc44:	01b12e23          	sw	s11,28(sp)
    cc48:	00050413          	mv	s0,a0
    cc4c:	cb8f40ef          	jal	1104 <memset>
    cc50:	00049663          	bnez	s1,cc5c <game_init+0x5c>
    cc54:	123454b7          	lui	s1,0x12345
    cc58:	67848493          	addi	s1,s1,1656 # 12345678 <__freertos_irq_stack_top+0x1225d4d8>
    cc5c:	00010737          	lui	a4,0x10
    cc60:	c0070713          	addi	a4,a4,-1024 # fc00 <demo_squares_render+0x3c>
    cc64:	00e42a23          	sw	a4,20(s0)
    cc68:	23000713          	li	a4,560
    cc6c:	04e42023          	sw	a4,64(s0)
    cc70:	01100713          	li	a4,17
    cc74:	00100793          	li	a5,1
    cc78:	02e42c23          	sw	a4,56(s0)
    cc7c:	03e00713          	li	a4,62
    cc80:	00200693          	li	a3,2
    cc84:	06400613          	li	a2,100
    cc88:	00001d37          	lui	s10,0x1
    cc8c:	000145b7          	lui	a1,0x14
    cc90:	04f42223          	sw	a5,68(s0)
    cc94:	04e42423          	sw	a4,72(s0)
    cc98:	04f42a23          	sw	a5,84(s0)
    cc9c:	00600713          	li	a4,6
    cca0:	02f42223          	sw	a5,36(s0)
    cca4:	02f42623          	sw	a5,44(s0)
    cca8:	09600793          	li	a5,150
    ccac:	00942223          	sw	s1,4(s0)
    ccb0:	00d42023          	sw	a3,0(s0)
    ccb4:	00b42823          	sw	a1,16(s0)
    ccb8:	00c42c23          	sw	a2,24(s0)
    ccbc:	00c42e23          	sw	a2,28(s0)
    ccc0:	02d42e23          	sw	a3,60(s0)
    ccc4:	04e42823          	sw	a4,80(s0)
    ccc8:	06f42423          	sw	a5,104(s0)
    cccc:	00500c93          	li	s9,5
    ccd0:	00000c13          	li	s8,0
    ccd4:	0b840913          	addi	s2,s0,184
    ccd8:	e10d0d13          	addi	s10,s10,-496 # e10 <CUSTOM2+0xdb5>
    ccdc:	0c000493          	li	s1,192
    cce0:	24400b13          	li	s6,580
    cce4:	15800a93          	li	s5,344
    cce8:	00100d93          	li	s11,1
    ccec:	03ac4c33          	div	s8,s8,s10
    ccf0:	00090713          	mv	a4,s2
    ccf4:	00000793          	li	a5,0
    ccf8:	00c0006f          	j	cd04 <game_init+0x104>
    ccfc:	00178793          	addi	a5,a5,1 # 23001 <background_pixels+0xb141>
    cd00:	10978463          	beq	a5,s1,ce08 <game_init+0x208>
    cd04:	00072683          	lw	a3,0(a4)
    cd08:	02c70713          	addi	a4,a4,44
    cd0c:	fe0698e3          	bnez	a3,ccfc <game_init+0xfc>
    cd10:	00442603          	lw	a2,4(s0)
    cd14:	00d61713          	slli	a4,a2,0xd
    cd18:	00e64633          	xor	a2,a2,a4
    cd1c:	01165713          	srli	a4,a2,0x11
    cd20:	00e64633          	xor	a2,a2,a4
    cd24:	00561693          	slli	a3,a2,0x5
    cd28:	00d646b3          	xor	a3,a2,a3
    cd2c:	00d69713          	slli	a4,a3,0xd
    cd30:	00d74733          	xor	a4,a4,a3
    cd34:	01175693          	srli	a3,a4,0x11
    cd38:	00d74733          	xor	a4,a4,a3
    cd3c:	00571693          	slli	a3,a4,0x5
    cd40:	00d74733          	xor	a4,a4,a3
    cd44:	00d71693          	slli	a3,a4,0xd
    cd48:	00e6c6b3          	xor	a3,a3,a4
    cd4c:	0116d593          	srli	a1,a3,0x11
    cd50:	00b6c6b3          	xor	a3,a3,a1
    cd54:	00569593          	slli	a1,a3,0x5
    cd58:	00b6c6b3          	xor	a3,a3,a1
    cd5c:	03677733          	remu	a4,a4,s6
    cd60:	00d42223          	sw	a3,4(s0)
    cd64:	00367613          	andi	a2,a2,3
    cd68:	0356f6b3          	remu	a3,a3,s5
    cd6c:	01e70713          	addi	a4,a4,30
    cd70:	05068693          	addi	a3,a3,80 # 23050 <background_pixels+0xb190>
    cd74:	11b60463          	beq	a2,s11,ce7c <game_init+0x27c>
    cd78:	00200593          	li	a1,2
    cd7c:	0eb60863          	beq	a2,a1,ce6c <game_init+0x26c>
    cd80:	0e061063          	bnez	a2,ce60 <game_init+0x260>
    cd84:	00001bb7          	lui	s7,0x1
    cd88:	00869993          	slli	s3,a3,0x8
    cd8c:	400b8b93          	addi	s7,s7,1024 # 1400 <strlen+0x7c>
    cd90:	02c00713          	li	a4,44
    cd94:	02e78a33          	mul	s4,a5,a4
    cd98:	02c00613          	li	a2,44
    cd9c:	00000593          	li	a1,0
    cda0:	001c1c13          	slli	s8,s8,0x1
    cda4:	004c0c13          	addi	s8,s8,4
    cda8:	0b8a0513          	addi	a0,s4,184
    cdac:	00a40533          	add	a0,s0,a0
    cdb0:	b54f40ef          	jal	1104 <memset>
    cdb4:	014407b3          	add	a5,s0,s4
    cdb8:	0bb7ac23          	sw	s11,184(a5)
    cdbc:	0b77ae23          	sw	s7,188(a5)
    cdc0:	0d37a023          	sw	s3,192(a5)
    cdc4:	08842703          	lw	a4,136(s0)
    cdc8:	00173713          	seqz	a4,a4
    cdcc:	40e00733          	neg	a4,a4
    cdd0:	02577713          	andi	a4,a4,37
    cdd4:	00870713          	addi	a4,a4,8
    cdd8:	0ce7a823          	sw	a4,208(a5)
    cddc:	06042703          	lw	a4,96(s0)
    cde0:	00170713          	addi	a4,a4,1
    cde4:	06e42023          	sw	a4,96(s0)
    cde8:	0ce7ac23          	sw	a4,216(a5)
    cdec:	06900713          	li	a4,105
    cdf0:	0d87a223          	sw	s8,196(a5)
    cdf4:	0d87a423          	sw	s8,200(a5)
    cdf8:	0ce7ae23          	sw	a4,220(a5)
    cdfc:	06442783          	lw	a5,100(s0)
    ce00:	00178793          	addi	a5,a5,1
    ce04:	06f42223          	sw	a5,100(s0)
    ce08:	fffc8c93          	addi	s9,s9,-1
    ce0c:	000c8663          	beqz	s9,ce18 <game_init+0x218>
    ce10:	00842c03          	lw	s8,8(s0)
    ce14:	ed9ff06f          	j	ccec <game_init+0xec>
    ce18:	01e00793          	li	a5,30
    ce1c:	04c12083          	lw	ra,76(sp)
    ce20:	04f42e23          	sw	a5,92(s0)
    ce24:	00042023          	sw	zero,0(s0)
    ce28:	04812403          	lw	s0,72(sp)
    ce2c:	04412483          	lw	s1,68(sp)
    ce30:	04012903          	lw	s2,64(sp)
    ce34:	03c12983          	lw	s3,60(sp)
    ce38:	03812a03          	lw	s4,56(sp)
    ce3c:	03412a83          	lw	s5,52(sp)
    ce40:	03012b03          	lw	s6,48(sp)
    ce44:	02c12b83          	lw	s7,44(sp)
    ce48:	02812c03          	lw	s8,40(sp)
    ce4c:	02412c83          	lw	s9,36(sp)
    ce50:	02012d03          	lw	s10,32(sp)
    ce54:	01c12d83          	lw	s11,28(sp)
    ce58:	05010113          	addi	sp,sp,80
    ce5c:	00008067          	ret
    ce60:	00871b93          	slli	s7,a4,0x8
    ce64:	0001b9b7          	lui	s3,0x1b
    ce68:	f29ff06f          	j	cd90 <game_init+0x190>
    ce6c:	000049b7          	lui	s3,0x4
    ce70:	00871b93          	slli	s7,a4,0x8
    ce74:	40098993          	addi	s3,s3,1024 # 4400 <__stack_size+0x400>
    ce78:	f19ff06f          	j	cd90 <game_init+0x190>
    ce7c:	00027bb7          	lui	s7,0x27
    ce80:	00869993          	slli	s3,a3,0x8
    ce84:	c00b8b93          	addi	s7,s7,-1024 # 26c00 <background_pixels+0xed40>
    ce88:	f09ff06f          	j	cd90 <game_init+0x190>

0000ce8c <game_tick>:
    ce8c:	00c52783          	lw	a5,12(a0)
    ce90:	f7010113          	addi	sp,sp,-144
    ce94:	08912223          	sw	s1,132(sp)
    ce98:	00178793          	addi	a5,a5,1
    ce9c:	08112623          	sw	ra,140(sp)
    cea0:	00f52623          	sw	a5,12(a0)
    cea4:	04b12423          	sw	a1,72(sp)
    cea8:	04c12623          	sw	a2,76(sp)
    ceac:	01461793          	slli	a5,a2,0x14
    ceb0:	00050493          	mv	s1,a0
    ceb4:	2e07cce3          	bltz	a5,d9ac <game_tick+0xb20>
    ceb8:	04067793          	andi	a5,a2,64
    cebc:	380792e3          	bnez	a5,da40 <game_tick+0xbb4>
    cec0:	00052783          	lw	a5,0(a0)
    cec4:	2c0784e3          	beqz	a5,d98c <game_tick+0xb00>
    cec8:	00100713          	li	a4,1
    cecc:	3ae786e3          	beq	a5,a4,da78 <game_tick+0xbec>
    ced0:	00500713          	li	a4,5
    ced4:	36e782e3          	beq	a5,a4,da38 <game_tick+0xbac>
    ced8:	00400713          	li	a4,4
    cedc:	62e784e3          	beq	a5,a4,dd04 <game_tick+0xe78>
    cee0:	02067613          	andi	a2,a2,32
    cee4:	340610e3          	bnez	a2,da24 <game_tick+0xb98>
    cee8:	00200713          	li	a4,2
    ceec:	30e792e3          	bne	a5,a4,d9f0 <game_tick+0xb64>
    cef0:	00852f03          	lw	t5,8(a0)
    cef4:	09212023          	sw	s2,128(sp)
    cef8:	70800913          	li	s2,1800
    cefc:	001f0613          	addi	a2,t5,1
    cf00:	03264933          	div	s2,a2,s2
    cf04:	05452783          	lw	a5,84(a0)
    cf08:	08812423          	sw	s0,136(sp)
    cf0c:	02052683          	lw	a3,32(a0)
    cf10:	fff78793          	addi	a5,a5,-1
    cf14:	00279713          	slli	a4,a5,0x2
    cf18:	00670713          	addi	a4,a4,6
    cf1c:	07312e23          	sw	s3,124(sp)
    cf20:	07412c23          	sw	s4,120(sp)
    cf24:	07512a23          	sw	s5,116(sp)
    cf28:	07612823          	sw	s6,112(sp)
    cf2c:	07712623          	sw	s7,108(sp)
    cf30:	07812423          	sw	s8,104(sp)
    cf34:	07912223          	sw	s9,100(sp)
    cf38:	07a12023          	sw	s10,96(sp)
    cf3c:	05b12e23          	sw	s11,92(sp)
    cf40:	00c12a23          	sw	a2,20(sp)
    cf44:	00c52423          	sw	a2,8(a0)
    cf48:	02f787b3          	mul	a5,a5,a5
    cf4c:	00191413          	slli	s0,s2,0x1
    cf50:	4017d793          	srai	a5,a5,0x1
    cf54:	00e787b3          	add	a5,a5,a4
    cf58:	008787b3          	add	a5,a5,s0
    cf5c:	04f52823          	sw	a5,80(a0)
    cf60:	00068663          	beqz	a3,cf6c <game_tick+0xe0>
    cf64:	fff68693          	addi	a3,a3,-1
    cf68:	02d52023          	sw	a3,32(a0)
    cf6c:	0684a783          	lw	a5,104(s1)
    cf70:	00078663          	beqz	a5,cf7c <game_tick+0xf0>
    cf74:	fff78793          	addi	a5,a5,-1
    cf78:	06f4a423          	sw	a5,104(s1)
    cf7c:	06c4a783          	lw	a5,108(s1)
    cf80:	00078663          	beqz	a5,cf8c <game_tick+0x100>
    cf84:	fff78793          	addi	a5,a5,-1
    cf88:	06f4a623          	sw	a5,108(s1)
    cf8c:	0704a783          	lw	a5,112(s1)
    cf90:	00078663          	beqz	a5,cf9c <game_tick+0x110>
    cf94:	fff78793          	addi	a5,a5,-1
    cf98:	06f4a823          	sw	a5,112(s1)
    cf9c:	0015d713          	srli	a4,a1,0x1
    cfa0:	0035d793          	srli	a5,a1,0x3
    cfa4:	0025d693          	srli	a3,a1,0x2
    cfa8:	0016f693          	andi	a3,a3,1
    cfac:	0015f593          	andi	a1,a1,1
    cfb0:	00177713          	andi	a4,a4,1
    cfb4:	0017f793          	andi	a5,a5,1
    cfb8:	40d787b3          	sub	a5,a5,a3
    cfbc:	40b70733          	sub	a4,a4,a1
    cfc0:	0404a683          	lw	a3,64(s1)
    cfc4:	00f76633          	or	a2,a4,a5
    cfc8:	00c03633          	snez	a2,a2
    cfcc:	02c4a423          	sw	a2,40(s1)
    cfd0:	0144a503          	lw	a0,20(s1)
    cfd4:	02f68633          	mul	a2,a3,a5
    cfd8:	0104a583          	lw	a1,16(s1)
    cfdc:	560702e3          	beqz	a4,dd40 <game_tick+0xeb4>
    cfe0:	02e686b3          	mul	a3,a3,a4
    cfe4:	02078863          	beqz	a5,d014 <game_tick+0x188>
    cfe8:	0b500793          	li	a5,181
    cfec:	02f686b3          	mul	a3,a3,a5
    cff0:	02f60633          	mul	a2,a2,a5
    cff4:	41f6d793          	srai	a5,a3,0x1f
    cff8:	0ff7f793          	zext.b	a5,a5
    cffc:	00d787b3          	add	a5,a5,a3
    d000:	4087d693          	srai	a3,a5,0x8
    d004:	41f65793          	srai	a5,a2,0x1f
    d008:	0ff7f793          	zext.b	a5,a5
    d00c:	00c787b3          	add	a5,a5,a2
    d010:	4087d793          	srai	a5,a5,0x8
    d014:	00026637          	lui	a2,0x26
    d018:	00b686b3          	add	a3,a3,a1
    d01c:	40060613          	addi	a2,a2,1024 # 26400 <background_pixels+0xe540>
    d020:	00d65463          	bge	a2,a3,d028 <game_tick+0x19c>
    d024:	00060693          	mv	a3,a2
    d028:	00002637          	lui	a2,0x2
    d02c:	c0060613          	addi	a2,a2,-1024 # 1c00 <main+0x7f0>
    d030:	00c6d463          	bge	a3,a2,d038 <game_tick+0x1ac>
    d034:	00060693          	mv	a3,a2
    d038:	0001a637          	lui	a2,0x1a
    d03c:	00d4a823          	sw	a3,16(s1)
    d040:	00a787b3          	add	a5,a5,a0
    d044:	60060613          	addi	a2,a2,1536 # 1a600 <background_pixels+0x2740>
    d048:	00f65463          	bge	a2,a5,d050 <game_tick+0x1c4>
    d04c:	00060793          	mv	a5,a2
    d050:	000056b7          	lui	a3,0x5
    d054:	20068693          	addi	a3,a3,512 # 5200 <__stack_size+0x1200>
    d058:	00d7d463          	bge	a5,a3,d060 <game_tick+0x1d4>
    d05c:	00068793          	mv	a5,a3
    d060:	00f4aa23          	sw	a5,20(s1)
    d064:	02e4a223          	sw	a4,36(s1)
    d068:	0884a983          	lw	s3,136(s1)
    d06c:	00099463          	bnez	s3,d074 <game_tick+0x1e8>
    d070:	3010106f          	j	eb70 <game_tick+0x1ce4>
    d074:	08c4ab03          	lw	s6,140(s1)
    d078:	00200793          	li	a5,2
    d07c:	fffb0713          	addi	a4,s6,-1
    d080:	00e7f463          	bgeu	a5,a4,d088 <game_tick+0x1fc>
    d084:	1dc0206f          	j	f260 <game_tick+0x23d4>
    d088:	05c4a683          	lw	a3,92(s1)
    d08c:	003b1a93          	slli	s5,s6,0x3
    d090:	416a8733          	sub	a4,s5,s6
    d094:	00013a37          	lui	s4,0x13
    d098:	00271793          	slli	a5,a4,0x2
    d09c:	144a0a13          	addi	s4,s4,324 # 13144 <stress_settings>
    d0a0:	fff68713          	addi	a4,a3,-1
    d0a4:	00fa07b3          	add	a5,s4,a5
    d0a8:	04e4ae23          	sw	a4,92(s1)
    d0ac:	0007a903          	lw	s2,0(a5)
    d0b0:	00e04463          	bgtz	a4,d0b8 <game_tick+0x22c>
    d0b4:	4210106f          	j	ecd4 <game_tick+0x1e48>
    d0b8:	01412783          	lw	a5,20(sp)
    d0bc:	0b400713          	li	a4,180
    d0c0:	02e7c7b3          	div	a5,a5,a4
    d0c4:	00f75463          	bge	a4,a5,d0cc <game_tick+0x240>
    d0c8:	0b400793          	li	a5,180
    d0cc:	01412703          	lw	a4,20(sp)
    d0d0:	0a000593          	li	a1,160
    d0d4:	00b12623          	sw	a1,12(sp)
    d0d8:	f4d72713          	slti	a4,a4,-179
    d0dc:	0a078613          	addi	a2,a5,160
    d0e0:	00174693          	xori	a3,a4,1
    d0e4:	00071463          	bnez	a4,d0ec <game_tick+0x260>
    d0e8:	00c12623          	sw	a2,12(sp)
    d0ec:	00c12703          	lw	a4,12(sp)
    d0f0:	07800f93          	li	t6,120
    d0f4:	40175713          	srai	a4,a4,0x1
    d0f8:	02e12c23          	sw	a4,56(sp)
    d0fc:	07878713          	addi	a4,a5,120
    d100:	00068463          	beqz	a3,d108 <game_tick+0x27c>
    d104:	00070f93          	mv	t6,a4
    d108:	01412683          	lw	a3,20(sp)
    d10c:	00002737          	lui	a4,0x2
    d110:	c2070713          	addi	a4,a4,-992 # 1c20 <main+0x810>
    d114:	02e6c8b3          	div	a7,a3,a4
    d118:	07378693          	addi	a3,a5,115
    d11c:	12278793          	addi	a5,a5,290
    d120:	02f12623          	sw	a5,44(sp)
    d124:	00300793          	li	a5,3
    d128:	01400713          	li	a4,20
    d12c:	02d12423          	sw	a3,40(sp)
    d130:	02ffc7b3          	div	a5,t6,a5
    d134:	02f12e23          	sw	a5,60(sp)
    d138:	01175463          	bge	a4,a7,d140 <game_tick+0x2b4>
    d13c:	01400893          	li	a7,20
    d140:	00013bb7          	lui	s7,0x13
    d144:	074b8793          	addi	a5,s7,116 # 13074 <CSWTCH.122>
    d148:	00002eb7          	lui	t4,0x2
    d14c:	00f12423          	sw	a5,8(sp)
    d150:	000137b7          	lui	a5,0x13
    d154:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    d158:	1b8e8e93          	addi	t4,t4,440 # 21b8 <main+0xda8>
    d15c:	0b848b13          	addi	s6,s1,184
    d160:	00100c13          	li	s8,1
    d164:	01d48eb3          	add	t4,s1,t4
    d168:	02f12a23          	sw	a5,52(sp)
    d16c:	00048793          	mv	a5,s1
    d170:	000b0413          	mv	s0,s6
    d174:	000c0493          	mv	s1,s8
    d178:	05112023          	sw	a7,64(sp)
    d17c:	05612223          	sw	s6,68(sp)
    d180:	03f12223          	sw	t6,36(sp)
    d184:	000e8e13          	mv	t3,t4
    d188:	00078c13          	mv	s8,a5
    d18c:	00042783          	lw	a5,0(s0)
    d190:	5c0780e3          	beqz	a5,df50 <game_tick+0x10c4>
    d194:	01c42783          	lw	a5,28(s0)
    d198:	00078663          	beqz	a5,d1a4 <game_tick+0x318>
    d19c:	fff78793          	addi	a5,a5,-1
    d1a0:	00f42e23          	sw	a5,28(s0)
    d1a4:	01842d83          	lw	s11,24(s0)
    d1a8:	5a0d90e3          	bnez	s11,df48 <game_tick+0x10bc>
    d1ac:	00442783          	lw	a5,4(s0)
    d1b0:	00842f83          	lw	t6,8(s0)
    d1b4:	010c2683          	lw	a3,16(s8)
    d1b8:	014c2703          	lw	a4,20(s8)
    d1bc:	01442f03          	lw	t5,20(s0)
    d1c0:	40f686b3          	sub	a3,a3,a5
    d1c4:	41f70733          	sub	a4,a4,t6
    d1c8:	41f6d813          	srai	a6,a3,0x1f
    d1cc:	41f75893          	srai	a7,a4,0x1f
    d1d0:	0ff87813          	zext.b	a6,a6
    d1d4:	0ff8f893          	zext.b	a7,a7
    d1d8:	00d80833          	add	a6,a6,a3
    d1dc:	00e888b3          	add	a7,a7,a4
    d1e0:	40885813          	srai	a6,a6,0x8
    d1e4:	4088d893          	srai	a7,a7,0x8
    d1e8:	03188633          	mul	a2,a7,a7
    d1ec:	00300713          	li	a4,3
    d1f0:	00078293          	mv	t0,a5
    d1f4:	000f8593          	mv	a1,t6
    d1f8:	030806b3          	mul	a3,a6,a6
    d1fc:	00c686b3          	add	a3,a3,a2
    d200:	00068613          	mv	a2,a3
    d204:	54ef0ae3          	beq	t5,a4,df58 <game_tick+0x10cc>
    d208:	00400713          	li	a4,4
    d20c:	6cef0ee3          	beq	t5,a4,e0e8 <game_tick+0x125c>
    d210:	00200713          	li	a4,2
    d214:	78ef0ce3          	beq	t5,a4,e1ac <game_tick+0x1320>
    d218:	00100713          	li	a4,1
    d21c:	00c12383          	lw	t2,12(sp)
    d220:	00ef1463          	bne	t5,a4,d228 <game_tick+0x39c>
    d224:	35c0106f          	j	e580 <game_tick+0x16f4>
    d228:	40000737          	lui	a4,0x40000
    d22c:	78e6dee3          	bge	a3,a4,e1c8 <game_tick+0x133c>
    d230:	00275713          	srli	a4,a4,0x2
    d234:	fee6eee3          	bltu	a3,a4,d230 <game_tick+0x3a4>
    d238:	34070ce3          	beqz	a4,dd90 <game_tick+0xf04>
    d23c:	00000693          	li	a3,0
    d240:	00d70533          	add	a0,a4,a3
    d244:	0016d693          	srli	a3,a3,0x1
    d248:	00a66663          	bltu	a2,a0,d254 <game_tick+0x3c8>
    d24c:	40a60633          	sub	a2,a2,a0
    d250:	00e686b3          	add	a3,a3,a4
    d254:	00275713          	srli	a4,a4,0x2
    d258:	fe0714e3          	bnez	a4,d240 <game_tick+0x3b4>
    d25c:	088c2703          	lw	a4,136(s8)
    d260:	00d05e63          	blez	a3,d27c <game_tick+0x3f0>
    d264:	02780833          	mul	a6,a6,t2
    d268:	027888b3          	mul	a7,a7,t2
    d26c:	02d84833          	div	a6,a6,a3
    d270:	02d8c8b3          	div	a7,a7,a3
    d274:	010782b3          	add	t0,a5,a6
    d278:	011f85b3          	add	a1,t6,a7
    d27c:	00542223          	sw	t0,4(s0)
    d280:	00b42423          	sw	a1,8(s0)
    d284:	30070ee3          	beqz	a4,dda0 <game_tick+0xf14>
    d288:	00028793          	mv	a5,t0
    d28c:	02c40913          	addi	s2,s0,44
    d290:	000276b7          	lui	a3,0x27
    d294:	a0068693          	addi	a3,a3,-1536 # 26a00 <background_pixels+0xeb40>
    d298:	00078a13          	mv	s4,a5
    d29c:	00f6d463          	bge	a3,a5,d2a4 <game_tick+0x418>
    d2a0:	00068a13          	mv	s4,a3
    d2a4:	000017b7          	lui	a5,0x1
    d2a8:	60078793          	addi	a5,a5,1536 # 1600 <main+0x1f0>
    d2ac:	00fa5463          	bge	s4,a5,d2b4 <game_tick+0x428>
    d2b0:	00078a13          	mv	s4,a5
    d2b4:	0001b7b7          	lui	a5,0x1b
    d2b8:	01442223          	sw	s4,4(s0)
    d2bc:	80078793          	addi	a5,a5,-2048 # 1a800 <background_pixels+0x2940>
    d2c0:	00058993          	mv	s3,a1
    d2c4:	00b7d463          	bge	a5,a1,d2cc <game_tick+0x440>
    d2c8:	00078993          	mv	s3,a5
    d2cc:	000057b7          	lui	a5,0x5
    d2d0:	00f9d463          	bge	s3,a5,d2d8 <game_tick+0x44c>
    d2d4:	000059b7          	lui	s3,0x5
    d2d8:	01342423          	sw	s3,8(s0)
    d2dc:	ffff0793          	addi	a5,t5,-1
    d2e0:	00300613          	li	a2,3
    d2e4:	01200693          	li	a3,18
    d2e8:	00f66c63          	bltu	a2,a5,d300 <game_tick+0x474>
    d2ec:	00812683          	lw	a3,8(sp)
    d2f0:	00279793          	slli	a5,a5,0x2
    d2f4:	00f687b3          	add	a5,a3,a5
    d2f8:	0007a683          	lw	a3,0(a5) # 5000 <__stack_size+0x1000>
    d2fc:	00868693          	addi	a3,a3,8
    d300:	020c2783          	lw	a5,32(s8)
    d304:	10079c63          	bnez	a5,d41c <game_tick+0x590>
    d308:	010c2583          	lw	a1,16(s8)
    d30c:	014c2603          	lw	a2,20(s8)
    d310:	02d686b3          	mul	a3,a3,a3
    d314:	41458833          	sub	a6,a1,s4
    d318:	413607b3          	sub	a5,a2,s3
    d31c:	41f85b13          	srai	s6,a6,0x1f
    d320:	41f7da93          	srai	s5,a5,0x1f
    d324:	0ffb7b13          	zext.b	s6,s6
    d328:	0ffafa93          	zext.b	s5,s5
    d32c:	010b0b33          	add	s6,s6,a6
    d330:	00fa8ab3          	add	s5,s5,a5
    d334:	408b5b13          	srai	s6,s6,0x8
    d338:	408ada93          	srai	s5,s5,0x8
    d33c:	036b07b3          	mul	a5,s6,s6
    d340:	035a8833          	mul	a6,s5,s5
    d344:	01078cb3          	add	s9,a5,a6
    d348:	0cdcda63          	bge	s9,a3,d41c <game_tick+0x590>
    d34c:	00100693          	li	a3,1
    d350:	02071863          	bnez	a4,d380 <game_tick+0x4f4>
    d354:	ffef0713          	addi	a4,t5,-2
    d358:	00e6b733          	sltu	a4,a3,a4
    d35c:	01412783          	lw	a5,20(sp)
    d360:	40e00733          	neg	a4,a4
    d364:	ffffe837          	lui	a6,0xffffe
    d368:	ffa77693          	andi	a3,a4,-6
    d36c:	3e180813          	addi	a6,a6,993 # ffffe3e1 <__freertos_irq_stack_top+0xfff16241>
    d370:	01068693          	addi	a3,a3,16
    d374:	0107c663          	blt	a5,a6,d380 <game_tick+0x4f4>
    d378:	04012783          	lw	a5,64(sp)
    d37c:	00f686b3          	add	a3,a3,a5
    d380:	018c2703          	lw	a4,24(s8)
    d384:	000c0513          	mv	a0,s8
    d388:	01c12823          	sw	t3,16(sp)
    d38c:	40d70733          	sub	a4,a4,a3
    d390:	00ec2c23          	sw	a4,24(s8)
    d394:	01400713          	li	a4,20
    d398:	02ec2023          	sw	a4,32(s8)
    d39c:	00800713          	li	a4,8
    d3a0:	06ec2623          	sw	a4,108(s8)
    d3a4:	00100693          	li	a3,1
    d3a8:	00c00713          	li	a4,12
    d3ac:	a10fe0ef          	jal	b5bc <particles>
    d3b0:	01012e03          	lw	t3,16(sp)
    d3b4:	000c8613          	mv	a2,s9
    d3b8:	40000737          	lui	a4,0x40000
    d3bc:	00275713          	srli	a4,a4,0x2
    d3c0:	feeceee3          	bltu	s9,a4,d3bc <game_tick+0x530>
    d3c4:	04070863          	beqz	a4,d414 <game_tick+0x588>
    d3c8:	00000693          	li	a3,0
    d3cc:	00d707b3          	add	a5,a4,a3
    d3d0:	0016d693          	srli	a3,a3,0x1
    d3d4:	00f66663          	bltu	a2,a5,d3e0 <game_tick+0x554>
    d3d8:	40f60633          	sub	a2,a2,a5
    d3dc:	00e686b3          	add	a3,a3,a4
    d3e0:	00275713          	srli	a4,a4,0x2
    d3e4:	fe0714e3          	bnez	a4,d3cc <game_tick+0x540>
    d3e8:	02068663          	beqz	a3,d414 <game_tick+0x588>
    d3ec:	002b1713          	slli	a4,s6,0x2
    d3f0:	002a9793          	slli	a5,s5,0x2
    d3f4:	40eb0733          	sub	a4,s6,a4
    d3f8:	40fa87b3          	sub	a5,s5,a5
    d3fc:	00a71713          	slli	a4,a4,0xa
    d400:	00a79793          	slli	a5,a5,0xa
    d404:	02d74733          	div	a4,a4,a3
    d408:	02d7c7b3          	div	a5,a5,a3
    d40c:	00ea0a33          	add	s4,s4,a4
    d410:	00f989b3          	add	s3,s3,a5
    d414:	01442223          	sw	s4,4(s0)
    d418:	01342423          	sw	s3,8(s0)
    d41c:	00148493          	addi	s1,s1,1
    d420:	0c100793          	li	a5,193
    d424:	00090413          	mv	s0,s2
    d428:	d6f492e3          	bne	s1,a5,d18c <game_tick+0x300>
    d42c:	034c2783          	lw	a5,52(s8)
    d430:	04412b03          	lw	s6,68(sp)
    d434:	000c0493          	mv	s1,s8
    d438:	00078463          	beqz	a5,d440 <game_tick+0x5b4>
    d43c:	5800106f          	j	e9bc <game_tick+0x1b30>
    d440:	0104a583          	lw	a1,16(s1)
    d444:	0144a703          	lw	a4,20(s1)
    d448:	00016837          	lui	a6,0x16
    d44c:	41f5d313          	srai	t1,a1,0x1f
    d450:	41f75893          	srai	a7,a4,0x1f
    d454:	0ff37313          	zext.b	t1,t1
    d458:	0ff8f893          	zext.b	a7,a7
    d45c:	00b30333          	add	t1,t1,a1
    d460:	00e888b3          	add	a7,a7,a4
    d464:	40835313          	srai	t1,t1,0x8
    d468:	4088d893          	srai	a7,a7,0x8
    d46c:	000b0693          	mv	a3,s6
    d470:	fff00e13          	li	t3,-1
    d474:	f9080813          	addi	a6,a6,-112 # 15f90 <elite_pixels+0x1090>
    d478:	00000613          	li	a2,0
    d47c:	0c000513          	li	a0,192
    d480:	0006a783          	lw	a5,0(a3)
    d484:	04078a63          	beqz	a5,d4d8 <game_tick+0x64c>
    d488:	0186a783          	lw	a5,24(a3)
    d48c:	04079663          	bnez	a5,d4d8 <game_tick+0x64c>
    d490:	0046af83          	lw	t6,4(a3)
    d494:	0086af03          	lw	t5,8(a3)
    d498:	41ffde93          	srai	t4,t6,0x1f
    d49c:	41ff5793          	srai	a5,t5,0x1f
    d4a0:	0ffefe93          	zext.b	t4,t4
    d4a4:	0ff7f793          	zext.b	a5,a5
    d4a8:	01fe8eb3          	add	t4,t4,t6
    d4ac:	01e787b3          	add	a5,a5,t5
    d4b0:	408ede93          	srai	t4,t4,0x8
    d4b4:	4087d793          	srai	a5,a5,0x8
    d4b8:	406e8eb3          	sub	t4,t4,t1
    d4bc:	411787b3          	sub	a5,a5,a7
    d4c0:	02f787b3          	mul	a5,a5,a5
    d4c4:	03de8eb3          	mul	t4,t4,t4
    d4c8:	01d787b3          	add	a5,a5,t4
    d4cc:	0107d663          	bge	a5,a6,d4d8 <game_tick+0x64c>
    d4d0:	00078813          	mv	a6,a5
    d4d4:	00060e13          	mv	t3,a2
    d4d8:	00160613          	addi	a2,a2,1
    d4dc:	02c68693          	addi	a3,a3,44
    d4e0:	faa610e3          	bne	a2,a0,d480 <game_tick+0x5f4>
    d4e4:	1c0e4463          	bltz	t3,d6ac <game_tick+0x820>
    d4e8:	02c00793          	li	a5,44
    d4ec:	02fe07b3          	mul	a5,t3,a5
    d4f0:	400006b7          	lui	a3,0x40000
    d4f4:	00f487b3          	add	a5,s1,a5
    d4f8:	0bc7ae83          	lw	t4,188(a5)
    d4fc:	0c07a783          	lw	a5,192(a5)
    d500:	40be8eb3          	sub	t4,t4,a1
    d504:	40e787b3          	sub	a5,a5,a4
    d508:	41f7d813          	srai	a6,a5,0x1f
    d50c:	41fed893          	srai	a7,t4,0x1f
    d510:	0ff8f893          	zext.b	a7,a7
    d514:	0ff87813          	zext.b	a6,a6
    d518:	00f80833          	add	a6,a6,a5
    d51c:	01d888b3          	add	a7,a7,t4
    d520:	4088d893          	srai	a7,a7,0x8
    d524:	40885813          	srai	a6,a6,0x8
    d528:	030807b3          	mul	a5,a6,a6
    d52c:	03188633          	mul	a2,a7,a7
    d530:	00f60633          	add	a2,a2,a5
    d534:	400007b7          	lui	a5,0x40000
    d538:	00d65863          	bge	a2,a3,d548 <game_tick+0x6bc>
    d53c:	0027d793          	srli	a5,a5,0x2
    d540:	fef66ee3          	bltu	a2,a5,d53c <game_tick+0x6b0>
    d544:	1e0788e3          	beqz	a5,df34 <game_tick+0x10a8>
    d548:	00000693          	li	a3,0
    d54c:	00d78533          	add	a0,a5,a3
    d550:	0016d693          	srli	a3,a3,0x1
    d554:	00a66663          	bltu	a2,a0,d560 <game_tick+0x6d4>
    d558:	40a60633          	sub	a2,a2,a0
    d55c:	00f686b3          	add	a3,a3,a5
    d560:	0027d793          	srli	a5,a5,0x2
    d564:	fe0794e3          	bnez	a5,d54c <game_tick+0x6c0>
    d568:	1cd056e3          	blez	a3,df34 <game_tick+0x10a8>
    d56c:	00389313          	slli	t1,a7,0x3
    d570:	41130333          	sub	t1,t1,a7
    d574:	00831313          	slli	t1,t1,0x8
    d578:	02d34333          	div	t1,t1,a3
    d57c:	00381e13          	slli	t3,a6,0x3
    d580:	410e0e33          	sub	t3,t3,a6
    d584:	008e1e13          	slli	t3,t3,0x8
    d588:	02de4e33          	div	t3,t3,a3
    d58c:	00031463          	bnez	t1,d594 <game_tick+0x708>
    d590:	1a0e04e3          	beqz	t3,df38 <game_tick+0x10ac>
    d594:	0314a623          	sw	a7,44(s1)
    d598:	0304a823          	sw	a6,48(s1)
    d59c:	f0100693          	li	a3,-255
    d5a0:	00100793          	li	a5,1
    d5a4:	00ded463          	bge	t4,a3,d5ac <game_tick+0x720>
    d5a8:	fff00793          	li	a5,-1
    d5ac:	0444a383          	lw	t2,68(s1)
    d5b0:	02f4a223          	sw	a5,36(s1)
    d5b4:	0e705463          	blez	t2,d69c <game_tick+0x810>
    d5b8:	fff38793          	addi	a5,t2,-1
    d5bc:	003e1f93          	slli	t6,t3,0x3
    d5c0:	00331e93          	slli	t4,t1,0x3
    d5c4:	00ee0a33          	add	s4,t3,a4
    d5c8:	41cf8fb3          	sub	t6,t6,t3
    d5cc:	406e8eb3          	sub	t4,t4,t1
    d5d0:	000022b7          	lui	t0,0x2
    d5d4:	002f9f93          	slli	t6,t6,0x2
    d5d8:	002e9e93          	slli	t4,t4,0x2
    d5dc:	00000893          	li	a7,0
    d5e0:	1b828293          	addi	t0,t0,440 # 21b8 <main+0xda8>
    d5e4:	000029b7          	lui	s3,0x2
    d5e8:	00100913          	li	s2,1
    d5ec:	006585b3          	add	a1,a1,t1
    d5f0:	03c00413          	li	s0,60
    d5f4:	02fe06b3          	mul	a3,t3,a5
    d5f8:	02f307b3          	mul	a5,t1,a5
    d5fc:	00369613          	slli	a2,a3,0x3
    d600:	40c686b3          	sub	a3,a3,a2
    d604:	00169813          	slli	a6,a3,0x1
    d608:	0a000613          	li	a2,160
    d60c:	00379713          	slli	a4,a5,0x3
    d610:	40e787b3          	sub	a5,a5,a4
    d614:	00179513          	slli	a0,a5,0x1
    d618:	00548733          	add	a4,s1,t0
    d61c:	00000793          	li	a5,0
    d620:	0100006f          	j	d630 <game_tick+0x7a4>
    d624:	00178793          	addi	a5,a5,1 # 40000001 <__freertos_irq_stack_top+0x3ff17e61>
    d628:	02070713          	addi	a4,a4,32 # 40000020 <__freertos_irq_stack_top+0x3ff17e80>
    d62c:	06c78863          	beq	a5,a2,d69c <game_tick+0x810>
    d630:	00072683          	lw	a3,0(a4)
    d634:	fe0698e3          	bnez	a3,d624 <game_tick+0x798>
    d638:	41f85693          	srai	a3,a6,0x1f
    d63c:	41f55713          	srai	a4,a0,0x1f
    d640:	0ff6f693          	zext.b	a3,a3
    d644:	0ff77713          	zext.b	a4,a4
    d648:	010686b3          	add	a3,a3,a6
    d64c:	00a70733          	add	a4,a4,a0
    d650:	00579793          	slli	a5,a5,0x5
    d654:	00f487b3          	add	a5,s1,a5
    d658:	4086d693          	srai	a3,a3,0x8
    d65c:	40875713          	srai	a4,a4,0x8
    d660:	00f987b3          	add	a5,s3,a5
    d664:	01c70733          	add	a4,a4,t3
    d668:	40d306b3          	sub	a3,t1,a3
    d66c:	1b27ac23          	sw	s2,440(a5)
    d670:	1ab7ae23          	sw	a1,444(a5)
    d674:	1d47a023          	sw	s4,448(a5)
    d678:	1cd7a223          	sw	a3,452(a5)
    d67c:	1ce7a423          	sw	a4,456(a5)
    d680:	03c4a703          	lw	a4,60(s1)
    d684:	00188893          	addi	a7,a7,1 # 1001 <_start+0x1>
    d688:	1c87a623          	sw	s0,460(a5)
    d68c:	1ce7a823          	sw	a4,464(a5)
    d690:	01f80833          	add	a6,a6,t6
    d694:	01d50533          	add	a0,a0,t4
    d698:	f91390e3          	bne	t2,a7,d618 <game_tick+0x78c>
    d69c:	0384a783          	lw	a5,56(s1)
    d6a0:	00400713          	li	a4,4
    d6a4:	06e4a823          	sw	a4,112(s1)
    d6a8:	02f4aa23          	sw	a5,52(s1)
    d6ac:	00002937          	lui	s2,0x2
    d6b0:	00003bb7          	lui	s7,0x3
    d6b4:	00023f37          	lui	t5,0x23
    d6b8:	800009b7          	lui	s3,0x80000
    d6bc:	1b890913          	addi	s2,s2,440 # 21b8 <main+0xda8>
    d6c0:	5b8b8b93          	addi	s7,s7,1464 # 35b8 <render_init+0x250>
    d6c4:	ffffcd37          	lui	s10,0xffffc
    d6c8:	00018cb7          	lui	s9,0x18
    d6cc:	00013c37          	lui	s8,0x13
    d6d0:	5d4f0f13          	addi	t5,t5,1492 # 235d4 <background_pixels+0xb714>
    d6d4:	fff98793          	addi	a5,s3,-1 # 7fffffff <__freertos_irq_stack_top+0x7ff17e5f>
    d6d8:	01248933          	add	s2,s1,s2
    d6dc:	01748bb3          	add	s7,s1,s7
    d6e0:	600d0d13          	addi	s10,s10,1536 # ffffc600 <__freertos_irq_stack_top+0xfff14460>
    d6e4:	200c8c93          	addi	s9,s9,512 # 18200 <background_pixels+0x340>
    d6e8:	074c0c13          	addi	s8,s8,116 # 13074 <CSWTCH.122>
    d6ec:	00f12623          	sw	a5,12(sp)
    d6f0:	01e12823          	sw	t5,16(sp)
    d6f4:	00092783          	lw	a5,0(s2)
    d6f8:	12078e63          	beqz	a5,d834 <game_tick+0x9a8>
    d6fc:	00492583          	lw	a1,4(s2)
    d700:	00892603          	lw	a2,8(s2)
    d704:	01492783          	lw	a5,20(s2)
    d708:	00c92e03          	lw	t3,12(s2)
    d70c:	01092803          	lw	a6,16(s2)
    d710:	fff78793          	addi	a5,a5,-1
    d714:	00be05b3          	add	a1,t3,a1
    d718:	00c80633          	add	a2,a6,a2
    d71c:	00b92223          	sw	a1,4(s2)
    d720:	00c92423          	sw	a2,8(s2)
    d724:	00f92a23          	sw	a5,20(s2)
    d728:	00f05ce3          	blez	a5,df40 <game_tick+0x10b4>
    d72c:	80058793          	addi	a5,a1,-2048 # 13800 <shadow_alpha+0x78>
    d730:	00027737          	lui	a4,0x27
    d734:	00f766e3          	bltu	a4,a5,df40 <game_tick+0x10b4>
    d738:	01a607b3          	add	a5,a2,s10
    d73c:	000b0693          	mv	a3,s6
    d740:	00000413          	li	s0,0
    d744:	00300e93          	li	t4,3
    d748:	0c000513          	li	a0,192
    d74c:	00fcfa63          	bgeu	s9,a5,d760 <game_tick+0x8d4>
    d750:	7f00006f          	j	df40 <game_tick+0x10b4>
    d754:	00140413          	addi	s0,s0,1
    d758:	02c68693          	addi	a3,a3,44 # 4000002c <__freertos_irq_stack_top+0x3ff17e8c>
    d75c:	0ca40c63          	beq	s0,a0,d834 <game_tick+0x9a8>
    d760:	0006a783          	lw	a5,0(a3)
    d764:	fe0788e3          	beqz	a5,d754 <game_tick+0x8c8>
    d768:	0186aa03          	lw	s4,24(a3)
    d76c:	fe0a14e3          	bnez	s4,d754 <game_tick+0x8c8>
    d770:	0046a303          	lw	t1,4(a3)
    d774:	0086a883          	lw	a7,8(a3)
    d778:	0146aa83          	lw	s5,20(a3)
    d77c:	40658fb3          	sub	t6,a1,t1
    d780:	41160f33          	sub	t5,a2,a7
    d784:	41ffd793          	srai	a5,t6,0x1f
    d788:	41ff5713          	srai	a4,t5,0x1f
    d78c:	0ff7f793          	zext.b	a5,a5
    d790:	0ff77713          	zext.b	a4,a4
    d794:	01f787b3          	add	a5,a5,t6
    d798:	01e70733          	add	a4,a4,t5
    d79c:	fffa8f93          	addi	t6,s5,-1
    d7a0:	4087d793          	srai	a5,a5,0x8
    d7a4:	40875713          	srai	a4,a4,0x8
    d7a8:	0a900f13          	li	t5,169
    d7ac:	01feec63          	bltu	t4,t6,d7c4 <game_tick+0x938>
    d7b0:	002f9f93          	slli	t6,t6,0x2
    d7b4:	01fc0fb3          	add	t6,s8,t6
    d7b8:	000faf03          	lw	t5,0(t6)
    d7bc:	003f0f13          	addi	t5,t5,3
    d7c0:	03ef0f33          	mul	t5,t5,t5
    d7c4:	02f787b3          	mul	a5,a5,a5
    d7c8:	02e70733          	mul	a4,a4,a4
    d7cc:	00e787b3          	add	a5,a5,a4
    d7d0:	f9e7d2e3          	bge	a5,t5,d754 <game_tick+0x8c8>
    d7d4:	00300793          	li	a5,3
    d7d8:	02fe4e33          	div	t3,t3,a5
    d7dc:	01892e83          	lw	t4,24(s2)
    d7e0:	00092023          	sw	zero,0(s2)
    d7e4:	00400713          	li	a4,4
    d7e8:	00000693          	li	a3,0
    d7ec:	00048513          	mv	a0,s1
    d7f0:	02f84833          	div	a6,a6,a5
    d7f4:	006e0db3          	add	s11,t3,t1
    d7f8:	02c00313          	li	t1,44
    d7fc:	02640433          	mul	s0,s0,t1
    d800:	011809b3          	add	s3,a6,a7
    d804:	00500313          	li	t1,5
    d808:	00848433          	add	s0,s1,s0
    d80c:	0c442883          	lw	a7,196(s0)
    d810:	0c642a23          	sw	t1,212(s0)
    d814:	0bb42e23          	sw	s11,188(s0)
    d818:	41d888b3          	sub	a7,a7,t4
    d81c:	0d142223          	sw	a7,196(s0)
    d820:	0d342023          	sw	s3,192(s0)
    d824:	01112423          	sw	a7,8(sp)
    d828:	d95fd0ef          	jal	b5bc <particles>
    d82c:	00812883          	lw	a7,8(sp)
    d830:	71105ae3          	blez	a7,e744 <game_tick+0x18b8>
    d834:	02090913          	addi	s2,s2,32
    d838:	eb791ee3          	bne	s2,s7,d6f4 <game_tick+0x868>
    d83c:	08c4a703          	lw	a4,140(s1)
    d840:	0884a903          	lw	s2,136(s1)
    d844:	fff70693          	addi	a3,a4,-1 # 26fff <background_pixels+0xf13f>
    d848:	00090463          	beqz	s2,d850 <game_tick+0x9c4>
    d84c:	1b40106f          	j	ea00 <game_tick+0x1b74>
    d850:	00200793          	li	a5,2
    d854:	00d7f463          	bgeu	a5,a3,d85c <game_tick+0x9d0>
    d858:	1190106f          	j	f170 <game_tick+0x22e4>
    d85c:	00371693          	slli	a3,a4,0x3
    d860:	00013a37          	lui	s4,0x13
    d864:	40e687b3          	sub	a5,a3,a4
    d868:	144a0a13          	addi	s4,s4,324 # 13144 <stress_settings>
    d86c:	00279793          	slli	a5,a5,0x2
    d870:	00fa07b3          	add	a5,s4,a5
    d874:	0047a603          	lw	a2,4(a5)
    d878:	40000793          	li	a5,1024
    d87c:	00060993          	mv	s3,a2
    d880:	00c7d463          	bge	a5,a2,d888 <game_tick+0x9fc>
    d884:	40000993          	li	s3,1024
    d888:	00060463          	beqz	a2,d890 <game_tick+0xa04>
    d88c:	6780106f          	j	ef04 <game_tick+0x2078>
    d890:	00023637          	lui	a2,0x23
    d894:	000248b7          	lui	a7,0x24
    d898:	5d460613          	addi	a2,a2,1492 # 235d4 <background_pixels+0xb714>
    d89c:	1d488893          	addi	a7,a7,468 # 241d4 <background_pixels+0xc314>
    d8a0:	00c48633          	add	a2,s1,a2
    d8a4:	011488b3          	add	a7,s1,a7
    d8a8:	0c300313          	li	t1,195
    d8ac:	40000e37          	lui	t3,0x40000
    d8b0:	0240006f          	j	d8d4 <game_tick+0xa48>
    d8b4:	04c4a783          	lw	a5,76(s1)
    d8b8:	00c62703          	lw	a4,12(a2)
    d8bc:	00e787b3          	add	a5,a5,a4
    d8c0:	04f4a623          	sw	a5,76(s1)
    d8c4:	00062023          	sw	zero,0(a2)
    d8c8:	01060613          	addi	a2,a2,16
    d8cc:	01161463          	bne	a2,a7,d8d4 <game_tick+0xa48>
    d8d0:	0140106f          	j	e8e4 <game_tick+0x1a58>
    d8d4:	00062783          	lw	a5,0(a2)
    d8d8:	fe0788e3          	beqz	a5,d8c8 <game_tick+0xa3c>
    d8dc:	00462803          	lw	a6,4(a2)
    d8e0:	0104a583          	lw	a1,16(s1)
    d8e4:	00862503          	lw	a0,8(a2)
    d8e8:	0144a783          	lw	a5,20(s1)
    d8ec:	410585b3          	sub	a1,a1,a6
    d8f0:	41f5d693          	srai	a3,a1,0x1f
    d8f4:	40a787b3          	sub	a5,a5,a0
    d8f8:	41f7d713          	srai	a4,a5,0x1f
    d8fc:	0ff6f693          	zext.b	a3,a3
    d900:	0ff77713          	zext.b	a4,a4
    d904:	00b686b3          	add	a3,a3,a1
    d908:	00f70733          	add	a4,a4,a5
    d90c:	4086d693          	srai	a3,a3,0x8
    d910:	40875713          	srai	a4,a4,0x8
    d914:	02d685b3          	mul	a1,a3,a3
    d918:	02e707b3          	mul	a5,a4,a4
    d91c:	00f585b3          	add	a1,a1,a5
    d920:	f8b35ae3          	bge	t1,a1,d8b4 <game_tick+0xa28>
    d924:	0484a783          	lw	a5,72(s1)
    d928:	02f787b3          	mul	a5,a5,a5
    d92c:	f8f5dee3          	bge	a1,a5,d8c8 <game_tick+0xa3c>
    d930:	00058f13          	mv	t5,a1
    d934:	400007b7          	lui	a5,0x40000
    d938:	01c5d663          	bge	a1,t3,d944 <game_tick+0xab8>
    d93c:	0027d793          	srli	a5,a5,0x2
    d940:	fef5eee3          	bltu	a1,a5,d93c <game_tick+0xab0>
    d944:	00000593          	li	a1,0
    d948:	00b78eb3          	add	t4,a5,a1
    d94c:	0015d593          	srli	a1,a1,0x1
    d950:	01df6663          	bltu	t5,t4,d95c <game_tick+0xad0>
    d954:	41df0f33          	sub	t5,t5,t4
    d958:	00f585b3          	add	a1,a1,a5
    d95c:	0027d793          	srli	a5,a5,0x2
    d960:	fe0794e3          	bnez	a5,d948 <game_tick+0xabc>
    d964:	00b05e63          	blez	a1,d980 <game_tick+0xaf4>
    d968:	00a69693          	slli	a3,a3,0xa
    d96c:	00a71713          	slli	a4,a4,0xa
    d970:	02b6c6b3          	div	a3,a3,a1
    d974:	02b74733          	div	a4,a4,a1
    d978:	00d80833          	add	a6,a6,a3
    d97c:	00e50533          	add	a0,a0,a4
    d980:	01062223          	sw	a6,4(a2)
    d984:	00a62423          	sw	a0,8(a2)
    d988:	f41ff06f          	j	d8c8 <game_tick+0xa3c>
    d98c:	40067793          	andi	a5,a2,1024
    d990:	06078863          	beqz	a5,da00 <game_tick+0xb74>
    d994:	08c12083          	lw	ra,140(sp)
    d998:	00100793          	li	a5,1
    d99c:	00f52023          	sw	a5,0(a0)
    d9a0:	08412483          	lw	s1,132(sp)
    d9a4:	09010113          	addi	sp,sp,144
    d9a8:	00008067          	ret
    d9ac:	000237b7          	lui	a5,0x23
    d9b0:	00f507b3          	add	a5,a0,a5
    d9b4:	5b87a583          	lw	a1,1464(a5) # 235b8 <background_pixels+0xb6f8>
    d9b8:	00058a63          	beqz	a1,d9cc <game_tick+0xb40>
    d9bc:	5cc7a703          	lw	a4,1484(a5)
    d9c0:	00070663          	beqz	a4,d9cc <game_tick+0xb40>
    d9c4:	5d07a503          	lw	a0,1488(a5)
    d9c8:	000700e7          	jalr	a4
    d9cc:	000237b7          	lui	a5,0x23
    d9d0:	00f487b3          	add	a5,s1,a5
    d9d4:	00c105b7          	lui	a1,0xc10
    d9d8:	5a07ac23          	sw	zero,1464(a5) # 235b8 <background_pixels+0xb6f8>
    d9dc:	5a07ae23          	sw	zero,1468(a5)
    d9e0:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    d9e4:	00048513          	mv	a0,s1
    d9e8:	bf5fe0ef          	jal	c5dc <begin_run.constprop.0>
    d9ec:	0004a023          	sw	zero,0(s1)
    d9f0:	08c12083          	lw	ra,140(sp)
    d9f4:	08412483          	lw	s1,132(sp)
    d9f8:	09010113          	addi	sp,sp,144
    d9fc:	00008067          	ret
    da00:	01067613          	andi	a2,a2,16
    da04:	fe0606e3          	beqz	a2,d9f0 <game_tick+0xb64>
    da08:	08c12083          	lw	ra,140(sp)
    da0c:	00048513          	mv	a0,s1
    da10:	08412483          	lw	s1,132(sp)
    da14:	00c105b7          	lui	a1,0xc10
    da18:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    da1c:	09010113          	addi	sp,sp,144
    da20:	bbdfe06f          	j	c5dc <begin_run.constprop.0>
    da24:	ffd78793          	addi	a5,a5,-3
    da28:	00f037b3          	snez	a5,a5
    da2c:	00278793          	addi	a5,a5,2
    da30:	00f52023          	sw	a5,0(a0)
    da34:	fbdff06f          	j	d9f0 <game_tick+0xb64>
    da38:	01067613          	andi	a2,a2,16
    da3c:	fa060ae3          	beqz	a2,d9f0 <game_tick+0xb64>
    da40:	0884a783          	lw	a5,136(s1)
    da44:	fc0782e3          	beqz	a5,da08 <game_tick+0xb7c>
    da48:	08c4a603          	lw	a2,140(s1)
    da4c:	00300793          	li	a5,3
    da50:	2ec7c463          	blt	a5,a2,dd38 <game_tick+0xeac>
    da54:	00c04463          	bgtz	a2,da5c <game_tick+0xbd0>
    da58:	00100613          	li	a2,1
    da5c:	00c105b7          	lui	a1,0xc10
    da60:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    da64:	00048513          	mv	a0,s1
    da68:	08c12083          	lw	ra,140(sp)
    da6c:	08412483          	lw	s1,132(sp)
    da70:	09010113          	addi	sp,sp,144
    da74:	f20fe06f          	j	c194 <begin_run>
    da78:	09067793          	andi	a5,a2,144
    da7c:	00078463          	beqz	a5,da84 <game_tick+0xbf8>
    da80:	1dc0106f          	j	ec5c <game_tick+0x1dd0>
    da84:	10067793          	andi	a5,a2,256
    da88:	48079e63          	bnez	a5,df24 <game_tick+0x1098>
    da8c:	20067793          	andi	a5,a2,512
    da90:	00078463          	beqz	a5,da98 <game_tick+0xc0c>
    da94:	2980106f          	j	ed2c <game_tick+0x1ea0>
    da98:	02067613          	andi	a2,a2,32
    da9c:	f4060ae3          	beqz	a2,d9f0 <game_tick+0xb64>
    daa0:	000237b7          	lui	a5,0x23
    daa4:	00f507b3          	add	a5,a0,a5
    daa8:	5b87a583          	lw	a1,1464(a5) # 235b8 <background_pixels+0xb6f8>
    daac:	09212023          	sw	s2,128(sp)
    dab0:	08812423          	sw	s0,136(sp)
    dab4:	07312e23          	sw	s3,124(sp)
    dab8:	07412c23          	sw	s4,120(sp)
    dabc:	07512a23          	sw	s5,116(sp)
    dac0:	07612823          	sw	s6,112(sp)
    dac4:	07712623          	sw	s7,108(sp)
    dac8:	07812423          	sw	s8,104(sp)
    dacc:	07912223          	sw	s9,100(sp)
    dad0:	07a12023          	sw	s10,96(sp)
    dad4:	05b12e23          	sw	s11,92(sp)
    dad8:	5cc7a903          	lw	s2,1484(a5)
    dadc:	00058c63          	beqz	a1,daf4 <game_tick+0xc68>
    dae0:	00090a63          	beqz	s2,daf4 <game_tick+0xc68>
    dae4:	5d07a503          	lw	a0,1488(a5)
    dae8:	00078413          	mv	s0,a5
    daec:	000900e7          	jalr	s2
    daf0:	5cc42903          	lw	s2,1484(s0)
    daf4:	00023437          	lui	s0,0x23
    daf8:	00848433          	add	s0,s1,s0
    dafc:	00025637          	lui	a2,0x25
    db00:	5c842a03          	lw	s4,1480(s0) # 235c8 <background_pixels+0xb708>
    db04:	5d042983          	lw	s3,1488(s0)
    db08:	1d460613          	addi	a2,a2,468 # 251d4 <background_pixels+0xd314>
    db0c:	00000593          	li	a1,0
    db10:	00048513          	mv	a0,s1
    db14:	df0f30ef          	jal	1104 <memset>
    db18:	00010737          	lui	a4,0x10
    db1c:	5d442423          	sw	s4,1480(s0)
    db20:	5d242623          	sw	s2,1484(s0)
    db24:	5d342823          	sw	s3,1488(s0)
    db28:	c0070713          	addi	a4,a4,-1024 # fc00 <demo_squares_render+0x3c>
    db2c:	00e4aa23          	sw	a4,20(s1)
    db30:	23000713          	li	a4,560
    db34:	00c106b7          	lui	a3,0xc10
    db38:	04e4a023          	sw	a4,64(s1)
    db3c:	01100713          	li	a4,17
    db40:	00100793          	li	a5,1
    db44:	fee68693          	addi	a3,a3,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    db48:	02e4ac23          	sw	a4,56(s1)
    db4c:	03e00713          	li	a4,62
    db50:	00200613          	li	a2,2
    db54:	06400593          	li	a1,100
    db58:	00001ab7          	lui	s5,0x1
    db5c:	00d4a223          	sw	a3,4(s1)
    db60:	04f4a223          	sw	a5,68(s1)
    db64:	000146b7          	lui	a3,0x14
    db68:	04e4a423          	sw	a4,72(s1)
    db6c:	04f4aa23          	sw	a5,84(s1)
    db70:	00600713          	li	a4,6
    db74:	02f4a223          	sw	a5,36(s1)
    db78:	02f4a623          	sw	a5,44(s1)
    db7c:	09600793          	li	a5,150
    db80:	00c4a023          	sw	a2,0(s1)
    db84:	00d4a823          	sw	a3,16(s1)
    db88:	00b4ac23          	sw	a1,24(s1)
    db8c:	00b4ae23          	sw	a1,28(s1)
    db90:	02c4ae23          	sw	a2,60(s1)
    db94:	04e4a823          	sw	a4,80(s1)
    db98:	06f4a423          	sw	a5,104(s1)
    db9c:	00500b13          	li	s6,5
    dba0:	00000a13          	li	s4,0
    dba4:	0b848813          	addi	a6,s1,184
    dba8:	e10a8a93          	addi	s5,s5,-496 # e10 <CUSTOM2+0xdb5>
    dbac:	0c000413          	li	s0,192
    dbb0:	24400d93          	li	s11,580
    dbb4:	15800d13          	li	s10,344
    dbb8:	00100b93          	li	s7,1
    dbbc:	02c00c93          	li	s9,44
    dbc0:	06900c13          	li	s8,105
    dbc4:	035a4a33          	div	s4,s4,s5
    dbc8:	00080713          	mv	a4,a6
    dbcc:	00000793          	li	a5,0
    dbd0:	0100006f          	j	dbe0 <game_tick+0xd54>
    dbd4:	00178793          	addi	a5,a5,1
    dbd8:	02c70713          	addi	a4,a4,44
    dbdc:	10878a63          	beq	a5,s0,dcf0 <game_tick+0xe64>
    dbe0:	00072683          	lw	a3,0(a4)
    dbe4:	fe0698e3          	bnez	a3,dbd4 <game_tick+0xd48>
    dbe8:	0044a703          	lw	a4,4(s1)
    dbec:	00d71693          	slli	a3,a4,0xd
    dbf0:	00d74733          	xor	a4,a4,a3
    dbf4:	01175693          	srli	a3,a4,0x11
    dbf8:	00d74633          	xor	a2,a4,a3
    dbfc:	00561693          	slli	a3,a2,0x5
    dc00:	00d646b3          	xor	a3,a2,a3
    dc04:	00d69713          	slli	a4,a3,0xd
    dc08:	00d74733          	xor	a4,a4,a3
    dc0c:	01175693          	srli	a3,a4,0x11
    dc10:	00d74733          	xor	a4,a4,a3
    dc14:	00571693          	slli	a3,a4,0x5
    dc18:	00d74733          	xor	a4,a4,a3
    dc1c:	00d71693          	slli	a3,a4,0xd
    dc20:	00e6c6b3          	xor	a3,a3,a4
    dc24:	0116d593          	srli	a1,a3,0x11
    dc28:	00b6c6b3          	xor	a3,a3,a1
    dc2c:	00569593          	slli	a1,a3,0x5
    dc30:	00b6c6b3          	xor	a3,a3,a1
    dc34:	03b77933          	remu	s2,a4,s11
    dc38:	00d4a223          	sw	a3,4(s1)
    dc3c:	00367713          	andi	a4,a2,3
    dc40:	03a6f9b3          	remu	s3,a3,s10
    dc44:	01e90913          	addi	s2,s2,30
    dc48:	05098993          	addi	s3,s3,80
    dc4c:	01771463          	bne	a4,s7,dc54 <game_tick+0xdc8>
    dc50:	5a00106f          	j	f1f0 <game_tick+0x2364>
    dc54:	00200693          	li	a3,2
    dc58:	00d71463          	bne	a4,a3,dc60 <game_tick+0xdd4>
    dc5c:	5840106f          	j	f1e0 <game_tick+0x2354>
    dc60:	00071463          	bnez	a4,dc68 <game_tick+0xddc>
    dc64:	56c0106f          	j	f1d0 <game_tick+0x2344>
    dc68:	00891913          	slli	s2,s2,0x8
    dc6c:	0001b9b7          	lui	s3,0x1b
    dc70:	039787b3          	mul	a5,a5,s9
    dc74:	02c00613          	li	a2,44
    dc78:	00000593          	li	a1,0
    dc7c:	01012623          	sw	a6,12(sp)
    dc80:	0b878513          	addi	a0,a5,184
    dc84:	00a48533          	add	a0,s1,a0
    dc88:	00f12423          	sw	a5,8(sp)
    dc8c:	c78f30ef          	jal	1104 <memset>
    dc90:	00812783          	lw	a5,8(sp)
    dc94:	001a1693          	slli	a3,s4,0x1
    dc98:	00468693          	addi	a3,a3,4 # 14004 <__global_pointer$+0x3fc>
    dc9c:	00f487b3          	add	a5,s1,a5
    dca0:	0b77ac23          	sw	s7,184(a5)
    dca4:	0b27ae23          	sw	s2,188(a5)
    dca8:	0d37a023          	sw	s3,192(a5)
    dcac:	0884a703          	lw	a4,136(s1)
    dcb0:	00c12803          	lw	a6,12(sp)
    dcb4:	00173713          	seqz	a4,a4
    dcb8:	40e00733          	neg	a4,a4
    dcbc:	02577713          	andi	a4,a4,37
    dcc0:	00870713          	addi	a4,a4,8
    dcc4:	0ce7a823          	sw	a4,208(a5)
    dcc8:	0604a703          	lw	a4,96(s1)
    dccc:	00170713          	addi	a4,a4,1
    dcd0:	06e4a023          	sw	a4,96(s1)
    dcd4:	0ce7ac23          	sw	a4,216(a5)
    dcd8:	0cd7a223          	sw	a3,196(a5)
    dcdc:	0cd7a423          	sw	a3,200(a5)
    dce0:	0d87ae23          	sw	s8,220(a5)
    dce4:	0644a783          	lw	a5,100(s1)
    dce8:	00178793          	addi	a5,a5,1
    dcec:	06f4a223          	sw	a5,100(s1)
    dcf0:	fffb0b13          	addi	s6,s6,-1
    dcf4:	000b1463          	bnez	s6,dcfc <game_tick+0xe70>
    dcf8:	1ec0106f          	j	eee4 <game_tick+0x2058>
    dcfc:	0084aa03          	lw	s4,8(s1)
    dd00:	ec5ff06f          	j	dbc4 <game_tick+0xd38>
    dd04:	08067793          	andi	a5,a2,128
    dd08:	00000593          	li	a1,0
    dd0c:	00079e63          	bnez	a5,dd28 <game_tick+0xe9c>
    dd10:	10067793          	andi	a5,a2,256
    dd14:	00078463          	beqz	a5,dd1c <game_tick+0xe90>
    dd18:	00c0106f          	j	ed24 <game_tick+0x1e98>
    dd1c:	20067613          	andi	a2,a2,512
    dd20:	00200593          	li	a1,2
    dd24:	cc0606e3          	beqz	a2,d9f0 <game_tick+0xb64>
    dd28:	08c12083          	lw	ra,140(sp)
    dd2c:	08412483          	lw	s1,132(sp)
    dd30:	09010113          	addi	sp,sp,144
    dd34:	b04fe06f          	j	c038 <game_choose_upgrade.part.0>
    dd38:	00300613          	li	a2,3
    dd3c:	d21ff06f          	j	da5c <game_tick+0xbd0>
    dd40:	000267b7          	lui	a5,0x26
    dd44:	40078793          	addi	a5,a5,1024 # 26400 <background_pixels+0xe540>
    dd48:	00b7d463          	bge	a5,a1,dd50 <game_tick+0xec4>
    dd4c:	00078593          	mv	a1,a5
    dd50:	000027b7          	lui	a5,0x2
    dd54:	c0078793          	addi	a5,a5,-1024 # 1c00 <main+0x7f0>
    dd58:	00f5d463          	bge	a1,a5,dd60 <game_tick+0xed4>
    dd5c:	00078593          	mv	a1,a5
    dd60:	0001a7b7          	lui	a5,0x1a
    dd64:	00b4a823          	sw	a1,16(s1)
    dd68:	00a60633          	add	a2,a2,a0
    dd6c:	60078793          	addi	a5,a5,1536 # 1a600 <background_pixels+0x2740>
    dd70:	00c7d463          	bge	a5,a2,dd78 <game_tick+0xeec>
    dd74:	00078613          	mv	a2,a5
    dd78:	000057b7          	lui	a5,0x5
    dd7c:	20078793          	addi	a5,a5,512 # 5200 <__stack_size+0x1200>
    dd80:	00f65463          	bge	a2,a5,dd88 <game_tick+0xefc>
    dd84:	00078613          	mv	a2,a5
    dd88:	00c4aa23          	sw	a2,20(s1)
    dd8c:	adcff06f          	j	d068 <game_tick+0x1dc>
    dd90:	088c2703          	lw	a4,136(s8)
    dd94:	00542223          	sw	t0,4(s0)
    dd98:	00b42423          	sw	a1,8(s0)
    dd9c:	ce071663          	bnez	a4,d288 <game_tick+0x3fc>
    dda0:	008c2783          	lw	a5,8(s8)
    dda4:	fff48693          	addi	a3,s1,-1
    dda8:	02c40913          	addi	s2,s0,44
    ddac:	00f686b3          	add	a3,a3,a5
    ddb0:	00300793          	li	a5,3
    ddb4:	02f6e6b3          	rem	a3,a3,a5
    ddb8:	00028793          	mv	a5,t0
    ddbc:	cc069a63          	bnez	a3,d290 <game_tick+0x404>
    ddc0:	0c000793          	li	a5,192
    ddc4:	3ef488e3          	beq	s1,a5,e9b4 <game_tick+0x1b28>
    ddc8:	02c40913          	addi	s2,s0,44
    ddcc:	00090613          	mv	a2,s2
    ddd0:	00028793          	mv	a5,t0
    ddd4:	00058f93          	mv	t6,a1
    ddd8:	00300993          	li	s3,3
    dddc:	00062683          	lw	a3,0(a2)
    dde0:	12068a63          	beqz	a3,df14 <game_tick+0x1088>
    dde4:	01862303          	lw	t1,24(a2)
    dde8:	12031663          	bnez	t1,df14 <game_tick+0x1088>
    ddec:	00462583          	lw	a1,4(a2)
    ddf0:	00862683          	lw	a3,8(a2)
    ddf4:	ffff0393          	addi	t2,t5,-1
    ddf8:	40b785b3          	sub	a1,a5,a1
    ddfc:	40df86b3          	sub	a3,t6,a3
    de00:	41f6d893          	srai	a7,a3,0x1f
    de04:	41f5d813          	srai	a6,a1,0x1f
    de08:	0ff87813          	zext.b	a6,a6
    de0c:	0ff8f893          	zext.b	a7,a7
    de10:	00d888b3          	add	a7,a7,a3
    de14:	00b80833          	add	a6,a6,a1
    de18:	40885813          	srai	a6,a6,0x8
    de1c:	4088d893          	srai	a7,a7,0x8
    de20:	03080533          	mul	a0,a6,a6
    de24:	00078293          	mv	t0,a5
    de28:	000f8e93          	mv	t4,t6
    de2c:	00a00a13          	li	s4,10
    de30:	031886b3          	mul	a3,a7,a7
    de34:	00d50533          	add	a0,a0,a3
    de38:	0079ea63          	bltu	s3,t2,de4c <game_tick+0xfc0>
    de3c:	00812683          	lw	a3,8(sp)
    de40:	00239393          	slli	t2,t2,0x2
    de44:	007683b3          	add	t2,a3,t2
    de48:	0003aa03          	lw	s4,0(t2)
    de4c:	01462583          	lw	a1,20(a2)
    de50:	00a00693          	li	a3,10
    de54:	fff58593          	addi	a1,a1,-1
    de58:	00b9ea63          	bltu	s3,a1,de6c <game_tick+0xfe0>
    de5c:	00812683          	lw	a3,8(sp)
    de60:	00259593          	slli	a1,a1,0x2
    de64:	00b685b3          	add	a1,a3,a1
    de68:	0005a683          	lw	a3,0(a1)
    de6c:	014686b3          	add	a3,a3,s4
    de70:	ffc68693          	addi	a3,a3,-4
    de74:	02d686b3          	mul	a3,a3,a3
    de78:	08d55e63          	bge	a0,a3,df14 <game_tick+0x1088>
    de7c:	011866b3          	or	a3,a6,a7
    de80:	00069663          	bnez	a3,de8c <game_tick+0x1000>
    de84:	00100513          	li	a0,1
    de88:	00100813          	li	a6,1
    de8c:	400006b7          	lui	a3,0x40000
    de90:	0026d693          	srli	a3,a3,0x2
    de94:	fed56ee3          	bltu	a0,a3,de90 <game_tick+0x1004>
    de98:	00000593          	li	a1,0
    de9c:	04068663          	beqz	a3,dee8 <game_tick+0x105c>
    dea0:	00b68333          	add	t1,a3,a1
    dea4:	0015d593          	srli	a1,a1,0x1
    dea8:	00656663          	bltu	a0,t1,deb4 <game_tick+0x1028>
    deac:	40650533          	sub	a0,a0,t1
    deb0:	00d585b3          	add	a1,a1,a3
    deb4:	0026d693          	srli	a3,a3,0x2
    deb8:	fe0694e3          	bnez	a3,dea0 <game_tick+0x1014>
    debc:	1e0580e3          	beqz	a1,e89c <game_tick+0x1a10>
    dec0:	00281313          	slli	t1,a6,0x2
    dec4:	00289693          	slli	a3,a7,0x2
    dec8:	01030333          	add	t1,t1,a6
    decc:	011686b3          	add	a3,a3,a7
    ded0:	00431313          	slli	t1,t1,0x4
    ded4:	00469693          	slli	a3,a3,0x4
    ded8:	02b34333          	div	t1,t1,a1
    dedc:	02b6c5b3          	div	a1,a3,a1
    dee0:	006782b3          	add	t0,a5,t1
    dee4:	00bf8eb3          	add	t4,t6,a1
    dee8:	00542223          	sw	t0,4(s0)
    deec:	01d42423          	sw	t4,8(s0)
    def0:	00462683          	lw	a3,4(a2)
    def4:	00862783          	lw	a5,8(a2)
    def8:	406686b3          	sub	a3,a3,t1
    defc:	40b787b3          	sub	a5,a5,a1
    df00:	00d62223          	sw	a3,4(a2)
    df04:	00f62423          	sw	a5,8(a2)
    df08:	00442783          	lw	a5,4(s0)
    df0c:	00842f83          	lw	t6,8(s0)
    df10:	01442f03          	lw	t5,20(s0)
    df14:	02c60613          	addi	a2,a2,44
    df18:	ecce12e3          	bne	t3,a2,dddc <game_tick+0xf50>
    df1c:	000f8593          	mv	a1,t6
    df20:	b70ff06f          	j	d290 <game_tick+0x404>
    df24:	00c105b7          	lui	a1,0xc10
    df28:	00200613          	li	a2,2
    df2c:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    df30:	b39ff06f          	j	da68 <game_tick+0xbdc>
    df34:	00000e13          	li	t3,0
    df38:	70000313          	li	t1,1792
    df3c:	e58ff06f          	j	d594 <game_tick+0x708>
    df40:	00092023          	sw	zero,0(s2)
    df44:	8f1ff06f          	j	d834 <game_tick+0x9a8>
    df48:	fffd8313          	addi	t1,s11,-1
    df4c:	00642c23          	sw	t1,24(s0)
    df50:	02c40913          	addi	s2,s0,44
    df54:	cc8ff06f          	j	d41c <game_tick+0x590>
    df58:	00004737          	lui	a4,0x4
    df5c:	20370713          	addi	a4,a4,515 # 4203 <__stack_size+0x203>
    df60:	56d75663          	bge	a4,a3,e4cc <game_tick+0x1640>
    df64:	0000b737          	lui	a4,0xb
    df68:	c4470713          	addi	a4,a4,-956 # ac44 <handle_char+0x94>
    df6c:	4ad75e63          	bge	a4,a3,e428 <game_tick+0x159c>
    df70:	40000737          	lui	a4,0x40000
    df74:	26e6dee3          	bge	a3,a4,e9f0 <game_tick+0x1b64>
    df78:	00275713          	srli	a4,a4,0x2
    df7c:	fee6eee3          	bltu	a3,a4,df78 <game_tick+0x10ec>
    df80:	00068393          	mv	t2,a3
    df84:	00000593          	li	a1,0
    df88:	00b70533          	add	a0,a4,a1
    df8c:	0015d593          	srli	a1,a1,0x1
    df90:	00a3e663          	bltu	t2,a0,df9c <game_tick+0x1110>
    df94:	40a383b3          	sub	t2,t2,a0
    df98:	00e585b3          	add	a1,a1,a4
    df9c:	00275713          	srli	a4,a4,0x2
    dfa0:	fe0714e3          	bnez	a4,df88 <game_tick+0x10fc>
    dfa4:	12b054e3          	blez	a1,e8cc <game_tick+0x1a40>
    dfa8:	00c12703          	lw	a4,12(sp)
    dfac:	02e80ab3          	mul	s5,a6,a4
    dfb0:	02e88a33          	mul	s4,a7,a4
    dfb4:	02bacab3          	div	s5,s5,a1
    dfb8:	02ba4a33          	div	s4,s4,a1
    dfbc:	02442703          	lw	a4,36(s0)
    dfc0:	008c2583          	lw	a1,8(s8)
    dfc4:	fff70713          	addi	a4,a4,-1 # 3fffffff <__freertos_irq_stack_top+0x3ff17e5f>
    dfc8:	02e42223          	sw	a4,36(s0)
    dfcc:	20e04263          	bgtz	a4,e1d0 <game_tick+0x1344>
    dfd0:	00001b37          	lui	s6,0x1
    dfd4:	e10b0b13          	addi	s6,s6,-496 # e10 <CUSTOM2+0xdb5>
    dfd8:	3e800713          	li	a4,1000
    dfdc:	0365cb33          	div	s6,a1,s6
    dfe0:	002b1793          	slli	a5,s6,0x2
    dfe4:	016789b3          	add	s3,a5,s6
    dfe8:	00299993          	slli	s3,s3,0x2
    dfec:	25898993          	addi	s3,s3,600 # 1b258 <background_pixels+0x3398>
    dff0:	01375463          	bge	a4,s3,dff8 <game_tick+0x116c>
    dff4:	3e800993          	li	s3,1000
    dff8:	25800713          	li	a4,600
    dffc:	00e9d463          	bge	s3,a4,e004 <game_tick+0x1178>
    e000:	25800993          	li	s3,600
    e004:	006b0713          	addi	a4,s6,6
    e008:	00171b93          	slli	s7,a4,0x1
    e00c:	02800f13          	li	t5,40
    e010:	017f5463          	bge	t5,s7,e018 <game_tick+0x118c>
    e014:	02800b93          	li	s7,40
    e018:	00c00f13          	li	t5,12
    e01c:	01ebd463          	bge	s7,t5,e024 <game_tick+0x1198>
    e020:	00c00b93          	li	s7,12
    e024:	02842f03          	lw	t5,40(s0)
    e028:	00300f93          	li	t6,3
    e02c:	001f0f13          	addi	t5,t5,1
    e030:	03ff6fb3          	rem	t6,t5,t6
    e034:	03e42423          	sw	t5,40(s0)
    e038:	560f9263          	bnez	t6,e59c <game_tick+0x1710>
    e03c:	00013737          	lui	a4,0x13
    e040:	0e470c93          	addi	s9,a4,228 # 130e4 <ring.2>
    e044:	060c8913          	addi	s2,s9,96
    e048:	00078d13          	mv	s10,a5
    e04c:	000e0d93          	mv	s11,t3
    e050:	004ca603          	lw	a2,4(s9)
    e054:	000ca803          	lw	a6,0(s9)
    e058:	00000793          	li	a5,0
    e05c:	02c98633          	mul	a2,s3,a2
    e060:	000b8713          	mv	a4,s7
    e064:	00040593          	mv	a1,s0
    e068:	000c0513          	mv	a0,s8
    e06c:	008c8c93          	addi	s9,s9,8
    e070:	03098833          	mul	a6,s3,a6
    e074:	41f65693          	srai	a3,a2,0x1f
    e078:	0ff6f693          	zext.b	a3,a3
    e07c:	00c686b3          	add	a3,a3,a2
    e080:	4086d693          	srai	a3,a3,0x8
    e084:	41f85613          	srai	a2,a6,0x1f
    e088:	0ff67613          	zext.b	a2,a2
    e08c:	01060633          	add	a2,a2,a6
    e090:	40865613          	srai	a2,a2,0x8
    e094:	be1fd0ef          	jal	bc74 <hostile_shot>
    e098:	fb991ce3          	bne	s2,s9,e050 <game_tick+0x11c4>
    e09c:	000d0793          	mv	a5,s10
    e0a0:	000d8e13          	mv	t3,s11
    e0a4:	016787b3          	add	a5,a5,s6
    e0a8:	40f007b3          	neg	a5,a5
    e0ac:	06e78793          	addi	a5,a5,110
    e0b0:	06e00713          	li	a4,110
    e0b4:	00f75463          	bge	a4,a5,e0bc <game_tick+0x1230>
    e0b8:	06e00793          	li	a5,110
    e0bc:	03200713          	li	a4,50
    e0c0:	00e7d463          	bge	a5,a4,e0c8 <game_tick+0x123c>
    e0c4:	03200793          	li	a5,50
    e0c8:	00442283          	lw	t0,4(s0)
    e0cc:	00842583          	lw	a1,8(s0)
    e0d0:	02f42223          	sw	a5,36(s0)
    e0d4:	088c2703          	lw	a4,136(s8)
    e0d8:	01442f03          	lw	t5,20(s0)
    e0dc:	005a82b3          	add	t0,s5,t0
    e0e0:	00ba05b3          	add	a1,s4,a1
    e0e4:	998ff06f          	j	d27c <game_tick+0x3f0>
    e0e8:	00002737          	lui	a4,0x2
    e0ec:	70f70713          	addi	a4,a4,1807 # 270f <main+0x12ff>
    e0f0:	42d75a63          	bge	a4,a3,e524 <game_tick+0x1698>
    e0f4:	00007737          	lui	a4,0x7
    e0f8:	0e470713          	addi	a4,a4,228 # 70e4 <render_game+0x1390>
    e0fc:	36d75e63          	bge	a4,a3,e478 <game_tick+0x15ec>
    e100:	40000737          	lui	a4,0x40000
    e104:	0ee6dae3          	bge	a3,a4,e9f8 <game_tick+0x1b6c>
    e108:	00275713          	srli	a4,a4,0x2
    e10c:	fee6eee3          	bltu	a3,a4,e108 <game_tick+0x127c>
    e110:	00068293          	mv	t0,a3
    e114:	00000393          	li	t2,0
    e118:	007705b3          	add	a1,a4,t2
    e11c:	0013d393          	srli	t2,t2,0x1
    e120:	00b2e663          	bltu	t0,a1,e12c <game_tick+0x12a0>
    e124:	40b282b3          	sub	t0,t0,a1
    e128:	00e383b3          	add	t2,t2,a4
    e12c:	00275713          	srli	a4,a4,0x2
    e130:	fe0714e3          	bnez	a4,e118 <game_tick+0x128c>
    e134:	7a705263          	blez	t2,e8d8 <game_tick+0x1a4c>
    e138:	02412703          	lw	a4,36(sp)
    e13c:	02e802b3          	mul	t0,a6,a4
    e140:	02e885b3          	mul	a1,a7,a4
    e144:	0272c2b3          	div	t0,t0,t2
    e148:	0275ccb3          	div	s9,a1,t2
    e14c:	008c2383          	lw	t2,8(s8)
    e150:	088c2703          	lw	a4,136(s8)
    e154:	00001937          	lui	s2,0x1
    e158:	e1090913          	addi	s2,s2,-496 # e10 <CUSTOM2+0xdb5>
    e15c:	0323c3b3          	div	t2,t2,s2
    e160:	12c00913          	li	s2,300
    e164:	02071663          	bnez	a4,e190 <game_tick+0x1304>
    e168:	00139913          	slli	s2,t2,0x1
    e16c:	00790933          	add	s2,s2,t2
    e170:	00291913          	slli	s2,s2,0x2
    e174:	1a490913          	addi	s2,s2,420
    e178:	28000993          	li	s3,640
    e17c:	0129d463          	bge	s3,s2,e184 <game_tick+0x12f8>
    e180:	28000913          	li	s2,640
    e184:	1a400993          	li	s3,420
    e188:	01395463          	bge	s2,s3,e190 <game_tick+0x1304>
    e18c:	1a400913          	li	s2,420
    e190:	02442983          	lw	s3,36(s0)
    e194:	fff98993          	addi	s3,s3,-1
    e198:	03342223          	sw	s3,36(s0)
    e19c:	05305263          	blez	s3,e1e0 <game_tick+0x1354>
    e1a0:	00f282b3          	add	t0,t0,a5
    e1a4:	01fc85b3          	add	a1,s9,t6
    e1a8:	8d4ff06f          	j	d27c <game_tick+0x3f0>
    e1ac:	01412503          	lw	a0,20(sp)
    e1b0:	f4d00713          	li	a4,-179
    e1b4:	02812383          	lw	t2,40(sp)
    e1b8:	86e55863          	bge	a0,a4,d228 <game_tick+0x39c>
    e1bc:	40000737          	lui	a4,0x40000
    e1c0:	07300393          	li	t2,115
    e1c4:	86e6c663          	blt	a3,a4,d230 <game_tick+0x3a4>
    e1c8:	40000737          	lui	a4,0x40000
    e1cc:	870ff06f          	j	d23c <game_tick+0x3b0>
    e1d0:	088c2703          	lw	a4,136(s8)
    e1d4:	015782b3          	add	t0,a5,s5
    e1d8:	014f85b3          	add	a1,t6,s4
    e1dc:	8a0ff06f          	j	d27c <game_tick+0x3f0>
    e1e0:	400009b7          	lui	s3,0x40000
    e1e4:	7d36d463          	bge	a3,s3,e9ac <game_tick+0x1b20>
    e1e8:	0029d993          	srli	s3,s3,0x2
    e1ec:	ff36eee3          	bltu	a3,s3,e1e8 <game_tick+0x135c>
    e1f0:	20098a63          	beqz	s3,e404 <game_tick+0x1578>
    e1f4:	00000693          	li	a3,0
    e1f8:	00d985b3          	add	a1,s3,a3
    e1fc:	0016d693          	srli	a3,a3,0x1
    e200:	00b66663          	bltu	a2,a1,e20c <game_tick+0x1380>
    e204:	40b60633          	sub	a2,a2,a1
    e208:	013686b3          	add	a3,a3,s3
    e20c:	0029d993          	srli	s3,s3,0x2
    e210:	fe0994e3          	bnez	s3,e1f8 <game_tick+0x136c>
    e214:	1ed05863          	blez	a3,e404 <game_tick+0x1578>
    e218:	03280833          	mul	a6,a6,s2
    e21c:	02d84ab3          	div	s5,a6,a3
    e220:	032888b3          	mul	a7,a7,s2
    e224:	02d8cb33          	div	s6,a7,a3
    e228:	000a9463          	bnez	s5,e230 <game_tick+0x13a4>
    e22c:	1c0b0e63          	beqz	s6,e408 <game_tick+0x157c>
    e230:	02842683          	lw	a3,40(s0)
    e234:	00168693          	addi	a3,a3,1 # 40000001 <__freertos_irq_stack_top+0x3ff17e61>
    e238:	02d42423          	sw	a3,40(s0)
    e23c:	1e070063          	beqz	a4,e41c <game_tick+0x1590>
    e240:	08cc2603          	lw	a2,140(s8)
    e244:	00200693          	li	a3,2
    e248:	fff60913          	addi	s2,a2,-1
    e24c:	0126f463          	bgeu	a3,s2,e254 <game_tick+0x13c8>
    e250:	0180106f          	j	f268 <game_tick+0x23dc>
    e254:	03412583          	lw	a1,52(sp)
    e258:	00361693          	slli	a3,a2,0x3
    e25c:	40c68933          	sub	s2,a3,a2
    e260:	00291913          	slli	s2,s2,0x2
    e264:	01258933          	add	s2,a1,s2
    e268:	01092a03          	lw	s4,16(s2)
    e26c:	4b405e63          	blez	s4,e728 <game_tick+0x189c>
    e270:	01fa5793          	srli	a5,s4,0x1f
    e274:	014787b3          	add	a5,a5,s4
    e278:	4017d793          	srai	a5,a5,0x1
    e27c:	40f007b3          	neg	a5,a5
    e280:	003b1993          	slli	s3,s6,0x3
    e284:	003a9913          	slli	s2,s5,0x3
    e288:	016989b3          	add	s3,s3,s6
    e28c:	01590933          	add	s2,s2,s5
    e290:	00738613          	addi	a2,t2,7
    e294:	00912823          	sw	s1,16(sp)
    e298:	01912c23          	sw	s9,24(sp)
    e29c:	02fb06b3          	mul	a3,s6,a5
    e2a0:	00712e23          	sw	t2,28(sp)
    e2a4:	03c12023          	sw	t3,32(sp)
    e2a8:	00040493          	mv	s1,s0
    e2ac:	000c0c93          	mv	s9,s8
    e2b0:	000d8413          	mv	s0,s11
    e2b4:	00299993          	slli	s3,s3,0x2
    e2b8:	00291913          	slli	s2,s2,0x2
    e2bc:	00028d93          	mv	s11,t0
    e2c0:	02fa87b3          	mul	a5,s5,a5
    e2c4:	00369f93          	slli	t6,a3,0x3
    e2c8:	00df8fb3          	add	t6,t6,a3
    e2cc:	002f9b93          	slli	s7,t6,0x2
    e2d0:	00379f13          	slli	t5,a5,0x3
    e2d4:	00ff0f33          	add	t5,t5,a5
    e2d8:	002f1d13          	slli	s10,t5,0x2
    e2dc:	000d0c13          	mv	s8,s10
    e2e0:	00060d13          	mv	s10,a2
    e2e4:	06071863          	bnez	a4,e354 <game_tick+0x14c8>
    e2e8:	01000793          	li	a5,16
    e2ec:	000d0713          	mv	a4,s10
    e2f0:	01a7d463          	bge	a5,s10,e2f8 <game_tick+0x146c>
    e2f4:	01000713          	li	a4,16
    e2f8:	00700793          	li	a5,7
    e2fc:	00f75463          	bge	a4,a5,e304 <game_tick+0x1478>
    e300:	00700713          	li	a4,7
    e304:	41fc5693          	srai	a3,s8,0x1f
    e308:	41fbd613          	srai	a2,s7,0x1f
    e30c:	0ff6f693          	zext.b	a3,a3
    e310:	0ff67613          	zext.b	a2,a2
    e314:	018686b3          	add	a3,a3,s8
    e318:	01760633          	add	a2,a2,s7
    e31c:	4086d693          	srai	a3,a3,0x8
    e320:	40865613          	srai	a2,a2,0x8
    e324:	00100793          	li	a5,1
    e328:	016686b3          	add	a3,a3,s6
    e32c:	40ca8633          	sub	a2,s5,a2
    e330:	00048593          	mv	a1,s1
    e334:	000c8513          	mv	a0,s9
    e338:	00140413          	addi	s0,s0,1
    e33c:	939fd0ef          	jal	bc74 <hostile_shot>
    e340:	013b8bb3          	add	s7,s7,s3
    e344:	012c0c33          	add	s8,s8,s2
    e348:	068a0463          	beq	s4,s0,e3b0 <game_tick+0x1524>
    e34c:	088ca703          	lw	a4,136(s9)
    e350:	f8070ce3          	beqz	a4,e2e8 <game_tick+0x145c>
    e354:	0284a783          	lw	a5,40(s1)
    e358:	08cca703          	lw	a4,140(s9)
    e35c:	41fc5693          	srai	a3,s8,0x1f
    e360:	00f407b3          	add	a5,s0,a5
    e364:	02e7e7b3          	rem	a5,a5,a4
    e368:	41fbd613          	srai	a2,s7,0x1f
    e36c:	0ff6f693          	zext.b	a3,a3
    e370:	0ff67613          	zext.b	a2,a2
    e374:	018686b3          	add	a3,a3,s8
    e378:	01760633          	add	a2,a2,s7
    e37c:	4086d693          	srai	a3,a3,0x8
    e380:	40865613          	srai	a2,a2,0x8
    e384:	00100713          	li	a4,1
    e388:	016686b3          	add	a3,a3,s6
    e38c:	40ca8633          	sub	a2,s5,a2
    e390:	00048593          	mv	a1,s1
    e394:	000c8513          	mv	a0,s9
    e398:	00140413          	addi	s0,s0,1
    e39c:	013b8bb3          	add	s7,s7,s3
    e3a0:	012c0c33          	add	s8,s8,s2
    e3a4:	00178793          	addi	a5,a5,1
    e3a8:	8cdfd0ef          	jal	bc74 <hostile_shot>
    e3ac:	fa8a10e3          	bne	s4,s0,e34c <game_tick+0x14c0>
    e3b0:	000c8c13          	mv	s8,s9
    e3b4:	088c2703          	lw	a4,136(s8)
    e3b8:	00048413          	mv	s0,s1
    e3bc:	01c12383          	lw	t2,28(sp)
    e3c0:	01012483          	lw	s1,16(sp)
    e3c4:	01812c83          	lw	s9,24(sp)
    e3c8:	02012e03          	lw	t3,32(sp)
    e3cc:	000d8293          	mv	t0,s11
    e3d0:	32071c63          	bnez	a4,e708 <game_tick+0x187c>
    e3d4:	00139393          	slli	t2,t2,0x1
    e3d8:	04b00793          	li	a5,75
    e3dc:	407783b3          	sub	t2,a5,t2
    e3e0:	0077d463          	bge	a5,t2,e3e8 <game_tick+0x155c>
    e3e4:	04b00393          	li	t2,75
    e3e8:	02400793          	li	a5,36
    e3ec:	4af3cc63          	blt	t2,a5,e8a4 <game_tick+0x1a18>
    e3f0:	00442783          	lw	a5,4(s0)
    e3f4:	00842f83          	lw	t6,8(s0)
    e3f8:	01442f03          	lw	t5,20(s0)
    e3fc:	02742223          	sw	t2,36(s0)
    e400:	da1ff06f          	j	e1a0 <game_tick+0x1314>
    e404:	00000b13          	li	s6,0
    e408:	02842683          	lw	a3,40(s0)
    e40c:	00090a93          	mv	s5,s2
    e410:	00168693          	addi	a3,a3,1
    e414:	02d42423          	sw	a3,40(s0)
    e418:	e20714e3          	bnez	a4,e240 <game_tick+0x13b4>
    e41c:	00000793          	li	a5,0
    e420:	00100a13          	li	s4,1
    e424:	e5dff06f          	j	e280 <game_tick+0x13f4>
    e428:	40000737          	lui	a4,0x40000
    e42c:	00275713          	srli	a4,a4,0x2
    e430:	fee6eee3          	bltu	a3,a4,e42c <game_tick+0x15a0>
    e434:	00068293          	mv	t0,a3
    e438:	00000a13          	li	s4,0
    e43c:	014705b3          	add	a1,a4,s4
    e440:	001a5a13          	srli	s4,s4,0x1
    e444:	00b2e663          	bltu	t0,a1,e450 <game_tick+0x15c4>
    e448:	40b282b3          	sub	t0,t0,a1
    e44c:	00ea0a33          	add	s4,s4,a4
    e450:	00275713          	srli	a4,a4,0x2
    e454:	fe0714e3          	bnez	a4,e43c <game_tick+0x15b0>
    e458:	460a0663          	beqz	s4,e8c4 <game_tick+0x1a38>
    e45c:	03812703          	lw	a4,56(sp)
    e460:	41100ab3          	neg	s5,a7
    e464:	02ea8ab3          	mul	s5,s5,a4
    e468:	02e80733          	mul	a4,a6,a4
    e46c:	034acab3          	div	s5,s5,s4
    e470:	03474a33          	div	s4,a4,s4
    e474:	b49ff06f          	j	dfbc <game_tick+0x1130>
    e478:	40000737          	lui	a4,0x40000
    e47c:	00275713          	srli	a4,a4,0x2
    e480:	fee6eee3          	bltu	a3,a4,e47c <game_tick+0x15f0>
    e484:	00068393          	mv	t2,a3
    e488:	00000593          	li	a1,0
    e48c:	00b70533          	add	a0,a4,a1
    e490:	0015d593          	srli	a1,a1,0x1
    e494:	00a3e663          	bltu	t2,a0,e4a0 <game_tick+0x1614>
    e498:	40a383b3          	sub	t2,t2,a0
    e49c:	00e585b3          	add	a1,a1,a4
    e4a0:	00275713          	srli	a4,a4,0x2
    e4a4:	fe0714e3          	bnez	a4,e48c <game_tick+0x1600>
    e4a8:	00058c93          	mv	s9,a1
    e4ac:	40058863          	beqz	a1,e8bc <game_tick+0x1a30>
    e4b0:	03c12703          	lw	a4,60(sp)
    e4b4:	411002b3          	neg	t0,a7
    e4b8:	02e282b3          	mul	t0,t0,a4
    e4bc:	02e80733          	mul	a4,a6,a4
    e4c0:	02b2c2b3          	div	t0,t0,a1
    e4c4:	02b74cb3          	div	s9,a4,a1
    e4c8:	c85ff06f          	j	e14c <game_tick+0x12c0>
    e4cc:	40000737          	lui	a4,0x40000
    e4d0:	00275713          	srli	a4,a4,0x2
    e4d4:	fee6eee3          	bltu	a3,a4,e4d0 <game_tick+0x1644>
    e4d8:	3e070a63          	beqz	a4,e8cc <game_tick+0x1a40>
    e4dc:	00068293          	mv	t0,a3
    e4e0:	00000a13          	li	s4,0
    e4e4:	014705b3          	add	a1,a4,s4
    e4e8:	001a5a13          	srli	s4,s4,0x1
    e4ec:	00b2e663          	bltu	t0,a1,e4f8 <game_tick+0x166c>
    e4f0:	40b282b3          	sub	t0,t0,a1
    e4f4:	00ea0a33          	add	s4,s4,a4
    e4f8:	00275713          	srli	a4,a4,0x2
    e4fc:	fe0714e3          	bnez	a4,e4e4 <game_tick+0x1658>
    e500:	3c0a0263          	beqz	s4,e8c4 <game_tick+0x1a38>
    e504:	00c12583          	lw	a1,12(sp)
    e508:	41000ab3          	neg	s5,a6
    e50c:	41100733          	neg	a4,a7
    e510:	02ba8ab3          	mul	s5,s5,a1
    e514:	02b70733          	mul	a4,a4,a1
    e518:	034acab3          	div	s5,s5,s4
    e51c:	03474a33          	div	s4,a4,s4
    e520:	a9dff06f          	j	dfbc <game_tick+0x1130>
    e524:	40000737          	lui	a4,0x40000
    e528:	00275713          	srli	a4,a4,0x2
    e52c:	fee6eee3          	bltu	a3,a4,e528 <game_tick+0x169c>
    e530:	3a070463          	beqz	a4,e8d8 <game_tick+0x1a4c>
    e534:	00068393          	mv	t2,a3
    e538:	00000593          	li	a1,0
    e53c:	00b70533          	add	a0,a4,a1
    e540:	0015d593          	srli	a1,a1,0x1
    e544:	00a3e663          	bltu	t2,a0,e550 <game_tick+0x16c4>
    e548:	40a383b3          	sub	t2,t2,a0
    e54c:	00e585b3          	add	a1,a1,a4
    e550:	00275713          	srli	a4,a4,0x2
    e554:	fe0714e3          	bnez	a4,e53c <game_tick+0x16b0>
    e558:	00058c93          	mv	s9,a1
    e55c:	36058063          	beqz	a1,e8bc <game_tick+0x1a30>
    e560:	02412583          	lw	a1,36(sp)
    e564:	410002b3          	neg	t0,a6
    e568:	41100733          	neg	a4,a7
    e56c:	02b282b3          	mul	t0,t0,a1
    e570:	02b70733          	mul	a4,a4,a1
    e574:	0392c2b3          	div	t0,t0,s9
    e578:	03974cb3          	div	s9,a4,s9
    e57c:	bd1ff06f          	j	e14c <game_tick+0x12c0>
    e580:	01412503          	lw	a0,20(sp)
    e584:	f4d00713          	li	a4,-179
    e588:	02c12383          	lw	t2,44(sp)
    e58c:	00e54463          	blt	a0,a4,e594 <game_tick+0x1708>
    e590:	c99fe06f          	j	d228 <game_tick+0x39c>
    e594:	12200393          	li	t2,290
    e598:	c91fe06f          	j	d228 <game_tick+0x39c>
    e59c:	00003f37          	lui	t5,0x3
    e5a0:	a30f0f13          	addi	t5,t5,-1488 # 2a30 <main+0x1620>
    e5a4:	03e5c5b3          	div	a1,a1,t5
    e5a8:	00700f13          	li	t5,7
    e5ac:	00159593          	slli	a1,a1,0x1
    e5b0:	00358593          	addi	a1,a1,3
    e5b4:	00bf5463          	bge	t5,a1,e5bc <game_tick+0x1730>
    e5b8:	00700593          	li	a1,7
    e5bc:	00300f13          	li	t5,3
    e5c0:	01e5d463          	bge	a1,t5,e5c8 <game_tick+0x173c>
    e5c4:	00300593          	li	a1,3
    e5c8:	40000f37          	lui	t5,0x40000
    e5cc:	6fe6d063          	bge	a3,t5,ecac <game_tick+0x1e20>
    e5d0:	002f5f13          	srli	t5,t5,0x2
    e5d4:	ffe6eee3          	bltu	a3,t5,e5d0 <game_tick+0x1744>
    e5d8:	100f0a63          	beqz	t5,e6ec <game_tick+0x1860>
    e5dc:	00000293          	li	t0,0
    e5e0:	005f0733          	add	a4,t5,t0
    e5e4:	0012d293          	srli	t0,t0,0x1
    e5e8:	00e66663          	bltu	a2,a4,e5f4 <game_tick+0x1768>
    e5ec:	40e60633          	sub	a2,a2,a4
    e5f0:	01e282b3          	add	t0,t0,t5
    e5f4:	002f5f13          	srli	t5,t5,0x2
    e5f8:	fe0f14e3          	bnez	t5,e5e0 <game_tick+0x1754>
    e5fc:	01f5d613          	srli	a2,a1,0x1f
    e600:	00b606b3          	add	a3,a2,a1
    e604:	4016d693          	srai	a3,a3,0x1
    e608:	40d006b3          	neg	a3,a3
    e60c:	0e505263          	blez	t0,e6f0 <game_tick+0x1864>
    e610:	03380833          	mul	a6,a6,s3
    e614:	02584f33          	div	t5,a6,t0
    e618:	033888b3          	mul	a7,a7,s3
    e61c:	0258c2b3          	div	t0,a7,t0
    e620:	000f1463          	bnez	t5,e628 <game_tick+0x179c>
    e624:	0c028663          	beqz	t0,e6f0 <game_tick+0x1864>
    e628:	00629993          	slli	s3,t0,0x6
    e62c:	005989b3          	add	s3,s3,t0
    e630:	006f1913          	slli	s2,t5,0x6
    e634:	00912823          	sw	s1,16(sp)
    e638:	01512c23          	sw	s5,24(sp)
    e63c:	01412e23          	sw	s4,28(sp)
    e640:	03612023          	sw	s6,32(sp)
    e644:	000d8a13          	mv	s4,s11
    e648:	01e90933          	add	s2,s2,t5
    e64c:	00058493          	mv	s1,a1
    e650:	00078d93          	mv	s11,a5
    e654:	03c12823          	sw	t3,48(sp)
    e658:	02d28633          	mul	a2,t0,a3
    e65c:	02df06b3          	mul	a3,t5,a3
    e660:	00661893          	slli	a7,a2,0x6
    e664:	00c88d33          	add	s10,a7,a2
    e668:	000d0a93          	mv	s5,s10
    e66c:	00098d13          	mv	s10,s3
    e670:	00028993          	mv	s3,t0
    e674:	00669813          	slli	a6,a3,0x6
    e678:	00d80cb3          	add	s9,a6,a3
    e67c:	000c8b13          	mv	s6,s9
    e680:	000f0c93          	mv	s9,t5
    e684:	41fb5693          	srai	a3,s6,0x1f
    e688:	41fad613          	srai	a2,s5,0x1f
    e68c:	0ff6f693          	zext.b	a3,a3
    e690:	0ff67613          	zext.b	a2,a2
    e694:	016686b3          	add	a3,a3,s6
    e698:	01560633          	add	a2,a2,s5
    e69c:	4086d693          	srai	a3,a3,0x8
    e6a0:	40865613          	srai	a2,a2,0x8
    e6a4:	00000793          	li	a5,0
    e6a8:	000b8713          	mv	a4,s7
    e6ac:	013686b3          	add	a3,a3,s3
    e6b0:	40cc8633          	sub	a2,s9,a2
    e6b4:	00040593          	mv	a1,s0
    e6b8:	000c0513          	mv	a0,s8
    e6bc:	001a0a13          	addi	s4,s4,1
    e6c0:	db4fd0ef          	jal	bc74 <hostile_shot>
    e6c4:	01aa8ab3          	add	s5,s5,s10
    e6c8:	012b0b33          	add	s6,s6,s2
    e6cc:	fb449ce3          	bne	s1,s4,e684 <game_tick+0x17f8>
    e6d0:	01012483          	lw	s1,16(sp)
    e6d4:	01812a83          	lw	s5,24(sp)
    e6d8:	01c12a03          	lw	s4,28(sp)
    e6dc:	02012b03          	lw	s6,32(sp)
    e6e0:	03012e03          	lw	t3,48(sp)
    e6e4:	000d8793          	mv	a5,s11
    e6e8:	9bdff06f          	j	e0a4 <game_tick+0x1218>
    e6ec:	01f5d613          	srli	a2,a1,0x1f
    e6f0:	00b606b3          	add	a3,a2,a1
    e6f4:	4016d693          	srai	a3,a3,0x1
    e6f8:	40d006b3          	neg	a3,a3
    e6fc:	00000293          	li	t0,0
    e700:	00098f13          	mv	t5,s3
    e704:	f25ff06f          	j	e628 <game_tick+0x179c>
    e708:	08cc2603          	lw	a2,140(s8)
    e70c:	00200793          	li	a5,2
    e710:	fff60693          	addi	a3,a2,-1
    e714:	34d7eee3          	bltu	a5,a3,f270 <game_tick+0x23e4>
    e718:	00442783          	lw	a5,4(s0)
    e71c:	00842f83          	lw	t6,8(s0)
    e720:	01442f03          	lw	t5,20(s0)
    e724:	00361693          	slli	a3,a2,0x3
    e728:	40c686b3          	sub	a3,a3,a2
    e72c:	03412603          	lw	a2,52(sp)
    e730:	00269693          	slli	a3,a3,0x2
    e734:	00d606b3          	add	a3,a2,a3
    e738:	0146a383          	lw	t2,20(a3)
    e73c:	02742223          	sw	t2,36(s0)
    e740:	a61ff06f          	j	e1a0 <game_tick+0x1314>
    e744:	0a042c23          	sw	zero,184(s0)
    e748:	0644a703          	lw	a4,100(s1)
    e74c:	0584a403          	lw	s0,88(s1)
    e750:	00300793          	li	a5,3
    e754:	fff70713          	addi	a4,a4,-1 # 3fffffff <__freertos_irq_stack_top+0x3ff17e5f>
    e758:	00140413          	addi	s0,s0,1
    e75c:	06e4a223          	sw	a4,100(s1)
    e760:	0484ac23          	sw	s0,88(s1)
    e764:	26fa8663          	beq	s5,a5,e9d0 <game_tick+0x1b44>
    e768:	00400793          	li	a5,4
    e76c:	00200893          	li	a7,2
    e770:	00fa8a63          	beq	s5,a5,e784 <game_tick+0x18f8>
    e774:	00200793          	li	a5,2
    e778:	00100893          	li	a7,1
    e77c:	00fa9463          	bne	s5,a5,e784 <game_tick+0x18f8>
    e780:	00300893          	li	a7,3
    e784:	01012783          	lw	a5,16(sp)
    e788:	00c12583          	lw	a1,12(sp)
    e78c:	00000613          	li	a2,0
    e790:	00f486b3          	add	a3,s1,a5
    e794:	0c000513          	li	a0,192
    e798:	0580006f          	j	e7f0 <game_tick+0x1964>
    e79c:	0046ae03          	lw	t3,4(a3)
    e7a0:	0086a303          	lw	t1,8(a3)
    e7a4:	41be0e33          	sub	t3,t3,s11
    e7a8:	41330333          	sub	t1,t1,s3
    e7ac:	41fe5793          	srai	a5,t3,0x1f
    e7b0:	41f35713          	srai	a4,t1,0x1f
    e7b4:	0ff7f793          	zext.b	a5,a5
    e7b8:	0ff77713          	zext.b	a4,a4
    e7bc:	01c787b3          	add	a5,a5,t3
    e7c0:	00670733          	add	a4,a4,t1
    e7c4:	4087d793          	srai	a5,a5,0x8
    e7c8:	40875713          	srai	a4,a4,0x8
    e7cc:	02f787b3          	mul	a5,a5,a5
    e7d0:	02e70733          	mul	a4,a4,a4
    e7d4:	00e787b3          	add	a5,a5,a4
    e7d8:	00b7d663          	bge	a5,a1,e7e4 <game_tick+0x1958>
    e7dc:	00078593          	mv	a1,a5
    e7e0:	00060a13          	mv	s4,a2
    e7e4:	00160613          	addi	a2,a2,1
    e7e8:	01068693          	addi	a3,a3,16
    e7ec:	4ca60463          	beq	a2,a0,ecb4 <game_tick+0x1e28>
    e7f0:	0006a783          	lw	a5,0(a3)
    e7f4:	fa0794e3          	bnez	a5,e79c <game_tick+0x1910>
    e7f8:	00461793          	slli	a5,a2,0x4
    e7fc:	00023737          	lui	a4,0x23
    e800:	00f487b3          	add	a5,s1,a5
    e804:	00e787b3          	add	a5,a5,a4
    e808:	00100713          	li	a4,1
    e80c:	5ce7aa23          	sw	a4,1492(a5)
    e810:	5db7ac23          	sw	s11,1496(a5)
    e814:	5d37ae23          	sw	s3,1500(a5)
    e818:	5f17a023          	sw	a7,1504(a5)
    e81c:	00900713          	li	a4,9
    e820:	00200693          	li	a3,2
    e824:	00098613          	mv	a2,s3
    e828:	000d8593          	mv	a1,s11
    e82c:	00048513          	mv	a0,s1
    e830:	d8dfc0ef          	jal	b5bc <particles>
    e834:	00300793          	li	a5,3
    e838:	02fa8a63          	beq	s5,a5,e86c <game_tick+0x19e0>
    e83c:	00f00793          	li	a5,15
    e840:	02f46433          	rem	s0,s0,a5
    e844:	00040463          	beqz	s0,e84c <game_tick+0x19c0>
    e848:	fedfe06f          	j	d834 <game_tick+0x9a8>
    e84c:	0184a783          	lw	a5,24(s1)
    e850:	00f04463          	bgtz	a5,e858 <game_tick+0x19cc>
    e854:	fe1fe06f          	j	d834 <game_tick+0x9a8>
    e858:	01c4a703          	lw	a4,28(s1)
    e85c:	00578793          	addi	a5,a5,5
    e860:	02f74863          	blt	a4,a5,e890 <game_tick+0x1a04>
    e864:	00f4ac23          	sw	a5,24(s1)
    e868:	fcdfe06f          	j	d834 <game_tick+0x9a8>
    e86c:	0844a703          	lw	a4,132(s1)
    e870:	0184a783          	lw	a5,24(s1)
    e874:	00170713          	addi	a4,a4,1 # 23001 <background_pixels+0xb141>
    e878:	08e4a223          	sw	a4,132(s1)
    e87c:	00f04463          	bgtz	a5,e884 <game_tick+0x19f8>
    e880:	fb5fe06f          	j	d834 <game_tick+0x9a8>
    e884:	01c4a703          	lw	a4,28(s1)
    e888:	00c78793          	addi	a5,a5,12
    e88c:	fcf75ce3          	bge	a4,a5,e864 <game_tick+0x19d8>
    e890:	00070793          	mv	a5,a4
    e894:	00f4ac23          	sw	a5,24(s1)
    e898:	f9dfe06f          	j	d834 <game_tick+0x9a8>
    e89c:	00000313          	li	t1,0
    e8a0:	e48ff06f          	j	dee8 <game_tick+0x105c>
    e8a4:	02400393          	li	t2,36
    e8a8:	00442783          	lw	a5,4(s0)
    e8ac:	00842f83          	lw	t6,8(s0)
    e8b0:	01442f03          	lw	t5,20(s0)
    e8b4:	02742223          	sw	t2,36(s0)
    e8b8:	8e9ff06f          	j	e1a0 <game_tick+0x1314>
    e8bc:	00000293          	li	t0,0
    e8c0:	88dff06f          	j	e14c <game_tick+0x12c0>
    e8c4:	00000a93          	li	s5,0
    e8c8:	ef4ff06f          	j	dfbc <game_tick+0x1130>
    e8cc:	00000a13          	li	s4,0
    e8d0:	00000a93          	li	s5,0
    e8d4:	ee8ff06f          	j	dfbc <game_tick+0x1130>
    e8d8:	00000c93          	li	s9,0
    e8dc:	00000293          	li	t0,0
    e8e0:	86dff06f          	j	e14c <game_tick+0x12c0>
    e8e4:	000247b7          	lui	a5,0x24
    e8e8:	000258b7          	lui	a7,0x25
    e8ec:	1d478793          	addi	a5,a5,468 # 241d4 <background_pixels+0xc314>
    e8f0:	1d488893          	addi	a7,a7,468 # 251d4 <background_pixels+0xd314>
    e8f4:	00f487b3          	add	a5,s1,a5
    e8f8:	011488b3          	add	a7,s1,a7
    e8fc:	0eb00313          	li	t1,235
    e900:	0007a703          	lw	a4,0(a5)
    e904:	04070e63          	beqz	a4,e960 <game_tick+0x1ad4>
    e908:	0107a803          	lw	a6,16(a5)
    e90c:	0147a683          	lw	a3,20(a5)
    e910:	00c7a603          	lw	a2,12(a5)
    e914:	02680533          	mul	a0,a6,t1
    e918:	fff70713          	addi	a4,a4,-1
    e91c:	00d60633          	add	a2,a2,a3
    e920:	00e7a023          	sw	a4,0(a5)
    e924:	0087a583          	lw	a1,8(a5)
    e928:	00c7a623          	sw	a2,12(a5)
    e92c:	010585b3          	add	a1,a1,a6
    e930:	00b7a423          	sw	a1,8(a5)
    e934:	026686b3          	mul	a3,a3,t1
    e938:	41f55713          	srai	a4,a0,0x1f
    e93c:	0ff77713          	zext.b	a4,a4
    e940:	00a70733          	add	a4,a4,a0
    e944:	40875713          	srai	a4,a4,0x8
    e948:	00e7a823          	sw	a4,16(a5)
    e94c:	41f6d713          	srai	a4,a3,0x1f
    e950:	0ff77713          	zext.b	a4,a4
    e954:	00d70733          	add	a4,a4,a3
    e958:	40875713          	srai	a4,a4,0x8
    e95c:	00e7aa23          	sw	a4,20(a5)
    e960:	02078793          	addi	a5,a5,32
    e964:	f9179ee3          	bne	a5,a7,e900 <game_tick+0x1a74>
    e968:	0184a783          	lw	a5,24(s1)
    e96c:	30f04063          	bgtz	a5,ec6c <game_tick+0x1de0>
    e970:	00500793          	li	a5,5
    e974:	0004ac23          	sw	zero,24(s1)
    e978:	00f4a023          	sw	a5,0(s1)
    e97c:	08812403          	lw	s0,136(sp)
    e980:	08012903          	lw	s2,128(sp)
    e984:	07c12983          	lw	s3,124(sp)
    e988:	07812a03          	lw	s4,120(sp)
    e98c:	07412a83          	lw	s5,116(sp)
    e990:	07012b03          	lw	s6,112(sp)
    e994:	06c12b83          	lw	s7,108(sp)
    e998:	06812c03          	lw	s8,104(sp)
    e99c:	06412c83          	lw	s9,100(sp)
    e9a0:	06012d03          	lw	s10,96(sp)
    e9a4:	05c12d83          	lw	s11,92(sp)
    e9a8:	848ff06f          	j	d9f0 <game_tick+0xb64>
    e9ac:	400009b7          	lui	s3,0x40000
    e9b0:	845ff06f          	j	e1f4 <game_tick+0x1368>
    e9b4:	00028793          	mv	a5,t0
    e9b8:	8d9fe06f          	j	d290 <game_tick+0x404>
    e9bc:	fff78793          	addi	a5,a5,-1
    e9c0:	02fc2a23          	sw	a5,52(s8)
    e9c4:	00078463          	beqz	a5,e9cc <game_tick+0x1b40>
    e9c8:	ce5fe06f          	j	d6ac <game_tick+0x820>
    e9cc:	a75fe06f          	j	d440 <game_tick+0x5b4>
    e9d0:	01412703          	lw	a4,20(sp)
    e9d4:	000017b7          	lui	a5,0x1
    e9d8:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    e9dc:	02f747b3          	div	a5,a4,a5
    e9e0:	00279893          	slli	a7,a5,0x2
    e9e4:	00f888b3          	add	a7,a7,a5
    e9e8:	01988893          	addi	a7,a7,25
    e9ec:	d99ff06f          	j	e784 <game_tick+0x18f8>
    e9f0:	40000737          	lui	a4,0x40000
    e9f4:	d8cff06f          	j	df80 <game_tick+0x10f4>
    e9f8:	40000737          	lui	a4,0x40000
    e9fc:	f14ff06f          	j	e110 <game_tick+0x1284>
    ea00:	00023637          	lui	a2,0x23
    ea04:	00c48633          	add	a2,s1,a2
    ea08:	5b862783          	lw	a5,1464(a2) # 235b8 <background_pixels+0xb6f8>
    ea0c:	44078863          	beqz	a5,ee5c <game_tick+0x1fd0>
    ea10:	00200593          	li	a1,2
    ea14:	5bc62c03          	lw	s8,1468(a2)
    ea18:	76d5e663          	bltu	a1,a3,f184 <game_tick+0x22f8>
    ea1c:	00371693          	slli	a3,a4,0x3
    ea20:	00013a37          	lui	s4,0x13
    ea24:	40e68633          	sub	a2,a3,a4
    ea28:	144a0a13          	addi	s4,s4,324 # 13144 <stress_settings>
    ea2c:	00261613          	slli	a2,a2,0x2
    ea30:	00ca0633          	add	a2,s4,a2
    ea34:	00462603          	lw	a2,4(a2)
    ea38:	01865463          	bge	a2,s8,ea40 <game_tick+0x1bb4>
    ea3c:	00060c13          	mv	s8,a2
    ea40:	000c1463          	bnez	s8,ea48 <game_tick+0x1bbc>
    ea44:	e4dfe06f          	j	d890 <game_tick+0xa04>
    ea48:	40e68733          	sub	a4,a3,a4
    ea4c:	00271713          	slli	a4,a4,0x2
    ea50:	000236b7          	lui	a3,0x23
    ea54:	00ea0733          	add	a4,s4,a4
    ea58:	00d486b3          	add	a3,s1,a3
    ea5c:	5bc6a683          	lw	a3,1468(a3) # 235bc <background_pixels+0xb6fc>
    ea60:	00472703          	lw	a4,4(a4) # 40000004 <__freertos_irq_stack_top+0x3ff17e64>
    ea64:	00e6d463          	bge	a3,a4,ea6c <game_tick+0x1be0>
    ea68:	00068713          	mv	a4,a3
    ea6c:	00e02733          	sgtz	a4,a4
    ea70:	40e00733          	neg	a4,a4
    ea74:	00e7f7b3          	and	a5,a5,a4
    ea78:	01804463          	bgtz	s8,ea80 <game_tick+0x1bf4>
    ea7c:	e15fe06f          	j	d890 <game_tick+0xa04>
    ea80:	ffffcb37          	lui	s6,0xffffc
    ea84:	00018ab7          	lui	s5,0x18
    ea88:	000239b7          	lui	s3,0x23
    ea8c:	00078413          	mv	s0,a5
    ea90:	00000913          	li	s2,0
    ea94:	013489b3          	add	s3,s1,s3
    ea98:	00027a37          	lui	s4,0x27
    ea9c:	600b0b13          	addi	s6,s6,1536 # ffffc600 <__freertos_irq_stack_top+0xfff14460>
    eaa0:	200a8a93          	addi	s5,s5,512 # 18200 <background_pixels+0x340>
    eaa4:	08f00b93          	li	s7,143
    eaa8:	07c0006f          	j	eb24 <game_tick+0x1c98>
    eaac:	80078693          	addi	a3,a5,-2048
    eab0:	0ada6663          	bltu	s4,a3,eb5c <game_tick+0x1cd0>
    eab4:	016706b3          	add	a3,a4,s6
    eab8:	0adae263          	bltu	s5,a3,eb5c <game_tick+0x1cd0>
    eabc:	0104a583          	lw	a1,16(s1)
    eac0:	0144a603          	lw	a2,20(s1)
    eac4:	40f587b3          	sub	a5,a1,a5
    eac8:	40e60733          	sub	a4,a2,a4
    eacc:	41f7d693          	srai	a3,a5,0x1f
    ead0:	41f75513          	srai	a0,a4,0x1f
    ead4:	0ff6f693          	zext.b	a3,a3
    ead8:	0ff57513          	zext.b	a0,a0
    eadc:	00f687b3          	add	a5,a3,a5
    eae0:	00e50733          	add	a4,a0,a4
    eae4:	4087d793          	srai	a5,a5,0x8
    eae8:	40875713          	srai	a4,a4,0x8
    eaec:	02f787b3          	mul	a5,a5,a5
    eaf0:	02e70733          	mul	a4,a4,a4
    eaf4:	00e787b3          	add	a5,a5,a4
    eaf8:	00fbce63          	blt	s7,a5,eb14 <game_tick+0x1c88>
    eafc:	00042023          	sw	zero,0(s0)
    eb00:	5c09a783          	lw	a5,1472(s3) # 235c0 <background_pixels+0xb700>
    eb04:	00f95463          	bge	s2,a5,eb0c <game_tick+0x1c80>
    eb08:	5d29a023          	sw	s2,1472(s3)
    eb0c:	0204a783          	lw	a5,32(s1)
    eb10:	68078663          	beqz	a5,f19c <game_tick+0x2310>
    eb14:	00190913          	addi	s2,s2,1
    eb18:	02040413          	addi	s0,s0,32
    eb1c:	01891463          	bne	s2,s8,eb24 <game_tick+0x1c98>
    eb20:	d71fe06f          	j	d890 <game_tick+0xa04>
    eb24:	00042783          	lw	a5,0(s0)
    eb28:	fe0786e3          	beqz	a5,eb14 <game_tick+0x1c88>
    eb2c:	00442783          	lw	a5,4(s0)
    eb30:	00842703          	lw	a4,8(s0)
    eb34:	01442683          	lw	a3,20(s0)
    eb38:	00c42583          	lw	a1,12(s0)
    eb3c:	01042603          	lw	a2,16(s0)
    eb40:	fff68693          	addi	a3,a3,-1
    eb44:	00b787b3          	add	a5,a5,a1
    eb48:	00c70733          	add	a4,a4,a2
    eb4c:	00f42223          	sw	a5,4(s0)
    eb50:	00e42423          	sw	a4,8(s0)
    eb54:	00d42a23          	sw	a3,20(s0)
    eb58:	f4d04ae3          	bgtz	a3,eaac <game_tick+0x1c20>
    eb5c:	00042023          	sw	zero,0(s0)
    eb60:	5c09a783          	lw	a5,1472(s3)
    eb64:	faf958e3          	bge	s2,a5,eb14 <game_tick+0x1c88>
    eb68:	5d29a023          	sw	s2,1472(s3)
    eb6c:	fa9ff06f          	j	eb14 <game_tick+0x1c88>
    eb70:	01412703          	lw	a4,20(sp)
    eb74:	000017b7          	lui	a5,0x1
    eb78:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    eb7c:	02f76733          	rem	a4,a4,a5
    eb80:	70800613          	li	a2,1800
    eb84:	0804a683          	lw	a3,128(s1)
    eb88:	20c70e63          	beq	a4,a2,eda4 <game_tick+0x1f18>
    eb8c:	00071663          	bnez	a4,eb98 <game_tick+0x1d0c>
    eb90:	00168693          	addi	a3,a3,1
    eb94:	08d4a023          	sw	a3,128(s1)
    eb98:	28069a63          	bnez	a3,ee2c <game_tick+0x1fa0>
    eb9c:	07c4a703          	lw	a4,124(s1)
    eba0:	0744a783          	lw	a5,116(s1)
    eba4:	30071463          	bnez	a4,eeac <game_tick+0x2020>
    eba8:	01240433          	add	s0,s0,s2
    ebac:	00241413          	slli	s0,s0,0x2
    ebb0:	01840413          	addi	s0,s0,24
    ebb4:	30078a63          	beqz	a5,eec8 <game_tick+0x203c>
    ebb8:	03040413          	addi	s0,s0,48
    ebbc:	0bc00713          	li	a4,188
    ebc0:	00875463          	bge	a4,s0,ebc8 <game_tick+0x1d3c>
    ebc4:	0bc00413          	li	s0,188
    ebc8:	01800713          	li	a4,24
    ebcc:	00e45463          	bge	s0,a4,ebd4 <game_tick+0x1d48>
    ebd0:	01800413          	li	s0,24
    ebd4:	0784a703          	lw	a4,120(s1)
    ebd8:	fff70713          	addi	a4,a4,-1
    ebdc:	06e4ac23          	sw	a4,120(s1)
    ebe0:	4ee05263          	blez	a4,f0c4 <game_tick+0x2238>
    ebe4:	05c4a783          	lw	a5,92(s1)
    ebe8:	fff78793          	addi	a5,a5,-1
    ebec:	04f4ae23          	sw	a5,92(s1)
    ebf0:	00f05463          	blez	a5,ebf8 <game_tick+0x1d6c>
    ebf4:	cc4fe06f          	j	d0b8 <game_tick+0x22c>
    ebf8:	01412703          	lw	a4,20(sp)
    ebfc:	000017b7          	lui	a5,0x1
    ec00:	95f78793          	addi	a5,a5,-1697 # 95f <CUSTOM2+0x904>
    ec04:	3ee7ca63          	blt	a5,a4,eff8 <game_tick+0x216c>
    ec08:	70700793          	li	a5,1799
    ec0c:	12e7c863          	blt	a5,a4,ed3c <game_tick+0x1eb0>
    ec10:	38300793          	li	a5,899
    ec14:	60e7c663          	blt	a5,a4,f220 <game_tick+0x2394>
    ec18:	0644a783          	lw	a5,100(s1)
    ec1c:	0087d863          	bge	a5,s0,ec2c <game_tick+0x1da0>
    ec20:	00000593          	li	a1,0
    ec24:	00048513          	mv	a0,s1
    ec28:	d8dfc0ef          	jal	b9b4 <spawn_enemy>
    ec2c:	01412703          	lw	a4,20(sp)
    ec30:	f1000793          	li	a5,-240
    ec34:	02f747b3          	div	a5,a4,a5
    ec38:	02a00713          	li	a4,42
    ec3c:	02a78793          	addi	a5,a5,42
    ec40:	00f75463          	bge	a4,a5,ec48 <game_tick+0x1dbc>
    ec44:	02a00793          	li	a5,42
    ec48:	00800713          	li	a4,8
    ec4c:	00e7d463          	bge	a5,a4,ec54 <game_tick+0x1dc8>
    ec50:	00800793          	li	a5,8
    ec54:	04f4ae23          	sw	a5,92(s1)
    ec58:	c60fe06f          	j	d0b8 <game_tick+0x22c>
    ec5c:	00c105b7          	lui	a1,0xc10
    ec60:	00100613          	li	a2,1
    ec64:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    ec68:	e01fe06f          	j	da68 <game_tick+0xbdc>
    ec6c:	08812403          	lw	s0,136(sp)
    ec70:	08012903          	lw	s2,128(sp)
    ec74:	07c12983          	lw	s3,124(sp)
    ec78:	07812a03          	lw	s4,120(sp)
    ec7c:	07412a83          	lw	s5,116(sp)
    ec80:	07012b03          	lw	s6,112(sp)
    ec84:	06c12b83          	lw	s7,108(sp)
    ec88:	06812c03          	lw	s8,104(sp)
    ec8c:	06412c83          	lw	s9,100(sp)
    ec90:	06012d03          	lw	s10,96(sp)
    ec94:	05c12d83          	lw	s11,92(sp)
    ec98:	08c12083          	lw	ra,140(sp)
    ec9c:	00048513          	mv	a0,s1
    eca0:	08412483          	lw	s1,132(sp)
    eca4:	09010113          	addi	sp,sp,144
    eca8:	a05fc06f          	j	b6ac <check_level>
    ecac:	40000f37          	lui	t5,0x40000
    ecb0:	92dff06f          	j	e5dc <game_tick+0x1750>
    ecb4:	004a1793          	slli	a5,s4,0x4
    ecb8:	00023737          	lui	a4,0x23
    ecbc:	00f487b3          	add	a5,s1,a5
    ecc0:	00e787b3          	add	a5,a5,a4
    ecc4:	5e07a703          	lw	a4,1504(a5)
    ecc8:	01170733          	add	a4,a4,a7
    eccc:	5ee7a023          	sw	a4,1504(a5)
    ecd0:	b4dff06f          	j	e81c <game_tick+0x1990>
    ecd4:	0087a983          	lw	s3,8(a5)
    ecd8:	03305a63          	blez	s3,ed0c <game_tick+0x1e80>
    ecdc:	0c000793          	li	a5,192
    ece0:	0127d463          	bge	a5,s2,ece8 <game_tick+0x1e5c>
    ece4:	0c000913          	li	s2,192
    ece8:	00000413          	li	s0,0
    ecec:	0180006f          	j	ed04 <game_tick+0x1e78>
    ecf0:	00400593          	li	a1,4
    ecf4:	00048513          	mv	a0,s1
    ecf8:	00140413          	addi	s0,s0,1
    ecfc:	cb9fc0ef          	jal	b9b4 <spawn_enemy>
    ed00:	01340663          	beq	s0,s3,ed0c <game_tick+0x1e80>
    ed04:	0644a783          	lw	a5,100(s1)
    ed08:	ff27c4e3          	blt	a5,s2,ecf0 <game_tick+0x1e64>
    ed0c:	416a87b3          	sub	a5,s5,s6
    ed10:	00279793          	slli	a5,a5,0x2
    ed14:	00fa07b3          	add	a5,s4,a5
    ed18:	00c7a783          	lw	a5,12(a5)
    ed1c:	04f4ae23          	sw	a5,92(s1)
    ed20:	b98fe06f          	j	d0b8 <game_tick+0x22c>
    ed24:	00100593          	li	a1,1
    ed28:	800ff06f          	j	dd28 <game_tick+0xe9c>
    ed2c:	00c105b7          	lui	a1,0xc10
    ed30:	00300613          	li	a2,3
    ed34:	fee58593          	addi	a1,a1,-18 # c0ffee <__freertos_irq_stack_top+0xb27e4e>
    ed38:	d31fe06f          	j	da68 <game_tick+0xbdc>
    ed3c:	0644a783          	lw	a5,100(s1)
    ed40:	ee87d6e3          	bge	a5,s0,ec2c <game_tick+0x1da0>
    ed44:	0044a703          	lw	a4,4(s1)
    ed48:	00d71793          	slli	a5,a4,0xd
    ed4c:	00e7c7b3          	xor	a5,a5,a4
    ed50:	0117d713          	srli	a4,a5,0x11
    ed54:	00f74733          	xor	a4,a4,a5
    ed58:	00571793          	slli	a5,a4,0x5
    ed5c:	00e7c7b3          	xor	a5,a5,a4
    ed60:	00500713          	li	a4,5
    ed64:	02e7f733          	remu	a4,a5,a4
    ed68:	00f4a223          	sw	a5,4(s1)
    ed6c:	18070463          	beqz	a4,eef4 <game_tick+0x2068>
    ed70:	00d79713          	slli	a4,a5,0xd
    ed74:	00f747b3          	xor	a5,a4,a5
    ed78:	0117d713          	srli	a4,a5,0x11
    ed7c:	00f74733          	xor	a4,a4,a5
    ed80:	00571793          	slli	a5,a4,0x5
    ed84:	00e7c7b3          	xor	a5,a5,a4
    ed88:	00300593          	li	a1,3
    ed8c:	02b7f5b3          	remu	a1,a5,a1
    ed90:	00f4a223          	sw	a5,4(s1)
    ed94:	00048513          	mv	a0,s1
    ed98:	0015b593          	seqz	a1,a1
    ed9c:	c19fc0ef          	jal	b9b4 <spawn_enemy>
    eda0:	e8dff06f          	j	ec2c <game_tick+0x1da0>
    eda4:	01412703          	lw	a4,20(sp)
    eda8:	07800613          	li	a2,120
    edac:	02f747b3          	div	a5,a4,a5
    edb0:	00179713          	slli	a4,a5,0x1
    edb4:	00f707b3          	add	a5,a4,a5
    edb8:	00179793          	slli	a5,a5,0x1
    edbc:	02478793          	addi	a5,a5,36
    edc0:	00f65463          	bge	a2,a5,edc8 <game_tick+0x1f3c>
    edc4:	07800793          	li	a5,120
    edc8:	0744a703          	lw	a4,116(s1)
    edcc:	0604ac23          	sw	zero,120(s1)
    edd0:	00e787b3          	add	a5,a5,a4
    edd4:	25800713          	li	a4,600
    edd8:	06e4ae23          	sw	a4,124(s1)
    eddc:	00100713          	li	a4,1
    ede0:	08e4a823          	sw	a4,144(s1)
    ede4:	0b400713          	li	a4,180
    ede8:	06f4aa23          	sw	a5,116(s1)
    edec:	06e4a423          	sw	a4,104(s1)
    edf0:	02069e63          	bnez	a3,ee2c <game_tick+0x1fa0>
    edf4:	01240433          	add	s0,s0,s2
    edf8:	00241413          	slli	s0,s0,0x2
    edfc:	25700713          	li	a4,599
    ee00:	06e4ae23          	sw	a4,124(s1)
    ee04:	01840413          	addi	s0,s0,24
    ee08:	03040413          	addi	s0,s0,48
    ee0c:	0bc00713          	li	a4,188
    ee10:	00875463          	bge	a4,s0,ee18 <game_tick+0x1f8c>
    ee14:	0bc00413          	li	s0,188
    ee18:	01800713          	li	a4,24
    ee1c:	00e45463          	bge	s0,a4,ee24 <game_tick+0x1f98>
    ee20:	01800413          	li	s0,24
    ee24:	dc0780e3          	beqz	a5,ebe4 <game_tick+0x1d58>
    ee28:	dadff06f          	j	ebd4 <game_tick+0x1d48>
    ee2c:	00300593          	li	a1,3
    ee30:	00048513          	mv	a0,s1
    ee34:	b81fc0ef          	jal	b9b4 <spawn_enemy>
    ee38:	d60502e3          	beqz	a0,eb9c <game_tick+0x1d10>
    ee3c:	0804a783          	lw	a5,128(s1)
    ee40:	fff78793          	addi	a5,a5,-1
    ee44:	08f4a023          	sw	a5,128(s1)
    ee48:	00200793          	li	a5,2
    ee4c:	08f4a823          	sw	a5,144(s1)
    ee50:	0d200793          	li	a5,210
    ee54:	06f4a423          	sw	a5,104(s1)
    ee58:	d45ff06f          	j	eb9c <game_tick+0x1d10>
    ee5c:	00200793          	li	a5,2
    ee60:	3ad7e063          	bltu	a5,a3,f200 <game_tick+0x2374>
    ee64:	00371793          	slli	a5,a4,0x3
    ee68:	40e78733          	sub	a4,a5,a4
    ee6c:	000137b7          	lui	a5,0x13
    ee70:	00271713          	slli	a4,a4,0x2
    ee74:	14478793          	addi	a5,a5,324 # 13144 <stress_settings>
    ee78:	00e787b3          	add	a5,a5,a4
    ee7c:	0047ac03          	lw	s8,4(a5)
    ee80:	000c1463          	bnez	s8,ee88 <game_tick+0x1ffc>
    ee84:	a0dfe06f          	j	d890 <game_tick+0xa04>
    ee88:	01804463          	bgtz	s8,ee90 <game_tick+0x2004>
    ee8c:	a05fe06f          	j	d890 <game_tick+0xa04>
    ee90:	000017b7          	lui	a5,0x1
    ee94:	0187d463          	bge	a5,s8,ee9c <game_tick+0x2010>
    ee98:	00001c37          	lui	s8,0x1
    ee9c:	000037b7          	lui	a5,0x3
    eea0:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    eea4:	00f487b3          	add	a5,s1,a5
    eea8:	bd9ff06f          	j	ea80 <game_tick+0x1bf4>
    eeac:	fff70713          	addi	a4,a4,-1 # 22fff <background_pixels+0xb13f>
    eeb0:	01240433          	add	s0,s0,s2
    eeb4:	00241413          	slli	s0,s0,0x2
    eeb8:	06e4ae23          	sw	a4,124(s1)
    eebc:	01840413          	addi	s0,s0,24
    eec0:	f40714e3          	bnez	a4,ee08 <game_tick+0x1f7c>
    eec4:	cf1ff06f          	j	ebb4 <game_tick+0x1d28>
    eec8:	0bc00793          	li	a5,188
    eecc:	0087d463          	bge	a5,s0,eed4 <game_tick+0x2048>
    eed0:	0bc00413          	li	s0,188
    eed4:	01800793          	li	a5,24
    eed8:	d0f456e3          	bge	s0,a5,ebe4 <game_tick+0x1d58>
    eedc:	01800413          	li	s0,24
    eee0:	d05ff06f          	j	ebe4 <game_tick+0x1d58>
    eee4:	01e00793          	li	a5,30
    eee8:	04f4ae23          	sw	a5,92(s1)
    eeec:	0004a023          	sw	zero,0(s1)
    eef0:	a8dff06f          	j	e97c <game_tick+0x1af0>
    eef4:	00200593          	li	a1,2
    eef8:	00048513          	mv	a0,s1
    eefc:	ab9fc0ef          	jal	b9b4 <spawn_enemy>
    ef00:	d2dff06f          	j	ec2c <game_tick+0x1da0>
    ef04:	40e687b3          	sub	a5,a3,a4
    ef08:	00279793          	slli	a5,a5,0x2
    ef0c:	00fa07b3          	add	a5,s4,a5
    ef10:	0047a783          	lw	a5,4(a5)
    ef14:	30f05063          	blez	a5,f214 <game_tick+0x2388>
    ef18:	000037b7          	lui	a5,0x3
    ef1c:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    ef20:	00f487b3          	add	a5,s1,a5
    ef24:	00c04463          	bgtz	a2,ef2c <game_tick+0x20a0>
    ef28:	969fe06f          	j	d890 <game_tick+0xa04>
    ef2c:	ffffcb37          	lui	s6,0xffffc
    ef30:	00018ab7          	lui	s5,0x18
    ef34:	00078413          	mv	s0,a5
    ef38:	00027a37          	lui	s4,0x27
    ef3c:	600b0b13          	addi	s6,s6,1536 # ffffc600 <__freertos_irq_stack_top+0xfff14460>
    ef40:	200a8a93          	addi	s5,s5,512 # 18200 <background_pixels+0x340>
    ef44:	08f00b93          	li	s7,143
    ef48:	0700006f          	j	efb8 <game_tick+0x212c>
    ef4c:	80078693          	addi	a3,a5,-2048
    ef50:	0ada6063          	bltu	s4,a3,eff0 <game_tick+0x2164>
    ef54:	016706b3          	add	a3,a4,s6
    ef58:	08daec63          	bltu	s5,a3,eff0 <game_tick+0x2164>
    ef5c:	0104a583          	lw	a1,16(s1)
    ef60:	0144a603          	lw	a2,20(s1)
    ef64:	40f587b3          	sub	a5,a1,a5
    ef68:	40e60733          	sub	a4,a2,a4
    ef6c:	41f7d513          	srai	a0,a5,0x1f
    ef70:	41f75693          	srai	a3,a4,0x1f
    ef74:	0ff57513          	zext.b	a0,a0
    ef78:	0ff6f693          	zext.b	a3,a3
    ef7c:	00f50533          	add	a0,a0,a5
    ef80:	00e687b3          	add	a5,a3,a4
    ef84:	4087d793          	srai	a5,a5,0x8
    ef88:	40855713          	srai	a4,a0,0x8
    ef8c:	02f787b3          	mul	a5,a5,a5
    ef90:	02e70733          	mul	a4,a4,a4
    ef94:	00e787b3          	add	a5,a5,a4
    ef98:	00fbc863          	blt	s7,a5,efa8 <game_tick+0x211c>
    ef9c:	00042023          	sw	zero,0(s0)
    efa0:	0204a783          	lw	a5,32(s1)
    efa4:	18078c63          	beqz	a5,f13c <game_tick+0x22b0>
    efa8:	00190913          	addi	s2,s2,1
    efac:	02040413          	addi	s0,s0,32
    efb0:	01299463          	bne	s3,s2,efb8 <game_tick+0x212c>
    efb4:	8ddfe06f          	j	d890 <game_tick+0xa04>
    efb8:	00042783          	lw	a5,0(s0)
    efbc:	fe0786e3          	beqz	a5,efa8 <game_tick+0x211c>
    efc0:	00442583          	lw	a1,4(s0)
    efc4:	00842603          	lw	a2,8(s0)
    efc8:	01442683          	lw	a3,20(s0)
    efcc:	00c42783          	lw	a5,12(s0)
    efd0:	01042703          	lw	a4,16(s0)
    efd4:	fff68693          	addi	a3,a3,-1
    efd8:	00b787b3          	add	a5,a5,a1
    efdc:	00c70733          	add	a4,a4,a2
    efe0:	00f42223          	sw	a5,4(s0)
    efe4:	00e42423          	sw	a4,8(s0)
    efe8:	00d42a23          	sw	a3,20(s0)
    efec:	f6d040e3          	bgtz	a3,ef4c <game_tick+0x20c0>
    eff0:	00042023          	sw	zero,0(s0)
    eff4:	fb5ff06f          	j	efa8 <game_tick+0x211c>
    eff8:	000017b7          	lui	a5,0x1
    effc:	e1078793          	addi	a5,a5,-496 # e10 <CUSTOM2+0xdb5>
    f000:	02f747b3          	div	a5,a4,a5
    f004:	00600713          	li	a4,6
    f008:	00178913          	addi	s2,a5,1
    f00c:	0b275663          	bge	a4,s2,f0b8 <game_tick+0x222c>
    f010:	00600913          	li	s2,6
    f014:	00500a13          	li	s4,5
    f018:	00300a93          	li	s5,3
    f01c:	01c0006f          	j	f038 <game_tick+0x21ac>
    f020:	00f4a223          	sw	a5,4(s1)
    f024:	00400593          	li	a1,4
    f028:	00048513          	mv	a0,s1
    f02c:	00198993          	addi	s3,s3,1
    f030:	985fc0ef          	jal	b9b4 <spawn_enemy>
    f034:	bf390ce3          	beq	s2,s3,ec2c <game_tick+0x1da0>
    f038:	0644a783          	lw	a5,100(s1)
    f03c:	be87d8e3          	bge	a5,s0,ec2c <game_tick+0x1da0>
    f040:	0044a703          	lw	a4,4(s1)
    f044:	00d71793          	slli	a5,a4,0xd
    f048:	00e7c7b3          	xor	a5,a5,a4
    f04c:	0117d713          	srli	a4,a5,0x11
    f050:	00f74733          	xor	a4,a4,a5
    f054:	00571793          	slli	a5,a4,0x5
    f058:	00e7c7b3          	xor	a5,a5,a4
    f05c:	0347f733          	remu	a4,a5,s4
    f060:	fc0700e3          	beqz	a4,f020 <game_tick+0x2194>
    f064:	00d79713          	slli	a4,a5,0xd
    f068:	00f747b3          	xor	a5,a4,a5
    f06c:	0117d713          	srli	a4,a5,0x11
    f070:	00f74733          	xor	a4,a4,a5
    f074:	00571793          	slli	a5,a4,0x5
    f078:	00e7c7b3          	xor	a5,a5,a4
    f07c:	0347f733          	remu	a4,a5,s4
    f080:	00071863          	bnez	a4,f090 <game_tick+0x2204>
    f084:	00f4a223          	sw	a5,4(s1)
    f088:	00200593          	li	a1,2
    f08c:	f9dff06f          	j	f028 <game_tick+0x219c>
    f090:	00d79713          	slli	a4,a5,0xd
    f094:	00f747b3          	xor	a5,a4,a5
    f098:	0117d713          	srli	a4,a5,0x11
    f09c:	00f74733          	xor	a4,a4,a5
    f0a0:	00571793          	slli	a5,a4,0x5
    f0a4:	00e7c7b3          	xor	a5,a5,a4
    f0a8:	0357f5b3          	remu	a1,a5,s5
    f0ac:	00f4a223          	sw	a5,4(s1)
    f0b0:	0015b593          	seqz	a1,a1
    f0b4:	f75ff06f          	j	f028 <game_tick+0x219c>
    f0b8:	f5204ee3          	bgtz	s2,f014 <game_tick+0x2188>
    f0bc:	00100913          	li	s2,1
    f0c0:	f55ff06f          	j	f014 <game_tick+0x2188>
    f0c4:	00400913          	li	s2,4
    f0c8:	00a00a13          	li	s4,10
    f0cc:	06078263          	beqz	a5,f130 <game_tick+0x22a4>
    f0d0:	0644a783          	lw	a5,100(s1)
    f0d4:	0487de63          	bge	a5,s0,f130 <game_tick+0x22a4>
    f0d8:	0044a783          	lw	a5,4(s1)
    f0dc:	00400593          	li	a1,4
    f0e0:	00d79713          	slli	a4,a5,0xd
    f0e4:	00e7c7b3          	xor	a5,a5,a4
    f0e8:	0117d713          	srli	a4,a5,0x11
    f0ec:	00e7c7b3          	xor	a5,a5,a4
    f0f0:	00579713          	slli	a4,a5,0x5
    f0f4:	00e7c7b3          	xor	a5,a5,a4
    f0f8:	0347f733          	remu	a4,a5,s4
    f0fc:	00f4a223          	sw	a5,4(s1)
    f100:	00070463          	beqz	a4,f108 <game_tick+0x227c>
    f104:	00372593          	slti	a1,a4,3
    f108:	00048513          	mv	a0,s1
    f10c:	8a9fc0ef          	jal	b9b4 <spawn_enemy>
    f110:	00050863          	beqz	a0,f120 <game_tick+0x2294>
    f114:	0744a783          	lw	a5,116(s1)
    f118:	fff78793          	addi	a5,a5,-1
    f11c:	06f4aa23          	sw	a5,116(s1)
    f120:	fff90913          	addi	s2,s2,-1
    f124:	00090663          	beqz	s2,f130 <game_tick+0x22a4>
    f128:	0744a783          	lw	a5,116(s1)
    f12c:	fa1ff06f          	j	f0cc <game_tick+0x2240>
    f130:	00c00793          	li	a5,12
    f134:	06f4ac23          	sw	a5,120(s1)
    f138:	aadff06f          	j	ebe4 <game_tick+0x1d58>
    f13c:	01842703          	lw	a4,24(s0)
    f140:	0184a783          	lw	a5,24(s1)
    f144:	00100693          	li	a3,1
    f148:	00048513          	mv	a0,s1
    f14c:	40e787b3          	sub	a5,a5,a4
    f150:	00f4ac23          	sw	a5,24(s1)
    f154:	02d00793          	li	a5,45
    f158:	02f4a023          	sw	a5,32(s1)
    f15c:	00a00793          	li	a5,10
    f160:	06f4a623          	sw	a5,108(s1)
    f164:	00a00713          	li	a4,10
    f168:	c54fc0ef          	jal	b5bc <particles>
    f16c:	e3dff06f          	j	efa8 <game_tick+0x211c>
    f170:	000037b7          	lui	a5,0x3
    f174:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    f178:	00f487b3          	add	a5,s1,a5
    f17c:	40000993          	li	s3,1024
    f180:	dadff06f          	j	ef2c <game_tick+0x20a0>
    f184:	000c1463          	bnez	s8,f18c <game_tick+0x2300>
    f188:	f08fe06f          	j	d890 <game_tick+0xa04>
    f18c:	01802733          	sgtz	a4,s8
    f190:	40e00733          	neg	a4,a4
    f194:	00e7f7b3          	and	a5,a5,a4
    f198:	8e1ff06f          	j	ea78 <game_tick+0x1bec>
    f19c:	01842703          	lw	a4,24(s0)
    f1a0:	0184a783          	lw	a5,24(s1)
    f1a4:	00100693          	li	a3,1
    f1a8:	00048513          	mv	a0,s1
    f1ac:	40e787b3          	sub	a5,a5,a4
    f1b0:	00f4ac23          	sw	a5,24(s1)
    f1b4:	02d00793          	li	a5,45
    f1b8:	02f4a023          	sw	a5,32(s1)
    f1bc:	00a00793          	li	a5,10
    f1c0:	06f4a623          	sw	a5,108(s1)
    f1c4:	00a00713          	li	a4,10
    f1c8:	bf4fc0ef          	jal	b5bc <particles>
    f1cc:	949ff06f          	j	eb14 <game_tick+0x1c88>
    f1d0:	00001937          	lui	s2,0x1
    f1d4:	00899993          	slli	s3,s3,0x8
    f1d8:	40090913          	addi	s2,s2,1024 # 1400 <strlen+0x7c>
    f1dc:	a95fe06f          	j	dc70 <game_tick+0xde4>
    f1e0:	000049b7          	lui	s3,0x4
    f1e4:	00891913          	slli	s2,s2,0x8
    f1e8:	40098993          	addi	s3,s3,1024 # 4400 <__stack_size+0x400>
    f1ec:	a85fe06f          	j	dc70 <game_tick+0xde4>
    f1f0:	00027937          	lui	s2,0x27
    f1f4:	00899993          	slli	s3,s3,0x8
    f1f8:	c0090913          	addi	s2,s2,-1024 # 26c00 <background_pixels+0xed40>
    f1fc:	a75fe06f          	j	dc70 <game_tick+0xde4>
    f200:	000037b7          	lui	a5,0x3
    f204:	5b878793          	addi	a5,a5,1464 # 35b8 <render_init+0x250>
    f208:	00f487b3          	add	a5,s1,a5
    f20c:	00001c37          	lui	s8,0x1
    f210:	871ff06f          	j	ea80 <game_tick+0x1bf4>
    f214:	00000793          	li	a5,0
    f218:	d0c04ae3          	bgtz	a2,ef2c <game_tick+0x20a0>
    f21c:	e74fe06f          	j	d890 <game_tick+0xa04>
    f220:	0644a783          	lw	a5,100(s1)
    f224:	a087d4e3          	bge	a5,s0,ec2c <game_tick+0x1da0>
    f228:	0044a703          	lw	a4,4(s1)
    f22c:	00300593          	li	a1,3
    f230:	00048513          	mv	a0,s1
    f234:	00d71793          	slli	a5,a4,0xd
    f238:	00e7c7b3          	xor	a5,a5,a4
    f23c:	0117d713          	srli	a4,a5,0x11
    f240:	00e7c7b3          	xor	a5,a5,a4
    f244:	00579713          	slli	a4,a5,0x5
    f248:	00e7c7b3          	xor	a5,a5,a4
    f24c:	02b7f5b3          	remu	a1,a5,a1
    f250:	00f4a223          	sw	a5,4(s1)
    f254:	0015b593          	seqz	a1,a1
    f258:	f5cfc0ef          	jal	b9b4 <spawn_enemy>
    f25c:	9d1ff06f          	j	ec2c <game_tick+0x1da0>
    f260:	00002783          	lw	a5,0(zero) # 0 <regnum_x0>
    f264:	00100073          	ebreak
    f268:	01002783          	lw	a5,16(zero) # 10 <regnum_a6>
    f26c:	00100073          	ebreak
    f270:	01402783          	lw	a5,20(zero) # 14 <regnum_s4>
    f274:	00100073          	ebreak

0000f278 <game_upgrade_name>:
    f278:	00500793          	li	a5,5
    f27c:	00a7ee63          	bltu	a5,a0,f298 <game_upgrade_name+0x20>
    f280:	000137b7          	lui	a5,0x13
    f284:	00251513          	slli	a0,a0,0x2
    f288:	0cc78793          	addi	a5,a5,204 # 130cc <names.1>
    f28c:	00a787b3          	add	a5,a5,a0
    f290:	0007a503          	lw	a0,0(a5)
    f294:	00008067          	ret
    f298:	00012537          	lui	a0,0x12
    f29c:	ee050513          	addi	a0,a0,-288 # 11ee0 <_data+0x7a4>
    f2a0:	00008067          	ret

0000f2a4 <game_upgrade_line>:
    f2a4:	00500793          	li	a5,5
    f2a8:	02a7e863          	bltu	a5,a0,f2d8 <game_upgrade_line+0x34>
    f2ac:	00200793          	li	a5,2
    f2b0:	02b7e463          	bltu	a5,a1,f2d8 <game_upgrade_line+0x34>
    f2b4:	00151793          	slli	a5,a0,0x1
    f2b8:	00a787b3          	add	a5,a5,a0
    f2bc:	00b787b3          	add	a5,a5,a1
    f2c0:	00013737          	lui	a4,0x13
    f2c4:	08470713          	addi	a4,a4,132 # 13084 <lines.0>
    f2c8:	00279793          	slli	a5,a5,0x2
    f2cc:	00f707b3          	add	a5,a4,a5
    f2d0:	0007a503          	lw	a0,0(a5)
    f2d4:	00008067          	ret
    f2d8:	00012537          	lui	a0,0x12
    f2dc:	ee050513          	addi	a0,a0,-288 # 11ee0 <_data+0x7a4>
    f2e0:	00008067          	ret

0000f2e4 <overlay.isra.0>:
    f2e4:	f5010113          	addi	sp,sp,-176
    f2e8:	09712623          	sw	s7,140(sp)
    f2ec:	00078b93          	mv	s7,a5
    f2f0:	000017b7          	lui	a5,0x1
    f2f4:	0a912223          	sw	s1,164(sp)
    f2f8:	0b212023          	sw	s2,160(sp)
    f2fc:	09512a23          	sw	s5,148(sp)
    f300:	09612823          	sw	s6,144(sp)
    f304:	00058493          	mv	s1,a1
    f308:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    f30c:	00000593          	li	a1,0
    f310:	00060b13          	mv	s6,a2
    f314:	00068a93          	mv	s5,a3
    f318:	00000613          	li	a2,0
    f31c:	28000693          	li	a3,640
    f320:	00070913          	mv	s2,a4
    f324:	02800713          	li	a4,40
    f328:	0a112623          	sw	ra,172(sp)
    f32c:	0a812423          	sw	s0,168(sp)
    f330:	09312e23          	sw	s3,156(sp)
    f334:	00050413          	mv	s0,a0
    f338:	00088993          	mv	s3,a7
    f33c:	09412c23          	sw	s4,152(sp)
    f340:	09812423          	sw	s8,136(sp)
    f344:	09912223          	sw	s9,132(sp)
    f348:	00080c13          	mv	s8,a6
    f34c:	09a12023          	sw	s10,128(sp)
    f350:	07b12e23          	sw	s11,124(sp)
    f354:	0b012a03          	lw	s4,176(sp)
    f358:	0b412d03          	lw	s10,180(sp)
    f35c:	0b812c83          	lw	s9,184(sp)
    f360:	97df90ef          	jal	8cdc <gfx_fill_rect>
    f364:	0d012683          	lw	a3,208(sp)
    f368:	0000f7b7          	lui	a5,0xf
    f36c:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    f370:	00200713          	li	a4,2
    f374:	00600613          	li	a2,6
    f378:	00c00593          	li	a1,12
    f37c:	00040513          	mv	a0,s0
    f380:	aa8fa0ef          	jal	9628 <gfx_text>
    f384:	0d412683          	lw	a3,212(sp)
    f388:	000097b7          	lui	a5,0x9
    f38c:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    f390:	00100713          	li	a4,1
    f394:	01b00613          	li	a2,27
    f398:	00c00593          	li	a1,12
    f39c:	00040513          	mv	a0,s0
    f3a0:	a88fa0ef          	jal	9628 <gfx_text>
    f3a4:	00012637          	lui	a2,0x12
    f3a8:	00048693          	mv	a3,s1
    f3ac:	88060613          	addi	a2,a2,-1920 # 11880 <_data+0x144>
    f3b0:	06000593          	li	a1,96
    f3b4:	01010513          	addi	a0,sp,16
    f3b8:	7cc010ef          	jal	10b84 <mini_snprintf>
    f3bc:	00200593          	li	a1,2
    f3c0:	01010513          	addi	a0,sp,16
    f3c4:	d2cfa0ef          	jal	98f0 <gfx_text_width>
    f3c8:	0000b7b7          	lui	a5,0xb
    f3cc:	27400593          	li	a1,628
    f3d0:	03100713          	li	a4,49
    f3d4:	00050d93          	mv	s11,a0
    f3d8:	40a585b3          	sub	a1,a1,a0
    f3dc:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    f3e0:	00974a63          	blt	a4,s1,f3f4 <overlay.isra.0+0x110>
    f3e4:	0000f7b7          	lui	a5,0xf
    f3e8:	01d00713          	li	a4,29
    f3ec:	b8e78793          	addi	a5,a5,-1138 # eb8e <game_tick+0x1d02>
    f3f0:	24974c63          	blt	a4,s1,f648 <overlay.isra.0+0x364>
    f3f4:	00200713          	li	a4,2
    f3f8:	01010693          	addi	a3,sp,16
    f3fc:	00600613          	li	a2,6
    f400:	00040513          	mv	a0,s0
    f404:	a24fa0ef          	jal	9628 <gfx_text>
    f408:	22e00593          	li	a1,558
    f40c:	41b585b3          	sub	a1,a1,s11
    f410:	1e0a0063          	beqz	s4,f5f0 <overlay.isra.0+0x30c>
    f414:	0000b7b7          	lui	a5,0xb
    f418:	000116b7          	lui	a3,0x11
    f41c:	75c68693          	addi	a3,a3,1884 # 1175c <_data+0x20>
    f420:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    f424:	00100713          	li	a4,1
    f428:	00a00613          	li	a2,10
    f42c:	00040513          	mv	a0,s0
    f430:	9f8fa0ef          	jal	9628 <gfx_text>
    f434:	00013637          	lui	a2,0x13
    f438:	000a8693          	mv	a3,s5
    f43c:	80460613          	addi	a2,a2,-2044 # 12804 <_data+0x10c8>
    f440:	06000593          	li	a1,96
    f444:	01010513          	addi	a0,sp,16
    f448:	73c010ef          	jal	10b84 <mini_snprintf>
    f44c:	00100593          	li	a1,1
    f450:	01010513          	addi	a0,sp,16
    f454:	c9cfa0ef          	jal	98f0 <gfx_text_width>
    f458:	0000f7b7          	lui	a5,0xf
    f45c:	27400593          	li	a1,628
    f460:	40a585b3          	sub	a1,a1,a0
    f464:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    f468:	00100713          	li	a4,1
    f46c:	01010693          	addi	a3,sp,16
    f470:	01b00613          	li	a2,27
    f474:	00040513          	mv	a0,s0
    f478:	9b0fa0ef          	jal	9628 <gfx_text>
    f47c:	000017b7          	lui	a5,0x1
    f480:	8c378793          	addi	a5,a5,-1853 # 8c3 <CUSTOM2+0x868>
    f484:	02c00713          	li	a4,44
    f488:	28000693          	li	a3,640
    f48c:	1b400613          	li	a2,436
    f490:	00000593          	li	a1,0
    f494:	00040513          	mv	a0,s0
    f498:	845f90ef          	jal	8cdc <gfx_fill_rect>
    f49c:	1da05863          	blez	s10,f66c <overlay.isra.0+0x388>
    f4a0:	0bc12783          	lw	a5,188(sp)
    f4a4:	3e800713          	li	a4,1000
    f4a8:	00013637          	lui	a2,0x13
    f4ac:	02e7e833          	rem	a6,a5,a4
    f4b0:	06400793          	li	a5,100
    f4b4:	000d0693          	mv	a3,s10
    f4b8:	81060613          	addi	a2,a2,-2032 # 12810 <_data+0x10d4>
    f4bc:	06000593          	li	a1,96
    f4c0:	01010513          	addi	a0,sp,16
    f4c4:	02f847b3          	div	a5,a6,a5
    f4c8:	0bc12803          	lw	a6,188(sp)
    f4cc:	02e84733          	div	a4,a6,a4
    f4d0:	6b4010ef          	jal	10b84 <mini_snprintf>
    f4d4:	0000b7b7          	lui	a5,0xb
    f4d8:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    f4dc:	000a0663          	beqz	s4,f4e8 <overlay.isra.0+0x204>
    f4e0:	000097b7          	lui	a5,0x9
    f4e4:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    f4e8:	00200713          	li	a4,2
    f4ec:	01010693          	addi	a3,sp,16
    f4f0:	1bc00613          	li	a2,444
    f4f4:	00c00593          	li	a1,12
    f4f8:	00040513          	mv	a0,s0
    f4fc:	92cfa0ef          	jal	9628 <gfx_text>
    f500:	15905a63          	blez	s9,f654 <overlay.isra.0+0x370>
    f504:	0c012783          	lw	a5,192(sp)
    f508:	3e800713          	li	a4,1000
    f50c:	00013637          	lui	a2,0x13
    f510:	02e7e833          	rem	a6,a5,a4
    f514:	06400793          	li	a5,100
    f518:	000c8693          	mv	a3,s9
    f51c:	83060613          	addi	a2,a2,-2000 # 12830 <_data+0x10f4>
    f520:	06000593          	li	a1,96
    f524:	01010513          	addi	a0,sp,16
    f528:	02f847b3          	div	a5,a6,a5
    f52c:	0c012803          	lw	a6,192(sp)
    f530:	02e84733          	div	a4,a6,a4
    f534:	650010ef          	jal	10b84 <mini_snprintf>
    f538:	0c0a0663          	beqz	s4,f604 <overlay.isra.0+0x320>
    f53c:	0000b7b7          	lui	a5,0xb
    f540:	70d78793          	addi	a5,a5,1805 # b70d <check_level+0x61>
    f544:	00200713          	li	a4,2
    f548:	01010693          	addi	a3,sp,16
    f54c:	1bc00613          	li	a2,444
    f550:	14a00593          	li	a1,330
    f554:	00040513          	mv	a0,s0
    f558:	8d0fa0ef          	jal	9628 <gfx_text>
    f55c:	0cc12783          	lw	a5,204(sp)
    f560:	0c412883          	lw	a7,196(sp)
    f564:	00013637          	lui	a2,0x13
    f568:	00f12223          	sw	a5,4(sp)
    f56c:	0c812783          	lw	a5,200(sp)
    f570:	00098813          	mv	a6,s3
    f574:	000b8713          	mv	a4,s7
    f578:	00f12023          	sw	a5,0(sp)
    f57c:	00090693          	mv	a3,s2
    f580:	000c0793          	mv	a5,s8
    f584:	85060613          	addi	a2,a2,-1968 # 12850 <_data+0x1114>
    f588:	06000593          	li	a1,96
    f58c:	01010513          	addi	a0,sp,16
    f590:	5f4010ef          	jal	10b84 <mini_snprintf>
    f594:	000097b7          	lui	a5,0x9
    f598:	01010693          	addi	a3,sp,16
    f59c:	00040513          	mv	a0,s0
    f5a0:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    f5a4:	00100713          	li	a4,1
    f5a8:	1d200613          	li	a2,466
    f5ac:	00c00593          	li	a1,12
    f5b0:	878fa0ef          	jal	9628 <gfx_text>
    f5b4:	0ac12083          	lw	ra,172(sp)
    f5b8:	0a812403          	lw	s0,168(sp)
    f5bc:	0a412483          	lw	s1,164(sp)
    f5c0:	0a012903          	lw	s2,160(sp)
    f5c4:	09c12983          	lw	s3,156(sp)
    f5c8:	09812a03          	lw	s4,152(sp)
    f5cc:	09412a83          	lw	s5,148(sp)
    f5d0:	09012b03          	lw	s6,144(sp)
    f5d4:	08c12b83          	lw	s7,140(sp)
    f5d8:	08812c03          	lw	s8,136(sp)
    f5dc:	08412c83          	lw	s9,132(sp)
    f5e0:	08012d03          	lw	s10,128(sp)
    f5e4:	07c12d83          	lw	s11,124(sp)
    f5e8:	0b010113          	addi	sp,sp,176
    f5ec:	00008067          	ret
    f5f0:	0000f7b7          	lui	a5,0xf
    f5f4:	000116b7          	lui	a3,0x11
    f5f8:	76468693          	addi	a3,a3,1892 # 11764 <_data+0x28>
    f5fc:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    f600:	e25ff06f          	j	f424 <overlay.isra.0+0x140>
    f604:	000097b7          	lui	a5,0x9
    f608:	01010693          	addi	a3,sp,16
    f60c:	00040513          	mv	a0,s0
    f610:	51378793          	addi	a5,a5,1299 # 9513 <gfx_blit_alpha+0x3af>
    f614:	00200713          	li	a4,2
    f618:	1bc00613          	li	a2,444
    f61c:	14a00593          	li	a1,330
    f620:	808fa0ef          	jal	9628 <gfx_text>
    f624:	00013637          	lui	a2,0x13
    f628:	00098793          	mv	a5,s3
    f62c:	00090713          	mv	a4,s2
    f630:	000b0693          	mv	a3,s6
    f634:	89860613          	addi	a2,a2,-1896 # 12898 <_data+0x115c>
    f638:	06000593          	li	a1,96
    f63c:	01010513          	addi	a0,sp,16
    f640:	544010ef          	jal	10b84 <mini_snprintf>
    f644:	f51ff06f          	j	f594 <overlay.isra.0+0x2b0>
    f648:	0000f7b7          	lui	a5,0xf
    f64c:	5eb78793          	addi	a5,a5,1515 # f5eb <overlay.isra.0+0x307>
    f650:	da5ff06f          	j	f3f4 <overlay.isra.0+0x110>
    f654:	00013637          	lui	a2,0x13
    f658:	84460613          	addi	a2,a2,-1980 # 12844 <_data+0x1108>
    f65c:	06000593          	li	a1,96
    f660:	01010513          	addi	a0,sp,16
    f664:	520010ef          	jal	10b84 <mini_snprintf>
    f668:	ed1ff06f          	j	f538 <overlay.isra.0+0x254>
    f66c:	00013637          	lui	a2,0x13
    f670:	82460613          	addi	a2,a2,-2012 # 12824 <_data+0x10e8>
    f674:	06000593          	li	a1,96
    f678:	01010513          	addi	a0,sp,16
    f67c:	508010ef          	jal	10b84 <mini_snprintf>
    f680:	e55ff06f          	j	f4d4 <overlay.isra.0+0x1f0>

0000f684 <demo_init>:
    f684:	fa010113          	addi	sp,sp,-96
    f688:	05312623          	sw	s3,76(sp)
    f68c:	05412423          	sw	s4,72(sp)
    f690:	00028837          	lui	a6,0x28
    f694:	00013a37          	lui	s4,0x13
    f698:	000d49b7          	lui	s3,0xd4
    f69c:	05212823          	sw	s2,80(sp)
    f6a0:	00050e93          	mv	t4,a0
    f6a4:	00050913          	mv	s2,a0
    f6a8:	04112e23          	sw	ra,92(sp)
    f6ac:	04812c23          	sw	s0,88(sp)
    f6b0:	04912a23          	sw	s1,84(sp)
    f6b4:	05512223          	sw	s5,68(sp)
    f6b8:	05612023          	sw	s6,64(sp)
    f6bc:	03712e23          	sw	s7,60(sp)
    f6c0:	03812c23          	sw	s8,56(sp)
    f6c4:	03912a23          	sw	s9,52(sp)
    f6c8:	1c4a0a13          	addi	s4,s4,452 # 131c4 <names.1>
    f6cc:	12498993          	addi	s3,s3,292 # d4124 <images>
    f6d0:	00000513          	li	a0,0
    f6d4:	00000e13          	li	t3,0
    f6d8:	1df00f93          	li	t6,479
    f6dc:	27f00313          	li	t1,639
    f6e0:	ba000893          	li	a7,-1120
    f6e4:	d8080813          	addi	a6,a6,-640 # 27d80 <background_pixels+0xfec0>
    f6e8:	1e000f13          	li	t5,480
    f6ec:	03f545b3          	div	a1,a0,t6
    f6f0:	000e8613          	mv	a2,t4
    f6f4:	00000693          	li	a3,0
    f6f8:	4025d593          	srai	a1,a1,0x2
    f6fc:	00559593          	slli	a1,a1,0x5
    f700:	01059593          	slli	a1,a1,0x10
    f704:	4105d593          	srai	a1,a1,0x10
    f708:	0266c7b3          	div	a5,a3,t1
    f70c:	00d50733          	add	a4,a0,a3
    f710:	00260613          	addi	a2,a2,2
    f714:	0ff68693          	addi	a3,a3,255
    f718:	03174733          	div	a4,a4,a7
    f71c:	4037d793          	srai	a5,a5,0x3
    f720:	00b79793          	slli	a5,a5,0xb
    f724:	00f5e7b3          	or	a5,a1,a5
    f728:	0ff70713          	addi	a4,a4,255
    f72c:	40375713          	srai	a4,a4,0x3
    f730:	00e7e7b3          	or	a5,a5,a4
    f734:	fef61f23          	sh	a5,-2(a2)
    f738:	fd0698e3          	bne	a3,a6,f708 <demo_init+0x84>
    f73c:	001e0e13          	addi	t3,t3,1 # 40000001 <__freertos_irq_stack_top+0x3ff17e61>
    f740:	0ff50513          	addi	a0,a0,255
    f744:	500e8e93          	addi	t4,t4,1280
    f748:	fbee12e3          	bne	t3,t5,f6ec <demo_init+0x68>
    f74c:	000014b7          	lui	s1,0x1
    f750:	0000f437          	lui	s0,0xf
    f754:	00000a93          	li	s5,0
    f758:	8c348493          	addi	s1,s1,-1853 # 8c3 <CUSTOM2+0x868>
    f75c:	77a40413          	addi	s0,s0,1914 # f77a <demo_init+0xf6>
    f760:	00200c37          	lui	s8,0x200
    f764:	00300b93          	li	s7,3
    f768:	00100b13          	li	s6,1
    f76c:	00090593          	mv	a1,s2
    f770:	1e000693          	li	a3,480
    f774:	28000613          	li	a2,640
    f778:	00810513          	addi	a0,sp,8
    f77c:	cd8f90ef          	jal	8c54 <gfx_begin>
    f780:	00048793          	mv	a5,s1
    f784:	06400713          	li	a4,100
    f788:	12c00693          	li	a3,300
    f78c:	0be00613          	li	a2,190
    f790:	0aa00593          	li	a1,170
    f794:	00810513          	addi	a0,sp,8
    f798:	d44f90ef          	jal	8cdc <gfx_fill_rect>
    f79c:	000a2c83          	lw	s9,0(s4)
    f7a0:	00700593          	li	a1,7
    f7a4:	001a8a93          	addi	s5,s5,1
    f7a8:	000c8513          	mv	a0,s9
    f7ac:	944fa0ef          	jal	98f0 <gfx_text_width>
    f7b0:	01f55793          	srli	a5,a0,0x1f
    f7b4:	00a787b3          	add	a5,a5,a0
    f7b8:	4017d593          	srai	a1,a5,0x1
    f7bc:	40b005b3          	neg	a1,a1
    f7c0:	00040793          	mv	a5,s0
    f7c4:	00700713          	li	a4,7
    f7c8:	000c8693          	mv	a3,s9
    f7cc:	0d700613          	li	a2,215
    f7d0:	14058593          	addi	a1,a1,320
    f7d4:	00810513          	addi	a0,sp,8
    f7d8:	e51f90ef          	jal	9628 <gfx_text>
    f7dc:	00810513          	addi	a0,sp,8
    f7e0:	ce0f90ef          	jal	8cc0 <gfx_end>
    f7e4:	28000793          	li	a5,640
    f7e8:	00f9a023          	sw	a5,0(s3)
    f7ec:	1e000793          	li	a5,480
    f7f0:	0129a423          	sw	s2,8(s3)
    f7f4:	00f9a223          	sw	a5,4(s3)
    f7f8:	0009a623          	sw	zero,12(s3)
    f7fc:	01890933          	add	s2,s2,s8
    f800:	157a8a63          	beq	s5,s7,f954 <demo_init+0x2d0>
    f804:	076a8e63          	beq	s5,s6,f880 <demo_init+0x1fc>
    f808:	00090313          	mv	t1,s2
    f80c:	00000893          	li	a7,0
    f810:	00000593          	li	a1,0
    f814:	28000813          	li	a6,640
    f818:	1e000e13          	li	t3,480
    f81c:	4038d513          	srai	a0,a7,0x3
    f820:	01f57513          	andi	a0,a0,31
    f824:	00030613          	mv	a2,t1
    f828:	00000693          	li	a3,0
    f82c:	00b6c7b3          	xor	a5,a3,a1
    f830:	00169713          	slli	a4,a3,0x1
    f834:	4037d793          	srai	a5,a5,0x3
    f838:	01f7f793          	andi	a5,a5,31
    f83c:	00b74733          	xor	a4,a4,a1
    f840:	00b79793          	slli	a5,a5,0xb
    f844:	00371713          	slli	a4,a4,0x3
    f848:	00f567b3          	or	a5,a0,a5
    f84c:	7e077713          	andi	a4,a4,2016
    f850:	00e7e7b3          	or	a5,a5,a4
    f854:	00f61023          	sh	a5,0(a2)
    f858:	00168693          	addi	a3,a3,1
    f85c:	00260613          	addi	a2,a2,2
    f860:	fd0696e3          	bne	a3,a6,f82c <demo_init+0x1a8>
    f864:	00158593          	addi	a1,a1,1
    f868:	00388893          	addi	a7,a7,3
    f86c:	50030313          	addi	t1,t1,1280
    f870:	fbc596e3          	bne	a1,t3,f81c <demo_init+0x198>
    f874:	004a0a13          	addi	s4,s4,4
    f878:	01098993          	addi	s3,s3,16
    f87c:	ef1ff06f          	j	f76c <demo_init+0xe8>
    f880:	00002eb7          	lui	t4,0x2
    f884:	00002e37          	lui	t3,0x2
    f888:	fffff8b7          	lui	a7,0xfffff
    f88c:	fffff837          	lui	a6,0xfffff
    f890:	00090f13          	mv	t5,s2
    f894:	00000593          	li	a1,0
    f898:	de0e8e93          	addi	t4,t4,-544 # 1de0 <main+0x9d0>
    f89c:	9e0e0e13          	addi	t3,t3,-1568 # 19e0 <main+0x5d0>
    f8a0:	5e088893          	addi	a7,a7,1504 # fffff5e0 <__freertos_irq_stack_top+0xfff17440>
    f8a4:	1e080813          	addi	a6,a6,480 # fffff1e0 <__freertos_irq_stack_top+0xfff17040>
    f8a8:	28000313          	li	t1,640
    f8ac:	1e000f93          	li	t6,480
    f8b0:	f1058513          	addi	a0,a1,-240
    f8b4:	02a50533          	mul	a0,a0,a0
    f8b8:	000f0613          	mv	a2,t5
    f8bc:	00000693          	li	a3,0
    f8c0:	ec068793          	addi	a5,a3,-320
    f8c4:	00d5c733          	xor	a4,a1,a3
    f8c8:	40575713          	srai	a4,a4,0x5
    f8cc:	00177713          	andi	a4,a4,1
    f8d0:	00088393          	mv	t2,a7
    f8d4:	00080293          	mv	t0,a6
    f8d8:	02f787b3          	mul	a5,a5,a5
    f8dc:	00a787b3          	add	a5,a5,a0
    f8e0:	40a7d793          	srai	a5,a5,0xa
    f8e4:	0017f793          	andi	a5,a5,1
    f8e8:	04f70063          	beq	a4,a5,f928 <demo_init+0x2a4>
    f8ec:	00078463          	beqz	a5,f8f4 <demo_init+0x270>
    f8f0:	00038293          	mv	t0,t2
    f8f4:	00b2e793          	ori	a5,t0,11
    f8f8:	0192e293          	ori	t0,t0,25
    f8fc:	02070c63          	beqz	a4,f934 <demo_init+0x2b0>
    f900:	00f61023          	sh	a5,0(a2)
    f904:	00168693          	addi	a3,a3,1
    f908:	00260613          	addi	a2,a2,2
    f90c:	fa669ae3          	bne	a3,t1,f8c0 <demo_init+0x23c>
    f910:	00158593          	addi	a1,a1,1
    f914:	500f0f13          	addi	t5,t5,1280 # 40000500 <__freertos_irq_stack_top+0x3ff18360>
    f918:	f9f59ce3          	bne	a1,t6,f8b0 <demo_init+0x22c>
    f91c:	004a0a13          	addi	s4,s4,4
    f920:	01098993          	addi	s3,s3,16
    f924:	e49ff06f          	j	f76c <demo_init+0xe8>
    f928:	000e0293          	mv	t0,t3
    f92c:	000e8393          	mv	t2,t4
    f930:	fbdff06f          	j	f8ec <demo_init+0x268>
    f934:	00561023          	sh	t0,0(a2)
    f938:	00168693          	addi	a3,a3,1
    f93c:	00260613          	addi	a2,a2,2
    f940:	f86690e3          	bne	a3,t1,f8c0 <demo_init+0x23c>
    f944:	00158593          	addi	a1,a1,1
    f948:	500f0f13          	addi	t5,t5,1280
    f94c:	f7f592e3          	bne	a1,t6,f8b0 <demo_init+0x22c>
    f950:	fcdff06f          	j	f91c <demo_init+0x298>
    f954:	000d45b7          	lui	a1,0xd4
    f958:	82c1a503          	lw	a0,-2004(gp) # 13434 <rng>
    f95c:	00010837          	lui	a6,0x10
    f960:	15458593          	addi	a1,a1,340 # d4154 <squares>
    f964:	00013337          	lui	t1,0x13
    f968:	01058fb3          	add	t6,a1,a6
    f96c:	1b430313          	addi	t1,t1,436 # 131b4 <pal.0>
    f970:	02300f13          	li	t5,35
    f974:	28000e93          	li	t4,640
    f978:	18c00e13          	li	t3,396
    f97c:	06100893          	li	a7,97
    f980:	fff80813          	addi	a6,a6,-1 # ffff <mini_vsnprintf+0x113>
    f984:	00d51793          	slli	a5,a0,0xd
    f988:	00a7c7b3          	xor	a5,a5,a0
    f98c:	0117d713          	srli	a4,a5,0x11
    f990:	00f74733          	xor	a4,a4,a5
    f994:	00571793          	slli	a5,a4,0x5
    f998:	00e7c7b3          	xor	a5,a5,a4
    f99c:	03e7f3b3          	remu	t2,a5,t5
    f9a0:	00d79713          	slli	a4,a5,0xd
    f9a4:	00f74733          	xor	a4,a4,a5
    f9a8:	01175793          	srli	a5,a4,0x11
    f9ac:	00e7c7b3          	xor	a5,a5,a4
    f9b0:	00579613          	slli	a2,a5,0x5
    f9b4:	00f64633          	xor	a2,a2,a5
    f9b8:	00d61713          	slli	a4,a2,0xd
    f9bc:	00c74733          	xor	a4,a4,a2
    f9c0:	01175793          	srli	a5,a4,0x11
    f9c4:	00e7c7b3          	xor	a5,a5,a4
    f9c8:	00579713          	slli	a4,a5,0x5
    f9cc:	00f74733          	xor	a4,a4,a5
    f9d0:	00d71793          	slli	a5,a4,0xd
    f9d4:	00e7c7b3          	xor	a5,a5,a4
    f9d8:	0117d693          	srli	a3,a5,0x11
    f9dc:	00f6c6b3          	xor	a3,a3,a5
    f9e0:	00569793          	slli	a5,a3,0x5
    f9e4:	00d7c7b3          	xor	a5,a5,a3
    f9e8:	00d79693          	slli	a3,a5,0xd
    f9ec:	00f6c6b3          	xor	a3,a3,a5
    f9f0:	0116d513          	srli	a0,a3,0x11
    f9f4:	00d54533          	xor	a0,a0,a3
    f9f8:	00551693          	slli	a3,a0,0x5
    f9fc:	00a6c6b3          	xor	a3,a3,a0
    fa00:	00d69413          	slli	s0,a3,0xd
    fa04:	00d44433          	xor	s0,s0,a3
    fa08:	01145513          	srli	a0,s0,0x11
    fa0c:	00854533          	xor	a0,a0,s0
    fa10:	00757413          	andi	s0,a0,7
    fa14:	00141413          	slli	s0,s0,0x1
    fa18:	00830433          	add	s0,t1,s0
    fa1c:	00045983          	lhu	s3,0(s0)
    fa20:	01058593          	addi	a1,a1,16
    fa24:	0317f7b3          	remu	a5,a5,a7
    fa28:	00638393          	addi	t2,t2,6
    fa2c:	407e04b3          	sub	s1,t3,t2
    fa30:	407e8433          	sub	s0,t4,t2
    fa34:	fe759e23          	sh	t2,-4(a1)
    fa38:	ff359f23          	sh	s3,-2(a1)
    fa3c:	00551913          	slli	s2,a0,0x5
    fa40:	00a94533          	xor	a0,s2,a0
    fa44:	0316f6b3          	remu	a3,a3,a7
    fa48:	fd078793          	addi	a5,a5,-48
    fa4c:	01079793          	slli	a5,a5,0x10
    fa50:	0107d793          	srli	a5,a5,0x10
    fa54:	0107f7b3          	and	a5,a5,a6
    fa58:	02977733          	remu	a4,a4,s1
    fa5c:	fd068693          	addi	a3,a3,-48
    fa60:	01069693          	slli	a3,a3,0x10
    fa64:	00d7e7b3          	or	a5,a5,a3
    fa68:	fef5ac23          	sw	a5,-8(a1)
    fa6c:	02867633          	remu	a2,a2,s0
    fa70:	02870793          	addi	a5,a4,40
    fa74:	00479793          	slli	a5,a5,0x4
    fa78:	fef5aa23          	sw	a5,-12(a1)
    fa7c:	00461793          	slli	a5,a2,0x4
    fa80:	fef5a823          	sw	a5,-16(a1)
    fa84:	f0bf90e3          	bne	t6,a1,f984 <demo_init+0x300>
    fa88:	05c12083          	lw	ra,92(sp)
    fa8c:	05812403          	lw	s0,88(sp)
    fa90:	82a1a623          	sw	a0,-2004(gp) # 13434 <rng>
    fa94:	05412483          	lw	s1,84(sp)
    fa98:	05012903          	lw	s2,80(sp)
    fa9c:	04c12983          	lw	s3,76(sp)
    faa0:	04812a03          	lw	s4,72(sp)
    faa4:	04412a83          	lw	s5,68(sp)
    faa8:	04012b03          	lw	s6,64(sp)
    faac:	03c12b83          	lw	s7,60(sp)
    fab0:	03812c03          	lw	s8,56(sp)
    fab4:	03412c83          	lw	s9,52(sp)
    fab8:	06010113          	addi	sp,sp,96
    fabc:	00008067          	ret

0000fac0 <demo_squares_tick>:
    fac0:	000017b7          	lui	a5,0x1
    fac4:	00001fb7          	lui	t6,0x1
    fac8:	00a7c463          	blt	a5,a0,fad0 <demo_squares_tick+0x10>
    facc:	00050f93          	mv	t6,a0
    fad0:	0ea05863          	blez	a0,fbc0 <demo_squares_tick+0x100>
    fad4:	ff010113          	addi	sp,sp,-16
    fad8:	000d47b7          	lui	a5,0xd4
    fadc:	00812623          	sw	s0,12(sp)
    fae0:	00912423          	sw	s1,8(sp)
    fae4:	01212223          	sw	s2,4(sp)
    fae8:	15478793          	addi	a5,a5,340 # d4154 <squares>
    faec:	00000e93          	li	t4,0
    faf0:	28000493          	li	s1,640
    faf4:	1b400413          	li	s0,436
    faf8:	27f00393          	li	t2,639
    fafc:	50000293          	li	t0,1280
    fb00:	0047a603          	lw	a2,4(a5)
    fb04:	00879883          	lh	a7,8(a5)
    fb08:	0007a703          	lw	a4,0(a5)
    fb0c:	00a79803          	lh	a6,10(a5)
    fb10:	00c79683          	lh	a3,12(a5)
    fb14:	00e88733          	add	a4,a7,a4
    fb18:	00c805b3          	add	a1,a6,a2
    fb1c:	40d48533          	sub	a0,s1,a3
    fb20:	40d406b3          	sub	a3,s0,a3
    fb24:	00451513          	slli	a0,a0,0x4
    fb28:	00469693          	slli	a3,a3,0x4
    fb2c:	00e7a023          	sw	a4,0(a5)
    fb30:	00b7a223          	sw	a1,4(a5)
    fb34:	001e8e93          	addi	t4,t4,1
    fb38:	40b28f33          	sub	t5,t0,a1
    fb3c:	41000833          	neg	a6,a6
    fb40:	40e00933          	neg	s2,a4
    fb44:	411008b3          	neg	a7,a7
    fb48:	00058613          	mv	a2,a1
    fb4c:	00151e13          	slli	t3,a0,0x1
    fb50:	00169313          	slli	t1,a3,0x1
    fb54:	00075863          	bgez	a4,fb64 <demo_squares_tick+0xa4>
    fb58:	01179423          	sh	a7,8(a5)
    fb5c:	0127a023          	sw	s2,0(a5)
    fb60:	00090713          	mv	a4,s2
    fb64:	40ee0e33          	sub	t3,t3,a4
    fb68:	00e55a63          	bge	a0,a4,fb7c <demo_squares_tick+0xbc>
    fb6c:	0087d703          	lhu	a4,8(a5)
    fb70:	01c7a023          	sw	t3,0(a5)
    fb74:	40e00733          	neg	a4,a4
    fb78:	00e79423          	sh	a4,8(a5)
    fb7c:	00b3c863          	blt	t2,a1,fb8c <demo_squares_tick+0xcc>
    fb80:	01e7a223          	sw	t5,4(a5)
    fb84:	000f0613          	mv	a2,t5
    fb88:	01079523          	sh	a6,10(a5)
    fb8c:	40c30333          	sub	t1,t1,a2
    fb90:	00c6da63          	bge	a3,a2,fba4 <demo_squares_tick+0xe4>
    fb94:	00a7d703          	lhu	a4,10(a5)
    fb98:	0067a223          	sw	t1,4(a5)
    fb9c:	40e00733          	neg	a4,a4
    fba0:	00e79523          	sh	a4,10(a5)
    fba4:	01078793          	addi	a5,a5,16
    fba8:	f5fecce3          	blt	t4,t6,fb00 <demo_squares_tick+0x40>
    fbac:	00c12403          	lw	s0,12(sp)
    fbb0:	00812483          	lw	s1,8(sp)
    fbb4:	00412903          	lw	s2,4(sp)
    fbb8:	01010113          	addi	sp,sp,16
    fbbc:	00008067          	ret
    fbc0:	00008067          	ret

0000fbc4 <demo_squares_render>:
    fbc4:	f5010113          	addi	sp,sp,-176
    fbc8:	0a812423          	sw	s0,168(sp)
    fbcc:	0a912223          	sw	s1,164(sp)
    fbd0:	0b212023          	sw	s2,160(sp)
    fbd4:	09312e23          	sw	s3,156(sp)
    fbd8:	09412c23          	sw	s4,152(sp)
    fbdc:	0a112623          	sw	ra,172(sp)
    fbe0:	000017b7          	lui	a5,0x1
    fbe4:	0205a483          	lw	s1,32(a1)
    fbe8:	00058913          	mv	s2,a1
    fbec:	00060413          	mv	s0,a2
    fbf0:	00050993          	mv	s3,a0
    fbf4:	00001a37          	lui	s4,0x1
    fbf8:	00c7c463          	blt	a5,a2,fc00 <demo_squares_render+0x3c>
    fbfc:	00060a13          	mv	s4,a2
    fc00:	000027b7          	lui	a5,0x2
    fc04:	92478793          	addi	a5,a5,-1756 # 1924 <main+0x514>
    fc08:	18c00713          	li	a4,396
    fc0c:	28000693          	li	a3,640
    fc10:	02800613          	li	a2,40
    fc14:	00000593          	li	a1,0
    fc18:	00098513          	mv	a0,s3
    fc1c:	8c0f90ef          	jal	8cdc <gfx_fill_rect>
    fc20:	16805263          	blez	s0,fd84 <demo_squares_render+0x1c0>
    fc24:	000d4437          	lui	s0,0xd4
    fc28:	15440413          	addi	s0,s0,340 # d4154 <squares>
    fc2c:	06048263          	beqz	s1,fc90 <demo_squares_render+0xcc>
    fc30:	00000493          	li	s1,0
    fc34:	0140006f          	j	fc48 <demo_squares_render+0x84>
    fc38:	00148493          	addi	s1,s1,1
    fc3c:	8a0f90ef          	jal	8cdc <gfx_fill_rect>
    fc40:	01040413          	addi	s0,s0,16
    fc44:	0544d063          	bge	s1,s4,fc84 <demo_squares_render+0xc0>
    fc48:	00042583          	lw	a1,0(s0)
    fc4c:	00442603          	lw	a2,4(s0)
    fc50:	00c41703          	lh	a4,12(s0)
    fc54:	0014f813          	andi	a6,s1,1
    fc58:	00e45783          	lhu	a5,14(s0)
    fc5c:	00098513          	mv	a0,s3
    fc60:	4045d593          	srai	a1,a1,0x4
    fc64:	00070693          	mv	a3,a4
    fc68:	40465613          	srai	a2,a2,0x4
    fc6c:	fc0806e3          	beqz	a6,fc38 <demo_squares_render+0x74>
    fc70:	09600813          	li	a6,150
    fc74:	00148493          	addi	s1,s1,1
    fc78:	a3cf90ef          	jal	8eb4 <gfx_fill_alpha>
    fc7c:	01040413          	addi	s0,s0,16
    fc80:	fd44c4e3          	blt	s1,s4,fc48 <demo_squares_render+0x84>
    fc84:	00013737          	lui	a4,0x13
    fc88:	8e470713          	addi	a4,a4,-1820 # 128e4 <_data+0x11a8>
    fc8c:	03c0006f          	j	fcc8 <demo_squares_render+0x104>
    fc90:	00c41703          	lh	a4,12(s0)
    fc94:	00442603          	lw	a2,4(s0)
    fc98:	00042583          	lw	a1,0(s0)
    fc9c:	00e45783          	lhu	a5,14(s0)
    fca0:	00070693          	mv	a3,a4
    fca4:	40465613          	srai	a2,a2,0x4
    fca8:	4045d593          	srai	a1,a1,0x4
    fcac:	00098513          	mv	a0,s3
    fcb0:	00148493          	addi	s1,s1,1
    fcb4:	828f90ef          	jal	8cdc <gfx_fill_rect>
    fcb8:	01040413          	addi	s0,s0,16
    fcbc:	fd44cae3          	blt	s1,s4,fc90 <demo_squares_render+0xcc>
    fcc0:	00012737          	lui	a4,0x12
    fcc4:	ee070713          	addi	a4,a4,-288 # 11ee0 <_data+0x7a4>
    fcc8:	01c9a883          	lw	a7,28(s3)
    fccc:	0209a803          	lw	a6,32(s3)
    fcd0:	0149a783          	lw	a5,20(s3)
    fcd4:	03010413          	addi	s0,sp,48
    fcd8:	00013637          	lui	a2,0x13
    fcdc:	000a0693          	mv	a3,s4
    fce0:	8f860613          	addi	a2,a2,-1800 # 128f8 <_data+0x11bc>
    fce4:	06000593          	li	a1,96
    fce8:	00040513          	mv	a0,s0
    fcec:	699000ef          	jal	10b84 <mini_snprintf>
    fcf0:	04092303          	lw	t1,64(s2)
    fcf4:	03892503          	lw	a0,56(s2)
    fcf8:	06092383          	lw	t2,96(s2)
    fcfc:	05092283          	lw	t0,80(s2)
    fd00:	04c92f83          	lw	t6,76(s2)
    fd04:	04892f03          	lw	t5,72(s2)
    fd08:	04492e83          	lw	t4,68(s2)
    fd0c:	06492e03          	lw	t3,100(s2)
    fd10:	03492883          	lw	a7,52(s2)
    fd14:	03092803          	lw	a6,48(s2)
    fd18:	02c92783          	lw	a5,44(s2)
    fd1c:	02892703          	lw	a4,40(s2)
    fd20:	00c92683          	lw	a3,12(s2)
    fd24:	00892603          	lw	a2,8(s2)
    fd28:	00092583          	lw	a1,0(s2)
    fd2c:	00612223          	sw	t1,4(sp)
    fd30:	00013337          	lui	t1,0x13
    fd34:	00a12023          	sw	a0,0(sp)
    fd38:	00712c23          	sw	t2,24(sp)
    fd3c:	00512a23          	sw	t0,20(sp)
    fd40:	01f12823          	sw	t6,16(sp)
    fd44:	01e12623          	sw	t5,12(sp)
    fd48:	01d12423          	sw	t4,8(sp)
    fd4c:	00098513          	mv	a0,s3
    fd50:	92430313          	addi	t1,t1,-1756 # 12924 <_data+0x11e8>
    fd54:	02812223          	sw	s0,36(sp)
    fd58:	02612023          	sw	t1,32(sp)
    fd5c:	01c12e23          	sw	t3,28(sp)
    fd60:	d84ff0ef          	jal	f2e4 <overlay.isra.0>
    fd64:	0ac12083          	lw	ra,172(sp)
    fd68:	0a812403          	lw	s0,168(sp)
    fd6c:	0a412483          	lw	s1,164(sp)
    fd70:	0a012903          	lw	s2,160(sp)
    fd74:	09c12983          	lw	s3,156(sp)
    fd78:	09812a03          	lw	s4,152(sp)
    fd7c:	0b010113          	addi	sp,sp,176
    fd80:	00008067          	ret
    fd84:	f2048ee3          	beqz	s1,fcc0 <demo_squares_render+0xfc>
    fd88:	efdff06f          	j	fc84 <demo_squares_render+0xc0>

0000fd8c <demo_images_render>:
    fd8c:	f5010113          	addi	sp,sp,-176
    fd90:	0a812423          	sw	s0,168(sp)
    fd94:	0a912223          	sw	s1,164(sp)
    fd98:	0b212023          	sw	s2,160(sp)
    fd9c:	09312e23          	sw	s3,156(sp)
    fda0:	09412c23          	sw	s4,152(sp)
    fda4:	0a112623          	sw	ra,172(sp)
    fda8:	00068913          	mv	s2,a3
    fdac:	00050a13          	mv	s4,a0
    fdb0:	00058413          	mv	s0,a1
    fdb4:	00060993          	mv	s3,a2
    fdb8:	00100493          	li	s1,1
    fdbc:	00d05463          	blez	a3,fdc4 <demo_images_render+0x38>
    fdc0:	00068493          	mv	s1,a3
    fdc4:	0299d4b3          	divu	s1,s3,s1
    fdc8:	00300793          	li	a5,3
    fdcc:	00000713          	li	a4,0
    fdd0:	00000613          	li	a2,0
    fdd4:	00000593          	li	a1,0
    fdd8:	000a0513          	mv	a0,s4
    fddc:	00399993          	slli	s3,s3,0x3
    fde0:	02f4f4b3          	remu	s1,s1,a5
    fde4:	000d47b7          	lui	a5,0xd4
    fde8:	12478793          	addi	a5,a5,292 # d4124 <images>
    fdec:	00449693          	slli	a3,s1,0x4
    fdf0:	00d786b3          	add	a3,a5,a3
    fdf4:	ef9f80ef          	jal	8cec <gfx_blit>
    fdf8:	28000593          	li	a1,640
    fdfc:	02b9f5b3          	remu	a1,s3,a1
    fe00:	0000f7b7          	lui	a5,0xf
    fe04:	77a78793          	addi	a5,a5,1914 # f77a <demo_init+0xf6>
    fe08:	00800693          	li	a3,8
    fe0c:	18c00713          	li	a4,396
    fe10:	02800613          	li	a2,40
    fe14:	000a0513          	mv	a0,s4
    fe18:	ec5f80ef          	jal	8cdc <gfx_fill_rect>
    fe1c:	00100793          	li	a5,1
    fe20:	00148693          	addi	a3,s1,1
    fe24:	0af90e63          	beq	s2,a5,fee0 <demo_images_render+0x154>
    fe28:	000127b7          	lui	a5,0x12
    fe2c:	83078793          	addi	a5,a5,-2000 # 11830 <_data+0xf4>
    fe30:	03010493          	addi	s1,sp,48
    fe34:	00013637          	lui	a2,0x13
    fe38:	00090713          	mv	a4,s2
    fe3c:	94060613          	addi	a2,a2,-1728 # 12940 <_data+0x1204>
    fe40:	06000593          	li	a1,96
    fe44:	00048513          	mv	a0,s1
    fe48:	53d000ef          	jal	10b84 <mini_snprintf>
    fe4c:	04042303          	lw	t1,64(s0)
    fe50:	03842503          	lw	a0,56(s0)
    fe54:	06042383          	lw	t2,96(s0)
    fe58:	05042283          	lw	t0,80(s0)
    fe5c:	04c42f83          	lw	t6,76(s0)
    fe60:	04842f03          	lw	t5,72(s0)
    fe64:	04442e83          	lw	t4,68(s0)
    fe68:	06442e03          	lw	t3,100(s0)
    fe6c:	03442883          	lw	a7,52(s0)
    fe70:	03042803          	lw	a6,48(s0)
    fe74:	02c42783          	lw	a5,44(s0)
    fe78:	02842703          	lw	a4,40(s0)
    fe7c:	00c42683          	lw	a3,12(s0)
    fe80:	00842603          	lw	a2,8(s0)
    fe84:	00042583          	lw	a1,0(s0)
    fe88:	00612223          	sw	t1,4(sp)
    fe8c:	00013337          	lui	t1,0x13
    fe90:	00a12023          	sw	a0,0(sp)
    fe94:	00712c23          	sw	t2,24(sp)
    fe98:	00512a23          	sw	t0,20(sp)
    fe9c:	01f12823          	sw	t6,16(sp)
    fea0:	01e12623          	sw	t5,12(sp)
    fea4:	01d12423          	sw	t4,8(sp)
    fea8:	000a0513          	mv	a0,s4
    feac:	98030313          	addi	t1,t1,-1664 # 12980 <_data+0x1244>
    feb0:	02912223          	sw	s1,36(sp)
    feb4:	02612023          	sw	t1,32(sp)
    feb8:	01c12e23          	sw	t3,28(sp)
    febc:	c28ff0ef          	jal	f2e4 <overlay.isra.0>
    fec0:	0ac12083          	lw	ra,172(sp)
    fec4:	0a812403          	lw	s0,168(sp)
    fec8:	0a412483          	lw	s1,164(sp)
    fecc:	0a012903          	lw	s2,160(sp)
    fed0:	09c12983          	lw	s3,156(sp)
    fed4:	09812a03          	lw	s4,152(sp)
    fed8:	0b010113          	addi	sp,sp,176
    fedc:	00008067          	ret
    fee0:	000127b7          	lui	a5,0x12
    fee4:	ee078793          	addi	a5,a5,-288 # 11ee0 <_data+0x7a4>
    fee8:	f49ff06f          	j	fe30 <demo_images_render+0xa4>

0000feec <mini_vsnprintf>:
    feec:	f9010113          	addi	sp,sp,-112
    fef0:	05412c23          	sw	s4,88(sp)
    fef4:	05712623          	sw	s7,76(sp)
    fef8:	06112623          	sw	ra,108(sp)
    fefc:	00064783          	lbu	a5,0(a2)
    ff00:	00050b93          	mv	s7,a0
    ff04:	00058a13          	mv	s4,a1
    ff08:	120786e3          	beqz	a5,10834 <mini_vsnprintf+0x948>
    ff0c:	07212023          	sw	s2,96(sp)
    ff10:	05312e23          	sw	s3,92(sp)
    ff14:	05512a23          	sw	s5,84(sp)
    ff18:	05612823          	sw	s6,80(sp)
    ff1c:	05812423          	sw	s8,72(sp)
    ff20:	05912223          	sw	s9,68(sp)
    ff24:	05a12023          	sw	s10,64(sp)
    ff28:	03b12e23          	sw	s11,60(sp)
    ff2c:	06812423          	sw	s0,104(sp)
    ff30:	06912223          	sw	s1,100(sp)
    ff34:	00060993          	mv	s3,a2
    ff38:	00000913          	li	s2,0
    ff3c:	02500b13          	li	s6,37
    ff40:	03000c93          	li	s9,48
    ff44:	02000d93          	li	s11,32
    ff48:	00900a93          	li	s5,9
    ff4c:	07800c13          	li	s8,120
    ff50:	05700d13          	li	s10,87
    ff54:	00198413          	addi	s0,s3,1
    ff58:	07678e63          	beq	a5,s6,ffd4 <mini_vsnprintf+0xe8>
    ff5c:	00190713          	addi	a4,s2,1
    ff60:	000a0863          	beqz	s4,ff70 <mini_vsnprintf+0x84>
    ff64:	01477663          	bgeu	a4,s4,ff70 <mini_vsnprintf+0x84>
    ff68:	012b8933          	add	s2,s7,s2
    ff6c:	00f90023          	sb	a5,0(s2)
    ff70:	0019c783          	lbu	a5,1(s3)
    ff74:	00070913          	mv	s2,a4
    ff78:	00040993          	mv	s3,s0
    ff7c:	fc079ce3          	bnez	a5,ff54 <mini_vsnprintf+0x68>
    ff80:	00090513          	mv	a0,s2
    ff84:	120a0663          	beqz	s4,100b0 <mini_vsnprintf+0x1c4>
    ff88:	6b496263          	bltu	s2,s4,1062c <mini_vsnprintf+0x740>
    ff8c:	fffa0a13          	addi	s4,s4,-1 # fff <CUSTOM2+0xfa4>
    ff90:	014b8bb3          	add	s7,s7,s4
    ff94:	06812403          	lw	s0,104(sp)
    ff98:	06412483          	lw	s1,100(sp)
    ff9c:	06012903          	lw	s2,96(sp)
    ffa0:	05c12983          	lw	s3,92(sp)
    ffa4:	05412a83          	lw	s5,84(sp)
    ffa8:	05012b03          	lw	s6,80(sp)
    ffac:	04812c03          	lw	s8,72(sp)
    ffb0:	04412c83          	lw	s9,68(sp)
    ffb4:	04012d03          	lw	s10,64(sp)
    ffb8:	03c12d83          	lw	s11,60(sp)
    ffbc:	000b8023          	sb	zero,0(s7)
    ffc0:	06c12083          	lw	ra,108(sp)
    ffc4:	05812a03          	lw	s4,88(sp)
    ffc8:	04c12b83          	lw	s7,76(sp)
    ffcc:	07010113          	addi	sp,sp,112
    ffd0:	00008067          	ret
    ffd4:	0019c803          	lbu	a6,1(s3)
    ffd8:	02000613          	li	a2,32
    ffdc:	01981863          	bne	a6,s9,ffec <mini_vsnprintf+0x100>
    ffe0:	0029c803          	lbu	a6,2(s3)
    ffe4:	00298413          	addi	s0,s3,2
    ffe8:	03000613          	li	a2,48
    ffec:	fd080713          	addi	a4,a6,-48
    fff0:	0ff77793          	zext.b	a5,a4
    fff4:	00000493          	li	s1,0
    fff8:	02fae463          	bltu	s5,a5,10020 <mini_vsnprintf+0x134>
    fffc:	00144803          	lbu	a6,1(s0)
   10000:	00249793          	slli	a5,s1,0x2
   10004:	009787b3          	add	a5,a5,s1
   10008:	00179793          	slli	a5,a5,0x1
   1000c:	00f704b3          	add	s1,a4,a5
   10010:	fd080713          	addi	a4,a6,-48
   10014:	0ff77793          	zext.b	a5,a4
   10018:	00140413          	addi	s0,s0,1
   1001c:	fefaf0e3          	bgeu	s5,a5,fffc <mini_vsnprintf+0x110>
   10020:	070c6063          	bltu	s8,a6,10080 <mini_vsnprintf+0x194>
   10024:	030d7463          	bgeu	s10,a6,1004c <mini_vsnprintf+0x160>
   10028:	fa880793          	addi	a5,a6,-88
   1002c:	0ff7f793          	zext.b	a5,a5
   10030:	04fde863          	bltu	s11,a5,10080 <mini_vsnprintf+0x194>
   10034:	00013737          	lui	a4,0x13
   10038:	00279793          	slli	a5,a5,0x2
   1003c:	1d070713          	addi	a4,a4,464 # 131d0 <names.1+0xc>
   10040:	00e787b3          	add	a5,a5,a4
   10044:	0007a783          	lw	a5,0(a5)
   10048:	00078067          	jr	a5
   1004c:	f2080ae3          	beqz	a6,ff80 <mini_vsnprintf+0x94>
   10050:	02500793          	li	a5,37
   10054:	02f81663          	bne	a6,a5,10080 <mini_vsnprintf+0x194>
   10058:	00190713          	addi	a4,s2,1
   1005c:	000a0863          	beqz	s4,1006c <mini_vsnprintf+0x180>
   10060:	01477663          	bgeu	a4,s4,1006c <mini_vsnprintf+0x180>
   10064:	012b8933          	add	s2,s7,s2
   10068:	01090023          	sb	a6,0(s2)
   1006c:	00144783          	lbu	a5,1(s0)
   10070:	00140993          	addi	s3,s0,1
   10074:	00070913          	mv	s2,a4
   10078:	ec079ee3          	bnez	a5,ff54 <mini_vsnprintf+0x68>
   1007c:	f05ff06f          	j	ff80 <mini_vsnprintf+0x94>
   10080:	00290713          	addi	a4,s2,2
   10084:	fe0a04e3          	beqz	s4,1006c <mini_vsnprintf+0x180>
   10088:	00190793          	addi	a5,s2,1
   1008c:	0147fa63          	bgeu	a5,s4,100a0 <mini_vsnprintf+0x1b4>
   10090:	012b8933          	add	s2,s7,s2
   10094:	02500613          	li	a2,37
   10098:	00c90023          	sb	a2,0(s2)
   1009c:	00044803          	lbu	a6,0(s0)
   100a0:	fd4776e3          	bgeu	a4,s4,1006c <mini_vsnprintf+0x180>
   100a4:	00fb87b3          	add	a5,s7,a5
   100a8:	01078023          	sb	a6,0(a5)
   100ac:	fc1ff06f          	j	1006c <mini_vsnprintf+0x180>
   100b0:	06812403          	lw	s0,104(sp)
   100b4:	06412483          	lw	s1,100(sp)
   100b8:	06012903          	lw	s2,96(sp)
   100bc:	05c12983          	lw	s3,92(sp)
   100c0:	05412a83          	lw	s5,84(sp)
   100c4:	05012b03          	lw	s6,80(sp)
   100c8:	04812c03          	lw	s8,72(sp)
   100cc:	04412c83          	lw	s9,68(sp)
   100d0:	04012d03          	lw	s10,64(sp)
   100d4:	03c12d83          	lw	s11,60(sp)
   100d8:	06c12083          	lw	ra,108(sp)
   100dc:	05812a03          	lw	s4,88(sp)
   100e0:	04c12b83          	lw	s7,76(sp)
   100e4:	07010113          	addi	sp,sp,112
   100e8:	00008067          	ret
   100ec:	07500793          	li	a5,117
   100f0:	0006a583          	lw	a1,0(a3)
   100f4:	00468693          	addi	a3,a3,4
   100f8:	74f80663          	beq	a6,a5,10844 <mini_vsnprintf+0x958>
   100fc:	07800793          	li	a5,120
   10100:	50f81e63          	bne	a6,a5,1061c <mini_vsnprintf+0x730>
   10104:	00012737          	lui	a4,0x12
   10108:	01000793          	li	a5,16
   1010c:	cd070713          	addi	a4,a4,-816 # 11cd0 <_data+0x594>
   10110:	02f5f533          	remu	a0,a1,a5
   10114:	00a70533          	add	a0,a4,a0
   10118:	00054803          	lbu	a6,0(a0)
   1011c:	02f5d533          	divu	a0,a1,a5
   10120:	01010c23          	sb	a6,24(sp)
   10124:	04f5ece3          	bltu	a1,a5,1097c <mini_vsnprintf+0xa90>
   10128:	02f575b3          	remu	a1,a0,a5
   1012c:	00b705b3          	add	a1,a4,a1
   10130:	0005c803          	lbu	a6,0(a1)
   10134:	02f555b3          	divu	a1,a0,a5
   10138:	01010ca3          	sb	a6,25(sp)
   1013c:	04f56ee3          	bltu	a0,a5,10998 <mini_vsnprintf+0xaac>
   10140:	02f5f533          	remu	a0,a1,a5
   10144:	00a70533          	add	a0,a4,a0
   10148:	00054883          	lbu	a7,0(a0)
   1014c:	02f5d533          	divu	a0,a1,a5
   10150:	01110d23          	sb	a7,26(sp)
   10154:	06f5eee3          	bltu	a1,a5,109d0 <mini_vsnprintf+0xae4>
   10158:	02f575b3          	remu	a1,a0,a5
   1015c:	00b705b3          	add	a1,a4,a1
   10160:	0005c803          	lbu	a6,0(a1)
   10164:	02f555b3          	divu	a1,a0,a5
   10168:	01010da3          	sb	a6,27(sp)
   1016c:	0af564e3          	bltu	a0,a5,10a14 <mini_vsnprintf+0xb28>
   10170:	02f5f533          	remu	a0,a1,a5
   10174:	00a70533          	add	a0,a4,a0
   10178:	00054883          	lbu	a7,0(a0)
   1017c:	02f5d533          	divu	a0,a1,a5
   10180:	01110e23          	sb	a7,28(sp)
   10184:	0af5e6e3          	bltu	a1,a5,10a30 <mini_vsnprintf+0xb44>
   10188:	02f575b3          	remu	a1,a0,a5
   1018c:	00b705b3          	add	a1,a4,a1
   10190:	0005c803          	lbu	a6,0(a1)
   10194:	02f555b3          	divu	a1,a0,a5
   10198:	01010ea3          	sb	a6,29(sp)
   1019c:	0cf56ae3          	bltu	a0,a5,10a70 <mini_vsnprintf+0xb84>
   101a0:	02f5f533          	remu	a0,a1,a5
   101a4:	00a70533          	add	a0,a4,a0
   101a8:	00054883          	lbu	a7,0(a0)
   101ac:	02f5d533          	divu	a0,a1,a5
   101b0:	01110f23          	sb	a7,30(sp)
   101b4:	0cf5ece3          	bltu	a1,a5,10a8c <mini_vsnprintf+0xba0>
   101b8:	02f575b3          	remu	a1,a0,a5
   101bc:	00b705b3          	add	a1,a4,a1
   101c0:	0005c803          	lbu	a6,0(a1)
   101c4:	02f555b3          	divu	a1,a0,a5
   101c8:	01010fa3          	sb	a6,31(sp)
   101cc:	0ef56ce3          	bltu	a0,a5,10ac4 <mini_vsnprintf+0xbd8>
   101d0:	02f5f533          	remu	a0,a1,a5
   101d4:	00a70533          	add	a0,a4,a0
   101d8:	00054503          	lbu	a0,0(a0)
   101dc:	02f5d8b3          	divu	a7,a1,a5
   101e0:	02a10023          	sb	a0,32(sp)
   101e4:	10f5e2e3          	bltu	a1,a5,10ae8 <mini_vsnprintf+0xbfc>
   101e8:	01170733          	add	a4,a4,a7
   101ec:	00074803          	lbu	a6,0(a4)
   101f0:	00900793          	li	a5,9
   101f4:	00a00313          	li	t1,10
   101f8:	02078713          	addi	a4,a5,32
   101fc:	01010593          	addi	a1,sp,16
   10200:	00b708b3          	add	a7,a4,a1
   10204:	fe58c703          	lbu	a4,-27(a7)
   10208:	fe78c503          	lbu	a0,-25(a7)
   1020c:	fe68c583          	lbu	a1,-26(a7)
   10210:	02e103a3          	sb	a4,39(sp)
   10214:	02a102a3          	sb	a0,37(sp)
   10218:	02b10323          	sb	a1,38(sp)
   1021c:	00300713          	li	a4,3
   10220:	04e78e63          	beq	a5,a4,1027c <mini_vsnprintf+0x390>
   10224:	fe48c583          	lbu	a1,-28(a7)
   10228:	00400713          	li	a4,4
   1022c:	02b10423          	sb	a1,40(sp)
   10230:	04e78663          	beq	a5,a4,1027c <mini_vsnprintf+0x390>
   10234:	fe38c583          	lbu	a1,-29(a7)
   10238:	00500713          	li	a4,5
   1023c:	02b104a3          	sb	a1,41(sp)
   10240:	02e78e63          	beq	a5,a4,1027c <mini_vsnprintf+0x390>
   10244:	fe28c583          	lbu	a1,-30(a7)
   10248:	00600713          	li	a4,6
   1024c:	02b10523          	sb	a1,42(sp)
   10250:	02e78663          	beq	a5,a4,1027c <mini_vsnprintf+0x390>
   10254:	fe18c583          	lbu	a1,-31(a7)
   10258:	00700713          	li	a4,7
   1025c:	02b105a3          	sb	a1,43(sp)
   10260:	00e78e63          	beq	a5,a4,1027c <mini_vsnprintf+0x390>
   10264:	fe08c583          	lbu	a1,-32(a7)
   10268:	00900713          	li	a4,9
   1026c:	02b10623          	sb	a1,44(sp)
   10270:	06e798e3          	bne	a5,a4,10ae0 <mini_vsnprintf+0xbf4>
   10274:	01814703          	lbu	a4,24(sp)
   10278:	02e106a3          	sb	a4,45(sp)
   1027c:	76935e63          	bge	t1,s1,109f8 <mini_vsnprintf+0xb0c>
   10280:	012484b3          	add	s1,s1,s2
   10284:	3a0a1863          	bnez	s4,10634 <mini_vsnprintf+0x748>
   10288:	40648933          	sub	s2,s1,t1
   1028c:	00178793          	addi	a5,a5,1
   10290:	00f90933          	add	s2,s2,a5
   10294:	2fc0006f          	j	10590 <mini_vsnprintf+0x6a4>
   10298:	0006a583          	lw	a1,0(a3)
   1029c:	00468693          	addi	a3,a3,4
   102a0:	3005c463          	bltz	a1,105a8 <mini_vsnprintf+0x6bc>
   102a4:	00a00713          	li	a4,10
   102a8:	02e5f7b3          	remu	a5,a1,a4
   102ac:	00900813          	li	a6,9
   102b0:	00058513          	mv	a0,a1
   102b4:	03078793          	addi	a5,a5,48
   102b8:	00f10c23          	sb	a5,24(sp)
   102bc:	02e5d733          	divu	a4,a1,a4
   102c0:	78b87c63          	bgeu	a6,a1,10a58 <mini_vsnprintf+0xb6c>
   102c4:	00a00813          	li	a6,10
   102c8:	06300893          	li	a7,99
   102cc:	030777b3          	remu	a5,a4,a6
   102d0:	03078793          	addi	a5,a5,48
   102d4:	00f10ca3          	sb	a5,25(sp)
   102d8:	03075733          	divu	a4,a4,a6
   102dc:	6ca8f863          	bgeu	a7,a0,109ac <mini_vsnprintf+0xac0>
   102e0:	3e700893          	li	a7,999
   102e4:	030777b3          	remu	a5,a4,a6
   102e8:	03078793          	addi	a5,a5,48
   102ec:	00f10d23          	sb	a5,26(sp)
   102f0:	03075733          	divu	a4,a4,a6
   102f4:	6ca8f663          	bgeu	a7,a0,109c0 <mini_vsnprintf+0xad4>
   102f8:	000028b7          	lui	a7,0x2
   102fc:	70f88893          	addi	a7,a7,1807 # 270f <main+0x12ff>
   10300:	030777b3          	remu	a5,a4,a6
   10304:	03078793          	addi	a5,a5,48
   10308:	00f10da3          	sb	a5,27(sp)
   1030c:	03075733          	divu	a4,a4,a6
   10310:	6ea8fa63          	bgeu	a7,a0,10a04 <mini_vsnprintf+0xb18>
   10314:	000188b7          	lui	a7,0x18
   10318:	69f88893          	addi	a7,a7,1695 # 1869f <background_pixels+0x7df>
   1031c:	030777b3          	remu	a5,a4,a6
   10320:	03078793          	addi	a5,a5,48
   10324:	00f10e23          	sb	a5,28(sp)
   10328:	03075733          	divu	a4,a4,a6
   1032c:	6ea8fa63          	bgeu	a7,a0,10a20 <mini_vsnprintf+0xb34>
   10330:	000f48b7          	lui	a7,0xf4
   10334:	23f88893          	addi	a7,a7,575 # f423f <__freertos_irq_stack_top+0xc09f>
   10338:	030777b3          	remu	a5,a4,a6
   1033c:	03078793          	addi	a5,a5,48
   10340:	00f10ea3          	sb	a5,29(sp)
   10344:	03075733          	divu	a4,a4,a6
   10348:	70a8fc63          	bgeu	a7,a0,10a60 <mini_vsnprintf+0xb74>
   1034c:	009898b7          	lui	a7,0x989
   10350:	67f88893          	addi	a7,a7,1663 # 98967f <__freertos_irq_stack_top+0x8a14df>
   10354:	030777b3          	remu	a5,a4,a6
   10358:	03078793          	addi	a5,a5,48
   1035c:	00f10f23          	sb	a5,30(sp)
   10360:	03075733          	divu	a4,a4,a6
   10364:	70a8fc63          	bgeu	a7,a0,10a7c <mini_vsnprintf+0xb90>
   10368:	05f5e8b7          	lui	a7,0x5f5e
   1036c:	0ff88893          	addi	a7,a7,255 # 5f5e0ff <__freertos_irq_stack_top+0x5e75f5f>
   10370:	030777b3          	remu	a5,a4,a6
   10374:	03078793          	addi	a5,a5,48
   10378:	00f10fa3          	sb	a5,31(sp)
   1037c:	03075733          	divu	a4,a4,a6
   10380:	72a8fa63          	bgeu	a7,a0,10ab4 <mini_vsnprintf+0xbc8>
   10384:	3b9ad8b7          	lui	a7,0x3b9ad
   10388:	9ff88893          	addi	a7,a7,-1537 # 3b9ac9ff <__freertos_irq_stack_top+0x3b8c485f>
   1038c:	030777b3          	remu	a5,a4,a6
   10390:	03078793          	addi	a5,a5,48
   10394:	02f10023          	sb	a5,32(sp)
   10398:	03075733          	divu	a4,a4,a6
   1039c:	72a8fa63          	bgeu	a7,a0,10ad0 <mini_vsnprintf+0xbe4>
   103a0:	03070713          	addi	a4,a4,48
   103a4:	0ff77793          	zext.b	a5,a4
   103a8:	02f100a3          	sb	a5,33(sp)
   103ac:	5805c263          	bltz	a1,10930 <mini_vsnprintf+0xa44>
   103b0:	00a00713          	li	a4,10
   103b4:	00900593          	li	a1,9
   103b8:	01010513          	addi	a0,sp,16
   103bc:	02058593          	addi	a1,a1,32
   103c0:	00a585b3          	add	a1,a1,a0
   103c4:	fe75c503          	lbu	a0,-25(a1)
   103c8:	02a102a3          	sb	a0,37(sp)
   103cc:	fe65c803          	lbu	a6,-26(a1)
   103d0:	00300513          	li	a0,3
   103d4:	03010323          	sb	a6,38(sp)
   103d8:	06a70e63          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   103dc:	fe55c803          	lbu	a6,-27(a1)
   103e0:	00400513          	li	a0,4
   103e4:	030103a3          	sb	a6,39(sp)
   103e8:	06a70663          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   103ec:	fe45c803          	lbu	a6,-28(a1)
   103f0:	00500513          	li	a0,5
   103f4:	03010423          	sb	a6,40(sp)
   103f8:	04a70e63          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   103fc:	fe35c803          	lbu	a6,-29(a1)
   10400:	00600513          	li	a0,6
   10404:	030104a3          	sb	a6,41(sp)
   10408:	04a70663          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   1040c:	fe25c803          	lbu	a6,-30(a1)
   10410:	00700513          	li	a0,7
   10414:	03010523          	sb	a6,42(sp)
   10418:	02a70e63          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   1041c:	fe15c803          	lbu	a6,-31(a1)
   10420:	00800513          	li	a0,8
   10424:	030105a3          	sb	a6,43(sp)
   10428:	02a70663          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   1042c:	fe05c803          	lbu	a6,-32(a1)
   10430:	00900513          	li	a0,9
   10434:	03010623          	sb	a6,44(sp)
   10438:	00a70e63          	beq	a4,a0,10454 <mini_vsnprintf+0x568>
   1043c:	fdf5c803          	lbu	a6,-33(a1)
   10440:	00b00513          	li	a0,11
   10444:	030106a3          	sb	a6,45(sp)
   10448:	6ea71663          	bne	a4,a0,10b34 <mini_vsnprintf+0xc48>
   1044c:	fde5c583          	lbu	a1,-34(a1)
   10450:	02b10723          	sb	a1,46(sp)
   10454:	58975c63          	bge	a4,s1,109ec <mini_vsnprintf+0xb00>
   10458:	2c0a1663          	bnez	s4,10724 <mini_vsnprintf+0x838>
   1045c:	012484b3          	add	s1,s1,s2
   10460:	40e48933          	sub	s2,s1,a4
   10464:	00e90933          	add	s2,s2,a4
   10468:	1280006f          	j	10590 <mini_vsnprintf+0x6a4>
   1046c:	0006a583          	lw	a1,0(a3)
   10470:	00468693          	addi	a3,a3,4
   10474:	3e058063          	beqz	a1,10854 <mini_vsnprintf+0x968>
   10478:	0005c783          	lbu	a5,0(a1)
   1047c:	00140993          	addi	s3,s0,1
   10480:	40078a63          	beqz	a5,10894 <mini_vsnprintf+0x9a8>
   10484:	00158513          	addi	a0,a1,1
   10488:	00d12623          	sw	a3,12(sp)
   1048c:	00b12423          	sw	a1,8(sp)
   10490:	ef5f00ef          	jal	1384 <strlen>
   10494:	00150793          	addi	a5,a0,1
   10498:	00812583          	lw	a1,8(sp)
   1049c:	00c12683          	lw	a3,12(sp)
   104a0:	4497d063          	bge	a5,s1,108e0 <mini_vsnprintf+0x9f4>
   104a4:	080a0063          	beqz	s4,10524 <mini_vsnprintf+0x638>
   104a8:	00190893          	addi	a7,s2,1
   104ac:	00078713          	mv	a4,a5
   104b0:	40f88533          	sub	a0,a7,a5
   104b4:	40f90933          	sub	s2,s2,a5
   104b8:	02000813          	li	a6,32
   104bc:	00e50633          	add	a2,a0,a4
   104c0:	01467863          	bgeu	a2,s4,104d0 <mini_vsnprintf+0x5e4>
   104c4:	00e90633          	add	a2,s2,a4
   104c8:	00cb8633          	add	a2,s7,a2
   104cc:	01060023          	sb	a6,0(a2)
   104d0:	00170713          	addi	a4,a4,1
   104d4:	fe9744e3          	blt	a4,s1,104bc <mini_vsnprintf+0x5d0>
   104d8:	00000713          	li	a4,0
   104dc:	0097d663          	bge	a5,s1,104e8 <mini_vsnprintf+0x5fc>
   104e0:	fff48713          	addi	a4,s1,-1
   104e4:	40f70733          	sub	a4,a4,a5
   104e8:	01170733          	add	a4,a4,a7
   104ec:	36078c63          	beqz	a5,10864 <mini_vsnprintf+0x978>
   104f0:	00e78933          	add	s2,a5,a4
   104f4:	00070793          	mv	a5,a4
   104f8:	00178793          	addi	a5,a5,1
   104fc:	0147fc63          	bgeu	a5,s4,10514 <mini_vsnprintf+0x628>
   10500:	40e78633          	sub	a2,a5,a4
   10504:	00c58633          	add	a2,a1,a2
   10508:	fff64503          	lbu	a0,-1(a2)
   1050c:	00fb8633          	add	a2,s7,a5
   10510:	fea60fa3          	sb	a0,-1(a2)
   10514:	ff2792e3          	bne	a5,s2,104f8 <mini_vsnprintf+0x60c>
   10518:	00144783          	lbu	a5,1(s0)
   1051c:	a2079ce3          	bnez	a5,ff54 <mini_vsnprintf+0x68>
   10520:	a61ff06f          	j	ff80 <mini_vsnprintf+0x94>
   10524:	00000713          	li	a4,0
   10528:	0097d663          	bge	a5,s1,10534 <mini_vsnprintf+0x648>
   1052c:	fff48713          	addi	a4,s1,-1
   10530:	40f70733          	sub	a4,a4,a5
   10534:	00190913          	addi	s2,s2,1
   10538:	01270733          	add	a4,a4,s2
   1053c:	32078463          	beqz	a5,10864 <mini_vsnprintf+0x978>
   10540:	00e78933          	add	s2,a5,a4
   10544:	00144783          	lbu	a5,1(s0)
   10548:	fd5ff06f          	j	1051c <mini_vsnprintf+0x630>
   1054c:	00100793          	li	a5,1
   10550:	0006a583          	lw	a1,0(a3)
   10554:	00468693          	addi	a3,a3,4
   10558:	4297d863          	bge	a5,s1,10988 <mini_vsnprintf+0xa9c>
   1055c:	040a0263          	beqz	s4,105a0 <mini_vsnprintf+0x6b4>
   10560:	fff90793          	addi	a5,s2,-1
   10564:	009787b3          	add	a5,a5,s1
   10568:	02000613          	li	a2,32
   1056c:	00190913          	addi	s2,s2,1
   10570:	01497663          	bgeu	s2,s4,1057c <mini_vsnprintf+0x690>
   10574:	012b8733          	add	a4,s7,s2
   10578:	fec70fa3          	sb	a2,-1(a4)
   1057c:	fef918e3          	bne	s2,a5,1056c <mini_vsnprintf+0x680>
   10580:	00190913          	addi	s2,s2,1
   10584:	01497663          	bgeu	s2,s4,10590 <mini_vsnprintf+0x6a4>
   10588:	00fb87b3          	add	a5,s7,a5
   1058c:	00b78023          	sb	a1,0(a5)
   10590:	00144783          	lbu	a5,1(s0)
   10594:	00140993          	addi	s3,s0,1
   10598:	9a079ee3          	bnez	a5,ff54 <mini_vsnprintf+0x68>
   1059c:	9e5ff06f          	j	ff80 <mini_vsnprintf+0x94>
   105a0:	01248933          	add	s2,s1,s2
   105a4:	fedff06f          	j	10590 <mini_vsnprintf+0x6a4>
   105a8:	40b00533          	neg	a0,a1
   105ac:	00a00713          	li	a4,10
   105b0:	02e577b3          	remu	a5,a0,a4
   105b4:	00900813          	li	a6,9
   105b8:	03078793          	addi	a5,a5,48
   105bc:	00f10c23          	sb	a5,24(sp)
   105c0:	02e55733          	divu	a4,a0,a4
   105c4:	d0a860e3          	bltu	a6,a0,102c4 <mini_vsnprintf+0x3d8>
   105c8:	00000593          	li	a1,0
   105cc:	00100713          	li	a4,1
   105d0:	03000793          	li	a5,48
   105d4:	36f60463          	beq	a2,a5,1093c <mini_vsnprintf+0xa50>
   105d8:	00258793          	addi	a5,a1,2
   105dc:	01010513          	addi	a0,sp,16
   105e0:	02070593          	addi	a1,a4,32
   105e4:	00a58533          	add	a0,a1,a0
   105e8:	00070593          	mv	a1,a4
   105ec:	02d00713          	li	a4,45
   105f0:	fee50423          	sb	a4,-24(a0)
   105f4:	02058593          	addi	a1,a1,32
   105f8:	01010513          	addi	a0,sp,16
   105fc:	00078713          	mv	a4,a5
   10600:	00a585b3          	add	a1,a1,a0
   10604:	02d00793          	li	a5,45
   10608:	fe75c803          	lbu	a6,-25(a1)
   1060c:	00200513          	li	a0,2
   10610:	030102a3          	sb	a6,37(sp)
   10614:	daa71ce3          	bne	a4,a0,103cc <mini_vsnprintf+0x4e0>
   10618:	e3dff06f          	j	10454 <mini_vsnprintf+0x568>
   1061c:	00012737          	lui	a4,0x12
   10620:	01000793          	li	a5,16
   10624:	ce470713          	addi	a4,a4,-796 # 11ce4 <_data+0x5a8>
   10628:	ae9ff06f          	j	10110 <mini_vsnprintf+0x224>
   1062c:	012b8bb3          	add	s7,s7,s2
   10630:	965ff06f          	j	ff94 <mini_vsnprintf+0xa8>
   10634:	406484b3          	sub	s1,s1,t1
   10638:	00190913          	addi	s2,s2,1
   1063c:	01497663          	bgeu	s2,s4,10648 <mini_vsnprintf+0x75c>
   10640:	012b8733          	add	a4,s7,s2
   10644:	fec70fa3          	sb	a2,-1(a4)
   10648:	fe9918e3          	bne	s2,s1,10638 <mini_vsnprintf+0x74c>
   1064c:	00148713          	addi	a4,s1,1
   10650:	01477663          	bgeu	a4,s4,1065c <mini_vsnprintf+0x770>
   10654:	009b8633          	add	a2,s7,s1
   10658:	01060023          	sb	a6,0(a2)
   1065c:	0a078e63          	beqz	a5,10718 <mini_vsnprintf+0x82c>
   10660:	00248613          	addi	a2,s1,2
   10664:	01467863          	bgeu	a2,s4,10674 <mini_vsnprintf+0x788>
   10668:	02514583          	lbu	a1,37(sp)
   1066c:	00eb8733          	add	a4,s7,a4
   10670:	00b70023          	sb	a1,0(a4)
   10674:	00100713          	li	a4,1
   10678:	0ae78063          	beq	a5,a4,10718 <mini_vsnprintf+0x82c>
   1067c:	00348713          	addi	a4,s1,3
   10680:	01477863          	bgeu	a4,s4,10690 <mini_vsnprintf+0x7a4>
   10684:	02614583          	lbu	a1,38(sp)
   10688:	00cb8633          	add	a2,s7,a2
   1068c:	00b60023          	sb	a1,0(a2)
   10690:	00200613          	li	a2,2
   10694:	08c78263          	beq	a5,a2,10718 <mini_vsnprintf+0x82c>
   10698:	00448613          	addi	a2,s1,4
   1069c:	01467863          	bgeu	a2,s4,106ac <mini_vsnprintf+0x7c0>
   106a0:	02714583          	lbu	a1,39(sp)
   106a4:	00eb8733          	add	a4,s7,a4
   106a8:	00b70023          	sb	a1,0(a4)
   106ac:	00300713          	li	a4,3
   106b0:	06e78463          	beq	a5,a4,10718 <mini_vsnprintf+0x82c>
   106b4:	00548713          	addi	a4,s1,5
   106b8:	1d476663          	bltu	a4,s4,10884 <mini_vsnprintf+0x998>
   106bc:	00400613          	li	a2,4
   106c0:	04c78c63          	beq	a5,a2,10718 <mini_vsnprintf+0x82c>
   106c4:	00648593          	addi	a1,s1,6
   106c8:	1d45ec63          	bltu	a1,s4,108a0 <mini_vsnprintf+0x9b4>
   106cc:	00500713          	li	a4,5
   106d0:	04e78463          	beq	a5,a4,10718 <mini_vsnprintf+0x82c>
   106d4:	00748613          	addi	a2,s1,7
   106d8:	1f466463          	bltu	a2,s4,108c0 <mini_vsnprintf+0x9d4>
   106dc:	00600713          	li	a4,6
   106e0:	02e78c63          	beq	a5,a4,10718 <mini_vsnprintf+0x82c>
   106e4:	00848713          	addi	a4,s1,8
   106e8:	21476c63          	bltu	a4,s4,10900 <mini_vsnprintf+0xa14>
   106ec:	00700613          	li	a2,7
   106f0:	02c78463          	beq	a5,a2,10718 <mini_vsnprintf+0x82c>
   106f4:	00948613          	addi	a2,s1,9
   106f8:	21466c63          	bltu	a2,s4,10910 <mini_vsnprintf+0xa24>
   106fc:	00900713          	li	a4,9
   10700:	00e79c63          	bne	a5,a4,10718 <mini_vsnprintf+0x82c>
   10704:	00a48713          	addi	a4,s1,10
   10708:	01477863          	bgeu	a4,s4,10718 <mini_vsnprintf+0x82c>
   1070c:	02d14703          	lbu	a4,45(sp)
   10710:	00cb8633          	add	a2,s7,a2
   10714:	00e60023          	sb	a4,0(a2)
   10718:	00178793          	addi	a5,a5,1
   1071c:	00978933          	add	s2,a5,s1
   10720:	e71ff06f          	j	10590 <mini_vsnprintf+0x6a4>
   10724:	012485b3          	add	a1,s1,s2
   10728:	40e585b3          	sub	a1,a1,a4
   1072c:	00190913          	addi	s2,s2,1
   10730:	01497663          	bgeu	s2,s4,1073c <mini_vsnprintf+0x850>
   10734:	012b8533          	add	a0,s7,s2
   10738:	fec50fa3          	sb	a2,-1(a0)
   1073c:	feb918e3          	bne	s2,a1,1072c <mini_vsnprintf+0x840>
   10740:	00158613          	addi	a2,a1,1
   10744:	01467663          	bgeu	a2,s4,10750 <mini_vsnprintf+0x864>
   10748:	00bb8533          	add	a0,s7,a1
   1074c:	00f50023          	sb	a5,0(a0)
   10750:	00100793          	li	a5,1
   10754:	0cf70c63          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   10758:	00258793          	addi	a5,a1,2
   1075c:	0147f863          	bgeu	a5,s4,1076c <mini_vsnprintf+0x880>
   10760:	02514503          	lbu	a0,37(sp)
   10764:	00cb8633          	add	a2,s7,a2
   10768:	00a60023          	sb	a0,0(a2)
   1076c:	00200613          	li	a2,2
   10770:	0ac70e63          	beq	a4,a2,1082c <mini_vsnprintf+0x940>
   10774:	00358613          	addi	a2,a1,3
   10778:	01467863          	bgeu	a2,s4,10788 <mini_vsnprintf+0x89c>
   1077c:	02614503          	lbu	a0,38(sp)
   10780:	00fb87b3          	add	a5,s7,a5
   10784:	00a78023          	sb	a0,0(a5)
   10788:	00300793          	li	a5,3
   1078c:	0af70063          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   10790:	00458513          	addi	a0,a1,4
   10794:	01457863          	bgeu	a0,s4,107a4 <mini_vsnprintf+0x8b8>
   10798:	02714783          	lbu	a5,39(sp)
   1079c:	00cb8633          	add	a2,s7,a2
   107a0:	00f60023          	sb	a5,0(a2)
   107a4:	00400793          	li	a5,4
   107a8:	08f70263          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   107ac:	00558793          	addi	a5,a1,5
   107b0:	0d47e263          	bltu	a5,s4,10874 <mini_vsnprintf+0x988>
   107b4:	00500613          	li	a2,5
   107b8:	06c70a63          	beq	a4,a2,1082c <mini_vsnprintf+0x940>
   107bc:	00658513          	addi	a0,a1,6
   107c0:	0f456863          	bltu	a0,s4,108b0 <mini_vsnprintf+0x9c4>
   107c4:	00600793          	li	a5,6
   107c8:	06f70263          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   107cc:	00758613          	addi	a2,a1,7
   107d0:	11466063          	bltu	a2,s4,108d0 <mini_vsnprintf+0x9e4>
   107d4:	00700793          	li	a5,7
   107d8:	04f70a63          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   107dc:	00858793          	addi	a5,a1,8
   107e0:	1147e863          	bltu	a5,s4,108f0 <mini_vsnprintf+0xa04>
   107e4:	00800613          	li	a2,8
   107e8:	04c70263          	beq	a4,a2,1082c <mini_vsnprintf+0x940>
   107ec:	00958613          	addi	a2,a1,9
   107f0:	13466863          	bltu	a2,s4,10920 <mini_vsnprintf+0xa34>
   107f4:	00900793          	li	a5,9
   107f8:	02f70a63          	beq	a4,a5,1082c <mini_vsnprintf+0x940>
   107fc:	00a58793          	addi	a5,a1,10
   10800:	0147f863          	bgeu	a5,s4,10810 <mini_vsnprintf+0x924>
   10804:	02d14503          	lbu	a0,45(sp)
   10808:	00cb8633          	add	a2,s7,a2
   1080c:	00a60023          	sb	a0,0(a2)
   10810:	00b00613          	li	a2,11
   10814:	00c71c63          	bne	a4,a2,1082c <mini_vsnprintf+0x940>
   10818:	00b58613          	addi	a2,a1,11
   1081c:	01467863          	bgeu	a2,s4,1082c <mini_vsnprintf+0x940>
   10820:	02e14603          	lbu	a2,46(sp)
   10824:	00fb87b3          	add	a5,s7,a5
   10828:	00c78023          	sb	a2,0(a5)
   1082c:	00b70933          	add	s2,a4,a1
   10830:	d61ff06f          	j	10590 <mini_vsnprintf+0x6a4>
   10834:	00000513          	li	a0,0
   10838:	8a0580e3          	beqz	a1,100d8 <mini_vsnprintf+0x1ec>
   1083c:	000b8023          	sb	zero,0(s7)
   10840:	f80ff06f          	j	ffc0 <mini_vsnprintf+0xd4>
   10844:	00012737          	lui	a4,0x12
   10848:	00a00793          	li	a5,10
   1084c:	ce470713          	addi	a4,a4,-796 # 11ce4 <_data+0x5a8>
   10850:	8c1ff06f          	j	10110 <mini_vsnprintf+0x224>
   10854:	000135b7          	lui	a1,0x13
   10858:	00140993          	addi	s3,s0,1
   1085c:	9b858593          	addi	a1,a1,-1608 # 129b8 <_data+0x127c>
   10860:	c25ff06f          	j	10484 <mini_vsnprintf+0x598>
   10864:	00144783          	lbu	a5,1(s0)
   10868:	00070913          	mv	s2,a4
   1086c:	ee079463          	bnez	a5,ff54 <mini_vsnprintf+0x68>
   10870:	f10ff06f          	j	ff80 <mini_vsnprintf+0x94>
   10874:	02814603          	lbu	a2,40(sp)
   10878:	00ab8533          	add	a0,s7,a0
   1087c:	00c50023          	sb	a2,0(a0)
   10880:	f35ff06f          	j	107b4 <mini_vsnprintf+0x8c8>
   10884:	02814583          	lbu	a1,40(sp)
   10888:	00cb8633          	add	a2,s7,a2
   1088c:	00b60023          	sb	a1,0(a2)
   10890:	e2dff06f          	j	106bc <mini_vsnprintf+0x7d0>
   10894:	c89052e3          	blez	s1,10518 <mini_vsnprintf+0x62c>
   10898:	c00a18e3          	bnez	s4,104a8 <mini_vsnprintf+0x5bc>
   1089c:	c89ff06f          	j	10524 <mini_vsnprintf+0x638>
   108a0:	02914603          	lbu	a2,41(sp)
   108a4:	00eb8733          	add	a4,s7,a4
   108a8:	00c70023          	sb	a2,0(a4)
   108ac:	e21ff06f          	j	106cc <mini_vsnprintf+0x7e0>
   108b0:	02914603          	lbu	a2,41(sp)
   108b4:	00fb87b3          	add	a5,s7,a5
   108b8:	00c78023          	sb	a2,0(a5)
   108bc:	f09ff06f          	j	107c4 <mini_vsnprintf+0x8d8>
   108c0:	02a14703          	lbu	a4,42(sp)
   108c4:	00bb85b3          	add	a1,s7,a1
   108c8:	00e58023          	sb	a4,0(a1)
   108cc:	e11ff06f          	j	106dc <mini_vsnprintf+0x7f0>
   108d0:	02a14783          	lbu	a5,42(sp)
   108d4:	00ab8533          	add	a0,s7,a0
   108d8:	00f50023          	sb	a5,0(a0)
   108dc:	ef9ff06f          	j	107d4 <mini_vsnprintf+0x8e8>
   108e0:	00090713          	mv	a4,s2
   108e4:	c00a16e3          	bnez	s4,104f0 <mini_vsnprintf+0x604>
   108e8:	00e78933          	add	s2,a5,a4
   108ec:	c59ff06f          	j	10544 <mini_vsnprintf+0x658>
   108f0:	02b14503          	lbu	a0,43(sp)
   108f4:	00cb8633          	add	a2,s7,a2
   108f8:	00a60023          	sb	a0,0(a2)
   108fc:	ee9ff06f          	j	107e4 <mini_vsnprintf+0x8f8>
   10900:	02b14583          	lbu	a1,43(sp)
   10904:	00cb8633          	add	a2,s7,a2
   10908:	00b60023          	sb	a1,0(a2)
   1090c:	de1ff06f          	j	106ec <mini_vsnprintf+0x800>
   10910:	02c14583          	lbu	a1,44(sp)
   10914:	00eb8733          	add	a4,s7,a4
   10918:	00b70023          	sb	a1,0(a4)
   1091c:	de1ff06f          	j	106fc <mini_vsnprintf+0x810>
   10920:	02c14503          	lbu	a0,44(sp)
   10924:	00fb87b3          	add	a5,s7,a5
   10928:	00a78023          	sb	a0,0(a5)
   1092c:	ec9ff06f          	j	107f4 <mini_vsnprintf+0x908>
   10930:	00900593          	li	a1,9
   10934:	00a00713          	li	a4,10
   10938:	c99ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   1093c:	00190513          	addi	a0,s2,1
   10940:	000a0a63          	beqz	s4,10954 <mini_vsnprintf+0xa68>
   10944:	01457863          	bgeu	a0,s4,10954 <mini_vsnprintf+0xa68>
   10948:	012b8933          	add	s2,s7,s2
   1094c:	02d00793          	li	a5,45
   10950:	00f90023          	sb	a5,0(s2)
   10954:	02058793          	addi	a5,a1,32
   10958:	01010593          	addi	a1,sp,16
   1095c:	009038b3          	snez	a7,s1
   10960:	00b785b3          	add	a1,a5,a1
   10964:	00100813          	li	a6,1
   10968:	fe85c783          	lbu	a5,-24(a1)
   1096c:	411484b3          	sub	s1,s1,a7
   10970:	00050913          	mv	s2,a0
   10974:	af0700e3          	beq	a4,a6,10454 <mini_vsnprintf+0x568>
   10978:	c91ff06f          	j	10608 <mini_vsnprintf+0x71c>
   1097c:	00000793          	li	a5,0
   10980:	00100313          	li	t1,1
   10984:	8f9ff06f          	j	1027c <mini_vsnprintf+0x390>
   10988:	00190713          	addi	a4,s2,1
   1098c:	1e0a1663          	bnez	s4,10b78 <mini_vsnprintf+0xc8c>
   10990:	00070913          	mv	s2,a4
   10994:	bfdff06f          	j	10590 <mini_vsnprintf+0x6a4>
   10998:	01814703          	lbu	a4,24(sp)
   1099c:	00100793          	li	a5,1
   109a0:	00200313          	li	t1,2
   109a4:	02e102a3          	sb	a4,37(sp)
   109a8:	8d5ff06f          	j	1027c <mini_vsnprintf+0x390>
   109ac:	1605c263          	bltz	a1,10b10 <mini_vsnprintf+0xc24>
   109b0:	01814583          	lbu	a1,24(sp)
   109b4:	00200713          	li	a4,2
   109b8:	02b102a3          	sb	a1,37(sp)
   109bc:	a99ff06f          	j	10454 <mini_vsnprintf+0x568>
   109c0:	1405ce63          	bltz	a1,10b1c <mini_vsnprintf+0xc30>
   109c4:	00300713          	li	a4,3
   109c8:	00200593          	li	a1,2
   109cc:	9edff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   109d0:	01814703          	lbu	a4,24(sp)
   109d4:	030102a3          	sb	a6,37(sp)
   109d8:	00200793          	li	a5,2
   109dc:	02e10323          	sb	a4,38(sp)
   109e0:	00088813          	mv	a6,a7
   109e4:	00300313          	li	t1,3
   109e8:	895ff06f          	j	1027c <mini_vsnprintf+0x390>
   109ec:	a60a0ce3          	beqz	s4,10464 <mini_vsnprintf+0x578>
   109f0:	00090593          	mv	a1,s2
   109f4:	d4dff06f          	j	10740 <mini_vsnprintf+0x854>
   109f8:	880a0ae3          	beqz	s4,1028c <mini_vsnprintf+0x3a0>
   109fc:	00090493          	mv	s1,s2
   10a00:	c4dff06f          	j	1064c <mini_vsnprintf+0x760>
   10a04:	1205c263          	bltz	a1,10b28 <mini_vsnprintf+0xc3c>
   10a08:	00400713          	li	a4,4
   10a0c:	00300593          	li	a1,3
   10a10:	9a9ff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10a14:	00300793          	li	a5,3
   10a18:	00400313          	li	t1,4
   10a1c:	fdcff06f          	j	101f8 <mini_vsnprintf+0x30c>
   10a20:	1205c463          	bltz	a1,10b48 <mini_vsnprintf+0xc5c>
   10a24:	00500713          	li	a4,5
   10a28:	00400593          	li	a1,4
   10a2c:	98dff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10a30:	01914783          	lbu	a5,25(sp)
   10a34:	01a14703          	lbu	a4,26(sp)
   10a38:	030102a3          	sb	a6,37(sp)
   10a3c:	02f103a3          	sb	a5,39(sp)
   10a40:	00088813          	mv	a6,a7
   10a44:	02e10323          	sb	a4,38(sp)
   10a48:	00500313          	li	t1,5
   10a4c:	00400793          	li	a5,4
   10a50:	03410893          	addi	a7,sp,52
   10a54:	fd0ff06f          	j	10224 <mini_vsnprintf+0x338>
   10a58:	00100713          	li	a4,1
   10a5c:	9f9ff06f          	j	10454 <mini_vsnprintf+0x568>
   10a60:	0e05ca63          	bltz	a1,10b54 <mini_vsnprintf+0xc68>
   10a64:	00600713          	li	a4,6
   10a68:	00500593          	li	a1,5
   10a6c:	94dff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10a70:	00500793          	li	a5,5
   10a74:	00600313          	li	t1,6
   10a78:	f80ff06f          	j	101f8 <mini_vsnprintf+0x30c>
   10a7c:	0c05c063          	bltz	a1,10b3c <mini_vsnprintf+0xc50>
   10a80:	00700713          	li	a4,7
   10a84:	00600593          	li	a1,6
   10a88:	931ff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10a8c:	01b14783          	lbu	a5,27(sp)
   10a90:	01c14703          	lbu	a4,28(sp)
   10a94:	030102a3          	sb	a6,37(sp)
   10a98:	02f103a3          	sb	a5,39(sp)
   10a9c:	00088813          	mv	a6,a7
   10aa0:	02e10323          	sb	a4,38(sp)
   10aa4:	00700313          	li	t1,7
   10aa8:	00600793          	li	a5,6
   10aac:	03610893          	addi	a7,sp,54
   10ab0:	f74ff06f          	j	10224 <mini_vsnprintf+0x338>
   10ab4:	0a05c663          	bltz	a1,10b60 <mini_vsnprintf+0xc74>
   10ab8:	00800713          	li	a4,8
   10abc:	00700593          	li	a1,7
   10ac0:	8f9ff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10ac4:	00700793          	li	a5,7
   10ac8:	00800313          	li	t1,8
   10acc:	f2cff06f          	j	101f8 <mini_vsnprintf+0x30c>
   10ad0:	0805ce63          	bltz	a1,10b6c <mini_vsnprintf+0xc80>
   10ad4:	00900713          	li	a4,9
   10ad8:	00800593          	li	a1,8
   10adc:	8ddff06f          	j	103b8 <mini_vsnprintf+0x4cc>
   10ae0:	00800793          	li	a5,8
   10ae4:	f98ff06f          	j	1027c <mini_vsnprintf+0x390>
   10ae8:	01d14783          	lbu	a5,29(sp)
   10aec:	01e14703          	lbu	a4,30(sp)
   10af0:	030102a3          	sb	a6,37(sp)
   10af4:	02f103a3          	sb	a5,39(sp)
   10af8:	02e10323          	sb	a4,38(sp)
   10afc:	00050813          	mv	a6,a0
   10b00:	00900313          	li	t1,9
   10b04:	00800793          	li	a5,8
   10b08:	03810893          	addi	a7,sp,56
   10b0c:	f18ff06f          	j	10224 <mini_vsnprintf+0x338>
   10b10:	00100593          	li	a1,1
   10b14:	00200713          	li	a4,2
   10b18:	ab9ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b1c:	00200593          	li	a1,2
   10b20:	00300713          	li	a4,3
   10b24:	aadff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b28:	00300593          	li	a1,3
   10b2c:	00400713          	li	a4,4
   10b30:	aa1ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b34:	00a00713          	li	a4,10
   10b38:	91dff06f          	j	10454 <mini_vsnprintf+0x568>
   10b3c:	00600593          	li	a1,6
   10b40:	00700713          	li	a4,7
   10b44:	a8dff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b48:	00400593          	li	a1,4
   10b4c:	00500713          	li	a4,5
   10b50:	a81ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b54:	00500593          	li	a1,5
   10b58:	00600713          	li	a4,6
   10b5c:	a75ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b60:	00700593          	li	a1,7
   10b64:	00800713          	li	a4,8
   10b68:	a69ff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b6c:	00800593          	li	a1,8
   10b70:	00900713          	li	a4,9
   10b74:	a5dff06f          	j	105d0 <mini_vsnprintf+0x6e4>
   10b78:	00090793          	mv	a5,s2
   10b7c:	00070913          	mv	s2,a4
   10b80:	a05ff06f          	j	10584 <mini_vsnprintf+0x698>

00010b84 <mini_snprintf>:
   10b84:	fc010113          	addi	sp,sp,-64
   10b88:	02c10313          	addi	t1,sp,44
   10b8c:	02d12623          	sw	a3,44(sp)
   10b90:	00030693          	mv	a3,t1
   10b94:	00112e23          	sw	ra,28(sp)
   10b98:	02e12823          	sw	a4,48(sp)
   10b9c:	02f12a23          	sw	a5,52(sp)
   10ba0:	03012c23          	sw	a6,56(sp)
   10ba4:	03112e23          	sw	a7,60(sp)
   10ba8:	00612623          	sw	t1,12(sp)
   10bac:	b40ff0ef          	jal	feec <mini_vsnprintf>
   10bb0:	01c12083          	lw	ra,28(sp)
   10bb4:	04010113          	addi	sp,sp,64
   10bb8:	00008067          	ret

00010bbc <blit_refill>:
   10bbc:	000e4737          	lui	a4,0xe4
   10bc0:	15470713          	addi	a4,a4,340 # e4154 <blit_st>
   10bc4:	03872583          	lw	a1,56(a4)
   10bc8:	f8101637          	lui	a2,0xf8101
   10bcc:	00462783          	lw	a5,4(a2) # f8101004 <__freertos_irq_stack_top+0xf8018e64>
   10bd0:	02c72683          	lw	a3,44(a4)
   10bd4:	00158593          	addi	a1,a1,1
   10bd8:	0107d793          	srli	a5,a5,0x10
   10bdc:	02b72c23          	sw	a1,56(a4)
   10be0:	00d7fa63          	bgeu	a5,a3,10bf4 <blit_refill+0x38>
   10be4:	03c72603          	lw	a2,60(a4)
   10be8:	40f686b3          	sub	a3,a3,a5
   10bec:	00d67463          	bgeu	a2,a3,10bf4 <blit_refill+0x38>
   10bf0:	02d72e23          	sw	a3,60(a4)
   10bf4:	04079263          	bnez	a5,10c38 <blit_refill+0x7c>
   10bf8:	f8b0c7b7          	lui	a5,0xf8b0c
   10bfc:	ff87a503          	lw	a0,-8(a5) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
   10c00:	f81016b7          	lui	a3,0xf8101
   10c04:	00468693          	addi	a3,a3,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
   10c08:	0006a783          	lw	a5,0(a3)
   10c0c:	0107d793          	srli	a5,a5,0x10
   10c10:	fe078ce3          	beqz	a5,10c08 <blit_refill+0x4c>
   10c14:	f8b0c5b7          	lui	a1,0xf8b0c
   10c18:	ff85a683          	lw	a3,-8(a1) # f8b0bff8 <__freertos_irq_stack_top+0xf8a23e58>
   10c1c:	03072803          	lw	a6,48(a4)
   10c20:	03472603          	lw	a2,52(a4)
   10c24:	010686b3          	add	a3,a3,a6
   10c28:	40a686b3          	sub	a3,a3,a0
   10c2c:	00160613          	addi	a2,a2,1
   10c30:	02d72823          	sw	a3,48(a4)
   10c34:	02c72a23          	sw	a2,52(a4)
   10c38:	04000693          	li	a3,64
   10c3c:	00f6f463          	bgeu	a3,a5,10c44 <blit_refill+0x88>
   10c40:	04000793          	li	a5,64
   10c44:	02f72223          	sw	a5,36(a4)
   10c48:	00008067          	ret

00010c4c <blit_queue_used>:
   10c4c:	f81016b7          	lui	a3,0xf8101
   10c50:	000e4737          	lui	a4,0xe4
   10c54:	0046a783          	lw	a5,4(a3) # f8101004 <__freertos_irq_stack_top+0xf8018e64>
   10c58:	15470713          	addi	a4,a4,340 # e4154 <blit_st>
   10c5c:	02c72603          	lw	a2,44(a4)
   10c60:	0107d793          	srli	a5,a5,0x10
   10c64:	00000513          	li	a0,0
   10c68:	00c7fa63          	bgeu	a5,a2,10c7c <blit_queue_used+0x30>
   10c6c:	03c72683          	lw	a3,60(a4)
   10c70:	40f60533          	sub	a0,a2,a5
   10c74:	00a6f463          	bgeu	a3,a0,10c7c <blit_queue_used+0x30>
   10c78:	02a72e23          	sw	a0,60(a4)
   10c7c:	04000693          	li	a3,64
   10c80:	00f6f463          	bgeu	a3,a5,10c88 <blit_queue_used+0x3c>
   10c84:	04000793          	li	a5,64
   10c88:	02f72223          	sw	a5,36(a4)
   10c8c:	00008067          	ret

00010c90 <blit_init>:
   10c90:	f81017b7          	lui	a5,0xf8101
   10c94:	0007a703          	lw	a4,0(a5) # f8101000 <__freertos_irq_stack_top+0xf8018e60>
   10c98:	424c57b7          	lui	a5,0x424c5
   10c9c:	43178793          	addi	a5,a5,1073 # 424c5431 <__freertos_irq_stack_top+0x423dd291>
   10ca0:	00000513          	li	a0,0
   10ca4:	0ef71463          	bne	a4,a5,10d8c <blit_init+0xfc>
   10ca8:	f8101737          	lui	a4,0xf8101
   10cac:	00470713          	addi	a4,a4,4 # f8101004 <__freertos_irq_stack_top+0xf8018e64>
   10cb0:	00072783          	lw	a5,0(a4)
   10cb4:	0017f793          	andi	a5,a5,1
   10cb8:	fe079ce3          	bnez	a5,10cb0 <blit_init+0x20>
   10cbc:	000e47b7          	lui	a5,0xe4
   10cc0:	15478793          	addi	a5,a5,340 # e4154 <blit_st>
   10cc4:	0207a223          	sw	zero,36(a5)
   10cc8:	f8101737          	lui	a4,0xf8101
   10ccc:	00300693          	li	a3,3
   10cd0:	00d72423          	sw	a3,8(a4) # f8101008 <__freertos_irq_stack_top+0xf8018e68>
   10cd4:	0007a023          	sw	zero,0(a5)
   10cd8:	f8101eb7          	lui	t4,0xf8101
   10cdc:	000ea823          	sw	zero,16(t4) # f8101010 <__freertos_irq_stack_top+0xf8018e70>
   10ce0:	0007a223          	sw	zero,4(a5)
   10ce4:	f8101e37          	lui	t3,0xf8101
   10ce8:	000e2a23          	sw	zero,20(t3) # f8101014 <__freertos_irq_stack_top+0xf8018e74>
   10cec:	0007a423          	sw	zero,8(a5)
   10cf0:	f8101337          	lui	t1,0xf8101
   10cf4:	00032c23          	sw	zero,24(t1) # f8101018 <__freertos_irq_stack_top+0xf8018e78>
   10cf8:	0007a623          	sw	zero,12(a5)
   10cfc:	f81018b7          	lui	a7,0xf8101
   10d00:	0008ae23          	sw	zero,28(a7) # f810101c <__freertos_irq_stack_top+0xf8018e7c>
   10d04:	0007a823          	sw	zero,16(a5)
   10d08:	f8101837          	lui	a6,0xf8101
   10d0c:	02082023          	sw	zero,32(a6) # f8101020 <__freertos_irq_stack_top+0xf8018e80>
   10d10:	0007aa23          	sw	zero,20(a5)
   10d14:	f8101537          	lui	a0,0xf8101
   10d18:	02052223          	sw	zero,36(a0) # f8101024 <__freertos_irq_stack_top+0xf8018e84>
   10d1c:	0007ac23          	sw	zero,24(a5)
   10d20:	f81015b7          	lui	a1,0xf8101
   10d24:	0205a423          	sw	zero,40(a1) # f8101028 <__freertos_irq_stack_top+0xf8018e88>
   10d28:	0007ae23          	sw	zero,28(a5)
   10d2c:	f8101637          	lui	a2,0xf8101
   10d30:	00010737          	lui	a4,0x10
   10d34:	02062623          	sw	zero,44(a2) # f810102c <__freertos_irq_stack_top+0xf8018e8c>
   10d38:	81f70713          	addi	a4,a4,-2017 # f81f <demo_init+0x19b>
   10d3c:	f81016b7          	lui	a3,0xf8101
   10d40:	02e7a023          	sw	a4,32(a5)
   10d44:	f8101f37          	lui	t5,0xf8101
   10d48:	02e6a823          	sw	a4,48(a3) # f8101030 <__freertos_irq_stack_top+0xf8018e90>
   10d4c:	00cf2703          	lw	a4,12(t5) # f810100c <__freertos_irq_stack_top+0xf8018e6c>
   10d50:	00e00f13          	li	t5,14
   10d54:	10000693          	li	a3,256
   10d58:	0ff77713          	zext.b	a4,a4
   10d5c:	fff70e93          	addi	t4,a4,-1
   10d60:	01df6663          	bltu	t5,t4,10d6c <blit_init+0xdc>
   10d64:	00100693          	li	a3,1
   10d68:	00e696b3          	sll	a3,a3,a4
   10d6c:	02d7a623          	sw	a3,44(a5)
   10d70:	0207a223          	sw	zero,36(a5)
   10d74:	0207a423          	sw	zero,40(a5)
   10d78:	0207ae23          	sw	zero,60(a5)
   10d7c:	0207ac23          	sw	zero,56(a5)
   10d80:	0207aa23          	sw	zero,52(a5)
   10d84:	0207a823          	sw	zero,48(a5)
   10d88:	00100513          	li	a0,1
   10d8c:	00008067          	ret

00010d90 <__divdi3>:
   10d90:	00000813          	li	a6,0
   10d94:	1205c663          	bltz	a1,10ec0 <__divdi3+0x130>
   10d98:	0006dc63          	bgez	a3,10db0 <__divdi3+0x20>
   10d9c:	00c037b3          	snez	a5,a2
   10da0:	40d006b3          	neg	a3,a3
   10da4:	fff84813          	not	a6,a6
   10da8:	40f686b3          	sub	a3,a3,a5
   10dac:	40c00633          	neg	a2,a2
   10db0:	00060893          	mv	a7,a2
   10db4:	00050713          	mv	a4,a0
   10db8:	00058313          	mv	t1,a1
   10dbc:	0e069a63          	bnez	a3,10eb0 <__divdi3+0x120>
   10dc0:	16c5f463          	bgeu	a1,a2,10f28 <__divdi3+0x198>
   10dc4:	000107b7          	lui	a5,0x10
   10dc8:	22f66463          	bltu	a2,a5,10ff0 <__divdi3+0x260>
   10dcc:	010007b7          	lui	a5,0x1000
   10dd0:	01800693          	li	a3,24
   10dd4:	00f67463          	bgeu	a2,a5,10ddc <__divdi3+0x4c>
   10dd8:	01000693          	li	a3,16
   10ddc:	00d65e33          	srl	t3,a2,a3
   10de0:	00002797          	auipc	a5,0x2
   10de4:	47478793          	addi	a5,a5,1140 # 13254 <__clz_tab>
   10de8:	01c787b3          	add	a5,a5,t3
   10dec:	0007c783          	lbu	a5,0(a5)
   10df0:	02000e13          	li	t3,32
   10df4:	00d787b3          	add	a5,a5,a3
   10df8:	40fe06b3          	sub	a3,t3,a5
   10dfc:	00fe0c63          	beq	t3,a5,10e14 <__divdi3+0x84>
   10e00:	00d59333          	sll	t1,a1,a3
   10e04:	00f557b3          	srl	a5,a0,a5
   10e08:	00d618b3          	sll	a7,a2,a3
   10e0c:	0067e333          	or	t1,a5,t1
   10e10:	00d51733          	sll	a4,a0,a3
   10e14:	0108d613          	srli	a2,a7,0x10
   10e18:	02c35533          	divu	a0,t1,a2
   10e1c:	01089693          	slli	a3,a7,0x10
   10e20:	0106d693          	srli	a3,a3,0x10
   10e24:	01075793          	srli	a5,a4,0x10
   10e28:	02c37333          	remu	t1,t1,a2
   10e2c:	02a685b3          	mul	a1,a3,a0
   10e30:	01031313          	slli	t1,t1,0x10
   10e34:	0067e7b3          	or	a5,a5,t1
   10e38:	00b7fc63          	bgeu	a5,a1,10e50 <__divdi3+0xc0>
   10e3c:	00f887b3          	add	a5,a7,a5
   10e40:	fff50313          	addi	t1,a0,-1
   10e44:	0117e463          	bltu	a5,a7,10e4c <__divdi3+0xbc>
   10e48:	42b7e663          	bltu	a5,a1,11274 <__divdi3+0x4e4>
   10e4c:	00030513          	mv	a0,t1
   10e50:	40b787b3          	sub	a5,a5,a1
   10e54:	02c7d5b3          	divu	a1,a5,a2
   10e58:	01071713          	slli	a4,a4,0x10
   10e5c:	01075713          	srli	a4,a4,0x10
   10e60:	02c7f7b3          	remu	a5,a5,a2
   10e64:	02b686b3          	mul	a3,a3,a1
   10e68:	01079793          	slli	a5,a5,0x10
   10e6c:	00f76733          	or	a4,a4,a5
   10e70:	00d77e63          	bgeu	a4,a3,10e8c <__divdi3+0xfc>
   10e74:	00e88733          	add	a4,a7,a4
   10e78:	fff58793          	addi	a5,a1,-1
   10e7c:	01176663          	bltu	a4,a7,10e88 <__divdi3+0xf8>
   10e80:	ffe58593          	addi	a1,a1,-2
   10e84:	00d76463          	bltu	a4,a3,10e8c <__divdi3+0xfc>
   10e88:	00078593          	mv	a1,a5
   10e8c:	01051513          	slli	a0,a0,0x10
   10e90:	00b56533          	or	a0,a0,a1
   10e94:	00000593          	li	a1,0
   10e98:	00080a63          	beqz	a6,10eac <__divdi3+0x11c>
   10e9c:	00a037b3          	snez	a5,a0
   10ea0:	40b005b3          	neg	a1,a1
   10ea4:	40f585b3          	sub	a1,a1,a5
   10ea8:	40a00533          	neg	a0,a0
   10eac:	00008067          	ret
   10eb0:	02d5f463          	bgeu	a1,a3,10ed8 <__divdi3+0x148>
   10eb4:	00000593          	li	a1,0
   10eb8:	00000513          	li	a0,0
   10ebc:	fddff06f          	j	10e98 <__divdi3+0x108>
   10ec0:	00a037b3          	snez	a5,a0
   10ec4:	40b005b3          	neg	a1,a1
   10ec8:	40f585b3          	sub	a1,a1,a5
   10ecc:	40a00533          	neg	a0,a0
   10ed0:	fff00813          	li	a6,-1
   10ed4:	ec5ff06f          	j	10d98 <__divdi3+0x8>
   10ed8:	000107b7          	lui	a5,0x10
   10edc:	1ef6e863          	bltu	a3,a5,110cc <__divdi3+0x33c>
   10ee0:	01000737          	lui	a4,0x1000
   10ee4:	01800793          	li	a5,24
   10ee8:	00e6f463          	bgeu	a3,a4,10ef0 <__divdi3+0x160>
   10eec:	01000793          	li	a5,16
   10ef0:	00f6d8b3          	srl	a7,a3,a5
   10ef4:	00002717          	auipc	a4,0x2
   10ef8:	36070713          	addi	a4,a4,864 # 13254 <__clz_tab>
   10efc:	01170733          	add	a4,a4,a7
   10f00:	00074703          	lbu	a4,0(a4)
   10f04:	02000313          	li	t1,32
   10f08:	00f70733          	add	a4,a4,a5
   10f0c:	40e308b3          	sub	a7,t1,a4
   10f10:	1ee31663          	bne	t1,a4,110fc <__divdi3+0x36c>
   10f14:	32b6e463          	bltu	a3,a1,1123c <__divdi3+0x4ac>
   10f18:	00c53533          	sltu	a0,a0,a2
   10f1c:	00153513          	seqz	a0,a0
   10f20:	00000593          	li	a1,0
   10f24:	f75ff06f          	j	10e98 <__divdi3+0x108>
   10f28:	0c060c63          	beqz	a2,11000 <__divdi3+0x270>
   10f2c:	000107b7          	lui	a5,0x10
   10f30:	2ef67c63          	bgeu	a2,a5,11228 <__divdi3+0x498>
   10f34:	10063313          	sltiu	t1,a2,256
   10f38:	00133313          	seqz	t1,t1
   10f3c:	00331313          	slli	t1,t1,0x3
   10f40:	006656b3          	srl	a3,a2,t1
   10f44:	00002797          	auipc	a5,0x2
   10f48:	31078793          	addi	a5,a5,784 # 13254 <__clz_tab>
   10f4c:	00d787b3          	add	a5,a5,a3
   10f50:	0007c683          	lbu	a3,0(a5)
   10f54:	02000793          	li	a5,32
   10f58:	006686b3          	add	a3,a3,t1
   10f5c:	40d78eb3          	sub	t4,a5,a3
   10f60:	0cd79463          	bne	a5,a3,11028 <__divdi3+0x298>
   10f64:	40c586b3          	sub	a3,a1,a2
   10f68:	01065313          	srli	t1,a2,0x10
   10f6c:	01061613          	slli	a2,a2,0x10
   10f70:	01065613          	srli	a2,a2,0x10
   10f74:	00100593          	li	a1,1
   10f78:	0266d533          	divu	a0,a3,t1
   10f7c:	01075793          	srli	a5,a4,0x10
   10f80:	0266f6b3          	remu	a3,a3,t1
   10f84:	02c50e33          	mul	t3,a0,a2
   10f88:	01069693          	slli	a3,a3,0x10
   10f8c:	00d7e7b3          	or	a5,a5,a3
   10f90:	01c7fc63          	bgeu	a5,t3,10fa8 <__divdi3+0x218>
   10f94:	00f887b3          	add	a5,a7,a5
   10f98:	fff50693          	addi	a3,a0,-1
   10f9c:	0117e463          	bltu	a5,a7,10fa4 <__divdi3+0x214>
   10fa0:	2dc7e463          	bltu	a5,t3,11268 <__divdi3+0x4d8>
   10fa4:	00068513          	mv	a0,a3
   10fa8:	41c787b3          	sub	a5,a5,t3
   10fac:	0267d6b3          	divu	a3,a5,t1
   10fb0:	01071713          	slli	a4,a4,0x10
   10fb4:	01075713          	srli	a4,a4,0x10
   10fb8:	0267f7b3          	remu	a5,a5,t1
   10fbc:	02c68633          	mul	a2,a3,a2
   10fc0:	01079793          	slli	a5,a5,0x10
   10fc4:	00f76733          	or	a4,a4,a5
   10fc8:	00c77e63          	bgeu	a4,a2,10fe4 <__divdi3+0x254>
   10fcc:	00e88733          	add	a4,a7,a4
   10fd0:	fff68793          	addi	a5,a3,-1
   10fd4:	01176663          	bltu	a4,a7,10fe0 <__divdi3+0x250>
   10fd8:	ffe68693          	addi	a3,a3,-2
   10fdc:	00c76463          	bltu	a4,a2,10fe4 <__divdi3+0x254>
   10fe0:	00078693          	mv	a3,a5
   10fe4:	01051513          	slli	a0,a0,0x10
   10fe8:	00d56533          	or	a0,a0,a3
   10fec:	eadff06f          	j	10e98 <__divdi3+0x108>
   10ff0:	10063693          	sltiu	a3,a2,256
   10ff4:	0016b693          	seqz	a3,a3
   10ff8:	00369693          	slli	a3,a3,0x3
   10ffc:	de1ff06f          	j	10ddc <__divdi3+0x4c>
   11000:	00000693          	li	a3,0
   11004:	00002797          	auipc	a5,0x2
   11008:	25078793          	addi	a5,a5,592 # 13254 <__clz_tab>
   1100c:	00d787b3          	add	a5,a5,a3
   11010:	0007c683          	lbu	a3,0(a5)
   11014:	00000313          	li	t1,0
   11018:	02000793          	li	a5,32
   1101c:	006686b3          	add	a3,a3,t1
   11020:	40d78eb3          	sub	t4,a5,a3
   11024:	f4d780e3          	beq	a5,a3,10f64 <__divdi3+0x1d4>
   11028:	01d618b3          	sll	a7,a2,t4
   1102c:	00d5de33          	srl	t3,a1,a3
   11030:	0108d313          	srli	t1,a7,0x10
   11034:	026e57b3          	divu	a5,t3,t1
   11038:	01089613          	slli	a2,a7,0x10
   1103c:	01065613          	srli	a2,a2,0x10
   11040:	00d556b3          	srl	a3,a0,a3
   11044:	01d51733          	sll	a4,a0,t4
   11048:	01d595b3          	sll	a1,a1,t4
   1104c:	00b6e6b3          	or	a3,a3,a1
   11050:	0106d593          	srli	a1,a3,0x10
   11054:	026e7e33          	remu	t3,t3,t1
   11058:	02f60533          	mul	a0,a2,a5
   1105c:	010e1e13          	slli	t3,t3,0x10
   11060:	01c5e5b3          	or	a1,a1,t3
   11064:	00a5fe63          	bgeu	a1,a0,11080 <__divdi3+0x2f0>
   11068:	00b885b3          	add	a1,a7,a1
   1106c:	fff78e13          	addi	t3,a5,-1
   11070:	1f15e463          	bltu	a1,a7,11258 <__divdi3+0x4c8>
   11074:	1ea5f263          	bgeu	a1,a0,11258 <__divdi3+0x4c8>
   11078:	ffe78793          	addi	a5,a5,-2
   1107c:	011585b3          	add	a1,a1,a7
   11080:	40a585b3          	sub	a1,a1,a0
   11084:	0265d533          	divu	a0,a1,t1
   11088:	01069693          	slli	a3,a3,0x10
   1108c:	0106d693          	srli	a3,a3,0x10
   11090:	0265f5b3          	remu	a1,a1,t1
   11094:	02a60e33          	mul	t3,a2,a0
   11098:	01059593          	slli	a1,a1,0x10
   1109c:	00b6e6b3          	or	a3,a3,a1
   110a0:	01c6fe63          	bgeu	a3,t3,110bc <__divdi3+0x32c>
   110a4:	00d886b3          	add	a3,a7,a3
   110a8:	fff50593          	addi	a1,a0,-1
   110ac:	1916ee63          	bltu	a3,a7,11248 <__divdi3+0x4b8>
   110b0:	19c6fc63          	bgeu	a3,t3,11248 <__divdi3+0x4b8>
   110b4:	ffe50513          	addi	a0,a0,-2
   110b8:	011686b3          	add	a3,a3,a7
   110bc:	01079593          	slli	a1,a5,0x10
   110c0:	41c686b3          	sub	a3,a3,t3
   110c4:	00a5e5b3          	or	a1,a1,a0
   110c8:	eb1ff06f          	j	10f78 <__divdi3+0x1e8>
   110cc:	1006b793          	sltiu	a5,a3,256
   110d0:	0017b793          	seqz	a5,a5
   110d4:	00379793          	slli	a5,a5,0x3
   110d8:	00f6d8b3          	srl	a7,a3,a5
   110dc:	00002717          	auipc	a4,0x2
   110e0:	17870713          	addi	a4,a4,376 # 13254 <__clz_tab>
   110e4:	01170733          	add	a4,a4,a7
   110e8:	00074703          	lbu	a4,0(a4)
   110ec:	02000313          	li	t1,32
   110f0:	00f70733          	add	a4,a4,a5
   110f4:	40e308b3          	sub	a7,t1,a4
   110f8:	e0e30ee3          	beq	t1,a4,10f14 <__divdi3+0x184>
   110fc:	00e65eb3          	srl	t4,a2,a4
   11100:	011696b3          	sll	a3,a3,a7
   11104:	00deeeb3          	or	t4,t4,a3
   11108:	00e5d333          	srl	t1,a1,a4
   1110c:	010edf13          	srli	t5,t4,0x10
   11110:	03e357b3          	divu	a5,t1,t5
   11114:	010e9e13          	slli	t3,t4,0x10
   11118:	011595b3          	sll	a1,a1,a7
   1111c:	010e5e13          	srli	t3,t3,0x10
   11120:	00e55733          	srl	a4,a0,a4
   11124:	00b76733          	or	a4,a4,a1
   11128:	01075693          	srli	a3,a4,0x10
   1112c:	01161633          	sll	a2,a2,a7
   11130:	03e37333          	remu	t1,t1,t5
   11134:	02fe05b3          	mul	a1,t3,a5
   11138:	01031313          	slli	t1,t1,0x10
   1113c:	0066e6b3          	or	a3,a3,t1
   11140:	00b6fe63          	bgeu	a3,a1,1115c <__divdi3+0x3cc>
   11144:	00de86b3          	add	a3,t4,a3
   11148:	fff78313          	addi	t1,a5,-1
   1114c:	11d6ea63          	bltu	a3,t4,11260 <__divdi3+0x4d0>
   11150:	10b6f863          	bgeu	a3,a1,11260 <__divdi3+0x4d0>
   11154:	ffe78793          	addi	a5,a5,-2
   11158:	01d686b3          	add	a3,a3,t4
   1115c:	40b686b3          	sub	a3,a3,a1
   11160:	03e6d5b3          	divu	a1,a3,t5
   11164:	01071713          	slli	a4,a4,0x10
   11168:	01075713          	srli	a4,a4,0x10
   1116c:	03e6f6b3          	remu	a3,a3,t5
   11170:	02be0333          	mul	t1,t3,a1
   11174:	01069693          	slli	a3,a3,0x10
   11178:	00d76733          	or	a4,a4,a3
   1117c:	00677e63          	bgeu	a4,t1,11198 <__divdi3+0x408>
   11180:	00ee8733          	add	a4,t4,a4
   11184:	fff58693          	addi	a3,a1,-1
   11188:	0dd76463          	bltu	a4,t4,11250 <__divdi3+0x4c0>
   1118c:	0c677263          	bgeu	a4,t1,11250 <__divdi3+0x4c0>
   11190:	ffe58593          	addi	a1,a1,-2
   11194:	01d70733          	add	a4,a4,t4
   11198:	01079793          	slli	a5,a5,0x10
   1119c:	00010f37          	lui	t5,0x10
   111a0:	00b7e7b3          	or	a5,a5,a1
   111a4:	ffff0693          	addi	a3,t5,-1 # ffff <mini_vsnprintf+0x113>
   111a8:	00d7f5b3          	and	a1,a5,a3
   111ac:	0107de13          	srli	t3,a5,0x10
   111b0:	00d676b3          	and	a3,a2,a3
   111b4:	01065613          	srli	a2,a2,0x10
   111b8:	02d58eb3          	mul	t4,a1,a3
   111bc:	40670733          	sub	a4,a4,t1
   111c0:	02de06b3          	mul	a3,t3,a3
   111c4:	010ed313          	srli	t1,t4,0x10
   111c8:	02c585b3          	mul	a1,a1,a2
   111cc:	00d585b3          	add	a1,a1,a3
   111d0:	00b305b3          	add	a1,t1,a1
   111d4:	02ce0e33          	mul	t3,t3,a2
   111d8:	00d5f463          	bgeu	a1,a3,111e0 <__divdi3+0x450>
   111dc:	01ee0e33          	add	t3,t3,t5
   111e0:	0105d693          	srli	a3,a1,0x10
   111e4:	01c686b3          	add	a3,a3,t3
   111e8:	02d76a63          	bltu	a4,a3,1121c <__divdi3+0x48c>
   111ec:	00d70863          	beq	a4,a3,111fc <__divdi3+0x46c>
   111f0:	00078513          	mv	a0,a5
   111f4:	00000593          	li	a1,0
   111f8:	ca1ff06f          	j	10e98 <__divdi3+0x108>
   111fc:	000106b7          	lui	a3,0x10
   11200:	fff68693          	addi	a3,a3,-1 # ffff <mini_vsnprintf+0x113>
   11204:	00d5f733          	and	a4,a1,a3
   11208:	01071713          	slli	a4,a4,0x10
   1120c:	00defeb3          	and	t4,t4,a3
   11210:	01151533          	sll	a0,a0,a7
   11214:	01d70733          	add	a4,a4,t4
   11218:	fce57ce3          	bgeu	a0,a4,111f0 <__divdi3+0x460>
   1121c:	fff78513          	addi	a0,a5,-1
   11220:	00000593          	li	a1,0
   11224:	c75ff06f          	j	10e98 <__divdi3+0x108>
   11228:	010007b7          	lui	a5,0x1000
   1122c:	04f67a63          	bgeu	a2,a5,11280 <__divdi3+0x4f0>
   11230:	01065693          	srli	a3,a2,0x10
   11234:	01000313          	li	t1,16
   11238:	d0dff06f          	j	10f44 <__divdi3+0x1b4>
   1123c:	00000593          	li	a1,0
   11240:	00100513          	li	a0,1
   11244:	c55ff06f          	j	10e98 <__divdi3+0x108>
   11248:	00058513          	mv	a0,a1
   1124c:	e71ff06f          	j	110bc <__divdi3+0x32c>
   11250:	00068593          	mv	a1,a3
   11254:	f45ff06f          	j	11198 <__divdi3+0x408>
   11258:	000e0793          	mv	a5,t3
   1125c:	e25ff06f          	j	11080 <__divdi3+0x2f0>
   11260:	00030793          	mv	a5,t1
   11264:	ef9ff06f          	j	1115c <__divdi3+0x3cc>
   11268:	ffe50513          	addi	a0,a0,-2
   1126c:	011787b3          	add	a5,a5,a7
   11270:	d39ff06f          	j	10fa8 <__divdi3+0x218>
   11274:	ffe50513          	addi	a0,a0,-2
   11278:	011787b3          	add	a5,a5,a7
   1127c:	bd5ff06f          	j	10e50 <__divdi3+0xc0>
   11280:	01865693          	srli	a3,a2,0x18
   11284:	01800313          	li	t1,24
   11288:	cbdff06f          	j	10f44 <__divdi3+0x1b4>

0001128c <__udivdi3>:
   1128c:	00060813          	mv	a6,a2
   11290:	00050893          	mv	a7,a0
   11294:	00058713          	mv	a4,a1
   11298:	0e069063          	bnez	a3,11378 <__udivdi3+0xec>
   1129c:	12c5fe63          	bgeu	a1,a2,113d8 <__udivdi3+0x14c>
   112a0:	000107b7          	lui	a5,0x10
   112a4:	1ef66e63          	bltu	a2,a5,114a0 <__udivdi3+0x214>
   112a8:	010007b7          	lui	a5,0x1000
   112ac:	01800693          	li	a3,24
   112b0:	00f67463          	bgeu	a2,a5,112b8 <__udivdi3+0x2c>
   112b4:	01000693          	li	a3,16
   112b8:	00d65333          	srl	t1,a2,a3
   112bc:	00002797          	auipc	a5,0x2
   112c0:	f9878793          	addi	a5,a5,-104 # 13254 <__clz_tab>
   112c4:	006787b3          	add	a5,a5,t1
   112c8:	0007c783          	lbu	a5,0(a5)
   112cc:	02000313          	li	t1,32
   112d0:	00d787b3          	add	a5,a5,a3
   112d4:	40f306b3          	sub	a3,t1,a5
   112d8:	00f30c63          	beq	t1,a5,112f0 <__udivdi3+0x64>
   112dc:	00d59733          	sll	a4,a1,a3
   112e0:	00f557b3          	srl	a5,a0,a5
   112e4:	00d61833          	sll	a6,a2,a3
   112e8:	00e7e733          	or	a4,a5,a4
   112ec:	00d518b3          	sll	a7,a0,a3
   112f0:	01085613          	srli	a2,a6,0x10
   112f4:	02c75533          	divu	a0,a4,a2
   112f8:	01081693          	slli	a3,a6,0x10
   112fc:	0106d693          	srli	a3,a3,0x10
   11300:	0108d793          	srli	a5,a7,0x10
   11304:	02c77733          	remu	a4,a4,a2
   11308:	02a685b3          	mul	a1,a3,a0
   1130c:	01071713          	slli	a4,a4,0x10
   11310:	00e7e7b3          	or	a5,a5,a4
   11314:	00b7fc63          	bgeu	a5,a1,1132c <__udivdi3+0xa0>
   11318:	00f807b3          	add	a5,a6,a5
   1131c:	fff50713          	addi	a4,a0,-1
   11320:	0107e463          	bltu	a5,a6,11328 <__udivdi3+0x9c>
   11324:	40b7e063          	bltu	a5,a1,11724 <__udivdi3+0x498>
   11328:	00070513          	mv	a0,a4
   1132c:	40b787b3          	sub	a5,a5,a1
   11330:	02c7d733          	divu	a4,a5,a2
   11334:	01089893          	slli	a7,a7,0x10
   11338:	0108d893          	srli	a7,a7,0x10
   1133c:	02c7f7b3          	remu	a5,a5,a2
   11340:	02e686b3          	mul	a3,a3,a4
   11344:	01079793          	slli	a5,a5,0x10
   11348:	00f8e8b3          	or	a7,a7,a5
   1134c:	00d8fe63          	bgeu	a7,a3,11368 <__udivdi3+0xdc>
   11350:	011808b3          	add	a7,a6,a7
   11354:	fff70793          	addi	a5,a4,-1
   11358:	0108e663          	bltu	a7,a6,11364 <__udivdi3+0xd8>
   1135c:	ffe70713          	addi	a4,a4,-2
   11360:	00d8e463          	bltu	a7,a3,11368 <__udivdi3+0xdc>
   11364:	00078713          	mv	a4,a5
   11368:	01051513          	slli	a0,a0,0x10
   1136c:	00e56533          	or	a0,a0,a4
   11370:	00000593          	li	a1,0
   11374:	00008067          	ret
   11378:	00d5f863          	bgeu	a1,a3,11388 <__udivdi3+0xfc>
   1137c:	00000593          	li	a1,0
   11380:	00000513          	li	a0,0
   11384:	00008067          	ret
   11388:	000107b7          	lui	a5,0x10
   1138c:	1ef6e863          	bltu	a3,a5,1157c <__udivdi3+0x2f0>
   11390:	01000737          	lui	a4,0x1000
   11394:	01800793          	li	a5,24
   11398:	00e6f463          	bgeu	a3,a4,113a0 <__udivdi3+0x114>
   1139c:	01000793          	li	a5,16
   113a0:	00f6d833          	srl	a6,a3,a5
   113a4:	00002717          	auipc	a4,0x2
   113a8:	eb070713          	addi	a4,a4,-336 # 13254 <__clz_tab>
   113ac:	01070733          	add	a4,a4,a6
   113b0:	00074703          	lbu	a4,0(a4)
   113b4:	02000893          	li	a7,32
   113b8:	00f70733          	add	a4,a4,a5
   113bc:	40e88833          	sub	a6,a7,a4
   113c0:	1ee89663          	bne	a7,a4,115ac <__udivdi3+0x320>
   113c4:	32b6e463          	bltu	a3,a1,116ec <__udivdi3+0x460>
   113c8:	00c53533          	sltu	a0,a0,a2
   113cc:	00153513          	seqz	a0,a0
   113d0:	00000593          	li	a1,0
   113d4:	00008067          	ret
   113d8:	0c060c63          	beqz	a2,114b0 <__udivdi3+0x224>
   113dc:	000107b7          	lui	a5,0x10
   113e0:	2ef67c63          	bgeu	a2,a5,116d8 <__udivdi3+0x44c>
   113e4:	10063713          	sltiu	a4,a2,256
   113e8:	00173713          	seqz	a4,a4
   113ec:	00371713          	slli	a4,a4,0x3
   113f0:	00e656b3          	srl	a3,a2,a4
   113f4:	00002797          	auipc	a5,0x2
   113f8:	e6078793          	addi	a5,a5,-416 # 13254 <__clz_tab>
   113fc:	00d787b3          	add	a5,a5,a3
   11400:	0007c783          	lbu	a5,0(a5)
   11404:	02000693          	li	a3,32
   11408:	00e787b3          	add	a5,a5,a4
   1140c:	40f68eb3          	sub	t4,a3,a5
   11410:	0cf69463          	bne	a3,a5,114d8 <__udivdi3+0x24c>
   11414:	40c587b3          	sub	a5,a1,a2
   11418:	01065693          	srli	a3,a2,0x10
   1141c:	01061613          	slli	a2,a2,0x10
   11420:	01065613          	srli	a2,a2,0x10
   11424:	00100593          	li	a1,1
   11428:	02d7d533          	divu	a0,a5,a3
   1142c:	0108d713          	srli	a4,a7,0x10
   11430:	02d7f7b3          	remu	a5,a5,a3
   11434:	02c50333          	mul	t1,a0,a2
   11438:	01079793          	slli	a5,a5,0x10
   1143c:	00f767b3          	or	a5,a4,a5
   11440:	0067fc63          	bgeu	a5,t1,11458 <__udivdi3+0x1cc>
   11444:	00f807b3          	add	a5,a6,a5
   11448:	fff50713          	addi	a4,a0,-1
   1144c:	0107e463          	bltu	a5,a6,11454 <__udivdi3+0x1c8>
   11450:	2c67e463          	bltu	a5,t1,11718 <__udivdi3+0x48c>
   11454:	00070513          	mv	a0,a4
   11458:	406787b3          	sub	a5,a5,t1
   1145c:	02d7d733          	divu	a4,a5,a3
   11460:	01089893          	slli	a7,a7,0x10
   11464:	0108d893          	srli	a7,a7,0x10
   11468:	02d7f7b3          	remu	a5,a5,a3
   1146c:	02c70633          	mul	a2,a4,a2
   11470:	01079793          	slli	a5,a5,0x10
   11474:	00f8e8b3          	or	a7,a7,a5
   11478:	00c8fe63          	bgeu	a7,a2,11494 <__udivdi3+0x208>
   1147c:	011808b3          	add	a7,a6,a7
   11480:	fff70793          	addi	a5,a4,-1
   11484:	0108e663          	bltu	a7,a6,11490 <__udivdi3+0x204>
   11488:	ffe70713          	addi	a4,a4,-2
   1148c:	00c8e463          	bltu	a7,a2,11494 <__udivdi3+0x208>
   11490:	00078713          	mv	a4,a5
   11494:	01051513          	slli	a0,a0,0x10
   11498:	00e56533          	or	a0,a0,a4
   1149c:	00008067          	ret
   114a0:	10063693          	sltiu	a3,a2,256
   114a4:	0016b693          	seqz	a3,a3
   114a8:	00369693          	slli	a3,a3,0x3
   114ac:	e0dff06f          	j	112b8 <__udivdi3+0x2c>
   114b0:	00000693          	li	a3,0
   114b4:	00002797          	auipc	a5,0x2
   114b8:	da078793          	addi	a5,a5,-608 # 13254 <__clz_tab>
   114bc:	00d787b3          	add	a5,a5,a3
   114c0:	0007c783          	lbu	a5,0(a5)
   114c4:	00000713          	li	a4,0
   114c8:	02000693          	li	a3,32
   114cc:	00e787b3          	add	a5,a5,a4
   114d0:	40f68eb3          	sub	t4,a3,a5
   114d4:	f4f680e3          	beq	a3,a5,11414 <__udivdi3+0x188>
   114d8:	01d61833          	sll	a6,a2,t4
   114dc:	00f5d333          	srl	t1,a1,a5
   114e0:	01085693          	srli	a3,a6,0x10
   114e4:	02d35e33          	divu	t3,t1,a3
   114e8:	01081613          	slli	a2,a6,0x10
   114ec:	01d595b3          	sll	a1,a1,t4
   114f0:	01065613          	srli	a2,a2,0x10
   114f4:	00f557b3          	srl	a5,a0,a5
   114f8:	00b7e7b3          	or	a5,a5,a1
   114fc:	0107d713          	srli	a4,a5,0x10
   11500:	01d518b3          	sll	a7,a0,t4
   11504:	02d37333          	remu	t1,t1,a3
   11508:	03c605b3          	mul	a1,a2,t3
   1150c:	01031313          	slli	t1,t1,0x10
   11510:	00676733          	or	a4,a4,t1
   11514:	00b77e63          	bgeu	a4,a1,11530 <__udivdi3+0x2a4>
   11518:	00e80733          	add	a4,a6,a4
   1151c:	fffe0513          	addi	a0,t3,-1
   11520:	1f076463          	bltu	a4,a6,11708 <__udivdi3+0x47c>
   11524:	1eb77263          	bgeu	a4,a1,11708 <__udivdi3+0x47c>
   11528:	ffee0e13          	addi	t3,t3,-2
   1152c:	01070733          	add	a4,a4,a6
   11530:	40b70733          	sub	a4,a4,a1
   11534:	02d75533          	divu	a0,a4,a3
   11538:	01079793          	slli	a5,a5,0x10
   1153c:	0107d793          	srli	a5,a5,0x10
   11540:	02d77733          	remu	a4,a4,a3
   11544:	02a60333          	mul	t1,a2,a0
   11548:	01071713          	slli	a4,a4,0x10
   1154c:	00e7e7b3          	or	a5,a5,a4
   11550:	0067fe63          	bgeu	a5,t1,1156c <__udivdi3+0x2e0>
   11554:	00f807b3          	add	a5,a6,a5
   11558:	fff50713          	addi	a4,a0,-1
   1155c:	1907ee63          	bltu	a5,a6,116f8 <__udivdi3+0x46c>
   11560:	1867fc63          	bgeu	a5,t1,116f8 <__udivdi3+0x46c>
   11564:	ffe50513          	addi	a0,a0,-2
   11568:	010787b3          	add	a5,a5,a6
   1156c:	010e1593          	slli	a1,t3,0x10
   11570:	406787b3          	sub	a5,a5,t1
   11574:	00a5e5b3          	or	a1,a1,a0
   11578:	eb1ff06f          	j	11428 <__udivdi3+0x19c>
   1157c:	1006b793          	sltiu	a5,a3,256
   11580:	0017b793          	seqz	a5,a5
   11584:	00379793          	slli	a5,a5,0x3
   11588:	00f6d833          	srl	a6,a3,a5
   1158c:	00002717          	auipc	a4,0x2
   11590:	cc870713          	addi	a4,a4,-824 # 13254 <__clz_tab>
   11594:	01070733          	add	a4,a4,a6
   11598:	00074703          	lbu	a4,0(a4)
   1159c:	02000893          	li	a7,32
   115a0:	00f70733          	add	a4,a4,a5
   115a4:	40e88833          	sub	a6,a7,a4
   115a8:	e0e88ee3          	beq	a7,a4,113c4 <__udivdi3+0x138>
   115ac:	00e65e33          	srl	t3,a2,a4
   115b0:	010696b3          	sll	a3,a3,a6
   115b4:	00de6e33          	or	t3,t3,a3
   115b8:	00e5d8b3          	srl	a7,a1,a4
   115bc:	010e5e93          	srli	t4,t3,0x10
   115c0:	03d8d7b3          	divu	a5,a7,t4
   115c4:	010e1313          	slli	t1,t3,0x10
   115c8:	010595b3          	sll	a1,a1,a6
   115cc:	01035313          	srli	t1,t1,0x10
   115d0:	00e55733          	srl	a4,a0,a4
   115d4:	00b76733          	or	a4,a4,a1
   115d8:	01075693          	srli	a3,a4,0x10
   115dc:	01061633          	sll	a2,a2,a6
   115e0:	03d8f8b3          	remu	a7,a7,t4
   115e4:	02f305b3          	mul	a1,t1,a5
   115e8:	01089893          	slli	a7,a7,0x10
   115ec:	0116e6b3          	or	a3,a3,a7
   115f0:	00b6fe63          	bgeu	a3,a1,1160c <__udivdi3+0x380>
   115f4:	00de06b3          	add	a3,t3,a3
   115f8:	fff78893          	addi	a7,a5,-1
   115fc:	11c6ea63          	bltu	a3,t3,11710 <__udivdi3+0x484>
   11600:	10b6f863          	bgeu	a3,a1,11710 <__udivdi3+0x484>
   11604:	ffe78793          	addi	a5,a5,-2
   11608:	01c686b3          	add	a3,a3,t3
   1160c:	40b686b3          	sub	a3,a3,a1
   11610:	03d6d5b3          	divu	a1,a3,t4
   11614:	01071713          	slli	a4,a4,0x10
   11618:	01075713          	srli	a4,a4,0x10
   1161c:	03d6f6b3          	remu	a3,a3,t4
   11620:	02b308b3          	mul	a7,t1,a1
   11624:	01069693          	slli	a3,a3,0x10
   11628:	00d76733          	or	a4,a4,a3
   1162c:	01177e63          	bgeu	a4,a7,11648 <__udivdi3+0x3bc>
   11630:	00ee0733          	add	a4,t3,a4
   11634:	fff58693          	addi	a3,a1,-1
   11638:	0dc76463          	bltu	a4,t3,11700 <__udivdi3+0x474>
   1163c:	0d177263          	bgeu	a4,a7,11700 <__udivdi3+0x474>
   11640:	ffe58593          	addi	a1,a1,-2
   11644:	01c70733          	add	a4,a4,t3
   11648:	01079793          	slli	a5,a5,0x10
   1164c:	00010eb7          	lui	t4,0x10
   11650:	00b7e7b3          	or	a5,a5,a1
   11654:	fffe8693          	addi	a3,t4,-1 # ffff <mini_vsnprintf+0x113>
   11658:	00d7f5b3          	and	a1,a5,a3
   1165c:	0107d313          	srli	t1,a5,0x10
   11660:	00d676b3          	and	a3,a2,a3
   11664:	01065613          	srli	a2,a2,0x10
   11668:	02d58e33          	mul	t3,a1,a3
   1166c:	41170733          	sub	a4,a4,a7
   11670:	02d306b3          	mul	a3,t1,a3
   11674:	010e5893          	srli	a7,t3,0x10
   11678:	02c585b3          	mul	a1,a1,a2
   1167c:	00d585b3          	add	a1,a1,a3
   11680:	00b885b3          	add	a1,a7,a1
   11684:	02c30333          	mul	t1,t1,a2
   11688:	00d5f463          	bgeu	a1,a3,11690 <__udivdi3+0x404>
   1168c:	01d30333          	add	t1,t1,t4
   11690:	0105d693          	srli	a3,a1,0x10
   11694:	006686b3          	add	a3,a3,t1
   11698:	02d76a63          	bltu	a4,a3,116cc <__udivdi3+0x440>
   1169c:	00d70863          	beq	a4,a3,116ac <__udivdi3+0x420>
   116a0:	00078513          	mv	a0,a5
   116a4:	00000593          	li	a1,0
   116a8:	00008067          	ret
   116ac:	000106b7          	lui	a3,0x10
   116b0:	fff68693          	addi	a3,a3,-1 # ffff <mini_vsnprintf+0x113>
   116b4:	00d5f733          	and	a4,a1,a3
   116b8:	01071713          	slli	a4,a4,0x10
   116bc:	00de7e33          	and	t3,t3,a3
   116c0:	01051533          	sll	a0,a0,a6
   116c4:	01c70733          	add	a4,a4,t3
   116c8:	fce57ce3          	bgeu	a0,a4,116a0 <__udivdi3+0x414>
   116cc:	fff78513          	addi	a0,a5,-1
   116d0:	00000593          	li	a1,0
   116d4:	00008067          	ret
   116d8:	010007b7          	lui	a5,0x1000
   116dc:	04f67a63          	bgeu	a2,a5,11730 <__udivdi3+0x4a4>
   116e0:	01065693          	srli	a3,a2,0x10
   116e4:	01000713          	li	a4,16
   116e8:	d0dff06f          	j	113f4 <__udivdi3+0x168>
   116ec:	00000593          	li	a1,0
   116f0:	00100513          	li	a0,1
   116f4:	00008067          	ret
   116f8:	00070513          	mv	a0,a4
   116fc:	e71ff06f          	j	1156c <__udivdi3+0x2e0>
   11700:	00068593          	mv	a1,a3
   11704:	f45ff06f          	j	11648 <__udivdi3+0x3bc>
   11708:	00050e13          	mv	t3,a0
   1170c:	e25ff06f          	j	11530 <__udivdi3+0x2a4>
   11710:	00088793          	mv	a5,a7
   11714:	ef9ff06f          	j	1160c <__udivdi3+0x380>
   11718:	ffe50513          	addi	a0,a0,-2
   1171c:	010787b3          	add	a5,a5,a6
   11720:	d39ff06f          	j	11458 <__udivdi3+0x1cc>
   11724:	ffe50513          	addi	a0,a0,-2
   11728:	010787b3          	add	a5,a5,a6
   1172c:	c01ff06f          	j	1132c <__udivdi3+0xa0>
   11730:	01865693          	srli	a3,a2,0x18
   11734:	01800713          	li	a4,24
   11738:	cbdff06f          	j	113f4 <__udivdi3+0x168>
