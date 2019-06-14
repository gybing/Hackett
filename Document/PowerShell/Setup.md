# 安装screenfetch
Install-Module windows-screenfetch -Scope CurrentUser
Import-Module windows-screenfetch
Set-ExecutionPolicy -Scope CurrentUser Bypass

# 切换UTF-8
chcp 65001

# 添加Sarasa Term SC字体注册表项修复乱码
$SystemLinkKey ='\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\FontLink\SystemLink\Sarasa Term SC'
New-ItemProperty $SystemLinkKey Entry5 -value "MICROSS.TTF,Microsoft Sans Serif,108,122","MICROSS.TTF,Microsoft Sans Serif","MINGLIU.TTC,PMingLiU","MSMINCHO.TTC,MS PMincho","BATANG.TTC,Batang","MSYH.TTC,Microsoft YaHei UI","MSJH.TTC,Microsoft JhengHei UI","YUGOTHM.TTC,Yu Gothic UI","MALGUN.TTF,Malgun Gothic","SEGUISYM.TTF,Segoe UI Symbol" -propertyType multistring

# 配色
./colortool -b ./ayu.itermcolors

# 安装文件夹、文件配色样式
Install-Module PSColor
Import-Module PSColor
Install-Module DirColors
Import-Module DirColors
Update-DirColors .\dircolors.256dark

# 安装扩展
Install-Module posh-git -Scope CurrentUser
Install-Module oh-my-posh -Scope CurrentUser
Import-Module posh-git
Import-Module oh-my-posh
Set-Theme PowerLine

# 创建启动文件
create file 
C:\Users\hanrj\Documents\WindowsPowerShell\Microsoft.PowerShell_profile.ps1
C:\Users\hanrj\Documents\WindowsPowerShell\Microsoft.VSCode_profile.ps1
with content:
Import-Module DirColors
Update-DirColors ~/dircolors
Import-Module posh-git
if (!(Get-SshAgent)) {
    Start-SshAgent
}
Import-Module oh-my-posh
Set-Theme PowerLine
Screenfetch