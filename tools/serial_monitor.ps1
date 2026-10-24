param(
    [string]$LogRoot = '',
    [int]$Baud = 115200,
    [switch]$SelfTest,
    [switch]$UiSmokeTest
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
if (!$LogRoot) { $LogRoot = Join-Path (Split-Path $PSScriptRoot -Parent) 'logs' }
$LogRoot = [IO.Path]::GetFullPath($LogRoot)
$script:utf8 = New-Object Text.UTF8Encoding($false)
$script:state = @{
    Port=$null; Raw=$null; Events=$null; Decoder=$null; Bytes=0L
    PortName=''; LogPath=''; EventPath=''; MetaPath=''; Started=''
    Session=''; BaseId=''; Label=''; Connected=$false; LastMeta=[datetime]::MinValue
}
$script:outputBox = $null
$script:statusLabel = $null
$script:pathLabel = $null
$script:connectButton = $null
$script:portBox = $null
$script:labelBox = $null
$script:nextLabel = ''
$script:entryBox = $null
$script:timer = $null




function China-Time {
    [DateTimeOffset]::UtcNow.ToOffset([TimeSpan]::FromHours(8))
}
function Set-Status([string]$Message) {
    if ($script:statusLabel) { $script:statusLabel.Text = $Message }
}
function Normalize-LogLabel([string]$Value) {
    $value = $Value.Trim()
    if (!$value -or $value -eq '未标记') { return '' }
    $value = $value.ToUpperInvariant()
    if ($value.Length -gt 16 -or $value -cnotmatch '^[A-Z0-9][A-Z0-9_-]*$') {
        throw '版本名只能包含 1–16 个英文字母、数字、_ 或 -，且须以字母或数字开头。'
    }
    return $value
}
function Get-RequestedLabel {
    if ($script:labelBox) { return Normalize-LogLabel ([string]$script:labelBox.Text) }
    return Normalize-LogLabel $script:nextLabel
}
function Save-Metadata {
    if (!$script:state.Session) { return }
    $info = [ordered]@{
        session=$script:state.Session; hardware_label=$script:state.Label
        port=$script:state.PortName; baud=$Baud
        started=$script:state.Started; updated=(China-Time).ToString('o')
        connected=$script:state.Connected; bytes_received=$script:state.Bytes
        raw_log=$script:state.LogPath; command_log=$script:state.EventPath
    }
    $json = $info | ConvertTo-Json
    [IO.File]::WriteAllText($script:state.MetaPath, $json, $script:utf8)
    [IO.File]::WriteAllText((Join-Path $LogRoot 'latest.json'), $json, $script:utf8)
    $script:state.LastMeta = [datetime]::UtcNow
}
function Write-Event([string]$Action, [string]$Detail) {
    if ($script:state.Events) {
        $safe = $Detail.Replace("`t", ' ').Replace("`r", ' ').Replace("`n", ' ')
        $script:state.Events.WriteLine("$((China-Time).ToString('o'))`t$Action`t$safe")
    }
}
function Start-LogSession([string]$PortName, [string]$Action='CONNECT') {
    $label=Get-RequestedLabel
    $day = Join-Path $LogRoot (China-Time).ToString('yyyyMMdd')
    [void][IO.Directory]::CreateDirectory($day)
    $baseId = 'uart_{0}_{1}_{2}' -f (China-Time).ToString('yyyyMMdd_HHmmss_fff'), $PortName, ([guid]::NewGuid().ToString('N').Substring(0,6))
    $id = if ($label) { $baseId+'_'+$label } else { $baseId }
    $script:state.Session=$id
    $script:state.BaseId=$baseId
    $script:state.Label=$label
    $script:state.PortName=$PortName
    $script:state.Started=(China-Time).ToString('o')
    $script:state.Bytes=0L
    $script:state.LogPath=Join-Path $day ($id+'.txt')
    $script:state.EventPath=Join-Path $day ($id+'.events.tsv')
    $script:state.MetaPath=Join-Path $day ($id+'.json')
    $script:state.Decoder=$script:utf8.GetDecoder()
    try {
        $script:state.Raw=[IO.File]::Open($script:state.LogPath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
        $script:state.Events=New-Object IO.StreamWriter($script:state.EventPath, $false, $script:utf8)
        $script:state.Events.AutoFlush=$true
        $script:state.Events.WriteLine("time_china`taction`tdetail")
        $script:state.Connected=$true
        Write-Event $Action "$PortName $Baud 8N1; hardware_label=$label (user-entered); receive-only until an explicit button/Send"
        Save-Metadata
        if ($script:pathLabel) { $script:pathLabel.Text='自动保存：'+$script:state.LogPath }
    } catch {
        if ($script:state.Raw) { $script:state.Raw.Dispose(); $script:state.Raw=$null }
        if ($script:state.Events) { $script:state.Events.Dispose(); $script:state.Events=$null }
        $script:state.Connected=$false
        throw
    }
}
function Set-CurrentLogLabel([string]$Value) {
    $label=Normalize-LogLabel $Value
    if (!$script:state.Session -or !$script:state.Raw) {
        Set-Status '当前没有正在记录的文件；连接或点击新记录后，版本名会自动写入文件名。'
        return $false
    }
    $script:state.Label=$label
    Write-Event 'LABEL' "hardware_label=$label (user-entered)"
    Save-Metadata
    $name=if ($label) { $label } else { '未标记' }
    if ($script:pathLabel) { $script:pathLabel.Text=('当前日志：'+$script:state.LogPath+'  →  结束时标记 '+$name) }
    Set-Status ('当前记录已标记为 '+$name+'；断开或新记录时自动整理文件名，串口保持连接。')
    return $true
}
function Rename-ClosedLogFiles {
    if (!$script:state.BaseId -or !$script:state.MetaPath) { return }
    $targetId=if ($script:state.Label) { $script:state.BaseId+'_'+$script:state.Label } else { $script:state.BaseId }
    if ($targetId -eq $script:state.Session) { return }
    $day=Split-Path $script:state.MetaPath -Parent
    $moves=@(
        @{ Source=$script:state.LogPath; Target=(Join-Path $day ($targetId+'.txt')) },
        @{ Source=$script:state.EventPath; Target=(Join-Path $day ($targetId+'.events.tsv')) },
        @{ Source=$script:state.MetaPath; Target=(Join-Path $day ($targetId+'.json')) }
    )
    foreach ($move in $moves) {
        if (Test-Path -LiteralPath $move.Target) { throw ('目标日志已存在：'+$move.Target) }
    }
    $done=New-Object Collections.ArrayList
    try {
        foreach ($move in $moves) {
            Move-Item -LiteralPath $move.Source -Destination $move.Target -ErrorAction Stop
            [void]$done.Add($move)
        }
    } catch {
        for ($i=$done.Count-1; $i -ge 0; $i--) {
            $move=$done[$i]
            Move-Item -LiteralPath $move.Target -Destination $move.Source -ErrorAction Stop
        }
        throw
    }
    $script:state.Session=$targetId
    $script:state.LogPath=$moves[0].Target
    $script:state.EventPath=$moves[1].Target
    $script:state.MetaPath=$moves[2].Target
    if ($script:pathLabel) { $script:pathLabel.Text='自动保存：'+$script:state.LogPath }
}
function Receive-Pending {
    if (!$script:state.Port -or !$script:state.Connected) { return }
    if (!$script:state.Port.IsOpen) { throw '串口已关闭' }
    $budget=65536
    while ($script:state.Port.IsOpen -and $script:state.Port.BytesToRead -gt 0 -and $budget -gt 0) {
        $size=[Math]::Min([Math]::Min([int]$script:state.Port.BytesToRead,8192),$budget)
        $buffer=New-Object byte[] $size
        $n=$script:state.Port.Read($buffer,0,$size)
        if ($n -le 0) { break }
        # Persist the exact incoming bytes before doing any UI decoding.
        $script:state.Raw.Write($buffer,0,$n)
        $script:state.Bytes+=$n; $budget-=$n
        $chars=New-Object char[] ($n*2+2)
        $count=$script:state.Decoder.GetChars($buffer,0,$n,$chars,0,$false)
        if ($script:outputBox -and $count) {
            $script:outputBox.AppendText((New-Object string($chars,0,$count)))
            if ($script:outputBox.TextLength -gt 120000) {
                $script:outputBox.Select(0,$script:outputBox.TextLength-60000)
                $script:outputBox.SelectedText=''
                $script:outputBox.SelectionStart=$script:outputBox.TextLength
            }
            $script:outputBox.ScrollToCaret()
        }
    }
    $script:state.Raw.Flush()
    if (([datetime]::UtcNow-$script:state.LastMeta).TotalSeconds -ge 1) { Save-Metadata }
    Set-Status ("已连接 {0} · 已自动保存 {1:N0} 字节 · 关闭窗口会释放串口" -f $script:state.PortName,$script:state.Bytes)
}
function Close-LogFiles([string]$Action, [string]$Reason) {
    try { Write-Event $Action $Reason } catch {}
    $script:state.Connected=$false
    if ($script:state.Raw) {
        try { $script:state.Raw.Flush($true) } catch {}
        finally { $script:state.Raw.Dispose(); $script:state.Raw=$null }
    }
    if ($script:state.Events) { try { $script:state.Events.Dispose() } catch {}; $script:state.Events=$null }
    try { Rename-ClosedLogFiles } catch { Set-Status ('版本文件名整理失败，原日志仍保留：'+$_.Exception.Message) }
    try { Save-Metadata } catch {}
}
function Disconnect-Session([string]$Reason='用户断开', [bool]$Drain=$true) {



    if (!$script:state.Port -and !$script:state.Raw) { return }
    if ($Drain) { try { Receive-Pending } catch {} }
    Close-LogFiles 'DISCONNECT' $Reason
    if ($script:state.Port) {
        try { $script:state.Port.Close() } catch {}
        try { $script:state.Port.Dispose() } catch {}
        $script:state.Port=$null
    }
    if ($script:connectButton) { $script:connectButton.Text='连接并自动记录' }
    if ($script:portBox) { $script:portBox.Enabled=$true }
    Set-Status ($Reason+'；日志已保存，串口已释放。')
}
function Start-NewRecord {
    if (!$script:state.Connected -or !$script:state.Port -or !$script:state.Port.IsOpen) {
        Set-Status '请先连接串口，再开始新记录。'
        return $false
    }
    try { [void](Get-RequestedLabel) }
    catch { Set-Status $_.Exception.Message; return $false }
    try {
        # Drain pending bytes into the previous file; never throw them away.
        Receive-Pending
        $name=$script:state.PortName
        Close-LogFiles 'END_RECORD' '开始下一轮记录；串口保持连接'
        Start-LogSession $name 'NEW_RECORD'
        if ($script:outputBox) { $script:outputBox.Clear() }
        if ($script:entryBox) { $script:entryBox.Clear() }
        Set-Status ('新记录已开始：'+$name+' · 计数归零、窗口清空，旧文件已保留；板卡状态未重置。')
        return $true
    } catch {
        Disconnect-Session ('新建记录失败：'+$_.Exception.Message) $false
        return $false
    }
}
function Send-OneCharacter([string]$Text) {
    # Reject pasted prose as a whole, never truncate it to its first shortcut.
    if ($Text.Length -ne 1 -or [int][char]$Text[0] -lt 32 -or [int][char]$Text[0] -gt 126) {
        Set-Status '未发送：一次只能发送一个英文半角字符，不接受整段文字。'
        return $false
    }
    if (!$script:state.Port -or !$script:state.Connected -or !$script:state.Port.IsOpen) {
        Set-Status '请先连接板卡串口。'
        return $false
    }
    try {
        $script:state.Port.Write($Text)
        Write-Event 'TX' ("char={0} hex={1:X2}" -f $Text,[int][char]$Text[0])
        Set-Status ("已发送 [{0}]；接收与指令均自动保存。" -f $Text)
        return $true
    } catch {
        Write-Event 'ERROR' $_.Exception.Message
        Disconnect-Session ('发送失败：'+$_.Exception.Message) $false
        return $false
    }
}
function Refresh-Ports {
    $previous=[string]$script:portBox.SelectedItem
    if (!$previous) {
        try {
            $lastSession=Get-Content -LiteralPath (Join-Path $LogRoot 'latest.json') -Raw -ErrorAction Stop | ConvertFrom-Json
            $previous=[string]$lastSession.port
        } catch {}
    }
    $script:portBox.Items.Clear()
    foreach ($port in @([IO.Ports.SerialPort]::GetPortNames() | Sort-Object { [int]($_ -replace '\D','') })) {
        [void]$script:portBox.Items.Add($port)
    }
    if ($script:portBox.Items.Contains($previous)) { $script:portBox.SelectedItem=$previous }
    elseif ($script:portBox.Items.Count) { $script:portBox.SelectedIndex=0 }
}
function Connect-SelectedPort {
    if ($script:state.Connected) { Disconnect-Session; return }
    $name=[string]$script:portBox.SelectedItem
    if (!$name) { Set-Status '没有可选串口，请连接板卡下载/串口 USB 后点刷新。'; return }
    try { [void](Get-RequestedLabel) }
    catch { Set-Status $_.Exception.Message; return }
    $port=$null
    try {
        $port=New-Object IO.Ports.SerialPort($name,$Baud,[IO.Ports.Parity]::None,8,[IO.Ports.StopBits]::One)
        $port.Handshake=[IO.Ports.Handshake]::None
        $port.DtrEnable=$false; $port.RtsEnable=$false
        $port.Encoding=[Text.Encoding]::ASCII
        $port.ReadTimeout=100; $port.WriteTimeout=500
        $port.Open()
        $script:state.Port=$port
        Start-LogSession $name
        $script:connectButton.Text='断开并保存'
        $script:portBox.Enabled=$false
        Set-Status ('已连接 '+$name+'；开始自动记录。')
    } catch {
        if ($port) { try { $port.Dispose() } catch {} }
        $script:state.Port=$null
        Set-Status ('连接失败：'+$_.Exception.Message+'。若被占用，请关闭其他终端的同一串口连接。')
    }
}

if ($SelfTest) {
    # Exercise the same capture/send/close functions without opening real COM.
    $fake=New-Object PSObject -Property @{
        IsOpen=$true; Queue=(New-Object 'Collections.Generic.List[byte]')
        Sent=(New-Object 'Collections.Generic.List[string]'); Disposed=$false
    }
    $fake | Add-Member ScriptProperty BytesToRead { $this.Queue.Count }
    $fake | Add-Member ScriptMethod Read { param($b,$offset,$n)
        $take=[Math]::Min($n,$this.Queue.Count)
        for ($i=0;$i -lt $take;$i++) { $b[$offset+$i]=$this.Queue[$i] }
        $this.Queue.RemoveRange(0,$take); return $take
    }
    $fake | Add-Member ScriptMethod Write { param($s) $this.Sent.Add([string]$s) }
    $fake | Add-Member ScriptMethod Close { $this.IsOpen=$false }
    $fake | Add-Member ScriptMethod Dispose { $this.Disposed=$true }
    $view=New-Object PSObject -Property @{ Content=(New-Object Text.StringBuilder) }
    $view | Add-Member ScriptMethod AppendText { param($s) [void]$this.Content.Append($s) }
    $view | Add-Member ScriptMethod ScrollToCaret {}
    $view | Add-Member ScriptMethod Clear { [void]$this.Content.Clear() }
    $view | Add-Member ScriptProperty TextLength { $this.Content.Length }
    $script:outputBox=$view
    $script:state.Port=$fake
    Start-LogSession 'COM_TEST'
    $data=$script:utf8.GetBytes("fps 60.0 | MOT2`r`n测试UTF8`r`n[auto compare OFF]`r`n")
    foreach ($byte in $data) { $fake.Queue.Add($byte); Receive-Pending }
    if ((Send-OneCharacter 'DIRECT capacity') -or (Send-OneCharacter 'v+++..........')) { throw 'Pasted text was accepted' }
    if (!(Send-OneCharacter '@') -or !(Send-OneCharacter '!')) { throw 'Single command failed' }
    try { [void](Normalize-LogLabel 'R8/bad'); throw 'Invalid version label was accepted' }
    catch { if ($_.Exception.Message -eq 'Invalid version label was accepted') { throw } }
    $log=$script:state.LogPath; $events=$script:state.EventPath
    # Check that the file is readable and current while capture still owns it.
    # A tailing reader must share the existing writer, as Get-Content -Wait does.
    $reader=[IO.File]::Open($log,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::ReadWrite)
    $memory=New-Object IO.MemoryStream
    try { $reader.CopyTo($memory); $live=$memory.ToArray() } finally { $reader.Dispose(); $memory.Dispose() }
    if ([Convert]::ToBase64String($live) -ne [Convert]::ToBase64String($data)) { throw 'Live capture bytes differ' }
    if ($view.Content.ToString() -ne $script:utf8.GetString($data)) { throw 'Split UTF8 display decoding failed' }
    $oldMeta=$script:state.MetaPath
    if (!(Set-CurrentLogLabel 'R7A')) { throw 'Current record could not be marked' }
    $script:nextLabel='R8/bad'
    if ((Start-NewRecord) -or !$fake.IsOpen -or $fake.Disposed) { throw 'Invalid next label disrupted capture' }
    $script:nextLabel='R8'
    $pendingData=$script:utf8.GetBytes("pending before split`r`n")
    foreach ($byte in $pendingData) { $fake.Queue.Add($byte) }
    $oldExpected=[byte[]]($data+$pendingData)
    if (!(Start-NewRecord)) { throw 'Record rotation failed' }
    if (!$fake.IsOpen -or $fake.Disposed -or $script:state.Bytes -ne 0 -or $view.TextLength -ne 0) { throw 'Rotation closed port or failed to clear display/counter' }
    if ($script:state.LogPath -eq $log) { throw 'Rotation reused a previous file' }
    $oldId=[IO.Path]::GetFileNameWithoutExtension($log)
    $oldDay=Split-Path $log -Parent
    $log=Join-Path $oldDay ($oldId+'_R7A.txt')
    $events=Join-Path $oldDay ($oldId+'_R7A.events.tsv')
    $oldMeta=Join-Path $oldDay ($oldId+'_R7A.json')
    if (!(Test-Path -LiteralPath $log) -or $script:state.Label -ne 'R8' -or $script:state.LogPath -notlike '*_R8.txt') { throw 'Version labels not applied to old/new records' }
    if ([Convert]::ToBase64String([IO.File]::ReadAllBytes($log)) -ne [Convert]::ToBase64String($oldExpected)) { throw 'Rotation altered or discarded old pending bytes' }
    $oldInfo=Get-Content -LiteralPath $oldMeta -Raw | ConvertFrom-Json
    if ($oldInfo.connected -or $oldInfo.bytes_received -ne $oldExpected.Length -or
        $oldInfo.hardware_label -ne 'R7A' -or $oldInfo.raw_log -ne $log) { throw 'Old session not finalized or labelled' }
    $nextData=$script:utf8.GetBytes("new test only`r`n")
    foreach ($byte in $nextData) { $fake.Queue.Add($byte) }
    Receive-Pending
    if (!(Send-OneCharacter 'h')) { throw 'Command after rotation failed' }
    if (!(Send-OneCharacter '%') -or !(Send-OneCharacter '&')) { throw '720p shortcuts failed' }
    $newLog=$script:state.LogPath; $newEvents=$script:state.EventPath
    Disconnect-Session 'selftest complete'
    if ($fake.IsOpen -or !$fake.Disposed) { throw 'Port not released' }
    $meta=Get-Content -LiteralPath (Join-Path $LogRoot 'latest.json') -Raw | ConvertFrom-Json
    if ($meta.connected -or $meta.bytes_received -ne $nextData.Length -or
        $meta.hardware_label -ne 'R8' -or $meta.raw_log -ne $newLog) { throw 'Metadata mismatch' }
    if ([Convert]::ToBase64String([IO.File]::ReadAllBytes($newLog)) -ne [Convert]::ToBase64String($nextData)) { throw 'Records were mixed' }
    if (($fake.Sent -join '') -cne '@!h%&') { throw 'Unexpected transmission' }
    $audit=[IO.File]::ReadAllText($events)
    if ($audit -notmatch 'hex=40' -or $audit -notmatch 'hex=21' -or $audit -notmatch 'END_RECORD' -or
        $audit -notmatch 'hardware_label=R7A') { throw 'Old command audit missing' }
    $nextAudit=[IO.File]::ReadAllText($newEvents)
    if ($nextAudit -notmatch 'NEW_RECORD' -or $nextAudit -notmatch 'hex=68' -or
        $nextAudit -notmatch 'hex=25' -or $nextAudit -notmatch 'hex=26' -or
        $nextAudit -notmatch 'DISCONNECT') { throw 'New command audit missing' }
    'PASS serial monitor: exact capture, split UTF8, single-char safety, 720p square shortcuts, TX audit, record rotation, version labels, close/dispose, latest metadata'
    exit 0
}

[Windows.Forms.Application]::EnableVisualStyles()
$form=New-Object Windows.Forms.Form
$form.Text="720p + 2万方块 · 串口自动保存 · $Baud / 8N1"
$form.Size=New-Object Drawing.Size(1060,720)
$form.MinimumSize=New-Object Drawing.Size(1000,540)
$form.StartPosition='CenterScreen'
$form.Font=New-Object Drawing.Font('Microsoft YaHei UI',9)
$script:portBox=New-Object Windows.Forms.ComboBox
$script:portBox.Location=New-Object Drawing.Point(12,14)
$script:portBox.Size=New-Object Drawing.Size(100,26)
$script:portBox.DropDownStyle='DropDownList'
$form.Controls.Add($script:portBox)
$refresh=New-Object Windows.Forms.Button
$refresh.Text='刷新串口'; $refresh.SetBounds(120,12,92,29)
$refresh.Add_Click({ if (!$script:state.Connected) { Refresh-Ports } })
$form.Controls.Add($refresh)
$script:connectButton=New-Object Windows.Forms.Button
$script:connectButton.Text='连接并自动记录'; $script:connectButton.SetBounds(220,12,150,29)
$script:connectButton.Add_Click({ Connect-SelectedPort })
$form.Controls.Add($script:connectButton)
$newRecord=New-Object Windows.Forms.Button
$newRecord.Text='新记录（清屏）'; $newRecord.SetBounds(380,12,150,29)
$newRecord.Add_Click({ [void](Start-NewRecord) })
$form.Controls.Add($newRecord)
$folder=New-Object Windows.Forms.Button
$folder.Text='打开日志文件夹'; $folder.SetBounds(540,12,145,29)
$folder.Add_Click({
    [void][IO.Directory]::CreateDirectory($LogRoot)
    # The user explicitly clicked Open Folder, so this Explorer window is visible.
    Start-Process explorer.exe -ArgumentList ('"'+$LogRoot+'"')
})
$form.Controls.Add($folder)
$versionLabel=New-Object Windows.Forms.Label
$versionLabel.Text='版本：'; $versionLabel.SetBounds(700,18,48,24); $form.Controls.Add($versionLabel)
$script:labelBox=New-Object Windows.Forms.ComboBox
$script:labelBox.SetBounds(748,13,95,28)
$script:labelBox.DropDownStyle='DropDown'
foreach ($label in @('720P-R3-R19','CPU','MOT2','TABLE','CALLS','未标记')) { [void]$script:labelBox.Items.Add($label) }
$script:labelBox.Text='720P-R3-R19'
$form.Controls.Add($script:labelBox)
$markCurrent=New-Object Windows.Forms.Button
$markCurrent.Text='标记当前记录'; $markCurrent.SetBounds(850,12,142,29)
$markCurrent.Add_Click({
    try { [void](Set-CurrentLogLabel ([string]$script:labelBox.Text)) }
    catch { Set-Status $_.Exception.Message }
})
$form.Controls.Add($markCurrent)
$script:pathLabel=New-Object Windows.Forms.Label
$script:pathLabel.Text='连接后自动建立新日志；只记录连接之后收到的数据。'
$script:pathLabel.SetBounds(12,50,1010,40); $script:pathLabel.Anchor='Top,Left,Right'
$form.Controls.Add($script:pathLabel)
$quick=New-Object Windows.Forms.FlowLayoutPanel
$quick.SetBounds(10,92,1015,36); $quick.Anchor='Top,Left,Right'
foreach ($item in @(@('2万方块 @','@'),@('运动 CPU/HW !','!'),@('紧凑表 %','%'),@('同状态对照 &','&'),@('同步 v','v'),@('绘制 CPU/HW g','g'),@('透明 e','e'),@('帮助 h','h'),@('下一场景 n','n'))) {
    $button=New-Object Windows.Forms.Button
    $button.UseMnemonic=$false; $button.Text=$item[0]; $button.Tag=$item[1]; $button.AutoSize=$true; $button.Height=28
    $button.Add_Click({ param($sender,$event) [void](Send-OneCharacter ([string]$sender.Tag)) })
    $quick.Controls.Add($button)
}
$form.Controls.Add($quick)
$gameQuick=New-Object Windows.Forms.FlowLayoutPanel
$gameQuick.SetBounds(10,130,1015,36); $gameQuick.Anchor='Top,Left,Right'
foreach ($item in @(@('方块减100 ,',','),@('方块加100 .','.'),@('测试音 (','('),@('总音量 )',')'),@('音库 /','/'),@('统计 f','f'),@('降档 -','-'),@('升档 +','+'),@('第四档 4','4'),@('环形技能 J','J'))) {
    $button=New-Object Windows.Forms.Button
    $button.UseMnemonic=$false; $button.Text=$item[0]; $button.Tag=$item[1]; $button.AutoSize=$true; $button.Height=28
    $button.Add_Click({ param($sender,$event) [void](Send-OneCharacter ([string]$sender.Tag)) })
    $gameQuick.Controls.Add($button)
}
$form.Controls.Add($gameQuick)
$sendLabel=New-Object Windows.Forms.Label
$sendLabel.Text='其他单字符：'; $sendLabel.SetBounds(12,176,100,25); $form.Controls.Add($sendLabel)
$script:entryBox=New-Object Windows.Forms.TextBox
$script:entryBox.SetBounds(110,172,180,28); $form.Controls.Add($script:entryBox)
$send=New-Object Windows.Forms.Button
$send.Text='发送'; $send.SetBounds(300,171,70,29)
$send.Add_Click({ if (Send-OneCharacter $script:entryBox.Text) { $script:entryBox.Clear() } })
$script:entryBox.Add_KeyDown({ param($sender,$event)
    if ($event.KeyCode -eq [Windows.Forms.Keys]::Enter) {
        $event.SuppressKeyPress=$true
        if (Send-OneCharacter $script:entryBox.Text) { $script:entryBox.Clear() }
    }
})
$form.Controls.Add($send)
$hint=New-Object Windows.Forms.Label
$hint.UseMnemonic=$false
$hint.Text='方块：逗号减100、句号加100；音频：左括号测试音、右括号音量。连接即自动保存。'
$hint.SetBounds(385,177,640,24); $form.Controls.Add($hint)
$script:outputBox=New-Object Windows.Forms.RichTextBox
$script:outputBox.SetBounds(12,213,1018,413); $script:outputBox.Anchor='Top,Bottom,Left,Right'
$script:outputBox.ReadOnly=$true; $script:outputBox.WordWrap=$false
$script:outputBox.Font=New-Object Drawing.Font('Consolas',10)
$script:outputBox.BackColor=[Drawing.Color]::FromArgb(24,28,34)
$script:outputBox.ForeColor=[Drawing.Color]::FromArgb(222,230,235)
$form.Controls.Add($script:outputBox)
$script:statusLabel=New-Object Windows.Forms.Label
$script:statusLabel.Text='未连接。先关闭 Tera Term / MobaXterm 等终端对同一 COM 口的连接。'
$script:statusLabel.SetBounds(12,640,1018,38); $script:statusLabel.Anchor='Bottom,Left,Right'
$form.Controls.Add($script:statusLabel)
$script:timer=New-Object Windows.Forms.Timer
$script:timer.Interval=50
$script:timer.Add_Tick({
    if ($script:state.Connected) {
        try { Receive-Pending }
        catch { Disconnect-Session ('串口断开：'+$_.Exception.Message) $false }
    }
})
$form.Add_FormClosing({
    $script:timer.Stop()
    Disconnect-Session '关闭日志窗口'
})
if ($UiSmokeTest) {
    Refresh-Ports
    if ($script:labelBox.Text -ne '720P-R3-R19' -or !$script:labelBox.Items.Contains('MOT2') -or $markCurrent.Text -ne '标记当前记录') { throw 'Version label controls mismatch' }
    if ($quick.Controls.Count -ne 9 -or $quick.Controls[0].Tag -ne '@' -or $quick.Controls[1].Tag -ne '!' -or $quick.Controls[2].Tag -ne '%' -or $quick.Controls[3].Tag -ne '&') { throw '720p quick controls mismatch' }
    if ($gameQuick.Controls.Count -ne 10 -or $gameQuick.Controls[0].Tag -ne ',' -or $gameQuick.Controls[1].Tag -ne '.' -or $gameQuick.Controls[2].Tag -ne '(') { throw 'Audio controls mismatch' }
    # Realize child handles offscreen so DrawToBitmap includes the controls.
    $form.ShowInTaskbar=$false; $form.StartPosition='Manual'
    $form.Location=New-Object Drawing.Point(-20000,-20000)
    $form.Show(); [Windows.Forms.Application]::DoEvents()
    [void][IO.Directory]::CreateDirectory($LogRoot)
    $bitmap=New-Object Drawing.Bitmap($form.Width,$form.Height)
    try {
        $form.DrawToBitmap($bitmap,(New-Object Drawing.Rectangle(0,0,$form.Width,$form.Height)))
        $bitmap.Save((Join-Path $LogRoot 'ui.png'),[Drawing.Imaging.ImageFormat]::Png)
    } finally { $bitmap.Dispose(); $script:timer.Dispose(); $form.Dispose() }
    'PASS serial monitor UI: builds and renders, nine square controls plus ten fine-load/audio/skill controls, no COM opened'
    exit 0
}
try {
    Refresh-Ports
    $script:timer.Start()
    [void]$form.ShowDialog()
} finally {
    $script:timer.Stop(); $script:timer.Dispose()
    Disconnect-Session '程序退出'
    $form.Dispose()
}
