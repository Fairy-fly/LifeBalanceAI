# coding: utf-8  
import codecs  
  
# === 1. Fix .ui: save button text ===  
fp = r'D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.ui'  
f = codecs.open(fp, 'r', 'utf-8')  
lines = f.readlines()  
f.close()  
bao = chr(0x4fdd)+chr(0x5b58)  # ±£´æ  
bao2 = bao+chr(0x753b)+chr(0x50cf)  # ±£´æ»­Ïñ  
for i,l in enumerate(lines):  
    if bao in l and 'btnSaveProfile' in str(lines[max(0,i-2):i+1]):  
        lines[i] = '           <string>' + bao2 + '</string>\n'  
        print('UI fixed line '+str(i+1))  
        break  
f = codecs.open(fp, 'w', 'utf-8')  
f.writelines(lines)  
f.close()  
