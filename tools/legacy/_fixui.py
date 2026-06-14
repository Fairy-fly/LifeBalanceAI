# coding: utf-8  
import codecs  
fp = r'D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.ui'  
f = codecs.open(fp, 'r', 'utf-8')  
lines = f.readlines()  
f.close()  
c1 = chr(0x4fdd)+chr(0x5b58)  # ±£´æ  
c2 = chr(0x4fdd)+chr(0x5b58)+chr(0x753b)+chr(0x50cf)  # ±£´æ»­Ïñ  
old = '           <string>' + c2 + '</string>\n'  
new = '           <string>' + c1 + '</string>\n'  
for i,l in enumerate(lines):  
    if c2 in l:  
        lines[i] = new  
        print('Reverted UI line '+str(i+1))  
        break  
f = codecs.open(fp, 'w', 'utf-8')  
f.writelines(lines)  
f.close()  
