# coding: utf-8  
import codecs  
fp = r'D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.h'  
f = codecs.open(fp, 'r', 'utf-8')  
lines = f.readlines()  
f.close()  
new_lines = []  
for l in lines:  
    if 'm_btnCancelEdit' not in l:  
        new_lines.append(l)  
f = codecs.open(fp, 'w', 'utf-8')  
f.writelines(new_lines)  
f.close()  
print('Removed m_btnCancelEdit from .h')  
